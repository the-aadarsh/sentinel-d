#ifndef SENTINEL_UTILS_H
#define SENTINEL_UTILS_H

#include <stddef.h>
#include <time.h>

/* ---------------------------------------------------------------------------
 * String helpers
 * --------------------------------------------------------------------------- */

/* Trim leading and trailing ASCII whitespace in-place. Returns s. */
char *str_trim(char *s);

/* Bounded strcpy; always NUL-terminates. Returns dst. */
char *str_copy_safe(char *dst, const char *src, size_t dst_size);

/* Case-insensitive comparison (portable). */
int str_casecmp(const char *a, const char *b);

/* Returns 1 if s starts with prefix, 0 otherwise. */
int str_starts_with(const char *s, const char *prefix);

/* ---------------------------------------------------------------------------
 * Time helpers
 * --------------------------------------------------------------------------- */

/* Write current UTC time as ISO-8601 into buf (needs at least 25 bytes).
 * Returns buf. */
char *time_iso8601(char *buf, size_t len);

/* Return monotonic milliseconds (for latency measurement). */
long long time_monotonic_ms(void);

/* ---------------------------------------------------------------------------
 * JSON helpers
 * --------------------------------------------------------------------------- */

/* Copy src into dst, escaping characters that are invalid inside a JSON string.
 * Always NUL-terminates.  Returns number of bytes written (excl. NUL). */
size_t json_escape(char *dst, const char *src, size_t dst_size);

/* ---------------------------------------------------------------------------
 * File / I/O helpers
 * --------------------------------------------------------------------------- */

/* Read one line from fp into buf (max len bytes), stripping newline chars.
 * Returns buf on success, NULL on EOF or error. */
char *file_read_line(FILE *fp, char *buf, size_t len);

#endif /* SENTINEL_UTILS_H */
