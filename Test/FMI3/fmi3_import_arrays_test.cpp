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

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#include "catch.hpp"

/**
 * Returns total size of array
 * TODO: should this perhaps be a util API method?
 */
static fmi3_uint64_t get_array_size(const fmi3_uint64_t* dimSizes, size_t nDim) {
    fmi3_uint64_t size = nDim > 0 ? 1 : 0;

    for (size_t i = 0; i < nDim; i++) {
        size *= *(dimSizes++);
    }

    return size;
}

/**
 * Get array with dimension start sizes, resolved from VRs if necessary.
 * @param fmu: parsed model description
 * @param cb: callbacks
 * @param dimList list of dimensions
 * @param dimSizes: contains the start sizes, must be freed
 * TODO: should this perhaps be a util API method?
 */
static void get_dimensions_start_sizes(fmi3_import_t* fmu, jm_callbacks* cb, fmi3_import_dimension_list_t* dimList, fmi3_uint64_t** dimSizes) {
    size_t nDims = fmi3_import_get_dimension_list_size(dimList);

    /* allocate memory for sizes array */
    *dimSizes = (fmi3_uint64_t*) cb->malloc(sizeof(fmi3_uint64_t) * nDims);
    REQUIRE(*dimSizes != nullptr);

    /* fill the array */
    for (size_t i = 0; i < nDims; i++) {
        INFO("i = " << i);
        fmi3_import_dimension_t* d = fmi3_import_get_dimension(dimList, i);

        if (fmi3_import_get_dimension_has_vr(d)) {
            fmi3_value_reference_t dimVr = fmi3_import_get_dimension_vr(d);
            fmi3_import_uint32_variable_t* var = (fmi3_import_uint32_variable_t*)fmi3_import_get_variable_by_vr(fmu, dimVr);
            REQUIRE(var != nullptr);
            *((*dimSizes) + i) = fmi3_import_get_uint32_variable_start(var);
        } else { /* has start */
            *((*dimSizes) + i) = fmi3_import_get_dimension_start(d);
        }
    }
}

/* general array tester for float64 */
static void test_array_ok_64(fmi3_import_t* xml, const char* varName, fmi3_float64_t* startsExp, fmi3_uint64_t arrSizeExp, fmi3_uint64_t* dimSizesExp, size_t nDimsExp)
{
    fmi3_import_variable_t* v;
    fmi3_float64_t* starts;
    fmi3_import_dimension_list_t* dimList;
    size_t nDims;
    fmi3_uint64_t* dimSizes; /* needs to be int, because we resolve start attribute from Integer variable */
    fmi3_uint64_t arrSize;

    jm_callbacks* cb = jm_get_default_callbacks();

    v = fmi3_import_get_variable_by_name(xml, varName);
    REQUIRE(v != nullptr);

    REQUIRE(fmi3_import_get_variable_has_start(v) != 0);
    REQUIRE(fmi3_import_variable_is_array(v) != 0);

    dimList = fmi3_import_get_variable_dimension_list(v);
    nDims = fmi3_import_get_dimension_list_size(dimList);

    /* check num dimensions */
    REQUIRE(nDims == nDimsExp);

    /* check resolved dimension start sizes */
    get_dimensions_start_sizes(xml, cb, dimList, &dimSizes);
    for (size_t i = 0; i < nDims; i++) {
        INFO("i = " << i);
        REQUIRE(dimSizes[i] == dimSizesExp[i]);
    }

    /* check num start values */
    starts = fmi3_import_get_float64_variable_start_array(fmi3_import_get_variable_as_float64(v));
    arrSize = get_array_size(dimSizes, nDims);
    REQUIRE(arrSize == arrSizeExp);

    /* check start values */
    for (size_t i = 0; i < arrSize; i++) {
        INFO("i = " << i);
        REQUIRE(starts[i] == startsExp[i]);
    }

    cb->free(dimSizes);
}

/* parse 1(x1) array */
static void test_array1_64(fmi3_import_t *xml)
{
    fmi3_float64_t startsExp[1] = { 1.0 };
    fmi3_uint64_t nStartsExp = 1;
    const char* varName = "array1_64";
    fmi3_uint64_t dimSizesExp[1] = { 1 };
    size_t nDimsExp = 1;

    test_array_ok_64(xml, varName, (fmi3_float64_t*)startsExp, nStartsExp, dimSizesExp, nDimsExp);
}

/* parse 1x2 array */
static void test_array2_64(fmi3_import_t* xml)
{
    fmi3_float64_t startsExp[2] = { 1.0, 2.0 };
    fmi3_uint64_t nStartsExp = 2;
    const char* varName = "array2_64";
    fmi3_uint64_t dimSizesExp[1] = { 2 };
    size_t nDimsExp = 1;

    test_array_ok_64(xml, varName, (fmi3_float64_t*)startsExp, nStartsExp, dimSizesExp, nDimsExp);
}

/* parse 2x2 array */
static void test_array3_64(fmi3_import_t* xml)
{
    fmi3_float64_t startsExp[2][2] =        {{ 1.0, 2.0 },
                                             { 3.0, 4.0 }};
    fmi3_uint64_t nStartsExp = 4;
    const char* varName = "array3_64";
    fmi3_uint64_t dimSizesExp[2] = { 2, 2 };
    size_t nDimsExp = 2;

    test_array_ok_64(xml, varName, (fmi3_float64_t*)startsExp, nStartsExp, dimSizesExp, nDimsExp);
}

/* parse 2x2x2 array */
static void test_array4_64(fmi3_import_t* xml)
{
    fmi3_float64_t startsExp[2][2][2] = {{{1.0, 2.0}, {3.0, 4.0}}, {{5.0, 6.0}, {7.0, 8.0}}}; /* note to self: {: new row, {{: new column, {{{: new aisle */
    fmi3_uint64_t nStartsExp = 8;
    const char* varName = "array4_64";
    fmi3_uint64_t dimSizesExp[3] = { 2, 2, 2 };
    size_t nDimsExp = 3;

    test_array_ok_64(xml, varName, (fmi3_float64_t*)startsExp, nStartsExp, dimSizesExp, nDimsExp);
}

/* parse different kind of specifying float value via string */
static void test_array5_64(fmi3_import_t* xml)
{
    fmi3_float64_t startsExp[2][2] = {{ 123.456,     +1234.456 },
                                     { -1.2344e56,  -.45E-6   }};
    fmi3_uint64_t nStartsExp = 4;
    const char* varName = "array5_64";
    fmi3_uint64_t dimSizesExp[3] = { 2, 2 };
    size_t nDimsExp = 2;

    test_array_ok_64(xml, varName, (fmi3_float64_t*)startsExp, nStartsExp, dimSizesExp, nDimsExp);
}

/* general array tester for float32 */
static void test_array_ok_32(fmi3_import_t* xml, const char* varName, fmi3_float32_t* startsExp, fmi3_uint64_t arrSizeExp, fmi3_uint64_t* dimSizesExp, size_t nDimsExp)
{
    fmi3_import_variable_t* v;
    fmi3_float32_t* starts;
    fmi3_import_dimension_list_t* dimList;
    size_t nDims;
    fmi3_uint64_t* dimSizes;
    fmi3_uint64_t arrSize;

    jm_callbacks* cb = jm_get_default_callbacks();

    v = fmi3_import_get_variable_by_name(xml, varName);
    REQUIRE(v != nullptr);

    REQUIRE(fmi3_import_get_variable_has_start(v) != 0);
    REQUIRE(fmi3_import_variable_is_array(v) != 0);

    dimList = fmi3_import_get_variable_dimension_list(v);
    nDims = fmi3_import_get_dimension_list_size(dimList);

    /* check num dimensions */
    REQUIRE(nDims == nDimsExp);

    /* check resolved dimension start sizes */
    get_dimensions_start_sizes(xml, cb, dimList, &dimSizes);
    for (size_t i = 0; i < nDims; i++) {
        INFO("i = " << i);
        REQUIRE(dimSizes[i] == dimSizesExp[i]);
    }

    /* check num start values */
    starts = fmi3_import_get_float32_variable_start_array(fmi3_import_get_variable_as_float32(v));
    arrSize = get_array_size(dimSizes, nDims);
    REQUIRE(arrSize == arrSizeExp);

    /* check start values */
    for (size_t i = 0; i < arrSize; i++) {
        INFO("i = " << i);
        REQUIRE(starts[i] == startsExp[i]);
    }

    cb->free(dimSizes);
}

/* parse 1(x1) array */
static void test_array1_32(fmi3_import_t *xml)
{
    fmi3_float32_t startsExp[1] = { 1.0F };
    fmi3_uint64_t nStartsExp = 1;
    const char* varName = "array1_32";
    fmi3_uint64_t dimSizesExp[1] = { 1 };
    size_t nDimsExp = 1;

    test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, dimSizesExp, nDimsExp);
}

/* parse 1x2 array */
static void test_array2_32(fmi3_import_t* xml)
{
    fmi3_float32_t startsExp[2] = { 1.0F, 2.0F };
    fmi3_uint64_t nStartsExp = 2;
    const char* varName = "array2_32";
    fmi3_uint64_t dimSizesExp[1] = { 2 };
    size_t nDimsExp = 1;

    test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, dimSizesExp, nDimsExp);
}

/* parse 2x2 array */
static void test_array3_32(fmi3_import_t* xml)
{
    fmi3_float32_t startsExp[2][2] =        {{ 1.0F, 2.0F },
                                             { 3.0F, 4.0F }};
    fmi3_uint64_t nStartsExp = 4;
    const char* varName = "array3_32";
    fmi3_uint64_t dimSizesExp[2] = { 2, 2 };
    size_t nDimsExp = 2;

    test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, dimSizesExp, nDimsExp);
}

/* parse 2x2x2 array */
static void test_array4_32(fmi3_import_t* xml)
{
    fmi3_float32_t startsExp[2][2][2] = {{{1.0F, 2.0F}, {3.0F, 4.0F}}, {{5.0F, 6.0F}, {7.0F, 8.0F}}}; /* note to self: {: new row, {{: new column, {{{: new aisle */
    fmi3_uint64_t nStartsExp = 8;
    const char* varName = "array4_32";
    fmi3_uint64_t dimSizesExp[3] = { 2, 2, 2 };
    size_t nDimsExp = 3;

    test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, dimSizesExp, nDimsExp);
}

/* parse special float32 string representations */
static void test_array5_32(fmi3_import_t* xml)
{
    fmi3_float32_t startsExp[2][2] = {{ 123.456F,    +1234.456F },
                                     { -3.4028e36F, -.45E-6F   }}; /* -1.2344e56 < -FLT_MAX, so not using same as for double */
    fmi3_uint64_t nStartsExp = 4;
    const char* varName = "array5_32";
    fmi3_uint64_t dimSizesExp[2] = { 2, 2 };
    size_t nDimsExp = 2;

    test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, dimSizesExp, nDimsExp);
}

/* parse 1x2x3 array */
static void test_array6_32(fmi3_import_t* xml)
{
    fmi3_float32_t startsExp[1][2][3] = {{{1.0F, 2.0F, 3.0F}, {4.0F, 5.0F, 6.0F}}}; /* note to self: {: new row, {{: new column, {{{: new aisle */
    fmi3_uint64_t nStartsExp = 6;
    const char* varName = "array6_32";
    fmi3_uint64_t dimSizesExp[3] = { 1, 2, 3 };
    size_t nDimsExp = 3;

    test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, (fmi3_uint64_t*)dimSizesExp, nDimsExp);
}

/* parse 1x2 array, dimension size is specified by VR to constant integer */
static void test_array7_32(fmi3_import_t* xml)
{
    fmi3_float32_t startsExp[3] = { 1.0F, 2.0F, 3.3F };
    fmi3_uint64_t nStartsExp = 3;
    const char* varName = "array7_32";
    fmi3_uint64_t dimSizesExp[1] = { 3 };
    size_t nDimsExp = 1;

    test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, (fmi3_uint64_t*)dimSizesExp, nDimsExp);
}

/* parse 2x2x2 array,
    dim1: start value
    dim2: vr,
    dim3: start value
*/
static void test_array8_32(fmi3_import_t* xml)
{
    fmi3_float32_t startsExp[2][2][2] = {{{1.0F, 2.0F}, {3.0F, 4.0F}}, {{5.0F, 6.0F}, {7.0F, 8.0F}}};
    fmi3_uint64_t nStartsExp = 8;
    const char* varName = "array8_32";
    fmi3_uint64_t dimSizesExp[3] = { 2, 2, 2 };
    size_t nDimsExp = 3;

    test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, (fmi3_uint64_t*)dimSizesExp, nDimsExp);
}

/* parse 2x2x2 array: check that we can find vr + index of all dimensions that come from vr
    dim1: start value
    dim2: vr,
    dim3: start value
*/
static void test_array8_32_can_find_index_and_vr_of_dimensions(fmi3_import_t* xml)
{
    fmi3_import_variable_t* v;
    fmi3_float32_t startsExp[2][2][2] = {{{1.0F, 2.0F}, {3.0F, 4.0F}}, {{5.0F, 6.0F}, {7.0F, 8.0F}}};
    fmi3_float32_t* starts;

    unsigned int nDimsExp = 3;
    fmi3_uint64_t sizeTot = 1;

    fmi3_import_dimension_t* dim;
    fmi3_import_dimension_list_t* dimList;
    size_t nDims;

    v = fmi3_import_get_variable_by_name(xml, "array8_32");
    REQUIRE(v != nullptr);

    REQUIRE(fmi3_import_get_variable_has_start(v) != 0);
    REQUIRE(fmi3_import_variable_is_array(v) != 0);

    dimList = fmi3_import_get_variable_dimension_list(v);
    nDims = fmi3_import_get_dimension_list_size(dimList);

    REQUIRE(nDims == nDimsExp);

    /* check if vr or start attribute */
    dim = fmi3_import_get_dimension(dimList, 0);
    REQUIRE(fmi3_import_get_dimension_has_start(dim) != 0);
    REQUIRE(fmi3_import_get_dimension_has_vr(dim) == 0);

    dim = fmi3_import_get_dimension(dimList, 1);
    REQUIRE(fmi3_import_get_dimension_has_start(dim) == 0);
    REQUIRE(fmi3_import_get_dimension_has_vr(dim) != 0);

    dim = fmi3_import_get_dimension(dimList, 2);
    REQUIRE(fmi3_import_get_dimension_has_start(dim) != 0);
    REQUIRE(fmi3_import_get_dimension_has_vr(dim) == 0);

    /* check values (both start and integer) */
    for (size_t i = 0; i < nDims; i++) {
        INFO("i = " << i);
        dim = fmi3_import_get_dimension(dimList, i);
        int has_start = fmi3_import_get_dimension_has_start(dim);
        if (has_start) { // get dimension directly
            sizeTot *= fmi3_import_get_dimension_start(dim);
        } else { // has VR instead of start, resolve
            fmi3_value_reference_t dimVR = fmi3_import_get_dimension_vr(dim);
            sizeTot *= fmi3_import_get_uint32_variable_start(
                    (fmi3_import_uint32_variable_t*)fmi3_import_get_variable_by_vr(xml, dimVR));
        }
    }
    REQUIRE(sizeTot == 8);

    starts = fmi3_import_get_float32_variable_start_array(fmi3_import_get_variable_as_float32(v));
    for (size_t i = 0; i < sizeTot; i++) {
        INFO("i = " << i);
        fmi3_float32_t exp = *((fmi3_float32_t*)startsExp + i);
        fmi3_float32_t act = *(starts + i);
        REQUIRE(exp == act);
    }
}

TEST_CASE("Testing valid arrays") {
    const char* xmldir = FMI3_TEST_XML_DIR "/arrays/valid/base";
    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);

    /* test valid */
    test_array1_64(xml);
    test_array2_64(xml);
    test_array3_64(xml);
    test_array4_64(xml);
    test_array5_64(xml);
    test_array1_32(xml);
    test_array2_32(xml);
    test_array3_32(xml);
    test_array4_32(xml);
    test_array5_32(xml);
    test_array6_32(xml);
    test_array7_32(xml);
    test_array8_32(xml);
    test_array8_32_can_find_index_and_vr_of_dimensions(xml);

    fmi3_import_free(xml);
}

// TODO: Add proper checks for log messages
TEST_CASE("Testing invalid array; enclosed string between doubles") {
    const char* xmldir = FMI3_TEST_XML_DIR "/arrays/invalid/enclosed_string";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Float64': could not parse value for Float64 attribute 'start'="));
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Testing invalid array; contains both doubles and string") {
    const char* xmldir = FMI3_TEST_XML_DIR "/arrays/invalid/mixed_string_double";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Float64': could not parse value for Float64 attribute 'start'="));
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Testing invalid array; is string") {
    const char* xmldir = FMI3_TEST_XML_DIR "/arrays/invalid/string";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Float64': could not parse value for Float64 attribute 'start'="));
    fmi3_testutil_import_free(tfmu);
}
