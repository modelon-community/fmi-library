#ifndef FMILIB_TEST_H
#define FMILIB_TEST_H

#include <stdio.h>

#ifdef _MSC_VER /* Visual Studio */
#define TEST_FAILED(msg)                                                    \
    do {                                                                    \
        printf("  %s FAILED: %s\n", __FUNCTION__, msg);                            \
        return 0;                                                           \
    } while (0)
#else /* __FUNCTION__ magic variable won't be available */
#define TEST_FAILED(msg)                                                    \
    do {                                                                    \
        printf("  test FAILED at line %d: %s\n", __LINE__, msg);           \
        return 0;                                                           \
    } while (0)
#endif

#define ASSERT_MSG(cond, msg)                                               \
    do {                                                                    \
        if (!(cond)) {                                                      \
            TEST_FAILED(msg);                                               \
        }                                                                   \
    } while (0)

#define TEST_OK (1)

#endif /* FMILIB_TEST_H */
