#include <stdio.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"


/* Parse enum variable with minimal specified information. Tests defaults. */
static int test_enum_default_attrs(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "defaultsEnumVar");
    fmi3_import_enum_variable_t* ev;
    fmi3_import_variable_typedef_t* t;
    fmi3_import_enumeration_typedef_t* et;

    REQUIRE(v != NULL);
    REQUIRE(fmi3_import_get_variable_vr(v) == 1);
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
static int test_enum_all_attrs(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "allAttrsEnumVar");
    fmi3_import_enum_variable_t* ev;
    fmi3_import_variable_typedef_t* t;
    fmi3_import_enumeration_typedef_t* et;

    REQUIRE(v != NULL);
    REQUIRE(fmi3_import_get_variable_vr(v) == 2);
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

    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != NULL);
    
    SECTION("parse default enum attributes") {
        test_enum_default_attrs(xml);
    }
    SECTION("parse all enum attributes") {
        test_enum_all_attrs(xml);
    }

    fmi3_import_free(xml);
}
