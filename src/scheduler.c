/*
 * scheduler.c – poll loop and endpoint state machine for SENTINEL-D
 *
 * The scheduler runs indefinitely, checking each endpoint every
 * CHECK_INTERVAL_SECONDS seconds.  It applies:
 *
 *   - Failure threshold: don't alert until N consecutive failures.
 *   - Cooldown: don't re-alert until COOLDOWN_SECONDS after the last alert.
 *   - Recovery: send a recovery alert when a previously failing endpoint
 *     returns to healthy.
 *
 * Graceful shutdown is handled via a volatile sig_atomic_t flag set by
 * SIGTERM / SIGINT handlers.
 */

/* Feature test macros for POSIX compatibility */
#define _POSIX_C_SOURCE 200112L
#define _DEFAULT_SOURCE

#include "scheduler.h"
#include "http_client.h"
#include "alert.h"
#include "logger.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

/* Set by signal handler; read by poll loop. */
static volatile sig_atomic_t g_stop = 0;

static void handle_signal(int sig)
{
    (void)sig;
    g_stop = 1;
}

/* ---------------------------------------------------------------------------
 * Internal: interruptible sleep
 *
 * nanosleep may be interrupted by a signal (EINTR).  We restart unless
 * g_stop has been set.
 * --------------------------------------------------------------------------- */
static void interruptible_sleep(int seconds)
{
    struct timespec remaining = {
        .tv_sec = seconds,
        .tv_nsec = 0};
    while (!g_stop)
    {
        struct timespec interrupted = {0};
        int rc = nanosleep(&remaining, &interrupted);
        if (rc == 0)
            break; /* slept full duration */
        if (errno == EINTR)
        {
            remaining = interrupted; /* partial sleep, retry */
        }
        else
        {
            break; /* unexpected error */
        }
    }
}

/* ---------------------------------------------------------------------------
 * Public API
 * --------------------------------------------------------------------------- */

void scheduler_init_states(EndpointState *states, const SentinelConfig *cfg)
{
    memset(states, 0, sizeof(EndpointState) * (size_t)cfg->endpoint_count);
    for (int i = 0; i < cfg->endpoint_count; i++)
    {
        states[i].url = cfg->endpoints[i];
    }
}

void scheduler_process_result(EndpointState *state,
                              const SentinelConfig *cfg,
                              const HttpResult *result)
{
    int healthy = (result->error == HTTP_ERR_NONE && result->status_code == 200);

    state->total_checks++;

    if (healthy)
    {
        LOG_DEBUG("Check OK  %s  HTTP %d  %ldms",
                  state->url, result->status_code, result->latency_ms);

        if (state->is_alerting)
        {
            /* Recovering from a failure state → send recovery notification */
            state->is_alerting = 0;
            state->consecutive_fail = 0;
            alert_send_recovery(cfg, state->url);
        }
        else
        {
            state->consecutive_fail = 0;
        }
        return;
    }

    /* Unhealthy */
    state->consecutive_fail++;
    state->total_failures++;

    if (result->error != HTTP_ERR_NONE)
    {
        LOG_WARN("Check FAIL %s  error=%s (%s)  consecutive=%d",
                 state->url,
                 http_error_str(result->error),
                 result->error_msg,
                 state->consecutive_fail);
    }
    else
    {
        LOG_WARN("Check FAIL %s  HTTP %d  %ldms  consecutive=%d",
                 state->url, result->status_code, result->latency_ms,
                 state->consecutive_fail);
    }

    /* Apply failure threshold: only alert after N consecutive failures. */
    if (state->consecutive_fail < cfg->failure_threshold)
    {
        LOG_DEBUG("Below failure threshold (%d/%d), not alerting yet",
                  state->consecutive_fail, cfg->failure_threshold);
        return;
    }

    /* Apply cooldown: don't re-send alerts too frequently. */
    time_t now = time(NULL);
    if (state->is_alerting)
    {
        double elapsed = difftime(now, state->last_alert_time);
        if (elapsed < (double)cfg->cooldown_seconds)
        {
            LOG_DEBUG("Alert cooldown active (%.0fs remaining)",
                      (double)cfg->cooldown_seconds - elapsed);
            return;
        }
    }

    /* Send the alert */
    state->is_alerting = 1;
    state->last_alert_time = now;
    alert_send(cfg, state->url, result, state->consecutive_fail);
}

void scheduler_run(const SentinelConfig *cfg)
{
    /* Install signal handlers for graceful shutdown */
    struct sigaction sa = {0};
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    /* Allocate per-endpoint state on the stack (max endpoints bounded). */
    EndpointState states[CONFIG_MAX_ENDPOINTS];
    scheduler_init_states(states, cfg);

    LOG_INFO("Scheduler started: %d endpoint(s), interval=%ds",
             cfg->endpoint_count, cfg->check_interval_seconds);

    while (!g_stop)
    {
        char ts[32];
        time_iso8601(ts, sizeof(ts));
        LOG_INFO("Poll cycle start [%s]", ts);

        for (int i = 0; i < cfg->endpoint_count && !g_stop; i++)
        {
            const char *url = cfg->endpoints[i];

            /* Optional retry loop */
            HttpResult result = {0};
            int attempt;
            for (attempt = 0; attempt <= cfg->retry_count; attempt++)
            {
                if (attempt > 0)
                {
                    LOG_DEBUG("Retry %d/%d for %s", attempt, cfg->retry_count, url);
                }
                http_check(url, cfg->http_timeout_seconds, &result);
                if (result.error == HTTP_ERR_NONE && result.status_code == 200)
                    break; /* Success — no need to retry */
            }

            scheduler_process_result(&states[i], cfg, &result);
        }

        if (!g_stop)
        {
            LOG_DEBUG("Poll cycle done, sleeping %ds", cfg->check_interval_seconds);
            interruptible_sleep(cfg->check_interval_seconds);
        }
    }

    LOG_INFO("Scheduler received shutdown signal — stopping gracefully");

    /* Print a final summary */
    for (int i = 0; i < cfg->endpoint_count; i++)
    {
        LOG_INFO("Endpoint summary [%s]: checks=%lld failures=%lld",
                 states[i].url,
                 states[i].total_checks,
                 states[i].total_failures);
    }
}
