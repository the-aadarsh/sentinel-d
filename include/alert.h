#ifndef SENTINEL_ALERT_H
#define SENTINEL_ALERT_H

#include "config.h"
#include "http_client.h"

/* ---------------------------------------------------------------------------
 * Alert types.
 * --------------------------------------------------------------------------- */
typedef enum {
    ALERT_TYPE_FAILURE  = 0,  /* Endpoint is unhealthy */
    ALERT_TYPE_RECOVERY = 1   /* Endpoint has recovered */
} AlertType;

/* ---------------------------------------------------------------------------
 * Build a Discord webhook JSON payload into buf (max buf_size bytes).
 * This is a pure function with no side effects — suitable for unit testing.
 *   type             – ALERT_TYPE_FAILURE or ALERT_TYPE_RECOVERY
 *   cfg              – runtime configuration (service name, environment)
 *   endpoint         – the URL that was checked
 *   result           – HTTP result (may be NULL for recovery alerts)
 *   consecutive      – how many failures in a row (0 for recovery)
 * Returns the number of bytes written (excl. NUL), or -1 on truncation.
 * --------------------------------------------------------------------------- */
int alert_build_payload(char *buf, size_t buf_size,
                        AlertType type,
                        const SentinelConfig *cfg,
                        const char *endpoint,
                        const HttpResult *result,
                        int consecutive);

/* ---------------------------------------------------------------------------
 * Send an alert to the Discord webhook configured in cfg.
 * Returns 0 on success, -1 on failure (logs reason).
 * --------------------------------------------------------------------------- */
int alert_send(const SentinelConfig *cfg,
               const char *endpoint,
               const HttpResult *result,
               int consecutive);

/* Send a recovery alert (endpoint is back to healthy). */
int alert_send_recovery(const SentinelConfig *cfg,
                        const char *endpoint);

#endif /* SENTINEL_ALERT_H */
