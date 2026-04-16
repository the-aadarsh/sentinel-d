/*
 * utils.c – shared utility functions for SENTINEL-D
 */

/* Feature test macros for POSIX compatibility */
#define _POSIX_C_SOURCE 200112L
#define _DEFAULT_SOURCE

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>

/* ---------------------------------------------------------------------------
 * String helpers
 * --------------------------------------------------------------------------- */

char *str_trim(char *s)
{
    if (!s)
        return s;

    /* Trim leading whitespace */
    while (*s && isspace((unsigned char)*s))
        s++;

    if (*s == '\0')
        return s;

    /* Trim trailing whitespace */
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end))
        end--;
    *(end + 1) = '\0';

    return s;
}

char *str_copy_safe(char *dst, const char *src, size_t dst_size)
{
    if (!dst || dst_size == 0)
        return dst;
    if (!src)
    {
        dst[0] = '\0';
        return dst;
    }
    size_t i = 0;
    while (i < dst_size - 1 && src[i] != '\0')
    {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
    return dst;
}

int str_casecmp(const char *a, const char *b)
{
    if (!a && !b)
        return 0;
    if (!a)
        return -1;
    if (!b)
        return 1;
    while (*a && *b)
    {
        int diff = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (diff != 0)
            return diff;
        a++;
        b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

int str_starts_with(const char *s, const char *prefix)
{
    if (!s || !prefix)
        return 0;
    while (*prefix)
    {
        if (*s != *prefix)
            return 0;
        s++;
        prefix++;
    }
    return 1;
}

/* ---------------------------------------------------------------------------
 * Time helpers
 * --------------------------------------------------------------------------- */

char *time_iso8601(char *buf, size_t len)
{
    time_t now = time(NULL);
    struct tm tm_utc;
    gmtime_r(&now, &tm_utc);
    strftime(buf, len, "%Y-%m-%dT%H:%M:%SZ", &tm_utc);
    return buf;
}

long long time_monotonic_ms(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
    {
        /* Fallback: gettimeofday (not truly monotonic but portable) */
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (long long)tv.tv_sec * 1000LL + (long long)tv.tv_usec / 1000LL;
    }
    return (long long)ts.tv_sec * 1000LL + (long long)ts.tv_nsec / 1000000LL;
}

/* ---------------------------------------------------------------------------
 * JSON helpers
 * --------------------------------------------------------------------------- */

size_t json_escape(char *dst, const char *src, size_t dst_size)
{
    if (!dst || dst_size == 0)
        return 0;
    if (!src)
    {
        dst[0] = '\0';
        return 0;
    }

    size_t written = 0;

    /* Reserve 1 byte for the NUL terminator and 1 for potential escaped char */
    while (*src && written + 2 < dst_size)
    {
        unsigned char c = (unsigned char)*src;
        switch (c)
        {
        case '"':
            dst[written++] = '\\';
            dst[written++] = '"';
            break;
        case '\\':
            dst[written++] = '\\';
            dst[written++] = '\\';
            break;
        case '\n':
            dst[written++] = '\\';
            dst[written++] = 'n';
            break;
        case '\r':
            dst[written++] = '\\';
            dst[written++] = 'r';
            break;
        case '\t':
            dst[written++] = '\\';
            dst[written++] = 't';
            break;
        default:
            if (c < 0x20)
            {
                /* Other control characters: emit \u00XX */
                if (written + 7 >= dst_size)
                    goto done;
                written += (size_t)snprintf(dst + written,
                                            dst_size - written,
                                            "\\u%04x", c);
            }
            else
            {
                dst[written++] = (char)c;
            }
            break;
        }
        src++;
    }

done:
    dst[written] = '\0';
    return written;
}

/* ---------------------------------------------------------------------------
 * File helpers
 * --------------------------------------------------------------------------- */

char *file_read_line(FILE *fp, char *buf, size_t len)
{
    if (!fp || !buf || len == 0)
        return NULL;
    if (!fgets(buf, (int)len, fp))
        return NULL;

    /* Strip trailing newline characters */
    size_t n = strlen(buf);
    while (n > 0 && (buf[n - 1] == '\n' || buf[n - 1] == '\r'))
    {
        buf[--n] = '\0';
    }
    return buf;
}
