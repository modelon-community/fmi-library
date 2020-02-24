#ifndef FMILIB_TEST_H
#define FMILIB_TEST_H

#include <stdio.h>


/**
 * This function is called before a test fails via macro, so you can put a 
 * breakpoint on it to make debugging easier
 */
static void enter_breakpoint()
{
    /* you can put a breakpoint on this line */
}

/* 
    TODO: all variadic macros are giving warnings when compiled with GCC, because
    variadic macros were introduced in C99

    - should I add some flag that suppress warning, or should I remove the vararg?
 */

#ifdef _MSC_VER /* Visual Studio */
#define PRINT_FAILURE(...)                                                  \
    do {                                                                    \
        printf("  %s FAILED: ", __FUNCTION__);                              \
        printf(__VA_ARGS__);                                                \
        printf("\n");                                                       \
    } while(0)
#else /* __FUNCTION__ magic variable won't be available */
#define PRINT_FAILURE(...)                                                  \
    do {                                                                    \
        printf("  test FAILED at line %d: ", __LINE__);                     \
        printf(__VA_ARGS__);                                                \
        printf("\n");                                                       \
    } while (0)
#endif

#define TEST_FAILED(...)                                                    \
    do {                                                                    \
        PRINT_FAILURE(__VA_ARGS__);                                         \
        enter_breakpoint();                                                 \
        return 0;                                                           \
    } while (0)

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

/** 
 * Prints error and then 'goto' specified label if 'cond' is not true. Use
 * varargs argument for error message formatting.
 */
#define ASSERT_MSG_GOTO(label, cond, ...)                                   \
    do {                                                                    \
        if (!(cond)) {                                                      \
            PRINT_FAILURE(__VA_ARGS__);                                     \
            enter_breakpoint();                                             \
            goto label;                                                     \
        }                                                                   \
    } while (0)

#define TEST_OK (1)
#define TEST_FAIL (0)

#endif /* FMILIB_TEST_H */
