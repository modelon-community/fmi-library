#include <stdio.h>
#include <stdlib.h>

#include <fmilib.h>
#include "config_test.h"
#include "fmil_test.h"

#define INVALID_TESTS_SILENT (0)

#define CHECK_ALLOC(MEM_PTR) \
    if (!MEM_PTR) { \
        printf("Failed to alloc memory\n"); \
        exit(1); /* No point in recovering since it's just a test */ \
    }

/*
   General interface for checking log messages. It follows the same pattern as
   "logger" and "componentEnvironment" in the FMI API.
 */
typedef void (*check_log_fcn)(void* context, const char* module, jm_log_level_enu_t log_level, const char* msg);
typedef struct {
    check_log_fcn check_fcn;     /* function that performs check */
    void*         check_fcn_ctx; /* context for check_fcn */
    int           silent;        /* does not print log to standard output */
    int           success;       /* contains 1 if complete parsing check succeeded, else 0 */
    const char*   err_msg;       /* error message to print on failure */
} check_log_ctx;

static void test_logger(jm_callbacks* cb, jm_string module, jm_log_level_enu_t log_level, jm_string message)
{
    /* Perform check */
    check_log_ctx* log_ctx = (check_log_ctx*)cb->context;
    log_ctx->check_fcn(log_ctx, module, log_level, message);

    /* Print log to standard output */
    if (!log_ctx->silent) {
        jm_default_logger(cb, module, log_level, message);
    }
}


/**
    TODO: function copied from PR 21; extract when merged
*/
static char* concat(char* s1, char* s2)
{
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    /* +1 for the zero-terminator */
    char* result = (char*)malloc((len1 + len2 + 1) * sizeof(char));
    if (result == NULL) {
        exit(CTEST_RETURN_FAIL);
    }
    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1); /* +1 to copy the null-terminator */
    return result;
}

/**
    TODO: function copied from PR 21; extract when merged
*/
static fmi2_import_t* parse_xml(const char* model_desc_path, check_log_ctx* chk_log_ctx)
{
    jm_callbacks* cb = jm_get_default_callbacks();
    fmi_import_context_t* ctx = fmi_import_allocate_context(cb);
    fmi2_import_t* xml;

    if (chk_log_ctx) {
        cb->logger = test_logger;
        cb->context = chk_log_ctx;
    }

    if (ctx == NULL) {
        return NULL;
    }

    xml = fmi2_import_parse_xml(ctx, model_desc_path, NULL);

    fmi_import_free_context(ctx);
    return xml;
}

static void print_test_failure_summary(const char* err_msg, const char* xmlDir) {
    printf("---\n");
    printf("Test failed.\n");
    printf("Error message: \n");
    printf("  ");
    printf("%s\n", err_msg);
    printf("XML dir:\n");
    printf("  ");
    printf("%s\n", xmlDir);
    printf("---\n");
}


/**
    TODO: function copied from PR 21; extract when merged (NOTE: Heavily
    modified on this branch, use this version when merging)
*/
/**
 * Tests that a unit referenced by a Type's attribute exists in UnitDefinitions.
 *
 * @expectFailure:
 *    If the parsing is expected to fatally fail
 *
 * @xmlTestRootDir:
 *    path to 'parser_test_xmls' directory
 *
 * @xmlDirRel:
 *    Relative path to the XML directory from root dir for parser test
 *    XMLs. Must start with leading '/' character.
 *
 * @chk_log_ctx:
 *    logger context to use for checking the log. Success status is
 *    cleared before parsing. If NULL, default logger will be used, and log
 *    checking will be skipped.
 *
 * @xmlOut:
 *    return arg that will point to the parsed XML struct. Provide NULL
 *    if not desired. Must be freed if requested.
 */
static int test_parse_xml(int expectFailure, char* xmlTestRootDir, char* xmlDirRel, check_log_ctx* chk_log_ctx,
    fmi2_import_t** xmlOut)
{
    char* xmlDir; /* path to the XML's parent directory */
    fmi2_import_t* xml;
    int ret = CTEST_RETURN_SUCCESS;

    xmlDir = concat(xmlTestRootDir, xmlDirRel);

    if (!chk_log_ctx->silent) {
        printf("\n");
        printf("Running test in relative dir: %s\n", xmlDirRel);
        printf("-----------------------------\n");
    }

    xml = parse_xml(xmlDir, chk_log_ctx);
    if (xml != NULL && expectFailure) {
        printf("Expected parsing to fail, but didn't. XML dir: '%s'", xmlDir);
        ret = CTEST_RETURN_FAIL;
        goto clean2;
    } else if (xml == NULL && !expectFailure) {
        printf("Unexpectedly failed to parse XML. XML dir: '%s'", xmlDir);
        ret = CTEST_RETURN_FAIL;
        goto clean1;
    }

    if (chk_log_ctx && !chk_log_ctx->success) {
        print_test_failure_summary(chk_log_ctx->err_msg, xmlDir);
        ret = CTEST_RETURN_FAIL;
    }

    if (xmlOut && ret != CTEST_RETURN_FAIL) {
        *xmlOut = xml;
    }

clean2:
    if (!xmlOut) {
        fmi2_import_free(xml);
    }
clean1:
    free(xmlDir);

    return ret;
}

typedef struct {
    const char* expected_msg;
} check_fcn_ctx_log_once;

/* Checks that a message is logged once or more. */
static void check_fcn_log_once(void* chk_log_ctx, const char* module, jm_log_level_enu_t log_level, const char* msg)
{
    check_log_ctx* log_ctx = (check_log_ctx*)chk_log_ctx;
    check_fcn_ctx_log_once* fcn_ctx = (check_fcn_ctx_log_once*)log_ctx->check_fcn_ctx;

    if (0 == strcmp(fcn_ctx->expected_msg, msg)) {
        log_ctx->success = 1;
    }
}

/**
   Returns a pointer to a check_log_ctx, or exits program unsuccessfully on
   failure. Must be freed.
 */
static check_log_ctx* create_ctx_log_once(const char* expected_msg) {
    check_log_ctx* log_ctx = malloc(sizeof(check_log_ctx));
    CHECK_ALLOC(log_ctx);

    log_ctx->check_fcn = check_fcn_log_once;

    log_ctx->check_fcn_ctx = malloc(sizeof(check_fcn_ctx_log_once));
    CHECK_ALLOC(log_ctx->check_fcn_ctx);

    ((check_fcn_ctx_log_once*)log_ctx->check_fcn_ctx)->expected_msg = expected_msg;

    log_ctx->silent  = INVALID_TESTS_SILENT;
    log_ctx->success = 0;
    log_ctx->err_msg = "Failed to find expected error message in log.";

    return log_ctx;
}

static void free_ctx_log_once(check_log_ctx** chk_log_ctx) {
    free((*chk_log_ctx)->check_fcn_ctx);
    free(*chk_log_ctx);
    *chk_log_ctx = NULL;
}

/* Checks that all log entries' log levels are less than warning. */
static void check_fcn_fail_on_warn_or_worse(void* chk_log_ctx, const char* module, jm_log_level_enu_t log_level, const char* msg)
{
    check_log_ctx* log_ctx = (check_log_ctx*)chk_log_ctx;

    if (    log_level == jm_log_level_warning ||
            log_level == jm_log_level_error   ||
            log_level == jm_log_level_fatal) {
        log_ctx->success = 0;
    }
}

/**
   Returns a pointer to a check_log_ctx that checks for warning or worse log
   messages. Program exist unsuccessfully on failure to return pointer. Pointer
   must be freed with respective free-method.
 */
static check_log_ctx* create_ctx_fail_on_warn_or_worse() {
    check_log_ctx* log_ctx = malloc(sizeof(check_log_ctx));
    CHECK_ALLOC(log_ctx);

    log_ctx->check_fcn     = check_fcn_fail_on_warn_or_worse;
    log_ctx->check_fcn_ctx = NULL;
    log_ctx->silent        = INVALID_TESTS_SILENT;
    log_ctx->success       = 1;
    log_ctx->err_msg       = "Found log entries with too high log level";

    return log_ctx;
}

static void free_ctx_fail_on_warn_or_worse(check_log_ctx** chk_log_ctx) {
    free(*chk_log_ctx);
    *chk_log_ctx = NULL;
}

int main(int argc, char **argv)
{
    char* xml_dir = NULL;
    int ret = 0;
    check_log_ctx* log_ctx;

    if (argc != 2) {
        printf("Usage: %s <path_to_'model_structure'_dir>\n", argv[0]);
        return CTEST_RETURN_FAIL;
    }

    /* ================ */
    /* -- Test valid -- */
    /* ================ */
    log_ctx = create_ctx_fail_on_warn_or_worse();
    ret |= test_parse_xml(0, argv[1], "/model_structure/valid", log_ctx, NULL);
    free_ctx_fail_on_warn_or_worse(&log_ctx);


    /* ================== */
    /* -- Test invalid -- */
    /* ================== */

    /* Test that an error is raised when a ModelStructure.Derivatives list
       references a variable that doesn't have the attribute 'derivative'.
     */
    log_ctx = create_ctx_log_once("The state derivative 'state_var' does not specify the state variable "
                                  "that it is a derivative of.");
    ret |= test_parse_xml(1, argv[1], "/model_structure/invalid/derivative_reference", log_ctx, NULL);
    free_ctx_log_once(&log_ctx);

    /* Test that an error is raised when a ModelStructure.Outputs list
       doesn't contain a reference to all variables with causality="output".
     */
    log_ctx = create_ctx_log_once("Output variable not found in ModelStructure.Outputs (index: '2')");
    ret |= test_parse_xml(0, argv[1], "/model_structure/invalid/outputs_missing", log_ctx, NULL);
    free_ctx_log_once(&log_ctx);

    /* Test that an error is raised when a ModelStructure.Outputs list
       references a variable with causality!="output".
     */
    log_ctx = create_ctx_log_once("ModelStructure.Outputs listed a variable that doesn't have "
                                  "causality='output' (index: '1')");
    ret |= test_parse_xml(0, argv[1], "/model_structure/invalid/output_reference", log_ctx, NULL);
    free_ctx_log_once(&log_ctx);

    /* Test that an error is raised when a ModelStructure.Outputs list
       has a dependency to an invalid variable.
     */
    log_ctx = create_ctx_log_once("Dependency for Outputs.Unknown incorrect. Expected continuous state variable, "
                                  "input or output. Dependency's index: '2'");
    ret |= test_parse_xml(0, argv[1], "/model_structure/invalid/output_deps", log_ctx, NULL);
    free_ctx_log_once(&log_ctx);

    /* Test that an error is raised when a ModelStructure.Derivatives list
       has a dependency to an invalid variable.
     */
    log_ctx = create_ctx_log_once("Dependency for Derivatives.Unknown incorrect. Expected continuous state variable, "
                                  "input or output. Dependency's index: '4'");
    ret |= test_parse_xml(0, argv[1], "/model_structure/invalid/derivative_deps", log_ctx, NULL);
    free_ctx_log_once(&log_ctx);

    /* Test that an error is raised when a ModelStructure.InitialUnknown list
       has a dependency to an invalid variable.
     */
    log_ctx = create_ctx_log_once("Dependency for InitialUnknowns.Unknown incorrect. Expected input, output, or variable with "
                                  "attribute initial='exact'. Dependency's index: '1'");
    ret |= test_parse_xml(0, argv[1], "/model_structure/invalid/initunknown_deps", log_ctx, NULL);
    free_ctx_log_once(&log_ctx);


    /* Test that a fatal error is raised when a dependency contains an index to a non-existing variable. */

    /* index = 0 */
    log_ctx = create_ctx_log_once("XML element 'Unknown': listed index < 1: dependencies[0]=0");
    ret |= test_parse_xml(1, argv[1], "/model_structure/invalid/dependency_not_exist_lt1", log_ctx, NULL);
    free_ctx_log_once(&log_ctx);
    /* index = -1 */
    log_ctx = create_ctx_log_once("XML element 'Unknown': listed index < 1: dependencies[0]=-1");
    ret |= test_parse_xml(1, argv[1], "/model_structure/invalid/dependency_not_exist_lt2", log_ctx, NULL);
    free_ctx_log_once(&log_ctx);
    /* index > n_vars */
    log_ctx = create_ctx_log_once("XML element 'Unknown': listed index > number of ScalarVariables (1): dependencies[0]=2");
    ret |= test_parse_xml(1, argv[1], "/model_structure/invalid/dependency_not_exist_gt", log_ctx, NULL);
    free_ctx_log_once(&log_ctx);

    return ret;
}
