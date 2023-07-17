#include <stdio.h>
#include <float.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

static fmi1_import_t *parse_xml(const char *model_desc_path)
{
    jm_callbacks *cb = jm_get_default_callbacks();
    fmi_import_context_t *ctx = fmi_import_allocate_context(cb);
    fmi1_import_t *xml;

    if (ctx == NULL) {
        return NULL;
    }

    xml = fmi1_import_parse_xml(ctx, model_desc_path);

    fmi_import_free_context(ctx);
    return xml;
}

/* verify correct default values when quantity is not specified */
static int test_quantity_default(fmi1_import_t *xml)
{
    fmi1_import_variable_typedef_t* td;
    fmi1_import_type_definitions_t* tds;
    unsigned int n_tds;
    unsigned int i; /* td_idx */
    const char* tname;
    int nBaseTypes = 5; /* integer, real, string, boolean, enum */
    int nTdsTested = 0;
    char* tdPrefix = "td_minimal_";

    tds = fmi1_import_get_type_definitions(xml);
    ASSERT_MSG(tds != NULL, "could not get type definitions");
    n_tds = fmi1_import_get_type_definition_number(tds);

    /* check that NULL is returned for all types */
    for (i = 0; i < n_tds; i++) {
        td = fmi1_import_get_typedef(tds, i);
        tname = fmi1_import_get_type_name(td);
        if (strncmp(tname, tdPrefix, strlen(tdPrefix)) == 0) { /* only test the minimal tds, (test XML might contain others) */
            ASSERT_MSG(fmi1_import_get_type_quantity(td) == NULL, "expected default quantity to be null ptr");
            nTdsTested++;
        }
    }

    ASSERT_MSG(nTdsTested == nBaseTypes, "too few typedefs found");

    return CTEST_RETURN_SUCCESS;
}

/* verify quantity on variable, same for all variables */
static int test_var_quantity(fmi1_import_t *xml, fmi1_string_t exp, const char* varNames[])
{
    fmi1_import_variable_t* v;
    fmi1_string_t quantity;

    /* real */
    v = fmi1_import_get_variable_by_name(xml, varNames[0]);
    ASSERT_MSG(v != NULL, "variable wasn't found by name");
    quantity = fmi1_import_get_real_variable_quantity(fmi1_import_get_variable_as_real(v));
    ASSERT_MSG(quantity == exp || /* this allows exp == NULL */
            strcmp(quantity, exp) == 0, "wrong variable attribute value: quantity");

    /* int */
    v = fmi1_import_get_variable_by_name(xml, varNames[1]);
    ASSERT_MSG(v != NULL, "variable wasn't found by name");
    quantity = fmi1_import_get_integer_variable_quantity(fmi1_import_get_variable_as_integer(v));
    ASSERT_MSG(quantity == exp || /* this allows exp == NULL */
            strcmp(quantity, exp) == 0, "wrong variable attribute value: quantity");

    /* enum */
    v = fmi1_import_get_variable_by_name(xml, varNames[2]);
    ASSERT_MSG(v != NULL, "variable wasn't found by name");
    quantity = fmi1_import_get_enum_variable_quantity(fmi1_import_get_variable_as_enum(v));
    ASSERT_MSG(quantity == exp || /* this allows exp == NULL */
            strcmp(quantity, exp) == 0, "wrong variable attribute value: quantity");

    return CTEST_RETURN_SUCCESS;
}

/* verify vars with defined quantity */
static int test_var_quantity_defined(fmi1_import_t *xml)
{
    const char* varNames[] = { "real_with_attr", "int_with_attr", "enum_with_attr" };
    return test_var_quantity(xml, "velocity", varNames);
}

/* verify vars with undefined quantity */
static int test_var_quantity_undefined(fmi1_import_t *xml)
{
    const char* varNames[] = { "real_no_attr", "int_no_attr", "enum_no_attr" };
    return test_var_quantity(xml, NULL, varNames);
}

/* verify real-specific type attributes on variable */
static int test_real_var_attributes_exist(fmi1_import_t *xml, fmi1_boolean_t expRelativeQuantity, fmi1_string_t varName)
{
    fmi1_import_variable_t* v;
    fmi1_import_real_variable_t* vReal;
    fmi1_boolean_t relativeQuantity;

    /* real */
    v = fmi1_import_get_variable_by_name(xml, varName);
    ASSERT_MSG(v != NULL, "variable wasn't found by name");

    vReal = fmi1_import_get_variable_as_real(v);

    relativeQuantity = fmi1_import_get_real_variable_relative_quantity(vReal);
    ASSERT_MSG(relativeQuantity == expRelativeQuantity, "wrong variable attribute value: relativeQuantity");

    return CTEST_RETURN_SUCCESS;
}

static int test_real_var_attributes_defined(fmi1_import_t *xml)
{
    fmi1_boolean_t expRelativeQuantity = fmi1_true;

    return test_real_var_attributes_exist(xml, expRelativeQuantity, "real_with_attr");
}

static int test_real_var_attributes_undefined(fmi1_import_t *xml)
{
    fmi1_boolean_t expRelativeQuantity = fmi1_false;

    return test_real_var_attributes_exist(xml, expRelativeQuantity, "real_no_attr");
}

static int test_real_var_attributes_defined_in_typedef(fmi1_import_t *xml)
{
    fmi1_boolean_t expRelativeQuantity = fmi1_true;

    return test_real_var_attributes_exist(xml, expRelativeQuantity, "real_with_typedef");
}

static int test_real_var_attributes_defined_in_typedef_partially(fmi1_import_t *xml)
{
    fmi1_boolean_t expRelativeQuantity = fmi1_true;

    return test_real_var_attributes_exist(xml, expRelativeQuantity, "real_with_typedef_override");
}

int main(int argc, char **argv)
{
    fmi1_import_t *xml;
    int ret = 1;
    if (argc != 2) {
        printf("Usage: %s <path_to_dir_containing_float_modelDescription>\n", argv[0]);
        return CTEST_RETURN_FAIL;
    }

    printf("Running fmi1_import_variable_types_test\n");

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

    fmi1_import_free(xml);
    return ret;
}
