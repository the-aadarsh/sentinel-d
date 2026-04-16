/*
 * config.c – configuration loading, validation, and access for SENTINEL-D
 *
 * Config file format: KEY=value pairs, one per line.
 * Lines beginning with '#' or that are blank are ignored.
 * Whitespace around key and value is stripped.
 * Environment variables override file values when present.
 */

#include "config.h"
#include "logger.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <unistd.h> /* for POSIX functions */
#include <string.h> /* for strtok_r, strdup */

#include <strings.h> /* for POSIX strtok_r */
#include <stdlib.h>  /* for strdup */

/* ---------------------------------------------------------------------------
 * Internal helpers
 * --------------------------------------------------------------------------- */
/* Split a comma-separated endpoints string into cfg->endpoints[]. */
static void parse_endpoints(SentinelConfig *cfg, const char *value)
{
    /* Free any previously parsed endpoints */
    for (int i = 0; i < cfg->endpoint_count; i++)
    {
        free(cfg->endpoints[i]);
        cfg->endpoints[i] = NULL;
    }
    cfg->endpoint_count = 0;

    char buf[CONFIG_VALUE_MAX * 4]; /* generous buffer for all endpoints */
    str_copy_safe(buf, value, sizeof(buf));

    char *saveptr = NULL;
    char *token = strtok_r(buf, ",", &saveptr);
    while (token && cfg->endpoint_count < CONFIG_MAX_ENDPOINTS)
    {
        char *trimmed = str_trim(token);
        if (*trimmed != '\0')
        {
            cfg->endpoints[cfg->endpoint_count] = strdup(trimmed);
            if (!cfg->endpoints[cfg->endpoint_count])
            {
                /* OOM – stop parsing */
                break;
            }
            cfg->endpoint_count++;
        }
        token = strtok_r(NULL, ",", &saveptr);
    }
}

/* ---------------------------------------------------------------------------
 * Public API
 * --------------------------------------------------------------------------- */

void config_set_defaults(SentinelConfig *cfg)
{
    memset(cfg, 0, sizeof(*cfg));
    str_copy_safe(cfg->service_name, "sentinel-d", sizeof(cfg->service_name));
    str_copy_safe(cfg->environment, "production", sizeof(cfg->environment));
    cfg->check_interval_seconds = 30;
    cfg->http_timeout_seconds = 10;
    cfg->retry_count = 0;
    cfg->cooldown_seconds = 300;
    cfg->failure_threshold = 1;
    cfg->log_level = LOG_INFO;
}

int config_apply_kv(SentinelConfig *cfg, const char *key, const char *value)
{
    if (!key || !value)
        return -1;

    if (strcmp(key, "SERVICE_NAME") == 0)
    {
        str_copy_safe(cfg->service_name, value, sizeof(cfg->service_name));
    }
    else if (strcmp(key, "ENVIRONMENT") == 0)
    {
        str_copy_safe(cfg->environment, value, sizeof(cfg->environment));
    }
    else if (strcmp(key, "CHECK_INTERVAL_SECONDS") == 0)
    {
        cfg->check_interval_seconds = atoi(value);
    }
    else if (strcmp(key, "HTTP_TIMEOUT_SECONDS") == 0)
    {
        cfg->http_timeout_seconds = atoi(value);
    }
    else if (strcmp(key, "RETRY_COUNT") == 0)
    {
        cfg->retry_count = atoi(value);
    }
    else if (strcmp(key, "COOLDOWN_SECONDS") == 0)
    {
        cfg->cooldown_seconds = atoi(value);
    }
    else if (strcmp(key, "FAILURE_THRESHOLD") == 0)
    {
        cfg->failure_threshold = atoi(value);
    }
    else if (strcmp(key, "ENDPOINTS") == 0)
    {
        parse_endpoints(cfg, value);
    }
    else if (strcmp(key, "DISCORD_WEBHOOK_URL") == 0)
    {
        str_copy_safe(cfg->discord_webhook_url, value,
                      sizeof(cfg->discord_webhook_url));
    }
    else if (strcmp(key, "LOG_LEVEL") == 0)
    {
        cfg->log_level = log_level_from_str(value);
    }
    else if (strcmp(key, "LOG_FILE") == 0)
    {
        str_copy_safe(cfg->log_file, value, sizeof(cfg->log_file));
    }
    else if (strcmp(key, "PID_FILE") == 0)
    {
        str_copy_safe(cfg->pid_file, value, sizeof(cfg->pid_file));
    }
    else
    {
        /* Unknown key — not an error, just warn */
        return 1;
    }
    return 0;
}

int config_load(const char *path, SentinelConfig *cfg)
{
    config_set_defaults(cfg);

    /* ---- 1. Load from file ---- */
    FILE *fp = fopen(path, "r");
    if (!fp)
    {
        fprintf(stderr, "[ERROR] Cannot open config file '%s': %s\n",
                path, strerror(errno));
        return -1;
    }

    char line[CONFIG_VALUE_MAX * 2];
    int lineno = 0;

    while (file_read_line(fp, line, sizeof(line)))
    {
        lineno++;
        char *trimmed = str_trim(line);

        /* Skip blanks and comments */
        if (*trimmed == '\0' || *trimmed == '#')
            continue;

        /* Split at first '=' */
        char *eq = strchr(trimmed, '=');
        if (!eq)
        {
            fprintf(stderr, "[WARN ] Config line %d has no '=', skipping: %s\n",
                    lineno, trimmed);
            continue;
        }
        *eq = '\0';
        char *key = str_trim(trimmed);
        char *value = str_trim(eq + 1);

        int rc = config_apply_kv(cfg, key, value);
        if (rc > 0)
        {
            fprintf(stderr, "[DEBUG] Config: unknown key '%s' at line %d\n",
                    key, lineno);
        }
    }
    fclose(fp);

    /* ---- 2. Override with environment variables ---- */
    static const char *known_keys[] = {
        "SERVICE_NAME", "ENVIRONMENT", "CHECK_INTERVAL_SECONDS",
        "HTTP_TIMEOUT_SECONDS", "RETRY_COUNT", "COOLDOWN_SECONDS",
        "FAILURE_THRESHOLD", "ENDPOINTS", "DISCORD_WEBHOOK_URL",
        "LOG_LEVEL", "LOG_FILE", "PID_FILE", NULL};
    for (int i = 0; known_keys[i]; i++)
    {
        const char *env = getenv(known_keys[i]);
        if (env)
        {
            config_apply_kv(cfg, known_keys[i], env);
        }
    }

    return 0;
}

int config_validate(const SentinelConfig *cfg)
{
    int ok = 1;

    if (cfg->endpoint_count == 0)
    {
        fprintf(stderr, "[ERROR] Config: ENDPOINTS must contain at least one URL\n");
        ok = 0;
    }
    if (cfg->discord_webhook_url[0] == '\0')
    {
        fprintf(stderr, "[ERROR] Config: DISCORD_WEBHOOK_URL is required\n");
        ok = 0;
    }
    if (cfg->check_interval_seconds <= 0)
    {
        fprintf(stderr, "[ERROR] Config: CHECK_INTERVAL_SECONDS must be > 0\n");
        ok = 0;
    }
    if (cfg->http_timeout_seconds <= 0)
    {
        fprintf(stderr, "[ERROR] Config: HTTP_TIMEOUT_SECONDS must be > 0\n");
        ok = 0;
    }
    if (cfg->failure_threshold <= 0)
    {
        fprintf(stderr, "[ERROR] Config: FAILURE_THRESHOLD must be > 0\n");
        ok = 0;
    }
    if (cfg->cooldown_seconds < 0)
    {
        fprintf(stderr, "[ERROR] Config: COOLDOWN_SECONDS must be >= 0\n");
        ok = 0;
    }

    return ok ? 0 : -1;
}

void config_free(SentinelConfig *cfg)
{
    for (int i = 0; i < cfg->endpoint_count; i++)
    {
        free(cfg->endpoints[i]);
        cfg->endpoints[i] = NULL;
    }
    cfg->endpoint_count = 0;
}

void config_dump(const SentinelConfig *cfg)
{
    LOG_INFO("Config: service=%s env=%s interval=%ds timeout=%ds "
             "retries=%d cooldown=%ds threshold=%d endpoints=%d "
             "log_level=%d",
             cfg->service_name, cfg->environment,
             cfg->check_interval_seconds, cfg->http_timeout_seconds,
             cfg->retry_count, cfg->cooldown_seconds,
             cfg->failure_threshold, cfg->endpoint_count,
             (int)cfg->log_level);

    for (int i = 0; i < cfg->endpoint_count; i++)
    {
        LOG_INFO("Config: endpoint[%d] = %s", i, cfg->endpoints[i]);
    }

    /* Never log the webhook URL in plain text */
    LOG_INFO("Config: discord_webhook_url = <redacted>");
}
