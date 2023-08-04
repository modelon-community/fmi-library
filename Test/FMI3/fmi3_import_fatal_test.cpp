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

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#include "catch.hpp"

/* Variety of tests verifying fatal parsing errors for missing VRs. */

TEST_CASE("Fatal error test, no VR: boolean") {
    const char* xmldir = FMI3_TEST_XML_DIR "/fatal/no_vr/boolean";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);
    
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Boolean': required attribute 'valueReference' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Fatal failure in parsing ModelVariables. Variable(s) failed to parse or an essential error check failed.")); // counts as 2

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 3);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Fatal error test, no VR: enum") {
    const char* xmldir = FMI3_TEST_XML_DIR "/fatal/no_vr/enum";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);
    
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Enumeration': required attribute 'valueReference' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Fatal failure in parsing ModelVariables. Variable(s) failed to parse or an essential error check failed.")); // counts as 2
    // +1; declaredType will not be parsed

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 4);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Fatal error test, no VR: float64") {
    const char* xmldir = FMI3_TEST_XML_DIR "/fatal/no_vr/float64";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);
    
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Float64': required attribute 'valueReference' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Fatal failure in parsing ModelVariables. Variable(s) failed to parse or an essential error check failed.")); // counts as 2

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 3);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Fatal error test, no VR: int64") {
    const char* xmldir = FMI3_TEST_XML_DIR "/fatal/no_vr/int64";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);
    
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Int64': required attribute 'valueReference' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Fatal failure in parsing ModelVariables. Variable(s) failed to parse or an essential error check failed.")); // counts as 2

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 3);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Fatal error test, no VR: string") {
    const char* xmldir = FMI3_TEST_XML_DIR "/fatal/no_vr/string";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);
    
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'String': required attribute 'valueReference' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Fatal failure in parsing ModelVariables. Variable(s) failed to parse or an essential error check failed.")); // counts as 2

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 3);
    fmi3_testutil_import_free(tfmu);
}
