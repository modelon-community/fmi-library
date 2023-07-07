/*
    Copyright (C) 2023 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#include <stdio.h>
#include <stdint.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#include "catch.hpp"

// Tests for the Unit & DisplayUnit constructs

#define DOUBLE_TOL (1.e-12) // tolerance for various conversions involving floating point arithmetics
#define FLOAT_TOL  (1.e-4) // tolerance for various conversions involving floating point arithmetics

static void test_unit_basics(fmi3_import_t* xml) {
    fmi3_import_unit_definition_list_t* udList;
    udList = fmi3_import_get_unit_definition_list(xml);
    REQUIRE(udList != nullptr);
    REQUIRE(fmi3_import_get_unit_definitions_number(udList) == 1); // there is exactly one unit definition

    // check unit
    fmi3_import_unit_t* u = fmi3_import_get_unit(udList, 0);
    REQUIRE(u != nullptr);
    REQUIRE(fmi3_import_get_unit(udList, 1) == nullptr); // only one unit exists, but should not segfault
    REQUIRE(strcmp(fmi3_import_get_unit_name(u), "K") == 0);

    // Check SI unit exponents
    const int* siExp;
    siExp = fmi3_import_get_SI_unit_exponents(u);
    REQUIRE(siExp[fmi3_SI_base_unit_kg]  == 0);
    REQUIRE(siExp[fmi3_SI_base_unit_m]   == 0);
    REQUIRE(siExp[fmi3_SI_base_unit_s]   == 0);
    REQUIRE(siExp[fmi3_SI_base_unit_A]   == 0);
    REQUIRE(siExp[fmi3_SI_base_unit_K]   == 1);
    REQUIRE(siExp[fmi3_SI_base_unit_mol] == 0);
    REQUIRE(siExp[fmi3_SI_base_unit_cd]  == 0);
    REQUIRE(siExp[fmi3_SI_base_unit_rad] == 0);

    REQUIRE(fmi3_import_get_SI_unit_factor(u) == 1.);
    REQUIRE(fmi3_import_get_SI_unit_offset(u) == 0.);

    // trivial in this example, since factor == 1. & offset = 0.
    REQUIRE(fmi3_import_convert_to_SI_base_unit(1., u) == 1.);
    REQUIRE(fmi3_import_convert_from_SI_base_unit(1., u) == 1.);

    // check displayUnit
    unsigned int duNum = fmi3_import_get_unit_display_unit_number(u);
    REQUIRE(duNum == 4);

    fmi3_import_display_unit_t* du;
    fmi3_import_unit_t* uBase;
    // degF
    du = fmi3_import_get_unit_display_unit(u, 0);
    REQUIRE(du != nullptr);
    uBase = fmi3_import_get_base_unit(du);
    REQUIRE(uBase == u);
    REQUIRE(strcmp(fmi3_import_get_display_unit_name(du), "degF") == 0);
    REQUIRE(fmi3_import_get_display_unit_factor(du) == 1.7999999999999998);
    REQUIRE(fmi3_import_get_display_unit_offset(du) == -459.67);
    REQUIRE(fmi3_import_get_display_unit_inverse(du) == 0);

    // degC
    du = fmi3_import_get_unit_display_unit(u, 1);
    REQUIRE(du != nullptr);
    uBase = fmi3_import_get_base_unit(du);
    REQUIRE(uBase == u);
    REQUIRE(strcmp(fmi3_import_get_display_unit_name(du), "degC") == 0);
    REQUIRE(fmi3_import_get_display_unit_factor(du) == 1.0);
    REQUIRE(fmi3_import_get_display_unit_offset(du) == -273.15);
    REQUIRE(fmi3_import_get_display_unit_inverse(du) == 0);

    // degK_inv
    du = fmi3_import_get_unit_display_unit(u, 2);
    REQUIRE(du != nullptr);
    uBase = fmi3_import_get_base_unit(du);
    REQUIRE(uBase == u);
    REQUIRE(strcmp(fmi3_import_get_display_unit_name(du), "degK_inv") == 0);
    REQUIRE(fmi3_import_get_display_unit_factor(du) == 1.0);
    REQUIRE(fmi3_import_get_display_unit_offset(du) == 0.0);
    REQUIRE(fmi3_import_get_display_unit_inverse(du) == 1);

    // 10degK_inv
    du = fmi3_import_get_unit_display_unit(u, 3);
    REQUIRE(du != nullptr);
    uBase = fmi3_import_get_base_unit(du);
    REQUIRE(uBase == u);
    REQUIRE(strcmp(fmi3_import_get_display_unit_name(du), "10degK_inv") == 0);
    REQUIRE(fmi3_import_get_display_unit_factor(du) == 10.0);
    REQUIRE(fmi3_import_get_display_unit_offset(du) == 0.0);
    REQUIRE(fmi3_import_get_display_unit_inverse(du) == 1);

    // out of bounds; should not segfault
    du = fmi3_import_get_unit_display_unit(u, 4);
    REQUIRE(du == nullptr);
}

/* Parsing and testing of Unit and DisplayUnit for Float64 variable */
static void test_float64(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "temp64_C");
    REQUIRE(v != nullptr);

    fmi3_import_float64_variable_t* var = fmi3_import_get_variable_as_float64(v);
    REQUIRE(var != nullptr);

    // unit
    fmi3_import_unit_t* u = fmi3_import_get_float64_variable_unit(var);
    REQUIRE(u != nullptr);
    REQUIRE(strcmp(fmi3_import_get_unit_name(u), "K") == 0);

    fmi3_float64_t du_val_orig, du_val_exp, du_val_conv, du_val_reconv;

    // associated displayUnit
    fmi3_import_display_unit_t* du = fmi3_import_get_float64_variable_display_unit(var);
    REQUIRE(du != nullptr);
    REQUIRE(strcmp(fmi3_import_get_display_unit_name(du), "degC") == 0);

    // test default displayUnit: degC
    du_val_orig = 222.22; // K, base unit

    du_val_exp = du_val_orig - 273.15; // degC
    du_val_conv = fmi3_import_float64_convert_to_display_unit(du_val_orig, du, 0);
    REQUIRE(du_val_exp == du_val_conv);
    du_val_reconv = fmi3_import_float64_convert_from_display_unit(du_val_conv, du, 0);
    REQUIRE(du_val_orig == du_val_reconv);

    // test as relativeQuantity; value is a temperature difference, offsets cancel out and are ignored
    du_val_orig = 10; // K
    du_val_exp = 10; // degC, temperature differences in K and degC are identical
    du_val_conv = fmi3_import_float64_convert_to_display_unit(du_val_orig, du, 1);
    REQUIRE(du_val_exp == du_val_conv);
    du_val_reconv = fmi3_import_float64_convert_from_display_unit(du_val_conv, du, 1);
    REQUIRE(du_val_orig == du_val_reconv);

    // Test other displayUnits
    // degF
    du = fmi3_import_get_unit_display_unit(u, 0);
    REQUIRE(du != nullptr);
    REQUIRE(strcmp(fmi3_import_get_display_unit_name(du), "degF") == 0);

    du_val_orig = 273.15; // K, base unit
    du_val_exp = 32.0; // degF
    du_val_conv = fmi3_import_float64_convert_to_display_unit(du_val_orig, du, 0);
    // Add some tolerance for conversion due to floatig point errors
    REQUIRE_THAT(du_val_exp - du_val_conv, Catch::Matchers::Floating::WithinAbsMatcher(0., DOUBLE_TOL));
    du_val_reconv = fmi3_import_float64_convert_from_display_unit(du_val_conv, du, 0);
    REQUIRE_THAT(du_val_orig - du_val_reconv, Catch::Matchers::Floating::WithinAbsMatcher(0., DOUBLE_TOL));

    // test as relativeQuantity; value is a temperature difference, offsets cancel out and are ignored
    du_val_orig = 10; // K
    du_val_exp = 10 * (9./5.); // degF, temperature differences in K and degF are only affected by factor
    du_val_conv = fmi3_import_float64_convert_to_display_unit(du_val_orig, du, 1);
    REQUIRE_THAT(du_val_exp - du_val_conv, Catch::Matchers::Floating::WithinAbsMatcher(0., DOUBLE_TOL));
    du_val_reconv = fmi3_import_float64_convert_from_display_unit(du_val_conv, du, 1);
    REQUIRE_THAT(du_val_orig - du_val_reconv, Catch::Matchers::Floating::WithinAbsMatcher(0., DOUBLE_TOL));

    // degK_inv
    du = fmi3_import_get_unit_display_unit(u, 2);
    REQUIRE(du != nullptr);
    REQUIRE(strcmp(fmi3_import_get_display_unit_name(du), "degK_inv") == 0);

    du_val_orig = 273.15; // K, base unit
    du_val_exp = 1./273.15; // degK_inv
    du_val_conv = fmi3_import_float64_convert_to_display_unit(du_val_orig, du, 0);
    // Add some tolerance for conversion due to floatig point errors
    REQUIRE_THAT(du_val_exp - du_val_conv, Catch::Matchers::Floating::WithinAbsMatcher(0., DOUBLE_TOL));
    du_val_reconv = fmi3_import_float64_convert_from_display_unit(du_val_conv, du, 0);
    REQUIRE_THAT(du_val_orig - du_val_reconv, Catch::Matchers::Floating::WithinAbsMatcher(0., DOUBLE_TOL));

    // test as relativeQuantity; value is a temperature difference, offsets cancel out and are ignored
    du_val_orig = 10; // K
    du_val_exp = 1./10.; // degK_inv
    du_val_conv = fmi3_import_float64_convert_to_display_unit(du_val_orig, du, 1);
    REQUIRE_THAT(du_val_exp - du_val_conv, Catch::Matchers::Floating::WithinAbsMatcher(0., DOUBLE_TOL));
    du_val_reconv = fmi3_import_float64_convert_from_display_unit(du_val_conv, du, 1);
    REQUIRE_THAT(du_val_orig - du_val_reconv, Catch::Matchers::Floating::WithinAbsMatcher(0., DOUBLE_TOL));

    // 10degK_inv
    du = fmi3_import_get_unit_display_unit(u, 3);
    REQUIRE(du != nullptr);
    REQUIRE(strcmp(fmi3_import_get_display_unit_name(du), "10degK_inv") == 0);

    du_val_orig = 273.15; // K, base unit
    du_val_exp = 10./273.15; // 10degK_inv
    du_val_conv = fmi3_import_float64_convert_to_display_unit(du_val_orig, du, 0);
    // Add some tolerance for conversion due to floatig point errors
    REQUIRE_THAT(du_val_exp - du_val_conv, Catch::Matchers::Floating::WithinAbsMatcher(0., DOUBLE_TOL));
    du_val_reconv = fmi3_import_float64_convert_from_display_unit(du_val_conv, du, 0);
    REQUIRE_THAT(du_val_orig - du_val_reconv, Catch::Matchers::Floating::WithinAbsMatcher(0., DOUBLE_TOL));

    // test as relativeQuantity; value is a temperature difference, offsets cancel out and are ignored
    du_val_orig = 10; // K
    du_val_exp = 1.; // 10degK_inv
    du_val_conv = fmi3_import_float64_convert_to_display_unit(du_val_orig, du, 1);
    REQUIRE_THAT(du_val_exp - du_val_conv, Catch::Matchers::Floating::WithinAbsMatcher(0., DOUBLE_TOL));
    du_val_reconv = fmi3_import_float64_convert_from_display_unit(du_val_conv, du, 1);
    REQUIRE_THAT(du_val_orig - du_val_reconv, Catch::Matchers::Floating::WithinAbsMatcher(0., DOUBLE_TOL));
}

/* Parsing and testing of Unit and DisplayUnit for Float32 variable */
static void test_float32(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "temp32_C");
    REQUIRE(v != nullptr);

    fmi3_import_float32_variable_t* var = fmi3_import_get_variable_as_float32(v);
    REQUIRE(var != nullptr);

    // unit
    fmi3_import_unit_t* u = fmi3_import_get_float32_variable_unit(var);
    REQUIRE(u != nullptr);
    REQUIRE(strcmp(fmi3_import_get_unit_name(u), "K") == 0);

    fmi3_float32_t du_val_orig, du_val_exp, du_val_conv, du_val_reconv;

    // associated displayUnit
    fmi3_import_display_unit_t* du = fmi3_import_get_float32_variable_display_unit(var);
    REQUIRE(du != nullptr);
    REQUIRE(strcmp(fmi3_import_get_display_unit_name(du), "degC") == 0);

    // test default displayUnit: degC
    du_val_orig = 222.22f; // K, base unit

    du_val_exp = du_val_orig - 273.15f; // degC
    du_val_conv = fmi3_import_float32_convert_to_display_unit(du_val_orig, du, 0);
    REQUIRE(du_val_exp == du_val_conv);
    du_val_reconv = fmi3_import_float32_convert_from_display_unit(du_val_conv, du, 0);
    REQUIRE(du_val_orig == du_val_reconv);

    // test as relativeQuantity; value is a temperature difference, offsets cancel out and are ignored
    du_val_orig = 10.f; // K
    du_val_exp = 10.f; // degC, temperature differences in K and degC are identical
    du_val_conv = fmi3_import_float32_convert_to_display_unit(du_val_orig, du, 1);
    REQUIRE(du_val_exp == du_val_conv);
    du_val_reconv = fmi3_import_float32_convert_from_display_unit(du_val_conv, du, 1);
    REQUIRE(du_val_orig == du_val_reconv);

    // Test other displayUnits
    // degF
    du = fmi3_import_get_unit_display_unit(u, 0);
    REQUIRE(du != nullptr);
    REQUIRE(strcmp(fmi3_import_get_display_unit_name(du), "degF") == 0);

    du_val_orig = 273.15f; // K, base unit
    du_val_exp = 32.0f; // degF
    du_val_conv = fmi3_import_float32_convert_to_display_unit(du_val_orig, du, 0);
    // Add some tolerance for conversion due to floatig point errors
    REQUIRE_THAT(du_val_exp - du_val_conv, Catch::Matchers::Floating::WithinAbsMatcher(0., FLOAT_TOL));
    du_val_reconv = fmi3_import_float32_convert_from_display_unit(du_val_conv, du, 0);
    REQUIRE_THAT(du_val_orig - du_val_reconv, Catch::Matchers::Floating::WithinAbsMatcher(0., FLOAT_TOL));

    // test as relativeQuantity; value is a temperature difference, offsets cancel out and are ignored
    du_val_orig = 10.f; // K
    du_val_exp = 10.f * (9.f/5.f); // degF, temperature differences in K and degF are only affected by factor
    du_val_conv = fmi3_import_float32_convert_to_display_unit(du_val_orig, du, 1);
    REQUIRE_THAT(du_val_exp - du_val_conv, Catch::Matchers::Floating::WithinAbsMatcher(0., FLOAT_TOL));
    du_val_reconv = fmi3_import_float32_convert_from_display_unit(du_val_conv, du, 1);
    REQUIRE_THAT(du_val_orig - du_val_reconv, Catch::Matchers::Floating::WithinAbsMatcher(0., FLOAT_TOL));

    // degK_inv
    du = fmi3_import_get_unit_display_unit(u, 2);
    REQUIRE(du != nullptr);
    REQUIRE(strcmp(fmi3_import_get_display_unit_name(du), "degK_inv") == 0);

    du_val_orig = 273.15f; // K, base unit
    du_val_exp = 1.f/273.15f; // degK_inv
    du_val_conv = fmi3_import_float32_convert_to_display_unit(du_val_orig, du, 0);
    // Add some tolerance for conversion due to floatig point errors
    REQUIRE_THAT(du_val_exp - du_val_conv, Catch::Matchers::Floating::WithinAbsMatcher(0., FLOAT_TOL));
    du_val_reconv = fmi3_import_float32_convert_from_display_unit(du_val_conv, du, 0);
    REQUIRE_THAT(du_val_orig - du_val_reconv, Catch::Matchers::Floating::WithinAbsMatcher(0., FLOAT_TOL));

    // test as relativeQuantity; value is a temperature difference, offsets cancel out and are ignored
    du_val_orig = 10.f; // K
    du_val_exp = 1.f/10.f; // degK_inv
    du_val_conv = fmi3_import_float32_convert_to_display_unit(du_val_orig, du, 1);
    REQUIRE_THAT(du_val_exp - du_val_conv, Catch::Matchers::Floating::WithinAbsMatcher(0., FLOAT_TOL));
    du_val_reconv = fmi3_import_float32_convert_from_display_unit(du_val_conv, du, 1);
    REQUIRE_THAT(du_val_orig - du_val_reconv, Catch::Matchers::Floating::WithinAbsMatcher(0., FLOAT_TOL));

    // 10degK_inv
    du = fmi3_import_get_unit_display_unit(u, 3);
    REQUIRE(du != nullptr);
    REQUIRE(strcmp(fmi3_import_get_display_unit_name(du), "10degK_inv") == 0);

    du_val_orig = 273.15f; // K, base unit
    du_val_exp = 10.f/273.15f; // 10degK_inv
    du_val_conv = fmi3_import_float32_convert_to_display_unit(du_val_orig, du, 0);
    // Add some tolerance for conversion due to floatig point errors
    REQUIRE_THAT(du_val_exp - du_val_conv, Catch::Matchers::Floating::WithinAbsMatcher(0., FLOAT_TOL));
    du_val_reconv = fmi3_import_float32_convert_from_display_unit(du_val_conv, du, 0);
    REQUIRE_THAT(du_val_orig - du_val_reconv, Catch::Matchers::Floating::WithinAbsMatcher(0., FLOAT_TOL));

    // test as relativeQuantity; value is a temperature difference, offsets cancel out and are ignored
    du_val_orig = 10.f; // K
    du_val_exp = 1.f; // 10degK_inv
    du_val_conv = fmi3_import_float32_convert_to_display_unit(du_val_orig, du, 1);
    REQUIRE_THAT(du_val_exp - du_val_conv, Catch::Matchers::Floating::WithinAbsMatcher(0., FLOAT_TOL));
    du_val_reconv = fmi3_import_float32_convert_from_display_unit(du_val_conv, du, 1);
    REQUIRE_THAT(du_val_orig - du_val_reconv, Catch::Matchers::Floating::WithinAbsMatcher(0., FLOAT_TOL));
}

TEST_CASE("Unit and DisplayUnit testing") {
    const char* xmldir = FMI3_TEST_XML_DIR "/unit_display_unit/valid";

    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);

    SECTION("Basic Unit verification") {
        test_unit_basics(xml);
    }

    SECTION("Float64") {
        test_float64(xml);
    }

    SECTION("Float32") {
        test_float32(xml);
    }

    fmi3_import_free(xml);   
}

TEST_CASE("Invalid DisplayUnit - inserve = true  + non-zero offset") {
    const char* xmldir = FMI3_TEST_XML_DIR "/unit_display_unit/invalid/inverse_offset";

    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);

    const char* errMsg = fmi3_import_get_last_error(xml);
    REQUIRE(strcmp(errMsg, "DisplayUnit attribute 'inverse' = true only allowed for 'offset' = 0") == 0);
    fmi3_import_free(xml);
}

TEST_CASE("Invalid DisplayUnit - zero factor") {
    const char* xmldir = FMI3_TEST_XML_DIR "/unit_display_unit/invalid/zero_factor";

    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);

    const char* errMsg = fmi3_import_get_last_error(xml);
    REQUIRE(strcmp(errMsg, "DisplayUnit attribute 'factor' cannot be equal to zero") == 0);
    fmi3_import_free(xml);
}
