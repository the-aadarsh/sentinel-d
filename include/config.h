#ifndef SENTINEL_CONFIG_H
#define SENTINEL_CONFIG_H

#include <stddef.h>
#include "logger.h"

/* Maximum number of endpoints supported. */
#define CONFIG_MAX_ENDPOINTS 64

/* Maximum length for a single string config value. */
#define CONFIG_VALUE_MAX 1024

/* ---------------------------------------------------------------------------
 * Configuration structure
 * All string fields are heap-allocated; call config_free() when done.
 * --------------------------------------------------------------------------- */
typedef struct {
    /* Identity */
    char service_name[CONFIG_VALUE_MAX];
    char environment[CONFIG_VALUE_MAX];

    /* Polling behaviour */
    int  check_interval_seconds;   /* how often to poll, default 30 */
    int  http_timeout_seconds;     /* per-request timeout, default 10 */
    int  retry_count;              /* retries before failure, default 0 */
    int  cooldown_seconds;         /* min seconds between repeat alerts */
    int  failure_threshold;        /* consecutive failures before alerting */

    /* Endpoints */
    char *endpoints[CONFIG_MAX_ENDPOINTS];
    int   endpoint_count;

    /* Alerting */
    char discord_webhook_url[CONFIG_VALUE_MAX];

    /* Observability */
    LogLevel log_level;
    char     log_file[CONFIG_VALUE_MAX];   /* empty string → stderr */
    char     pid_file[CONFIG_VALUE_MAX];   /* empty string → no PID file */
} SentinelConfig;

/* ---------------------------------------------------------------------------
 * API
 * --------------------------------------------------------------------------- */

/* Load config from file at path, then override with matching env vars.
 * Populates *cfg.  Returns 0 on success, -1 on error (prints reason). */
int config_load(const char *path, SentinelConfig *cfg);

/* Validate that all required fields are present and sane.
 * Returns 0 on success, -1 on validation failure. */
int config_validate(const SentinelConfig *cfg);

/* Free heap memory owned by cfg (endpoint strings). */
void config_free(SentinelConfig *cfg);

/* Print config summary to log (redacts the webhook URL). */
void config_dump(const SentinelConfig *cfg);

/* Apply a single KEY=VALUE pair to cfg (used internally and for testing). */
int config_apply_kv(SentinelConfig *cfg, const char *key, const char *value);

/* Set default values on a freshly zeroed SentinelConfig. */
void config_set_defaults(SentinelConfig *cfg);

#endif /* SENTINEL_CONFIG_H */
