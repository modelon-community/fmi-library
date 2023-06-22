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

/**
 * Returns total size of array
 * TODO: should this perhaps be a util API method?
 */
static fmi3_uint64_t get_array_size(const fmi3_uint64_t* dimSizes, size_t nDim) {
    fmi3_uint64_t size = nDim > 0 ? 1 : 0;
    size_t i;

    for (i = 0; i < nDim; i++) {
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
static int get_dimensions_start_sizes(fmi3_import_t* fmu, jm_callbacks* cb, fmi3_import_dimension_list_t* dimList, fmi3_uint64_t** dimSizes) {
    size_t i;
    size_t nDims;

    nDims = fmi3_import_get_dimension_list_size(dimList);

    /* allocate memory for sizes array */
    *dimSizes = (fmi3_uint64_t*) cb->malloc(sizeof(fmi3_uint64_t) * nDims);
    if (!*dimSizes) {
        printf("unable to alloc memory for dimSizes, line: %d", __LINE__);
        return 1;
    }

    /* fill the array */
    for (i = 0; i < nDims; i++) {
        fmi3_import_dimension_t* d = fmi3_import_get_dimension_list_item(dimList, i);

        if (fmi3_import_get_dimension_has_vr(d)) {
            fmi3_value_reference_t dimVr = fmi3_import_get_dimension_vr(d);
            fmi3_import_uint32_variable_t* var = (fmi3_import_uint32_variable_t*)fmi3_import_get_variable_by_vr(fmu, dimVr);
            ASSERT_MSG(var != NULL, "variable that specifies dimension length was not found");
            *((*dimSizes) + i) = fmi3_import_get_uint32_variable_start(var);
        } else { /* has start */
            *((*dimSizes) + i) = fmi3_import_get_dimension_start(d);
        }
    }

    return 0;
}

/* general array tester for float64 */
static int test_array_ok_64(fmi3_import_t* xml, char* varName, fmi3_float64_t* startsExp, fmi3_uint64_t arrSizeExp, fmi3_uint64_t* dimSizesExp, size_t nDimsExp)
{
    int res = TEST_FAIL; /* test result variable */

    fmi3_import_variable_t* v;
    fmi3_float64_t* starts;
    fmi3_import_dimension_list_t* dimList;
    size_t nDims;
    int is_array;
    unsigned int i;
    int has_start;
    fmi3_uint64_t* dimSizes; /* needs to be int, because we resolve start attribute from Integer variable */
    fmi3_uint64_t arrSize;

    jm_callbacks* cb = jm_get_default_callbacks();

    v = fmi3_import_get_variable_by_name(xml, varName);
    ASSERT_MSG(v != NULL, "variable not found by name");

    has_start = fmi3_import_get_variable_has_start(v);
    ASSERT_MSG(has_start == 1, "no start value found");

    is_array = fmi3_import_variable_is_array(v);
    ASSERT_MSG(is_array, "wrong variable type: expected array, but wasn't");

    dimList = fmi3_import_get_variable_dimension_list(xml, v); /* allocates memory */
    nDims = fmi3_import_get_dimension_list_size(dimList);

    /* check num dimensions */
    ASSERT_MSG_GOTO(cleanup1, nDims == nDimsExp, "incorrect num of dims, exp: %d, act: %d", nDimsExp, nDims);

    /* check resolved dimension start sizes */
    ASSERT_MSG_GOTO(cleanup1, !get_dimensions_start_sizes(xml, cb, dimList, &dimSizes), "failed to get dimension start sizes"); /* allocates memory */
    for (i = 0; i < nDims; i++) {
        ASSERT_MSG_GOTO(cleanup2, dimSizes[i] == dimSizesExp[i], "incorrect dim size, idx: %d, exp: %llu, act: %llu", i, dimSizes[i], dimSizesExp[i]);
    }

    /* check num start values */
    starts = fmi3_import_get_float64_variable_start_array(fmi3_import_get_variable_as_float64(v));
    arrSize = get_array_size(dimSizes, nDims);
    ASSERT_MSG_GOTO(cleanup2, arrSize == arrSizeExp, "incorrect array size, exp: %d, act: %d", arrSize, arrSizeExp);

    /* check start values */
    for (i = 0; i < arrSize; i++) {
        ASSERT_MSG_GOTO(cleanup2, starts[i] == startsExp[i], "wrong start value of array variable, idx: %d, exp: %f, act: %f", i, starts[i], startsExp[i]);
    }

    res = TEST_OK;

cleanup2:
    fmi3_import_free_dimension_list(dimList);
cleanup1:
    cb->free(dimSizes);

    return res;
}

/* parse 1(x1) array */
static int test_array1_64(fmi3_import_t *xml)
{
    fmi3_float64_t startsExp[1] = { 1.0 };
    fmi3_uint64_t nStartsExp = 1;
    char* varName = "array1_64";
    fmi3_uint64_t dimSizesExp[1] = { 1 };
    size_t nDimsExp = 1;

    return test_array_ok_64(xml, varName, (fmi3_float64_t*)startsExp, nStartsExp, dimSizesExp, nDimsExp);
}

/* parse 1x2 array */
static int test_array2_64(fmi3_import_t* xml)
{
    fmi3_float64_t startsExp[2] = { 1.0, 2.0 };
    fmi3_uint64_t nStartsExp = 2;
    char* varName = "array2_64";
    fmi3_uint64_t dimSizesExp[1] = { 2 };
    size_t nDimsExp = 1;

    return test_array_ok_64(xml, varName, (fmi3_float64_t*)startsExp, nStartsExp, dimSizesExp, nDimsExp);
}

/* parse 2x2 array */
static int test_array3_64(fmi3_import_t* xml)
{
    fmi3_float64_t startsExp[2][2] =        {{ 1.0, 2.0 },
                                             { 3.0, 4.0 }};
    fmi3_uint64_t nStartsExp = 4;
    char* varName = "array3_64";
    fmi3_uint64_t dimSizesExp[2] = { 2, 2 };
    size_t nDimsExp = 2;

    return test_array_ok_64(xml, varName, (fmi3_float64_t*)startsExp, nStartsExp, dimSizesExp, nDimsExp);
}

/* parse 2x2x2 array */
static int test_array4_64(fmi3_import_t* xml)
{
    fmi3_float64_t startsExp[2][2][2] = {{{1.0, 2.0}, {3.0, 4.0}}, {{5.0, 6.0}, {7.0, 8.0}}}; /* note to self: {: new row, {{: new column, {{{: new aisle */
    fmi3_uint64_t nStartsExp = 8;
    char* varName = "array4_64";
    fmi3_uint64_t dimSizesExp[3] = { 2, 2, 2 };
    size_t nDimsExp = 3;

    return test_array_ok_64(xml, varName, (fmi3_float64_t*)startsExp, nStartsExp, dimSizesExp, nDimsExp);
}

/* parse different kind of specifying float value via string */
static int test_array5_64(fmi3_import_t* xml)
{
    fmi3_float64_t startsExp[2][2] = {{ 123.456,     +1234.456 },
                                     { -1.2344e56,  -.45E-6   }};
    fmi3_uint64_t nStartsExp = 4;
    char* varName = "array5_64";
    fmi3_uint64_t dimSizesExp[3] = { 2, 2 };
    size_t nDimsExp = 2;

    return test_array_ok_64(xml, varName, (fmi3_float64_t*)startsExp, nStartsExp, dimSizesExp, nDimsExp);
}

/* general array tester for float32 */
static int test_array_ok_32(fmi3_import_t* xml, char* varName, fmi3_float32_t* startsExp, fmi3_uint64_t arrSizeExp, fmi3_uint64_t* dimSizesExp, size_t nDimsExp)
{
    int res = TEST_FAIL; /* test result variable */

    fmi3_import_variable_t* v;
    fmi3_float32_t* starts;
    fmi3_import_dimension_list_t* dimList;
    size_t nDims;
    int is_array;
    unsigned int i;
    int has_start;
    fmi3_uint64_t* dimSizes;
    fmi3_uint64_t arrSize;

    jm_callbacks* cb = jm_get_default_callbacks();

    v = fmi3_import_get_variable_by_name(xml, varName);
    ASSERT_MSG(v != NULL, "variable not found by name");

    has_start = fmi3_import_get_variable_has_start(v);
    ASSERT_MSG(has_start == 1, "no start value found");

    is_array = fmi3_import_variable_is_array(v);
    ASSERT_MSG(is_array, "wrong variable type: expected array, but wasn't");

    dimList = fmi3_import_get_variable_dimension_list(xml, v); /* allocates memory */
    nDims = fmi3_import_get_dimension_list_size(dimList);

    /* check num dimensions */
    ASSERT_MSG_GOTO(cleanup1, nDims == nDimsExp, "incorrect num of dims, exp: %d, act: %d", nDimsExp, nDims);

    /* check resolved dimension start sizes */
    ASSERT_MSG_GOTO(cleanup1, !get_dimensions_start_sizes(xml, cb, dimList, &dimSizes), "failed to get dimension start sizes"); /* allocates memory */
    for (i = 0; i < nDims; i++) {
        ASSERT_MSG_GOTO(cleanup2, dimSizes[i] == dimSizesExp[i], "incorrect dim size, idx: %d, exp: %d, act: %d", i, dimSizes[i], dimSizesExp[i]);
    }

    /* check num start values */
    starts = fmi3_import_get_float32_variable_start_array(fmi3_import_get_variable_as_float32(v));
    arrSize = get_array_size(dimSizes, nDims);
    ASSERT_MSG_GOTO(cleanup2, arrSize == arrSizeExp, "incorrect array size, exp: %d, act: %d", arrSize, arrSizeExp);

    /* check start values */
    for (i = 0; i < arrSize; i++) {
        ASSERT_MSG_GOTO(cleanup2, starts[i] == startsExp[i], "wrong start value of array variable, idx: %d, exp: %f, act: %f", i, starts[i], startsExp[i]);
    }

    res = TEST_OK;

cleanup2:
    fmi3_import_free_dimension_list(dimList);
cleanup1:
    cb->free(dimSizes);

    return res;
}

/* parse 1(x1) array */
static int test_array1_32(fmi3_import_t *xml)
{
    fmi3_float32_t startsExp[1] = { 1.0F };
    fmi3_uint64_t nStartsExp = 1;
    char* varName = "array1_32";
    fmi3_uint64_t dimSizesExp[1] = { 1 };
    size_t nDimsExp = 1;

    return test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, dimSizesExp, nDimsExp);
}

/* parse 1x2 array */
static int test_array2_32(fmi3_import_t* xml)
{
    fmi3_float32_t startsExp[2] = { 1.0F, 2.0F };
    fmi3_uint64_t nStartsExp = 2;
    char* varName = "array2_32";
    fmi3_uint64_t dimSizesExp[1] = { 2 };
    size_t nDimsExp = 1;

    return test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, dimSizesExp, nDimsExp);
}

/* parse 2x2 array */
static int test_array3_32(fmi3_import_t* xml)
{
    fmi3_float32_t startsExp[2][2] =        {{ 1.0F, 2.0F },
                                             { 3.0F, 4.0F }};
    fmi3_uint64_t nStartsExp = 4;
    char* varName = "array3_32";
    fmi3_uint64_t dimSizesExp[2] = { 2, 2 };
    size_t nDimsExp = 2;

    return test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, dimSizesExp, nDimsExp);
}

/* parse 2x2x2 array */
static int test_array4_32(fmi3_import_t* xml)
{
    fmi3_float32_t startsExp[2][2][2] = {{{1.0F, 2.0F}, {3.0F, 4.0F}}, {{5.0F, 6.0F}, {7.0F, 8.0F}}}; /* note to self: {: new row, {{: new column, {{{: new aisle */
    fmi3_uint64_t nStartsExp = 8;
    char* varName = "array4_32";
    fmi3_uint64_t dimSizesExp[3] = { 2, 2, 2 };
    size_t nDimsExp = 3;

    return test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, dimSizesExp, nDimsExp);
}

/* parse special float32 string representations */
static int test_array5_32(fmi3_import_t* xml)
{
    fmi3_float32_t startsExp[2][2] = {{ 123.456F,    +1234.456F },
                                     { -3.4028e36F, -.45E-6F   }}; /* -1.2344e56 < -FLT_MAX, so not using same as for double */
    fmi3_uint64_t nStartsExp = 4;
    char* varName = "array5_32";
    fmi3_uint64_t dimSizesExp[2] = { 2, 2 };
    size_t nDimsExp = 2;

    return test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, dimSizesExp, nDimsExp);
}

/* parse 1x2x3 array */
static int test_array6_32(fmi3_import_t* xml)
{
    fmi3_float32_t startsExp[1][2][3] = {{{1.0F, 2.0F, 3.0F}, {4.0F, 5.0F, 6.0F}}}; /* note to self: {: new row, {{: new column, {{{: new aisle */
    fmi3_uint64_t nStartsExp = 6;
    char* varName = "array6_32";
    fmi3_uint64_t dimSizesExp[3] = { 1, 2, 3 };
    size_t nDimsExp = 3;

    return test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, (fmi3_uint64_t*)dimSizesExp, nDimsExp);
}

/* parse 1x2 array, dimension size is specified by VR to constant integer */
static int test_array7_32(fmi3_import_t* xml)
{
    fmi3_float32_t startsExp[3] = { 1.0F, 2.0F, 3.3F };
    fmi3_uint64_t nStartsExp = 3;
    char* varName = "array7_32";
    fmi3_uint64_t dimSizesExp[1] = { 3 };
    size_t nDimsExp = 1;

    return test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, (fmi3_uint64_t*)dimSizesExp, nDimsExp);
}

/* parse 2x2x2 array,
    dim1: start value
    dim2: vr,
    dim3: start value
*/
static int test_array8_32(fmi3_import_t* xml)
{
    fmi3_float32_t startsExp[2][2][2] = {{{1.0F, 2.0F}, {3.0F, 4.0F}}, {{5.0F, 6.0F}, {7.0F, 8.0F}}};
    fmi3_uint64_t nStartsExp = 8;
    char* varName = "array8_32";
    fmi3_uint64_t dimSizesExp[3] = { 2, 2, 2 };
    size_t nDimsExp = 3;

    return test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, (fmi3_uint64_t*)dimSizesExp, nDimsExp);
}

/* parse 2x2x2 array: check that we can find vr + index of all dimensions that come from vr
    dim1: start value
    dim2: vr,
    dim3: start value
*/
static int test_array8_32_can_find_index_and_vr_of_dimensions(fmi3_import_t* xml)
{
    fmi3_import_variable_t* v;
    fmi3_float32_t startsExp[2][2][2] = {{{1.0F, 2.0F}, {3.0F, 4.0F}}, {{5.0F, 6.0F}, {7.0F, 8.0F}}};
    fmi3_float32_t* starts;
    unsigned int i;
    int is_array;
    int has_start;

    unsigned int nDimsExp = 3;
    fmi3_uint64_t sizeTot = 1;

    fmi3_import_dimension_t* dim;
    fmi3_import_dimension_list_t* dimList;
    size_t nDims;

    int res = TEST_FAIL;

    v = fmi3_import_get_variable_by_name(xml, "array8_32");
    ASSERT_MSG(v != NULL, "variable not found by name");

    has_start = fmi3_import_get_variable_has_start(v);
    ASSERT_MSG(has_start == 1, "no start value found");

    is_array = fmi3_import_variable_is_array(v);
    ASSERT_MSG(is_array, "wrong variable type: expected array, but wasn't");

    dimList = fmi3_import_get_variable_dimension_list(xml, v); /* allocates memory */
    nDims = fmi3_import_get_dimension_list_size(dimList);

    ASSERT_MSG_GOTO(cleanup, nDims == nDimsExp, "wrong number of dimensions: %d, expected: %d", nDims, nDimsExp);

    /* check if vr or start attribute */
    dim = fmi3_import_get_dimension_list_item(dimList, 0);
    ASSERT_MSG_GOTO(cleanup, fmi3_import_get_dimension_has_start(dim), "failed to determine if start/vr");
    ASSERT_MSG_GOTO(cleanup, !fmi3_import_get_dimension_has_vr(dim), "failed to determine if start/vr");

    dim = fmi3_import_get_dimension_list_item(dimList, 1);
    ASSERT_MSG_GOTO(cleanup, !fmi3_import_get_dimension_has_start(dim), "failed to determine if start/vr");
    ASSERT_MSG_GOTO(cleanup, fmi3_import_get_dimension_has_vr(dim), "failed to determine if start/vr");

    dim = fmi3_import_get_dimension_list_item(dimList, 2);
    ASSERT_MSG_GOTO(cleanup, fmi3_import_get_dimension_has_start(dim), "failed to determine if start/vr");
    ASSERT_MSG_GOTO(cleanup, !fmi3_import_get_dimension_has_vr(dim), "failed to determine if start/vr");

    /* check values (both start and integer) */
    for (i = 0; i < nDims; i++) {
        dim = fmi3_import_get_dimension_list_item(dimList, i);
        if (fmi3_import_get_dimension_has_start(dim)) {
            sizeTot *= fmi3_import_get_dimension_start(dim);
        }
        else {
            /* TODO: this gives start-size, but it might change during runtime.
                the C API gives the values of an array, but it doesn't specify the arrays runtime size.
                If I understand the API correctly, WE should give the size of the array that is given to the function, not the reverse,
                and that requires us to first find the total size of the array (i.e. product of dimension runtime sizes)
            */
            /* if (get_from_xml) { */
            fmi3_value_reference_t dimVr = fmi3_import_get_dimension_vr(dim);
            sizeTot *= fmi3_import_get_uint32_variable_start(
                    (fmi3_import_uint32_variable_t*)fmi3_import_get_variable_by_vr(xml, dimVr));
            /* } else {  // runtime
                TODO: sizeTot *= fmi3_import_get_integer(...);
            */
        }
    }
    ASSERT_MSG_GOTO(cleanup, sizeTot == 8, "failed to check all array values, stopped at idx: %d", sizeTot);

    starts = fmi3_import_get_float32_variable_start_array(fmi3_import_get_variable_as_float32(v));
    for (i = 0; i < sizeTot; i++) {
        fmi3_float32_t exp = *((fmi3_float32_t*)startsExp + i);
        fmi3_float32_t act = *(starts + i);
        ASSERT_MSG_GOTO(cleanup, exp == act, "wrong start value of array variable, loop_idx: %d, exp: %f, act: %f", i, exp, act);
    }

    res = TEST_OK;

cleanup:
    fmi3_import_free_dimension_list(dimList);

    return res;
}

/** 
 * Attempt to parse a bad array. Test will fail if XML is successfully parsed.
 *   md_parent_dir_path: absolute path to the parent directory of the modelDescription.xml to parse
 */
static void test_array_bad(const char* md_parent_dir_path)
{
    fmi3_import_t* xml = parse_xml(md_parent_dir_path);
    REQUIRE(xml == nullptr);
}

// TODO: Name
TEST_CASE("Testing ") {
    char xmlPath[1000];
    size_t sizeXmlPath = sizeof(xmlPath) / sizeof(char);

    char dirPath[1000];
    size_t sizeDirPath = sizeof(dirPath) / sizeof(char);
    char* dirs[] = { "enclosed_string", "mixed_string_double", "string" };
    size_t nDirs = sizeof(dirs) / sizeof(char*);

    // fmi3_import_t *xml;
    int ret = 1;
    size_t i;

    printf("Running fmi3_import_arrays_test\n");

    // const char* xmldir = FMI3_TEST_XML_DIR "/variable_test/invalid/previous_self_reference";

    // fmi_testutil_build_xml_path(xmlPath, sizeXmlPath, "arrays", "/valid");
    // xml = parse_xml(xmlPath);
    const char* xmldir = FMI3_TEST_XML_DIR "/arrays/valid/base";
    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);

    /* test valid */
    ret &= test_array1_64(xml);
    ret &= test_array2_64(xml);
    ret &= test_array3_64(xml);
    ret &= test_array4_64(xml);
    ret &= test_array5_64(xml);

    ret &= test_array1_32(xml);
    ret &= test_array2_32(xml);
    ret &= test_array3_32(xml);
    ret &= test_array4_32(xml);
    ret &= test_array5_32(xml);
    ret &= test_array6_32(xml);
    ret &= test_array7_32(xml);
    ret &= test_array8_32(xml);
    ret &= test_array8_32_can_find_index_and_vr_of_dimensions(xml);

    fmi3_import_free(xml);
}

// TODO: name
TEST_CASE("Testing bad 1") {
    /*
     * Test bad
     *
     * NOTE: there are some invalid attribute values that are not being tested, but that would have incorrectly
     * passed if we did. Example: start="1e999", start="1a"
     * Regardless, I don't think FMIL should be responsible for full schema verification.

     * TODO: add test for fmi3_import_variable_is_array: give scalar as arg
     *
     */
    // fmi_testutil_build_xml_path(dirPath, sizeDirPath, "arrays", "/bad/");
    // printf("\nThe next tests are expected to fail...\n"); /* creating some space in the output log */
    // printf("---------------------------------------------------------------------------\n");
    // for (i = 0; i < nDirs; i++) {
    //     fmi_testutil_build_xml_path(xmlPath, sizeXmlPath, dirPath, dirs[i]);

    //     test_array_bad(xmlPath);
    // }
    // printf("---------------------------------------------------------------------------\n");
    const char* xmldir = FMI3_TEST_XML_DIR "/arrays/invalid/enclosed_string";
    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml == nullptr);
}

TEST_CASE("Testing bad 2") {
    const char* xmldir = FMI3_TEST_XML_DIR "/arrays/invalid/mixed_string_double";
    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml == nullptr);
}

TEST_CASE("Testing bad 3") {
    const char* xmldir = FMI3_TEST_XML_DIR "/arrays/invalid/string";
    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml == nullptr);
}
