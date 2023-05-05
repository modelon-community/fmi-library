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

    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
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

    fmi3_import_free(xml);
}

TEST_CASE("Variable name expansion") {
    const char* xmldir = FMI3_TEST_XML_DIR "/convenience/valid/nameExpansion";

    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);

    // No escape chars:
    require_name_expansion(xml, "xyz", "xyz");
    require_name_expansion(xml, "", "");

    // Escaping the escape char:
    require_name_expansion(xml, "##", "#");
    
    // Some cases that should give warnings (for now only tests that output is not modified):
    require_name_expansion(xml, "#x", "#x");
    require_name_expansion(xml, "#x#", "#x#");
    require_name_expansion(xml, "#", "#");
    require_name_expansion(xml, "#99#", "#99#");  // Referenced variable doesn't exist
    
    // Different types:
    require_name_expansion(xml, "#0#", "vInt32");
    require_name_expansion(xml, "#1#", "vFloat64");
    require_name_expansion(xml, "#2#", "vFloat32");

    // Expanded twice:
    require_name_expansion(xml, "#0# #1#", "vInt32 vFloat64");
    require_name_expansion(xml, "#0##1#", "vInt32vFloat64");

    // Same name:
    require_name_expansion(xml, "#3#", "sameName");
    require_name_expansion(xml, "#4#", "sameName");

    // Special characters:
    require_name_expansion(xml, "$^/\\", "$^/\\");
    require_name_expansion(xml, "#5#", " Special chars: $^/\\ ");

    fmi3_import_free(xml);
}

TEST_CASE("Variable name expansion via logger") {
    const char* xmldir = FMI3_TEST_XML_DIR "/convenience/valid/nameExpansion";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu != nullptr);

    fmi3_log_forwarding(tfmu->fmu, fmi3_status_ok, "Test", "Variable name: #1#");
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Variable name: vFloat64") == true);
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Variable name: vFloat32") == false);

    fmi3_log_forwarding(tfmu->fmu, fmi3_status_ok, "Test", "Variable name: #2#");
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Variable name: vFloat64") == true);
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Variable name: vFloat32") == true);

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Error check: Variables with same VR") {
    const char* xmldir = FMI3_TEST_XML_DIR "/convenience/invalid/sameVR";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu == nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "The following variables have the same value reference: v1, v3"));

    fmi3_testutil_import_free(tfmu);
}