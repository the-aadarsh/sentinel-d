/*
 * http_client.c – raw socket HTTP/HTTPS health checker for SENTINEL-D
 *
 * Design:
 *   - Plain HTTP uses POSIX BSD sockets directly (getaddrinfo + connect).
 *   - HTTPS uses OpenSSL (SSL_connect / SSL_read / SSL_write).
 *   - Per-request timeout is enforced via SO_RCVTIMEO / SO_SNDTIMEO.
 *   - We send a minimal HTTP/1.1 GET request and read only the status line;
 *     we do not need the body for health checking purposes.
 *   - No heap allocation of large buffers; stack buffers are bounded.
 */

#include "http_client.h"
#include "utils.h"
#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>     /* for struct addrinfo, getaddrinfo, freeaddrinfo */
#include <netdb.h>     /* for struct addrinfo, getaddrinfo, freeaddrinfo */
#include <sys/types.h> /* for struct addrinfo on some systems */
#include <sys/types.h>
/* Ensure <netdb.h> is included before any use of struct addrinfo */
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h> /* for struct addrinfo on some systems */
#include <netdb.h>     /* for struct addrinfo, getaddrinfo, freeaddrinfo */
#include <netinet/in.h>
#include <arpa/inet.h>

/* OpenSSL headers */
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>

/* Maximum size for the HTTP request we send. */
#define HTTP_REQUEST_BUF 4096
/* Read buffer for the response status line. */
#define HTTP_RESPONSE_BUF 4096

/* ---------------------------------------------------------------------------
 * Internal: URL parsing
 * --------------------------------------------------------------------------- */

typedef struct
{
    int is_https;
    char host[512];
    int port;
    char path[1024];
} ParsedURL;

static int parse_url(const char *url, ParsedURL *out)
{
    memset(out, 0, sizeof(*out));
    str_copy_safe(out->path, "/", sizeof(out->path));

    if (str_starts_with(url, "https://"))
    {
        out->is_https = 1;
        url += 8;
        out->port = 443;
    }
    else if (str_starts_with(url, "http://"))
    {
        out->is_https = 0;
        url += 7;
        out->port = 80;
    }
    else
    {
        return -1; /* Unsupported scheme */
    }

    /* Find optional path */
    const char *slash = strchr(url, '/');
    if (slash)
    {
        size_t host_len = (size_t)(slash - url);
        if (host_len >= sizeof(out->host))
            return -1;
        memcpy(out->host, url, host_len);
        out->host[host_len] = '\0';
        str_copy_safe(out->path, slash, sizeof(out->path));
    }
    else
    {
        str_copy_safe(out->host, url, sizeof(out->host));
    }

    /* Extract optional port from host (host:port) */
    char *colon = strchr(out->host, ':');
    if (colon)
    {
        out->port = atoi(colon + 1);
        *colon = '\0';
    }

    if (out->host[0] == '\0' || out->port <= 0 || out->port > 65535)
        return -1;

    return 0;
}

/* ---------------------------------------------------------------------------
 * Internal: socket helpers
 * --------------------------------------------------------------------------- */

static void set_socket_timeout(int fd, int seconds)
{
    struct timeval tv = {.tv_sec = seconds, .tv_usec = 0};
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
}

/* Opens a TCP connection to host:port.
 * Returns socket fd on success, -1 on error (fills errbuf). */
static int tcp_connect(const char *host, int port, int timeout_sec,
                       HttpErrorType *err_out, char *errbuf, size_t errbuf_size)
{
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);

    struct addrinfo hints, *res = NULL;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(host, port_str, &hints, &res);
    if (rc != 0)
    {
        snprintf(errbuf, errbuf_size, "DNS lookup failed: %s", gai_strerror(rc));
        *err_out = HTTP_ERR_DNS;
        return -1;
    }

    int fd = -1;
    for (struct addrinfo *rp = res; rp; rp = rp->ai_next)
    {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd < 0)
            continue;

        set_socket_timeout(fd, timeout_sec);

        if (connect(fd, rp->ai_addr, rp->ai_addrlen) == 0)
        {
            break; /* Connected */
        }
        /* Connect failed: categorize error */
        if (errno == ETIMEDOUT || errno == EINPROGRESS)
        {
            *err_out = HTTP_ERR_TIMEOUT;
        }
        else
        {
            *err_out = HTTP_ERR_CONNECT;
        }
        snprintf(errbuf, errbuf_size, "connect failed: %s", strerror(errno));
        close(fd);
        fd = -1;
    }
    freeaddrinfo(res);

    if (fd < 0 && *err_out == HTTP_ERR_NONE)
    {
        *err_out = HTTP_ERR_CONNECT;
        snprintf(errbuf, errbuf_size, "No valid address to connect to");
    }
    return fd;
}

/* ---------------------------------------------------------------------------
 * Internal: HTTP GET and status-line parsing
 * --------------------------------------------------------------------------- */

/* Build a minimal HTTP/1.1 GET request. */
static int build_http_request(char *buf, size_t buf_size,
                              const char *host, const char *path)
{
    return snprintf(buf, buf_size,
                    "GET %s HTTP/1.1\r\n"
                    "Host: %s\r\n"
                    "User-Agent: sentinel-d/1.0\r\n"
                    "Connection: close\r\n"
                    "Accept: */*\r\n"
                    "\r\n",
                    path, host);
}

/* Parse "HTTP/1.x NNN ..." → return NNN, or -1 on failure. */
static int parse_status_line(const char *buf)
{
    /* Expect "HTTP/1." at start */
    if (strncmp(buf, "HTTP/1", 6) != 0)
        return -1;
    const char *sp = strchr(buf, ' ');
    if (!sp)
        return -1;
    int code = atoi(sp + 1);
    if (code < 100 || code > 599)
        return -1;
    return code;
}

/* ---------------------------------------------------------------------------
 * Plain HTTP path
 * --------------------------------------------------------------------------- */

static int do_http_check(const ParsedURL *url, int timeout_sec,
                         HttpResult *result)
{
    HttpErrorType err = HTTP_ERR_NONE;
    int fd = tcp_connect(url->host, url->port, timeout_sec,
                         &err, result->error_msg, sizeof(result->error_msg));
    if (fd < 0)
    {
        result->error = err;
        return -1;
    }

    char request[HTTP_REQUEST_BUF];
    build_http_request(request, sizeof(request), url->host, url->path);

    if (send(fd, request, strlen(request), 0) < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            result->error = HTTP_ERR_TIMEOUT;
            snprintf(result->error_msg, sizeof(result->error_msg),
                     "send timed out: %s", strerror(errno));
        }
        else
        {
            result->error = HTTP_ERR_SYSTEM;
            snprintf(result->error_msg, sizeof(result->error_msg),
                     "send error: %s", strerror(errno));
        }
        close(fd);
        return -1;
    }

    /* Read response into buffer; we only need the first line. */
    char resp[HTTP_RESPONSE_BUF];
    ssize_t n = recv(fd, resp, sizeof(resp) - 1, 0);
    close(fd);

    if (n < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            result->error = HTTP_ERR_TIMEOUT;
            snprintf(result->error_msg, sizeof(result->error_msg),
                     "recv timed out: %s", strerror(errno));
        }
        else
        {
            result->error = HTTP_ERR_SYSTEM;
            snprintf(result->error_msg, sizeof(result->error_msg),
                     "recv error: %s", strerror(errno));
        }
        return -1;
    }
    if (n == 0)
    {
        result->error = HTTP_ERR_PROTO;
        snprintf(result->error_msg, sizeof(result->error_msg),
                 "Empty response (connection closed immediately)");
        return -1;
    }
    resp[n] = '\0';

    int code = parse_status_line(resp);
    if (code < 0)
    {
        result->error = HTTP_ERR_PROTO;
        snprintf(result->error_msg, sizeof(result->error_msg),
                 "Malformed HTTP response");
        return -1;
    }
    result->status_code = code;
    return 0;
}

/* ---------------------------------------------------------------------------
 * HTTPS path (OpenSSL)
 * --------------------------------------------------------------------------- */

/* Module-level SSL_CTX (initialized once). */
static SSL_CTX *g_ssl_ctx = NULL;

static int ssl_ctx_init(void)
{
    if (g_ssl_ctx)
        return 0;

    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();

    g_ssl_ctx = SSL_CTX_new(TLS_client_method());
    if (!g_ssl_ctx)
    {
        return -1;
    }
    /* Use system certificate store */
    SSL_CTX_set_default_verify_paths(g_ssl_ctx);
    SSL_CTX_set_verify(g_ssl_ctx, SSL_VERIFY_PEER, NULL);
    return 0;
}

static int do_https_check(const ParsedURL *url, int timeout_sec,
                          HttpResult *result)
{
    if (ssl_ctx_init() < 0)
    {
        result->error = HTTP_ERR_SSL;
        snprintf(result->error_msg, sizeof(result->error_msg),
                 "Failed to initialize OpenSSL context");
        return -1;
    }

    HttpErrorType err = HTTP_ERR_NONE;
    int fd = tcp_connect(url->host, url->port, timeout_sec,
                         &err, result->error_msg, sizeof(result->error_msg));
    if (fd < 0)
    {
        result->error = err;
        return -1;
    }

    SSL *ssl = SSL_new(g_ssl_ctx);
    if (!ssl)
    {
        result->error = HTTP_ERR_SSL;
        snprintf(result->error_msg, sizeof(result->error_msg),
                 "SSL_new failed");
        close(fd);
        return -1;
    }

    SSL_set_fd(ssl, fd);
    /* SNI extension: required by most CDNs */
    SSL_set_tlsext_host_name(ssl, url->host);

    int handshake_rc = SSL_connect(ssl);
    if (handshake_rc != 1)
    {
        unsigned long ssl_err = ERR_get_error();
        char ssl_errbuf[256] = {0};
        ERR_error_string_n(ssl_err, ssl_errbuf, sizeof(ssl_errbuf));
        result->error = HTTP_ERR_SSL;
        snprintf(result->error_msg, sizeof(result->error_msg),
                 "TLS handshake failed: %s", ssl_errbuf);
        SSL_free(ssl);
        close(fd);
        return -1;
    }

    char request[HTTP_REQUEST_BUF];
    build_http_request(request, sizeof(request), url->host, url->path);

    if (SSL_write(ssl, request, (int)strlen(request)) <= 0)
    {
        result->error = HTTP_ERR_TIMEOUT;
        snprintf(result->error_msg, sizeof(result->error_msg),
                 "SSL_write failed");
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(fd);
        return -1;
    }

    char resp[HTTP_RESPONSE_BUF];
    int n = SSL_read(ssl, resp, sizeof(resp) - 1);
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(fd);

    if (n <= 0)
    {
        result->error = HTTP_ERR_TIMEOUT;
        snprintf(result->error_msg, sizeof(result->error_msg),
                 "SSL_read failed or timed out");
        return -1;
    }
    resp[n] = '\0';

    int code = parse_status_line(resp);
    if (code < 0)
    {
        result->error = HTTP_ERR_PROTO;
        snprintf(result->error_msg, sizeof(result->error_msg),
                 "Malformed HTTP response over TLS");
        return -1;
    }
    result->status_code = code;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Public API
 * --------------------------------------------------------------------------- */

int http_check(const char *url, int timeout_seconds, HttpResult *result)
{
    memset(result, 0, sizeof(*result));

    ParsedURL parsed;
    if (parse_url(url, &parsed) < 0)
    {
        result->error = HTTP_ERR_PROTO;
        snprintf(result->error_msg, sizeof(result->error_msg),
                 "Invalid or unsupported URL: %s", url);
        return -1;
    }

    long long t0 = time_monotonic_ms();
    int rc = parsed.is_https
                 ? do_https_check(&parsed, timeout_seconds, result)
                 : do_http_check(&parsed, timeout_seconds, result);
    result->latency_ms = (long)(time_monotonic_ms() - t0);

    return rc;
}

const char *http_error_str(HttpErrorType e)
{
    switch (e)
    {
    case HTTP_ERR_NONE:
        return "none";
    case HTTP_ERR_TIMEOUT:
        return "timeout";
    case HTTP_ERR_CONNECT:
        return "connection_refused";
    case HTTP_ERR_DNS:
        return "dns_failure";
    case HTTP_ERR_SSL:
        return "tls_error";
    case HTTP_ERR_PROTO:
        return "protocol_error";
    case HTTP_ERR_SYSTEM:
        return "system_error";
    default:
        return "unknown";
    }
}
