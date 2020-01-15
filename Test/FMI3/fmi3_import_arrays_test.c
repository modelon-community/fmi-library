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

/* parse 1(x1) array */
static int test_array1_64(fmi3_import_t *xml)
{
    fmi3_import_variable_t* v;
    fmi3_float64_t starts_exp[1] = { 1.0 };
    fmi3_float64_t* starts;
    const unsigned int* dims;
    unsigned int nDims;
    int is_array ;
    unsigned int i;
    int has_start;

    v = fmi3_import_get_variable_by_name(xml, "array1_64");
    ASSERT_MSG(v != NULL, "variable not found by name");

    has_start = fmi3_import_get_variable_has_start(v);
    ASSERT_MSG(has_start == 1, "no start value found");

    is_array = fmi3_import_variable_is_array(v);
    ASSERT_MSG(is_array, "wrong variable type: expected array, but wasn't");

    fmi3_import_variable_get_dimensions(xml, v, &dims, &nDims);
    ASSERT_MSG(dims[0] == 1, "wrong dimension size");

    /* check that all start values are OK */
    starts = fmi3_import_get_float64_variable_start_array(fmi3_import_get_variable_as_float64(v));
    for (i = 0; i < dims[0]; i++) {
        if (starts[i] != starts_exp[i]) {
            printf("test failed at loop idx: %d", i);
            ASSERT_MSG(0, "wrong start value of array variable");
        }
    }

    return TEST_OK;
}

/* parse 1x2 array */
static int test_array2_64(fmi3_import_t* xml)
{
    fmi3_import_variable_t* v;
    fmi3_float64_t starts_exp[2] = { 1.0, 2.0 };
    fmi3_float64_t* starts;
    const unsigned int* dims;
    unsigned int nDims;
    int is_array;
    unsigned int i;
    int has_start;

    v = fmi3_import_get_variable_by_name(xml, "array2_64");
    ASSERT_MSG(v != NULL, "variable not found by name");

    has_start = fmi3_import_get_variable_has_start(v);
    ASSERT_MSG(has_start == 1, "no start value found");

    is_array = fmi3_import_variable_is_array(v);
    ASSERT_MSG(is_array, "wrong variable type: expected array, but wasn't");

    fmi3_import_variable_get_dimensions(xml, v, &dims, &nDims);
    ASSERT_MSG(dims[0] == 2, "wrong dimension size");

    /* check that all start values are OK */
    starts = fmi3_import_get_float64_variable_start_array(fmi3_import_get_variable_as_float64(v));
    for (i = 0; i < dims[0]; i++) {
        if (starts[i] != starts_exp[i]) {
            printf("test failed at loop idx: %d", i);
            ASSERT_MSG(0, "wrong start value of array variable");
        }
    }

    return TEST_OK;
}

/* parse 2x2 array */
static int test_array_2x2_64(fmi3_import_t* xml, const char* varName, fmi3_float64_t starts_exp[2][2])
{
    fmi3_import_variable_t* v;
    fmi3_float64_t* starts;
    const unsigned int* dims;
    unsigned int nDims;
    int is_array;
    int r, c;
    int has_start;
    int nRows;
    int nCols;

    v = fmi3_import_get_variable_by_name(xml, varName);
    ASSERT_MSG(v != NULL, "variable not found by name: '%s'", varName);

    has_start = fmi3_import_get_variable_has_start(v);
    ASSERT_MSG(has_start == 1, "no start value found");

    is_array = fmi3_import_variable_is_array(v);
    ASSERT_MSG(is_array, "wrong variable type: expected array, but wasn't");

    fmi3_import_variable_get_dimensions(xml, v, &dims, &nDims);
    ASSERT_MSG(dims[0] == 2, "wrong dimension size [0]");
    ASSERT_MSG(dims[1] == 2, "wrong dimension size [1]");

    nRows = dims[0];
    nCols = dims[1];

    /* check that all start values are OK */
    starts = fmi3_import_get_float64_variable_start_array(fmi3_import_get_variable_as_float64(v));
    for (r = 0; r < nRows; r++) {
        for (c = 0; c < nCols; c++) {
            if (starts[r * nCols + c] != starts_exp[r][c]) {
                printf("test failed at loop idx: %d", r);
                ASSERT_MSG(0, "wrong start value of array variable");
            }
        }
    }

    return TEST_OK;
}

/* parse 2x2x2 array */
static int test_array4_64(fmi3_import_t* xml)
{
    fmi3_import_variable_t* v;
    fmi3_float64_t starts_exp[2][2][2] = {{{1.0, 2.0}, {3.0, 4.0}}, {{5.0, 6.0}, {7.0, 8.0}}}; /* note to self: {: new row, {{: new column, {{{: new aisle */
    fmi3_float64_t* starts;
    const unsigned int* dims;
    unsigned int nDims;
    int is_array;
    int r, c, a;
    int has_start;
    int nRows;
    int nCols;
    int nAisles; /* 3rd dimension */

    v = fmi3_import_get_variable_by_name(xml, "array4_64");
    ASSERT_MSG(v != NULL, "variable not found by name");

    has_start = fmi3_import_get_variable_has_start(v);
    ASSERT_MSG(has_start == 1, "no start value found");

    is_array = fmi3_import_variable_is_array(v);
    ASSERT_MSG(is_array, "wrong variable type: expected array, but wasn't");

    fmi3_import_variable_get_dimensions(xml, v, &dims, &nDims);
    ASSERT_MSG(dims[0] == 2, "wrong dimension size [0]");
    ASSERT_MSG(dims[1] == 2, "wrong dimension size [1]");
    ASSERT_MSG(dims[2] == 2, "wrong dimension size [2]");

    nRows   = dims[0];
    nCols   = dims[1];
    nAisles = dims[2];

    /* check that all start values are OK */
    starts = fmi3_import_get_float64_variable_start_array(fmi3_import_get_variable_as_float64(v));
    for (r = 0; r < nRows; r++) {
        for (c = 0; c < nCols; c++) {
            for (a = 0; a < nAisles; a++) {
                if (starts[(r*nCols*nAisles) + (c*nAisles) + a] != starts_exp[r][c][a]) {
                    printf("test failed at loop idx: %d", r);
                    ASSERT_MSG(0, "wrong start value of array variable");
                }
            }
        }
    }

    return TEST_OK;
}

/* parse 1(x1) array */
static int test_array1_32(fmi3_import_t *xml)
{
    fmi3_import_variable_t* v;
    fmi3_float32_t starts_exp[1] = { 1.0 };
    fmi3_float32_t* starts;
    const unsigned int* dims;
    unsigned int nDims;
    int is_array ;
    unsigned int i;
    int has_start;

    v = fmi3_import_get_variable_by_name(xml, "array1_32");
    ASSERT_MSG(v != NULL, "variable not found by name");

    has_start = fmi3_import_get_variable_has_start(v);
    ASSERT_MSG(has_start == 1, "no start value found");

    is_array = fmi3_import_variable_is_array(v);
    ASSERT_MSG(is_array, "wrong variable type: expected array, but wasn't");

    fmi3_import_variable_get_dimensions(xml, v, &dims, &nDims);
    ASSERT_MSG(dims[0] == 1, "wrong dimension size");

    /* check that all start values are OK */
    starts = fmi3_import_get_float32_variable_start_array(fmi3_import_get_variable_as_float32(v));
    for (i = 0; i < dims[0]; i++) {
        if (starts[i] != starts_exp[i]) {
            printf("test failed at loop idx: %d", i);
            ASSERT_MSG(0, "wrong start value of array variable");
        }
    }

    return TEST_OK;
}

/* parse 1x2 array */
static int test_array2_32(fmi3_import_t* xml)
{
    fmi3_import_variable_t* v;
    fmi3_float32_t starts_exp[2] = { 1.0, 2.0 };
    fmi3_float32_t* starts;
    const unsigned int* dims;
    unsigned int nDims;
    int is_array;
    unsigned int i;
    int has_start;

    v = fmi3_import_get_variable_by_name(xml, "array2_32");
    ASSERT_MSG(v != NULL, "variable not found by name");

    has_start = fmi3_import_get_variable_has_start(v);
    ASSERT_MSG(has_start == 1, "no start value found");

    is_array = fmi3_import_variable_is_array(v);
    ASSERT_MSG(is_array, "wrong variable type: expected array, but wasn't");

    fmi3_import_variable_get_dimensions(xml, v, &dims, &nDims);
    ASSERT_MSG(dims[0] == 2, "wrong dimension size");

    /* check that all start values are OK */
    starts = fmi3_import_get_float32_variable_start_array(fmi3_import_get_variable_as_float32(v));
    for (i = 0; i < dims[0]; i++) {
        if (starts[i] != starts_exp[i]) {
            printf("test failed at loop idx: %d", i);
            ASSERT_MSG(0, "wrong start value of array variable");
        }
    }

    return TEST_OK;
}

/* parse 2x2 array */
static int test_array_2x2_32(fmi3_import_t* xml, const char* varName, fmi3_float32_t starts_exp[2][2])
{
    fmi3_import_variable_t* v;
    fmi3_float32_t* starts;
    const unsigned int* dims;
    unsigned int nDims;
    int is_array;
    int r, c;
    int has_start;
    int nRows;
    int nCols;

    v = fmi3_import_get_variable_by_name(xml, varName);
    ASSERT_MSG(v != NULL, "variable not found by name: '%s'", varName);

    has_start = fmi3_import_get_variable_has_start(v);
    ASSERT_MSG(has_start == 1, "no start value found");

    is_array = fmi3_import_variable_is_array(v);
    ASSERT_MSG(is_array, "wrong variable type: expected array, but wasn't");

    fmi3_import_variable_get_dimensions(xml, v, &dims, &nDims);
    ASSERT_MSG(dims[0] == 2, "wrong dimension size [0]");
    ASSERT_MSG(dims[1] == 2, "wrong dimension size [1]");

    nRows = dims[0];
    nCols = dims[1];

    /* check that all start values are OK */
    starts = fmi3_import_get_float32_variable_start_array(fmi3_import_get_variable_as_float32(v));
    for (r = 0; r < nRows; r++) {
        for (c = 0; c < nCols; c++) {
            if (starts[r * nCols + c] != starts_exp[r][c]) {
                printf("test failed at loop idx: %d", r);
                ASSERT_MSG(0, "wrong start value of array variable");
            }
        }
    }

    return TEST_OK;
}

/* parse 2x2x2 array */
static int test_array4_32(fmi3_import_t* xml)
{
    fmi3_import_variable_t* v;
    fmi3_float32_t starts_exp[2][2][2] = {{{1.0, 2.0}, {3.0, 4.0}}, {{5.0, 6.0}, {7.0, 8.0}}}; /* note to self: {: new row, {{: new column, {{{: new aisle */
    fmi3_float32_t* starts;
    const unsigned int* dims;
    unsigned int nDims;
    int is_array;
    int r, c, a;
    int has_start;
    int nRows;
    int nCols;
    int nAisles; /* 3rd dimension */

    v = fmi3_import_get_variable_by_name(xml, "array4_32");
    ASSERT_MSG(v != NULL, "variable not found by name");

    has_start = fmi3_import_get_variable_has_start(v);
    ASSERT_MSG(has_start == 1, "no start value found");

    is_array = fmi3_import_variable_is_array(v);
    ASSERT_MSG(is_array, "wrong variable type: expected array, but wasn't");

    fmi3_import_variable_get_dimensions(xml, v, &dims, &nDims);
    ASSERT_MSG(dims[0] == 2, "wrong dimension size [0]");
    ASSERT_MSG(dims[1] == 2, "wrong dimension size [1]");
    ASSERT_MSG(dims[2] == 2, "wrong dimension size [2]");

    nRows   = dims[0];
    nCols   = dims[1];
    nAisles = dims[2];

    /* check that all start values are OK */
    starts = fmi3_import_get_float32_variable_start_array(fmi3_import_get_variable_as_float32(v));
    for (r = 0; r < nRows; r++) {
        for (c = 0; c < nCols; c++) {
            for (a = 0; a < nAisles; a++) {
                fmi3_float32_t exp = starts_exp[r][c][a];
                fmi3_float32_t act = starts[(r * nCols * nAisles) + (c * nAisles) + a];
                ASSERT_MSG(exp == act, "wrong start value of array variable, loop_idx: %d:%d:%d, exp: %f, act: %f", r, c, a, exp, act);
            }
        }
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

    fmi3_float64_t starts_exp_array3_64[2][2] =        {{ 1.0, 2.0 },
                                                        { 3.0, 4.0 }};
    fmi3_float32_t starts_exp_array3_32[2][2] =        {{ 1.0F, 2.0F },
                                                        { 3.0F, 4.0F }};
    fmi3_float64_t starts_exp_array5_64[2][2] = {{ 123.456,     +1234.456 },
                                                 { -1.2344e56,  -.45E-6   }};
    fmi3_float32_t starts_exp_array5_32[2][2] = {{ 123.456F,    +1234.456F },
                                                 { -3.4028e36F, -.45E-6F   }}; /* -1.2344e56 < -FLT_MAX, so not using same as for double */

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
    ret &= test_array_2x2_64(xml, "array3_64", starts_exp_array3_64);
    ret &= test_array4_64(xml);
    ret &= test_array_2x2_64(xml, "array5_64", starts_exp_array5_64);

    ret &= test_array1_32(xml);
    ret &= test_array2_32(xml);
    ret &= test_array_2x2_32(xml, "array3_32", starts_exp_array3_32);
    ret &= test_array4_32(xml);
    ret &= test_array_2x2_32(xml, "array5_32", starts_exp_array5_32);

    fmi3_import_free(xml);

    /*
     * test bad
     *
     * NOTE: there are some invalid attribute values that are not being tested, but that would have incorrectly
     * passed if we did. Example: start="1e999", start="1a"
     * Regardless, I don't think FMIL should be responsible for full schema verification.
     */
    for (i = 0; i < NUM_BAD_DIRS; i++) {
        strncpy(path_bad_specific, path_bad_base, BUFSIZE);
        strncat(path_bad_specific, "/", BUFSIZE);
        strncat(path_bad_specific, dirs_bad[i], BUFSIZE);

        ret &= test_array_bad(path_bad_specific);
    }
    
    return ret == 0 ? CTEST_RETURN_FAIL : CTEST_RETURN_SUCCESS;
}
