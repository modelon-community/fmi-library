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
static int test_array1(fmi3_import_t *xml)
{
    fmi3_import_variable_t* v;
    fmi3_float64_t starts_exp[1] = { 1.0 };
    fmi3_float64_t* starts;
    const int* dims;
    size_t nDims;
    int is_array ;
    int i;
    int has_start;

    v = fmi3_import_get_variable_by_name(xml, "array1");
    ASSERT_MSG(v != NULL, "variable not found by name");

    has_start = fmi3_import_get_variable_has_start(v);
    ASSERT_MSG(has_start == 1, "no start value found");

    is_array = fmi3_import_variable_is_array(v);
    ASSERT_MSG(is_array, "wrong variable type: expected array, but wasn't");

    fmi3_import_variable_get_dimensions(xml, v, &dims, &nDims);
    ASSERT_MSG(dims[0] == 1, "wrong dimension size");

    starts = fmi3_import_get_float64_variable_start_array(v);
    for (i = 0; i < dims[0]; i++) {
        if (starts[i] != starts_exp[i]) {
            printf("test failed at loop idx: %d", i);
            ASSERT_MSG(0, "wrong start value of array variable");
        }
    }

    return TEST_OK;
}

int main(int argc, char **argv)
{
    fmi3_import_t *xml;
    int ret = 1;
    if (argc != 2) {
        printf("Usage: %s <path_to_dir_containing_arrays_modelDescription>\n", argv[0]);
        return CTEST_RETURN_FAIL;
    }

    printf("Running fmi3_import_arrays_test\n");

    xml = parse_xml(argv[1]);
    if (xml == NULL) {
        return CTEST_RETURN_FAIL;
    }

    ret &= test_array1(xml);

    fmi3_import_free(xml);
    return ret == 0 ? CTEST_RETURN_FAIL : CTEST_RETURN_SUCCESS;
}
