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

/**
 * Returns total size of array
 * TODO: should this perhaps be a util API method?
 */
static size_t get_array_size(const int* dimSizes, size_t nDim) {
    size_t size = nDim > 0 ? 1 : 0;
    size_t i;

    for (i = 0; i < nDim; i++) {
        size *= *(dimSizes++);
    }

    return size;
}

/**
 * Get array with dimension start sizes, resolved from VRs if necessary.
 * @param dimSizes: contains the start sizes, must be freed
 * TODO: should this perhaps be a util API method?
 */
static int get_dimensions_start_sizes(fmi3_import_t* fmu, jm_callbacks* cb, fmi3_xml_dimension_t* dims, size_t nDims, int** dimSizes) {
    size_t i;
    int* sizes;

    sizes = *dimSizes = cb->malloc(sizeof(int) * nDims);
    if (!sizes) {
        return 1;
        assert("failed to get memory");
    }

    for (i = 0; i < nDims; i++) {
        fmi3_xml_dimension_t* d = dims + i;

        if (fmi3_import_get_dimension_has_vr(d)) {
            fmi3_import_integer_variable_t* var = (fmi3_import_integer_variable_t*)fmi3_import_get_variable_by_vr(fmu, fmi3_base_type_int, fmi3_import_get_dimension_vr(d));
            *((*dimSizes) + i) = fmi3_import_get_integer_variable_start(var);
        } else { /* has start */
            *((*dimSizes) + i) = fmi3_import_get_dimension_start(d);
        }
    }

    return 0;
}

/* general array tester for float64 */
static int test_array_ok_64(fmi3_import_t* xml, char* varName, fmi3_float64_t* startsExp, size_t nStartsExp, size_t* dimSizesExp, size_t nDimsExp)
{
    int res = TEST_FAIL; /* test result variable */

    fmi3_import_variable_t* v;
    fmi3_float64_t* starts;
    fmi3_import_dimension_t* dims;
    size_t nDims;
    int is_array;
    unsigned int i;
    int has_start;
    int* dimSizes; /* needs to be int, because we resolve start attribute from Integer variable */
    size_t arraySize;

    jm_callbacks* cb = jm_get_default_callbacks();

    v = fmi3_import_get_variable_by_name(xml, varName);
    ASSERT_MSG(v != NULL, "variable not found by name");

    has_start = fmi3_import_get_variable_has_start(v);
    ASSERT_MSG(has_start == 1, "no start value found");

    is_array = fmi3_import_variable_is_array(v);
    ASSERT_MSG(is_array, "wrong variable type: expected array, but wasn't");

    fmi3_import_get_variable_dimensions(v, &dims, &nDims);

    /* check num dimensions */
    ASSERT_MSG(nDims == nDimsExp, "incorrect num of dims, exp: %d, act: %d", nDimsExp, nDims);

    /* check resolved dimension start sizes */
    get_dimensions_start_sizes(xml, cb, dims, nDims, &dimSizes);
    for (i = 0; i < nDims; i++) {
        ASSERT_MSG_GOTO(cleanup, dimSizes[i] == dimSizesExp[i], "incorrect dim size, idx: %d, exp: %d, act: %d", i, dimSizes[i], dimSizesExp[i]);
    }

    /* check num start values */
    starts = fmi3_import_get_float64_variable_start_array(fmi3_import_get_variable_as_float64(v));
    arraySize = get_array_size(dimSizes, nDims);
    ASSERT_MSG_GOTO(cleanup, arraySize == nStartsExp, "incorrect array size, exp: %d, act: %d", arraySize, nStartsExp);

    /* check start values */
    for (i = 0; i < arraySize; i++) {
        ASSERT_MSG_GOTO(cleanup, starts[i] == startsExp[i], "wrong start value of array variable, idx: %d, exp: %f, act: %f", i, starts[i], startsExp[i]);
    }

    res = TEST_OK;

cleanup:
    cb->free(dimSizes);
    return res;
}

/* parse 1(x1) array */
static int test_array1_64(fmi3_import_t *xml)
{
    fmi3_float64_t startsExp[1] = { 1.0 };
    size_t nStartsExp = 1;
    char* varName = "array1_64";
    size_t dimSizesExp[1] = { 1 };
    size_t nDimsExp = 1;

    return test_array_ok_64(xml, varName, (fmi3_float64_t*)startsExp, nStartsExp, (size_t*)dimSizesExp, nDimsExp);
}

/* parse 1x2 array */
static int test_array2_64(fmi3_import_t* xml)
{
    fmi3_float64_t startsExp[2] = { 1.0, 2.0 };
    size_t nStartsExp = 2;
    char* varName = "array2_64";
    size_t dimSizesExp[1] = { 2 };
    size_t nDimsExp = 1;

    return test_array_ok_64(xml, varName, (fmi3_float64_t*)startsExp, nStartsExp, (size_t*)dimSizesExp, nDimsExp);
}

/* parse 2x2 array */
static int test_array3_64(fmi3_import_t* xml)
{
    fmi3_float64_t startsExp[2][2] =        {{ 1.0, 2.0 },
                                             { 3.0, 4.0 }};
    size_t nStartsExp = 4;
    char* varName = "array3_64";
    size_t dimSizesExp[2] = { 2, 2 };
    size_t nDimsExp = 2;

    return test_array_ok_64(xml, varName, (fmi3_float64_t*)startsExp, nStartsExp, (size_t*)dimSizesExp, nDimsExp);
}

/* parse 2x2x2 array */
static int test_array4_64(fmi3_import_t* xml)
{
    fmi3_float64_t startsExp[2][2][2] = {{{1.0, 2.0}, {3.0, 4.0}}, {{5.0, 6.0}, {7.0, 8.0}}}; /* note to self: {: new row, {{: new column, {{{: new aisle */
    size_t nStartsExp = 8;
    char* varName = "array4_64";
    size_t dimSizesExp[3] = { 2, 2, 2 };
    size_t nDimsExp = 3;

    return test_array_ok_64(xml, varName, (fmi3_float64_t*)startsExp, nStartsExp, (size_t*)dimSizesExp, nDimsExp);
}

/* parse different kind of specifying float value via string */
static int test_array5_64(fmi3_import_t* xml)
{
    fmi3_float64_t startsExp[2][2] = {{ 123.456,     +1234.456 },
                                     { -1.2344e56,  -.45E-6   }};
    size_t nStartsExp = 4;
    char* varName = "array5_64";
    size_t dimSizesExp[3] = { 2, 2 };
    size_t nDimsExp = 2;

    return test_array_ok_64(xml, varName, (fmi3_float64_t*)startsExp, nStartsExp, (size_t*)dimSizesExp, nDimsExp);
}

/* general array tester for float32 */
static int test_array_ok_32(fmi3_import_t* xml, char* varName, fmi3_float32_t* startsExp, size_t nStartsExp, size_t* dimSizesExp, size_t nDimsExp)
{
    int res = TEST_FAIL; /* test result variable */

    fmi3_import_variable_t* v;
    fmi3_float32_t* starts;
    fmi3_import_dimension_t* dims;
    size_t nDims;
    int is_array;
    unsigned int i;
    int has_start;
    int* dimSizes; /* needs to be int, because we resolve from integer variable */
    size_t arraySize;

    jm_callbacks* cb = jm_get_default_callbacks();

    v = fmi3_import_get_variable_by_name(xml, varName);
    ASSERT_MSG(v != NULL, "variable not found by name");

    has_start = fmi3_import_get_variable_has_start(v);
    ASSERT_MSG(has_start == 1, "no start value found");

    is_array = fmi3_import_variable_is_array(v);
    ASSERT_MSG(is_array, "wrong variable type: expected array, but wasn't");

    fmi3_import_get_variable_dimensions(v, &dims, &nDims);

    /* check num dimensions */
    ASSERT_MSG(nDims == nDimsExp, "incorrect num of dims, exp: %d, act: %d", nDimsExp, nDims);

    /* check resolved dimension start sizes */
    get_dimensions_start_sizes(xml, cb, dims, nDims, &dimSizes);
    for (i = 0; i < nDims; i++) {
        ASSERT_MSG_GOTO(cleanup, dimSizes[i] == dimSizesExp[i], "incorrect dim size, idx: %d, exp: %d, act: %d", i, dimSizes[i], dimSizesExp[i]);
    }

    /* check num start values */
    starts = fmi3_import_get_float32_variable_start_array(fmi3_import_get_variable_as_float32(v));
    arraySize = get_array_size(dimSizes, nDims);
    ASSERT_MSG_GOTO(cleanup, arraySize == nStartsExp, "incorrect array size, exp: %d, act: %d", arraySize, nStartsExp);

    /* check start values */
    for (i = 0; i < arraySize; i++) {
        ASSERT_MSG_GOTO(cleanup, starts[i] == startsExp[i], "wrong start value of array variable, idx: %d, exp: %f, act: %f", i, starts[i], startsExp[i]);
    }

    res = TEST_OK;

cleanup:
    cb->free(dimSizes);
    return res;
}

/* parse 1(x1) array */
static int test_array1_32(fmi3_import_t *xml)
{
    fmi3_float32_t startsExp[1] = { 1.0F };
    size_t nStartsExp = 1;
    char* varName = "array1_32";
    size_t dimSizesExp[1] = { 1 };
    size_t nDimsExp = 1;

    return test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, (size_t*)dimSizesExp, nDimsExp);
}

/* parse 1x2 array */
static int test_array2_32(fmi3_import_t* xml)
{
    fmi3_float32_t startsExp[2] = { 1.0F, 2.0F };
    size_t nStartsExp = 2;
    char* varName = "array2_32";
    size_t dimSizesExp[1] = { 2 };
    size_t nDimsExp = 1;

    return test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, (size_t*)dimSizesExp, nDimsExp);
}

/* parse 2x2 array */
static int test_array3_32(fmi3_import_t* xml)
{
    fmi3_float32_t startsExp[2][2] =        {{ 1.0F, 2.0F },
                                             { 3.0F, 4.0F }};
    size_t nStartsExp = 4;
    char* varName = "array3_32";
    size_t dimSizesExp[2] = { 2, 2 };
    size_t nDimsExp = 2;

    return test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, (size_t*)dimSizesExp, nDimsExp);
}

/* parse 2x2x2 array */
static int test_array4_32(fmi3_import_t* xml)
{
    fmi3_float32_t startsExp[2][2][2] = {{{1.0F, 2.0F}, {3.0F, 4.0F}}, {{5.0F, 6.0F}, {7.0F, 8.0F}}}; /* note to self: {: new row, {{: new column, {{{: new aisle */
    size_t nStartsExp = 8;
    char* varName = "array4_32";
    size_t dimSizesExp[3] = { 2, 2, 2 };
    size_t nDimsExp = 3;

    return test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, (size_t*)dimSizesExp, nDimsExp);
}

/* parse special float32 string representations */
static int test_array5_32(fmi3_import_t* xml)
{
    fmi3_float32_t startsExp[2][2] = {{ 123.456F,    +1234.456F },
                                     { -3.4028e36F, -.45E-6F   }}; /* -1.2344e56 < -FLT_MAX, so not using same as for double */
    size_t nStartsExp = 4;
    char* varName = "array5_32";
    size_t dimSizesExp[2] = { 2, 2 };
    size_t nDimsExp = 2;

    return test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, (size_t*)dimSizesExp, nDimsExp);
}

/* parse 1x2x3 array */
static int test_array6_32(fmi3_import_t* xml)
{
    fmi3_float32_t startsExp[1][2][3] = {{{1.0F, 2.0F, 3.0F}, {4.0F, 5.0F, 6.0F}}}; /* note to self: {: new row, {{: new column, {{{: new aisle */
    size_t nStartsExp = 6;
    char* varName = "array6_32";
    size_t dimSizesExp[3] = { 1, 2, 3 };
    size_t nDimsExp = 3;

    return test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, (size_t*)dimSizesExp, nDimsExp);
}

/* parse 1x2 array, dimension size is specified by VR to constant integer */
static int test_array7_32(fmi3_import_t* xml)
{
    fmi3_float32_t startsExp[3] = { 1.0F, 2.0F, 3.3F };
    size_t nStartsExp = 3;
    char* varName = "array7_32";
    size_t dimSizesExp[1] = { 3 };
    size_t nDimsExp = 1;

    return test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, (size_t*)dimSizesExp, nDimsExp);
}

/* parse 2x2x2 array,
    dim1: start value
    dim2: vr,
    dim3: start value
*/
static int test_array8_32(fmi3_import_t* xml)
{
    fmi3_float32_t startsExp[2][2][2] = {{{1.0F, 2.0F}, {3.0F, 4.0F}}, {{5.0F, 6.0F}, {7.0F, 8.0F}}};
    size_t nStartsExp = 8;
    char* varName = "array8_32";
    size_t dimSizesExp[3] = { 2, 2, 2 };
    size_t nDimsExp = 3;

    return test_array_ok_32(xml, varName, (fmi3_float32_t*)startsExp, nStartsExp, (size_t*)dimSizesExp, nDimsExp);
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
    unsigned int arrSizeExp = 8;
    unsigned int i;
    int is_array;
    int has_start;

    unsigned int nDimsExp = 3;
    size_t sizeTot = 1;

    fmi3_import_dimension_t* dim;
    fmi3_import_dimension_t* dims;
    size_t nDims;

    v = fmi3_import_get_variable_by_name(xml, "array8_32");
    ASSERT_MSG(v != NULL, "variable not found by name");

    has_start = fmi3_import_get_variable_has_start(v);
    ASSERT_MSG(has_start == 1, "no start value found");

    is_array = fmi3_import_variable_is_array(v);
    ASSERT_MSG(is_array, "wrong variable type: expected array, but wasn't");

    fmi3_import_get_variable_dimensions(v, &dims, &nDims);

    ASSERT_MSG(nDims == nDimsExp, "wrong number of dimensions: %d, expected: %d", nDims, nDimsExp);

    /* check if vr or start attribute */
    dim = dims + 0;
    ASSERT_MSG(fmi3_import_get_dimension_has_start(dim), "failed to determine if start/vr");
    ASSERT_MSG(!fmi3_import_get_dimension_has_vr(dim), "failed to determine if start/vr");

    dim = dims + 1;
    ASSERT_MSG(!fmi3_import_get_dimension_has_start(dim), "failed to determine if start/vr");
    ASSERT_MSG(fmi3_import_get_dimension_has_vr(dim), "failed to determine if start/vr");

    dim = dims + 2;
    ASSERT_MSG(fmi3_import_get_dimension_has_start(dim), "failed to determine if start/vr");
    ASSERT_MSG(!fmi3_import_get_dimension_has_vr(dim), "failed to determine if start/vr");

    /* check values (both start and integer) */
    for (i = 0; i < nDims; i++) {
        dim = dims + i;
        if (fmi3_import_get_dimension_has_start(dim)) {
            sizeTot *= fmi3_import_get_dimension_start(dim);
        }
        else {
            /* TODO: this gives start-size, but it might change during runtime.
                the C API gives the values of an array, but it doesn't specify the arrays runtime size.
                If I understand the API correctly, WE should give the size of the array that is given to the function, not the reverse,
                and that requires us to first find the total size of the array (i.e. product of dimension runtime sizes)
            */
            /* if (import_time) { */
            sizeTot *= fmi3_import_get_integer_variable_start(
                (fmi3_import_integer_variable_t*)fmi3_import_get_variable_by_vr(xml, fmi3_base_type_int, fmi3_import_get_dimension_vr(dim)));
            /* } else {  // runtime
                TODO: sizeTot *= fmi3_import_get_integer(...);
            */
        }
    }
    ASSERT_MSG(sizeTot == 8, "failed to check all array values, stopped at idx: %d", sizeTot);

    starts = fmi3_import_get_float32_variable_start_array(fmi3_import_get_variable_as_float32(v));
    for (i = 0; i < sizeTot; i++) {
        fmi3_float32_t exp = *((fmi3_float32_t*)startsExp + i);
        fmi3_float32_t act = *(starts + i);
        ASSERT_MSG(exp == act, "wrong start value of array variable, loop_idx: %d, exp: %f, act: %f", i, exp, act);
    }

    return TEST_OK;
}

/** 
 * Attempt to parse a bad array. Test will fail if XML is successfully parsed.
 *   md_parent_dir_path: absolute path to the parent directory of the modelDescription.xml to parse
 */
static int test_array_bad(const char* md_parent_dir_path)
{
    fmi3_import_t* xml = parse_xml(md_parent_dir_path);

    if (xml != NULL) {
        fmi3_import_free(xml);
        TEST_FAILED("unexpected non-NULL return value from parsing xml: %s", md_parent_dir_path);
    }

    return TEST_OK;
}

int main(int argc, char **argv)
{
#define BUFSIZE (1000)
    char path_valid[BUFSIZE];
    char path_bad_base[BUFSIZE];
    char path_bad_specific[BUFSIZE];
    const char* path_base;

    /* I don't want to write platform dependent code for getting all files in directory, so just hard coding all files for now */
#define NUM_BAD_DIRS (3)
    char dirs_bad[NUM_BAD_DIRS][BUFSIZE] = { "enclosed_string", "mixed_string_double", "string" };

    fmi3_import_t *xml;
    int ret = 1;
    int i;


    if (argc != 2) {
        printf("Usage: %s <path_to_(parser_test_xmls/arrays)>\n", argv[0]);
        return CTEST_RETURN_FAIL;
    }

    /* build paths to parents of modelDescriptions */
    path_base = argv[1];                                    /* .../parser_test_xmls/arrays */

#define INPUT_LEN_MAX 700                                   /* arbitrarily choosen */
    ASSERT_MSG(strlen(path_base) < INPUT_LEN_MAX,
            "Unreasonably long base path as input for test. Num chars: %d", strlen(path_base));

    strncpy(path_valid, path_base, BUFSIZE);
    strncat(path_valid, "/valid", BUFSIZE);

    strncpy(path_bad_base, path_base, BUFSIZE);
    strncat(path_bad_base, "/bad", BUFSIZE);

    printf("Running fmi3_import_arrays_test\n");

    xml = parse_xml(path_valid);
    if (xml == NULL) {
        return CTEST_RETURN_FAIL;
    }

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

    /*
     * test bad
     *
     * NOTE: there are some invalid attribute values that are not being tested, but that would have incorrectly
     * passed if we did. Example: start="1e999", start="1a"
     * Regardless, I don't think FMIL should be responsible for full schema verification.

     * TODO: add test for fmi3_import_variable_is_array: give scalar as arg
     *
     */
    printf("\nThe following tests are expected to fail...\n"); /* creating some space in the output log */
    printf("---------------------------------------------------------------------------\n");
    for (i = 0; i < NUM_BAD_DIRS; i++) {
        strncpy(path_bad_specific, path_bad_base, BUFSIZE);
        strncat(path_bad_specific, "/", BUFSIZE);
        strncat(path_bad_specific, dirs_bad[i], BUFSIZE);

        ret &= test_array_bad(path_bad_specific);
    }
    printf("---------------------------------------------------------------------------\n");
    
    return ret == 0 ? CTEST_RETURN_FAIL : CTEST_RETURN_SUCCESS;
}
