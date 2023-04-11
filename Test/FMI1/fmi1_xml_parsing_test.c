#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <locale.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

static const int NO_LOG_EXPECTED_MSG = 0;
static const int DO_LOG_EXPECTED_MSG = 1;

static int did_log_expected_msg;
static char *g_name_check_test_directory;
static char *g_expected_message = "Invalid structured ScalarVariable name";

void importlogger(jm_callbacks* c, jm_string module,
        jm_log_level_enu_t log_level, jm_string message)
{
    printf("module = %s, log level = %d: %s\n", module, log_level, message);
    if (!strncmp(g_expected_message, message, strlen(g_expected_message))) {
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
    full_path = concat(g_name_check_test_directory, xml_dir);
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

typedef int (*fmu_test_f)(fmi1_import_variable_list_t* vars);

void test_parsing_and_fmu(char *xml_dir, fmu_test_f fmu_test, int should_log_expected_msg)
{
    jm_callbacks* cb = create_parse_test_callbacks();
    int res = 0;

    fmi1_import_t* fmu = test_parser(cb, xml_dir, should_log_expected_msg, 0);
    fmi1_import_variable_list_t* vars = fmi1_import_get_variable_list(fmu);

    if (!fmu_test(vars)) {
        res = 1;
    }

    /* cleanup */
    free(vars);
    fmi1_import_free(fmu);
    destroy_parse_test_callbacks(cb);

    if (res) {
        exit(CTEST_RETURN_FAIL);
    }
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
    g_expected_message = "Two variables with the same name";
    pass_name_check("naming_conventions_xmls/list/cba");
    fail_name_check("naming_conventions_xmls/list/acad");

    /* flat hierarchy test */
    fail_name_check("naming_conventions_xmls/flat/acad");
    pass_name_check("naming_conventions_xmls/flat/q-char-nonescaped");
}

int should_have_no_vars(fmi1_import_variable_list_t* vars) {
    return fmi1_import_get_variable_list_size(vars) == 0;
}

int should_have_1_no_alias_var(fmi1_import_variable_list_t* vars) {
    fmi1_import_variable_t* var;

    if (fmi1_import_get_variable_list_size(vars) != 1) {
        return 0;
    }

    var = fmi1_import_get_variable(vars, 0);
    return (fmi1_import_get_variable_alias_kind(var) == fmi1_variable_is_not_alias);
}

int should_have_size_2_alias_group(fmi1_import_variable_list_t* vars) {
    fmi1_import_variable_t* var;

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

int should_have_size_2_no_alis(fmi1_import_variable_list_t* vars) {
    fmi1_import_variable_t* var;

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

int should_have_size_3_alias_group(fmi1_import_variable_list_t* vars) {
    fmi1_import_variable_t* var;

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

    g_expected_message = "Alias set with vr=0 (type=Real) do not have a 'noAlias' variable.";
    parser_log_expected_message("alias_validation/all_alias");
    g_expected_message = "Removing incorrect alias variable 'v2'";
    test_parsing_fail_and_fmu("alias_validation/all_alias", should_have_no_vars);

    g_expected_message = "Alias set with vr=0 (type=Real) do not have a 'noAlias' variable.";
    parser_log_expected_message("alias_validation/all_alias_mixed");
    g_expected_message = "Removing incorrect alias variable 'v2'";
    test_parsing_fail_and_fmu("alias_validation/all_alias_mixed", should_have_no_vars);

    g_expected_message = "Alias set with vr=0 (type=Real) do not have a 'noAlias' variable.";
    parser_log_expected_message("alias_validation/all_negated_alias");
    g_expected_message = "Removing incorrect alias variable 'v2'";
    test_parsing_fail_and_fmu("alias_validation/all_negated_alias", should_have_no_vars);

    g_expected_message = "Variables v1 and v2 reference the same vr 0. Marking 'v2' as alias.";
    test_parsing_fail_and_fmu("alias_validation/all_no_alias", should_have_size_2_alias_group);

    g_expected_message = "Variables v1 and v2 reference the same vr 0. Marking 'v2' as alias.";
    test_parsing_pass_and_fmu("alias_validation/small_valid_alias_set", should_have_size_2_alias_group);

    g_expected_message = "Variables v1 and v2 reference the same vr 0. Marking 'v2' as alias.";
    test_parsing_pass_and_fmu("alias_validation/medium_valid_alias_set", should_have_size_3_alias_group);

    g_expected_message = "Alias set with vr=0 (type=Real) do not have a 'noAlias' variable.";
    parser_log_expected_message("alias_validation/all_alias_two_sets");
    g_expected_message = "Alias set with vr=1 (type=Real) do not have a 'noAlias' variable.";
    test_parsing_fail_and_fmu("alias_validation/all_alias_two_sets", should_have_no_vars);

    g_expected_message = "Variables v1 and v2 reference the same vr 0. Marking 'v2' as alias.";
    test_parsing_pass_and_fmu("alias_validation/all_no_alias_two_sets", should_have_size_2_no_alis);

    g_expected_message = "Inconsistent start values in alias set";
    parser_no_log_expected_message("alias_validation/consistent_real_start_values");

    g_expected_message = "Inconsistent start values in alias set";
    parser_no_log_expected_message("alias_validation/consistent_int_start_values");

    g_expected_message = "Inconsistent start values in alias set";
    parser_no_log_expected_message("alias_validation/consistent_enum_start_values");

    g_expected_message = "Inconsistent start values in alias set";
    parser_no_log_expected_message("alias_validation/consistent_bool_start_values");

    g_expected_message = "Inconsistent start values in alias set";
    parser_no_log_expected_message("alias_validation/consistent_str_start_values");

    g_expected_message = "Inconsistent start values in alias set, "
        "start value '1.0"; /* Cannot check more of message due to potential roundings */
    test_parsing_fail_and_fmu("alias_validation/inconsistent_real_start_values",
        should_have_1_no_alias_var);

    g_expected_message = "Inconsistent start values in alias set, "
        "start value '1' of 'v1' does not match "
        "start value '3' of 'v2'.";
    test_parsing_fail_and_fmu("alias_validation/inconsistent_int_start_values",
        should_have_1_no_alias_var);

    g_expected_message = "Inconsistent start values in alias set, "
        "start value '1' of 'v1' does not match "
        "start value '2' of 'v2'.";
    test_parsing_fail_and_fmu("alias_validation/inconsistent_enum_start_values",
        should_have_1_no_alias_var);

    g_expected_message = "Inconsistent start values in alias set, "
        "start value 'true' of 'v1' does not match "
        "start value 'false' of 'v2'.";
    test_parsing_fail_and_fmu("alias_validation/inconsistent_bool_start_values",
        should_have_1_no_alias_var);

    g_expected_message = "Inconsistent start values in alias set, "
        "start value 'a' of 'v1' does not match "
        "start value 'b' of 'v2'.";
    test_parsing_fail_and_fmu("alias_validation/inconsistent_str_start_values",
        should_have_1_no_alias_var);

    g_expected_message = "Inconsistent start values in alias set, "
        "start value '1.0"; /* Cannot check more of message due to potential roundings */
    test_parsing_fail_and_fmu("alias_validation/inconsistent_neg_real_start_values",
        should_have_1_no_alias_var);

    g_expected_message = "Inconsistent start values in alias set, "
        "start value '1' of 'v1' does not match "
        "start value '1' of 'v2'(negated alias).";
    test_parsing_fail_and_fmu("alias_validation/inconsistent_neg_int_start_values",
        should_have_1_no_alias_var);

    g_expected_message = "Inconsistent start values in alias set, "
        "start value '1' of 'v1' does not match "
        "start value '1' of 'v2'(negated alias).";
    test_parsing_fail_and_fmu("alias_validation/inconsistent_neg_enum_start_values",
        should_have_1_no_alias_var);

    g_expected_message = "Inconsistent start values in alias set, "
        "start value 'true' of 'v1' does not match "
        "start value 'true' of 'v2'(negated alias).";
    test_parsing_fail_and_fmu("alias_validation/inconsistent_neg_bool_start_values",
        should_have_1_no_alias_var);

    g_expected_message = "Inconsistent start values in alias set, "
        "start value 'a' of 'v1' does not match "
        "start value 'a' of 'v2'(negated alias).";
    test_parsing_fail_and_fmu("alias_validation/inconsistent_neg_str_start_values",
        should_have_1_no_alias_var);

    g_expected_message = "Inconsistent start values in alias set";
    parser_no_log_expected_message("alias_validation/consistent_real_start_values2");

    g_expected_message = "Inconsistent start values in alias set, "
        "start value '1' of 'v2' does not match "
        "start value '3' of 'v3'.";
    parser_log_expected_message("alias_validation/inconsistent_int_start_values2");

    g_expected_message = "Inconsistent start values in alias set";
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

    g_expected_message = "Found capability flag canSignalEvents which has been "
        "deprecated as it fills no function";
    parser_log_expected_message("deprecated/canSignalEvents");
}

void test_variable_no_type(void)
{
    g_expected_message = "No variable type element for variable v. Assuming Real.";
    parser_log_expected_message("incorrect/variable_no_type");
}

static fmi1_import_t* parse_xml(jm_callbacks* cb, const char* xmldir) {
    fmi_import_context_t* context;
    fmi1_import_t* xml;

    context = fmi_import_allocate_context(cb);

    xml = fmi1_import_parse_xml(context, xmldir);
    fmi_import_free_context(context);

    return xml;
}

/**
 * Tests that parsing works as expected, and that the previous locale and
 * thread settings are reset.
 *
 * Test is by default disabled, because it requires the target machine to have
 * specific language packs.
 */
static void test_locale_lc_numeric() {

    jm_callbacks* cb = jm_get_default_callbacks();
    char* loc_old = NULL;
    char* tmp = NULL;

    /* Any locale that uses decimal comma instead of decimal point. */
#ifdef WIN32
    char* locale_bad = "Swedish_Sweden.1252";
#else
    char* locale_bad = "sv_SE.utf8";
#endif

    /* Set/get thread-specific settings (and later check that they are
     * restored).
     * Do nothing for Linux since I don't think it's possible to check equality
     * of locale_t.
     */
#ifdef _MSC_VER
    int thread_setting = _DISABLE_PER_THREAD_LOCALE;
    _configthreadlocale(thread_setting);
#endif

    /* NOT MT-SAFE: But it's the only way to test it for Linux. There are
     * currently no other tests that modify the locale globally, so should be
     * OK. Also, ctest tests are by default not run in parallel.
     */
    tmp = setlocale(LC_NUMERIC, locale_bad);
    if (!tmp) {
        /* If this errors, it's possible that your machine doesn't have
         * the locale installed.
         *
         * Windows: It seemed like I had at least Danish, French, Swedish
         *   installed by default.
         *
         * Linux (Ubuntu 18): I had to install a language pack to get this.
         */
        fail("failed to set locale");
    }

    /*
     * Value of 'tmp' returned from 'setlocale' may be changed by further calls
     * to setlocale, and it's also possible that the returned value is not
     * "string equal" to the argument (i.e. alias values for the same locale).
     * To be able to later compare the restored value, we therefore must copy
     * 'tmp'.
     */
    loc_old = (char*)malloc(strlen(tmp) + 1);
    if (!loc_old) {
        fail("failed to alloc memory");
    }
    strcpy(loc_old, tmp);
    tmp = NULL;

    {
        /* Perform parsing and verify that the bad global locale did not affect
         * the result. */

        int failed = 0;
        char* xmldir = concat(g_name_check_test_directory, "/env/locale");
        fmi1_import_t* xml = parse_xml(cb, xmldir);
        free(xmldir);

        if (xml == NULL) {
            fail("failed to parse FMU");
        }

        if (fmi1_import_get_default_experiment_start(xml)     != 2.3  ||
            fmi1_import_get_default_experiment_stop(xml)      != 3.55 ||
            fmi1_import_get_default_experiment_tolerance(xml) != 1e-6)
        {
            /* If the decimal delimiter is comma, sscanf will only parse
             * until the dot. */
            printf("Test failure: incorrect default experiment value\n");
            failed = 1;
        }

        fmi1_import_free(xml);

        if (failed) {
            fail("... see above printed messages");
        }
     }

    /* Cleanup and verify that locale is properly restored.
     *
     * Getting locale should be MT-safe if all setting of locale is done in
     * per_thread context.
     */
    tmp = setlocale(LC_NUMERIC, loc_old);
    free(loc_old);
    if (!tmp) {
        fail("failed to restore locale");
    } else if (strcmp(tmp, locale_bad)) {
        fail("unexpected locale");
    }

#ifdef _MSC_VER
    if (_configthreadlocale(0) != thread_setting) {
        /* This was set at the beginning of the test, and should now have been restored. */
        fail("unexpected Windows thread setting");
    }
#endif
}

int main(int argc, char *argv[])
{
    if (argc == 2) {
        g_name_check_test_directory = argv[1];
    } else {
        printf("Usage: %s <path to folder 'parser_test_xmls'>\n", argv[0]);
        exit(CTEST_RETURN_FAIL);
    }

    test_variable_naming_conventions();
    test_deprecation_errors();
    test_alias_set_error_handling();
    test_variable_no_type();

#ifdef FMILIB_TEST_LOCALE
    test_locale_lc_numeric();
#endif

    return 0;
}
