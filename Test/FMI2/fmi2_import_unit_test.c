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

static void log_default(jm_string module, jm_log_level_enu_t log_level, jm_string message)
{
    printf("module = %s, log level = %d: %s\n", module, log_level, message);
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

    log_default(module, log_level, message);
}

static void logger_disp_unit_without_unit(jm_callbacks* c, jm_string module,
    jm_log_level_enu_t log_level, jm_string message)
{
    char* expMsg = "Type or variable definition contained attribute for display unit 'degC', but not for unit";
    if (!strncmp(expMsg, message, strlen(expMsg))) {
        g_logger_found_err_msg = 1;
    }

    log_default(module, log_level, message);
}

static void logger_nonexisting_unit_from_type(jm_callbacks* c, jm_string module,
    jm_log_level_enu_t log_level, jm_string message)
{
    char* expMsg = "Type or variable referenced unit 'C' that does not exist in unit definitions";
    if (!strncmp(expMsg, message, strlen(expMsg))) {
        g_logger_found_err_msg = 1;
    }

    log_default(module, log_level, message);
}

static void logger_nonexisting_unit_from_variable(jm_callbacks* c, jm_string module,
    jm_log_level_enu_t log_level, jm_string message)
{
    char* expMsg = "Type or variable referenced unit 'F' that does not exist in unit definitions";
    if (!strncmp(expMsg, message, strlen(expMsg))) {
        g_logger_found_err_msg = 1;
    }

    log_default(module, log_level, message);
}

/**
 * Tests that a unit referenced by a Type's attribute exists in UnitDefinitions.
 *
 * @xmlTestRootDir: path to 'parser_test_xmls' directory
 * @xmlDirRel: Relative path to the XML directory from root dir for parser test
 *   XMLs. Must start with leading '/' character.
 * @xmlOut: return arg that will point to the parsed XML struct. Provide NULL
 *   if not desired.
 */
static int test_parse_xml(int expectFailure, char* xmlTestRootDir, char* xmlDirRel, jm_logger_f logger,
        fmi2_import_t** xmlOut)
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
        fmi2_import_free(xml);
    }
clean1:
    free(xmlDir);

    return ret;
}

/* Returns NULL if unit not found. */
static fmi2_import_unit_t* get_unit_by_name(fmi2_import_unit_definitions_t* uds, const char* name) {
    fmi2_import_unit_t* unit = NULL;
    size_t k;

    for (k = 0; k < fmi2_import_get_unit_definitions_number(uds); k++) {
        unit = fmi2_import_get_unit(uds, k);
        if (!strcmp(fmi2_import_get_unit_name(unit), name)) {
            return unit;
        }
    }

    return NULL;
}

/* Returns NULL if type not found. */
static fmi2_import_real_typedef_t* get_type_by_name(fmi2_import_type_definitions_t* tds, const char* name) {
    size_t k;
    fmi2_import_variable_typedef_t* type = NULL;
    fmi2_import_real_typedef_t* typeReal = NULL;

    for (k = 0; k < fmi2_import_get_type_definition_number(tds); k++) {
        type = fmi2_import_get_typedef(tds, k);
        if (!strcmp(fmi2_import_get_type_name(type), name)) {
            return fmi2_import_get_type_as_real(type);
        }
    }

    return NULL;
}

/* Set unitName/dispUnitName to NULL if attributes are expected to not exist */
static int verify_unit_real_type(fmi2_import_type_definitions_t* tds, const char* typeName,
        const char* unitName, const char* dispUnitName) {
    fmi2_import_real_typedef_t* type     = NULL;
    fmi2_import_display_unit_t* dispUnit = NULL;
    fmi2_import_unit_t* unit             = NULL;

    type = get_type_by_name(tds, typeName);
    ASSERT_MSG_CTEST(type != NULL, "Type wasn't found");

    unit = fmi2_import_get_real_type_unit(type);
    if (unitName) {
        ASSERT_MSG_CTEST(unit != NULL, "Unit was NULL");
        ASSERT_MSG_CTEST(!strcmp(fmi2_import_get_unit_name(unit), unitName),
                "Incorrect name for unit via variable");
    } else {
        ASSERT_MSG_CTEST(unit == NULL, "Unit was not NULL");
    }

    dispUnit = fmi2_import_get_type_display_unit(type);
    /*dispUnit = fmi2_import_get_real_type_display_unit(type);*/
    if (dispUnitName) {
        ASSERT_MSG_CTEST(dispUnit != NULL, "Display unit was NULL");
        ASSERT_MSG_CTEST(!strcmp(fmi2_import_get_display_unit_name(dispUnit), dispUnitName),
                "Incorrect name for unit");
    } else {
        ASSERT_MSG_CTEST(dispUnit == NULL, "Display unit was not NULL");
    }

    return CTEST_RETURN_SUCCESS;
}

/* Set names to NULL if attributes are expected to not exist */
static int verify_unit_real_variable(fmi2_import_t* xml, fmi2_value_reference_t vr,
        const char* unitName, const char* dispUnitName) {
    fmi2_import_real_variable_t* var     = NULL;
    fmi2_import_display_unit_t* dispUnit = NULL;
    fmi2_import_unit_t* unit             = NULL;

    var = fmi2_import_get_variable_as_real(fmi2_import_get_variable_by_vr(xml, fmi2_base_type_real, vr));
    ASSERT_MSG_CTEST(var != NULL, "Variable was NULL");

    unit = fmi2_import_get_real_variable_unit(var);
    if (unitName) {
        ASSERT_MSG_CTEST(unit != NULL, "Unit was NULL");
        ASSERT_MSG_CTEST(!strcmp(fmi2_import_get_unit_name(unit), unitName),
                "Incorrect name for unit via variable");
    } else {
        ASSERT_MSG_CTEST(unit == NULL, "Unit was not NULL");
    }

    dispUnit = fmi2_import_get_real_variable_display_unit(var);
    if (dispUnitName) {
        ASSERT_MSG_CTEST(dispUnit != NULL, "Display unit was NULL");
        ASSERT_MSG_CTEST(!strcmp(fmi2_import_get_display_unit_name(dispUnit), dispUnitName),
                "Incorrect name for unit");
    } else {
        ASSERT_MSG_CTEST(dispUnit == NULL, "Display unit was not NULL");
    }

    return CTEST_RETURN_SUCCESS;
}

static int test_get_units(char* xmlTestRootDir, char* xmlDirRel) {
    fmi2_import_t* xml = NULL;
    fmi2_import_type_definitions_t* tds;
    int ret = CTEST_RETURN_SUCCESS;

    ret |= test_parse_xml(0, xmlTestRootDir, xmlDirRel, logger_verify_valid, &xml);
    if (ret) return ret;

    {
        /* Check the unit definition - note that functions that allow us to
        retrieve an item based on its index is not based on the index in the
        XML, but in the internal list structure (which doesn't have to match
        the XML), which makes it hard to test */

        fmi2_import_unit_definitions_t* uds  = NULL;
        fmi2_import_unit_t* unit             = NULL;
        fmi2_import_display_unit_t* dispUnit = NULL;

        uds = fmi2_import_get_unit_definitions(xml);
        ASSERT_MSG_CTEST(uds != NULL, "UnitDefinitions was NULL");
        ASSERT_MSG_CTEST(fmi2_import_get_unit_definitions_number(uds) == 2,
                "Incorrect number of unit definitions found");

        /* Only checking "K" for now... */
        unit = get_unit_by_name(uds, "K");
        ASSERT_MSG_CTEST(unit != NULL, "Unit was NULL");
        ASSERT_MSG_CTEST(!strcmp(fmi2_import_get_unit_name(unit), "K"),
                "Incorrect name for unit");
        ASSERT_MSG_CTEST(fmi2_import_get_unit_display_unit_number(unit) == 2,
                "Incorrect number of display units");
    }

    /* Check displayUnit and unit through type/variable references */

    /* type */
    tds = fmi2_import_get_type_definitions(xml);
    ASSERT_MSG_CTEST(tds != NULL, "Type definitions was NULL");

    ret |= verify_unit_real_type(tds, "type1", "K",  "degC");
    ret |= verify_unit_real_type(tds, "type2", "K",  "degF");
    ret |= verify_unit_real_type(tds, "type3", "K",  NULL);
    ret |= verify_unit_real_type(tds, "type4", "K",  NULL);
    ret |= verify_unit_real_type(tds, "type5", NULL, NULL);

    /* variable */
    ret |= verify_unit_real_variable(xml, 1, "K",  "degC");
    ret |= verify_unit_real_variable(xml, 2, "K",  "degF");
    ret |= verify_unit_real_variable(xml, 3, "K",  NULL);
    ret |= verify_unit_real_variable(xml, 4, "K",  NULL);
    ret |= verify_unit_real_variable(xml, 5, NULL, NULL);
    /* inherited */
    ret |= verify_unit_real_variable(xml, 6,  "K",  "degC");
    ret |= verify_unit_real_variable(xml, 7,  "K",  "degF");
    ret |= verify_unit_real_variable(xml, 8,  "K",  NULL);
    ret |= verify_unit_real_variable(xml, 9,  "K",  NULL);
    ret |= verify_unit_real_variable(xml, 10, NULL, NULL);
    /* override */
    ret |= verify_unit_real_variable(xml, 11, "K", "degC");
    ret |= verify_unit_real_variable(xml, 12, "K", "degF");
    ret |= verify_unit_real_variable(xml, 13, "K", NULL);
    ret |= verify_unit_real_variable(xml, 14, "K", NULL);
    ret |= verify_unit_real_variable(xml, 15, "C", "degK");

    fmi2_import_free(xml);

    return CTEST_RETURN_SUCCESS;
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

    /* Test that we can retrieve units from valid XML */
    ret |= test_get_units(argv[1], "/unit/valid");

    /* invalid */
    ret |= test_parse_xml(1, argv[1], "/unit/nonexisting_unit_type", logger_nonexisting_unit_from_type, NULL);
    ret |= test_parse_xml(1, argv[1], "/unit/nonexisting_unit_variable", logger_nonexisting_unit_from_variable, NULL);
    ret |= test_parse_xml(1, argv[1], "/unit/disp_unit_without_unit", logger_disp_unit_without_unit, NULL);

    return ret ? CTEST_RETURN_FAIL : CTEST_RETURN_SUCCESS;
}
