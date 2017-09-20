#include <stdlib.h>
#include <fmilib.h>
#include <stdio.h>
#include "config_test.h"

static const int SHOULD_NOT_LOG_EXPECTED_MSG = 0;
static const int SHOULD_LOG_EXPECTED_MSG = 1;

static int did_not_log_expected_msg;
static char *expected_message = "Invalid structured ScalarVariable name";
static char *name_check_test_directory;

char *concat(char *s1, char *s2)
{
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    /* +1 for the zero-terminator */
    char *result = (char *) malloc((len1 + len2 + 1) * sizeof(char));
    if (result == NULL) {
        exit(CTEST_RETURN_FAIL);
    }
    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1); /* +1 to copy the null-terminator */
    return result;
}

void importlogger(jm_callbacks* c, jm_string module,
        jm_log_level_enu_t log_level, jm_string message)
{
    printf("module = %s, log level = %d: %s\n", module, log_level, message);
    if (!strncmp(expected_message, message, strlen(expected_message))) {
        did_not_log_expected_msg = 0;
    }
}

void test_parser(char *xml_dir, int should_not_log_expected_msg, int configuration)
{
    jm_callbacks *callbacks;
    fmi_import_context_t *context;
    fmi2_import_t *fmu;
    char *full_path;

    callbacks = (jm_callbacks *) malloc(sizeof(jm_callbacks));
    callbacks->malloc = malloc;
    callbacks->calloc = calloc;
    callbacks->realloc = realloc;
    callbacks->free = free;
    callbacks->logger = importlogger;
    callbacks->log_level = jm_log_level_all;
    callbacks->context = 0;
    context = fmi_import_allocate_context(callbacks);
    if (configuration != 0) {
        fmi_import_set_configuration(context, configuration);
    }

    did_not_log_expected_msg = 1;
    full_path = concat(name_check_test_directory, xml_dir);
    fmu = fmi2_import_parse_xml(context, full_path, NULL);

    free(full_path);
    fmi_import_free_context(context);
    if (fmu == NULL) {
        exit(CTEST_RETURN_FAIL);
    }
    if (!should_not_log_expected_msg && did_not_log_expected_msg ||
            did_not_log_expected_msg && !should_not_log_expected_msg) {
        exit(CTEST_RETURN_FAIL);
    }
}

void fail_name_check(char *xml_dir)
{
    test_parser(xml_dir, SHOULD_NOT_LOG_EXPECTED_MSG, FMI_IMPORT_NAME_CHECK);
    test_parser(xml_dir, SHOULD_LOG_EXPECTED_MSG, 0);
}

void pass_name_check(char *xml_dir)
{
    test_parser(xml_dir, SHOULD_LOG_EXPECTED_MSG, FMI_IMPORT_NAME_CHECK);
}

void parser_log_expected_message(char *xml_dir)
{
    test_parser(xml_dir, SHOULD_NOT_LOG_EXPECTED_MSG, 0);
}

void test_variable_naming_conventions(void)
{
    /* Test scalar variable names
     *
     * Every test below has a corresponding modelDescription in
     * Test/FMI2/naming_conventions_xmls/
     * What is passed to these macros are names of directories containing
     * modelDescriptions.
     */

    /* Test examples mentioned */
    fail_name_check("naming_conventions_xmls/examples/foo");
    fail_name_check("naming_conventions_xmls/examples/derderx");
    pass_name_check("naming_conventions_xmls/examples/derx2");

    /* FMI 2.0 standard examples from the documentaiton */
    pass_name_check("naming_conventions_xmls/standard/vehicle.engine.speed");
    pass_name_check("naming_conventions_xmls/standard/resistor12.u");
    pass_name_check("naming_conventions_xmls/standard/v_min");
    pass_name_check("naming_conventions_xmls/standard/robot.axis.motor234");
    pass_name_check("naming_conventions_xmls/standard/derpipe34.T142");

    /* Implementation test examples */
    fail_name_check("naming_conventions_xmls/implementation/empty");
    fail_name_check("naming_conventions_xmls/implementation/-0");
    pass_name_check("naming_conventions_xmls/implementation/_0");
    pass_name_check("naming_conventions_xmls/implementation/a0");
    fail_name_check("naming_conventions_xmls/implementation/0a");
    fail_name_check("naming_conventions_xmls/implementation/0");

    /* q-name tests */
    fail_name_check("naming_conventions_xmls/q-name/empty");
    pass_name_check("naming_conventions_xmls/q-name/space");
    fail_name_check("naming_conventions_xmls/q-name/backslash");
    pass_name_check("naming_conventions_xmls/q-name/q-char");
    pass_name_check("naming_conventions_xmls/q-name/escape");
    fail_name_check("naming_conventions_xmls/q-name/chinese"); /* this should pass in FMI 1.0 */

    /* der() tests */
    fail_name_check("naming_conventions_xmls/der/dera32"); /* this should pass in FMI 1.0 */
    fail_name_check("naming_conventions_xmls/der/dera12");
    pass_name_check("naming_conventions_xmls/der/dera32-no-space");
    pass_name_check("naming_conventions_xmls/der/dera");
    fail_name_check("naming_conventions_xmls/der/dera-no-closing-parenthesis");
    pass_name_check("naming_conventions_xmls/der/somederthing");
    pass_name_check("naming_conventions_xmls/der/der0");
    fail_name_check("naming_conventions_xmls/der/der2");
    fail_name_check("naming_conventions_xmls/der/adera");

    /* array and hierarchy tests */
    pass_name_check("naming_conventions_xmls/array/n0");
    fail_name_check("naming_conventions_xmls/array/a1comma");
    pass_name_check("naming_conventions_xmls/array/a12345678");
    fail_name_check("naming_conventions_xmls/array/a12345678space"); /* this should pass in FMI 1.0 */
    pass_name_check("naming_conventions_xmls/array/a1.a3");
    pass_name_check("naming_conventions_xmls/array/a.a123");
    fail_name_check("naming_conventions_xmls/array/aspace1");
    fail_name_check("naming_conventions_xmls/array/a1space");
    fail_name_check("naming_conventions_xmls/array/a1space1"); /* this should pass in FMI 1.0 */
    fail_name_check("naming_conventions_xmls/array/aspacebracket1");
    fail_name_check("naming_conventions_xmls/array/a-1");
    pass_name_check("naming_conventions_xmls/array/a1");
    pass_name_check("naming_conventions_xmls/array/a.a");
    pass_name_check("naming_conventions_xmls/array/a");

    /* list of variables */
    fail_name_check("naming_conventions_xmls/list/aemptyc");
    expected_message = "Two variables with the same name";
    pass_name_check("naming_conventions_xmls/list/cba");
    fail_name_check("naming_conventions_xmls/list/acad");

    /* flat hierarchy test */
    fail_name_check("naming_conventions_xmls/flat/acad");
    pass_name_check("naming_conventions_xmls/flat/q-char-nonescaped");
}

int main(int argc, char *argv[])
{
    if (argc == 2) {
        name_check_test_directory = argv[1];
    } else {
        printf("Usage: %s <path to folder naming_conventions_xmls>\n", argv[0]);
        exit(CTEST_RETURN_FAIL);
    }

    test_variable_naming_conventions();

    return 0;
}
