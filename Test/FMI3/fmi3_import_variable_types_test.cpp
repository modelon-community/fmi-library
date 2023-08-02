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
#include <limits.h>
#include <float.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#include "catch.hpp"

// Tests float32/64 variables for correct handling of precision

/* Parse small Float64 variable */
static void test_small_float64(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "var1");
    REQUIRE(v != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(v) == 1);
    REQUIRE(fmi3_import_get_variable_description(v) == nullptr);
    REQUIRE(fmi3_import_get_variable_causality(v) == fmi3_causality_enu_local);
    REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_continuous);

    fmi3_float64_t min_ref =        -DBL_MAX;
    fmi3_float64_t max_ref =        DBL_MAX;
    fmi3_float64_t nominal_ref =    1.000000000000000;
    fmi3_float64_t start_ref =      3.141592653589793;

    fmi3_import_float64_variable_t* var = fmi3_import_get_variable_as_float64(v);
    REQUIRE(var != nullptr);

    REQUIRE(fmi3_import_get_float64_variable_start(var) == start_ref); // user defined
    REQUIRE(fmi3_import_get_float64_variable_min(var) == min_ref); // default
    REQUIRE(fmi3_import_get_float64_variable_max(var) == max_ref); // default
    REQUIRE(fmi3_import_get_float64_variable_nominal(var) == nominal_ref); // default
}

/* Parse small Float32 variable */
static void test_small_float32(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "var2");
    REQUIRE(v != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(v) == 2);
    REQUIRE(fmi3_import_get_variable_description(v) == nullptr);
    REQUIRE(fmi3_import_get_variable_causality(v) == fmi3_causality_enu_local);
    REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_continuous);

    fmi3_float32_t min_ref =        -FLT_MAX;
    fmi3_float32_t max_ref =        FLT_MAX;
    fmi3_float32_t nominal_ref =    1.00000000f;
    fmi3_float32_t start_ref =      3.14159274f;

    fmi3_import_float32_variable_t* var = fmi3_import_get_variable_as_float32(v);
    REQUIRE(var != nullptr);

    REQUIRE(fmi3_import_get_float32_variable_start(var) == start_ref); // user defined
    REQUIRE(fmi3_import_get_float32_variable_min(var) == min_ref); // default
    REQUIRE(fmi3_import_get_float32_variable_max(var) == max_ref); // default
    REQUIRE(fmi3_import_get_float32_variable_nominal(var) == nominal_ref); // default
}

/* Parse user defined: min, max, nominal for float64 variable*/
static void test_full_float64(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "var3");
    REQUIRE(v != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(v) == 3);
    
    // Arbitrarily chosen values that don't get truncated/rounded
    fmi3_float64_t min_ref =       -3500.0000000000000;
    fmi3_float64_t max_ref =        3343492.7982000001;
    fmi3_float64_t nominal_ref =    7.7776999499999997;

    fmi3_import_float64_variable_t* var = fmi3_import_get_variable_as_float64(v);
    REQUIRE(var != nullptr);

    REQUIRE(fmi3_import_get_float64_variable_min(var) == min_ref); // user defined
    REQUIRE(fmi3_import_get_float64_variable_max(var) == max_ref); // user defined
    REQUIRE(fmi3_import_get_float64_variable_nominal(var) == nominal_ref); // user defined
}

/* Parse user defined: min, max, nominal for float32 variable*/
static void test_full_float_32(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "var4");
    REQUIRE(v != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(v) == 4);

    // Arbitrarily chosen values that don't get truncated/rounded
    fmi3_float32_t min_ref =       -3500.00000f;
    fmi3_float32_t max_ref =        3343492.75f;
    fmi3_float32_t nominal_ref =    7.77769995f;

    fmi3_import_float32_variable_t* var = fmi3_import_get_variable_as_float32(v);;
    REQUIRE(var != nullptr);

    REQUIRE(fmi3_import_get_float32_variable_min(var) == min_ref); // user defined
    REQUIRE(fmi3_import_get_float32_variable_max(var) == max_ref); // user defined
    REQUIRE(fmi3_import_get_float32_variable_nominal(var) == nominal_ref); // user defined
}

TEST_CASE("Varibles types testing") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variable_types/float";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    test_small_float64(fmu);
    test_small_float32(fmu);

    test_full_float64(fmu);
    test_full_float_32(fmu);

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 0);
    fmi3_testutil_import_free(tfmu);
}
