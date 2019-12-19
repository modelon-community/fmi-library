#ifndef FMILIB_TEST_H
#define FMILIB_TEST_H

#include <stdio.h>

#ifdef _MSC_VER /* Visual Studio */
#define TEST_FAILED(...)                                                    \
    do {                                                                    \
        printf("  %s FAILED: ", __FUNCTION__);                              \
        printf(__VA_ARGS__);                                                \
        printf("\n");                                                       \
        return 0;                                                           \
    } while (0)
#else /* __FUNCTION__ magic variable won't be available */
#define TEST_FAILED(...)                                                    \
    do {                                                                    \
        printf("  test FAILED at line %d: ", __LINE__);                     \
        printf(__VA_ARGS__);                                                \
        printf("\n");                                                       \
        return 0;                                                           \
    } while (0)
#endif

/** 
 * Fails the test if 'cond' is not true. Use varargs argument for error 
 * message formatting.
 */
#define ASSERT_MSG(cond, ...)                                               \
    do {                                                                    \
        if (!(cond)) {                                                      \
            TEST_FAILED(__VA_ARGS__);                                       \
        }                                                                   \
    } while (0)

#define TEST_OK (1)

#endif /* FMILIB_TEST_H */
