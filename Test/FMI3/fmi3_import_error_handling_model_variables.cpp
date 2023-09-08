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
 * This file contains tests specifically to verify correctness of error handling when parsing ModelVariables.
 * Such as:
 * Elements with multiple issues raise all appropriate errors warnings.
 * Invalid elements are discarded, but valid ones are still parsed
 * 
 * NOTE: No separation into valid/invalid, all tests contain errors/warnings
 */

TEST_CASE("Invalid VR, missing name") {
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/model_variables/missing_req_attributes";
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

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Fatal failure in parsing ModelVariables. Variable(s) failed to parse or an essential error check failed.")); // counts as 2

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 6*3 + 2);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Testing all global error checks") {
    // This is for testing that all global errors checks for ModelVariables are tested
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/model_variables/global_error_checks";
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
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Fatal failure in parsing ModelVariables. Variable(s) failed to parse or an essential error check failed.")); // counts as 2

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 7);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Test multiple attribute errors in a single variable") {
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/model_variables/multiple_attribute_issues";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Float64': failed to parse attribute causality='casual'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Float64': failed to parse attribute clocks='yes'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Only variables with variability 'discrete' may have the attribute 'previous'."));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "A variable must not refer to itself in the attribute 'previous'."));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Only variables with causality 'input' can have canHandleMultipleSetPerTimeInstant=false"));

    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 0);
    REQUIRE(var != nullptr);
    REQUIRE(fmi3_import_get_variable_causality(var) == fmi3_causality_enu_local); // default

    fmi3_import_variable_list_t* clockedList = fmi3_import_get_variable_clocks(fmu, var);
    REQUIRE(fmi3_import_get_variable_list_size(clockedList) == 0);
    fmi3_import_free_variable_list(clockedList);

    fmi3_import_variable_t* varPrev = fmi3_import_get_variable_previous(var);
    REQUIRE(varPrev != nullptr);
    REQUIRE(var == varPrev);
    REQUIRE(fmi3_import_get_variable_can_handle_multiple_set_per_time_instant(var) == false);


    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 5);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Test missing required attribute plus other attribute errors/warnings") {
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/model_variables/multiple_attribute_issues_req";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Float64': required attribute 'name' not found"));

    // We do not check attribute errors if name/valueReference are erroneous; test absence of error message
    REQUIRE(!fmi3_testutil_log_contains(tfmu, "A variable must not refer to itself in the attribute 'previous'."));

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Fatal failure in parsing ModelVariables. Variable(s) failed to parse or an essential error check failed.")); // counts as 2

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 3);
    fmi3_testutil_import_free(tfmu);
}

static void test_dimension(fmi3_import_dimension_t* dim, int hasStart, fmi3_uint64_t start, int hasVR, fmi3_value_reference_t vr) {
    REQUIRE(dim != nullptr);
    REQUIRE(fmi3_import_get_dimension_has_start(dim) == hasStart);
    if (hasStart) {
        REQUIRE(fmi3_import_get_dimension_start(dim) == start);
    }
    REQUIRE(fmi3_import_get_dimension_has_vr(dim) == hasVR);
    if (hasVR) {
        REQUIRE(fmi3_import_get_dimension_vr(dim) == vr);
    }
}

TEST_CASE("Test multiple errors in Dimension parsing") {
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/model_variables/multiple_dimension_errors";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Error parsing Dimension: no attribute 'start' or 'valueReference' found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Error parsing Dimension: mutually exclusive attributes 'start' and 'valueReference' found"));

    fmi3_import_variable_t* var = fmi3_import_get_variable_by_name(fmu, "floaty");
    REQUIRE(var != nullptr);
    fmi3_import_dimension_list_t* dimList = fmi3_import_get_variable_dimension_list(var);
    REQUIRE(dimList != nullptr);
    REQUIRE(fmi3_import_get_dimension_list_size(dimList) == 6);
    fmi3_import_dimension_t* dim;

    INFO("<Dimension start=\"2\"/>");
    test_dimension(fmi3_import_get_dimension(dimList, 0), 1, 2, 0, 0);
    INFO("<Dimension start=\"3\" valueReference=\"1\"/>"); // +1 problem
    test_dimension(fmi3_import_get_dimension(dimList, 1), 1, 3, 1, 1);
    INFO("<Dimension/>"); // +1 problem
    test_dimension(fmi3_import_get_dimension(dimList, 2), 0, 0, 0, 0);
    INFO("<Dimension valueReference=\"x\">"); // +1 problem
    test_dimension(fmi3_import_get_dimension(dimList, 3), 0, 0, 0, 0);
    INFO("<Dimension valueReference=\"1\" start=\"zero\">"); // +2 problems, mutually exclusive + invalid type
    test_dimension(fmi3_import_get_dimension(dimList, 4), 0, 0, 1, 1);
    INFO("<Dimension valueReference=\"1\"/>");
    test_dimension(fmi3_import_get_dimension(dimList, 5), 0, 0, 1, 1);

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 5);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Test multiple errors in Start elements for Binary") {
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/model_variables/binary_multiple_start";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Start missing the required attribute 'value'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Empty value attribute in Start element"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Hexadecimal string is not of even length: 'abc'."));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "String is not hexadecimal: gh"));

    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 1);
    REQUIRE(var != nullptr);
    fmi3_import_binary_variable_t* binVar = fmi3_import_get_variable_as_binary(var);
    REQUIRE(binVar != nullptr);

    REQUIRE(fmi3_import_get_binary_variable_start_size(binVar) == 2); // 2 valid ones

    fmi3_binary_t* bins = fmi3_import_get_binary_variable_start_array(binVar);
    REQUIRE(bins != nullptr);
    size_t* binSizes = fmi3_import_get_binary_variable_start_array_sizes(binVar);
    REQUIRE(binSizes != nullptr);

    INFO("<Start value=\"ffff\"/>");
    REQUIRE(binSizes[0] == 2);
    REQUIRE(bins[0][0] == 255); // ff = 15*16 + 15 = 255
    REQUIRE(bins[0][1] == 255); // ff = 15*16 + 15 = 255

    INFO("<Start value=\"abcdef\"/>");
    REQUIRE(binSizes[1] == 3);
    REQUIRE(bins[1][0] == 171); // ab = 10*16 + 11 = 171
    REQUIRE(bins[1][1] == 205); // cd = 12*16 + 13 = 205
    REQUIRE(bins[1][2] == 239); // ef = 14*16 + 15 = 239

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 4);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Clock with ALL invalid attributes") {
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/model_variables/multiple_attribute_issues_clock_including_req";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    /*
    <Clock name="clock0" valueReference="0" intervalVariability="continuous"
        canBeDeactivated="sometimes" priority="high" intervalDecimal="ten"
        shiftDecimal="twenty" supportsFraction="hopefully" resolution="low"
        intervalCounter="zero" shiftCounter="one"/>
    */
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Clock': failed to parse attribute intervalVariability='continuous'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Clock': could not parse value for boolean attribute 'canBeDeactivated'='sometimes'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Clock': could not parse value for boolean attribute 'supportsFraction'='hopefully'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Clock': failed to parse attribute priority='high'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Clock': failed to parse attribute resolution='low'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Clock': failed to parse attribute intervalCounter='zero'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Clock': failed to parse attribute shiftCounter='one'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Clock': failed to parse attribute intervalDecimal='ten'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Clock': failed to parse attribute shiftDecimal='twenty'"));

    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 0);
    REQUIRE(var != nullptr);
    fmi3_import_clock_variable_t* clockVar = fmi3_import_get_variable_as_clock(var);
    REQUIRE(clockVar != nullptr);

    REQUIRE(fmi3_import_get_clock_variable_can_be_deactivated(clockVar)   == false);
    REQUIRE(fmi3_import_get_clock_variable_has_priority(clockVar)         == false);
    REQUIRE(fmi3_import_get_clock_variable_interval_variability(clockVar) == fmi3_interval_variability_unknown);
    REQUIRE(fmi3_import_get_clock_variable_has_interval_decimal(clockVar) == false);
    REQUIRE(fmi3_import_get_clock_variable_shift_decimal(clockVar)        == 0.0f);
    REQUIRE(fmi3_import_get_clock_variable_supports_fraction(clockVar)    == false);
    REQUIRE(fmi3_import_get_clock_variable_has_resolution(clockVar)       == false);
    REQUIRE(fmi3_import_get_clock_variable_has_interval_counter(clockVar) == false);
    REQUIRE(fmi3_import_get_clock_variable_shift_counter(clockVar)        == 0);

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 9);    
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Clock with ALL OPTIONAL attributes invalid") {
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/model_variables/multiple_attribute_issues_clock";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    // <ClockType name="myClock" intervalVariability="constant" priority="1" intervalCounter="2" resolution="default"/>
    INFO("Verify ClockType");
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'ClockType': failed to parse attribute resolution='default'"));

    fmi3_import_type_definition_list_t* typeDefList = fmi3_import_get_type_definition_list(fmu);
    REQUIRE(typeDefList != nullptr);
    REQUIRE(fmi3_import_get_type_definition_list_size(typeDefList) == 1);

    fmi3_import_variable_typedef_t* typeDefGeneric = fmi3_import_get_typedef(typeDefList, 0);
    REQUIRE(typeDefGeneric != nullptr);
    REQUIRE_STREQ(fmi3_import_get_type_name(typeDefGeneric), "myClock");

    fmi3_import_clock_typedef_t* clockDef = fmi3_import_get_type_as_clock(typeDefGeneric);
    REQUIRE(clockDef != nullptr);
    REQUIRE(fmi3_import_get_clock_type_priority(clockDef) == 1);
    REQUIRE(fmi3_import_get_clock_type_interval_counter(clockDef) == 2);
    REQUIRE(fmi3_import_get_clock_type_has_resolution(clockDef) == false);
    
    /*
    <Clock name="clock1" valueReference="1" intervalVariability="constant"
        canBeDeactivated="no" priority="low" intervalDecimal="twenty"
        shiftDecimal="thirty" supportsFraction="yes" resolution="medium"
        intervalCounter="one" shiftCounter="two"/>
    */

    INFO("Verify Clock");
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Clock': could not parse value for boolean attribute 'canBeDeactivated'='no'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Clock': could not parse value for boolean attribute 'supportsFraction'='yes'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Clock': failed to parse attribute priority='low'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Clock': failed to parse attribute resolution='medium'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Clock': failed to parse attribute intervalCounter='one'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Clock': failed to parse attribute shiftCounter='two'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Clock': failed to parse attribute intervalDecimal='twenty'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Clock': failed to parse attribute shiftDecimal='thirty'"));

    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 1);
    REQUIRE(var != nullptr);
    REQUIRE(fmi3_import_get_variable_declared_type(var) == typeDefGeneric);
    fmi3_import_clock_variable_t* clockVar = fmi3_import_get_variable_as_clock(var);
    REQUIRE(clockVar != nullptr);

    REQUIRE(fmi3_import_get_clock_variable_interval_variability(clockVar) == fmi3_interval_variability_constant);

    // defaults, which are used as fallbacks
    REQUIRE(fmi3_import_get_clock_variable_can_be_deactivated(clockVar) == false);
    REQUIRE(fmi3_import_get_clock_variable_supports_fraction(clockVar)  == false);
    REQUIRE(fmi3_import_get_clock_variable_shift_decimal(clockVar) == 0.0);
    REQUIRE(fmi3_import_get_clock_variable_shift_counter(clockVar) == 0);

    // defaults from typedef
    REQUIRE(fmi3_import_get_clock_variable_has_priority(clockVar) == true);
    REQUIRE(fmi3_import_get_clock_variable_priority(clockVar) == 1);

    REQUIRE(fmi3_import_get_clock_variable_has_interval_counter(clockVar) == true);
    REQUIRE(fmi3_import_get_clock_variable_interval_counter(clockVar) == 2);
    // no defaults
    REQUIRE(fmi3_import_get_clock_variable_has_resolution(clockVar)       == false);
    REQUIRE(fmi3_import_get_clock_variable_has_interval_decimal(clockVar) == false);

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 9);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Clock with muliple errors in required attributes") {
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/model_variables/clock_multiple_req_invalid";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Clock': failed to parse attribute valueReference='zero'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Clock': required attribute 'name' not found"));

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Fatal failure in parsing ModelVariables. Variable(s) failed to parse or an essential error check failed.")); // counts as 2

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 4);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Binary variable with errors in binary specific attributes") {
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/model_variables/binary_specific_attributes";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    // <BinaryType name="myBin" mimeType="mimeType" maxSize="10"/>
    INFO("Verify BinaryType");

    fmi3_import_type_definition_list_t* typeDefList = fmi3_import_get_type_definition_list(fmu);
    REQUIRE(typeDefList != nullptr);
    REQUIRE(fmi3_import_get_type_definition_list_size(typeDefList) == 1);

    fmi3_import_variable_typedef_t* typeDefGeneric = fmi3_import_get_typedef(typeDefList, 0);
    REQUIRE(typeDefGeneric != nullptr);
    REQUIRE_STREQ(fmi3_import_get_type_name(typeDefGeneric), "myBin");

    fmi3_import_binary_typedef_t* binaryDef = fmi3_import_get_type_as_binary(typeDefGeneric);
    REQUIRE(binaryDef != nullptr);
    REQUIRE(fmi3_import_get_binary_type_has_max_size(binaryDef) == true);
    REQUIRE(fmi3_import_get_binary_type_max_size(binaryDef) == 10);
    REQUIRE_STREQ(fmi3_import_get_binary_type_mime_type(binaryDef), "mimeType")

    // <Binary name="binaryVar" valueReference="1" declaredType="myBin" mimeType="mime" maxSize="unlimited"/>
    REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Binary': failed to parse attribute maxSize='unlimited'"));

    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 1);
    REQUIRE(var != nullptr);
    REQUIRE(fmi3_import_get_variable_declared_type(var) == typeDefGeneric);
    fmi3_import_binary_variable_t* binVar = fmi3_import_get_variable_as_binary(var);
    REQUIRE(binVar != nullptr);

    REQUIRE(fmi3_import_get_binary_variable_has_max_size(binVar) == true); // fallback from type
    REQUIRE(fmi3_import_get_binary_variable_max_size(binVar) == 10);

    REQUIRE_STREQ(fmi3_import_get_binary_variable_mime_type(binVar), "mime");

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 1);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("String non-array variable with too many start values") {
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/model_variables/string_multiple_start";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    fmi3_import_variable_t* var;
    fmi3_import_string_variable_t* stringVar;
    fmi3_string_t* startArray;

    // valid one
    var = fmi3_import_get_variable_by_vr(fmu, 0);
    REQUIRE(var != nullptr);
    stringVar = fmi3_import_get_variable_as_string(var);
    REQUIRE(stringVar != nullptr);
    startArray = fmi3_import_get_string_variable_start_array(stringVar);
    REQUIRE_STREQ(startArray[0], "first")

    // Missing Dimension element
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Variable s1: Found 2 Start elements for non-array variable"));

    var = fmi3_import_get_variable_by_vr(fmu, 1);
    REQUIRE(var != nullptr);
    stringVar = fmi3_import_get_variable_as_string(var);
    REQUIRE(stringVar != nullptr);
    startArray = fmi3_import_get_string_variable_start_array(stringVar);
    REQUIRE_STREQ(startArray[0], "first")
    REQUIRE_STREQ(startArray[1], "second")

    // Invalid Dimension element
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Error parsing Dimension: no attribute 'start' or 'valueReference' found"));
    // Dimension still parses & Variable is recognized as array
    REQUIRE(!fmi3_testutil_log_contains(tfmu, "Variable s2: Found 2 Start elements for non-array variable"));

    var = fmi3_import_get_variable_by_vr(fmu, 2);
    REQUIRE(var != nullptr);

    fmi3_import_dimension_list_t* dimList = fmi3_import_get_variable_dimension_list(var);
    REQUIRE(dimList != nullptr);
    REQUIRE(fmi3_import_get_dimension_list_size(dimList) == 1);
    fmi3_import_dimension_t* dim = fmi3_import_get_dimension(dimList, 0);
    REQUIRE(dim != nullptr);
    REQUIRE(fmi3_import_get_dimension_has_start(dim) == false);
    REQUIRE(fmi3_import_get_dimension_has_vr(dim) == false);

    stringVar = fmi3_import_get_variable_as_string(var);
    REQUIRE(stringVar != nullptr);
    startArray = fmi3_import_get_string_variable_start_array(stringVar);
    REQUIRE_STREQ(startArray[0], "first")
    REQUIRE_STREQ(startArray[1], "second")

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 2);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Secondary error test; Derivate/previous reference on invalid variable") {
    // This is a limitation
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/model_variables/invalid_reference_resolve";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Float64': required attribute 'name' not found"));
    // Secondary errors
    REQUIRE(fmi3_testutil_log_contains(tfmu, "The valueReference in derivative=\"0\" did not resolve to any variable."));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "The valueReference in previous=\"0\" did not resolve to any variable."));

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Fatal failure in parsing ModelVariables. Variable(s) failed to parse or an essential error check failed.")); // counts as 2

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 5);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Attribute buffer clearing of non parsed attributes; Model variables") {
    // Test that XML parser clears buffer for ignored attributes when failing to parse an element
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/model_variables/attr_buffer_clearing_variable";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Float64': required attribute 'valueReference' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Int64': required attribute 'valueReference' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Boolean': required attribute 'valueReference' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Binary': required attribute 'valueReference' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Clock': required attribute 'valueReference' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'String': required attribute 'valueReference' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Enumeration': required attribute 'valueReference' not found"));
    // "variability" cleared from buffers after common attribute fails to parse; no error for an invalid variability value
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Fatal failure in parsing ModelVariables. Variable(s) failed to parse or an essential error check failed.")); // count as 2

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 9);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Attribute fuffer clearing of non parsed attributes; Alias") {
    // Test that XML parser clears buffer for ignored attributes when failing to parse an Alias element
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/model_variables/attr_buffer_clearing_alias";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Alias': required attribute 'name' not found")); // twice

    // check that all aliases are correct
    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 0);
    REQUIRE(var != nullptr);
    fmi3_import_alias_variable_list_t* aliasList = fmi3_import_get_variable_alias_list(var);
    REQUIRE(aliasList != nullptr);
    REQUIRE(fmi3_import_get_alias_variable_list_size(aliasList) == 2);
    fmi3_import_alias_variable_t* alias;

    // <Alias name="flo"/>
    alias = fmi3_import_get_alias(aliasList, 0);
    REQUIRE(alias != nullptr);
    REQUIRE_STREQ(fmi3_import_get_alias_variable_name(alias), "flo");

    // <Alias name="double"/>
    alias = fmi3_import_get_alias(aliasList, 1);
    REQUIRE(alias != nullptr);
    REQUIRE_STREQ(fmi3_import_get_alias_variable_name(alias), "double");
    // test that displayUnit from previous variable did not "leak" through
    REQUIRE(fmi3_import_get_alias_variable_display_unit(alias) == nullptr);
    
    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 2);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Invalid variable with aliases") {
    // Test that appropriate error messages are given for nested elements of invalid elements
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/model_variables/invalid_variable_with_alias";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Float64': required attribute 'name' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Skipping nested XML element 'Alias'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Fatal failure in parsing ModelVariables. Variable(s) failed to parse or an essential error check failed.")); // count as 2

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 4);
    fmi3_testutil_import_free(tfmu);
}
