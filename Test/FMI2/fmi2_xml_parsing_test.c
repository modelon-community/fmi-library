#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <locale.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

static char *EXPECTED_MESSAGE = "Invalid structured ScalarVariable name";

static int g_has_logged_expected_msg;
static char *g_name_check_test_directory;

void importlogger(jm_callbacks* c, jm_string module,
        jm_log_level_enu_t log_level, jm_string message)
{
    printf("module = %s, log level = %d: %s\n", module, log_level, message);
    if (!strncmp(EXPECTED_MESSAGE, message, strlen(EXPECTED_MESSAGE))) {
        g_has_logged_expected_msg = 1;
    }
}

void test_parser(char *xml_dir, int should_log_expected_msg, int configuration)
{
    jm_callbacks cb;
    fmi_import_context_t *context;
    fmi2_import_t *fmu;
    char *full_path;
    int res = 1;

    cb.malloc    = malloc;
    cb.calloc    = calloc;
    cb.realloc   = realloc;
    cb.free      = free;
    cb.logger    = importlogger;
    cb.log_level = jm_log_level_all;
    cb.context   = NULL;
    context = fmi_import_allocate_context(&cb);
    if (configuration != 0) {
        fmi_import_set_configuration(context, configuration);
    }

    g_has_logged_expected_msg = 0;

    full_path = concat(g_name_check_test_directory, xml_dir);
    fmu = fmi2_import_parse_xml(context, full_path, NULL);
    fmi_import_free_context(context);
    free(full_path);

    if (fmu == NULL) {
        goto err1;
    }
    if (should_log_expected_msg != g_has_logged_expected_msg) {
        goto err2;
    }
    res = 0;

err2:
    fmi2_import_free(fmu); /* this function is using callbacks, so free 'fmu' first */
err1:

    if (res) {
        exit(CTEST_RETURN_FAIL);
    }
}

void fail_name_check(char *xml_dir)
{
    test_parser(xml_dir, 1, FMI_IMPORT_NAME_CHECK);
    test_parser(xml_dir, 0, 0);
}

void pass_name_check(char *xml_dir)
{
    test_parser(xml_dir, 0, FMI_IMPORT_NAME_CHECK);
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
    EXPECTED_MESSAGE = "Two variables with the same name";
    pass_name_check("naming_conventions_xmls/list/cba");
    fail_name_check("naming_conventions_xmls/list/acad");

    /* flat hierarchy test */
    fail_name_check("naming_conventions_xmls/flat/acad");
    pass_name_check("naming_conventions_xmls/flat/q-char-nonescaped");
}

static fmi2_import_t* parse_xml(jm_callbacks* cb, const char* xmldir) {
    fmi_import_context_t* context;
    fmi2_import_t* xml;

    context = fmi_import_allocate_context(cb);

    xml = fmi2_import_parse_xml(context, xmldir, NULL);
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
        fmi2_import_t* xml = parse_xml(cb, xmldir);
        free(xmldir);

        if (xml == NULL) {
            fail("failed to parse FMU");
        }

        if (fmi2_import_get_default_experiment_start(xml)     != 2.3  ||
            fmi2_import_get_default_experiment_stop(xml)      != 3.55 ||
            fmi2_import_get_default_experiment_tolerance(xml) != 1e-6 ||
            fmi2_import_get_default_experiment_step(xml)      != 2e-3)
        {
            /* If the decimal delimiter is comma, sscanf will only parse
             * until the dot. */
            printf("Test failure: incorrect default experiment value\n");
            failed = 1;
        }

        fmi2_import_free(xml);

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

#ifdef FMILIB_TEST_LOCALE
    test_locale_lc_numeric();
#endif

    return 0;
}
