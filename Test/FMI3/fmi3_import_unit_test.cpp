#include <stdio.h>
#include <stdint.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#include "catch.hpp"

// Tests for the Unit & DisplayUnit constructs


/*
TODO; 
Notes:

vbase = factor∗vunit + if relativeQuantity then 0 else offset
"RelativeQuantity" is an attribute of a TypeDef on a variable

vdisplay = {factor∗vunit + offset       if inverse = false 
            factor*1/vunit              if inverse = true

*/

/* Parse unit and display unit info for float32 variable */

// TODO: copy this over to unit & display unit cpp test?
static void test_float64_unit(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "float64WithUnit");
    REQUIRE(v != nullptr);

    fmi3_import_unit_t* u;
    fmi3_import_display_unit_t* du;
    fmi3_float64_t du_val_orig;
    fmi3_float64_t du_val_exp;
    fmi3_float64_t du_val_conv;
    fmi3_float64_t du_val_reconv;

    /* Arbitrarily chosen values that don't get truncated/rounded */
    fmi3_import_float64_variable_t* var = fmi3_import_get_variable_as_float64(v);
    REQUIRE(var != nullptr);

    /* just a few checks on unit and display unit, full check is done in fmi3_import_xml test (TODO: create bouncing ball example with different numeric types, for fmi3_import_xml test) */
    /* unit */
    u = fmi3_import_get_float64_variable_unit(var);
    REQUIRE(u != nullptr);
    REQUIRE(!strcmp("K", fmi3_import_get_unit_name(u)));

    /* display unit */
    du = fmi3_import_get_float64_variable_display_unit(var);
    REQUIRE(du != nullptr);
    REQUIRE(!strcmp("degC", fmi3_import_get_display_unit_name(du)));

    du_val_orig = 222.22;
    du_val_exp = du_val_orig - 273.15;
    // TODO: Fix
    // du_val_conv = fmi3_import_float64_convert_to_display_unit(du_val_orig, du, 0);
    // REQUIRE(du_val_exp == du_val_conv);

    // du_val_reconv = fmi3_import_float64_convert_from_display_unit(du_val_conv, du, 0);
    // REQUIRE(du_val_orig == du_val_reconv);
}

/* Parse unit and display unit info for float32 variable */
static void test_float32_unit(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "float32WithUnit");
    REQUIRE(v != nullptr);

    fmi3_import_unit_t* u;
    fmi3_import_display_unit_t* du;
    fmi3_float32_t du_val_orig;
    fmi3_float32_t du_val_exp;
    fmi3_float32_t du_val_conv;
    fmi3_float32_t du_val_reconv;

    /* Arbitrarily chosen values that don't get truncated/rounded */
    fmi3_import_float32_variable_t* var = fmi3_import_get_variable_as_float32(v);
    REQUIRE(var != nullptr);

    /* just a few checks on unit and display unit, full check is done in fmi3_import_xml test (TODO: create bouncing ball example with different numeric types, for fmi3_import_xml test) */
    /* unit */
    u = fmi3_import_get_float32_variable_unit(var);
    REQUIRE(u != nullptr);
    REQUIRE(!strcmp("K", fmi3_import_get_unit_name(u)));

    du = fmi3_import_get_float32_variable_display_unit(var);
    REQUIRE(du != nullptr); /* display unit */
    REQUIRE(!strcmp("degC", fmi3_import_get_display_unit_name(du)));

    du_val_orig = 222.22f;
    du_val_exp = du_val_orig - 273.15f;
    // TODO: FIX
    // du_val_conv = fmi3_import_float32_convert_to_display_unit(du_val_orig, du, 0);
    // REQUIRE(du_val_exp == du_val_conv);

    // du_val_reconv = fmi3_import_float32_convert_from_display_unit(du_val_conv, du, 0);
    // REQUIRE(du_val_orig == du_val_reconv);
}

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

    SECTION("Float64 old") {
        test_float32_unit(xml);
    }

    SECTION("Float32 old") {
        test_float32_unit(xml);
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
