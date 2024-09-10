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

/**
 * Test getting variables by VR from an XML where variables are not sorted in the XML by VR.
 */
static void test_get_variable_by_vr(fmi3_import_t* xml) {
    fmi3_import_variable_t* v;
    fmi3_import_clock_variable_t* cv;
    size_t nVar = 8;

    // For testing one more property of the variables.
    fmi3_base_type_enu_t baseTypes[] = {
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

/**
 * Test that fmi3_import_get_value_reference_list returns all VRs.
 */
static void test_fmi3_import_get_value_reference_list(fmi3_import_t* xml) {
    size_t nVar = 8;
    
    fmi3_import_variable_list_t* varList = fmi3_import_get_variable_list(xml, 0);
    REQUIRE(fmi3_import_get_variable_list_size(varList) == nVar);
    
    const fmi3_value_reference_t vrsExp[] = {0, 1, 2, 3, 4, 5, 6, 7};
    const fmi3_value_reference_t* vrs = fmi3_import_get_value_reference_list(varList);

    // Linear search to find that all VRs are listed. (Order does not matter.)
    for (size_t i = 0; i < nVar; i++) {
        bool found = false;
        for (size_t j = 0; j < nVar; j++) {
            if (vrs[j] == vrsExp[i]) {
                found = true;
                break;
            }
        }
        REQUIRE(found == true);
    }
    
    fmi3_import_free_variable_list(varList);
}

/**
 * Test that fmi3_import_get_variable_list with order arg set to VR is correct.
 */
static void test_fmi3_import_get_variable_list_order_by_vr(fmi3_import_t* xml) {
    size_t nVar = 8;
    fmi3_import_variable_t* v;
    fmi3_value_reference_t vr;

    const fmi3_value_reference_t vrsExp[] = {0, 1, 2, 3, 4, 5, 6, 7};
    fmi3_import_variable_list_t* varList = fmi3_import_get_variable_list(xml, 2);
    REQUIRE(fmi3_import_get_variable_list_size(varList) == nVar);
    
    const fmi3_value_reference_t* vrs = fmi3_import_get_value_reference_list(varList);
    for (size_t i = 0; i < nVar; i++) {
        v = fmi3_import_get_variable(varList, i);
        vr = fmi3_import_get_variable_vr(v);
        REQUIRE(vr == vrsExp[i]);
    }
    
    fmi3_import_free_variable_list(varList);
}

static void require_name_expansion(fmi3_import_t* xml, const char* msgIn, const char* msgExp) {
    #define BUFF_SZ 100
    char buff[BUFF_SZ];

    fmi3_import_expand_variable_references(xml, msgIn, buff, BUFF_SZ);
    REQUIRE(strcmp(msgExp, buff) == 0);
}

TEST_CASE("Variable parsing") {
    const char* xmldir = FMI3_TEST_XML_DIR "/convenience/valid/notSortedByVR";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* xml = tfmu->fmu;
    REQUIRE(xml != nullptr);
    
    SECTION("Get variable by VR") {
        test_get_variable_by_vr(xml);
    }
    SECTION("Test fmi3_import_get_value_reference_list") {
        test_fmi3_import_get_value_reference_list(xml);
    }
    SECTION("Test fmi3_import_get_variable_list sorted by VR") {
        test_fmi3_import_get_variable_list_order_by_vr(xml);
    }

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 0);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Variable name expansion") {
    const char* xmldir = FMI3_TEST_XML_DIR "/convenience/valid/nameExpansion";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* xml = tfmu->fmu;
    REQUIRE(xml != nullptr);

    // No escape chars:
    require_name_expansion(xml, "xyz", "xyz");
    require_name_expansion(xml, "", "");

    // Escaping the escape char:
    require_name_expansion(xml, "##", "#");
    
    // Some cases that should give warnings (for now only tests that output is not modified):
    // +4 problems
    require_name_expansion(xml, "#x", "#x");
    require_name_expansion(xml, "#x#", "#x#");
    require_name_expansion(xml, "#", "#");
    require_name_expansion(xml, "#99#", "#99#");  // Referenced variable doesn't exist
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Expected value reference in log message here: ''"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Could not find variable referenced in log message here: '99'"));
    
    // Different types:
    require_name_expansion(xml, "#0#", "vInt32");
    require_name_expansion(xml, "#1#", "vFloat64");
    require_name_expansion(xml, "#2#", "vFloat32");

    // Expanded twice:
    require_name_expansion(xml, "#0# #1#", "vInt32 vFloat64");
    require_name_expansion(xml, "#0##1#", "vInt32vFloat64");

    // Special characters:
    require_name_expansion(xml, "$^/\\", "$^/\\");
    require_name_expansion(xml, "#5#", " Special chars: $^/\\ ");

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 4);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Variable name expansion via logger") {
    const char* xmldir = FMI3_TEST_XML_DIR "/convenience/valid/nameExpansion";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu != nullptr);

    fmi3_log_forwarding(tfmu->fmu, fmi3_status_ok, "Test", "Variable name: #1#");
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Variable name: vFloat64"));
    REQUIRE(!fmi3_testutil_log_contains(tfmu, "Variable name: vFloat32"));

    fmi3_log_forwarding(tfmu->fmu, fmi3_status_ok, "Test", "Variable name: #2#");
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Variable name: vFloat64"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Variable name: vFloat32"));

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 0);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Test model counts variability") {
    const char* xmldir = FMI3_TEST_XML_DIR "/convenience/valid/modelCountsVariability";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* xml = tfmu->fmu;
    REQUIRE(xml != nullptr);

    fmi3_import_model_counts_t counts;
    fmi3_import_collect_model_counts(xml, &counts);

    REQUIRE(counts.num_constants  == 1);
    REQUIRE(counts.num_fixed      == 2);
    REQUIRE(counts.num_tunable    == 3);
    REQUIRE(counts.num_discrete   == 4);
    REQUIRE(counts.num_continuous == 5);

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 0);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Test model counts causality") {
    const char* xmldir = FMI3_TEST_XML_DIR "/convenience/valid/modelCountsCausality";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* xml = tfmu->fmu;
    REQUIRE(xml != nullptr);

    fmi3_import_model_counts_t counts;
    fmi3_import_collect_model_counts(xml, &counts);

    REQUIRE(counts.num_parameters            == 1);
    REQUIRE(counts.num_calculated_parameters == 2);
    REQUIRE(counts.num_inputs                == 3);
    REQUIRE(counts.num_outputs               == 4);
    REQUIRE(counts.num_local                 == 5);
    REQUIRE(counts.num_independent           == 6);
    REQUIRE(counts.num_structural_parameters == 7);

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 0);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Test model counts type") {
    const char* xmldir = FMI3_TEST_XML_DIR "/convenience/valid/modelCountsType";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* xml = tfmu->fmu;
    REQUIRE(xml != nullptr);

    fmi3_import_model_counts_t counts;
    fmi3_import_collect_model_counts(xml, &counts);

    REQUIRE(counts.num_float64_vars == 1);
    REQUIRE(counts.num_float32_vars == 2);
    REQUIRE(counts.num_int64_vars   == 3);
    REQUIRE(counts.num_int32_vars   == 4);
    REQUIRE(counts.num_int16_vars   == 5);
    REQUIRE(counts.num_int8_vars    == 6);
    REQUIRE(counts.num_uint64_vars  == 7);
    REQUIRE(counts.num_uint32_vars  == 8);
    REQUIRE(counts.num_uint16_vars  == 9);
    REQUIRE(counts.num_uint8_vars   == 10);
    REQUIRE(counts.num_enum_vars    == 11);
    REQUIRE(counts.num_bool_vars    == 12);
    REQUIRE(counts.num_string_vars  == 13);
    REQUIRE(counts.num_binary_vars  == 14);
    REQUIRE(counts.num_clock_vars   == 15);

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 0);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Get variable from alias name") {
    const char* xmldir = FMI3_TEST_XML_DIR "/convenience/valid/get_variable_by_alias_name1";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* xml = tfmu->fmu;
    REQUIRE(xml != nullptr);
    
    fmi3_import_variable_t* baseVar    = fmi3_import_get_variable_by_name(xml, "v1");
    fmi3_import_variable_t* aliasVar   = fmi3_import_get_variable_by_name(xml, "v1_a1");
    fmi3_import_variable_t* missingVar = fmi3_import_get_variable_by_name(xml, "v1_missing");
    REQUIRE(baseVar != nullptr);
    REQUIRE(baseVar == aliasVar);
    REQUIRE(missingVar == nullptr);
    REQUIRE_STREQ(fmi3_import_get_variable_name(baseVar), "v1");

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 0);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Get variable from alias name - mixed alphabetical order") {
    const char* xmldir = FMI3_TEST_XML_DIR "/convenience/valid/get_variable_by_alias_name2";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* xml = tfmu->fmu;
    REQUIRE(xml != nullptr);
    
    fmi3_import_variable_t* baseVar   = fmi3_import_get_variable_by_name(xml, "a");
    fmi3_import_variable_t* otherVar1 = fmi3_import_get_variable_by_name(xml, "b");
    fmi3_import_variable_t* aliasVar  = fmi3_import_get_variable_by_name(xml, "c");
    fmi3_import_variable_t* otherVar2 = fmi3_import_get_variable_by_name(xml, "d");
    REQUIRE(baseVar != nullptr);
    REQUIRE(baseVar == aliasVar);
    REQUIRE(baseVar != otherVar1);
    REQUIRE(otherVar1 != otherVar2);
    REQUIRE_STREQ(fmi3_import_get_variable_name(baseVar),   "a");
    REQUIRE_STREQ(fmi3_import_get_variable_name(otherVar1), "b");
    REQUIRE_STREQ(fmi3_import_get_variable_name(otherVar2), "d");

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 0);
    fmi3_testutil_import_free(tfmu);
}