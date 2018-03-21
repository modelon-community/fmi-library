#include <stdlib.h>
#include <fmilib.h>
#include <stdio.h>
#include "config_test.h"

static const int NO_LOG_EXPECTED_MSG = 0;
static const int DO_LOG_EXPECTED_MSG = 1;

static int did_log_expected_msg;
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
        did_log_expected_msg = 1;
    }
}

void destroy_parse_test_callbacks(jm_callbacks *cb) {
    fmi_import_free_context((fmi_import_context_t *)cb->context);
    free(cb);
}

jm_callbacks* create_parse_test_callbacks(void) {
    jm_callbacks *cb;

    cb = (jm_callbacks *) malloc(sizeof(jm_callbacks));
    cb->malloc = malloc;
    cb->calloc = calloc;
    cb->realloc = realloc;
    cb->free = free;
    cb->logger = importlogger;
    cb->log_level = jm_log_level_all;
    cb->context = fmi_import_allocate_context(cb);

    return cb;
}

fmi1_import_t* test_parser(jm_callbacks *cb, char *xml_dir, int should_log_expected_msg, int configuration)
{
    fmi1_import_t *fmu;
    char *full_path;

    if (configuration != 0) {
        fmi_import_set_configuration((fmi_import_context_t *)cb->context, configuration);
    }

    did_log_expected_msg = 0;
    full_path = concat(name_check_test_directory, xml_dir);
    fmu = fmi1_import_parse_xml((fmi_import_context_t *)cb->context, full_path);

    free(full_path);
    if (fmu == NULL) {
        exit(CTEST_RETURN_FAIL);
    }
    if (should_log_expected_msg != did_log_expected_msg) {
        exit(CTEST_RETURN_FAIL);
    }

    return fmu;
}

void test_parser_with_cleanup(char *xml_dir, int should_log_expected_msg, int configuration) {
    jm_callbacks* cb = create_parse_test_callbacks();
    
    fmi1_import_t* fmu = test_parser(cb, xml_dir, should_log_expected_msg, configuration);
    fmi1_import_free(fmu);
    destroy_parse_test_callbacks(cb);
}

void fail_name_check(char *xml_dir)
{
    test_parser_with_cleanup(xml_dir, DO_LOG_EXPECTED_MSG, FMI_IMPORT_NAME_CHECK);
    test_parser_with_cleanup(xml_dir, NO_LOG_EXPECTED_MSG, 0);
}

void pass_name_check(char *xml_dir)
{
    test_parser_with_cleanup(xml_dir, NO_LOG_EXPECTED_MSG, FMI_IMPORT_NAME_CHECK);
}

void parser_log_expected_message(char *xml_dir)
{
    test_parser_with_cleanup(xml_dir, DO_LOG_EXPECTED_MSG, 0);
}

void parser_no_log_expected_message(char *xml_dir)
{
    test_parser_with_cleanup(xml_dir, NO_LOG_EXPECTED_MSG, 0);
}

typedef int (*fmu_test_f)(fmi1_import_t* fmu);

void test_parsing_and_fmu(char *xml_dir, fmu_test_f fmu_test, int should_log_expected_msg)
{
    jm_callbacks* cb = create_parse_test_callbacks();
    
    fmi1_import_t* fmu = test_parser(cb, xml_dir, should_log_expected_msg, 0);
    if (!fmu_test(fmu)) {
        exit(CTEST_RETURN_FAIL);
    }
    fmi1_import_free(fmu);

    destroy_parse_test_callbacks(cb);
}

void test_parsing_fail_and_fmu(char *xml_dir, fmu_test_f fmu_test)
{
    test_parsing_and_fmu(xml_dir, fmu_test, DO_LOG_EXPECTED_MSG);
}

void test_parsing_pass_and_fmu(char *xml_dir, fmu_test_f fmu_test)
{
    test_parsing_and_fmu(xml_dir, fmu_test, NO_LOG_EXPECTED_MSG);
}

void test_variable_naming_conventions(void) {
    /* Test scalar variable names
     *
     * Every test below has a corresponding modelDescription in
     * Test/FMI1/parser_test_xmls/naming_conventions_xmls/
     * What is passed to these macros are names of directories containing
     * modelDescriptions.
     */

    /* Test examples mentioned */
    fail_name_check("naming_conventions_xmls/examples/foo");
    fail_name_check("naming_conventions_xmls/examples/derderx");
    pass_name_check("naming_conventions_xmls/examples/derx2");

    /* FMI 1.0 standard examples from the documentaiton */
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
    pass_name_check("naming_conventions_xmls/q-name/chinese"); /* this should fail in FMI 2.0 */

    /* der() tests */
    pass_name_check("naming_conventions_xmls/der/dera32"); /* this should fail in FMI 2.0 */
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
    pass_name_check("naming_conventions_xmls/array/a12345678space"); /* this should fail in FMI 2.0 */
    pass_name_check("naming_conventions_xmls/array/a1.a3");
    pass_name_check("naming_conventions_xmls/array/a.a123");
    fail_name_check("naming_conventions_xmls/array/aspace1");
    fail_name_check("naming_conventions_xmls/array/a1space");
    pass_name_check("naming_conventions_xmls/array/a1space1"); /* this should fail in FMI 2.0 */
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

int should_have_no_vars(fmi1_import_t* fmu) {
    fmi1_import_variable_list_t* vars;

    vars = fmi1_import_get_variable_list(fmu);
    return fmi1_import_get_variable_list_size(vars) == 0;
}

int should_have_1_no_alias_var(fmi1_import_t* fmu) {
    fmi1_import_variable_list_t* vars;
    fmi1_import_variable_t* var;

    vars = fmi1_import_get_variable_list(fmu);
    if (fmi1_import_get_variable_list_size(vars) != 1) {
        return 0;
    }

    var = fmi1_import_get_variable(vars, 0);
    return (fmi1_import_get_variable_alias_kind(var) == fmi1_variable_is_not_alias);
}

int should_have_size_2_alias_group(fmi1_import_t* fmu) {
    fmi1_import_variable_list_t* vars;
    fmi1_import_variable_t* var;

    vars = fmi1_import_get_variable_list(fmu);

    if (fmi1_import_get_variable_list_size(vars) != 2) {
        return 0;
    }

    var = fmi1_import_get_variable(vars, 0);
    if (fmi1_import_get_variable_alias_kind(var) != fmi1_variable_is_not_alias) {
        return 0;
    }

    var = fmi1_import_get_variable(vars, 1);
    return (fmi1_import_get_variable_alias_kind(var) == fmi1_variable_is_alias);
}

int should_have_size_2_no_alis(fmi1_import_t* fmu) {
    fmi1_import_variable_list_t* vars;
    fmi1_import_variable_t* var;

    vars = fmi1_import_get_variable_list(fmu);

    if (fmi1_import_get_variable_list_size(vars) != 2) {
        return 0;
    }

    var = fmi1_import_get_variable(vars, 0);
    if (fmi1_import_get_variable_alias_kind(var) != fmi1_variable_is_not_alias) {
        return 0;
    }

    var = fmi1_import_get_variable(vars, 1);
    return (fmi1_import_get_variable_alias_kind(var) == fmi1_variable_is_not_alias);
}

int should_have_size_3_alias_group(fmi1_import_t* fmu) {
    fmi1_import_variable_list_t* vars;
    fmi1_import_variable_t* var;

    vars = fmi1_import_get_variable_list(fmu);

    if (fmi1_import_get_variable_list_size(vars) != 3) {
        return 0;
    }

    var = fmi1_import_get_variable(vars, 0);
    if (fmi1_import_get_variable_alias_kind(var) != fmi1_variable_is_not_alias) {
        return 0;
    }

    var = fmi1_import_get_variable(vars, 1);
    if (fmi1_import_get_variable_alias_kind(var) != fmi1_variable_is_alias) {
        return 0;
    }

    var = fmi1_import_get_variable(vars, 2);
    return (fmi1_import_get_variable_alias_kind(var) == fmi1_variable_is_negated_alias);
}

void test_alias_set_error_handling(void) {
    /* Test that we log errors for incorrect alias set and 
     * that incorrect alias variables are removed or that
     * variable that should be alias variables becomes alias */

    expected_message = "Alias set with vr=0 (type=Real) do not have a 'noAlias' variable.";
    parser_log_expected_message("alias_validation/all_alias");
    expected_message = "Removing incorrect alias variable 'v2'";
    test_parsing_fail_and_fmu("alias_validation/all_alias", should_have_no_vars);
    
    expected_message = "Alias set with vr=0 (type=Real) do not have a 'noAlias' variable.";
    parser_log_expected_message("alias_validation/all_alias_mixed");
    expected_message = "Removing incorrect alias variable 'v2'";
    test_parsing_fail_and_fmu("alias_validation/all_alias_mixed", should_have_no_vars);

    expected_message = "Alias set with vr=0 (type=Real) do not have a 'noAlias' variable.";
    parser_log_expected_message("alias_validation/all_negated_alias");
    expected_message = "Removing incorrect alias variable 'v2'";
    test_parsing_fail_and_fmu("alias_validation/all_negated_alias", should_have_no_vars);

    expected_message = "Variables v1 and v2 reference the same vr 0. Marking 'v2' as alias.";
    test_parsing_fail_and_fmu("alias_validation/all_no_alias", should_have_size_2_alias_group);

    expected_message = "Variables v1 and v2 reference the same vr 0. Marking 'v2' as alias.";
    test_parsing_pass_and_fmu("alias_validation/small_valid_alias_set", should_have_size_2_alias_group);

    expected_message = "Variables v1 and v2 reference the same vr 0. Marking 'v2' as alias.";
    test_parsing_pass_and_fmu("alias_validation/medium_valid_alias_set", should_have_size_3_alias_group);

    expected_message = "Alias set with vr=0 (type=Real) do not have a 'noAlias' variable.";
    parser_log_expected_message("alias_validation/all_alias_two_sets");
    expected_message = "Alias set with vr=1 (type=Real) do not have a 'noAlias' variable.";
    test_parsing_fail_and_fmu("alias_validation/all_alias_two_sets", should_have_no_vars);

    expected_message = "Variables v1 and v2 reference the same vr 0. Marking 'v2' as alias.";
    test_parsing_pass_and_fmu("alias_validation/all_no_alias_two_sets", should_have_size_2_no_alis);

    expected_message = "Inconsistent start values in alias set";
    parser_no_log_expected_message("alias_validation/consistent_real_start_values");

    expected_message = "Inconsistent start values in alias set";
    parser_no_log_expected_message("alias_validation/consistent_int_start_values");

    expected_message = "Inconsistent start values in alias set";
    parser_no_log_expected_message("alias_validation/consistent_enum_start_values");

    expected_message = "Inconsistent start values in alias set";
    parser_no_log_expected_message("alias_validation/consistent_bool_start_values");

    expected_message = "Inconsistent start values in alias set";
    parser_no_log_expected_message("alias_validation/consistent_str_start_values");

    expected_message = "Inconsistent start values in alias set, "
        "start value '1.0"; /* Cannot check more of message due to potential roundings */
    test_parsing_fail_and_fmu("alias_validation/inconsistent_real_start_values",
        should_have_1_no_alias_var);

    expected_message = "Inconsistent start values in alias set, "
        "start value '1' of 'v1' does not match "
        "start value '3' of 'v2'.";
    test_parsing_fail_and_fmu("alias_validation/inconsistent_int_start_values",
        should_have_1_no_alias_var);

    expected_message = "Inconsistent start values in alias set, "
        "start value '1' of 'v1' does not match "
        "start value '2' of 'v2'.";
    test_parsing_fail_and_fmu("alias_validation/inconsistent_enum_start_values",
        should_have_1_no_alias_var);

    expected_message = "Inconsistent start values in alias set, "
        "start value 'true' of 'v1' does not match "
        "start value 'false' of 'v2'.";
    test_parsing_fail_and_fmu("alias_validation/inconsistent_bool_start_values",
        should_have_1_no_alias_var);

    expected_message = "Inconsistent start values in alias set, "
        "start value 'a' of 'v1' does not match "
        "start value 'b' of 'v2'.";
    test_parsing_fail_and_fmu("alias_validation/inconsistent_str_start_values",
        should_have_1_no_alias_var);

    expected_message = "Inconsistent start values in alias set, "
        "start value '1.0"; /* Cannot check more of message due to potential roundings */
    test_parsing_fail_and_fmu("alias_validation/inconsistent_neg_real_start_values",
        should_have_1_no_alias_var);

    expected_message = "Inconsistent start values in alias set, "
        "start value '1' of 'v1' does not match "
        "start value '1' of 'v2'(negated alias).";
    test_parsing_fail_and_fmu("alias_validation/inconsistent_neg_int_start_values",
        should_have_1_no_alias_var);

    expected_message = "Inconsistent start values in alias set, "
        "start value '1' of 'v1' does not match "
        "start value '1' of 'v2'(negated alias).";
    test_parsing_fail_and_fmu("alias_validation/inconsistent_neg_enum_start_values",
        should_have_1_no_alias_var);

    expected_message = "Inconsistent start values in alias set, "
        "start value 'true' of 'v1' does not match "
        "start value 'true' of 'v2'(negated alias).";
    test_parsing_fail_and_fmu("alias_validation/inconsistent_neg_bool_start_values",
        should_have_1_no_alias_var);

    expected_message = "Inconsistent start values in alias set, "
        "start value 'a' of 'v1' does not match "
        "start value 'a' of 'v2'(negated alias).";
    test_parsing_fail_and_fmu("alias_validation/inconsistent_neg_str_start_values",
        should_have_1_no_alias_var);

    expected_message = "Inconsistent start values in alias set";
    parser_no_log_expected_message("alias_validation/consistent_real_start_values2");

    expected_message = "Inconsistent start values in alias set, "
        "start value '1' of 'v2' does not match "
        "start value '3' of 'v3'.";
    parser_log_expected_message("alias_validation/inconsistent_int_start_values2");

    expected_message = "Inconsistent start values in alias set";
    test_parsing_pass_and_fmu("alias_validation/consistent_real_zero_start_values", should_have_size_2_alias_group);
}

void test_deprecation_errors(void) {
    /* Test that we give errors for deprecated behviour
     *
     * Every test below has a corresponding modelDescription in
     * Test/FMI1/parser_test_xmls/deprecated/
     * What is passed to these macros are names of directories containing
     * modelDescriptions.
     */

    expected_message = "Found capability flag canSignalEvents which have been "
        "deprecated as it fills no function";
    parser_log_expected_message("deprecated/canSignalEvents");
}

void test_variable_no_type(void)
{
    expected_message = "No variable type element for variable v. Assuming Real.";
    parser_log_expected_message("incorrect/variable_no_type");
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
    test_deprecation_errors();
    test_alias_set_error_handling();
    test_variable_no_type();

    return 0;
}
