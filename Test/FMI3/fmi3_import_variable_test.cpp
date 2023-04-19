#include <stdio.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"


/* Parse enum variable with minimal specified information. Tests defaults. */
static void test_enum_default_attrs(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "defaultEnumVar");
    fmi3_import_enum_variable_t* ev;
    fmi3_import_variable_typedef_t* t;
    fmi3_import_enumeration_typedef_t* et;

    REQUIRE(v != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(v) == 1);
    REQUIRE(fmi3_import_get_variable_description(v) == nullptr); /* Default description is empty */
    REQUIRE(fmi3_import_get_causality(v) == fmi3_causality_enu_local);
    REQUIRE(fmi3_import_get_variability(v) == fmi3_variability_enu_discrete);
    REQUIRE(fmi3_import_get_initial(v) == fmi3_initial_enu_calculated);

    ev = fmi3_import_get_variable_as_enum(v);
    REQUIRE(ev != nullptr);
    REQUIRE(fmi3_import_get_enum_variable_max(ev) == 3);
    REQUIRE(fmi3_import_get_enum_variable_min(ev) == 1);

    t = fmi3_import_get_variable_declared_type(v);
    REQUIRE(t != nullptr);

    REQUIRE(strcmp(fmi3_import_get_type_name(t), "MyEnum") == 0);
    REQUIRE(strcmp(fmi3_import_get_type_quantity(t), "TypeQuantity") == 0);

    et = fmi3_import_get_type_as_enum(t);
    REQUIRE(et != nullptr);
    REQUIRE(fmi3_import_get_enum_type_max(et) == 3);
    REQUIRE(fmi3_import_get_enum_type_min(et) == 1);
    REQUIRE(fmi3_import_get_enum_type_size(et) == 3);
}

/* Parse enum variable with all information specified */
static void test_enum_all_attrs(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "allAttrsEnumVar");
    fmi3_import_enum_variable_t* ev;
    fmi3_import_variable_typedef_t* t;
    fmi3_import_enumeration_typedef_t* et;

    REQUIRE(v != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(v) == 2);
    REQUIRE(strcmp(fmi3_import_get_variable_description(v), "myDescription") == 0);
    REQUIRE(fmi3_import_get_causality(v) == fmi3_causality_enu_output);
    REQUIRE(fmi3_import_get_variability(v) == fmi3_variability_enu_discrete);
    REQUIRE(fmi3_import_get_initial(v) == fmi3_initial_enu_exact);

    ev = fmi3_import_get_variable_as_enum(v);
    REQUIRE(ev != nullptr);
    REQUIRE(fmi3_import_get_enum_variable_max(ev) == 2);
    REQUIRE(fmi3_import_get_enum_variable_min(ev) == 1);
    REQUIRE(fmi3_import_get_enum_variable_start(ev) == 2);

    t = fmi3_import_get_variable_declared_type(v);
    REQUIRE(t != nullptr);
    REQUIRE(strcmp(fmi3_import_get_type_name(t), "MyEnum") == 0);
    REQUIRE(strcmp(fmi3_import_get_type_quantity(t), "TypeQuantity") == 0);

    et = fmi3_import_get_type_as_enum(t);
    REQUIRE(et != nullptr);
    REQUIRE(fmi3_import_get_enum_type_max(et) == 3);
    REQUIRE(fmi3_import_get_enum_type_min(et) == 1);
    REQUIRE(fmi3_import_get_enum_type_size(et) == 3);
}

/**
 * Tests parsing a Binary variable with default attributes.
 */
static void test_binary_default_attrs(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "defaultBinaryVar");
    fmi3_import_binary_variable_t* bv;
    fmi3_import_variable_typedef_t* t;
    fmi3_import_variable_list_t* clockVars;

    REQUIRE(v != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(v) == 3);
    REQUIRE(fmi3_import_get_variable_description(v) == nullptr);
    REQUIRE(fmi3_import_get_causality(v) == fmi3_causality_enu_local);
    REQUIRE(fmi3_import_get_variability(v) == fmi3_variability_enu_discrete);
    REQUIRE(fmi3_import_get_initial(v) == fmi3_initial_enu_calculated);  // Depends on causality and variability

    clockVars = fmi3_import_get_variable_clocks(xml, v);
    REQUIRE(fmi3_import_get_variable_list_size(clockVars) == 0);
    fmi3_import_free_variable_list(clockVars);

    bv = fmi3_import_get_variable_as_binary(v);
    REQUIRE(bv != nullptr);

    t = fmi3_import_get_variable_declared_type(v);
    REQUIRE(t == nullptr);  // No declared type
}

/**
 * Tests parsing a Binary variable with all attributes set
 */
static void test_binary_all_attrs(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "allAttrsBinaryVar");
    fmi3_import_binary_variable_t* bv;
    fmi3_import_variable_t* preBv;
    fmi3_import_variable_typedef_t* t;
    fmi3_import_variable_list_t* clockVars;
    fmi3_import_variable_t* cv;

    REQUIRE(v != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(v) == 4);
    REQUIRE(fmi3_import_get_causality(v) == fmi3_causality_enu_output);
    REQUIRE(fmi3_import_get_variability(v) == fmi3_variability_enu_discrete);
    REQUIRE(fmi3_import_get_initial(v) == fmi3_initial_enu_exact);
    REQUIRE(strcmp(fmi3_import_get_variable_description(v), "myDesc") == 0);
    REQUIRE(fmi3_import_get_canHandleMultipleSetPerTimeInstant(v) == true); /* default */

    preBv = fmi3_import_get_previous(v);
    REQUIRE(preBv != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(preBv) == 4001);

    clockVars = fmi3_import_get_variable_clocks(xml, v);
    REQUIRE(fmi3_import_get_variable_list_size(clockVars) == 1);
    cv = fmi3_import_get_variable(clockVars, 0);
    REQUIRE(cv != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(cv) == 4002);
    fmi3_import_free_variable_list(clockVars);

    bv = fmi3_import_get_variable_as_binary(v);
    REQUIRE(bv != nullptr);

    t = fmi3_import_get_variable_declared_type(v);
    REQUIRE(t == nullptr);  // No declared type
}

/**
 * Test multiple vrs in the clock attribute.
 */
static void test_clock_attr_multi_value(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "hasMultipleClocks");
    fmi3_import_variable_list_t* clockVars;
    fmi3_import_variable_t* cv;
    size_t nClocks = 3;

    REQUIRE(v != nullptr);
    clockVars = fmi3_import_get_variable_clocks(xml, v);
    REQUIRE(fmi3_import_get_variable_list_size(clockVars) == nClocks);
    for (size_t i = 0; i < nClocks; i++) {
        cv = fmi3_import_get_variable(clockVars, i);
        REQUIRE(cv != nullptr);
        REQUIRE(fmi3_import_get_variable_vr(cv) == 999001+i);
    }
    fmi3_import_free_variable_list(clockVars);
}

TEST_CASE("Variable parsing", "[xml_variables]") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variable_test";

    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);
    
    SECTION("Enum: parse default attributes") {
        test_enum_default_attrs(xml);
    }
    SECTION("Enum: parse all attributes") {
        test_enum_all_attrs(xml);
    }

    SECTION("Binary: parse default attributes") {
        test_binary_default_attrs(xml);
    }
    SECTION("Binary: parse all attributes") {
        test_binary_all_attrs(xml);
    }

    SECTION("ClockAttr: multiple values") {
        test_clock_attr_multi_value(xml);
    }

    fmi3_import_free(xml);
}
