#ifndef FMILIB_TESTUTIL_H
#define FMILIB_TESTUTIL_H

#include <stdio.h>

#include "fmilib.h"

#include "JM/jm_types.h"
#include "JM/jm_vector.h"

#ifdef __cplusplus
extern "C" {
#endif

// Require string equality. Macro depends on Catch2.
#define REQUIRE_STREQ(S1, S2)                                                                          \
    do {                                                                                               \
        if (S1 == nullptr && S2 == nullptr) { FAIL("Failed to compare strings. Both were nullptr."); } \
        else if (S1 == nullptr ) { FAIL("nullptr != " << S2); }                                        \
        else if (S2 == nullptr ) { FAIL(S1 << " != nullptr"); }                                        \
        else if (strcmp(S1, S2) != 0) { FAIL(S1 << " != " << S2); }                                    \
    } while (0);

// XXX: Currently cotains duplicated code for FMI 2 & 3 (also in fmi_testutil.c)
// Could replace via macros

typedef struct fmi2_testutil_import_t {
    fmi2_import_t* fmu;
    jm_vector(jm_voidp) log;      // All logged messages
    jm_vector(jm_voidp) warnLog;  // List of warning messages
    jm_vector(jm_voidp) errLog;   // List of error and fatal messages
    jm_callbacks cb;              // Holds the test-logger and its context
} fmi2_testutil_import_t;

typedef struct fmi3_testutil_import_t {
    fmi3_import_t* fmu;
    jm_vector(jm_voidp) log;      // All logged messages
    jm_vector(jm_voidp) warnLog;  // List of warning messages
    jm_vector(jm_voidp) errLog;   // List of error and fatal messages
    jm_callbacks cb;              // Holds the test-logger and its context
} fmi3_testutil_import_t;

/**
 * This function is called before a test fails via macro, so you can put a
 * breakpoint on it to make debugging easier
 */
void fmi_testutil_enter_breakpoint();

/**
 * Parses the modelDescription.xml which is located in 'xmldir'.
 * The returned pointer must be freed with 'fmi2_import_free'.
 */
fmi2_import_t* fmi2_testutil_parse_xml(const char* xmldir);

/**
 * Parses the modelDescription.xml which is located in 'xmldir'.
 * The returned pointer must be freed with 'fmi3_import_free'.
 */
fmi3_import_t* fmi3_testutil_parse_xml(const char* xmldir);

/**
 * Parse an XML and save the log.
 */
fmi2_testutil_import_t* fmi2_testutil_parse_xml_with_log(const char* xmldir);

/**
 * Parse an XML and save the log.
 */
fmi3_testutil_import_t* fmi3_testutil_parse_xml_with_log(const char* xmldir);

/**
 * Free the object.
 */
void fmi2_testutil_import_free(fmi2_testutil_import_t* testfmu);

/**
 * Free the object.
 */
void fmi3_testutil_import_free(fmi3_testutil_import_t* testfmu);

/**
 * Returns the number of logged errors.
 */
size_t fmi2_testutil_get_num_errors(fmi2_testutil_import_t* testfmu);

/**
 * Returns the number of logged errors.
 */
size_t fmi3_testutil_get_num_errors(fmi3_testutil_import_t* testfmu);

/**
 * Returns the number of logged problems, i.e. log-level warning or lower.
 */
size_t fmi2_testutil_get_num_problems(fmi2_testutil_import_t* testfmu);

/**
 * Returns the number of logged problems, i.e. log-level warning or lower.
 */
size_t fmi3_testutil_get_num_problems(fmi3_testutil_import_t* testfmu);

/**
 * Returns true if any logged message constains the given message substring.
 */
bool fmi2_testutil_log_contains(fmi2_testutil_import_t* testfmu, const char* msgSubstr);

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

#ifdef __cplusplus
}
#endif

#endif /* FMILIB_TESTUTIL_H */
