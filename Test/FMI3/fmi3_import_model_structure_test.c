#include <stdio.h>
#include <stdlib.h>

#include <fmilib.h>
#include "config_test.h"
#include "fmi_testutil.h"

/* Global set to 1 if logger cb found expected error message - must be reset
 * between tests. */
static int g_logger_found_err_msg = 0;

/**
    TODO: function copied from PR 21; extract when merged
*/
static fmi3_import_t* parse_xml(const char* model_desc_path, jm_logger_f logger)
{
    jm_callbacks* cb = jm_get_default_callbacks();
    fmi_import_context_t* ctx = fmi_import_allocate_context(cb);
    fmi3_import_t* xml;

    g_logger_found_err_msg = 0;

    if (logger) {
        cb->logger = logger;
    }

    if (ctx == NULL) {
        return NULL;
    }

    xml = fmi3_import_parse_xml(ctx, model_desc_path, NULL);

    fmi_import_free_context(ctx);
    return xml;
}

/**
    TODO: function copied from PR 21; extract when merged
*/
/**
 * Tests that a unit referenced by a Type's attribute exists in UnitDefinitions.
 *
 * @xmlTestRootDir: path to 'parser_test_xmls' directory
 * @xmlDirRel: Relative path to the XML directory from root dir for parser test
 *   XMLs. Must start with leading '/' character.
 * @logger: logger function to use. If NULL, default logger will be used.
 * @xmlOut: return arg that will point to the parsed XML struct. Provide NULL
 *   if not desired.
 */
static int test_parse_xml(int expectFailure, char* xmlTestRootDir, char* xmlDirRel, jm_logger_f logger,
    fmi3_import_t** xmlOut)
{
    char* xmlDir; /* path to the XML's parent directory */
    fmi3_import_t* xml;
    int ret = CTEST_RETURN_SUCCESS;

    xmlDir = concat(xmlTestRootDir, xmlDirRel);

    xml = parse_xml(xmlDir, logger);
    if (xml != NULL && expectFailure) {
        ret = CTEST_RETURN_FAIL;
        goto clean2;
    } else if (xml == NULL && !expectFailure) {
        ret = CTEST_RETURN_FAIL;
        goto clean1;
    }

    if (expectFailure && !g_logger_found_err_msg) {
        printf("Logger unexpectedly did not find error message\n");
        ret = CTEST_RETURN_FAIL;
    } else if (!expectFailure && g_logger_found_err_msg) {
        printf("Logger unexpectedly found error message\n");
        ret = CTEST_RETURN_FAIL;
    }

    if (xmlOut && ret != CTEST_RETURN_FAIL) {
        *xmlOut = xml;
    }

clean2:
    if (!xmlOut) {
        fmi3_import_free(xml);
    }
clean1:
    free(xmlDir);

    return ret;
}

/* Tests that an error is raised when a ModelStructure.Derivatives list
 * references a variable that doesn't have the attribute 'derivative'. */
static void logger_invalid_state_derivative_reference(jm_callbacks* cb, jm_string module,
        jm_log_level_enu_t log_level, jm_string message)
{
    char* expMsg = "The continuous state derivative 'state_var' does not specify the state variable that it is a derivative of.";
    if (!strncmp(expMsg, message, strlen(expMsg))) {
        g_logger_found_err_msg = 1;
    }

    jm_default_logger(cb, module, log_level, message);
}

/* Tests that an error is raised when a ModelStructure list is incorrectly ordered. */
static void logger_incorrect_order(jm_callbacks* cb, jm_string module,
        jm_log_level_enu_t log_level, jm_string message)
{
    char* expMsg = "Parse error at line";
    if (!strncmp(expMsg, message, strlen(expMsg))) {
        g_logger_found_err_msg = 1;
    }

    jm_default_logger(cb, module, log_level, message);
}

/* Tests that an error is raised when using an fmi2 style list. */
static void logger_fmi2_style_list(jm_callbacks* cb, jm_string module,
        jm_log_level_enu_t log_level, jm_string message)
{
    char* expMsg = "Parse error at line";
    if (!strncmp(expMsg, message, strlen(expMsg))) {
        g_logger_found_err_msg = 1;
    }

    jm_default_logger(cb, module, log_level, message);
}

/* Tests that an error is raised when for outputs with incorrect causality. */
static void logger_output_wrong_causality(jm_callbacks* cb, jm_string module,
        jm_log_level_enu_t log_level, jm_string message)
{
    char* expMsg = "TODO";
    if (!strncmp(expMsg, message, strlen(expMsg))) {
        g_logger_found_err_msg = 1;
    }

    jm_default_logger(cb, module, log_level, message);
}

static int test_modelstructure_valid(fmi3_import_t* xml)
{
    ASSERT_MSG(xml != NULL, "Failed to parse XML");

    ASSERT_MSG(fmi3_import_get_last_error(xml) != NULL, "Found unexpected error while parsing XML");

    return TEST_OK;
}

int main(int argc, char **argv)
{
    int ret = 0;

    printf("Running fmi3_import_model_structure_test\n");

    /* test valid */
    ret |= test_parse_xml(0, FMI3_TEST_XML_DIR, "/model_structure/valid", NULL, NULL);

    /* test invalid */
    ret |= test_parse_xml(1, FMI3_TEST_XML_DIR, "/model_structure/invalid/derivative_reference",
            logger_invalid_state_derivative_reference, NULL);
    ret |= test_parse_xml(1, FMI3_TEST_XML_DIR, "/model_structure/invalid/incorrect_order",
            logger_incorrect_order, NULL);
    ret |= test_parse_xml(1, FMI3_TEST_XML_DIR, "/model_structure/invalid/fmi2_style_list",
            logger_fmi2_style_list, NULL);

    // TODO: This test should fail
    // ret |= test_parse_xml(1, FMI3_TEST_XML_DIR, "/model_structure/invalid/output_wrong_causality",
    //         logger_output_wrong_causality, NULL);

    return ret;
}
