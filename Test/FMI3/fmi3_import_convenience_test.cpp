#include <stdio.h>
#include <stdint.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

/**
 * Test getting variables by VR from an XML where variables are not sorted in the XML by VR.
 */
static void test_get_variable_by_vr(fmi3_import_t* xml) {
    fmi3_import_variable_t* v;
    fmi3_import_clock_variable_t* cv;
    size_t nVar = 8;

    // For testing one more property of the variables.
    fmi3_base_type_enu_t baseTypes[nVar] = {
        fmi3_base_type_int32,
        fmi3_base_type_float64,
        fmi3_base_type_float64,
        fmi3_base_type_int32,
        fmi3_base_type_int32,
        fmi3_base_type_float64,
        fmi3_base_type_int32,
        fmi3_base_type_float64,
    };

    for (size_t i = 0; i < nVar; i++) {
        v = fmi3_import_get_variable_by_vr(xml, i);
        REQUIRE(v != nullptr);
        REQUIRE(fmi3_import_get_variable_vr(v) == i);
        REQUIRE(fmi3_import_get_variable_base_type(v) == baseTypes[i]);
    }
}

TEST_CASE("Variable parsing", "[xml_variables]") {
    const char* xmldir = FMI3_TEST_XML_DIR "/convenience/valid";

    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);
    
    SECTION("Get variable by VR - non-sorted XML") {
        test_get_variable_by_vr(xml);
    }
    // TODO:
    // fmi3_import_get_value_reference_list
    // fmi3_import_get_variable_list   

    fmi3_import_free(xml);
}