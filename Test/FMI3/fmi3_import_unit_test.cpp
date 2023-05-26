#include <stdio.h>
#include <stdint.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#include "catch.hpp"

// Tests for the Unit & DisplayUnit constructs

TEST_CASE("DisplayUnit parsing") {
    const char* xmldir = FMI3_TEST_XML_DIR "/display_unit/valid";

    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);

    // TODO: Do full test of all the import functions, resp copy them from the other one?

    // TODO: Also add a test for inverse = false

    SECTION("Float32") {
        fmi3_import_variable_t* v = fmi3_import_get_variable_by_vr(xml, 1);
        REQUIRE(v != nullptr);

        fmi3_import_float32_variable_t* f32v = fmi3_import_get_variable_as_float32(v);
        REQUIRE(f32v != nullptr);

        fmi3_import_display_unit_t* du = fmi3_import_get_float32_variable_display_unit(f32v);
        REQUIRE(du != nullptr);

        REQUIRE(fmi3_import_get_display_unit_inverse(du) == 1);

        REQUIRE(fmi3_import_float64_convert_to_display_unit(1000, du) == 1);
    }

    SECTION("Float64") {
        fmi3_import_variable_t* v = fmi3_import_get_variable_by_vr(xml, 0);
        REQUIRE(v != nullptr);

        fmi3_import_float64_variable_t* f64v = fmi3_import_get_variable_as_float64(v);
        REQUIRE(f64v != nullptr);

        fmi3_import_display_unit_t* du = fmi3_import_get_float64_variable_display_unit(f64v);
        REQUIRE(du != nullptr);

        REQUIRE(fmi3_import_get_display_unit_inverse(du) == 1);
        REQUIRE(fmi3_import_float64_convert_to_display_unit(1000, du) == 1);
    }

    fmi3_import_free(xml);   
}

TEST_CASE("Invalid DisplayUnit - inserve = true  + non-zero offset") {
    const char* xmldir = FMI3_TEST_XML_DIR "/display_unit/invalid/inverse_offset";

    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);

    const char* errMsg = fmi3_import_get_last_error(xml);
    REQUIRE(strcmp(errMsg, "DisplayUnit attribute 'inverse' = true only allowed for 'offset' = 0") == 0);
    fmi3_import_free(xml);
}

TEST_CASE("Invalid DisplayUnit - zero factor") {
    const char* xmldir = FMI3_TEST_XML_DIR "/display_unit/invalid/zero_factor";

    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);

    const char* errMsg = fmi3_import_get_last_error(xml);
    REQUIRE(strcmp(errMsg, "DisplayUnit attribute 'factor' cannot be equal to zero") == 0);
    fmi3_import_free(xml);
}






