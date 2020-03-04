#include <stdio.h>
#include <float.h>

#include "fmilib.h"
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

/* verify correct default values when quantity is not specified */
static int test_quantity_default(fmi2_import_t *xml)
{
    fmi2_import_variable_typedef_t *t;
    fmi2_import_type_definitions_t* tds;
    unsigned int n_tds;
    unsigned int i; /* td_idx */
    const char* tname;
    const char* str_empty = "";

    tds = fmi2_import_get_type_definitions(xml);
    ASSERT_MSG(tds != NULL, "could not get type definitions");
    n_tds = fmi2_import_get_type_definition_number(tds);
    ASSERT_MSG(n_tds >= 5, "too few typedefs found");

    /* check that NULL is returned for all types */
    for (i = 0; i < n_tds ; i++) {
        t = fmi2_import_get_typedef(tds, i);
        tname = fmi2_import_get_type_name(t);
        ASSERT_MSG(fmi2_import_get_type_quantity(t) == NULL, "expected default quantity to be null ptr");
    }

    return TEST_OK;
}

/* verify quantity on variable, same for all variables */
static int test_var_quantity(fmi2_import_t *xml, fmi2_string_t exp, const char* varNames[])
{
    fmi2_import_variable_t* v;
    fmi2_string_t quantity;

    /* real */
    v = fmi2_import_get_variable_by_name(xml, varNames[0]);
    ASSERT_MSG(v != NULL, "variable wasn't found by name");
    quantity = fmi2_import_get_real_variable_quantity(fmi2_import_get_variable_as_real(v));
    ASSERT_MSG(quantity == exp || /* this allows exp == NULL */
            strcmp(quantity, exp) == 0, "wrong variable attribute value: quantity");

    /* int */
    v = fmi2_import_get_variable_by_name(xml, varNames[1]);
    ASSERT_MSG(v != NULL, "variable wasn't found by name");
    quantity = fmi2_import_get_integer_variable_quantity(fmi2_import_get_variable_as_integer(v));
    ASSERT_MSG(quantity == exp || /* this allows exp == NULL */
            strcmp(quantity, exp) == 0, "wrong variable attribute value: quantity");

    /* enum */
    v = fmi2_import_get_variable_by_name(xml, varNames[2]);
    ASSERT_MSG(v != NULL, "variable wasn't found by name");
    quantity = fmi2_import_get_enum_variable_quantity(fmi2_import_get_variable_as_enum(v));
    ASSERT_MSG(quantity == exp || /* this allows exp == NULL */
            strcmp(quantity, exp) == 0, "wrong variable attribute value: quantity");

    return TEST_OK;
}

/* verify vars with defined quantity */
static int test_var_quantity_defined(fmi2_import_t *xml)
{
    const char* varNames[] = { "real_with_attr", "int_with_attr", "enum_with_attr" };
    return test_var_quantity(xml, "velocity", varNames);
}

/* verify vars with undefined quantity */
static int test_var_quantity_undefined(fmi2_import_t *xml)
{
    const char* varNames[] = { "real_no_attr", "int_no_attr", "enum_no_attr" };
    return test_var_quantity(xml, NULL, varNames);
}

/* verify real-specific type attributes on variable */
static int test_real_var_attributes_exist(fmi2_import_t *xml, fmi2_boolean_t expUnbounded, fmi2_boolean_t expRelativeQuantity, fmi2_string_t varName)
{
    fmi2_import_variable_t* v;
    fmi2_import_real_variable_t* vReal;
    fmi2_boolean_t unbounded;
    fmi2_boolean_t relativeQuantity;

    /* real */
    v = fmi2_import_get_variable_by_name(xml, varName);
    ASSERT_MSG(v != NULL, "variable wasn't found by name");

    vReal = fmi2_import_get_variable_as_real(v);
    unbounded = fmi2_import_get_real_variable_unbounded(vReal);
    ASSERT_MSG(unbounded == expUnbounded, "wrong variable attribute value: unbounded");

    relativeQuantity = fmi2_import_get_real_variable_relative_quantity(vReal);
    ASSERT_MSG(relativeQuantity == expRelativeQuantity, "wrong variable attribute value: relativeQuantity");

    return TEST_OK;
}

static int test_real_var_attributes_defined(fmi2_import_t *xml)
{
    fmi2_boolean_t expUnbounded = fmi2_true;
    fmi2_boolean_t expRelativeQuantity = fmi2_true;

    return test_real_var_attributes_exist(xml, expUnbounded, expRelativeQuantity, "real_with_attr");
}

static int test_real_var_attributes_undefined(fmi2_import_t *xml)
{
    fmi2_boolean_t expUnbounded = fmi2_false;
    fmi2_boolean_t expRelativeQuantity = fmi2_false;

    return test_real_var_attributes_exist(xml, expUnbounded, expRelativeQuantity, "real_no_attr");
}

static int test_real_var_attributes_defined_in_typedef(fmi2_import_t *xml)
{
    fmi2_boolean_t expUnbounded = fmi2_false;
    fmi2_boolean_t expRelativeQuantity = fmi2_true;

    return test_real_var_attributes_exist(xml, expUnbounded, expRelativeQuantity, "real_with_typedef");
}

static int test_real_var_attributes_defined_in_typedef_partially(fmi2_import_t *xml)
{
    fmi2_boolean_t expUnbounded = fmi2_false;
    fmi2_boolean_t expRelativeQuantity = fmi2_true;

    return test_real_var_attributes_exist(xml, expUnbounded, expRelativeQuantity, "real_with_typedef_override");
}

int main(int argc, char **argv)
{
    fmi2_import_t *xml;
    int ret = 1;
    if (argc != 2) {
        printf("Usage: %s <path_to_dir_containing_float_modelDescription>\n", argv[0]);
        return CTEST_RETURN_FAIL;
    }

    printf("Running fmi2_import_variable_types_test\n");

    xml = parse_xml(argv[1]);
    if (xml == NULL) {
        return CTEST_RETURN_FAIL;
    }

    /* typedefs */
    ret &= test_quantity_default(xml);

    /* var type attributes */
    ret &= test_var_quantity_defined(xml);
    ret &= test_var_quantity_undefined(xml);
    ret &= test_real_var_attributes_defined(xml);
    ret &= test_real_var_attributes_undefined(xml);
    ret &= test_real_var_attributes_defined_in_typedef(xml);
    ret &= test_real_var_attributes_defined_in_typedef_partially(xml);

    fmi2_import_free(xml);
    return ret == 0 ? CTEST_RETURN_FAIL : CTEST_RETURN_SUCCESS;
}
