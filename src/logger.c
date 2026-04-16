/*
 * logger.c – structured log emitter for SENTINEL-D
 *
 * Thread-safety: flockfile/funlockfile protects concurrent writes to the log
 * FILE handle.  All other state is written once at init time.
 */

/* Feature test macros for POSIX compatibility */
#define _POSIX_C_SOURCE 200112L
#define _DEFAULT_SOURCE

#include "logger.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

/* ---------------------------------------------------------------------------
 * Module-level state (initialized by log_init).
 * --------------------------------------------------------------------------- */
static LogLevel g_level = LOG_INFO;
static FILE *g_file = NULL;  /* NULL → use stderr */
static int g_file_owned = 0; /* 1 if we opened g_file ourselves */

static const char *LEVEL_TAGS[] = {
    "DEBUG", "INFO ", "WARN ", "ERROR"};

/* ---------------------------------------------------------------------------
 * Public API
 * --------------------------------------------------------------------------- */

int log_init(LogLevel level, const char *path)
{
    g_level = level;

    if (path && path[0] != '\0')
    {
        g_file = fopen(path, "a");
        if (!g_file)
        {
            fprintf(stderr, "[ERROR] Failed to open log file: %s\n", path);
            return -1;
        }
        g_file_owned = 1;
        /* Line-buffer the log file so each entry lands on disk promptly. */
        setvbuf(g_file, NULL, _IOLBF, 0);
    }
    else
    {
        g_file = stderr;
        g_file_owned = 0;
    }
    return 0;
}

void log_close(void)
{
    if (g_file_owned && g_file)
    {
        fclose(g_file);
        g_file = NULL;
        g_file_owned = 0;
    }
}

void log_msg(LogLevel level, const char *fmt, ...)
{
    if (level < g_level)
        return;

    FILE *out = g_file ? g_file : stderr;

    char ts[32];
    time_iso8601(ts, sizeof(ts));

    flockfile(out);

    fprintf(out, "%s [%s] ", ts, LEVEL_TAGS[level]);

    va_list ap;
    va_start(ap, fmt);
    vfprintf(out, fmt, ap);
    va_end(ap);

    fputc('\n', out);
    funlockfile(out);
}

LogLevel log_level_from_str(const char *s)
{
    if (!s)
        return LOG_INFO;
    if (str_casecmp(s, "DEBUG") == 0)
        return LOG_DEBUG;
    if (str_casecmp(s, "INFO") == 0)
        return LOG_INFO;
    if (str_casecmp(s, "WARN") == 0)
        return LOG_WARN;
    if (str_casecmp(s, "ERROR") == 0)
        return LOG_ERROR;
    return LOG_INFO;
}
