#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

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
    if (sscanf(str, "%lf", &val) != 1) {
        fail("call failed: sscanf");
    }

    if (!dblAlmostEq(expected, val)) {
        fail("parsed double not equal expected");
    }
}

static void test_parse_with_locale() {
    jm_callbacks* cb = jm_get_default_callbacks();
    jm_locale_t* jmloc1 = NULL;
    double dval = 0;
    const char* str_comma = "2,5";
    const char* str_point = "2.5";

    /* Any locale that uses decimal coma instead of decimal point. */
#ifdef WIN32
    char* locale_bad = "Swedish_Sweden.1252"; 
#else
    char* locale_bad = "sv_SE.utf8";
#endif

    jmloc1 = jm_mtsafe_setlocale_numeric(cb, locale_bad);
    if (!jmloc1) {
        fail("call failed: jm_mtsafe_setlocale_numeric");
    }

    /* Check that decimal comma works */
    sscanf_double(str_comma, 2.5);
    sscanf_double(str_point, 2.0);

    /* Check that decimal point works (nested setlocale_numeric call) */
    {
        jm_locale_t* jmloc2 = jm_mtsafe_setlocale_numeric(cb, "C");
        if (!jmloc2) {
            fail("call failed: jm_mtsafe_setlocale_numeric");
        }

        /* Check that decimal point works */
        sscanf_double(str_comma, 2.0);
        sscanf_double(str_point, 2.5);

        if (jm_mtsafe_resetlocale_numeric(cb, jmloc2)) {
            fail("call failed: jm_mtsafe_setlocale_numeric (nested)");
        }
        jmloc2 = NULL;
    }

    /* Check that decimal comma works, after restoring */
    sscanf_double(str_comma, 2.5);
    sscanf_double(str_point, 2.0);

    if (jm_mtsafe_resetlocale_numeric(cb, jmloc1)) {
        fail("call failed: jm_mtsafe_setlocale_numeric (nested)");
    }
    jmloc1 = NULL;
}

int main() {

    test_parse_with_locale();

    return CTEST_RETURN_SUCCESS;
}
