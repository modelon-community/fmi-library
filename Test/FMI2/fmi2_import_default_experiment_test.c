#include <stdio.h>

#include <fmilib.h>
#include "config_test.h"
#include "fmil_test.h"

static fmi2_import_t *parse_xml(const char *model_desc_path)
{
    jm_callbacks *cb = jm_get_default_callbacks();
    fmi_import_context_t *ctx = fmi_import_allocate_context(cb);
    fmi2_import_t *xml;

    if (ctx == NULL) {
        return NULL;
    }

    xml = fmi2_import_parse_xml(ctx, model_desc_path, NULL);

    fmi_import_free_context(ctx);
    return xml;
}

/* get values from DefaultExperiment */
static int test_default_experiment_defined(fmi2_import_t *xml)
{
    ASSERT_MSG(fmi2_import_get_default_experiment_start(xml) == 2.3, "wrong startTime");
    ASSERT_MSG(fmi2_import_get_default_experiment_stop(xml) == 3.55, "wrong stopTime");
    ASSERT_MSG(fmi2_import_get_default_experiment_tolerance(xml) == 1e-6, "wrong tolerance");
    ASSERT_MSG(fmi2_import_get_default_experiment_step(xml) == 2e-3, "wrong stepSize");

    return TEST_OK;
}

int main(int argc, char **argv)
{
#define PATH_BUF_SIZE 1000
    char path_buf[PATH_BUF_SIZE]; /* buffer for path to md */
    fmi2_import_t *xml;
    int ret = 1;
    if (argc != 2) {
        printf("Usage: %s <path to dir that contains 'defined' and 'undefined' "
                "parent dirs of modelDescription files (including trailing file separator "
                "(slash or backslash))>\n", argv[0]);
        return CTEST_RETURN_FAIL;
    }

    printf("Running fmi2_import_variable_test\n");

    xml = parse_xml(strcat(strncpy(path_buf, argv[1], PATH_BUF_SIZE), "defined"));
    if (xml == NULL) {
        return CTEST_RETURN_FAIL;
    }
    ret &= test_default_experiment_defined(xml);
    fmi2_import_free(xml);

    /*
        xml = parse_xml(strcat(strncpy(path_buf, argv[1], PATH_BUF_SIZE), "undefined"));
        if (xml == NULL) {
            return CTEST_RETURN_FAIL;
        }
        ret &= test_default_experiment_undefined(xml2);
        fmi2_import_free(xml);
    */

    return ret == 0 ? CTEST_RETURN_FAIL : CTEST_RETURN_SUCCESS;
#undef PATH_BUF_SIZE
}
