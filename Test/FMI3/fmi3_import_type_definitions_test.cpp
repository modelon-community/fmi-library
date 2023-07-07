/*
    Copyright (C) 2023 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#include <stdio.h>
#include <float.h>

#include <fmilib.h>
#include "config_test.h"
#include "fmi_testutil.h"

#include "catch.hpp"

#define UINTXX_MIN (0)

/* delta offsets from extreme values */
#define D_TYPE  (1)  /* for types when overriding defaults */
#define D_VAR   (2)  /* for variables when overriding defaults/typedefs */
#define D_START (3)  /* for variable start values when overriding defaults */

static int G_nErrors; /* global used to count the number of errors reported to the logger during parsing - reset before use! */

static fmi3_import_variable_typedef_t* get_typedef_by_name(fmi3_import_type_definition_list_t* tds, const char* name) {
    size_t nTds = fmi3_import_get_type_definition_list_size(tds);
    fmi3_import_variable_typedef_t *td;
    for (size_t i = 0; i < nTds; i++) {
        td = fmi3_import_get_typedef(tds, i);
        const char* tdName = fmi3_import_get_type_name(td);
        if (strcmp(tdName, name) == 0) {
            return td;
        }
    }
    return nullptr;
}

static fmi3_import_binary_variable_t* get_binary_var_by_name(fmi3_import_t* xml, const char* name) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, name);
    REQUIRE(v != nullptr);
    fmi3_import_binary_variable_t* bv = fmi3_import_get_variable_as_binary(v);
    REQUIRE(bv != nullptr);
    return bv;
}

/* Parse small Float64 typedef */
static int test_type1(fmi3_import_t *xml)
{
    fmi3_import_variable_t *v = fmi3_import_get_variable_by_name(xml, "var.name1");
    fmi3_import_variable_typedef_t *t;
    fmi3_import_float_typedef_t *type;

    REQUIRE(v != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(v) == 1073741824);

    t = fmi3_import_get_variable_declared_type(v);
    REQUIRE(t != nullptr);

    REQUIRE(strcmp(fmi3_import_get_type_name(t), "Float64.name") == 0);
    REQUIRE(strcmp(fmi3_import_get_type_quantity(t), "typeQuantity") == 0);

    type = fmi3_import_get_type_as_float(t);
    REQUIRE(type != nullptr);
    REQUIRE(fmi3_import_get_float64_type_max(type) == DBL_MAX);        /* default */
    REQUIRE(fmi3_import_get_float64_type_min(type) == 0.0);            /* from md */
    REQUIRE(fmi3_import_get_float64_type_nominal(type) == 1.0);        /* default */
    REQUIRE(fmi3_import_get_float64_type_is_relative_quantity(type) == fmi3_false);
    REQUIRE(fmi3_import_get_float64_type_is_unbounded(type) == fmi3_false);

    return CTEST_RETURN_SUCCESS;
}

/* Parse small Float32 typedef */
static int test_type2(fmi3_import_t *xml)
{
    fmi3_import_variable_t *v = fmi3_import_get_variable_by_name(xml, "var.name2");
    fmi3_import_variable_typedef_t *t;
    fmi3_import_float_typedef_t *type;

    REQUIRE(v != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(v) == 805306369);

    t = fmi3_import_get_variable_declared_type(v);
    REQUIRE(t != nullptr);

    REQUIRE(strcmp(fmi3_import_get_type_name(t), "Float32_name") == 0);
    REQUIRE(strcmp(fmi3_import_get_type_quantity(t), "type_quantity") == 0);

    type = fmi3_import_get_type_as_float(t);
    REQUIRE(type != nullptr);
    REQUIRE(fmi3_import_get_float32_type_max(type) == 1000.0f);        /* from md */
    REQUIRE(fmi3_import_get_float32_type_min(type) == -FLT_MAX);       /* default */
    REQUIRE(fmi3_import_get_float32_type_nominal(type) == 5.0f);       /* from md */
    REQUIRE(fmi3_import_get_float32_type_is_relative_quantity(type) == fmi3_false);
    REQUIRE(fmi3_import_get_float32_type_is_unbounded(type) == fmi3_false);

    return CTEST_RETURN_SUCCESS;
}


/******************************
 *** Non-numeric attributes ***
 ******************************/

 /* verify correct default values when quantity is not specified */
static int test_quantity_default(fmi3_import_t* xml)
{
    fmi3_import_variable_typedef_t* td;
    fmi3_import_type_definition_list_t* tds;
    size_t n_tds;
    unsigned int i; /* td_idx */
    const char* tname;
    int nBaseTypes = 13; /* intXX: 8, floatXX: 2, string, boolean, enum */
    int nTdsTested = 0;
    const char* tdPrefix = "td_minimal_";
    
    tds = fmi3_import_get_type_definition_list(xml);
    REQUIRE(tds != nullptr);
    n_tds = fmi3_import_get_type_definition_list_size(tds);

    /* check that nullptr is returned for all types */
    for (i = 0; i < n_tds; i++) {
        td = fmi3_import_get_typedef(tds, i);
        tname = fmi3_import_get_type_name(td);
        if (strncmp(tname, tdPrefix, strlen(tdPrefix)) == 0) { /* only test the minimal tds, (test XML might contain others) */
            REQUIRE(fmi3_import_get_type_quantity(td) == nullptr);
            nTdsTested++;
        }
    }

    REQUIRE(nTdsTested == nBaseTypes);

    return CTEST_RETURN_SUCCESS;
}

/* verify quantity on variable, same for all variables */
static int test_var_quantity(fmi3_import_t* xml, fmi3_string_t exp, const char* varNames[])
{
    fmi3_import_variable_t* v;
    fmi3_string_t quantity;

    /* float64 */
    v = fmi3_import_get_variable_by_name(xml, varNames[0]);
    REQUIRE(v != nullptr);
    quantity = fmi3_import_get_float64_variable_quantity(fmi3_import_get_variable_as_float64(v));
    REQUIRE((quantity == exp || /* this allows exp == nullptr */
            strcmp(quantity, exp) == 0));

    /* int32 */
    v = fmi3_import_get_variable_by_name(xml, varNames[1]);
    REQUIRE(v != nullptr);
    quantity = fmi3_import_get_int32_variable_quantity(fmi3_import_get_variable_as_int32(v));
    REQUIRE((quantity == exp || /* this allows exp == nullptr */
        strcmp(quantity, exp) == 0));

    /* enum */
    v = fmi3_import_get_variable_by_name(xml, varNames[2]);
    REQUIRE(v != nullptr);
    quantity = fmi3_import_get_enum_variable_quantity(fmi3_import_get_variable_as_enum(v));
    REQUIRE((quantity == exp || /* this allows exp == nullptr */
        strcmp(quantity, exp) == 0));

    return CTEST_RETURN_SUCCESS;
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
    return test_var_quantity(xml, nullptr, varNames);
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
    REQUIRE(v != nullptr);

    vFloat64 = fmi3_import_get_variable_as_float64(v);
    unbounded = fmi3_import_get_float64_variable_unbounded(vFloat64);
    REQUIRE(unbounded == expUnbounded);

    relativeQuantity = fmi3_import_get_float64_variable_relative_quantity(vFloat64);
    REQUIRE(relativeQuantity == expRelativeQuantity);

    return CTEST_RETURN_SUCCESS;
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
    REQUIRE(fmi3_import_get_int64_type_min(t) == INT64_MIN);
    REQUIRE(fmi3_import_get_int64_type_max(t) == INT64_MAX);

    return CTEST_RETURN_SUCCESS;
}

static int vcBasicInt32(fmi3_import_int_typedef_t* t) {
    REQUIRE(fmi3_import_get_int32_type_min(t) == INT32_MIN);
    REQUIRE(fmi3_import_get_int32_type_max(t) == INT32_MAX);

    return CTEST_RETURN_SUCCESS;
}

static int vcBasicInt16(fmi3_import_int_typedef_t* t) {
    REQUIRE(fmi3_import_get_int16_type_min(t) == INT16_MIN);
    REQUIRE(fmi3_import_get_int16_type_max(t) == INT16_MAX);

    return CTEST_RETURN_SUCCESS;
}

static int vcBasicInt8(fmi3_import_int_typedef_t* t) {
    REQUIRE(fmi3_import_get_int8_type_min(t) == INT8_MIN);
    REQUIRE(fmi3_import_get_int8_type_max(t) == INT8_MAX);

    return CTEST_RETURN_SUCCESS;
}

static int vcBasicUInt64(fmi3_import_int_typedef_t* t) {
    REQUIRE(fmi3_import_get_uint64_type_min(t) == UINTXX_MIN);
    REQUIRE(fmi3_import_get_uint64_type_max(t) == UINT64_MAX);

    return CTEST_RETURN_SUCCESS;
}

static int vcBasicUInt32(fmi3_import_int_typedef_t* t) {
    REQUIRE(fmi3_import_get_uint32_type_min(t) == UINTXX_MIN);
    REQUIRE(fmi3_import_get_uint32_type_max(t) == UINT32_MAX);

    return CTEST_RETURN_SUCCESS;
}

static int vcBasicUInt16(fmi3_import_int_typedef_t* t) {
    REQUIRE(fmi3_import_get_uint16_type_min(t) == UINTXX_MIN);
    REQUIRE(fmi3_import_get_uint16_type_max(t) == UINT16_MAX);

    return CTEST_RETURN_SUCCESS;
}

static int vcBasicUInt8(fmi3_import_int_typedef_t* t) {
    REQUIRE(fmi3_import_get_uint8_type_min(t) == UINTXX_MIN);
    REQUIRE(fmi3_import_get_uint8_type_max(t) == UINT8_MAX);

    return CTEST_RETURN_SUCCESS;
}

/* 'Override' value checkers: verify that default values from base type can be overriden by the typedef */

static int vcOverrideInt64(fmi3_import_int_typedef_t* t) {
    REQUIRE(fmi3_import_get_int64_type_min(t) == INT64_MIN + D_TYPE);
    REQUIRE(fmi3_import_get_int64_type_max(t) == INT64_MAX - D_TYPE);

    return CTEST_RETURN_SUCCESS;
}

static int vcOverrideInt32(fmi3_import_int_typedef_t* t) {
    REQUIRE(fmi3_import_get_int32_type_min(t) == INT32_MIN + D_TYPE);
    REQUIRE(fmi3_import_get_int32_type_max(t) == INT32_MAX - D_TYPE);

    return CTEST_RETURN_SUCCESS;
}

static int vcOverrideInt16(fmi3_import_int_typedef_t* t) {
    REQUIRE(fmi3_import_get_int16_type_min(t) == INT16_MIN + D_TYPE);
    REQUIRE(fmi3_import_get_int16_type_max(t) == INT16_MAX - D_TYPE);

    return CTEST_RETURN_SUCCESS;
}

static int vcOverrideInt8(fmi3_import_int_typedef_t* t) {
    REQUIRE(fmi3_import_get_int8_type_min(t) == INT8_MIN + D_TYPE);
    REQUIRE(fmi3_import_get_int8_type_max(t) == INT8_MAX - D_TYPE);

    return CTEST_RETURN_SUCCESS;
}

static int vcOverrideUInt64(fmi3_import_int_typedef_t* t) {
    REQUIRE(fmi3_import_get_uint64_type_min(t) == UINTXX_MIN + D_TYPE);
    REQUIRE(fmi3_import_get_uint64_type_max(t) == UINT64_MAX - D_TYPE);

    return CTEST_RETURN_SUCCESS;
}

static int vcOverrideUInt32(fmi3_import_int_typedef_t* t) {
    REQUIRE(fmi3_import_get_uint32_type_min(t) == UINTXX_MIN + D_TYPE);
    REQUIRE(fmi3_import_get_uint32_type_max(t) == UINT32_MAX - D_TYPE);

    return CTEST_RETURN_SUCCESS;
}

static int vcOverrideUInt16(fmi3_import_int_typedef_t* t) {
    REQUIRE(fmi3_import_get_uint16_type_min(t) == UINTXX_MIN + D_TYPE);
    REQUIRE(fmi3_import_get_uint16_type_max(t) == UINT16_MAX - D_TYPE);

    return CTEST_RETURN_SUCCESS;
}

static int vcOverrideUInt8(fmi3_import_int_typedef_t* t) {
    REQUIRE(fmi3_import_get_uint8_type_min(t) == UINTXX_MIN + D_TYPE);
    REQUIRE(fmi3_import_get_uint8_type_max(t) == UINT8_MAX - D_TYPE);

    return CTEST_RETURN_SUCCESS;
}

/* PartialOverride: min should be overridden, but not max */

static int vcPartialOverrideInt32(fmi3_import_int_typedef_t* t) {
    REQUIRE(fmi3_import_get_uint32_type_min(t) == -1);
    REQUIRE(fmi3_import_get_uint32_type_max(t) == INT32_MAX);

    return CTEST_RETURN_SUCCESS;
}

static int vcPartialOverrideUInt32(fmi3_import_int_typedef_t* t) {
    REQUIRE(fmi3_import_get_uint32_type_min(t) == 1);
    REQUIRE(fmi3_import_get_uint32_type_max(t) == UINT32_MAX);

    return CTEST_RETURN_SUCCESS;
}

/* 'basic' variable checkers: verify that default values are inherited */

static int vcInheritBasicTypedefVariableInt64(fmi3_import_variable_t* v) {
    fmi3_import_int64_variable_t* vt = fmi3_import_get_variable_as_int64(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_int64_variable_min(vt) == INT64_MIN);
    REQUIRE(fmi3_import_get_int64_variable_max(vt) == INT64_MAX);

    return CTEST_RETURN_SUCCESS;
}

static int vcInheritBasicTypedefVariableInt32(fmi3_import_variable_t* v) {
    fmi3_import_int32_variable_t* vt = fmi3_import_get_variable_as_int32(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_int32_variable_min(vt) == INT32_MIN);
    REQUIRE(fmi3_import_get_int32_variable_max(vt) == INT32_MAX);

    return CTEST_RETURN_SUCCESS;
}

static int vcInheritBasicTypedefVariableInt16(fmi3_import_variable_t* v) {
    fmi3_import_int16_variable_t* vt = fmi3_import_get_variable_as_int16(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_int16_variable_min(vt) == INT16_MIN);
    REQUIRE(fmi3_import_get_int16_variable_max(vt) == INT16_MAX);

    return CTEST_RETURN_SUCCESS;
}

static int vcInheritBasicTypedefVariableInt8(fmi3_import_variable_t* v) {
    fmi3_import_int8_variable_t* vt = fmi3_import_get_variable_as_int8(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_int8_variable_min(vt) == INT8_MIN);
    REQUIRE(fmi3_import_get_int8_variable_max(vt) == INT8_MAX);

    return CTEST_RETURN_SUCCESS;
}

static int vcInheritBasicTypedefVariableUInt64(fmi3_import_variable_t* v) {
    fmi3_import_uint64_variable_t* vt = fmi3_import_get_variable_as_uint64(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_uint64_variable_min(vt) == UINTXX_MIN);
    REQUIRE(fmi3_import_get_uint64_variable_max(vt) == UINT64_MAX);

    return CTEST_RETURN_SUCCESS;
}

static int vcInheritBasicTypedefVariableUInt32(fmi3_import_variable_t* v) {
    fmi3_import_uint32_variable_t* vt = fmi3_import_get_variable_as_uint32(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_uint32_variable_min(vt) == UINTXX_MIN);
    REQUIRE(fmi3_import_get_uint32_variable_max(vt) == UINT32_MAX);

    return CTEST_RETURN_SUCCESS;
}

static int vcInheritBasicTypedefVariableUInt16(fmi3_import_variable_t* v) {
    fmi3_import_uint16_variable_t* vt = fmi3_import_get_variable_as_uint16(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_uint16_variable_min(vt) == UINTXX_MIN);
    REQUIRE(fmi3_import_get_uint16_variable_max(vt) == UINT16_MAX);

    return CTEST_RETURN_SUCCESS;
}

static int vcInheritBasicTypedefVariableUInt8(fmi3_import_variable_t* v) {
    fmi3_import_uint8_variable_t* vt = fmi3_import_get_variable_as_uint8(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_uint8_variable_min(vt) == UINTXX_MIN);
    REQUIRE(fmi3_import_get_uint8_variable_max(vt) == UINT8_MAX);

    return CTEST_RETURN_SUCCESS;
}

/* 'InheritOverridingTypedef': verify that variable can inherit typedef that overrides defaults */

static int vcInheritOverridingTypedefVariableInt64(fmi3_import_variable_t* v) {
    fmi3_import_int64_variable_t* vt = fmi3_import_get_variable_as_int64(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_int64_variable_min(vt) == INT64_MIN + D_TYPE);
    REQUIRE(fmi3_import_get_int64_variable_max(vt) == INT64_MAX - D_TYPE);

    return CTEST_RETURN_SUCCESS;
}

static int vcInheritOverridingTypedefVariableInt32(fmi3_import_variable_t* v) {
    fmi3_import_int32_variable_t* vt = fmi3_import_get_variable_as_int32(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_int32_variable_min(vt) == INT32_MIN + D_TYPE);
    REQUIRE(fmi3_import_get_int32_variable_max(vt) == INT32_MAX - D_TYPE);

    return CTEST_RETURN_SUCCESS;
}

static int vcInheritOverridingTypedefVariableInt16(fmi3_import_variable_t* v) {
    fmi3_import_int16_variable_t* vt = fmi3_import_get_variable_as_int16(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_int16_variable_min(vt) == INT16_MIN + D_TYPE);
    REQUIRE(fmi3_import_get_int16_variable_max(vt) == INT16_MAX - D_TYPE);

    return CTEST_RETURN_SUCCESS;
}

static int vcInheritOverridingTypedefVariableInt8(fmi3_import_variable_t* v) {
    fmi3_import_int8_variable_t* vt = fmi3_import_get_variable_as_int8(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_int8_variable_min(vt) == INT8_MIN + D_TYPE);
    REQUIRE(fmi3_import_get_int8_variable_max(vt) == INT8_MAX - D_TYPE);

    return CTEST_RETURN_SUCCESS;
}

static int vcInheritOverridingTypedefVariableUInt64(fmi3_import_variable_t* v) {
    fmi3_import_uint64_variable_t* vt = fmi3_import_get_variable_as_uint64(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_uint64_variable_min(vt) == UINTXX_MIN + D_TYPE);
    REQUIRE(fmi3_import_get_uint64_variable_max(vt) == UINT64_MAX - D_TYPE);

    return CTEST_RETURN_SUCCESS;
}

static int vcInheritOverridingTypedefVariableUInt32(fmi3_import_variable_t* v) {
    fmi3_import_uint32_variable_t* vt = fmi3_import_get_variable_as_uint32(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_uint32_variable_min(vt) == UINTXX_MIN + D_TYPE);
    REQUIRE(fmi3_import_get_uint32_variable_max(vt) == UINT32_MAX - D_TYPE);

    return CTEST_RETURN_SUCCESS;
}

static int vcInheritOverridingTypedefVariableUInt16(fmi3_import_variable_t* v) {
    fmi3_import_uint16_variable_t* vt = fmi3_import_get_variable_as_uint16(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_uint16_variable_min(vt) == UINTXX_MIN + D_TYPE);
    REQUIRE(fmi3_import_get_uint16_variable_max(vt) == UINT16_MAX - D_TYPE);

    return CTEST_RETURN_SUCCESS;
}

static int vcInheritOverridingTypedefVariableUInt8(fmi3_import_variable_t* v) {
    fmi3_import_uint8_variable_t* vt = fmi3_import_get_variable_as_uint8(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_uint8_variable_min(vt) == UINTXX_MIN + D_TYPE);
    REQUIRE(fmi3_import_get_uint8_variable_max(vt) == UINT8_MAX  - D_TYPE);

    return CTEST_RETURN_SUCCESS;
}

/* 'Override' variable checkers: verify that values in variable can override defaults/typedef */

static int vcOverrideVariableInt64(fmi3_import_variable_t* v) {
    fmi3_import_int64_variable_t* vt = fmi3_import_get_variable_as_int64(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_int64_variable_min(vt) == INT64_MIN + D_VAR);
    REQUIRE(fmi3_import_get_int64_variable_max(vt) == INT64_MAX - D_VAR);

    return CTEST_RETURN_SUCCESS;
}

static int vcOverrideVariableInt32(fmi3_import_variable_t* v) {
    fmi3_import_int32_variable_t* vt = fmi3_import_get_variable_as_int32(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_int32_variable_min(vt) == INT32_MIN + D_VAR);
    REQUIRE(fmi3_import_get_int32_variable_max(vt) == INT32_MAX - D_VAR);

    return CTEST_RETURN_SUCCESS;
}

static int vcOverrideVariableInt16(fmi3_import_variable_t* v) {
    fmi3_import_int16_variable_t* vt = fmi3_import_get_variable_as_int16(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_int16_variable_min(vt) == INT16_MIN + D_VAR);
    REQUIRE(fmi3_import_get_int16_variable_max(vt) == INT16_MAX - D_VAR);

    return CTEST_RETURN_SUCCESS;
}

static int vcOverrideVariableInt8(fmi3_import_variable_t* v) {
    fmi3_import_int8_variable_t* vt = fmi3_import_get_variable_as_int8(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_int8_variable_min(vt) == INT8_MIN + D_VAR);
    REQUIRE(fmi3_import_get_int8_variable_max(vt) == INT8_MAX - D_VAR);

    return CTEST_RETURN_SUCCESS;
}

static int vcOverrideVariableUInt64(fmi3_import_variable_t* v) {
    fmi3_import_uint64_variable_t* vt = fmi3_import_get_variable_as_uint64(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_uint64_variable_min(vt) == UINTXX_MIN + D_VAR);
    REQUIRE(fmi3_import_get_uint64_variable_max(vt) == UINT64_MAX - D_VAR);

    return CTEST_RETURN_SUCCESS;
}

static int vcOverrideVariableUInt32(fmi3_import_variable_t* v) {
    fmi3_import_uint32_variable_t* vt = fmi3_import_get_variable_as_uint32(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_uint32_variable_min(vt) == UINTXX_MIN + D_VAR);
    REQUIRE(fmi3_import_get_uint32_variable_max(vt) == UINT32_MAX - D_VAR);

    return CTEST_RETURN_SUCCESS;
}

static int vcOverrideVariableUInt16(fmi3_import_variable_t* v) {
    fmi3_import_uint16_variable_t* vt = fmi3_import_get_variable_as_uint16(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_uint16_variable_min(vt) == UINTXX_MIN + D_VAR);
    REQUIRE(fmi3_import_get_uint16_variable_max(vt) == UINT16_MAX - D_VAR);

    return CTEST_RETURN_SUCCESS;
}

static int vcOverrideVariableUInt8(fmi3_import_variable_t* v) {
    fmi3_import_uint8_variable_t* vt = fmi3_import_get_variable_as_uint8(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_uint8_variable_min(vt) == UINTXX_MIN + D_VAR);
    REQUIRE(fmi3_import_get_uint8_variable_max(vt) == UINT8_MAX - D_VAR);

    return CTEST_RETURN_SUCCESS;
}

/* 'partial override' value checkers: partial overrides of values, i.e. some are inherited others overriden */

static int vcPartialOverrideTypedefVariableInt32(fmi3_import_variable_t* v) {
    fmi3_import_int32_variable_t* vt = fmi3_import_get_variable_as_int32(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_int32_variable_min(vt) == INT32_MIN + D_VAR);
    REQUIRE(fmi3_import_get_int32_variable_max(vt) == INT32_MAX);

    return CTEST_RETURN_SUCCESS;
}

static int vcPartialOverrideDefaultVariableInt32(fmi3_import_variable_t* v) {
    fmi3_import_int32_variable_t* vt = fmi3_import_get_variable_as_int32(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_int32_variable_min(vt) == INT32_MIN);
    REQUIRE(fmi3_import_get_int32_variable_max(vt) == INT32_MAX - D_VAR);

    return CTEST_RETURN_SUCCESS;
}

/* 'default start' value checkers: that default start values are set */

static int vcDefaultStartVariableInt64(fmi3_import_variable_t* v) {
    fmi3_import_int64_variable_t* vt = fmi3_import_get_variable_as_int64(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_int64_variable_start(vt) == 0);

    return CTEST_RETURN_SUCCESS;
}

static int vcDefaultStartVariableInt32(fmi3_import_variable_t* v) {
    fmi3_import_int32_variable_t* vt = fmi3_import_get_variable_as_int32(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_int32_variable_start(vt) == 0);

    return CTEST_RETURN_SUCCESS;
}

static int vcDefaultStartVariableInt16(fmi3_import_variable_t* v) {
    fmi3_import_int16_variable_t* vt = fmi3_import_get_variable_as_int16(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_int16_variable_start(vt) == 0);

    return CTEST_RETURN_SUCCESS;
}

static int vcDefaultStartVariableInt8(fmi3_import_variable_t* v) {
    fmi3_import_int8_variable_t* vt = fmi3_import_get_variable_as_int8(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_int8_variable_start(vt) == 0);

    return CTEST_RETURN_SUCCESS;
}

static int vcDefaultStartVariableUInt64(fmi3_import_variable_t* v) {
    fmi3_import_uint64_variable_t* vt = fmi3_import_get_variable_as_uint64(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_uint64_variable_start(vt) == 0);

    return CTEST_RETURN_SUCCESS;
}

static int vcDefaultStartVariableUInt32(fmi3_import_variable_t* v) {
    fmi3_import_uint32_variable_t* vt = fmi3_import_get_variable_as_uint32(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_uint32_variable_start(vt) == 0);

    return CTEST_RETURN_SUCCESS;
}

static int vcDefaultStartVariableUInt16(fmi3_import_variable_t* v) {
    fmi3_import_uint16_variable_t* vt = fmi3_import_get_variable_as_uint16(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_uint16_variable_start(vt) == 0);

    return CTEST_RETURN_SUCCESS;
}

static int vcDefaultStartVariableUInt8(fmi3_import_variable_t* v) {
    fmi3_import_uint8_variable_t* vt = fmi3_import_get_variable_as_uint8(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_uint8_variable_start(vt) == 0);

    return CTEST_RETURN_SUCCESS;
}

/* 'with start specified' value checkers: verifies that start values are read as specified in XML */

static int vcWithStartVariableInt64(fmi3_import_variable_t* v) {
    fmi3_import_int64_variable_t* vt = fmi3_import_get_variable_as_int64(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_int64_variable_start(vt) == INT64_MAX - D_START);

    return CTEST_RETURN_SUCCESS;
}

static int vcWithStartVariableInt32(fmi3_import_variable_t* v) {
    fmi3_import_int32_variable_t* vt = fmi3_import_get_variable_as_int32(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_int32_variable_start(vt) == INT32_MAX - D_START);

    return CTEST_RETURN_SUCCESS;
}

static int vcWithStartVariableInt16(fmi3_import_variable_t* v) {
    fmi3_import_int16_variable_t* vt = fmi3_import_get_variable_as_int16(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_int16_variable_start(vt) == INT16_MAX - D_START);

    return CTEST_RETURN_SUCCESS;
}

static int vcWithStartVariableInt8(fmi3_import_variable_t* v) {
    fmi3_import_int8_variable_t* vt = fmi3_import_get_variable_as_int8(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_int8_variable_start(vt) == INT8_MAX - D_START);

    return CTEST_RETURN_SUCCESS;
}

static int vcWithStartVariableUInt64(fmi3_import_variable_t* v) {
    fmi3_import_uint64_variable_t* vt = fmi3_import_get_variable_as_uint64(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_uint64_variable_start(vt) == UINT64_MAX - D_START);

    return CTEST_RETURN_SUCCESS;
}

static int vcWithStartVariableUInt32(fmi3_import_variable_t* v) {
    fmi3_import_uint32_variable_t* vt = fmi3_import_get_variable_as_uint32(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_uint32_variable_start(vt) == UINT32_MAX - D_START);

    return CTEST_RETURN_SUCCESS;
}

static int vcWithStartVariableUInt16(fmi3_import_variable_t* v) {
    fmi3_import_uint16_variable_t* vt = fmi3_import_get_variable_as_uint16(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_uint16_variable_start(vt) == UINT16_MAX - D_START);

    return CTEST_RETURN_SUCCESS;
}

static int vcWithStartVariableUInt8(fmi3_import_variable_t* v) {
    fmi3_import_uint8_variable_t* vt = fmi3_import_get_variable_as_uint8(v);
    REQUIRE(vt != nullptr);
    REQUIRE(fmi3_import_get_uint8_variable_start(vt) == UINT8_MAX - D_START);

    return CTEST_RETURN_SUCCESS;
}

/* return value: exit code */
static int get_typedef_by_name(fmi3_import_type_definition_list_t* tds, fmi2_string_t name, fmi3_import_variable_typedef_t **t) {
    unsigned int nTds;
    fmi3_import_variable_typedef_t *td;
    unsigned int i;

    nTds = fmi3_import_get_type_definition_list_size(tds);
    for (i = 0; i < nTds; i++) {
        td = fmi3_import_get_typedef(tds, i);

        if (strcmp(fmi3_import_get_type_name(td), name) == 0) {
            *t = td;
            return CTEST_RETURN_SUCCESS;
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
    fmi3_import_type_definition_list_t* tds;
    int res; /* result from test-function */

    tds = fmi3_import_get_type_definition_list(xml);
    REQUIRE(tds != nullptr);

    res = get_typedef_by_name(tds, typeName, &t);
    if (res != CTEST_RETURN_SUCCESS) return res;

    REQUIRE(strcmp(fmi3_import_get_type_name(t), typeName) == 0);
    /* testing default case: none specified in MD */
    REQUIRE(fmi3_import_get_type_quantity(t) == nullptr); 

    intType = fmi3_import_get_type_as_int(t);
    REQUIRE(intType != nullptr);

    res = vcIntXX(intType);
    if (res != CTEST_RETURN_SUCCESS) return res;

    return CTEST_RETURN_SUCCESS;
}

static int test_variable_intXX(fmi3_import_t *xml, fmi2_string_t varName,
        int (*vcVariableIntXX)(fmi3_import_variable_t* v))
{
    fmi3_import_variable_t* v;
    int res; /* result from test-function */

    v = fmi3_import_get_variable_by_name(xml, varName);
    REQUIRE(v != nullptr);

    /* specific testing for variable's type */
    res = vcVariableIntXX(v);
    if (res != CTEST_RETURN_SUCCESS) return res;

    return CTEST_RETURN_SUCCESS;
}

static int test_variable_all_attributes_int32(fmi3_import_t* xml, fmi2_string_t varName)
{
    fmi3_import_variable_t* v;
    fmi3_import_variable_typedef_t* t;

    v = fmi3_import_get_variable_by_name(xml, varName);
    REQUIRE(v != nullptr);

    REQUIRE(strcmp("description", fmi3_import_get_variable_description(v)) == 0);
    /* TODO: it feels a bit odd that 'fmi3_import_get_causality' doesn't include _variable_ in the name - was a bit hard to find... */
    REQUIRE(fmi3_import_get_causality(v) == fmi3_causality_enu_output);
    REQUIRE(fmi3_import_get_variability(v) == fmi3_variability_enu_discrete);
    REQUIRE(fmi3_import_get_initial(v) == fmi3_initial_enu_calculated);
    fmi3_import_int32_variable_t* v32 = fmi3_import_get_variable_as_int32(v);
    REQUIRE(v32 != nullptr);
    REQUIRE_STREQ(fmi3_import_get_int32_variable_quantity(v32) , "Frequency");

    t = fmi3_import_get_variable_declared_type(v);
    REQUIRE(t != nullptr);
    REQUIRE(strcmp("typedef_basic_int32", fmi3_import_get_type_name(t)) == 0);

    return CTEST_RETURN_SUCCESS;
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
static int test_value_boundary_check(const char* xmldir) {
    fmi3_import_t* xml;
    jm_callbacks cb;
    fmi_import_context_t* ctx;
    int res = -1;

    /* update these if you change xml: */
    int nBadTds = 20;
    int nBadVars = 21;

    /* create shallow copy, but change logger so we can count errors */
    shallow_copy_jm_callbacks(jm_get_default_callbacks(), &cb);
    cb.logger = jm_logger_count_errors;

    ctx = fmi_import_allocate_context(&cb);
    if (ctx == nullptr) {
        return -1;
    }

    /* parse the XML */
    G_nErrors = 0;
    xml = fmi3_import_parse_xml(ctx, xmldir, nullptr);
    fmi_import_free_context(ctx);
    if (xml == nullptr) {
        return -1;
    }

    /* check num logger error reports */
    if (G_nErrors != 2 * (nBadTds + nBadVars)) { /* x2 because we currently log more than once for each ... */
        printf("  ERROR: unexpected number of parsing errors\n");
        res = -1;
        goto err1;
    }
    
    res = 0;

err1:
    fmi3_import_free(xml);

    return res;
}

TEST_CASE("TypeDefinitions: FloatXX and IntXX") {
    fmi3_import_t* xml;
    int ret = 1;
    const char* xmldir = FMI3_TEST_XML_DIR "/type_definitions/valid";

    xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);

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

    REQUIRE(ret == CTEST_RETURN_SUCCESS);
}

static void require_variable_has_declared_type(fmi3_import_t* xml, const char* varName, const char* tdName) {
    fmi3_import_type_definition_list_t* tds; 
    fmi3_import_variable_t* v;
    fmi3_import_variable_typedef_t *tdVar, *tdByName;

    tds = fmi3_import_get_type_definition_list(xml);
    REQUIRE(tds != nullptr);

    v = fmi3_import_get_variable_by_name(xml, varName);
    REQUIRE(v != nullptr);
    tdVar = fmi3_import_get_variable_declared_type(v);
    tdByName = get_typedef_by_name(tds, tdName);
    REQUIRE(tdVar != nullptr);
    REQUIRE(tdByName != nullptr);
    
    REQUIRE(tdVar == tdByName);
}

TEST_CASE("TypeDefinitions: FloatXX and IntXX - declaredType") {
    const char* xmldir = FMI3_TEST_XML_DIR "/type_definitions/valid";

    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);
    
    SECTION("IntXX") {
        // Variable: No attr, Typedef: No attr
        require_variable_has_declared_type(xml, "variable_inherit_typedef_basic_int64", "typedef_basic_int64");
        require_variable_has_declared_type(xml, "variable_inherit_typedef_basic_uint8", "typedef_basic_uint8");

        // Variable: No attr, Typedef: Some attr
        require_variable_has_declared_type(xml, "variable_inherit_typedef_override_int64", "typedef_override_int64");
        require_variable_has_declared_type(xml, "variable_inherit_typedef_override_uint8", "typedef_override_uint8");

        // Variable: Some attr, TypeDef: No attr
        require_variable_has_declared_type(xml, "variable_override_basic_int64", "typedef_basic_int64");
        require_variable_has_declared_type(xml, "variable_override_basic_uint8", "typedef_basic_uint8");

        // Variable: Some attr, TypeDef: Some attr
        require_variable_has_declared_type(xml, "variable_override_typedef_int64", "typedef_override_int64");
        require_variable_has_declared_type(xml, "variable_override_typedef_uint8", "typedef_override_uint8");
    }
    SECTION("FloatXX") {
        // Variable: No attr, TypeDef: Some attr
        require_variable_has_declared_type(xml, "float64_with_typedef",          "float64_type_with_attributes");

        // Variable: Some attr, TypeDef: Some attr
        require_variable_has_declared_type(xml, "float64_with_typedef_override", "float64_type_with_attributes");
    }
    
    fmi3_import_free(xml);
}

TEST_CASE("TypeDefinitions: value_boundary_check") {
    const char* xmldir = FMI3_TEST_XML_DIR "/type_definitions/value_boundary_check";

    printf("\nThe next tests are expected to fail...\n");
    printf("---------------------------------------------------------------------------\n");
    REQUIRE(test_value_boundary_check(xmldir) == 0);
    printf("---------------------------------------------------------------------------\n");
}

TEST_CASE("TypeDefinitions: Binary") {
    fmi3_import_t* xml;
    const char* xmldir = FMI3_TEST_XML_DIR "/type_definitions/valid/binary";

    xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);

    const char* mimeType;
    const char* mimeTypeDefault = "application/octet-stream";
    size_t maxSize;
    size_t maxSizeDefault = 0;

    //--------------------------------------------------------------------------
    // Test attributes for TypeDefinitions
    //--------------------------------------------------------------------------
    fmi3_import_type_definition_list_t* tds; 
    fmi3_import_binary_typedef_t *tdMinimal, *tdMimeType, *tdAllAttr;

    tds = fmi3_import_get_type_definition_list(xml);
    REQUIRE(tds != nullptr);
    
    tdMinimal  = fmi3_import_get_type_as_binary(get_typedef_by_name(tds, "td_minimal"));
    tdMimeType = fmi3_import_get_type_as_binary(get_typedef_by_name(tds, "td_mimeType"));
    tdAllAttr  = fmi3_import_get_type_as_binary(get_typedef_by_name(tds, "td_allAttr"));
    REQUIRE(tdMinimal  != nullptr);
    REQUIRE(tdMimeType != nullptr);
    REQUIRE(tdAllAttr  != nullptr);
    
    SECTION("Minimal typedef") {
        mimeType = fmi3_import_get_binary_type_mime_type(tdMinimal);
        maxSize  = fmi3_import_get_binary_type_max_size(tdMinimal);
        REQUIRE(strcmp(mimeType, mimeTypeDefault) == 0);
        REQUIRE(maxSize == maxSizeDefault);
    }
    SECTION("Typedef that sets mimeType") {
        mimeType = fmi3_import_get_binary_type_mime_type(tdMimeType);
        maxSize  = fmi3_import_get_binary_type_max_size(tdMimeType);
        REQUIRE(strcmp(mimeType, "mt0") == 0);
        REQUIRE(maxSize == maxSizeDefault);
    }
    SECTION("Typedef that sets both mimeType and maxSize") {
        mimeType = fmi3_import_get_binary_type_mime_type(tdAllAttr);
        maxSize  = fmi3_import_get_binary_type_max_size(tdAllAttr);
        REQUIRE(strcmp(mimeType, "mt0") == 0);
        REQUIRE(maxSize == 999);
    }
    
    //---------------------------------------------------------------------------------
    // Test attribute for variables with declaredType, but without override on Variable
    //---------------------------------------------------------------------------------

    fmi3_import_variable_t* v;
    fmi3_import_binary_variable_t* bv;
    
    SECTION("Variable without typedef") {
        v = fmi3_import_get_variable_by_name(xml, "var_default_type_1");
        bv = fmi3_import_get_variable_as_binary(v);
        mimeType = fmi3_import_get_binary_variable_mime_type(bv);
        maxSize  = fmi3_import_get_binary_variable_max_size(bv);
        REQUIRE(fmi3_import_get_variable_declared_type(v) == nullptr);
        REQUIRE(strcmp(mimeType, mimeTypeDefault) == 0);
        REQUIRE(maxSize == maxSizeDefault);
    }
    SECTION("Variable inheriting minimal typedef") {
        v = fmi3_import_get_variable_by_name(xml, "var_inh_minimal_1");
        bv = fmi3_import_get_variable_as_binary(v);
        mimeType = fmi3_import_get_binary_variable_mime_type(bv);
        maxSize  = fmi3_import_get_binary_variable_max_size(bv);
        REQUIRE(fmi3_import_get_variable_declared_type(v) == (void*)tdMinimal);
        REQUIRE(strcmp(mimeType, mimeTypeDefault) == 0);
        REQUIRE(maxSize == maxSizeDefault);
    }
    SECTION("Variable inheriting typedef that defines mimeType") {
        v = fmi3_import_get_variable_by_name(xml, "var_inh_mimeType_1");
        bv = fmi3_import_get_variable_as_binary(v);
        mimeType = fmi3_import_get_binary_variable_mime_type(bv);
        maxSize  = fmi3_import_get_binary_variable_max_size(bv);
        REQUIRE(fmi3_import_get_variable_declared_type(v) == (void*)tdMimeType);
        REQUIRE(strcmp(mimeType, "mt0") == 0);
        REQUIRE(maxSize == maxSizeDefault);
    }
    SECTION("Variable inheriting typedef that defines mimeType and maxSize") {
        v = fmi3_import_get_variable_by_name(xml, "var_inh_allAttr_1");
        bv = fmi3_import_get_variable_as_binary(v);
        mimeType = fmi3_import_get_binary_variable_mime_type(bv);
        maxSize  = fmi3_import_get_binary_variable_max_size(bv);
        REQUIRE(fmi3_import_get_variable_declared_type(v) == (void*)tdAllAttr);
        REQUIRE(strcmp(mimeType, "mt0") == 0);
        REQUIRE(maxSize == 999);
    }
    
    //------------------------------------------------------------------------------
    // Test attribute for variables with declaredType and with overriding attributes
    //------------------------------------------------------------------------------
    
    SECTION("Variable without typedef, and variable overrides mimeType") {
        v = fmi3_import_get_variable_by_name(xml, "var_default_type_2");
        bv = fmi3_import_get_variable_as_binary(v);
        mimeType = fmi3_import_get_binary_variable_mime_type(bv);
        maxSize  = fmi3_import_get_binary_variable_max_size(bv);
        REQUIRE(fmi3_import_get_variable_declared_type(v) == nullptr);
        REQUIRE(strcmp(mimeType, "mt2") == 0);
        REQUIRE(maxSize == maxSizeDefault);
    }
    SECTION("Variable inheriting minimal typedef, and variable overrides mimeType") {
        v = fmi3_import_get_variable_by_name(xml, "var_inh_minimal_2");
        bv = fmi3_import_get_variable_as_binary(v);
        mimeType = fmi3_import_get_binary_variable_mime_type(bv);
        maxSize  = fmi3_import_get_binary_variable_max_size(bv);
        REQUIRE(fmi3_import_get_variable_declared_type(v) == (void*)tdMinimal);
        REQUIRE(strcmp(mimeType, "mt2") == 0);
        REQUIRE(maxSize == maxSizeDefault);
    }
    SECTION("Variable inheriting typedef that defines mimeType, and variable overrides mimeType") {
        v = fmi3_import_get_variable_by_name(xml, "var_inh_mimeType_2");
        bv = fmi3_import_get_variable_as_binary(v);
        mimeType = fmi3_import_get_binary_variable_mime_type(bv);
        maxSize  = fmi3_import_get_binary_variable_max_size(bv);
        REQUIRE(fmi3_import_get_variable_declared_type(v) == (void*)tdMimeType);
        REQUIRE(strcmp(mimeType, "mt2") == 0);
        REQUIRE(maxSize == maxSizeDefault);
    }
    SECTION("Variable inheriting typedef that defines mimeType and maxSize, and variable overrides both") {
        v = fmi3_import_get_variable_by_name(xml, "var_inh_allAttr_2");
        bv = fmi3_import_get_variable_as_binary(v);
        mimeType = fmi3_import_get_binary_variable_mime_type(bv);
        maxSize  = fmi3_import_get_binary_variable_max_size(bv);
        REQUIRE(fmi3_import_get_variable_declared_type(v) == (void*)tdAllAttr);
        REQUIRE(strcmp(mimeType, "mt2") == 0);
        REQUIRE(maxSize == 2);
    }

    fmi3_import_free(xml);
}

static void require_clock_variable_all_attr_default_except_resolution(fmi3_import_clock_variable_t* cv) {
    REQUIRE(fmi3_import_get_clock_variable_can_be_deactivated(cv)   == false);
    REQUIRE(fmi3_import_get_clock_variable_priority(cv)             == 0);
    REQUIRE(fmi3_import_get_clock_variable_interval_decimal(cv)     == 0.0);
    REQUIRE(fmi3_import_get_clock_variable_shift_decimal(cv)        == 0.0);
    REQUIRE(fmi3_import_get_clock_variable_supports_fraction(cv)    == false);
    REQUIRE(fmi3_import_get_clock_variable_interval_counter(cv)     == 0);
    REQUIRE(fmi3_import_get_clock_variable_shift_counter(cv)        == 0);
    REQUIRE(fmi3_import_get_clock_variable_interval_variability(cv) == fmi3_interval_variability_constant);
}

TEST_CASE("TypeDefinitions: Clock") {
    fmi3_import_t* xml;
    const char* xmldir = FMI3_TEST_XML_DIR "/type_definitions/valid/clock";

    xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);

    //--------------------------------------------------------------------------
    // Test attributes for TypeDefinitions
    //--------------------------------------------------------------------------
    fmi3_import_type_definition_list_t* tds; 
    fmi3_import_clock_typedef_t *tdMinimal, *tdResolution, *tdAllAttr;

    tds = fmi3_import_get_type_definition_list(xml);
    REQUIRE(tds != nullptr);
    
    tdMinimal    = fmi3_import_get_type_as_clock(get_typedef_by_name(tds, "td_minimal"));
    tdResolution = fmi3_import_get_type_as_clock(get_typedef_by_name(tds, "td_resolution"));
    tdAllAttr    = fmi3_import_get_type_as_clock(get_typedef_by_name(tds, "td_allAttr"));
    REQUIRE(tdMinimal    != nullptr);
    REQUIRE(tdResolution != nullptr);
    REQUIRE(tdAllAttr    != nullptr);
    
    SECTION("Minimal typedef") {
        REQUIRE(fmi3_import_get_clock_type_can_be_deactivated(tdMinimal)   == false);
        REQUIRE(fmi3_import_get_clock_type_priority(tdMinimal)             == 0);
        REQUIRE(fmi3_import_get_clock_type_interval_decimal(tdMinimal)     == 0.0);
        REQUIRE(fmi3_import_get_clock_type_shift_decimal(tdMinimal)        == 0.0);
        REQUIRE(fmi3_import_get_clock_type_supports_fraction(tdMinimal)    == false);
        REQUIRE(fmi3_import_get_clock_type_interval_counter(tdMinimal)     == 0);
        REQUIRE(fmi3_import_get_clock_type_shift_counter(tdMinimal)        == 0);
        REQUIRE(fmi3_import_get_clock_type_resolution(tdMinimal)           == 0);
        REQUIRE(fmi3_import_get_clock_type_interval_variability(tdMinimal) == fmi3_interval_variability_constant);
    }
    SECTION("Typedef that sets resolution - other attributes inherited") {
        REQUIRE(fmi3_import_get_clock_type_can_be_deactivated(tdResolution)   == false);
        REQUIRE(fmi3_import_get_clock_type_priority(tdResolution)             == 0);
        REQUIRE(fmi3_import_get_clock_type_interval_decimal(tdResolution)     == 0.0);
        REQUIRE(fmi3_import_get_clock_type_shift_decimal(tdResolution)        == 0.0);
        REQUIRE(fmi3_import_get_clock_type_supports_fraction(tdResolution)    == false);
        REQUIRE(fmi3_import_get_clock_type_interval_counter(tdResolution)     == 0);
        REQUIRE(fmi3_import_get_clock_type_shift_counter(tdResolution)        == 0);
        REQUIRE(fmi3_import_get_clock_type_resolution(tdResolution)           == 99);
        REQUIRE(fmi3_import_get_clock_type_interval_variability(tdResolution) == fmi3_interval_variability_constant);
    }
    SECTION("Typedef that sets all clock attributes") {
        REQUIRE(fmi3_import_get_clock_type_can_be_deactivated(tdAllAttr)   == true);
        REQUIRE(fmi3_import_get_clock_type_priority(tdAllAttr)             == 99);
        REQUIRE(fmi3_import_get_clock_type_interval_decimal(tdAllAttr)     == 99.0);
        REQUIRE(fmi3_import_get_clock_type_shift_decimal(tdAllAttr)        == 99.0);
        REQUIRE(fmi3_import_get_clock_type_supports_fraction(tdAllAttr)    == true);
        REQUIRE(fmi3_import_get_clock_type_interval_counter(tdAllAttr)     == 99);
        REQUIRE(fmi3_import_get_clock_type_shift_counter(tdAllAttr)        == 99);
        REQUIRE(fmi3_import_get_clock_type_resolution(tdAllAttr)           == 99);
        REQUIRE(fmi3_import_get_clock_type_interval_variability(tdAllAttr) == fmi3_interval_variability_countdown);
    }

    //---------------------------------------------------------------------------------
    // Test attribute for variables with declaredType, but without override on Variable
    //---------------------------------------------------------------------------------

    fmi3_import_variable_t* v;
    fmi3_import_clock_variable_t* cv;
    
    SECTION("Variable attr inheritance from typedef") {
        SECTION("From default type") {
            v = fmi3_import_get_variable_by_name(xml, "var_default_type_1");
            cv = fmi3_import_get_variable_as_clock(v);
            require_clock_variable_all_attr_default_except_resolution(cv);
            REQUIRE(fmi3_import_get_clock_variable_resolution(cv) == 0);
            REQUIRE(fmi3_import_get_variable_declared_type(v) == nullptr);
        }
        SECTION("From minimal typedef") {
            v = fmi3_import_get_variable_by_name(xml, "var_inh_minimal_1");
            cv = fmi3_import_get_variable_as_clock(v);
            require_clock_variable_all_attr_default_except_resolution(cv);
            REQUIRE(fmi3_import_get_clock_variable_resolution(cv) == 0);
            REQUIRE(fmi3_import_get_variable_declared_type(v) == (void*)tdMinimal);
        }
        SECTION("From typedef defining resolution") {
            v = fmi3_import_get_variable_by_name(xml, "var_inh_resolution_1");
            cv = fmi3_import_get_variable_as_clock(v);
            require_clock_variable_all_attr_default_except_resolution(cv);
            REQUIRE(fmi3_import_get_clock_variable_resolution(cv) == 99);
            REQUIRE(fmi3_import_get_variable_declared_type(v) == (void*)tdResolution);
        }
        SECTION("From typedef defining all clock attrs") {
            v = fmi3_import_get_variable_by_name(xml, "var_inh_allAttr_1");
            cv = fmi3_import_get_variable_as_clock(v);
            REQUIRE(fmi3_import_get_clock_variable_can_be_deactivated(cv)   == true);
            REQUIRE(fmi3_import_get_clock_variable_priority(cv)             == 99);
            REQUIRE(fmi3_import_get_clock_variable_interval_decimal(cv)     == 99.0);
            REQUIRE(fmi3_import_get_clock_variable_shift_decimal(cv)        == 99.0);
            REQUIRE(fmi3_import_get_clock_variable_supports_fraction(cv)    == true);
            REQUIRE(fmi3_import_get_clock_variable_interval_counter(cv)     == 99);
            REQUIRE(fmi3_import_get_clock_variable_shift_counter(cv)        == 99);
            REQUIRE(fmi3_import_get_clock_variable_interval_variability(cv) == fmi3_interval_variability_countdown);
            REQUIRE(fmi3_import_get_variable_declared_type(v) == (void*)tdAllAttr);
        }
    }
    //------------------------------------------------------------------------------
    // Test attribute for variables with declaredType and with overriding attributes
    //------------------------------------------------------------------------------
    
    SECTION("Variable attr inheritance from typedef, and variable also sets some attr") {
        SECTION("Typedef: default, Variable: resolution") {
            v = fmi3_import_get_variable_by_name(xml, "var_default_type_2");
            cv = fmi3_import_get_variable_as_clock(v);
            require_clock_variable_all_attr_default_except_resolution(cv);
            REQUIRE(fmi3_import_get_clock_variable_resolution(cv) == 2);
            REQUIRE(fmi3_import_get_variable_declared_type(v) == nullptr);
        }
        SECTION("Typedef: minimal, Variable: resolution") {
            v = fmi3_import_get_variable_by_name(xml, "var_inh_minimal_2");
            cv = fmi3_import_get_variable_as_clock(v);
            require_clock_variable_all_attr_default_except_resolution(cv);
            REQUIRE(fmi3_import_get_clock_variable_resolution(cv) == 2);
            REQUIRE(fmi3_import_get_variable_declared_type(v) == (void*)tdMinimal);
        }
        SECTION("Typedef: resolution, Variable: resolution") {
            v = fmi3_import_get_variable_by_name(xml, "var_inh_resolution_2");
            cv = fmi3_import_get_variable_as_clock(v);
            require_clock_variable_all_attr_default_except_resolution(cv);
            REQUIRE(fmi3_import_get_clock_variable_resolution(cv) == 2);
            REQUIRE(fmi3_import_get_variable_declared_type(v) == (void*)tdResolution);
        }
        SECTION("Typedef: allAttr, Variable: allAttr") {
            v = fmi3_import_get_variable_by_name(xml, "var_inh_allAttr_2");
            cv = fmi3_import_get_variable_as_clock(v);
            REQUIRE(fmi3_import_get_clock_variable_can_be_deactivated(cv)   == false);
            REQUIRE(fmi3_import_get_clock_variable_priority(cv)             == 2);
            REQUIRE(fmi3_import_get_clock_variable_interval_decimal(cv)     == 2.0);
            REQUIRE(fmi3_import_get_clock_variable_shift_decimal(cv)        == 2.0);
            REQUIRE(fmi3_import_get_clock_variable_supports_fraction(cv)    == false);
            REQUIRE(fmi3_import_get_clock_variable_interval_counter(cv)     == 2);
            REQUIRE(fmi3_import_get_clock_variable_shift_counter(cv)        == 2);
            REQUIRE(fmi3_import_get_clock_variable_interval_variability(cv) == fmi3_interval_variability_triggered);
            REQUIRE(fmi3_import_get_variable_declared_type(v) == (void*)tdAllAttr);
        }
    }

    fmi3_import_free(xml);
}