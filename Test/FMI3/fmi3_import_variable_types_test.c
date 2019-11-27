#include <stdio.h>
#include <float.h>

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

/* Parse small Float64 variable */
static int test_var1(fmi3_import_t *xml)
{
    fmi3_import_variable_t *v = fmi3_import_get_variable_by_name(xml, "var1");
    fmi3_import_float64_variable_t *var;

    fmi3_float64_t min;
    fmi3_float64_t max;
    fmi3_float64_t nominal;
    fmi3_float64_t start;

    fmi3_float64_t min_ref =        -DBL_MAX;
    fmi3_float64_t max_ref =        DBL_MAX;
    fmi3_float64_t nominal_ref =    1.000000000000000;
    fmi3_float64_t start_ref =      3.141592653589793;

    ASSERT_MSG(v != NULL, "Could not find variable to test");
    ASSERT_MSG(fmi3_import_get_variable_vr(v) == 1, "Bad vr");
    ASSERT_MSG(fmi3_import_get_variable_description(v) == NULL,
               "Default description not empty");
    ASSERT_MSG(fmi3_import_get_causality(v) == fmi3_causality_enu_local,
               "Default causality should be local");
    ASSERT_MSG(fmi3_import_get_variability(v) == fmi3_variability_enu_continuous,
               "Variability should be continuous");

    var = fmi3_import_get_variable_as_float64(v);
    ASSERT_MSG(var != NULL, "Failed to convert to Float64 variable");

    /* user defined */
    start = fmi3_import_get_float64_variable_start(var);
    ASSERT_MSG(start == start_ref, "Float64 variable's start value didn't match");

    /* default */
    min = fmi3_import_get_float64_variable_min(var);
    ASSERT_MSG(min == min_ref, "min value didn't match");

    /* default */
    max = fmi3_import_get_float64_variable_max(var);
    ASSERT_MSG(max == max_ref, "max value didn't match");

    /* default */
    nominal = fmi3_import_get_float64_variable_nominal(var);
    ASSERT_MSG(nominal == nominal_ref, "nominal value didn't match");

    /* TODO: add checks for the type */

    return TEST_OK;
}

/* Parse small Float32 variable */
static int test_var2(fmi3_import_t *xml)
{
    fmi3_import_variable_t *v = fmi3_import_get_variable_by_name(xml, "var2");
    fmi3_import_float32_variable_t *var;

    fmi3_float32_t min;
    fmi3_float32_t max;
    fmi3_float32_t nominal;
    fmi3_float32_t start;

    fmi3_float32_t min_ref =        -FLT_MAX;
    fmi3_float32_t max_ref =        FLT_MAX;
    fmi3_float32_t nominal_ref =    1.00000000f;
    fmi3_float32_t start_ref =      3.14159274f;

    ASSERT_MSG(v != NULL, "Could not find variable to test");
    ASSERT_MSG(fmi3_import_get_variable_vr(v) == 2, "Bad vr");
    ASSERT_MSG(fmi3_import_get_variable_description(v) == NULL,
               "Default description not empty");
    ASSERT_MSG(fmi3_import_get_causality(v) == fmi3_causality_enu_local,
               "Default causality should be local");
    ASSERT_MSG(fmi3_import_get_variability(v) == fmi3_variability_enu_continuous,
               "Variability should be continuous");

    var = fmi3_import_get_variable_as_float32(v);
    ASSERT_MSG(var != NULL, "Failed to convert to Float32 variable");

    /* user defined */
    start = fmi3_import_get_float32_variable_start(var);
    ASSERT_MSG(start == start_ref, "Float32 variable's start value didn't match");

    /* default */
    min = fmi3_import_get_float32_variable_min(var);
    ASSERT_MSG(min == min_ref, "min value didn't match");

    /* default */
    max = fmi3_import_get_float32_variable_max(var);
    ASSERT_MSG(max == max_ref, "max value didn't match");

    /* default */
    nominal = fmi3_import_get_float32_variable_nominal(var);
    ASSERT_MSG(nominal == nominal_ref, "nominal value didn't match");

    return TEST_OK;
}

/* Parse user defined: min, max, nominal */
static int test_var3(fmi3_import_t *xml)
{
    fmi3_import_variable_t *v = fmi3_import_get_variable_by_name(xml, "var3");
    fmi3_import_float64_variable_t *var;

    fmi3_float64_t min;
    fmi3_float64_t max;
    fmi3_float64_t nominal;

    /* Arbitrarily chosen values that don't get truncated/rounded */
    fmi3_float64_t min_ref =       -3500.0000000000000;
    fmi3_float64_t max_ref =        3343492.7982000001;
    fmi3_float64_t nominal_ref =    7.7776999499999997;

    ASSERT_MSG(v != NULL, "Could not find variable to test");
    ASSERT_MSG(fmi3_import_get_variable_vr(v) == 3, "Bad vr");

    var = fmi3_import_get_variable_as_float64(v);
    ASSERT_MSG(var != NULL, "Failed to convert to Float64 variable");

    /* user defined */
    min = fmi3_import_get_float64_variable_min(var);
    ASSERT_MSG(min == min_ref, "min value didn't match");

    /* user defined */
    max = fmi3_import_get_float64_variable_max(var);
    ASSERT_MSG(max == max_ref, "max value didn't match");

    /* user defined */
    nominal = fmi3_import_get_float64_variable_nominal(var);
    ASSERT_MSG(nominal == nominal_ref, "nominal value didn't match");

    return TEST_OK;
}

/* Parse user defined: min, max, nominal */
static int test_var4(fmi3_import_t *xml)
{
    fmi3_import_variable_t *v = fmi3_import_get_variable_by_name(xml, "var4");
    fmi3_import_float32_variable_t *var;

    fmi3_float32_t min;
    fmi3_float32_t max;
    fmi3_float32_t nominal;

    /* Arbitrarily chosen values that don't get truncated/rounded */
    fmi3_float32_t min_ref =       -3500.00000f;
    fmi3_float32_t max_ref =        3343492.75f;
    fmi3_float32_t nominal_ref =    7.77769995f;

    ASSERT_MSG(v != NULL, "Could not find variable to test");
    ASSERT_MSG(fmi3_import_get_variable_vr(v) == 4, "Bad vr");

    var = fmi3_import_get_variable_as_float32(v);
    ASSERT_MSG(var != NULL, "Failed to convert to Float32 variable");

    /* user defined */
    min = fmi3_import_get_float32_variable_min(var);
    ASSERT_MSG(min == min_ref, "min value didn't match");

    /* user defined */
    max = fmi3_import_get_float32_variable_max(var);
    ASSERT_MSG(max == max_ref, "max value didn't match");

    /* user defined */
    nominal = fmi3_import_get_float32_variable_nominal(var);
    ASSERT_MSG(nominal == nominal_ref, "nominal value didn't match");

    return TEST_OK;
}

int main(int argc, char **argv)
{
    fmi3_import_t *xml;
    int ret = 1;
    if (argc != 2) {
        printf("Usage: %s <path_to_dir_containing_float_modelDescription>\n", argv[0]);
        return CTEST_RETURN_FAIL;
    }

    printf("Running fmi3_import_variable_types_test\n");

    xml = parse_xml(argv[1]);
    if (xml == NULL) {
        return CTEST_RETURN_FAIL;
    }

    ret &= test_var1(xml);
    ret &= test_var2(xml);
    ret &= test_var3(xml);
    ret &= test_var4(xml);

    fmi3_import_free(xml);
    return ret == 0 ? CTEST_RETURN_FAIL : CTEST_RETURN_SUCCESS;
}
