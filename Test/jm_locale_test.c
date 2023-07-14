#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <locale.h>

#include "config_test.h"

#include <JM/jm_portability.h>

static void fail(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("Test failure: ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);

    exit(CTEST_RETURN_FAIL);
}

/* Very coarse comparison, enough for this test though. */
static int dblAlmostEq(double lhs, double rhs) {
    return fabs(lhs - rhs) < 0.0001;
}

static void sscanf_double(const char* str, double expected) {
    double val = 0;
    printf("Debug: setlocale: %s\n", setlocale(LC_NUMERIC, NULL));
    if (sscanf(str, "%lf", &val) != 1) {
        fail("call failed: sscanf");
    }

    if (!dblAlmostEq(expected, val)) {
        fail("parsed double did not equal expected, str: %s, expected: %f, actual: %f", str, expected, val);
    }
}

static void test_parse_with_locale() {
    jm_callbacks* cb = jm_get_default_callbacks();
    jm_locale_t* jmloc1 = NULL;
    const char* str_comma = "2,5";
    const char* str_point = "2.5";

    /* Any locale that uses decimal comma instead of decimal point. */
#ifdef WIN32
    char* locale_bad = "French_France.1252"; /* 'sv-SE' does not exist on Jenkins nodes */
#else
    char* locale_bad = "sv_SE.utf8";
#endif


    jmloc1 = jm_setlocale_numeric(cb, locale_bad);
    if (!jmloc1) {
        fail("call failed: jm_setlocale_numeric");
    }

    printf("Debug: parsing with str_comma expected\n");
    /* Check that decimal comma works */
    sscanf_double(str_comma, 2.5);
    sscanf_double(str_point, 2.0);

    /* Check that decimal point works (nested setlocale_numeric call) */
    {
        jm_locale_t* jmloc2 = jm_setlocale_numeric(cb, "C");
        if (!jmloc2) {
            fail("call failed: jm_setlocale_numeric");
        }

        printf("Debug: parsing with str_point expected\n");
        /* Check that decimal point works */
        sscanf_double(str_comma, 2.0);
        sscanf_double(str_point, 2.5);

        if (jm_resetlocale_numeric(cb, jmloc2)) {
            fail("call failed: jm_setlocale_numeric (nested)");
        }
        jmloc2 = NULL;
    }

    printf("Debug: parsing with str_comma expected (after restore)\n");
    /* Check that decimal comma works, after restoring */
    sscanf_double(str_comma, 2.5);
    sscanf_double(str_point, 2.0);

    if (jm_resetlocale_numeric(cb, jmloc1)) {
        fail("call failed: jm_setlocale_numeric");
    }
    jmloc1 = NULL;
}

int main() {

#ifdef FMILIB_TEST_LOCALE
    test_parse_with_locale();
#endif

    return CTEST_RETURN_SUCCESS;
}
