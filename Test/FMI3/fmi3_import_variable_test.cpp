#include <stdio.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

static fmi3_import_t* parse_xml(const char* model_desc_path) {
    jm_callbacks* cb = jm_get_default_callbacks();
    fmi_import_context_t* ctx = fmi_import_allocate_context(cb);
    REQUIRE(ctx != NULL);

    fmi3_import_t* xml;
    xml = fmi3_import_parse_xml(ctx, model_desc_path, NULL);
    REQUIRE(xml != NULL);

    fmi_import_free_context(ctx);

    return xml;
}

/* Parse enum variable with minimal specified information. Tests defaults. */
static int enum_minimal_test(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "minEnumVar");
    fmi3_import_enum_variable_t* ev;
    fmi3_import_variable_typedef_t* t;
    fmi3_import_enumeration_typedef_t* et;

    REQUIRE(v != NULL);
    REQUIRE(fmi3_import_get_variable_vr(v) == 4);
    REQUIRE(fmi3_import_get_variable_description(v) == NULL); /* Default description is empty */
    REQUIRE(fmi3_import_get_causality(v) == fmi3_causality_enu_local);
    REQUIRE(fmi3_import_get_variability(v) == fmi3_variability_enu_discrete);
    REQUIRE(fmi3_import_get_initial(v) == fmi3_initial_enu_calculated);

    ev = fmi3_import_get_variable_as_enum(v);
    REQUIRE(ev != NULL);
    REQUIRE(fmi3_import_get_enum_variable_max(ev) == 3);
    REQUIRE(fmi3_import_get_enum_variable_min(ev) == 1);

    t = fmi3_import_get_variable_declared_type(v);
    REQUIRE(t != NULL);

    REQUIRE(strcmp(fmi3_import_get_type_name(t), "MyEnum") == 0);
    REQUIRE(strcmp(fmi3_import_get_type_quantity(t), "TypeQuantity") == 0);

    et = fmi3_import_get_type_as_enum(t);
    REQUIRE(et != NULL);
    REQUIRE(fmi3_import_get_enum_type_max(et) == 3);
    REQUIRE(fmi3_import_get_enum_type_min(et) == 1);
    REQUIRE(fmi3_import_get_enum_type_size(et) == 3);

    return 0;
}

/* Parse enum variable with all information specified */
static int enum_maximal_test(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "maxEnumVar");
    fmi3_import_enum_variable_t* ev;
    fmi3_import_variable_typedef_t* t;
    fmi3_import_enumeration_typedef_t* et;

    REQUIRE(v != NULL);
    REQUIRE(fmi3_import_get_variable_vr(v) == 5);
    REQUIRE(strcmp(fmi3_import_get_variable_description(v), "myDescription") == 0);
    REQUIRE(fmi3_import_get_causality(v) == fmi3_causality_enu_output);
    REQUIRE(fmi3_import_get_variability(v) == fmi3_variability_enu_discrete);
    REQUIRE(fmi3_import_get_initial(v) == fmi3_initial_enu_exact);

    ev = fmi3_import_get_variable_as_enum(v);
    REQUIRE(ev != NULL);
    REQUIRE(fmi3_import_get_enum_variable_max(ev) == 2);
    REQUIRE(fmi3_import_get_enum_variable_min(ev) == 1);
    REQUIRE(fmi3_import_get_enum_variable_start(ev) == 2);

    t = fmi3_import_get_variable_declared_type(v);
    REQUIRE(t != NULL);
    REQUIRE(strcmp(fmi3_import_get_type_name(t), "MyEnum") == 0);
    REQUIRE(strcmp(fmi3_import_get_type_quantity(t), "TypeQuantity") == 0);

    et = fmi3_import_get_type_as_enum(t);
    REQUIRE(et != NULL);
    REQUIRE(fmi3_import_get_enum_type_max(et) == 3);
    REQUIRE(fmi3_import_get_enum_type_min(et) == 1);
    REQUIRE(fmi3_import_get_enum_type_size(et) == 3);

    return 0;
}

TEST_CASE("Variable parsing", "[xml_variables]") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variable_test";

    fmi3_import_t* xml = parse_xml(xmldir);
    REQUIRE(xml != NULL);
    
    SECTION("enum minimal test") {
        REQUIRE(enum_minimal_test(xml) == 0);
    }
    SECTION("enum maximal test") {
        REQUIRE(enum_maximal_test(xml) == 0);
    }

    fmi3_import_free(xml);
}
