#include <stdio.h>

#include <fmilib.h>
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

/* Parse enum variable with minimal specified information. Tests defauts. */
static int enum_minimal_test(fmi2_import_t *xml)
{
    fmi2_import_variable_t *v = fmi2_import_get_variable_by_name(xml, "minEnumVar");
    fmi2_import_enum_variable_t *ev;
    fmi2_import_variable_typedef_t *t;
    fmi2_import_enumeration_typedef_t *et;

    ASSERT_MSG(v != NULL, "Could not find variable to test");
    ASSERT_MSG(fmi2_import_get_variable_vr(v) == 4, "Bad vr");
    ASSERT_MSG(fmi2_import_get_variable_description(v) == NULL,
               "Default description not empty");
    ASSERT_MSG(fmi2_import_get_causality(v) == fmi2_causality_enu_local,
               "Default causality should be local");
    ASSERT_MSG(fmi2_import_get_variability(v) == fmi2_variability_enu_discrete,
               "Variability should be discrete");
    ASSERT_MSG(fmi2_import_get_initial(v) == fmi2_initial_enu_calculated,
               "Default initial should be local");

    ev = fmi2_import_get_variable_as_enum(v);
    ASSERT_MSG(ev != NULL, "Failed to convert to enum variable");
    ASSERT_MSG(fmi2_import_get_enum_variable_max(ev) == 3, "Wrong enum max");
    ASSERT_MSG(fmi2_import_get_enum_variable_min(ev) == 1, "Wrong enum min");

    t = fmi2_import_get_variable_declared_type(v);
    ASSERT_MSG(t != NULL, "No declaredType found for enum variable");

    ASSERT_MSG(strcmp(fmi2_import_get_type_name(t), "MyEnum") == 0,
               "Wrong type name for enum");
    ASSERT_MSG(strcmp(fmi2_import_get_type_quantity(t), "TypeQuantity") == 0,
               "Wrong quantity in enum type definition");

    et = fmi2_import_get_type_as_enum(t);
    ASSERT_MSG(et != NULL, "Failed to convert to enum type definition");
    ASSERT_MSG(fmi2_import_get_enum_type_max(et) == 3, "Wrong enum type max");
    ASSERT_MSG(fmi2_import_get_enum_type_min(et) == 1, "Wrong enum type min");
    ASSERT_MSG(fmi2_import_get_enum_type_size(et) == 3,
               "Wrong number of enum items");

    return TEST_OK;
}

/* Parse enum variable with all information specified */
static int enum_maximal_test(fmi2_import_t *xml)
{
    fmi2_import_variable_t *v = fmi2_import_get_variable_by_name(xml, "maxEnumVar");
    fmi2_import_enum_variable_t *ev;
    fmi2_import_variable_typedef_t *t;
    fmi2_import_enumeration_typedef_t *et;
    ASSERT_MSG(v != NULL, "Could not find variable to test");
    ASSERT_MSG(fmi2_import_get_variable_vr(v) == 5, "Bad vr");
    ASSERT_MSG(strcmp(fmi2_import_get_variable_description(v), "myDescription") == 0,
               "Wrong description");
    ASSERT_MSG(fmi2_import_get_causality(v) == fmi2_causality_enu_output,
               "Causality should be output");
    ASSERT_MSG(fmi2_import_get_variability(v) == fmi2_variability_enu_discrete,
               "Variability should be discrete");
    ASSERT_MSG(fmi2_import_get_initial(v) == fmi2_initial_enu_exact,
               "Initial should be exact");

    ev = fmi2_import_get_variable_as_enum(v);
    ASSERT_MSG(ev != NULL, "Failed to convert to enum variable");
    ASSERT_MSG(fmi2_import_get_enum_variable_max(ev) == 2, "Wrong enum max");
    ASSERT_MSG(fmi2_import_get_enum_variable_min(ev) == 1, "Wrong enum min");
    ASSERT_MSG(fmi2_import_get_enum_variable_start(ev) == 2, "Wrong enum start");

    t = fmi2_import_get_variable_declared_type(v);
    ASSERT_MSG(t != NULL, "No declaredType found for enum variable");

    ASSERT_MSG(strcmp(fmi2_import_get_type_name(t), "MyEnum") == 0,
               "Wrong type name for enum");
    ASSERT_MSG(strcmp(fmi2_import_get_type_quantity(t), "TypeQuantity") == 0,
               "Wrong quantity in enum type definition");

    et = fmi2_import_get_type_as_enum(t);
    ASSERT_MSG(et != NULL, "Failed to convert to enum type definition");
    ASSERT_MSG(fmi2_import_get_enum_type_max(et) == 3, "Wrong enum type max");
    ASSERT_MSG(fmi2_import_get_enum_type_min(et) == 1, "Wrong enum type min");
    ASSERT_MSG(fmi2_import_get_enum_type_size(et) == 3,
               "Wrong number of enum items");

    return TEST_OK;
}

int main(int argc, char **argv)
{
    fmi2_import_t *xml;
    int ret = 1;
    if (argc != 2) {
        printf("Usage: %s <path to allBaseTypes modelDescription>\n", argv[0]);
        return CTEST_RETURN_FAIL;
    }

    printf("Running fmi2_import_variable_test\n");

    xml = parse_xml(argv[1]);
    if (xml == NULL) {
        return CTEST_RETURN_FAIL;
    }

    ret &= enum_minimal_test(xml);
    ret &= enum_maximal_test(xml);

    fmi2_import_free(xml);
    return ret == 0 ? CTEST_RETURN_FAIL : CTEST_RETURN_SUCCESS;
}
