/*
 * test_config.c – unit tests for the config module.
 *
 * Tests config_set_defaults(), config_apply_kv(), config_load(), and
 * config_validate() by writing temp config files and verifying parsed output.
 * No external test framework; failures are reported via assert().
 */

#include "config.h"
#include "logger.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   /* unlink */

/* ---------------------------------------------------------------------------
 * Helpers
 * --------------------------------------------------------------------------- */

/* Write a config file to a temp path and return the path.
 * Caller is responsible for unlinking. */
static const char *write_tmp_config(const char *content)
{
    static char path[64];
    snprintf(path, sizeof(path), "/tmp/sentinel_test_cfg_%d.conf", getpid());

    FILE *f = fopen(path, "w");
    assert(f && "Failed to open temp config file");
    fputs(content, f);
    fclose(f);
    return path;
}

/* ---------------------------------------------------------------------------
 * Test: defaults are applied on a fresh config.
 * --------------------------------------------------------------------------- */
static void test_defaults(void)
{
    SentinelConfig cfg;
    config_set_defaults(&cfg);

    assert(cfg.check_interval_seconds == 30);
    assert(cfg.http_timeout_seconds   == 10);
    assert(cfg.retry_count            == 0);
    assert(cfg.cooldown_seconds       == 300);
    assert(cfg.failure_threshold      == 1);
    assert(cfg.log_level              == LOG_INFO);
    assert(cfg.endpoint_count         == 0);
    assert(cfg.discord_webhook_url[0] == '\0');

    printf("  [PASS] test_defaults\n");
}

/* ---------------------------------------------------------------------------
 * Test: config_apply_kv parses each supported key.
 * --------------------------------------------------------------------------- */
static void test_apply_kv(void)
{
    SentinelConfig cfg;
    config_set_defaults(&cfg);

    assert(config_apply_kv(&cfg, "SERVICE_NAME",            "my-svc")         == 0);
    assert(config_apply_kv(&cfg, "ENVIRONMENT",             "staging")        == 0);
    assert(config_apply_kv(&cfg, "CHECK_INTERVAL_SECONDS",  "60")             == 0);
    assert(config_apply_kv(&cfg, "HTTP_TIMEOUT_SECONDS",    "5")              == 0);
    assert(config_apply_kv(&cfg, "RETRY_COUNT",             "3")              == 0);
    assert(config_apply_kv(&cfg, "COOLDOWN_SECONDS",        "120")            == 0);
    assert(config_apply_kv(&cfg, "FAILURE_THRESHOLD",       "2")              == 0);
    assert(config_apply_kv(&cfg, "LOG_LEVEL",               "DEBUG")          == 0);
    assert(config_apply_kv(&cfg, "DISCORD_WEBHOOK_URL",
                                  "https://discord.com/api/webhooks/x/y")     == 0);
    assert(config_apply_kv(&cfg, "ENDPOINTS",
                           "https://a.com/health,https://b.com/health")        == 0);

    assert(strcmp(cfg.service_name, "my-svc")   == 0);
    assert(strcmp(cfg.environment, "staging")   == 0);
    assert(cfg.check_interval_seconds           == 60);
    assert(cfg.http_timeout_seconds             == 5);
    assert(cfg.retry_count                      == 3);
    assert(cfg.cooldown_seconds                 == 120);
    assert(cfg.failure_threshold                == 2);
    assert(cfg.log_level                        == LOG_DEBUG);
    assert(cfg.endpoint_count                   == 2);
    assert(strcmp(cfg.endpoints[0], "https://a.com/health") == 0);
    assert(strcmp(cfg.endpoints[1], "https://b.com/health") == 0);

    /* Unknown key should return 1 (warn), not -1 (error) */
    assert(config_apply_kv(&cfg, "UNKNOWN_KEY", "value") == 1);

    config_free(&cfg);
    printf("  [PASS] test_apply_kv\n");
}

/* ---------------------------------------------------------------------------
 * Test: config_load reads a file correctly.
 * --------------------------------------------------------------------------- */
static void test_load_file(void)
{
    const char *content =
        "# This is a comment\n"
        "\n"
        "SERVICE_NAME = test-load\n"
        "ENVIRONMENT  = prod\n"
        "CHECK_INTERVAL_SECONDS=45\n"
        "HTTP_TIMEOUT_SECONDS  = 7\n"
        "FAILURE_THRESHOLD=3\n"
        "DISCORD_WEBHOOK_URL=https://discord.com/api/webhooks/123/abc\n"
        "ENDPOINTS=https://example.com/health\n"
        "LOG_LEVEL=WARN\n";

    const char *path = write_tmp_config(content);

    SentinelConfig cfg;
    int rc = config_load(path, &cfg);
    assert(rc == 0);

    assert(strcmp(cfg.service_name, "test-load")              == 0);
    assert(strcmp(cfg.environment,  "prod")                   == 0);
    assert(cfg.check_interval_seconds == 45);
    assert(cfg.http_timeout_seconds   == 7);
    assert(cfg.failure_threshold      == 3);
    assert(cfg.endpoint_count         == 1);
    assert(strcmp(cfg.endpoints[0], "https://example.com/health") == 0);
    assert(cfg.log_level == LOG_WARN);

    config_free(&cfg);
    unlink(path);
    printf("  [PASS] test_load_file\n");
}

/* ---------------------------------------------------------------------------
 * Test: config_validate detects missing required fields.
 * --------------------------------------------------------------------------- */
static void test_validate_missing_webhook(void)
{
    SentinelConfig cfg;
    config_set_defaults(&cfg);

    /* No endpoints, no webhook: should fail */
    assert(config_validate(&cfg) < 0);

    /* Add endpoint but still no webhook: should fail */
    config_apply_kv(&cfg, "ENDPOINTS", "https://example.com/health");
    assert(config_validate(&cfg) < 0);

    /* Add webhook: should pass */
    config_apply_kv(&cfg, "DISCORD_WEBHOOK_URL",
                    "https://discord.com/api/webhooks/x/y");
    assert(config_validate(&cfg) == 0);

    config_free(&cfg);
    printf("  [PASS] test_validate_missing_webhook\n");
}

/* ---------------------------------------------------------------------------
 * Test: comma-separated endpoints with surrounding whitespace.
 * --------------------------------------------------------------------------- */
static void test_endpoint_parsing(void)
{
    SentinelConfig cfg;
    config_set_defaults(&cfg);

    config_apply_kv(&cfg, "ENDPOINTS",
        "  https://alpha.com/health , https://beta.com/health ,https://gamma.com/");

    assert(cfg.endpoint_count == 3);
    assert(strcmp(cfg.endpoints[0], "https://alpha.com/health") == 0);
    assert(strcmp(cfg.endpoints[1], "https://beta.com/health")  == 0);
    assert(strcmp(cfg.endpoints[2], "https://gamma.com/")       == 0);

    config_free(&cfg);
    printf("  [PASS] test_endpoint_parsing\n");
}

/* ---------------------------------------------------------------------------
 * main
 * --------------------------------------------------------------------------- */
int main(void)
{
    printf("=== test_config ===\n");

    test_defaults();
    test_apply_kv();
    test_load_file();
    test_validate_missing_webhook();
    test_endpoint_parsing();

    printf("All config tests passed.\n");
    return 0;
}
