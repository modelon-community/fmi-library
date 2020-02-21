#include <stdio.h>
#include <float.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmil_test.h"

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
    fmi1_import_variable_typedef_t *t;
    fmi1_import_type_definitions_t* tds;
    unsigned int n_tds;
    unsigned int i; /* td_idx */
    const char* tname;
    const char* str_empty = "";

    tds = fmi1_import_get_type_definitions(xml);
    ASSERT_MSG(tds != NULL, "could not get type definitions");
    n_tds = fmi1_import_get_type_definition_number(tds);
    ASSERT_MSG(n_tds >= 5, "too few typedefs found");

    /* check types that should by default return empty string */
    for (i = 0; i < n_tds ; i++) {
        t = fmi1_import_get_typedef(tds, i);
        tname = fmi1_import_get_type_name(t);
        if (    !strcmp("name_real", tname) ||
                !strcmp("name_integer", tname) ||
                !strcmp("name_enumeration", tname)) {
            ASSERT_MSG(strcmp(str_empty, fmi1_import_get_type_quantity(t)) == 0, "expected default quantity to be an empty string");
        } else if (    !strcmp("name_string", tname) ||
                       !strcmp("name_boolean", tname)) {
            ASSERT_MSG(fmi1_import_get_type_quantity(t) == NULL, "expected default quantity to be null ptr");
        }
    }

    return TEST_OK;
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

    ret &= test_quantity_default(xml);

    fmi1_import_free(xml);
    return ret == 0 ? CTEST_RETURN_FAIL : CTEST_RETURN_SUCCESS;
}
