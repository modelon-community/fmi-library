#include "fmilib.h"
#include "fmil_test.h"
#include "config_test.h"

static int test_invalid_variability_causality(fmi2_import_t *xml)
{
    fmi2_import_variable_t *v;
    const char *errmsg;
    const char *expected = "Invalid combination of variability constant and "
    "causality input for variable 'dummy1'. Setting variability to 'continuous'";

    v = fmi2_import_get_variable_by_vr(xml, fmi2_base_type_real, 0);
    ASSERT_MSG(fmi2_import_get_variability(v) == fmi2_variability_enu_continuous,
               "Invalid variability constant should be changed to continuous");

    errmsg = fmi2_import_get_last_error(xml);
    ASSERT_MSG(strncmp(expected, errmsg, strlen(expected)) == 0, errmsg);

    return TEST_OK;
}

int main(int argc, char **argv)
{
    int ret;
    fmi2_import_t *xml;
    jm_callbacks * cb = jm_get_default_callbacks();
    fmi_import_context_t *ctx;

    if (argc != 2) {
        printf("Usage: %s <variable_bad_causality_variability_dir>\n", argv[0]);
        return CTEST_RETURN_FAIL;
    }

    ctx = fmi_import_allocate_context(cb);
    if (ctx == NULL) {
        printf("Failed to allocate memory");
        return CTEST_RETURN_FAIL;
    }

    xml = fmi2_import_parse_xml(ctx, argv[1], NULL);
    fmi_import_free_context(ctx);
    if (xml == NULL) {
        return CTEST_RETURN_FAIL;
    }

    ret = test_invalid_variability_causality(xml);

    return ret == TEST_OK ? CTEST_RETURN_SUCCESS : CTEST_RETURN_FAIL;
}
