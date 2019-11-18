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

/* Parse minimal Float64 variable */
static int float64_test(fmi3_import_t *xml)
{
    fmi3_import_variable_t *v = fmi3_import_get_variable_by_name(xml, "var_float64");
    fmi3_import_float64_variable_t *v_f64;
    fmi3_import_variable_typedef_t *t;

    double pi_double = 3.141592653589793;
    fmi3Float64 start;

    ASSERT_MSG(v != NULL, "Could not find variable to test");
    ASSERT_MSG(fmi3_import_get_variable_vr(v) == 33, "Bad vr");
    ASSERT_MSG(fmi3_import_get_variable_description(v) == NULL,
               "Default description not empty");
    ASSERT_MSG(fmi3_import_get_causality(v) == fmi3_causality_enu_local,
               "Default causality should be local");
    ASSERT_MSG(fmi3_import_get_variability(v) == fmi3_variability_enu_continuous,
               "Variability should be continuous");

    v_f64 = fmi3_import_get_variable_as_float64(v);
    ASSERT_MSG(v_f64 != NULL, "Failed to convert to Float64 variable");
    start = fmi3_import_get_float64_variable_start(v_f64);

    ASSERT_MSG(start == pi_double, "Float64 variable's start value didn't match");

    /* TODO: add checks for the type */

    return TEST_OK;
}

int main(int argc, char **argv)
{
    fmi3_import_t *xml;
    int ret = 1;
    if (argc != 2) {
        printf("Usage: %s <path_to_dir_containing_float64's_modelDescription>\n", argv[0]);
        return CTEST_RETURN_FAIL;
    }

    printf("Running fmi3_import_variable_types_test\n");

    xml = parse_xml(argv[1]);
    if (xml == NULL) {
        return CTEST_RETURN_FAIL;
    }

    ret &= float64_test(xml);

    fmi3_import_free(xml);
    return ret == 0 ? CTEST_RETURN_FAIL : CTEST_RETURN_SUCCESS;
}
