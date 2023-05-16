#include <stdio.h>
#include <stdint.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"


/* Parse enum variable with minimal specified information. Tests defaults. */
static void test_enum_default_attrs(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "enumDefault");
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
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "enumAllAttr");
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
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "binaryDefault");
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
    REQUIRE(fmi3_import_get_binary_variable_start_size(bv) == 0);
    REQUIRE(fmi3_import_get_binary_variable_start(bv) == nullptr);
    REQUIRE(fmi3_import_get_binary_variable_max_size(bv) == 0);
    REQUIRE(strcmp(fmi3_import_get_binary_variable_mime_type(bv), "application/octet-stream") == 0);

    t = fmi3_import_get_variable_declared_type(v);
    REQUIRE(t == nullptr);  // No declared type
}

/**
 * Tests parsing a Binary variable with all attributes set
 */
static void test_binary_all_attrs(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "binaryAllAttr");
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
    REQUIRE(fmi3_import_get_binary_variable_max_size(bv) == 444);
    REQUIRE(strcmp(fmi3_import_get_binary_variable_mime_type(bv), "myMimeType") == 0);

    t = fmi3_import_get_variable_declared_type(v);
    REQUIRE(t == nullptr);  // No declared type
}

/**
 * Tests parsing an empty mimeType attribute.
 */
static void test_binary_empty_mimeType(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "binaryEmptyMimeType");
    fmi3_import_binary_variable_t* bv;

    REQUIRE(v != nullptr);

    bv = fmi3_import_get_variable_as_binary(v);
    REQUIRE(bv != nullptr);

    // BUG: Expected is to equal the empty string. Probably same for 'quantity'.
    REQUIRE(fmi3_import_get_binary_variable_mime_type(bv) == nullptr);
}

/**
 * Test parsing a Binary with a Start element.
 */
static void test_binary_start_value(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "binaryStart");
    fmi3_import_binary_variable_t* bv;
    fmi3_import_variable_t* preBv;
    fmi3_import_variable_typedef_t* t;
    fmi3_import_variable_list_t* clockVars;
    fmi3_import_variable_t* cv;

    REQUIRE(v != nullptr);
    bv = fmi3_import_get_variable_as_binary(v);
    REQUIRE(bv != nullptr);

    size_t nBytes       = fmi3_import_get_binary_variable_start_size(bv);
    fmi3_binary_t bytes = fmi3_import_get_binary_variable_start(bv);
    REQUIRE(nBytes == 8);
    REQUIRE(bytes[0] == 0x00U);
    REQUIRE(bytes[1] == 0x11U);
    REQUIRE(bytes[2] == 0xBBU);
    REQUIRE(bytes[3] == 0xffU);
    REQUIRE(bytes[4] == 0x02U);
    REQUIRE(bytes[5] == 0x9eU);
    REQUIRE(bytes[6] == 0xE4U);
    REQUIRE(bytes[7] == 0xCdU);
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

/**
 * Tests parsing a Clock variable with default attributes.
 */
static void test_clock_default_attrs(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "clockDefault");
    fmi3_import_clock_variable_t* cv;
    fmi3_import_variable_typedef_t* t;
    fmi3_import_variable_list_t* clockVars;

    REQUIRE(v != nullptr);
    REQUIRE(fmi3_import_get_variable_description(v) == nullptr);
    REQUIRE(fmi3_import_get_causality(v) == fmi3_causality_enu_local);
    REQUIRE(fmi3_import_get_variability(v) == fmi3_variability_enu_discrete);
    REQUIRE(fmi3_import_get_initial(v) == fmi3_initial_enu_calculated);  // Depends on causality and variability

    clockVars = fmi3_import_get_variable_clocks(xml, v);
    REQUIRE(fmi3_import_get_variable_list_size(clockVars) == 0);
    fmi3_import_free_variable_list(clockVars);

    cv = fmi3_import_get_variable_as_clock(v);
    REQUIRE(cv != nullptr);

    // Type specific attributes:
    REQUIRE(fmi3_import_get_clock_variable_can_be_deactivated(cv)   == false);
    REQUIRE(fmi3_import_get_clock_variable_priority(cv)             == 0);
    REQUIRE(fmi3_import_get_clock_variable_interval_variability(cv) == fmi3_interval_variability_constant);
    REQUIRE(fmi3_import_get_clock_variable_interval_decimal(cv)     == 0.0);
    REQUIRE(fmi3_import_get_clock_variable_shift_decimal(cv)        == 0.0);
    REQUIRE(fmi3_import_get_clock_variable_supports_fraction(cv)    == false);
    REQUIRE(fmi3_import_get_clock_variable_resolution(cv)           == 0);
    REQUIRE(fmi3_import_get_clock_variable_interval_counter(cv)     == 0);
    REQUIRE(fmi3_import_get_clock_variable_shift_counter(cv)        == 0);

    // TODO: Test interval_variability not set.

    t = fmi3_import_get_variable_declared_type(v);
    REQUIRE(t == nullptr);  // No declared type
}

/**
 * Tests parsing a Clock variable with all type-specific attributes.
 */
static void test_clock_all_attrs(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "clockAllAttrs");
    fmi3_import_clock_variable_t* cv;

    REQUIRE(v != nullptr);
    cv = fmi3_import_get_variable_as_clock(v);
    REQUIRE(cv != nullptr);

    // Type specific attributes:
    REQUIRE(fmi3_import_get_clock_variable_can_be_deactivated(cv)   == true);
    REQUIRE(fmi3_import_get_clock_variable_priority(cv)             == 1);
    REQUIRE(fmi3_import_get_clock_variable_interval_variability(cv) == fmi3_interval_variability_countdown);
    REQUIRE(fmi3_import_get_clock_variable_interval_decimal(cv)     == 2.0);
    REQUIRE(fmi3_import_get_clock_variable_shift_decimal(cv)        == 3.0);
    REQUIRE(fmi3_import_get_clock_variable_supports_fraction(cv)    == true);
    REQUIRE(fmi3_import_get_clock_variable_resolution(cv)           == 4);
    REQUIRE(fmi3_import_get_clock_variable_interval_counter(cv)     == 5);
    REQUIRE(fmi3_import_get_clock_variable_shift_counter(cv)        == 6);
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
    SECTION("Binary: empty mimeType attribute") {
        test_binary_empty_mimeType(xml);
    }
    SECTION("Binary: parse Start element and value") {
        test_binary_start_value(xml);
    }

    SECTION("Clock: parse default attributes") {
        test_clock_default_attrs(xml);
    }
    SECTION("Clock: parse all type-specific attributes") {
        test_clock_all_attrs(xml);
    }

    SECTION("ClockAttr: multiple values") {
        test_clock_attr_multi_value(xml);
    }

    fmi3_import_free(xml);
}

TEST_CASE("Invalid Clock variable - no intervalVariability attr", "[xml_variables]") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variable_test/invalid/intervalVariability1";

    /* Disabled the test for now because of memory leak
     fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml == nullptr); */
}

TEST_CASE("Invalid Binary variable - non-hexadecimal char first in byte tuple", "[xml_variables]") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variable_test/invalid/binaryStart1";

    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);

    const char* errMsg = fmi3_import_get_last_error(xml);
    REQUIRE(strcmp(errMsg, "String is not hexadecimal: gf") == 0);
    fmi3_import_free(xml);
}

TEST_CASE("Invalid Binary variable - non-hexadecimal char second in byte tuple", "[xml_variables]") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variable_test/invalid/binaryStart2";

    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);

    const char* errMsg = fmi3_import_get_last_error(xml);
    REQUIRE(strcmp(errMsg, "String is not hexadecimal: FG") == 0);
    fmi3_import_free(xml);
}

TEST_CASE("Invalid structuralParameter - requires start attribute") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variable_test/invalid/structuralParameter_no_start";

    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);

    const char* errMsg = fmi3_import_get_last_error(xml);
    REQUIRE(strcmp(errMsg, "Error: variable structVar: start value required for structuralParameter variables") == 0);

    fmi3_import_free(xml);
}

TEST_CASE("Invalid structuralParameter - has dimension") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variable_test/invalid/structuralParameter_with_dimension";

    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);

    const char* errMsg = fmi3_import_get_last_error(xml);
    REQUIRE(strcmp(errMsg, "Error: variable structVar: structuralParameters must not have Dimension elements.") == 0);

    fmi3_import_free(xml);
}
