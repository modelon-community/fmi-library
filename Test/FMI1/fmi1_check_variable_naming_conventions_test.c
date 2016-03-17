#include <stdlib.h>
#include <fmilib.h>
#include <stdio.h>
#include "config_test.h"

static const int SHOULD_FAIL_NAME_CHECK = 0;
static const int SHOULD_PASS_NAME_CHECK = 1;

static int name_check_passed;
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
        name_check_passed = 0;
    }
}

void test_check_name(char *xml_dir, int name_check_should_pass, int configuration)
{
    jm_callbacks *callbacks;
    fmi_import_context_t *context;
    fmi1_import_t *fmu;
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

    name_check_passed = 1;
    full_path = concat(name_check_test_directory, xml_dir);
    fmu = fmi1_import_parse_xml(context, full_path);

    free(full_path);
    fmi_import_free_context(context);
    if (fmu == NULL) {
        exit(CTEST_RETURN_FAIL);
    }
    if (!name_check_should_pass &&  name_check_passed ||
            name_check_should_pass && !name_check_passed) {
        exit(CTEST_RETURN_FAIL);
    }
}

void fail_name_check(char *xml_dir)
{
    test_check_name(xml_dir, SHOULD_FAIL_NAME_CHECK, FMI_IMPORT_NAME_CHECK);
    test_check_name(xml_dir, SHOULD_PASS_NAME_CHECK, 0);
}

void pass_name_check(char *xml_dir)
{
    test_check_name(xml_dir, SHOULD_PASS_NAME_CHECK, FMI_IMPORT_NAME_CHECK);
}

int main(int argc, char *argv[])
{
    if (argc == 2) {
        name_check_test_directory = argv[1];
    } else {
        printf("Usage: %s <path to folder naming_conventions_xmls>\n", argv[0]);
        exit(CTEST_RETURN_FAIL);
    }

    /* Test scalar variable names
     *
     * Every test below has a corresponding modelDescription in
     * Test/FMI1/naming_conventions_xmls/
     * What is passed to these macros are names of directories containing
     * modelDescriptions.
     */

    /* Test examples mentioned */
    fail_name_check("examples/foo");
    fail_name_check("examples/derderx");
    pass_name_check("examples/derx2");

    /* FMI 1.0 standard examples from the documentaiton */
    pass_name_check("standard/vehicle.engine.speed");
    pass_name_check("standard/resistor12.u");
    pass_name_check("standard/v_min");
    pass_name_check("standard/robot.axis.motor234");
    pass_name_check("standard/derpipe34.t142");

    /* Implementation test examples */
    fail_name_check("implementation/empty");
    fail_name_check("implementation/-0");
    pass_name_check("implementation/_0");
    pass_name_check("implementation/a0");
    fail_name_check("implementation/0a");
    fail_name_check("implementation/0");

    /* q-name tests */
    fail_name_check("q-name/empty");
    pass_name_check("q-name/space");
    fail_name_check("q-name/backslash");
    pass_name_check("q-name/q-char");
    pass_name_check("q-name/escape");
    pass_name_check("q-name/chinese"); /* this should fail in FMI 2.0 */

    /* der() tests */
    pass_name_check("der/dera32"); /* this should fail in FMI 2.0 */
    fail_name_check("der/dera12");
    pass_name_check("der/dera32-no-space");
    pass_name_check("der/dera");
    fail_name_check("der/dera-no-closing-parenthesis");
    pass_name_check("der/somederthing");
    pass_name_check("der/der0");
    fail_name_check("der/der2");
    fail_name_check("der/adera");

    /* array and hierarchy tests */
    pass_name_check("array/n0");
    fail_name_check("array/a1comma");
    pass_name_check("array/a12345678");
    pass_name_check("array/a12345678space"); /* this should fail in FMI 2.0 */
    pass_name_check("array/a1.a3");
    pass_name_check("array/a.a123");
    fail_name_check("array/aspace1");
    fail_name_check("array/a1space");
    pass_name_check("array/a1space1"); /* this should fail in FMI 2.0 */
    fail_name_check("array/aspacebracket1");
    fail_name_check("array/a-1");
    pass_name_check("array/a1");
    pass_name_check("array/a.a");
    pass_name_check("array/a");

    /* list of variables */
    fail_name_check("list/aemptyc");
    expected_message = "Two variables with the same name";
    pass_name_check("list/cba");
    fail_name_check("list/acad");

    return 0;
}



