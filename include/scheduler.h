#ifndef SENTINEL_SCHEDULER_H
#define SENTINEL_SCHEDULER_H

#include "config.h"
#include <time.h>
#include "http_client.h"

/* ---------------------------------------------------------------------------
 * Per-endpoint runtime state tracked across poll cycles.
 * --------------------------------------------------------------------------- */
typedef struct
{
    const char *url;          /* pointer into config->endpoints */
    int consecutive_fail;     /* number of failures in a row */
    int is_alerting;          /* 1 = we are in an active alert state */
    time_t last_alert_time;   /* epoch seconds of last alert sent */
    long long total_checks;   /* lifetime check counter */
    long long total_failures; /* lifetime failure counter */
} EndpointState;

/* ---------------------------------------------------------------------------
 * Initialize endpoint state array from config.
 * states must point to an array of at least cfg->endpoint_count entries.
 * --------------------------------------------------------------------------- */
void scheduler_init_states(EndpointState *states, const SentinelConfig *cfg);

/* ---------------------------------------------------------------------------
 * Run the main poll loop until a shutdown signal is received.
 *
 * This function blocks until SIGTERM or SIGINT is delivered.
 * It sets up signal handlers internally.
 * --------------------------------------------------------------------------- */
void scheduler_run(const SentinelConfig *cfg);

/* ---------------------------------------------------------------------------
 * Process one check result for a single endpoint.
 * Applies failure threshold, cooldown, and recovery logic.
 * Sends alerts as needed.
 * Exposed for unit testing.
 * --------------------------------------------------------------------------- */
void scheduler_process_result(EndpointState *state,
                              const SentinelConfig *cfg,
                              const HttpResult *result);

#endif /* SENTINEL_SCHEDULER_H */
