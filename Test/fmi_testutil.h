#ifndef FMILIB_TESTUTIL_H
#define FMILIB_TESTUTIL_H

#include <stdio.h>

#include "fmilib.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  TODO: Decide about naming convention for test utils functions.
 */

typedef struct fmi3_testutil_import_t {
    fmi3_import_t* fmu;
    jm_vector(jm_voidp) log;     // All logged messages
    jm_vector(jm_voidp) errLog;  // List of fatal and error messages
    jm_callbacks cb;             // Holds the test-logger and its context
} fmi3_testutil_import_t;

/**
 * TODO: Remove this function. Instead of using this function, build the path from defines in config_test.h.
 */
void fmi_testutil_build_xml_path(char* buf, size_t bufSize, const char* basePath, const char* appendPath);

/**
 * This function is called before a test fails via macro, so you can put a
 * breakpoint on it to make debugging easier
 */
void fmi_testutil_enter_breakpoint();

/**
 * Parses the modelDescription.xml which is located in 'xmldir'.
 * The returned pointer must be freed with 'fmi3_import_free'.
 */
fmi3_import_t* fmi3_testutil_parse_xml(const char* xmldir);

/**
 * Parse an XML and save the log.
 */
fmi3_testutil_import_t* fmi3_testutil_parse_xml_with_log(const char* xmldir);

/**
 * Free the object.
 */
void fmi3_testutil_import_free(fmi3_testutil_import_t* testfmu);

/**
 * Returns the number of logged errors.
 */
size_t fmi3_testutil_get_num_errors(fmi3_testutil_import_t* testfmu);

/**
 * Returns true if any logged message constains the given message substring.
 */
bool fmi3_testutil_log_contains(fmi3_testutil_import_t* testfmu, const char* msgSubstr);

/**
 * Immediately fails the test by exiting the program.
 */
void fail(const char* fmt, ...);

/**
 * Concats two strings. Caller must free the returned string.
 */
char* concat(char *s1, char *s2);

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
        printf("  Assertion FAILED at line %d: ", __LINE__);                \
        printf(__VA_ARGS__);                                                \
        printf("\n");                                                       \
    } while (0)
#endif

#define TEST_FAILED(...)                                                    \
    do {                                                                    \
        PRINT_FAILURE(__VA_ARGS__);                                         \
        fmi_testutil_enter_breakpoint();                                    \
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
            fmi_testutil_enter_breakpoint();                                \
            goto label;                                                     \
        }                                                                   \
    } while (0)

/**
 * TODO: Align with CTEST_RETURN_SUCCESS etc. Do not use for new code until OK is 0 and FAIL is non-zero.
 */
#define TEST_OK (1)
#define TEST_FAIL (0)

#ifdef __cplusplus
}
#endif

#endif /* FMILIB_TESTUTIL_H */