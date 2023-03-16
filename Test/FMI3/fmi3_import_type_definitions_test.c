#include <stdio.h>
#include <float.h>

#include <fmilib.h>
#include "config_test.h"
#include "fmil_test.h"

#define UINTXX_MIN (0)

/* delta offsets from extreme values */
#define D_TYPE (1)   /* for types when overriding defaults */
#define D_VAR (2)    /* for variables when overriding defaults/typedefs */
#define D_START (3)  /* for variable start values when overriding defaults */

static int G_nErrors; /* global used to count the number of errors reported to the logger during parsing - reset before use! */

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


/******************************
 *** Non-numeric attributes ***
 ******************************/

 /* verify correct default values when quantity is not specified */
static int test_quantity_default(fmi3_import_t* xml)
{
    fmi3_import_variable_typedef_t* td;
    fmi3_import_type_definitions_t* tds;
    unsigned int n_tds;
    unsigned int i; /* td_idx */
    const char* tname;
    const char* str_empty = "";
    int nBaseTypes = 13; /* intXX: 8, floatXX: 2, string, boolean, enum */
    int nTdsTested = 0;
    char* tdPrefix = "td_minimal_";
    
    tds = fmi3_import_get_type_definitions(xml);
    ASSERT_MSG(tds != NULL, "could not get type definitions");
    n_tds = fmi3_import_get_type_definition_number(tds);

    /* check that NULL is returned for all types */
    for (i = 0; i < n_tds; i++) {
        td = fmi3_import_get_typedef(tds, i);
        tname = fmi3_import_get_type_name(td);
        if (strncmp(tname, tdPrefix, strlen(tdPrefix)) == 0) { /* only test the minimal tds, (test XML might contain others) */
            ASSERT_MSG(fmi3_import_get_type_quantity(td) == NULL, "expected default quantity to be null ptr");
            nTdsTested++;
        }
    }

    ASSERT_MSG(nTdsTested == nBaseTypes, "too few typedefs found");

    return TEST_OK;
}

/* verify quantity on variable, same for all variables */
static int test_var_quantity(fmi3_import_t* xml, fmi3_string_t exp, const char* varNames[])
{
    fmi3_import_variable_t* v;
    fmi3_string_t quantity;

    /* float64 */
    v = fmi3_import_get_variable_by_name(xml, varNames[0]);
    ASSERT_MSG(v != NULL, "variable wasn't found by name");
    quantity = fmi3_import_get_float64_variable_quantity(fmi3_import_get_variable_as_float64(v));
    ASSERT_MSG(quantity == exp || /* this allows exp == NULL */
        strcmp(quantity, exp) == 0, "wrong variable attribute value: quantity");

    /* int32 */
    v = fmi3_import_get_variable_by_name(xml, varNames[1]);
    ASSERT_MSG(v != NULL, "variable wasn't found by name");
    quantity = fmi3_import_get_int32_variable_quantity(fmi3_import_get_variable_as_int32(v));
    ASSERT_MSG(quantity == exp || /* this allows exp == NULL */
        strcmp(quantity, exp) == 0, "wrong variable attribute value: quantity");

    /* enum */
    v = fmi3_import_get_variable_by_name(xml, varNames[2]);
    ASSERT_MSG(v != NULL, "variable wasn't found by name");
    quantity = fmi3_import_get_enum_variable_quantity(fmi3_import_get_variable_as_enum(v));
    ASSERT_MSG(quantity == exp || /* this allows exp == NULL */
        strcmp(quantity, exp) == 0, "wrong variable attribute value: quantity");

    return TEST_OK;
}

/* verify vars with defined quantity */
static int test_var_quantity_defined(fmi3_import_t* xml)
{
    const char* varNames[] = { "float64_with_attr", "int32_with_attr", "enum_with_attr" };
    return test_var_quantity(xml, "velocity", varNames);
}

/* verify vars with undefined quantity */
static int test_var_quantity_undefined(fmi3_import_t* xml)
{
    const char* varNames[] = { "float64_no_attr", "int32_no_attr", "enum_no_attr" };
    return test_var_quantity(xml, NULL, varNames);
}

/* verify float64-specific type attributes on variable */
static int test_float64_var_attributes_exist(fmi3_import_t* xml, fmi3_boolean_t expUnbounded, fmi3_boolean_t expRelativeQuantity, fmi3_string_t varName)
{
    fmi3_import_variable_t* v;
    fmi3_import_float64_variable_t* vFloat64;
    fmi3_boolean_t unbounded;
    fmi3_boolean_t relativeQuantity;

    /* float64 */
    v = fmi3_import_get_variable_by_name(xml, varName);
    ASSERT_MSG(v != NULL, "variable wasn't found by name");

    vFloat64 = fmi3_import_get_variable_as_float64(v);
    unbounded = fmi3_import_get_float64_variable_unbounded(vFloat64);
    ASSERT_MSG(unbounded == expUnbounded, "wrong variable attribute value: unbounded");

    relativeQuantity = fmi3_import_get_float64_variable_relative_quantity(vFloat64);
    ASSERT_MSG(relativeQuantity == expRelativeQuantity, "wrong variable attribute value: relativeQuantity");

    return TEST_OK;
}

static int test_float64_var_attributes_defined(fmi3_import_t* xml)
{
    fmi3_boolean_t expUnbounded = fmi3_true;
    fmi3_boolean_t expRelativeQuantity = fmi3_true;

    return test_float64_var_attributes_exist(xml, expUnbounded, expRelativeQuantity, "float64_with_attr");
}

static int test_float64_var_attributes_undefined(fmi3_import_t* xml)
{
    fmi3_boolean_t expUnbounded = fmi3_false;
    fmi3_boolean_t expRelativeQuantity = fmi3_false;

    return test_float64_var_attributes_exist(xml, expUnbounded, expRelativeQuantity, "float64_no_attr");
}

static int test_float64_var_attributes_defined_in_typedef(fmi3_import_t* xml)
{
    fmi3_boolean_t expUnbounded = fmi3_false;
    fmi3_boolean_t expRelativeQuantity = fmi3_true;

    return test_float64_var_attributes_exist(xml, expUnbounded, expRelativeQuantity, "float64_with_typedef");
}

static int test_float64_var_attributes_defined_in_typedef_partially(fmi3_import_t* xml)
{
    fmi3_boolean_t expUnbounded = fmi3_false;
    fmi3_boolean_t expRelativeQuantity = fmi3_true;

    return test_float64_var_attributes_exist(xml, expUnbounded, expRelativeQuantity, "float64_with_typedef_override");
}


/*************************************
 *** Inheritance of min/max values ***
 *************************************/

/* 'Basic' value checkers: verify that default values are inherited from the base type */

static int vcBasicInt64(fmi3_import_int_typedef_t* t) {
    ASSERT_MSG(fmi3_import_get_int64_type_min(t) == INT64_MIN, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_int64_type_max(t) == INT64_MAX, "wrong max for type");

    return TEST_OK;
}

static int vcBasicInt32(fmi3_import_int_typedef_t* t) {
    ASSERT_MSG(fmi3_import_get_int32_type_min(t) == INT32_MIN, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_int32_type_max(t) == INT32_MAX, "wrong max for type");

    return TEST_OK;
}

static int vcBasicInt16(fmi3_import_int_typedef_t* t) {
    ASSERT_MSG(fmi3_import_get_int16_type_min(t) == INT16_MIN, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_int16_type_max(t) == INT16_MAX, "wrong max for type");

    return TEST_OK;
}

static int vcBasicInt8(fmi3_import_int_typedef_t* t) {
    ASSERT_MSG(fmi3_import_get_int8_type_min(t) == INT8_MIN, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_int8_type_max(t) == INT8_MAX, "wrong max for type");

    return TEST_OK;
}

static int vcBasicUInt64(fmi3_import_int_typedef_t* t) {
    ASSERT_MSG(fmi3_import_get_uint64_type_min(t) == UINTXX_MIN, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_uint64_type_max(t) == UINT64_MAX, "wrong max for type");

    return TEST_OK;
}

static int vcBasicUInt32(fmi3_import_int_typedef_t* t) {
    ASSERT_MSG(fmi3_import_get_uint32_type_min(t) == UINTXX_MIN, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_uint32_type_max(t) == UINT32_MAX, "wrong max for type");

    return TEST_OK;
}

static int vcBasicUInt16(fmi3_import_int_typedef_t* t) {
    ASSERT_MSG(fmi3_import_get_uint16_type_min(t) == UINTXX_MIN, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_uint16_type_max(t) == UINT16_MAX, "wrong max for type");

    return TEST_OK;
}

static int vcBasicUInt8(fmi3_import_int_typedef_t* t) {
    ASSERT_MSG(fmi3_import_get_uint8_type_min(t) == UINTXX_MIN, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_uint8_type_max(t) == UINT8_MAX, "wrong max for type");

    return TEST_OK;
}

/* 'Override' value checkers: verify that default values from base type can be overriden by the typedef */

static int vcOverrideInt64(fmi3_import_int_typedef_t* t) {
    ASSERT_MSG(fmi3_import_get_int64_type_min(t) == INT64_MIN + D_TYPE, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_int64_type_max(t) == INT64_MAX - D_TYPE, "wrong max for type");

    return TEST_OK;
}

static int vcOverrideInt32(fmi3_import_int_typedef_t* t) {
    ASSERT_MSG(fmi3_import_get_int32_type_min(t) == INT32_MIN + D_TYPE, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_int32_type_max(t) == INT32_MAX - D_TYPE, "wrong max for type");

    return TEST_OK;
}

static int vcOverrideInt16(fmi3_import_int_typedef_t* t) {
    ASSERT_MSG(fmi3_import_get_int16_type_min(t) == INT16_MIN + D_TYPE, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_int16_type_max(t) == INT16_MAX - D_TYPE, "wrong max for type");

    return TEST_OK;
}

static int vcOverrideInt8(fmi3_import_int_typedef_t* t) {
    ASSERT_MSG(fmi3_import_get_int8_type_min(t) == INT8_MIN + D_TYPE, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_int8_type_max(t) == INT8_MAX - D_TYPE, "wrong max for type");

    return TEST_OK;
}

static int vcOverrideUInt64(fmi3_import_int_typedef_t* t) {
    ASSERT_MSG(fmi3_import_get_uint64_type_min(t) == UINTXX_MIN + D_TYPE, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_uint64_type_max(t) == UINT64_MAX - D_TYPE, "wrong max for type");

    return TEST_OK;
}

static int vcOverrideUInt32(fmi3_import_int_typedef_t* t) {
    ASSERT_MSG(fmi3_import_get_uint32_type_min(t) == UINTXX_MIN + D_TYPE, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_uint32_type_max(t) == UINT32_MAX - D_TYPE, "wrong max for type");

    return TEST_OK;
}

static int vcOverrideUInt16(fmi3_import_int_typedef_t* t) {
    ASSERT_MSG(fmi3_import_get_uint16_type_min(t) == UINTXX_MIN + D_TYPE, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_uint16_type_max(t) == UINT16_MAX - D_TYPE, "wrong max for type");

    return TEST_OK;
}

static int vcOverrideUInt8(fmi3_import_int_typedef_t* t) {
    ASSERT_MSG(fmi3_import_get_uint8_type_min(t) == UINTXX_MIN + D_TYPE, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_uint8_type_max(t) == UINT8_MAX - D_TYPE, "wrong max for type");

    return TEST_OK;
}

/* PartialOverride: min should be overridden, but not max */

static int vcPartialOverrideInt32(fmi3_import_int_typedef_t* t) {
    ASSERT_MSG(fmi3_import_get_uint32_type_min(t) == -1, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_uint32_type_max(t) == INT32_MAX, "wrong max for type");

    return TEST_OK;
}

static int vcPartialOverrideUInt32(fmi3_import_int_typedef_t* t) {
    ASSERT_MSG(fmi3_import_get_uint32_type_min(t) == 1, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_uint32_type_max(t) == UINT32_MAX, "wrong max for type");

    return TEST_OK;
}

/* 'basic' variable checkers: verify that default values are inherited */

static int vcInheritBasicTypedefVariableInt64(fmi3_import_variable_t* v) {
    fmi3_import_int64_variable_t* vt = fmi3_import_get_variable_as_int64(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_int64_variable_min(vt) == INT64_MIN, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_int64_variable_max(vt) == INT64_MAX, "wrong max for type");

    return TEST_OK;
}

static int vcInheritBasicTypedefVariableInt32(fmi3_import_variable_t* v) {
    fmi3_import_int32_variable_t* vt = fmi3_import_get_variable_as_int32(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_int32_variable_min(vt) == INT32_MIN, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_int32_variable_max(vt) == INT32_MAX, "wrong max for type");

    return TEST_OK;
}

static int vcInheritBasicTypedefVariableInt16(fmi3_import_variable_t* v) {
    fmi3_import_int16_variable_t* vt = fmi3_import_get_variable_as_int16(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_int16_variable_min(vt) == INT16_MIN, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_int16_variable_max(vt) == INT16_MAX, "wrong max for type");

    return TEST_OK;
}

static int vcInheritBasicTypedefVariableInt8(fmi3_import_variable_t* v) {
    fmi3_import_int8_variable_t* vt = fmi3_import_get_variable_as_int8(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_int8_variable_min(vt) == INT8_MIN, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_int8_variable_max(vt) == INT8_MAX, "wrong max for type");

    return TEST_OK;
}

static int vcInheritBasicTypedefVariableUInt64(fmi3_import_variable_t* v) {
    fmi3_import_uint64_variable_t* vt = fmi3_import_get_variable_as_uint64(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_uint64_variable_min(vt) == UINTXX_MIN, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_uint64_variable_max(vt) == UINT64_MAX, "wrong max for type");

    return TEST_OK;
}

static int vcInheritBasicTypedefVariableUInt32(fmi3_import_variable_t* v) {
    fmi3_import_uint32_variable_t* vt = fmi3_import_get_variable_as_uint32(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_uint32_variable_min(vt) == UINTXX_MIN, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_uint32_variable_max(vt) == UINT32_MAX, "wrong max for type");

    return TEST_OK;
}

static int vcInheritBasicTypedefVariableUInt16(fmi3_import_variable_t* v) {
    fmi3_import_uint16_variable_t* vt = fmi3_import_get_variable_as_uint16(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_uint16_variable_min(vt) == UINTXX_MIN, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_uint16_variable_max(vt) == UINT16_MAX, "wrong max for type");

    return TEST_OK;
}

static int vcInheritBasicTypedefVariableUInt8(fmi3_import_variable_t* v) {
    fmi3_import_uint8_variable_t* vt = fmi3_import_get_variable_as_uint8(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_uint8_variable_min(vt) == UINTXX_MIN, "wrong min for type");
    ASSERT_MSG(fmi3_import_get_uint8_variable_max(vt) == UINT8_MAX, "wrong max for type");

    return TEST_OK;
}

/* 'InheritOverridingTypedef': verify that variable can inherit typedef that overrides defaults */

static int vcInheritOverridingTypedefVariableInt64(fmi3_import_variable_t* v) {
    fmi3_import_int64_variable_t* vt = fmi3_import_get_variable_as_int64(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_int64_variable_min(vt) == INT64_MIN + D_TYPE, "wrong min for variable");
    ASSERT_MSG(fmi3_import_get_int64_variable_max(vt) == INT64_MAX - D_TYPE, "wrong max for variable");

    return TEST_OK;
}

static int vcInheritOverridingTypedefVariableInt32(fmi3_import_variable_t* v) {
    fmi3_import_int32_variable_t* vt = fmi3_import_get_variable_as_int32(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_int32_variable_min(vt) == INT32_MIN + D_TYPE, "wrong min for variable");
    ASSERT_MSG(fmi3_import_get_int32_variable_max(vt) == INT32_MAX - D_TYPE, "wrong max for variable");

    return TEST_OK;
}

static int vcInheritOverridingTypedefVariableInt16(fmi3_import_variable_t* v) {
    fmi3_import_int16_variable_t* vt = fmi3_import_get_variable_as_int16(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_int16_variable_min(vt) == INT16_MIN + D_TYPE, "wrong min for variable");
    ASSERT_MSG(fmi3_import_get_int16_variable_max(vt) == INT16_MAX - D_TYPE, "wrong max for variable");

    return TEST_OK;
}

static int vcInheritOverridingTypedefVariableInt8(fmi3_import_variable_t* v) {
    fmi3_import_int8_variable_t* vt = fmi3_import_get_variable_as_int8(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_int8_variable_min(vt) == INT8_MIN + D_TYPE, "wrong min for variable");
    ASSERT_MSG(fmi3_import_get_int8_variable_max(vt) == INT8_MAX - D_TYPE, "wrong max for variable");

    return TEST_OK;
}

static int vcInheritOverridingTypedefVariableUInt64(fmi3_import_variable_t* v) {
    fmi3_import_uint64_variable_t* vt = fmi3_import_get_variable_as_uint64(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_uint64_variable_min(vt) == UINTXX_MIN + D_TYPE, "wrong min for variable");
    ASSERT_MSG(fmi3_import_get_uint64_variable_max(vt) == UINT64_MAX - D_TYPE, "wrong max for variable");

    return TEST_OK;
}

static int vcInheritOverridingTypedefVariableUInt32(fmi3_import_variable_t* v) {
    fmi3_import_uint32_variable_t* vt = fmi3_import_get_variable_as_uint32(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_uint32_variable_min(vt) == UINTXX_MIN + D_TYPE, "wrong min for variable");
    ASSERT_MSG(fmi3_import_get_uint32_variable_max(vt) == UINT32_MAX - D_TYPE, "wrong max for variable");

    return TEST_OK;
}

static int vcInheritOverridingTypedefVariableUInt16(fmi3_import_variable_t* v) {
    fmi3_import_uint16_variable_t* vt = fmi3_import_get_variable_as_uint16(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_uint16_variable_min(vt) == UINTXX_MIN + D_TYPE, "wrong min for variable");
    ASSERT_MSG(fmi3_import_get_uint16_variable_max(vt) == UINT16_MAX - D_TYPE, "wrong max for variable");

    return TEST_OK;
}

static int vcInheritOverridingTypedefVariableUInt8(fmi3_import_variable_t* v) {
    fmi3_import_uint8_variable_t* vt = fmi3_import_get_variable_as_uint8(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_uint8_variable_min(vt) == UINTXX_MIN + D_TYPE, "wrong min for variable");
    ASSERT_MSG(fmi3_import_get_uint8_variable_max(vt) == UINT8_MAX - D_TYPE, "wrong max for variable");

    return TEST_OK;
}

/* 'Override' variable checkers: verify that values in variable can override defaults/typedef */

static int vcOverrideVariableInt64(fmi3_import_variable_t* v) {
    fmi3_import_int64_variable_t* vt = fmi3_import_get_variable_as_int64(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_int64_variable_min(vt) == INT64_MIN + D_VAR, "wrong min for variable");
    ASSERT_MSG(fmi3_import_get_int64_variable_max(vt) == INT64_MAX - D_VAR, "wrong max for variable");

    return TEST_OK;
}

static int vcOverrideVariableInt32(fmi3_import_variable_t* v) {
    fmi3_import_int32_variable_t* vt = fmi3_import_get_variable_as_int32(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_int32_variable_min(vt) == INT32_MIN + D_VAR, "wrong min for variable");
    ASSERT_MSG(fmi3_import_get_int32_variable_max(vt) == INT32_MAX - D_VAR, "wrong max for variable");

    return TEST_OK;
}

static int vcOverrideVariableInt16(fmi3_import_variable_t* v) {
    fmi3_import_int16_variable_t* vt = fmi3_import_get_variable_as_int16(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_int16_variable_min(vt) == INT16_MIN + D_VAR, "wrong min for variable");
    ASSERT_MSG(fmi3_import_get_int16_variable_max(vt) == INT16_MAX - D_VAR, "wrong max for variable");

    return TEST_OK;
}

static int vcOverrideVariableInt8(fmi3_import_variable_t* v) {
    fmi3_import_int8_variable_t* vt = fmi3_import_get_variable_as_int8(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_int8_variable_min(vt) == INT8_MIN + D_VAR, "wrong min for variable");
    ASSERT_MSG(fmi3_import_get_int8_variable_max(vt) == INT8_MAX - D_VAR, "wrong max for variable");

    return TEST_OK;
}

static int vcOverrideVariableUInt64(fmi3_import_variable_t* v) {
    fmi3_import_uint64_variable_t* vt = fmi3_import_get_variable_as_uint64(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_uint64_variable_min(vt) == UINTXX_MIN + D_VAR, "wrong min for variable");
    ASSERT_MSG(fmi3_import_get_uint64_variable_max(vt) == UINT64_MAX - D_VAR, "wrong max for variable");

    return TEST_OK;
}

static int vcOverrideVariableUInt32(fmi3_import_variable_t* v) {
    fmi3_import_uint32_variable_t* vt = fmi3_import_get_variable_as_uint32(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_uint32_variable_min(vt) == UINTXX_MIN + D_VAR, "wrong min for variable");
    ASSERT_MSG(fmi3_import_get_uint32_variable_max(vt) == UINT32_MAX - D_VAR, "wrong max for variable");

    return TEST_OK;
}

static int vcOverrideVariableUInt16(fmi3_import_variable_t* v) {
    fmi3_import_uint16_variable_t* vt = fmi3_import_get_variable_as_uint16(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_uint16_variable_min(vt) == UINTXX_MIN + D_VAR, "wrong min for variable");
    ASSERT_MSG(fmi3_import_get_uint16_variable_max(vt) == UINT16_MAX - D_VAR, "wrong max for variable");

    return TEST_OK;
}

static int vcOverrideVariableUInt8(fmi3_import_variable_t* v) {
    fmi3_import_uint8_variable_t* vt = fmi3_import_get_variable_as_uint8(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_uint8_variable_min(vt) == UINTXX_MIN + D_VAR, "wrong min for variable");
    ASSERT_MSG(fmi3_import_get_uint8_variable_max(vt) == UINT8_MAX - D_VAR, "wrong max for variable");

    return TEST_OK;
}

/* 'partial override' value checkers: partial overrides of values, i.e. some are inherited others overriden */

static int vcPartialOverrideTypedefVariableInt32(fmi3_import_variable_t* v) {
    fmi3_import_int32_variable_t* vt = fmi3_import_get_variable_as_int32(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_int32_variable_min(vt) == INT32_MIN + D_VAR, "wrong min for variable");
    ASSERT_MSG(fmi3_import_get_int32_variable_max(vt) == INT32_MAX, "wrong max for variable");

    return TEST_OK;
}

static int vcPartialOverrideDefaultVariableInt32(fmi3_import_variable_t* v) {
    fmi3_import_int32_variable_t* vt = fmi3_import_get_variable_as_int32(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_int32_variable_min(vt) == INT32_MIN, "wrong min for variable");
    ASSERT_MSG(fmi3_import_get_int32_variable_max(vt) == INT32_MAX - D_VAR, "wrong max for variable");

    return TEST_OK;
}

/* 'default start' value checkers: that default start values are set */

static int vcDefaultStartVariableInt64(fmi3_import_variable_t* v) {
    fmi3_import_int64_variable_t* vt = fmi3_import_get_variable_as_int64(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_int64_variable_start(vt) == 0, "wrong start for variable");

    return TEST_OK;
}

static int vcDefaultStartVariableInt32(fmi3_import_variable_t* v) {
    fmi3_import_int32_variable_t* vt = fmi3_import_get_variable_as_int32(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_int32_variable_start(vt) == 0, "wrong start for variable");

    return TEST_OK;
}

static int vcDefaultStartVariableInt16(fmi3_import_variable_t* v) {
    fmi3_import_int16_variable_t* vt = fmi3_import_get_variable_as_int16(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_int16_variable_start(vt) == 0, "wrong start for variable");

    return TEST_OK;
}

static int vcDefaultStartVariableInt8(fmi3_import_variable_t* v) {
    fmi3_import_int8_variable_t* vt = fmi3_import_get_variable_as_int8(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_int8_variable_start(vt) == 0, "wrong start for variable");

    return TEST_OK;
}

static int vcDefaultStartVariableUInt64(fmi3_import_variable_t* v) {
    fmi3_import_uint64_variable_t* vt = fmi3_import_get_variable_as_uint64(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_uint64_variable_start(vt) == 0, "wrong start for variable");

    return TEST_OK;
}

static int vcDefaultStartVariableUInt32(fmi3_import_variable_t* v) {
    fmi3_import_uint32_variable_t* vt = fmi3_import_get_variable_as_uint32(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_uint32_variable_start(vt) == 0, "wrong start for variable");

    return TEST_OK;
}

static int vcDefaultStartVariableUInt16(fmi3_import_variable_t* v) {
    fmi3_import_uint16_variable_t* vt = fmi3_import_get_variable_as_uint16(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_uint16_variable_start(vt) == 0, "wrong start for variable");

    return TEST_OK;
}

static int vcDefaultStartVariableUInt8(fmi3_import_variable_t* v) {
    fmi3_import_uint8_variable_t* vt = fmi3_import_get_variable_as_uint8(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_uint8_variable_start(vt) == 0, "wrong start for variable");

    return TEST_OK;
}

/* 'with start specified' value checkers: verifies that start values are read as specified in XML */

static int vcWithStartVariableInt64(fmi3_import_variable_t* v) {
    fmi3_import_int64_variable_t* vt = fmi3_import_get_variable_as_int64(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_int64_variable_start(vt) == INT64_MAX - D_START, "wrong start for variable");

    return TEST_OK;
}

static int vcWithStartVariableInt32(fmi3_import_variable_t* v) {
    fmi3_import_int32_variable_t* vt = fmi3_import_get_variable_as_int32(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_int32_variable_start(vt) == INT32_MAX - D_START, "wrong start for variable");

    return TEST_OK;
}

static int vcWithStartVariableInt16(fmi3_import_variable_t* v) {
    fmi3_import_int16_variable_t* vt = fmi3_import_get_variable_as_int16(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_int16_variable_start(vt) == INT16_MAX - D_START, "wrong start for variable");

    return TEST_OK;
}

static int vcWithStartVariableInt8(fmi3_import_variable_t* v) {
    fmi3_import_int8_variable_t* vt = fmi3_import_get_variable_as_int8(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_int8_variable_start(vt) == INT8_MAX - D_START, "wrong start for variable");

    return TEST_OK;
}

static int vcWithStartVariableUInt64(fmi3_import_variable_t* v) {
    fmi3_import_uint64_variable_t* vt = fmi3_import_get_variable_as_uint64(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_uint64_variable_start(vt) == UINT64_MAX - D_START, "wrong start for variable");

    return TEST_OK;
}

static int vcWithStartVariableUInt32(fmi3_import_variable_t* v) {
    fmi3_import_uint32_variable_t* vt = fmi3_import_get_variable_as_uint32(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_uint32_variable_start(vt) == UINT32_MAX - D_START, "wrong start for variable");

    return TEST_OK;
}

static int vcWithStartVariableUInt16(fmi3_import_variable_t* v) {
    fmi3_import_uint16_variable_t* vt = fmi3_import_get_variable_as_uint16(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_uint16_variable_start(vt) == UINT16_MAX - D_START, "wrong start for variable");

    return TEST_OK;
}

static int vcWithStartVariableUInt8(fmi3_import_variable_t* v) {
    fmi3_import_uint8_variable_t* vt = fmi3_import_get_variable_as_uint8(v);
    ASSERT_MSG(vt != NULL, "failed to convert to specific variable type");
    ASSERT_MSG(fmi3_import_get_uint8_variable_start(vt) == UINT8_MAX - D_START, "wrong start for variable");

    return TEST_OK;
}

/* return value: exit code */
static int get_typedef_by_name(fmi3_import_type_definitions_t* tds, fmi2_string_t name, fmi3_import_variable_typedef_t **t) {
    unsigned int nTds;
    fmi3_import_variable_typedef_t *td;
    unsigned int i;

    nTds = fmi3_import_get_type_definition_number(tds);
    for (i = 0; i < nTds; i++) {
        td = fmi3_import_get_typedef(tds, i);

        if (strcmp(fmi3_import_get_type_name(td), name) == 0) {
            *t = td;
            return TEST_OK;
        }
    }

    TEST_FAILED("typedef wasn't found by name");
}

/* parse small intXX typedef */
/* @vcIntXX performs all testing specific to the numeric type. vc: valueChecker */
static int test_typedef_intXX(fmi3_import_t *xml, fmi2_string_t typeName, int (*vcIntXX)(fmi3_import_int_typedef_t*))
{
    fmi3_import_variable_typedef_t *t;
    fmi3_import_int_typedef_t* intType;
    fmi3_import_type_definitions_t* tds;
    int res; /* result from test-function */

    tds = fmi3_import_get_type_definitions(xml);
    ASSERT_MSG(tds != NULL, "could not get type definitions");

    res = get_typedef_by_name(tds, typeName, &t);
    if (res != TEST_OK) return res;

    ASSERT_MSG(strcmp(fmi3_import_get_type_name(t), typeName) == 0, "wrong type name");
    /* testing default case: none specified in MD */
    ASSERT_MSG(fmi3_import_get_type_quantity(t) == NULL, "wrong quantity in type definition"); 

    intType = fmi3_import_get_type_as_int(t);
    ASSERT_MSG(intType != NULL, "failed to convert to intXX type definition");

    res = vcIntXX(intType);
    if (res != TEST_OK) return res;

    return TEST_OK;
}

static int test_variable_intXX(fmi3_import_t *xml, fmi2_string_t varName,
        int (*vcVariableIntXX)(fmi3_import_variable_t* v))
{
    fmi3_import_variable_t* v;
    int res; /* result from test-function */

    v = fmi3_import_get_variable_by_name(xml, varName);
    ASSERT_MSG(v != NULL, "could not find variable to test");

    /* specific testing for variable's type */
    res = vcVariableIntXX(v);
    if (res != TEST_OK) return res;

    return TEST_OK;
}

static int test_variable_all_attributes_int32(fmi3_import_t *xml, fmi2_string_t varName)
{
    fmi3_import_variable_t* v;
    fmi3_import_variable_typedef_t* t;

    v = fmi3_import_get_variable_by_name(xml, varName);
    ASSERT_MSG(v != NULL, "could not find variable to test");

    ASSERT_MSG(strcmp("description", fmi3_import_get_variable_description(v)) == 0, "incorrect attribute: description");
    /* TODO: it feels a bit odd that 'fmi3_import_get_causality' doesn't include _variable_ in the name - was a bit hard to find... */
    ASSERT_MSG(fmi3_import_get_causality(v) == fmi3_causality_enu_output, "incorrect attribute: causality");
    ASSERT_MSG(fmi3_import_get_variability(v) == fmi3_variability_enu_discrete, "incorrect attribute: variability");
    ASSERT_MSG(fmi3_import_get_initial(v) == fmi3_initial_enu_calculated, "incorrect attribute: initial");
    /* TODO: quantity getter is missing - this will be fixed when FMIL 2.2 fixes are applied */

    t = fmi3_import_get_variable_declared_type(v);
    ASSERT_MSG(t != NULL, "failed to get variable typedef");
    ASSERT_MSG(strcmp("typedef_basic_int32", fmi3_import_get_type_name(t)) == 0, "incorrect attribute: name");

    return TEST_OK;
}

static void jm_logger_count_errors(jm_callbacks* cb, jm_string module, jm_log_level_enu_t log_level, jm_string message) {
    jm_default_logger(cb, module, log_level, message);
    if (log_level == jm_log_level_error) {
        G_nErrors++;
    }
}

static void shallow_copy_jm_callbacks(jm_callbacks* cb_from, jm_callbacks* cb_to) {
    cb_to->calloc            =  cb_from->calloc;
    cb_to->context           =  cb_from->context;
    cb_to->free              =  cb_from->free;
    cb_to->logger            =  cb_from->logger;
    cb_to->log_level         =  cb_from->log_level;
    cb_to->malloc            =  cb_from->malloc;
    cb_to->realloc           =  cb_from->realloc;
    *cb_to->errMessageBuffer = *cb_from->errMessageBuffer;
}

/* test that we get correct number of errors */
static int test_value_boundary_check(const char* xmlPath) {
    fmi3_import_t* xml;
    jm_callbacks cb;
    fmi_import_context_t* ctx;
    int res = TEST_OK;

    /* update these if you change xml: */
    int nBadTds = 20;
    int nBadVars = 21;

    /* create shallow copy, but change logger so we can count errors */
    shallow_copy_jm_callbacks(jm_get_default_callbacks(), &cb);
    cb.logger = jm_logger_count_errors;

    ctx = fmi_import_allocate_context(&cb);
    if (ctx == NULL) {
        return TEST_FAIL;
    }

    /* parse the XML */
    G_nErrors = 0;
    xml = fmi3_import_parse_xml(ctx, xmlPath, NULL);
    fmi_import_free_context(ctx);
    if (xml == NULL) {
        return TEST_FAIL;
    }

    /* check num logger error reports */
    if (G_nErrors != 2 * (nBadTds + nBadVars)) { /* x2 because we currently log more than once for each ... */
        printf("  ERROR: unexpected number of parsing errors\n");
        res = TEST_FAIL;
        goto err1;
    }

err1:
    fmi3_import_free(xml);

    return res;
}

static void build_xml_path(char* buf, size_t bufSize, const char* basePath, const char* appendPath) {
    strncpy(buf, basePath,   bufSize);
    strncat(buf, appendPath, bufSize);
}

int main(int argc, char **argv)
{
    char xmlPath[1000];
    size_t sizeXmlPath = sizeof(xmlPath) / sizeof(char);
    fmi3_import_t *xml;
    int ret = 1;

    if (argc != 2) {
        printf("Usage: %s <Test/FMI3/parser_test_xmls path>\n", argv[0]);
        return CTEST_RETURN_FAIL;
    }

    build_xml_path(xmlPath, sizeXmlPath, argv[1], "/type_definitions/valid");

    printf("Running fmi3_import_variable_types_test\n");

    xml = parse_xml(xmlPath);
    if (xml == NULL) {
        return CTEST_RETURN_FAIL;
    }

    ret &= test_type1(xml);
    ret &= test_type2(xml);


    /******************************
     *** Non-numeric attributes ***
     ******************************/

    /* typedefs */
    ret &= test_quantity_default(xml);

    /* variable */
    ret &= test_var_quantity_defined(xml);
    ret &= test_var_quantity_undefined(xml);
    ret &= test_float64_var_attributes_defined(xml);
    ret &= test_float64_var_attributes_undefined(xml);
    ret &= test_float64_var_attributes_defined_in_typedef(xml);
    ret &= test_float64_var_attributes_defined_in_typedef_partially(xml);


    /*************************************
     *** Inheritance of min/max values ***
     *************************************/

    /* --- typedefs --- */
    ret &= test_typedef_intXX(xml, "typedef_basic_int64", vcBasicInt64);
    ret &= test_typedef_intXX(xml, "typedef_basic_int32", vcBasicInt32);
    ret &= test_typedef_intXX(xml, "typedef_basic_int16", vcBasicInt16);
    ret &= test_typedef_intXX(xml, "typedef_basic_int8",  vcBasicInt8);
    ret &= test_typedef_intXX(xml, "typedef_basic_uint64", vcBasicUInt64);
    ret &= test_typedef_intXX(xml, "typedef_basic_uint32", vcBasicUInt32);
    ret &= test_typedef_intXX(xml, "typedef_basic_uint16", vcBasicUInt16);
    ret &= test_typedef_intXX(xml, "typedef_basic_uint8",  vcBasicUInt8);

    /* full override (min and max) */
    ret &= test_typedef_intXX(xml, "typedef_override_int64", vcOverrideInt64);
    ret &= test_typedef_intXX(xml, "typedef_override_int32", vcOverrideInt32);
    ret &= test_typedef_intXX(xml, "typedef_override_int16", vcOverrideInt16);
    ret &= test_typedef_intXX(xml, "typedef_override_int8",  vcOverrideInt8);
    ret &= test_typedef_intXX(xml, "typedef_override_uint64", vcOverrideUInt64);
    ret &= test_typedef_intXX(xml, "typedef_override_uint32", vcOverrideUInt32);
    ret &= test_typedef_intXX(xml, "typedef_override_uint16", vcOverrideUInt16);
    ret &= test_typedef_intXX(xml, "typedef_override_uint8",  vcOverrideUInt8);

    /* partial override (only min) */
    ret &= test_typedef_intXX(xml, "typedef_partial_override_int32", vcPartialOverrideInt32); /* only testing for int32 - should be enough */
    ret &= test_typedef_intXX(xml, "typedef_partial_override_uint32", vcPartialOverrideUInt32); /* only testing for uint32 - should be enough */

    /* verify that leading '+' is valid */
    ret &= test_typedef_intXX(xml, "typedef_override_leadingplus_int64", vcOverrideInt64);

    /* --- variables ---  */

    /* no typedef: inherit default values */
    ret &= test_variable_intXX(xml, "variable_minimal_int64", vcInheritBasicTypedefVariableInt64);
    ret &= test_variable_intXX(xml, "variable_minimal_int32", vcInheritBasicTypedefVariableInt32);
    ret &= test_variable_intXX(xml, "variable_minimal_int16", vcInheritBasicTypedefVariableInt16);
    ret &= test_variable_intXX(xml, "variable_minimal_int8",  vcInheritBasicTypedefVariableInt8);
    ret &= test_variable_intXX(xml, "variable_minimal_uint64", vcInheritBasicTypedefVariableUInt64);
    ret &= test_variable_intXX(xml, "variable_minimal_uint32", vcInheritBasicTypedefVariableUInt32);
    ret &= test_variable_intXX(xml, "variable_minimal_uint16", vcInheritBasicTypedefVariableUInt16);
    ret &= test_variable_intXX(xml, "variable_minimal_uint8",  vcInheritBasicTypedefVariableUInt8);

    /* typedef: inherit typedef that inherits defaults */
    /*     re-using the same value checker as for basic variables, since it should be the default value */
    ret &= test_variable_intXX(xml, "variable_inherit_typedef_basic_int64", vcInheritBasicTypedefVariableInt64);
    ret &= test_variable_intXX(xml, "variable_inherit_typedef_basic_int32", vcInheritBasicTypedefVariableInt32);
    ret &= test_variable_intXX(xml, "variable_inherit_typedef_basic_int16", vcInheritBasicTypedefVariableInt16);
    ret &= test_variable_intXX(xml, "variable_inherit_typedef_basic_int8",  vcInheritBasicTypedefVariableInt8);
    ret &= test_variable_intXX(xml, "variable_inherit_typedef_basic_uint64", vcInheritBasicTypedefVariableUInt64);
    ret &= test_variable_intXX(xml, "variable_inherit_typedef_basic_uint32", vcInheritBasicTypedefVariableUInt32);
    ret &= test_variable_intXX(xml, "variable_inherit_typedef_basic_uint16", vcInheritBasicTypedefVariableUInt16);
    ret &= test_variable_intXX(xml, "variable_inherit_typedef_basic_uint8",  vcInheritBasicTypedefVariableUInt8);

    /* inherit typedef that overrides the default values */
    ret &= test_variable_intXX(xml, "variable_inherit_typedef_override_int64", vcInheritOverridingTypedefVariableInt64);
    ret &= test_variable_intXX(xml, "variable_inherit_typedef_override_int32", vcInheritOverridingTypedefVariableInt32);
    ret &= test_variable_intXX(xml, "variable_inherit_typedef_override_int16", vcInheritOverridingTypedefVariableInt16);
    ret &= test_variable_intXX(xml, "variable_inherit_typedef_override_int8",  vcInheritOverridingTypedefVariableInt8);
    ret &= test_variable_intXX(xml, "variable_inherit_typedef_override_uint64", vcInheritOverridingTypedefVariableUInt64);
    ret &= test_variable_intXX(xml, "variable_inherit_typedef_override_uint32", vcInheritOverridingTypedefVariableUInt32);
    ret &= test_variable_intXX(xml, "variable_inherit_typedef_override_uint16", vcInheritOverridingTypedefVariableUInt16);
    ret &= test_variable_intXX(xml, "variable_inherit_typedef_override_uint8",  vcInheritOverridingTypedefVariableUInt8);

    /* let variable override defaults */
    ret &= test_variable_intXX(xml, "variable_override_notypedef_int64", vcOverrideVariableInt64);
    ret &= test_variable_intXX(xml, "variable_override_notypedef_int32", vcOverrideVariableInt32);
    ret &= test_variable_intXX(xml, "variable_override_notypedef_int16", vcOverrideVariableInt16);
    ret &= test_variable_intXX(xml, "variable_override_notypedef_int8",  vcOverrideVariableInt8);
    ret &= test_variable_intXX(xml, "variable_override_notypedef_uint64", vcOverrideVariableUInt64);
    ret &= test_variable_intXX(xml, "variable_override_notypedef_uint32", vcOverrideVariableUInt32);
    ret &= test_variable_intXX(xml, "variable_override_notypedef_uint16", vcOverrideVariableUInt16);
    ret &= test_variable_intXX(xml, "variable_override_notypedef_uint8",  vcOverrideVariableUInt8);

    /* let variable override basic typedef (reusing same value checker) */
    ret &= test_variable_intXX(xml, "variable_override_basic_int64", vcOverrideVariableInt64);
    ret &= test_variable_intXX(xml, "variable_override_basic_int32", vcOverrideVariableInt32);
    ret &= test_variable_intXX(xml, "variable_override_basic_int16", vcOverrideVariableInt16);
    ret &= test_variable_intXX(xml, "variable_override_basic_int8",  vcOverrideVariableInt8);
    ret &= test_variable_intXX(xml, "variable_override_basic_uint64", vcOverrideVariableUInt64);
    ret &= test_variable_intXX(xml, "variable_override_basic_uint32", vcOverrideVariableUInt32);
    ret &= test_variable_intXX(xml, "variable_override_basic_uint16", vcOverrideVariableUInt16);
    ret &= test_variable_intXX(xml, "variable_override_basic_uint8",  vcOverrideVariableUInt8);

    /* let variable override typedef with values (reusing same value checker) */
    ret &= test_variable_intXX(xml, "variable_override_typedef_int64", vcOverrideVariableInt64);
    ret &= test_variable_intXX(xml, "variable_override_typedef_int32", vcOverrideVariableInt32);
    ret &= test_variable_intXX(xml, "variable_override_typedef_int16", vcOverrideVariableInt16);
    ret &= test_variable_intXX(xml, "variable_override_typedef_int8",  vcOverrideVariableInt8);
    ret &= test_variable_intXX(xml, "variable_override_typedef_uint64", vcOverrideVariableUInt64);
    ret &= test_variable_intXX(xml, "variable_override_typedef_uint32", vcOverrideVariableUInt32);
    ret &= test_variable_intXX(xml, "variable_override_typedef_uint16", vcOverrideVariableUInt16);
    ret &= test_variable_intXX(xml, "variable_override_typedef_uint8",  vcOverrideVariableUInt8);

    /* partial override of either typedef'ed or default values */
    ret &= test_variable_intXX(xml, "variable_partial_override_typedef_int32", vcPartialOverrideTypedefVariableInt32);
    ret &= test_variable_intXX(xml, "variable_partial_override_default_int32", vcPartialOverrideDefaultVariableInt32);


    /* --- variable start values ---*/

    /* default start */
    ret &= test_variable_intXX(xml, "variable_minimal_int64", vcDefaultStartVariableInt64);
    ret &= test_variable_intXX(xml, "variable_minimal_int32", vcDefaultStartVariableInt32);
    ret &= test_variable_intXX(xml, "variable_minimal_int16", vcDefaultStartVariableInt16);
    ret &= test_variable_intXX(xml, "variable_minimal_int8",  vcDefaultStartVariableInt8);
    ret &= test_variable_intXX(xml, "variable_minimal_uint64", vcDefaultStartVariableUInt64);
    ret &= test_variable_intXX(xml, "variable_minimal_uint32", vcDefaultStartVariableUInt32);
    ret &= test_variable_intXX(xml, "variable_minimal_uint16", vcDefaultStartVariableUInt16);
    ret &= test_variable_intXX(xml, "variable_minimal_uint8",  vcDefaultStartVariableUInt8);

    /* with start specified */
    ret &= test_variable_intXX(xml, "variable_with_start_int64", vcWithStartVariableInt64);
    ret &= test_variable_intXX(xml, "variable_with_start_int32", vcWithStartVariableInt32);
    ret &= test_variable_intXX(xml, "variable_with_start_int16", vcWithStartVariableInt16);
    ret &= test_variable_intXX(xml, "variable_with_start_int8",  vcWithStartVariableInt8);
    ret &= test_variable_intXX(xml, "variable_with_start_uint64", vcWithStartVariableUInt64);
    ret &= test_variable_intXX(xml, "variable_with_start_uint32", vcWithStartVariableUInt32);
    ret &= test_variable_intXX(xml, "variable_with_start_uint16", vcWithStartVariableUInt16);
    ret &= test_variable_intXX(xml, "variable_with_start_uint8",  vcWithStartVariableUInt8);


    /* --- variable with common attributes --- */
    ret &= test_variable_all_attributes_int32(xml, "variable_common_attributes_int32");

    fmi3_import_free(xml);


    /* --- value boundary check tests --- */

    printf("\nThe next tests are expected to fail...\n");
    printf("---------------------------------------------------------------------------\n");
    {
        build_xml_path(xmlPath, sizeXmlPath, argv[1], "/type_definitions/value_boundary_check/");
        ret &= test_value_boundary_check(xmlPath);
    }
    printf("---------------------------------------------------------------------------\n");

    return ret == 0 ? CTEST_RETURN_FAIL : CTEST_RETURN_SUCCESS;
}

#undef UINTXX_MIN
