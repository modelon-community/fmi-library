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

/* Parse small Float64 typedef */
static int test_type1(fmi3_import_t *xml)
{
    fmi3_import_variable_t *v = fmi3_import_get_variable_by_name(xml, "var.name1");
    fmi3_import_variable_typedef_t *t;
    fmi3_import_float_typedef_t *type;

    ASSERT_MSG(v != NULL, "could not find variable to test");
    ASSERT_MSG(fmi3_import_get_variable_vr(v) == 1073741824, "bad vr");

    t = fmi3_import_get_variable_declared_type(v);
    ASSERT_MSG(t != NULL, "no declaredType found for enum variable");

    ASSERT_MSG(strcmp(fmi3_import_get_type_name(t), "Float64.name") == 0, "wrong type name");
    ASSERT_MSG(strcmp(fmi3_import_get_type_quantity(t), "typeQuantity") == 0, "wrong quantity in type definition");

    type = fmi3_import_get_type_as_float(t);
    ASSERT_MSG(type != NULL, "failed to convert to float type definition");
    ASSERT_MSG(fmi3_import_get_float64_type_max(type) == DBL_MAX, "wrong max for type");        /* default */
    ASSERT_MSG(fmi3_import_get_float64_type_min(type) == 0.0, "wrong min for type");            /* from md */
    ASSERT_MSG(fmi3_import_get_float64_type_nominal(type) == 1.0, "wrong nominal for type");    /* default */
    ASSERT_MSG(fmi3_import_get_float64_type_is_relative_quantity(type) == fmi3_false, "wrong relativeQuantity value for type");
    ASSERT_MSG(fmi3_import_get_float64_type_is_unbounded(type) == fmi3_false, "wrong unbounded value for type");

    return TEST_OK;
}

/* Parse small Float32 typedef */
static int test_type2(fmi3_import_t *xml)
{
    fmi3_import_variable_t *v = fmi3_import_get_variable_by_name(xml, "var.name2");
    fmi3_import_variable_typedef_t *t;
    fmi3_import_float_typedef_t *type;

    ASSERT_MSG(v != NULL, "could not find variable to test");
    ASSERT_MSG(fmi3_import_get_variable_vr(v) == 805306369, "bad vr");

    t = fmi3_import_get_variable_declared_type(v);
    ASSERT_MSG(t != NULL, "no declaredType found for enum variable");

    ASSERT_MSG(strcmp(fmi3_import_get_type_name(t), "Float32_name") == 0, "wrong type name");
    ASSERT_MSG(strcmp(fmi3_import_get_type_quantity(t), "type_quantity") == 0, "wrong quantity in type definition");

    type = fmi3_import_get_type_as_float(t);
    ASSERT_MSG(type != NULL, "failed to convert to float type definition");
    ASSERT_MSG(fmi3_import_get_float32_type_max(type) == 1000.0f, "wrong max for type");        /* from md */
    ASSERT_MSG(fmi3_import_get_float32_type_min(type) == -FLT_MAX, "wrong min for type");       /* default */
    ASSERT_MSG(fmi3_import_get_float32_type_nominal(type) == 5.0f, "wrong nominal for type");   /* from md */
    ASSERT_MSG(fmi3_import_get_float32_type_is_relative_quantity(type) == fmi3_false, "wrong relativeQuantity value for type");
    ASSERT_MSG(fmi3_import_get_float32_type_is_unbounded(type) == fmi3_false, "wrong unbounded value for type");

    return TEST_OK;
}

/* parse small int8 typedef */
static int test_type3(fmi3_import_t *xml)
{
    fmi3_import_variable_t *v = fmi3_import_get_variable_by_name(xml, "v3");
    fmi3_import_variable_typedef_t *t;
    fmi3_import_int_typedef_t *type;
    unsigned int n_typedef;
    fmi3_import_type_definitions_t* tds;
    unsigned int td_idx = 2;

    tds = fmi3_import_get_type_definitions(xml);
    ASSERT_MSG(tds != NULL, "could not get type definitions");

    n_typedef = fmi3_import_get_type_definition_number(tds);
    ASSERT_MSG(n_typedef > td_idx, "too few typedefs in test");

    t = fmi3_import_get_typedef(tds, td_idx);
    ASSERT_MSG(t != NULL, "no declaredType found for enum variable");

    ASSERT_MSG(strcmp(fmi3_import_get_type_name(t), "name_int8") == 0, "wrong type name");
    ASSERT_MSG(strcmp(fmi3_import_get_type_quantity(t), "") == 0, "wrong quantity in type definition"); /* testing default case: none specified in MD */

    type = fmi3_import_get_type_as_intXX(t);
    ASSERT_MSG(type != NULL, "failed to convert to int8 type definition");
    ASSERT_MSG(fmi3_import_get_int8_type_min(type) == INT8_MIN, "wrong min for type");
    /*
    TODO: Continue adding checks
    ASSERT_MSG(fmi3_import_get_float32_type_min(type) == -FLT_MAX, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_float32_type_nominal(type) == 5.0f, "wrong nominal for type");
    ASSERT_MSG(fmi3_import_get_float32_type_is_relative_quantity(type) == fmi3_false, "wrong relativeQuantity value for type");
    ASSERT_MSG(fmi3_import_get_float32_type_is_unbounded(type) == fmi3_false, "wrong unbounded value for type");
    */

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

    ret &= test_type1(xml);
    ret &= test_type2(xml);
    ret &= test_type3(xml);

    fmi3_import_free(xml);
    return ret == 0 ? CTEST_RETURN_FAIL : CTEST_RETURN_SUCCESS;
}
