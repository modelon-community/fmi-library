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
 * This file contains tests specifically to verify correctness of error handling
 */

// TODO: Possibly move these to variable tests or similar, as appropriate

TEST_CASE("Invalid VR, missing name") {
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/invalid/missing_req_attributes";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);

    // Float64
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Float64': failed to parse attribute valueReference='VR'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Float64': required attribute 'name' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Float64': required attribute 'valueReference' not found"));

    // Int64
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Int64': failed to parse attribute valueReference='VR'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Int64': required attribute 'name' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Int64': required attribute 'valueReference' not found"));

    // Boolean
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Boolean': failed to parse attribute valueReference='VR'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Boolean': required attribute 'name' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Boolean': required attribute 'valueReference' not found"));

    // Binary
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Binary': failed to parse attribute valueReference='VR'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Binary': required attribute 'name' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Binary': required attribute 'valueReference' not found"));

    // Clock
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Clock': failed to parse attribute valueReference='VR'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Clock': required attribute 'name' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Clock': required attribute 'valueReference' not found"));

    // Enumeration
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Enumeration': failed to parse attribute valueReference='VR'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Enumeration': required attribute 'name' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Enumeration': required attribute 'valueReference' not found"));

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Fatal failure in parsing ModelVariables."));
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Testing all global error checks") {
    // This is for testing that all global errors checks for ModelVariables are tested
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/invalid/global_error_checks";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);

    // <Float64 name="floaty"/>
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Float64': required attribute 'valueReference' not found"));

    // duplicate VRs from valid ones
    REQUIRE(fmi3_testutil_log_contains(tfmu, "The following variables have the same valueReference: floaty3, floaty4"));

    // duplicate names from valid ones
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Two variables with the same name 'floaty' found. This is not allowed."));

    // failure to resolve derivate reference
    REQUIRE(fmi3_testutil_log_contains(tfmu, "The valueReference in derivative=\"1000\" did not resolve to any variable."));

    // failure to resolve previous reference
    REQUIRE(fmi3_testutil_log_contains(tfmu, "The valueReference in previous=\"1000\" did not resolve to any variable."));

    // Final failure in ModelVariable parsing
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Fatal failure in parsing ModelVariables."));
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Test multiple attribute errors in a single variable") {
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/invalid/multiple_attribute_issues";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Float64': failed to parse attribute causality='casual'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Float64': failed to parse attribute clocks='yes'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Only variables with variability='discrete' may have the attribute 'previous'."));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "A variable must not refer to itself in the attribute 'previous'."));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Only variables with causality='input' can have canHandleMultipleSetPerTimeInstant=false"));

    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 0);
    REQUIRE(var != nullptr);
    REQUIRE(fmi3_import_get_variable_causality(var) == fmi3_causality_enu_local); // default

    fmi3_import_variable_list_t* clockedList = fmi3_import_get_variable_clocks(fmu, var);
    REQUIRE(fmi3_import_get_variable_list_size(clockedList) == 0);
    fmi3_import_free_variable_list(clockedList);

    fmi3_import_variable_t* varPrev = fmi3_import_get_variable_previous(var);
    REQUIRE(varPrev != nullptr);
    REQUIRE(var == varPrev);
    REQUIRE(fmi3_import_get_variable_can_handle_multiple_set_per_time_instant(var) == fmi3_false);

    fmi3_testutil_import_free(tfmu);
}
