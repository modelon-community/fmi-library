#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

static int test_invalid_variability_causality(fmi3_import_t *xml)
{
    fmi3_import_variable_t *v;
    const char *errmsg;
    const char *expected = "Invalid combination of variability constant and "
    "causality input for variable 'dummy1'. Setting variability to 'continuous'";

    v = fmi3_import_get_variable_by_vr(xml, 0);
    ASSERT_MSG(fmi3_import_get_variability(v) == fmi3_variability_enu_continuous,
               "Invalid variability constant should be changed to continuous");

    errmsg = fmi3_import_get_last_error(xml);
    ASSERT_MSG(strncmp(expected, errmsg, strlen(expected)) == 0, "%s", errmsg);

    return TEST_OK;
}

int main(int argc, char **argv)
{
    int ret;
    fmi3_import_t *xml;
    jm_callbacks * cb = jm_get_default_callbacks();
    fmi_import_context_t *ctx;
    const char* xmldir = FMI3_TEST_XML_DIR "/variable_bad_variability_causality";

    ctx = fmi_import_allocate_context(cb);
    if (ctx == NULL) {
        printf("Failed to allocate memory");
        return CTEST_RETURN_FAIL;
    }

    xml = fmi3_import_parse_xml(ctx, xmldir, NULL);
    fmi_import_free_context(ctx);
    if (xml == NULL) {
        return CTEST_RETURN_FAIL;
    }

    ret = test_invalid_variability_causality(xml);

    fmi3_import_free(xml);
    return ret == TEST_OK ? CTEST_RETURN_SUCCESS : CTEST_RETURN_FAIL;
}
