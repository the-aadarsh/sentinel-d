#ifndef SENTINEL_HTTP_CLIENT_H
#define SENTINEL_HTTP_CLIENT_H

/* ---------------------------------------------------------------------------
 * Result codes for HTTP health checks.
 * --------------------------------------------------------------------------- */
typedef enum {
    HTTP_ERR_NONE    = 0,  /* Request completed (may still be non-200) */
    HTTP_ERR_TIMEOUT = 1,  /* Connect or read timed out */
    HTTP_ERR_CONNECT = 2,  /* TCP connection refused / network unreachable */
    HTTP_ERR_DNS     = 3,  /* Could not resolve hostname */
    HTTP_ERR_SSL     = 4,  /* TLS handshake or certificate error */
    HTTP_ERR_PROTO   = 5,  /* Malformed HTTP response */
    HTTP_ERR_SYSTEM  = 6   /* OS-level error (errno set) */
} HttpErrorType;

/* Result of a single HTTP health check. */
typedef struct {
    int           status_code;   /* e.g. 200, 503; 0 if no response received */
    HttpErrorType error;         /* error classification */
    long          latency_ms;    /* round-trip milliseconds; 0 on hard error */
    char          error_msg[256];/* human-readable error detail */
} HttpResult;

/* ---------------------------------------------------------------------------
 * Perform an HTTP GET to url with a per-request deadline of timeout_seconds.
 * Supports http:// and https:// (requires OpenSSL at link time).
 * The result is written into *result.
 * Returns 0 if a valid HTTP response was received (check status_code for
 * health), -1 for transport / protocol level errors.
 * --------------------------------------------------------------------------- */
int http_check(const char *url, int timeout_seconds, HttpResult *result);

/* Human-readable name for an HttpErrorType. */
const char *http_error_str(HttpErrorType e);

#endif /* SENTINEL_HTTP_CLIENT_H */
