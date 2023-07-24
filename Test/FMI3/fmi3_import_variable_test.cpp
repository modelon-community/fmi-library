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


/* Parse enum variable with minimal specified information. Tests defaults. */
static void test_enum_default_attrs(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "enumDefault");
    fmi3_import_enum_variable_t* ev;
    fmi3_import_variable_typedef_t* t;
    fmi3_import_enumeration_typedef_t* et;

    REQUIRE(v != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(v) == 1);
    REQUIRE(fmi3_import_get_variable_description(v) == nullptr); /* Default description is empty */
    REQUIRE(fmi3_import_get_variable_causality(v) == fmi3_causality_enu_local);
    REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_discrete);
    REQUIRE(fmi3_import_get_variable_initial(v) == fmi3_initial_enu_calculated);

    ev = fmi3_import_get_variable_as_enum(v);
    REQUIRE(ev != nullptr);
    REQUIRE(fmi3_import_get_enum_variable_max(ev) == 3);
    REQUIRE(fmi3_import_get_enum_variable_min(ev) == 1);

    t = fmi3_import_get_variable_declared_type(v);
    REQUIRE(t != nullptr);

    REQUIRE_STREQ(fmi3_import_get_type_name(t), "MyEnum");
    REQUIRE_STREQ(fmi3_import_get_type_quantity(t), "TypeQuantity");

    et = fmi3_import_get_type_as_enum(t);
    REQUIRE(et != nullptr);
    REQUIRE(fmi3_import_get_enum_type_max(et) == 3);
    REQUIRE(fmi3_import_get_enum_type_min(et) == 1);
    REQUIRE(fmi3_import_get_enum_type_size(et) == 3);
}

/* Parse enum variable with all information specified */
static void test_enum_all_attrs(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "enumAllAttr");
    fmi3_import_enum_variable_t* ev;
    fmi3_import_variable_typedef_t* t;
    fmi3_import_enumeration_typedef_t* et;

    REQUIRE(v != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(v) == 2);
    REQUIRE_STREQ(fmi3_import_get_variable_description(v), "myDescription");
    REQUIRE(fmi3_import_get_variable_causality(v) == fmi3_causality_enu_output);
    REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_discrete);
    REQUIRE(fmi3_import_get_variable_initial(v) == fmi3_initial_enu_exact);

    ev = fmi3_import_get_variable_as_enum(v);
    REQUIRE(ev != nullptr);
    REQUIRE(fmi3_import_get_enum_variable_max(ev) == 2);
    REQUIRE(fmi3_import_get_enum_variable_min(ev) == 1);
    REQUIRE(fmi3_import_get_enum_variable_start(ev) == 2);

    t = fmi3_import_get_variable_declared_type(v);
    REQUIRE(t != nullptr);
    REQUIRE_STREQ(fmi3_import_get_type_name(t), "MyEnum");
    REQUIRE_STREQ(fmi3_import_get_type_quantity(t), "TypeQuantity");

    et = fmi3_import_get_type_as_enum(t);
    REQUIRE(et != nullptr);
    REQUIRE(fmi3_import_get_enum_type_max(et) == 3);
    REQUIRE(fmi3_import_get_enum_type_min(et) == 1);
    REQUIRE(fmi3_import_get_enum_type_size(et) == 3);
}

/**
 * Tests parsing a Binary variable with default attributes.
 */
static void test_binary_default_attrs(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "binaryDefault");
    fmi3_import_binary_variable_t* bv;
    fmi3_import_variable_typedef_t* t;
    fmi3_import_variable_list_t* clockVars;

    REQUIRE(v != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(v) == 3);
    REQUIRE(fmi3_import_get_variable_description(v) == nullptr);
    REQUIRE(fmi3_import_get_variable_causality(v) == fmi3_causality_enu_local);
    REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_discrete);
    REQUIRE(fmi3_import_get_variable_initial(v) == fmi3_initial_enu_calculated);  // Depends on causality and variability
    REQUIRE(fmi3_import_get_variable_can_handle_multiple_set_per_time_instant(v) == true); // default
    REQUIRE(fmi3_import_get_variable_intermediate_update(v) == false); // default

    clockVars = fmi3_import_get_variable_clocks(xml, v);
    REQUIRE(fmi3_import_get_variable_list_size(clockVars) == 0);
    fmi3_import_free_variable_list(clockVars);

    bv = fmi3_import_get_variable_as_binary(v);
    REQUIRE(bv != nullptr);
    REQUIRE(fmi3_import_get_binary_variable_start_size(bv) == 0);
    REQUIRE(fmi3_import_get_binary_variable_start(bv) == nullptr);
    REQUIRE(fmi3_import_get_binary_variable_max_size(bv) == 0);
    REQUIRE_STREQ(fmi3_import_get_binary_variable_mime_type(bv), "application/octet-stream");

    t = fmi3_import_get_variable_declared_type(v);
    REQUIRE(t == nullptr);  // No declared type
}

/**
 * Tests parsing a Binary variable with all attributes set
 */
static void test_binary_all_attrs(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "binaryAllAttr");
    fmi3_import_binary_variable_t* bv;
    fmi3_import_variable_t* preBv;
    fmi3_import_variable_typedef_t* t;
    fmi3_import_variable_list_t* clockVars;
    fmi3_import_variable_t* cv;

    REQUIRE(v != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(v) == 4);
    REQUIRE(fmi3_import_get_variable_causality(v) == fmi3_causality_enu_output);
    REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_discrete);
    REQUIRE(fmi3_import_get_variable_initial(v) == fmi3_initial_enu_calculated);
    REQUIRE_STREQ(fmi3_import_get_variable_description(v), "myDesc");
    REQUIRE(fmi3_import_get_variable_can_handle_multiple_set_per_time_instant(v) == true); // default
    REQUIRE(fmi3_import_get_variable_intermediate_update(v) == true);

    preBv = fmi3_import_get_variable_previous(v);
    REQUIRE(preBv != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(preBv) == 4001);

    clockVars = fmi3_import_get_variable_clocks(xml, v);
    REQUIRE(fmi3_import_get_variable_list_size(clockVars) == 1);
    cv = fmi3_import_get_variable(clockVars, 0);
    REQUIRE(cv != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(cv) == 4002);
    fmi3_import_free_variable_list(clockVars);

    bv = fmi3_import_get_variable_as_binary(v);
    REQUIRE(bv != nullptr);
    REQUIRE(fmi3_import_get_binary_variable_max_size(bv) == 444);
    REQUIRE_STREQ(fmi3_import_get_binary_variable_mime_type(bv), "myMimeType");

    t = fmi3_import_get_variable_declared_type(v);
    REQUIRE(t == nullptr);  // No declared type
}

/**
 * Tests parsing an empty mimeType attribute.
 */
static void test_binary_empty_mimeType(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "binaryEmptyMimeType");
    fmi3_import_binary_variable_t* bv;

    REQUIRE(v != nullptr);

    bv = fmi3_import_get_variable_as_binary(v);
    REQUIRE(bv != nullptr);

    // BUG: Expected is to equal the empty string. Probably same for 'quantity'.
    REQUIRE(fmi3_import_get_binary_variable_mime_type(bv) == nullptr);
}

/**
 * Test parsing a Binary with a Start element.
 */
static void test_binary_start_value(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "binaryStart");
    fmi3_import_binary_variable_t* bv;
    fmi3_import_variable_t* preBv;
    fmi3_import_variable_typedef_t* t;
    fmi3_import_variable_list_t* clockVars;
    fmi3_import_variable_t* cv;

    REQUIRE(v != nullptr);
    bv = fmi3_import_get_variable_as_binary(v);
    REQUIRE(bv != nullptr);

    size_t nBytes       = fmi3_import_get_binary_variable_start_size(bv);
    fmi3_binary_t bytes = fmi3_import_get_binary_variable_start(bv);
    REQUIRE(nBytes == 8);
    REQUIRE(bytes[0] == 0x00U);
    REQUIRE(bytes[1] == 0x11U);
    REQUIRE(bytes[2] == 0xBBU);
    REQUIRE(bytes[3] == 0xffU);
    REQUIRE(bytes[4] == 0x02U);
    REQUIRE(bytes[5] == 0x9eU);
    REQUIRE(bytes[6] == 0xE4U);
    REQUIRE(bytes[7] == 0xCdU);
}

/**
 * Test multiple vrs in the clock attribute.
 */
static void test_clock_attr_multi_value(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "hasMultipleClocks");
    fmi3_import_variable_list_t* clockVars;
    fmi3_import_variable_t* cv;
    size_t nClocks = 3;

    REQUIRE(v != nullptr);
    clockVars = fmi3_import_get_variable_clocks(xml, v);
    REQUIRE(fmi3_import_get_variable_list_size(clockVars) == nClocks);
    for (size_t i = 0; i < nClocks; i++) {
        cv = fmi3_import_get_variable(clockVars, i);
        REQUIRE(cv != nullptr);
        REQUIRE(fmi3_import_get_variable_vr(cv) == 999001+i);
    }
    fmi3_import_free_variable_list(clockVars);
}

/**
 * Tests parsing a Clock variable with default attributes.
 */
static void test_clock_default_attrs(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "clockDefault");
    fmi3_import_clock_variable_t* cv;
    fmi3_import_variable_typedef_t* t;
    fmi3_import_variable_list_t* clockVars;

    REQUIRE(v != nullptr);
    REQUIRE(fmi3_import_get_variable_description(v) == nullptr);
    REQUIRE(fmi3_import_get_variable_causality(v) == fmi3_causality_enu_local);
    REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_discrete);
    REQUIRE(fmi3_import_get_variable_initial(v) == fmi3_initial_enu_calculated);  // Depends on causality and variability

    clockVars = fmi3_import_get_variable_clocks(xml, v);
    REQUIRE(fmi3_import_get_variable_list_size(clockVars) == 0);
    fmi3_import_free_variable_list(clockVars);

    cv = fmi3_import_get_variable_as_clock(v);
    REQUIRE(cv != nullptr);

    // Type specific attributes:
    REQUIRE(fmi3_import_get_clock_variable_can_be_deactivated(cv)   == false);
    REQUIRE(fmi3_import_get_clock_variable_has_priority(cv)         == false);
    REQUIRE(fmi3_import_get_clock_variable_interval_variability(cv) == fmi3_interval_variability_constant);
    REQUIRE(fmi3_import_get_clock_variable_has_interval_decimal(cv) == false);
    REQUIRE(fmi3_import_get_clock_variable_shift_decimal(cv)        == 0.0);
    REQUIRE(fmi3_import_get_clock_variable_supports_fraction(cv)    == false);
    REQUIRE(fmi3_import_get_clock_variable_has_resolution(cv)       == false);
    REQUIRE(fmi3_import_get_clock_variable_has_interval_counter(cv) == false);
    REQUIRE(fmi3_import_get_clock_variable_shift_counter(cv)        == 0);

    t = fmi3_import_get_variable_declared_type(v);
    REQUIRE(t == nullptr);  // No declared type
}

/**
 * Tests parsing a Clock variable with all type-specific attributes.
 */
static void test_clock_all_attrs(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "clockAllAttrs");
    fmi3_import_clock_variable_t* cv;

    REQUIRE(v != nullptr);
    cv = fmi3_import_get_variable_as_clock(v);
    REQUIRE(cv != nullptr);

    // Type specific attributes:
    REQUIRE(fmi3_import_get_clock_variable_can_be_deactivated(cv)   == true);
    REQUIRE(fmi3_import_get_clock_variable_has_priority(cv)         == true);
    REQUIRE(fmi3_import_get_clock_variable_priority(cv)             == 1);
    REQUIRE(fmi3_import_get_clock_variable_interval_variability(cv) == fmi3_interval_variability_countdown);
    REQUIRE(fmi3_import_get_clock_variable_has_interval_decimal(cv) == true);
    REQUIRE(fmi3_import_get_clock_variable_interval_decimal(cv)     == 2.0);
    REQUIRE(fmi3_import_get_clock_variable_shift_decimal(cv)        == 3.0);
    REQUIRE(fmi3_import_get_clock_variable_supports_fraction(cv)    == true);
    REQUIRE(fmi3_import_get_clock_variable_has_resolution(cv)       == true);
    REQUIRE(fmi3_import_get_clock_variable_resolution(cv)           == 4);
    REQUIRE(fmi3_import_get_clock_variable_has_interval_counter(cv) == true);
    REQUIRE(fmi3_import_get_clock_variable_interval_counter(cv)     == 5);
    REQUIRE(fmi3_import_get_clock_variable_shift_counter(cv)        == 6);
}

/**
 * Tests parsing a variable with non-default canHandleMultipleSetPerTimeInstant attribute.
 */
static void test_non_default_canhandlemultiplesetpertimeinstant(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "cannotHandle");
    REQUIRE(v != nullptr);

    REQUIRE(fmi3_import_get_variable_can_handle_multiple_set_per_time_instant(v) == 0);
}

TEST_CASE("Variable parsing") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variables/valid/basic1";

    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);

    SECTION("Enum: parse default attributes") {
        test_enum_default_attrs(xml);
    }
    SECTION("Enum: parse all attributes") {
        test_enum_all_attrs(xml);
    }

    SECTION("Binary: parse default attributes") {
        test_binary_default_attrs(xml);
    }
    SECTION("Binary: parse all attributes") {
        test_binary_all_attrs(xml);
    }
    SECTION("Binary: empty mimeType attribute") {
        test_binary_empty_mimeType(xml);
    }
    SECTION("Binary: parse Start element and value") {
        test_binary_start_value(xml);
    }

    SECTION("Clock: parse default attributes") {
        test_clock_default_attrs(xml);
    }
    SECTION("Clock: parse all type-specific attributes") {
        test_clock_all_attrs(xml);
    }

    SECTION("ClockAttr: multiple values") {
        test_clock_attr_multi_value(xml);
    }

    SECTION("canHandleMultipleSetPerTimeInstant attribute: non-default value") {
        test_non_default_canhandlemultiplesetpertimeinstant(xml);
    }

    fmi3_import_free(xml);
}

TEST_CASE("Invalid Clock variable - no intervalVariability attr") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variables/invalid/intervalVariability1";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Clock': required attribute 'intervalVariability' not found"));

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Invalid Binary variable - non-hexadecimal char first in byte tuple") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variables/invalid/binaryStart1";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);
    
    REQUIRE(fmi3_testutil_log_contains(tfmu, "String is not hexadecimal: gf"));

    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 1);
    REQUIRE(var != nullptr);
    fmi3_import_binary_variable_t* binVar = fmi3_import_get_variable_as_binary(var);
    REQUIRE(binVar != nullptr);
    REQUIRE(fmi3_import_get_binary_variable_start_size(binVar) == 0);

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Invalid Binary variable - non-hexadecimal char second in byte tuple") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variables/invalid/binaryStart2";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);
    
    REQUIRE(fmi3_testutil_log_contains(tfmu, "String is not hexadecimal: FG"));

    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 1);
    REQUIRE(var != nullptr);
    fmi3_import_binary_variable_t* binVar = fmi3_import_get_variable_as_binary(var);
    REQUIRE(binVar != nullptr);
    REQUIRE(fmi3_import_get_binary_variable_start_size(binVar) == 0);

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Invalid Binary variable - hex string of odd length") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variables/invalid/binaryStart3";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Hexadecimal string is not of even length: 'abc'. Truncating to even length."));

    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 1);
    REQUIRE(var != nullptr);
    fmi3_import_binary_variable_t* binVar = fmi3_import_get_variable_as_binary(var);
    REQUIRE(binVar != nullptr);
    REQUIRE(fmi3_import_get_binary_variable_start_size(binVar) == 1);
    fmi3_binary_t bin = fmi3_import_get_binary_variable_start(binVar);
    REQUIRE(*bin == 171); // "ab" = 10*16 + 11 = 171

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Invalid structuralParameter - requires start attribute") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variables/invalid/structuralParameter_no_start";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);
    
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Variable 'structVar': start value required for structuralParameter variables"));
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Invalid structuralParameter - has dimension") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variables/invalid/structuralParameter_with_dimension";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);
    
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Variable structVar: structuralParameters must not have Dimension elements."));

    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 0);
    REQUIRE(var != nullptr);
    REQUIRE(fmi3_import_get_variable_causality(var) == fmi3_causality_enu_structural_parameter);
    REQUIRE(fmi3_import_get_variable_has_start(var) != 0);
    // Dimension will be parsed and accepted anyways

    fmi3_import_dimension_list_t* dimList = fmi3_import_get_variable_dimension_list(var);
    REQUIRE(dimList != nullptr);
    REQUIRE(fmi3_import_get_dimension_list_size(dimList) == 1);

    // <Dimension start="1"/>
    fmi3_import_dimension_t* dim = fmi3_import_get_dimension(dimList, 0);
    REQUIRE(dim != nullptr);
    REQUIRE(fmi3_import_get_dimension_has_start(dim) == 1);
    REQUIRE(fmi3_import_get_dimension_start(dim) == 1);
    REQUIRE(fmi3_import_get_dimension_has_vr(dim) == 0);

    fmi3_testutil_import_free(tfmu);
}

static fmi3_import_alias_variable_list_t* get_aliases(fmi3_import_t* fmu, const char* baseVarName,
        size_t nAliasExp)
{
    fmi3_import_variable_t* v;
    fmi3_import_alias_variable_list_t* aliases;
    size_t nAlias;

    v = fmi3_import_get_variable_by_name(fmu, baseVarName);
    REQUIRE(v != nullptr);
    aliases = fmi3_import_get_variable_alias_list(v);
    REQUIRE(aliases != nullptr);
    nAlias  = fmi3_import_get_alias_variable_list_size(aliases);
    REQUIRE(nAlias == nAliasExp);

    return aliases;
}

static void check_aliases(fmi3_import_t* fmu, const char* baseVarName, size_t nAliasExp,
        const char** namesExp, const char** descExp, fmi3_import_display_unit_t** duExp)
{
    const char* desc;
    const char* name;
    fmi3_xml_display_unit_t* displayUnit;

    fmi3_import_alias_variable_list_t* aliases = get_aliases(fmu, baseVarName, nAliasExp);
    fmi3_import_alias_variable_t* alias;
    
    for (size_t i = 0; i < nAliasExp; i++) {
        alias = fmi3_import_get_alias(aliases, i);
        name = fmi3_import_get_alias_variable_name(alias);
        desc = fmi3_import_get_alias_variable_description(alias);
        displayUnit = fmi3_import_get_alias_variable_display_unit(alias);
        if (namesExp[i] == nullptr) {
            REQUIRE(name == nullptr);
        } else {
            REQUIRE_STREQ(name, namesExp[i]);
        }
        if (descExp[i] == nullptr) {
            REQUIRE(desc == nullptr);
        } else {
            REQUIRE_STREQ(desc, descExp[i]);
        }
        REQUIRE(displayUnit == duExp[i]);
    }
}

TEST_CASE("Alias variables") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variables/valid/alias1";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);
    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 0);
    
    // Get display unit that we expect some tests will reference:
    fmi3_import_unit_definition_list_t* uds = fmi3_import_get_unit_definition_list(fmu);
    fmi3_import_unit_t* degK = fmi3_import_get_unit(uds, 0);
    fmi3_import_display_unit_t* degC = fmi3_import_get_unit_display_unit(degK, 0);
    REQUIRE_STREQ(fmi3_import_get_display_unit_name(degC), "degC");

    fmi3_import_alias_variable_list_t* aliases;
    size_t nAlias;
    
    SECTION("Without alias") {
        fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(fmu, "v1");
        REQUIRE(v != nullptr);
        aliases = fmi3_import_get_variable_alias_list(v);
        nAlias  = fmi3_import_get_alias_variable_list_size(aliases);
        REQUIRE(nAlias == 0);
        REQUIRE(aliases == nullptr);
    }
    
    SECTION("Minimal alias") {
        const char* namesExp[]              = { "v2_a1" };
        const char* descExp[]               = { nullptr };
        fmi3_import_display_unit_t* duExp[] = { nullptr };
        check_aliases(fmu, "v2", 1, namesExp, descExp, duExp);
    }
    
    SECTION("Alias with optional attributes") {
        const char* namesExp[]              = { "v3_a1"      };
        const char* descExp[]               = { "v3_a1_desc" };
        fmi3_import_display_unit_t* duExp[] = { degC         };
        check_aliases(fmu, "v3", 1, namesExp, descExp, duExp);
    }
    
    SECTION("Multiple aliases") {
        const char* namesExp[]              = { "v4_a1", "v4_a2", "v4_a3" };
        const char* descExp[]               = { nullptr, nullptr, nullptr };
        fmi3_import_display_unit_t* duExp[] = { nullptr, nullptr, nullptr };
        check_aliases(fmu, "v4", 3, namesExp, descExp, duExp);
    }
    
    SECTION("Multiple aliases with different optional attributes") {
        const char* namesExp[]              = { "v5_a1", "v5_a2"      };
        const char* descExp[]               = { nullptr, "v5_a2_desc" };
        fmi3_import_display_unit_t* duExp[] = { degC,    nullptr      };
        check_aliases(fmu, "v5", 2, namesExp, descExp, duExp);
    }
    
    SECTION("Alias on non-Float64") {
        const char* namesExp[]              = { "v6_a1" };
        const char* descExp[]               = { nullptr };
        fmi3_import_display_unit_t* duExp[] = { nullptr };
        check_aliases(fmu, "v6", 1, namesExp, descExp, duExp);
    }
    
    SECTION("Aliases with other sibling elements") {
        const char* namesExp[]              = { "v7_a1", "v7_a2" };
        const char* descExp[]               = { nullptr, nullptr };
        fmi3_import_display_unit_t* duExp[] = { nullptr, nullptr };
        check_aliases(fmu, "v7", 2, namesExp, descExp, duExp);
    }
    
    SECTION("Aliases with same description") {
        const char* namesExp[]              = { "v8_a1", "v8_a2" };
        const char* descExp[]               = { "desc",  "desc" };
        fmi3_import_display_unit_t* duExp[] = { nullptr, nullptr };
        check_aliases(fmu, "v8", 2, namesExp, descExp, duExp);
    }
    
    SECTION("Alias with empty description") {
        const char* namesExp[]              = { "v9_a1" };
        const char* descExp[]               = { ""      };
        fmi3_import_display_unit_t* duExp[] = { nullptr };
        check_aliases(fmu, "v9", 1, namesExp, descExp, duExp);
    }
    
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Invalid Alias - unresolvable displayUnit") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variables/invalid/alias_unresolvable_displayUnit";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);
    
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Unknown displayUnit: missing_du"));

    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 1);
    REQUIRE(var != nullptr);
    REQUIRE_STREQ(fmi3_import_get_variable_name(var), "v1");
    
    // aliasList is non empty
    fmi3_import_alias_variable_list_t* aliasList = fmi3_import_get_variable_alias_list(var);
    REQUIRE(fmi3_import_get_alias_variable_list_size(aliasList) == 1);
    // alias exists, but does not have a display unit
    fmi3_import_alias_variable_t* alias = fmi3_import_get_alias(aliasList, 0);
    REQUIRE(alias != nullptr);
    REQUIRE(fmi3_import_get_alias_variable_display_unit(alias) == nullptr);

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Invalid Alias - no name") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variables/invalid/alias_no_name";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);
    
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Alias': required attribute 'name' not found"));

    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 1);
    REQUIRE(var != nullptr);
    REQUIRE_STREQ(fmi3_import_get_variable_name(var), "v1");
    REQUIRE(fmi3_import_get_variable_alias_list(var) == nullptr); // has no valid aliases
    
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Invalid valueReference - two vars with same VR - basic") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variables/invalid/same_vr1";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);
    
    REQUIRE(fmi3_testutil_log_contains(tfmu, "The following variables have the same valueReference: v1, v2"));

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Fatal failure in parsing ModelVariables."));
    
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Invalid valueReference - two vars with same VR - more vars") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variables/invalid/same_vr2";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);
    
    REQUIRE(fmi3_testutil_log_contains(tfmu, "The following variables have the same valueReference: v3, v8"));

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Fatal failure in parsing ModelVariables."));
    
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Invalid valueReference - three vars with same VR") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variables/invalid/same_vr3";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);
    
    REQUIRE(fmi3_testutil_log_contains(tfmu, "The following variables have the same valueReference: v3, v4"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "The following variables have the same valueReference: v4, v6"));

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Fatal failure in parsing ModelVariables."));
    
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Invalid valueReference - same VR but different type") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variables/invalid/same_vr4";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);
    
    REQUIRE(fmi3_testutil_log_contains(tfmu, "The following variables have the same valueReference: v1, v2"));

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Fatal failure in parsing ModelVariables."));
    
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Variable with name being the empty string") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variables/valid/variable_name_empty_str";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);
    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 0);

    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 1);
    REQUIRE(var != nullptr);
    REQUIRE_STREQ(fmi3_import_get_variable_name(var), "");

    var = fmi3_import_get_variable_by_name(fmu, "");
    REQUIRE(var != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(var) == 1);;

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Alias with name being the empty string") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variables/valid/alias_name_empty_str";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);
    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 0);

    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 1);
    REQUIRE(var != nullptr);
    fmi3_import_alias_variable_list_t* aliasList = fmi3_import_get_variable_alias_list(var);
    REQUIRE(aliasList != nullptr);
    REQUIRE(fmi3_import_get_alias_variable_list_size(aliasList) == 1);
    fmi3_import_alias_variable_t* alias = fmi3_import_get_alias(aliasList, 0);
    REQUIRE(alias != nullptr);
    REQUIRE_STREQ(fmi3_import_get_alias_variable_name(alias), "");

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Invalid intermediateUpdate - has causality parameter") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variables/invalid/intermediateUpdate_parameter";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    const char* logMsg = "Variables with causality='parameter' must not be marked with intermediateUpdate='true'.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, logMsg));

    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 0);
    REQUIRE(var != nullptr);
    REQUIRE(fmi3_import_get_variable_causality(var) == fmi3_causality_enu_parameter);
    REQUIRE(fmi3_import_get_variable_intermediate_update(var) != 0);

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Invalid previous - requires clocks") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variable_test/invalid/previous_no_clocks";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    const char* logMsg = "Only variables with the attribute 'clocks' may have the attribute 'previous'.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, logMsg));

    // Test that API works regardless
    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 11);
    REQUIRE(var != nullptr);
    fmi3_import_variable_t* var_prev = fmi3_import_get_variable_previous(var);
    REQUIRE(var_prev != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(var_prev) == 10);

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Invalid previous - requires variability='discrete'") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variable_test/invalid/previous_not_discrete";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    const char* logMsg = "Only variables with variability='discrete' may have the attribute 'previous'.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, logMsg));

    // Test that API works regardless
    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 11);
    REQUIRE(var != nullptr);
    fmi3_import_variable_t* var_prev = fmi3_import_get_variable_previous(var);
    REQUIRE(var_prev != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(var_prev) == 10);

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Invalid Clock variable - has previous") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variable_test/invalid/clock_with_previous";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    const char* logMsg = "Variables of type Clock must not have the 'previous' attribute.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, logMsg));

    // Test that API works regardless
    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 20);
    REQUIRE(var != nullptr);
    fmi3_import_variable_t* varPrev = fmi3_import_get_variable_previous(var);
    REQUIRE(varPrev != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(varPrev) == 10);

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Invalid Clock - has attribute intermediateUpdate") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variables/invalid/intermediateUpdate_clock";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    const char* logMsg = "Variables of type 'Clock' must not have the 'intermediateUpdate' attribute.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, logMsg));

    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 0);
    REQUIRE(var != nullptr);
    fmi3_import_clock_variable_t* clock = fmi3_import_get_variable_as_clock(var);
    REQUIRE(clock != nullptr);
    REQUIRE(fmi3_import_get_variable_intermediate_update(var) != 0); // set according to XML

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Info check - intermediateUpdate ignored unless Co-Simulation") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variables/valid/intermediateUpdate_not_cs";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    const char* logMsg = "Attribute 'intermediateUpdate' ignored since FMU kind is not Co-Simulation.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, logMsg));

    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 0);
    REQUIRE(var != nullptr);
    REQUIRE(fmi3_import_get_variable_intermediate_update(var) == 0); // ingored = default is used

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Invalid previous - self reference") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variable_test/invalid/previous_self_reference";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    const char* logMsg = "A variable must not refer to itself in the attribute 'previous'.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, logMsg));

    // Test that API works regardless
    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 11);
    REQUIRE(var != nullptr);
    fmi3_import_variable_t* var_prev = fmi3_import_get_variable_previous(var);
    REQUIRE(var_prev == var);

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Invalid; duplicate variable name") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variable_test/invalid/duplicate_name";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu == nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Two variables with the same name 'sameName' found. This is not allowed."));

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Fatal failure in parsing ModelVariables."));

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Invalid; clock without intervalVariability") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variable_test/invalid/clock_no_interval_variability";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu == nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Clock': required attribute 'intervalVariability' not found"));

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Fatal failure in parsing ModelVariables."));

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Invalid derivative; VR does not resolve to any variable") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variable_test/invalid/derivative_invalid_vr";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu == nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "The valueReference in derivative=\"1\" did not resolve to any variable."));

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Fatal failure in parsing ModelVariables."));

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Test API for getting derivatives of variables") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variable_test/valid/derivate";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    fmi3_import_variable_t* var;

    // Float32
    var = fmi3_import_get_variable_by_vr(fmu, 1);
    REQUIRE(var != nullptr);
    fmi3_import_float32_variable_t* v32 = fmi3_import_get_variable_as_float32(var);
    REQUIRE(v32 != nullptr);
    fmi3_import_float32_variable_t* v32prev = fmi3_import_get_float32_variable_derivative_of(v32);
    REQUIRE(v32prev != nullptr);
    REQUIRE(fmi3_import_get_float32_variable_derivative_of(v32prev) == nullptr);
    REQUIRE(fmi3_import_get_variable_vr((fmi3_import_variable_t*)v32prev) == 0);

    //Float64
    var = fmi3_import_get_variable_by_vr(fmu, 11);
    REQUIRE(var != nullptr);
    fmi3_import_float64_variable_t* v64 = fmi3_import_get_variable_as_float64(var);
    REQUIRE(v64 != nullptr);
    fmi3_import_float64_variable_t* v64prev = fmi3_import_get_float64_variable_derivative_of(v64);
    REQUIRE(v64prev != nullptr);
    REQUIRE(fmi3_import_get_float64_variable_derivative_of(v64prev) == nullptr);
    REQUIRE(fmi3_import_get_variable_vr((fmi3_import_variable_t*)v64prev) == 10);

    // Misc API testing
    // null inputs
    REQUIRE(fmi3_import_get_variable_as_float32(nullptr) == nullptr);
    REQUIRE(fmi3_import_get_variable_as_float64(nullptr) == nullptr);

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Invalid reinit for non-continuous state") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variable_test/invalid/reinit_non_continuous";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    const char* logMsg = "The reinit attribute may only be set on continuous-time states.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, logMsg));

    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 0);
    REQUIRE(var != nullptr);
    REQUIRE(fmi3_import_get_variable_variability(var) == fmi3_variability_enu_discrete);
    fmi3_import_float64_variable_t* f64var = fmi3_import_get_variable_as_float64(var);
    REQUIRE(f64var != nullptr);
    REQUIRE(fmi3_import_get_float64_variable_reinit(f64var) == fmi3_true);

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Invalid canHandleMultipleSetPerTimeInstant; set to non-default for causality!=input") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variable_test/invalid/canHandleMultipleSetPerTimeInstant_false_non_input";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    const char* logMsg = "Only variables with causality='input' can have canHandleMultipleSetPerTimeInstant=false";
    REQUIRE(fmi3_testutil_log_contains(tfmu, logMsg));

    fmi3_import_variable_t* var = fmi3_import_get_variable_by_vr(fmu, 0);
    REQUIRE(var != nullptr);
    REQUIRE(fmi3_import_get_variable_can_handle_multiple_set_per_time_instant(var) == 0);

    fmi3_testutil_import_free(tfmu);
}
