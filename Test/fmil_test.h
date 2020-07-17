#ifndef FMILIB_TEST_H
#define FMILIB_TEST_H

#include <stdio.h>

#define TEST_FAILED(msg)                                                    \
    do {                                                                    \
        printf("  %s: %s\n", __FUNCTION__, msg);                            \
        return 0;                                                           \
    } while (0)

#define ASSERT_MSG(cond, msg)                                               \
    do {                                                                    \
        if (!(cond)) {                                                      \
            TEST_FAILED(msg);                                               \
        }                                                                   \
    } while (0)

#define TEST_OK (1)

/* Below are the macros above, but copied to use CTEST_RETURN_* instead.
 * TODO: Refactor when merging with 3.0 branch so there is only one.
 */
#define TEST_FAILED_CTEST(msg)                                              \
    do {                                                                    \
        printf("  %s: %s\n", __FUNCTION__, msg);                            \
        return CTEST_RETURN_FAIL;                                           \
    } while (0)

#define ASSERT_MSG_CTEST(cond, msg)                                         \
    do {                                                                    \
        if (!(cond)) {                                                      \
            TEST_FAILED_CTEST(msg);                                         \
        }                                                                   \
    } while (0)

char* concat(char *s1, char *s2);

#endif /* FMILIB_TEST_H */
