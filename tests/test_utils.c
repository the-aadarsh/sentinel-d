/*
 * test_utils.c – unit tests for the utils module.
 *
 * Tests str_trim, str_copy_safe, str_casecmp, str_starts_with,
 * json_escape, and time_iso8601 format.
 */

#include "utils.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ---------------------------------------------------------------------------
 * str_trim
 * --------------------------------------------------------------------------- */
static void test_str_trim(void)
{
    char s1[] = "  hello  ";
    assert(strcmp(str_trim(s1), "hello") == 0);

    char s2[] = "no-space";
    assert(strcmp(str_trim(s2), "no-space") == 0);

    char s3[] = "   ";
    assert(strcmp(str_trim(s3), "") == 0);

    char s4[] = "\t leading-tab";
    assert(strcmp(str_trim(s4), "leading-tab") == 0);

    char s5[] = "trailing\n";
    assert(strcmp(str_trim(s5), "trailing") == 0);

    printf("  [PASS] test_str_trim\n");
}

/* ---------------------------------------------------------------------------
 * str_copy_safe
 * --------------------------------------------------------------------------- */
static void test_str_copy_safe(void)
{
    char buf[8];

    /* Normal copy */
    str_copy_safe(buf, "hello", sizeof(buf));
    assert(strcmp(buf, "hello") == 0);

    /* Truncation: "hello world" into 8 bytes → "hello w\0" */
    str_copy_safe(buf, "hello world", sizeof(buf));
    assert(buf[7] == '\0');       /* always NUL-terminated */
    assert(strncmp(buf, "hello w", 7) == 0);

    /* NULL source */
    str_copy_safe(buf, NULL, sizeof(buf));
    assert(buf[0] == '\0');

    printf("  [PASS] test_str_copy_safe\n");
}

/* ---------------------------------------------------------------------------
 * str_casecmp
 * --------------------------------------------------------------------------- */
static void test_str_casecmp(void)
{
    assert(str_casecmp("DEBUG", "debug") == 0);
    assert(str_casecmp("INFO",  "INFO")  == 0);
    assert(str_casecmp("abc",   "ABC")   == 0);
    assert(str_casecmp("A",     "B")     <  0);
    assert(str_casecmp("B",     "A")     >  0);
    assert(str_casecmp("",      "")      == 0);

    printf("  [PASS] test_str_casecmp\n");
}

/* ---------------------------------------------------------------------------
 * str_starts_with
 * --------------------------------------------------------------------------- */
static void test_str_starts_with(void)
{
    assert(str_starts_with("https://example.com", "https://") == 1);
    assert(str_starts_with("http://example.com",  "https://") == 0);
    assert(str_starts_with("",                    "https://") == 0);
    assert(str_starts_with("hello",               "")         == 1);

    printf("  [PASS] test_str_starts_with\n");
}

/* ---------------------------------------------------------------------------
 * json_escape
 * --------------------------------------------------------------------------- */
static void test_json_escape(void)
{
    char out[256];

    /* No escaping needed */
    json_escape(out, "hello world", sizeof(out));
    assert(strcmp(out, "hello world") == 0);

    /* Quote escaping */
    json_escape(out, "say \"hi\"", sizeof(out));
    assert(strcmp(out, "say \\\"hi\\\"") == 0);

    /* Backslash */
    json_escape(out, "C:\\path", sizeof(out));
    assert(strcmp(out, "C:\\\\path") == 0);

    /* Newline */
    json_escape(out, "line1\nline2", sizeof(out));
    assert(strcmp(out, "line1\\nline2") == 0);

    /* Tab */
    json_escape(out, "col1\tcol2", sizeof(out));
    assert(strcmp(out, "col1\\tcol2") == 0);

    /* Mixed */
    json_escape(out, "he said \"hello\"\nbye", sizeof(out));
    assert(strcmp(out, "he said \\\"hello\\\"\\nbye") == 0);

    /* Truncation safety: small buffer */
    char small[4];
    json_escape(small, "abcdef", sizeof(small));
    assert(small[3] == '\0');   /* must be NUL-terminated */

    /* NULL source */
    json_escape(out, NULL, sizeof(out));
    assert(out[0] == '\0');

    printf("  [PASS] test_json_escape\n");
}

/* ---------------------------------------------------------------------------
 * time_iso8601
 * --------------------------------------------------------------------------- */
static void test_time_iso8601(void)
{
    char buf[32];
    time_iso8601(buf, sizeof(buf));

    /* Format: YYYY-MM-DDTHH:MM:SSZ  (20 chars) */
    assert(strlen(buf) == 20);
    assert(buf[4]  == '-');
    assert(buf[7]  == '-');
    assert(buf[10] == 'T');
    assert(buf[13] == ':');
    assert(buf[16] == ':');
    assert(buf[19] == 'Z');

    printf("  [PASS] test_time_iso8601\n");
}

/* ---------------------------------------------------------------------------
 * time_monotonic_ms
 * --------------------------------------------------------------------------- */
static void test_time_monotonic_ms(void)
{
    long long t1 = time_monotonic_ms();
    /* Just verify it returns a positive non-zero value */
    assert(t1 > 0);

    long long t2 = time_monotonic_ms();
    /* t2 should be >= t1 */
    assert(t2 >= t1);

    printf("  [PASS] test_time_monotonic_ms\n");
}

/* ---------------------------------------------------------------------------
 * main
 * --------------------------------------------------------------------------- */
int main(void)
{
    printf("=== test_utils ===\n");

    test_str_trim();
    test_str_copy_safe();
    test_str_casecmp();
    test_str_starts_with();
    test_json_escape();
    test_time_iso8601();
    test_time_monotonic_ms();

    printf("All utils tests passed.\n");
    return 0;
}
