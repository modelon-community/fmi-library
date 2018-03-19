#include "fmilib.h"
#include "fmil_test.h"
#include "config_test.h"

static fmi2_import_t *parse_xml(const char *mdpath)
{
    fmi2_import_t *parsed;
    jm_callbacks *cb = jm_get_default_callbacks();
    fmi_import_context_t *ctx = fmi_import_allocate_context(cb);

    if (ctx == NULL) {
        return NULL;
    }

    parsed = fmi2_import_parse_xml(ctx, mdpath, NULL);
    fmi_import_free_context(ctx);
    return parsed;
}

static int validate_variable(fmi2_import_variable_t *v)
{
    ASSERT_MSG(v != NULL, "Variable should not be NULL");

    ASSERT_MSG(fmi2_import_get_variable_base_type(v) == fmi2_base_type_real,
               "Default type should be real");

    ASSERT_MSG(fmi2_import_get_variability(v) == fmi2_variability_enu_continuous,
               "Default variability should be continuous");

    ASSERT_MSG(fmi2_import_get_causality(v) == fmi2_causality_enu_input,
               "Default causality should be input");

    ASSERT_MSG(fmi2_import_get_initial(v) == fmi2_initial_enu_unknown,
               "Default initial should be unknown (undefined for input)");

    return TEST_OK;
}

static int missing_type_test(fmi2_import_t *xml)
{
    int ret = TEST_OK;

    ret &= validate_variable(fmi2_import_get_variable_by_name(xml, "dummy1"));
    ret &= validate_variable(fmi2_import_get_variable_by_name(xml, "dummy2"));

    return ret;
}

int main(int argc, char **argv)
{
    int ret;
    fmi2_import_t *xml;

    printf("Running test %s\n", argv[0]);

    if (argc != 2) {
        printf("Usage: %s <variable_no_test_type_dir>\n", argv[0]);
        return CTEST_RETURN_FAIL;
    }

    xml = parse_xml(argv[1]);
    if (xml == NULL) {
        return CTEST_RETURN_FAIL;
    }

    ret = missing_type_test(xml);


    fmi2_import_free(xml);
    return ret == TEST_OK ? CTEST_RETURN_SUCCESS : CTEST_RETURN_FAIL;
}
