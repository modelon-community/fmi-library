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
    fmi3_base_type_enu_t baseTypes[nVar] = {
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

TEST_CASE("Variable parsing") {
    const char* xmldir = FMI3_TEST_XML_DIR "/convenience/valid";

    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);
    
    SECTION("Get variable by VR - non-sorted XML") {
        test_get_variable_by_vr(xml);
    }
    // TODO:
    // fmi3_import_get_value_reference_list
    // fmi3_import_get_variable_list   

    fmi3_import_free(xml);
}

static void require_name_expansion(fmi3_import_t* xml, const char* msgIn, const char* msgExp) {
    #define BUFF_SZ 100
    char buff[BUFF_SZ];

    fmi3_import_expand_variable_references(xml, msgIn, buff, BUFF_SZ);
    REQUIRE(strcmp(msgExp, buff) == 0);
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

    fmi3_log_forwarding(tfmu->fmu, fmi3_status_ok, "Test", "Variable name: #1#");
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Variable name: vFloat64") == true);

    fmi3_testutil_import_free(tfmu);
}