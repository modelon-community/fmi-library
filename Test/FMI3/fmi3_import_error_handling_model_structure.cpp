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
#include <float.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#include "catch.hpp"

/** 
 * This file contains tests specifically to verify correctness of error handling when parsing ModelStructure.
 * Such as:
 * Elements with multiple issues raise all appropriate errors warnings.
 * Invalid elements are discarded, but valid ones are still parsed
 * 
 * NOTE: No separation into valid/invalid, all tests contain errors/warnings
 */

TEST_CASE("Test missing VRs") {
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/model_structure/missing_VRs";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Output': required attribute 'valueReference' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'ContinuousStateDerivative': required attribute 'valueReference' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'ClockedState': required attribute 'valueReference' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'InitialUnknown': required attribute 'valueReference' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'EventIndicator': required attribute 'valueReference' not found"));

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Model structure is not valid due to detected errors. Cannot continue."));

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Test invalid VRs") {
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/model_structure/invalid_VRs";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Failed to find variable for valueReference=1."));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Failed to find variable for valueReference=2."));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Failed to find variable for valueReference=3."));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Failed to find variable for valueReference=4."));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Failed to find variable for valueReference=5."));

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Model structure is not valid due to detected errors. Cannot continue."));

    fmi3_testutil_import_free(tfmu);
}
