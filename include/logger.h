#ifndef SENTINEL_LOGGER_H
#define SENTINEL_LOGGER_H

#include <stdio.h>

/* ---------------------------------------------------------------------------
 * Log levels (ascending severity).
 * --------------------------------------------------------------------------- */
typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO  = 1,
    LOG_WARN  = 2,
    LOG_ERROR = 3
} LogLevel;

/* ---------------------------------------------------------------------------
 * Initialize the logger.
 *   level   – minimum level to emit
 *   path    – file path to log to; NULL means stderr
 * Returns 0 on success, -1 on error.
 * --------------------------------------------------------------------------- */
int  log_init(LogLevel level, const char *path);

/* Flush and close the log file (if one was opened). */
void log_close(void);

/* Emit a formatted log line.  Includes ISO-8601 UTC timestamp and level tag. */
void log_msg(LogLevel level, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));

/* Convenience macros – hoist call-site location into the message. */
#define LOG_DEBUG(...)  log_msg(LOG_DEBUG, __VA_ARGS__)
#define LOG_INFO(...)   log_msg(LOG_INFO,  __VA_ARGS__)
#define LOG_WARN(...)   log_msg(LOG_WARN,  __VA_ARGS__)
#define LOG_ERROR(...)  log_msg(LOG_ERROR, __VA_ARGS__)

/* Convert a level string ("DEBUG","INFO","WARN","ERROR") to LogLevel.
 * Returns LOG_INFO for unknown strings. */
LogLevel log_level_from_str(const char *s);

#endif /* SENTINEL_LOGGER_H */
