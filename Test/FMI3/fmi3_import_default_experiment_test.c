#include <stdio.h>

#include <fmilib.h>
#include "config_test.h"
#include "fmil_test.h"

static fmi3_import_t *parse_xml(const char *model_desc_path)
{
    jm_callbacks *cb = jm_get_default_callbacks();
    fmi_import_context_t *ctx = fmi_import_allocate_context(cb);
    fmi3_import_t *xml;

    if (ctx == NULL) {
        return NULL;
    }

    xml = fmi3_import_parse_xml(ctx, model_desc_path, NULL);

    fmi_import_free_context(ctx);
    return xml;
}

/* get values from DefaultExperiment */
static int test_default_experiment_defined(fmi3_import_t *xml)
{
    /* test defined */
    ASSERT_MSG(fmi3_import_get_default_experiment_has_start(xml) == 1, "test_undefined, has_start: incorrect");
    ASSERT_MSG(fmi3_import_get_default_experiment_has_stop(xml) == 1, "test_undefined, has_stop: incorrect");
    ASSERT_MSG(fmi3_import_get_default_experiment_has_tolerance(xml) == 1, "test_undefined, has_tolerance: incorrect");
    ASSERT_MSG(fmi3_import_get_default_experiment_has_step(xml) == 1, "test_undefined, has_step: incorrect");

    /* test value */
    ASSERT_MSG(fmi3_import_get_default_experiment_start(xml) == 2.3, "test_defined: incorrect startTime");
    ASSERT_MSG(fmi3_import_get_default_experiment_stop(xml) == 3.55, "test_defined: incorrect stopTime");
    ASSERT_MSG(fmi3_import_get_default_experiment_tolerance(xml) == 1e-6, "test_defined: incorrect tolerance");
    ASSERT_MSG(fmi3_import_get_default_experiment_step(xml) == 2e-3, "test_defined: incorrect stepSize");

    return TEST_OK;
}

/* get default values from DefaultExperiment */
static int test_default_experiment_undefined(fmi3_import_t *xml)
{
    /* test defined */
    ASSERT_MSG(fmi3_import_get_default_experiment_has_start(xml) == 0, "test_undefined, has_start: incorrect");
    ASSERT_MSG(fmi3_import_get_default_experiment_has_stop(xml) == 0, "test_undefined, has_stop: incorrect");
    ASSERT_MSG(fmi3_import_get_default_experiment_has_tolerance(xml) == 0, "test_undefined, has_tolerance: incorrect");
    ASSERT_MSG(fmi3_import_get_default_experiment_has_step(xml) == 0, "test_undefined, has_step: incorrect");

    /* test value */
    ASSERT_MSG(fmi3_import_get_default_experiment_start(xml) == 0.0, "test_undefined: incorrect default startTime");
    ASSERT_MSG(fmi3_import_get_default_experiment_stop(xml) == 1.0, "test_undefined: incorrect default stopTime");
    ASSERT_MSG(fmi3_import_get_default_experiment_tolerance(xml) == 1e-4, "test_undefined: incorrect default tolerance");
    ASSERT_MSG(fmi3_import_get_default_experiment_step(xml) == 1e-2, "test_undefined: incorrect default stepSize");

    return TEST_OK;
}

/* get mix of default and user defined values from DefaultExperiment */
static int test_default_experiment_mixed(fmi3_import_t *xml)
{
    /* test defined */
    ASSERT_MSG(fmi3_import_get_default_experiment_has_start(xml) == 1, "test_mixed, has_start: incorrect");
    ASSERT_MSG(fmi3_import_get_default_experiment_has_stop(xml) == 0, "test_mixed, has_stop: incorrect");
    ASSERT_MSG(fmi3_import_get_default_experiment_has_tolerance(xml) == 1, "test_mixed, has_tolerance: incorrect");
    ASSERT_MSG(fmi3_import_get_default_experiment_has_step(xml) == 0, "test_mixed, has_step: incorrect");

    /* test value */
    ASSERT_MSG(fmi3_import_get_default_experiment_start(xml) == 2.3, "test_mixed: incorrect default startTime");
    ASSERT_MSG(fmi3_import_get_default_experiment_stop(xml) == 1.0, "test_mixed: incorrect default stopTime");
    ASSERT_MSG(fmi3_import_get_default_experiment_tolerance(xml) == 1e-6, "test_mixed: incorrect default tolerance");
    ASSERT_MSG(fmi3_import_get_default_experiment_step(xml) == 1e-2, "test_mixed: incorrect default stepSize");

    return TEST_OK;
}

int main(int argc, char **argv)
{
#define PATH_BUF_SIZE 1000
    char path_buf[PATH_BUF_SIZE]; /* buffer for path to md */
    fmi3_import_t *xml;
    int ret = 1;
    if (argc != 2) {
        printf("Usage: %s <path to dir that contains 'defined' and 'undefined' "
                "parent dirs of modelDescription files (including trailing file separator "
                "(slash or backslash))>\n", argv[0]);
        return CTEST_RETURN_FAIL;
    }

    printf("Running fmi3_import_variable_test\n");

    /* test 1 */
    xml = parse_xml(strcat(strncpy(path_buf, argv[1], PATH_BUF_SIZE), "defined"));
    if (xml == NULL) {
        return CTEST_RETURN_FAIL;
    }
    ret &= test_default_experiment_defined(xml);
    fmi3_import_free(xml);

    /* test 2 */
    xml = parse_xml(strcat(strncpy(path_buf, argv[1], PATH_BUF_SIZE), "undefined"));
    if (xml == NULL) {
        return CTEST_RETURN_FAIL;
    }
    ret &= test_default_experiment_undefined(xml);
    fmi3_import_free(xml);

    /* test 3 */
    xml = parse_xml(strcat(strncpy(path_buf, argv[1], PATH_BUF_SIZE), "mixed"));
    if (xml == NULL) {
        return CTEST_RETURN_FAIL;
    }
    ret &= test_default_experiment_mixed(xml);
    fmi3_import_free(xml);

    return ret == 0 ? CTEST_RETURN_FAIL : CTEST_RETURN_SUCCESS;
#undef PATH_BUF_SIZE
}
