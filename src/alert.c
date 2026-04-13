/*
 * alert.c – Discord webhook alert sender for SENTINEL-D
 *
 * alert_build_payload() is a pure function that assembles a Discord embed
 * JSON string.  alert_send() and alert_send_recovery() call http_check()
 * via the same raw-socket client (supporting HTTPS webhooks).
 *
 * Discord Webhook API: POST application/json to the webhook URL.
 * We use a single embed with color-coded fields.
 */

#include "alert.h"
#include "config.h"
#include "http_client.h"
#include "logger.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

/* Discord embed colours (decimal) */
#define COLOUR_FAILURE  15158332   /* red   #E74C3C */
#define COLOUR_RECOVERY 3066993    /* green #2ECC71 */
#define COLOUR_WARNING  16776960   /* amber #FFFF00 */

/* Maximum size of the JSON payload we send to Discord. */
#define PAYLOAD_BUF_SIZE 4096

/* ---------------------------------------------------------------------------
 * alert_build_payload – pure function, no I/O, suitable for unit tests.
 * --------------------------------------------------------------------------- */
int alert_build_payload(char *buf, size_t buf_size,
                        AlertType type,
                        const SentinelConfig *cfg,
                        const char *endpoint,
                        const HttpResult *result,
                        int consecutive)
{
    char ts[32];
    time_iso8601(ts, sizeof(ts));

    /* Escape user-controlled strings that end up in JSON values */
    char svc[256], env[256], ep[CONFIG_VALUE_MAX];
    json_escape(svc, cfg->service_name, sizeof(svc));
    json_escape(env, cfg->environment,  sizeof(env));
    json_escape(ep,  endpoint,          sizeof(ep));

    int colour = (type == ALERT_TYPE_RECOVERY) ? COLOUR_RECOVERY : COLOUR_FAILURE;
    const char *status_emoji = (type == ALERT_TYPE_RECOVERY) ? "✅" : "🚨";
    const char *status_title = (type == ALERT_TYPE_RECOVERY)
        ? "Service Recovered"
        : "Service Unhealthy";

    /* Build the description */
    char description[512];
    if (type == ALERT_TYPE_RECOVERY) {
        snprintf(description, sizeof(description),
                 "%s **%s** is back online on `%s`",
                 status_emoji, svc, env);
    } else if (result && result->status_code > 0) {
        snprintf(description, sizeof(description),
                 "%s **%s** returned HTTP **%d** on `%s`",
                 status_emoji, svc, result->status_code, env);
    } else if (result) {
        char errmsg[256];
        json_escape(errmsg, result->error_msg, sizeof(errmsg));
        snprintf(description, sizeof(description),
                 "%s **%s** is unreachable on `%s`: %s",
                 status_emoji, svc, env, errmsg);
    } else {
        snprintf(description, sizeof(description),
                 "%s **%s** is experiencing issues on `%s`",
                 status_emoji, svc, env);
    }

    char desc_escaped[512];
    json_escape(desc_escaped, description, sizeof(desc_escaped));

    /* Field: error type or latency */
    char extra_fields[512] = "";
    if (type != ALERT_TYPE_RECOVERY && result) {
        char errtype[64];
        json_escape(errtype, http_error_str(result->error), sizeof(errtype));

        if (result->status_code > 0) {
            snprintf(extra_fields, sizeof(extra_fields),
                ",{\"name\":\"Status Code\",\"value\":\"%d\",\"inline\":true}"
                ",{\"name\":\"Latency\",\"value\":\"%ldms\",\"inline\":true}"
                ",{\"name\":\"Consecutive Failures\",\"value\":\"%d\",\"inline\":true}",
                result->status_code, result->latency_ms, consecutive);
        } else {
            snprintf(extra_fields, sizeof(extra_fields),
                ",{\"name\":\"Error Type\",\"value\":\"%s\",\"inline\":true}"
                ",{\"name\":\"Consecutive Failures\",\"value\":\"%d\",\"inline\":true}",
                errtype, consecutive);
        }
    }

    int n = snprintf(buf, buf_size,
        "{"
          "\"embeds\":[{"
            "\"title\":\"%s\","
            "\"description\":\"%s\","
            "\"color\":%d,"
            "\"fields\":["
              "{\"name\":\"Service\",\"value\":\"%s\",\"inline\":true},"
              "{\"name\":\"Environment\",\"value\":\"%s\",\"inline\":true},"
              "{\"name\":\"Endpoint\",\"value\":\"%s\",\"inline\":false}"
              "%s"
            "],"
            "\"footer\":{\"text\":\"sentinel-d • %s\"}"
          "}]"
        "}",
        status_title, desc_escaped, colour,
        svc, env, ep,
        extra_fields,
        ts);

    if (n < 0 || (size_t)n >= buf_size) return -1;
    return n;
}

/* ---------------------------------------------------------------------------
 * Internal: POST JSON payload to a Discord HTTPS webhook URL.
 *
 * Discord webhooks are always HTTPS.  We reuse the approach from http_client.c
 * but we need to POST, not GET.  Rather than adding a POST method to HttpResult,
 * we implement the post inline here using OpenSSL directly.
 * --------------------------------------------------------------------------- */

/* We share the SSL_CTX from http_client.c via a local re-init.
 * To avoid linking issues we init our own context here. */
static SSL_CTX *g_alert_ssl_ctx = NULL;

static int alert_ssl_init(void)
{
    if (g_alert_ssl_ctx) return 0;
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
    g_alert_ssl_ctx = SSL_CTX_new(TLS_client_method());
    if (!g_alert_ssl_ctx) return -1;
    SSL_CTX_set_default_verify_paths(g_alert_ssl_ctx);
    SSL_CTX_set_verify(g_alert_ssl_ctx, SSL_VERIFY_PEER, NULL);
    return 0;
}

static int parse_webhook_host(const char *url, char *host, size_t host_size,
                               char *path, size_t path_size, int *port)
{
    if (!str_starts_with(url, "https://")) return -1;
    url += 8;
    *port = 443;

    const char *slash = strchr(url, '/');
    if (slash) {
        size_t hlen = (size_t)(slash - url);
        if (hlen >= host_size) return -1;
        memcpy(host, url, hlen);
        host[hlen] = '\0';
        str_copy_safe(path, slash, path_size);
    } else {
        str_copy_safe(host, url, host_size);
        str_copy_safe(path, "/", path_size);
    }
    /* Optional port override */
    char *colon = strchr(host, ':');
    if (colon) {
        *port = atoi(colon + 1);
        *colon = '\0';
    }
    return 0;
}

static int webhook_post(const char *webhook_url,
                        const char *payload, size_t payload_len)
{
    char host[512], path[1024];
    int  port;

    if (parse_webhook_host(webhook_url, host, sizeof(host),
                            path, sizeof(path), &port) < 0) {
        LOG_ERROR("Alert: invalid webhook URL format");
        return -1;
    }

    if (alert_ssl_init() < 0) {
        LOG_ERROR("Alert: failed to init SSL context");
        return -1;
    }

    /* Resolve and connect */
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);

    struct addrinfo hints = {0}, *res = NULL;
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host, port_str, &hints, &res) != 0) {
        LOG_ERROR("Alert: DNS lookup failed for webhook host");
        return -1;
    }

    int fd = -1;
    for (struct addrinfo *rp = res; rp; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd < 0) continue;
        if (connect(fd, rp->ai_addr, rp->ai_addrlen) == 0) break;
        close(fd); fd = -1;
    }
    freeaddrinfo(res);

    if (fd < 0) {
        LOG_ERROR("Alert: failed to connect to webhook host");
        return -1;
    }

    /* TLS handshake */
    SSL *ssl = SSL_new(g_alert_ssl_ctx);
    if (!ssl) { close(fd); return -1; }
    SSL_set_fd(ssl, fd);
    SSL_set_tlsext_host_name(ssl, host);

    if (SSL_connect(ssl) != 1) {
        LOG_ERROR("Alert: TLS handshake failed");
        SSL_free(ssl); close(fd);
        return -1;
    }

    /* Build HTTP POST request */
    char request[PAYLOAD_BUF_SIZE + 1024];
    int req_len = snprintf(request, sizeof(request),
        "POST %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "User-Agent: sentinel-d/1.0\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        path, host, payload_len, payload);

    if (req_len < 0 || (size_t)req_len >= sizeof(request)) {
        LOG_ERROR("Alert: request buffer overflow");
        SSL_shutdown(ssl); SSL_free(ssl); close(fd);
        return -1;
    }

    if (SSL_write(ssl, request, req_len) <= 0) {
        LOG_ERROR("Alert: SSL_write to webhook failed");
        SSL_shutdown(ssl); SSL_free(ssl); close(fd);
        return -1;
    }

    /* Read just enough to check the response status */
    char resp[512];
    int  n = SSL_read(ssl, resp, sizeof(resp) - 1);
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(fd);

    if (n <= 0) {
        LOG_WARN("Alert: no response from Discord webhook");
        return 0; /* The payload was sent; Discord may have accepted it */
    }
    resp[n] = '\0';

    /* Discord returns 204 No Content on success */
    if (strstr(resp, "HTTP/1.1 2") || strstr(resp, "HTTP/2 2")) {
        return 0;
    }

    /* Extract status for logging */
    char *sp = strchr(resp, ' ');
    int code = sp ? atoi(sp + 1) : 0;
    LOG_WARN("Alert: Discord webhook returned HTTP %d", code);
    return (code >= 400) ? -1 : 0;
}

/* ---------------------------------------------------------------------------
 * Public API
 * --------------------------------------------------------------------------- */

int alert_send(const SentinelConfig *cfg,
               const char *endpoint,
               const HttpResult *result,
               int consecutive)
{
    char payload[PAYLOAD_BUF_SIZE];
    int n = alert_build_payload(payload, sizeof(payload),
                                ALERT_TYPE_FAILURE,
                                cfg, endpoint, result, consecutive);
    if (n < 0) {
        LOG_ERROR("Alert: payload assembly failed (buffer too small?)");
        return -1;
    }

    LOG_INFO("Alert: sending failure alert for %s (consecutive=%d)",
             endpoint, consecutive);

    return webhook_post(cfg->discord_webhook_url, payload, (size_t)n);
}

int alert_send_recovery(const SentinelConfig *cfg, const char *endpoint)
{
    char payload[PAYLOAD_BUF_SIZE];
    int n = alert_build_payload(payload, sizeof(payload),
                                ALERT_TYPE_RECOVERY,
                                cfg, endpoint, NULL, 0);
    if (n < 0) {
        LOG_ERROR("Alert: recovery payload assembly failed");
        return -1;
    }

    LOG_INFO("Alert: sending recovery alert for %s", endpoint);

    return webhook_post(cfg->discord_webhook_url, payload, (size_t)n);
}
