#include <stdio.h>
#include <float.h>
#include <stdlib.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmil_test.h"

/* Global set to 1 if logger cb found expected error message - must be reset
 * between tests. */
int g_logger_found_err_msg = 0;

static fmi2_import_t* parse_xml(const char *model_desc_path, jm_logger_f logger)
{
    jm_callbacks *cb = jm_get_default_callbacks();
    fmi_import_context_t *ctx = fmi_import_allocate_context(cb);
    fmi2_import_t *xml;

    g_logger_found_err_msg = 0;

    if (logger) {
        cb->logger = logger;
    }

    if (ctx == NULL) {
        return NULL;
    }

    xml = fmi2_import_parse_xml(ctx, model_desc_path, NULL);

    fmi_import_free_context(ctx);
    return xml;
}

/* For the valid XMLs we expect no warnings or errors */
static void logger_verify_valid(jm_callbacks* c, jm_string module,
    jm_log_level_enu_t log_level, jm_string message)
{
    if (    log_level == jm_log_level_error   || 
            log_level == jm_log_level_warning ||
            log_level == jm_log_level_fatal) {
        g_logger_found_err_msg = 1;
    }
}

static void logger_disp_unit_without_unit(jm_callbacks* c, jm_string module,
    jm_log_level_enu_t log_level, jm_string message)
{
    char* expMsg = "Type or variable definition contained attribute for display unit 'degC', but not for unit";
    if (!strncmp(expMsg, message, strlen(expMsg))) {
        g_logger_found_err_msg = 1;
    }
}

static void logger_nonexisting_unit_from_type(jm_callbacks* c, jm_string module,
    jm_log_level_enu_t log_level, jm_string message)
{
    char* expMsg = "Type or variable referenced unit 'C' that does not exist in unit definitions";
    if (!strncmp(expMsg, message, strlen(expMsg))) {
        g_logger_found_err_msg = 1;
    }
}

static void logger_nonexisting_unit_from_variable(jm_callbacks* c, jm_string module,
    jm_log_level_enu_t log_level, jm_string message)
{
    char* expMsg = "Type or variable referenced unit 'F' that does not exist in unit definitions";
    if (!strncmp(expMsg, message, strlen(expMsg))) {
        g_logger_found_err_msg = 1;
    }
}

/**
 * Tests that a unit referenced by a Type's attribute exists in UnitDefinitions.
 *
 * @xmlTestRootDir: path to 'parser_test_xmls' directory
 * @xmlDirRel: Relative path to the XML directory from root dir for parser test
 *   XMLs. Must start with leading '/' character.
 */
static int test_parse_xml(int expectFailure, char* xmlTestRootDir, char* xmlDirRel, jm_logger_f logger)
{
    char* xmlDir; /* path to the XML's parent directory */
    fmi2_import_t* xml;
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

    if (    (expectFailure && !g_logger_found_err_msg) ||
            (!expectFailure && g_logger_found_err_msg)) {
        ret = CTEST_RETURN_FAIL;
    }

clean2:
	fmi2_import_free(xml);
clean1:
    free(xmlDir);

    return ret;
}

int main(int argc, char **argv)
{
    int ret = 0;

    if (argc != 2) {
        printf("Usage: %s <ARGS>\n", argv[0]);
        printf("  arg1: path to directory that contains the test xmls: 'parser_test_xmls'");
        return CTEST_RETURN_FAIL;
    }

    printf("Running %s\n", argv[0]);

    /* valid */
    ret |= test_parse_xml(0, argv[1], "/unit/valid", logger_verify_valid);
    ret |= test_parse_xml(0, argv[1], "/unit/disp_unit_not_in_unitdefs", logger_verify_valid);

    /* invalid */
    ret |= test_parse_xml(1, argv[1], "/unit/nonexisting_unit_type", logger_nonexisting_unit_from_type);
    ret |= test_parse_xml(1, argv[1], "/unit/nonexisting_unit_variable", logger_nonexisting_unit_from_variable);
    ret |= test_parse_xml(1, argv[1], "/unit/disp_unit_without_unit", logger_disp_unit_without_unit);

    return ret ? CTEST_RETURN_FAIL : CTEST_RETURN_SUCCESS;
}
