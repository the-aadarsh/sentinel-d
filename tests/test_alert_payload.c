/*
 * test_alert_payload.c – unit tests for alert_build_payload().
 *
 * alert_build_payload() is a pure function: no sockets, no I/O.
 * We verify that the output is valid JSON-shaped text containing the
 * expected fields for both failure and recovery alert types.
 */

#include "alert.h"
#include "config.h"
#include "http_client.h"
#include "utils.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Minimal config suitable for testing */
static SentinelConfig make_test_config(void)
{
    SentinelConfig cfg;
    config_set_defaults(&cfg);
    str_copy_safe(cfg.service_name, "test-service", sizeof(cfg.service_name));
    str_copy_safe(cfg.environment,  "staging",      sizeof(cfg.environment));
    str_copy_safe(cfg.discord_webhook_url,
                  "https://discord.com/api/webhooks/x/y",
                  sizeof(cfg.discord_webhook_url));
    return cfg;
}

/* ---------------------------------------------------------------------------
 * Test: failure payload contains expected fields.
 * --------------------------------------------------------------------------- */
static void test_failure_payload_status_code(void)
{
    SentinelConfig cfg = make_test_config();

    HttpResult result = {0};
    result.error       = HTTP_ERR_NONE;
    result.status_code = 503;
    result.latency_ms  = 142;

    char buf[4096];
    int n = alert_build_payload(buf, sizeof(buf),
                                ALERT_TYPE_FAILURE,
                                &cfg,
                                "https://api.example.com/health",
                                &result, 3);

    assert(n > 0 && "Payload build returned error");

    /* Must contain key Discord embed fields */
    assert(strstr(buf, "\"embeds\"")             != NULL);
    assert(strstr(buf, "\"color\"")              != NULL);
    assert(strstr(buf, "Service Unhealthy")      != NULL);
    assert(strstr(buf, "test-service")           != NULL);
    assert(strstr(buf, "staging")                != NULL);
    assert(strstr(buf, "api.example.com")        != NULL);
    assert(strstr(buf, "503")                    != NULL);

    /* Must be valid JSON-ish: starts with '{', ends with '}' */
    assert(buf[0] == '{');
    assert(buf[n-1] == '}');

    config_free(&cfg);
    printf("  [PASS] test_failure_payload_status_code\n");
}

/* ---------------------------------------------------------------------------
 * Test: failure payload for connection error (no status code).
 * --------------------------------------------------------------------------- */
static void test_failure_payload_connection_error(void)
{
    SentinelConfig cfg = make_test_config();

    HttpResult result = {0};
    result.error       = HTTP_ERR_CONNECT;
    result.status_code = 0;
    str_copy_safe(result.error_msg, "Connection refused", sizeof(result.error_msg));

    char buf[4096];
    int n = alert_build_payload(buf, sizeof(buf),
                                ALERT_TYPE_FAILURE,
                                &cfg,
                                "https://db.internal/health",
                                &result, 5);

    assert(n > 0);
    assert(strstr(buf, "Service Unhealthy") != NULL);
    assert(strstr(buf, "test-service")      != NULL);
    assert(strstr(buf, "db.internal")       != NULL);
    assert(buf[0] == '{');
    assert(buf[n-1] == '}');

    config_free(&cfg);
    printf("  [PASS] test_failure_payload_connection_error\n");
}

/* ---------------------------------------------------------------------------
 * Test: recovery payload is green and contains service info.
 * --------------------------------------------------------------------------- */
static void test_recovery_payload(void)
{
    SentinelConfig cfg = make_test_config();

    char buf[4096];
    int n = alert_build_payload(buf, sizeof(buf),
                                ALERT_TYPE_RECOVERY,
                                &cfg,
                                "https://api.example.com/health",
                                NULL, 0);

    assert(n > 0);
    assert(strstr(buf, "Service Recovered") != NULL);
    assert(strstr(buf, "test-service")      != NULL);
    assert(strstr(buf, "staging")           != NULL);
    /* Green colour for recovery */
    assert(strstr(buf, "3066993")           != NULL);
    assert(buf[0] == '{');
    assert(buf[n-1] == '}');

    config_free(&cfg);
    printf("  [PASS] test_recovery_payload\n");
}

/* ---------------------------------------------------------------------------
 * Test: JSON escaping of special characters in service name.
 * --------------------------------------------------------------------------- */
static void test_payload_json_escaping(void)
{
    SentinelConfig cfg = make_test_config();
    /* Inject characters that need JSON escaping */
    str_copy_safe(cfg.service_name, "svc\"quotes\\slash",
                  sizeof(cfg.service_name));

    HttpResult result = {0};
    result.status_code = 500;

    char buf[4096];
    int n = alert_build_payload(buf, sizeof(buf),
                                ALERT_TYPE_FAILURE,
                                &cfg,
                                "https://api.example.com/health",
                                &result, 1);

    assert(n > 0);
    /* The raw quote and backslash should be escaped in the output */
    assert(strstr(buf, "svc\\\"quotes\\\\slash") != NULL);

    config_free(&cfg);
    printf("  [PASS] test_payload_json_escaping\n");
}

/* ---------------------------------------------------------------------------
 * Test: buffer too small returns -1 without crashing.
 * --------------------------------------------------------------------------- */
static void test_payload_truncation(void)
{
    SentinelConfig cfg = make_test_config();
    HttpResult result  = {0};
    result.status_code = 503;

    char tiny[16];
    int n = alert_build_payload(tiny, sizeof(tiny),
                                ALERT_TYPE_FAILURE,
                                &cfg,
                                "https://example.com/health",
                                &result, 1);
    /* Must return -1 on truncation, not crash */
    assert(n == -1);

    config_free(&cfg);
    printf("  [PASS] test_payload_truncation\n");
}

/* ---------------------------------------------------------------------------
 * main
 * --------------------------------------------------------------------------- */
int main(void)
{
    printf("=== test_alert_payload ===\n");

    test_failure_payload_status_code();
    test_failure_payload_connection_error();
    test_recovery_payload();
    test_payload_json_escaping();
    test_payload_truncation();

    printf("All alert payload tests passed.\n");
    return 0;
}
