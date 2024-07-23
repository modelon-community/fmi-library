/*
    Copyright (C) 2024 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include "catch.hpp"
#include "config_test.h"
#include "fmi_testutil.h"
#include "fmilib.h"

TEST_CASE("Test parse terminals and icons") {
    const char* xmldir = FMI2_TEST_XML_DIR "/terminals_and_icons/valid/basic";
    fmi2_testutil_import_t* tfmu = fmi2_testutil_parse_xml_with_log(xmldir);
    fmi2_import_t* xml = tfmu->fmu;
    REQUIRE(xml != nullptr);
    REQUIRE(fmi2_import_get_has_terminals_and_icons(xml) != 0);

    SECTION("Testing getting terminal by name") {
        fmi3_import_terminal_t* term;

        // test the existing ones
        term = fmi2_import_get_terminal_by_name(xml, "terminalA");
        REQUIRE(term != nullptr);
        REQUIRE_STREQ(fmi3_import_get_terminal_name(term), "terminalA");

        term = fmi2_import_get_terminal_by_name(xml, "terminalB");
        REQUIRE(term != nullptr);
        REQUIRE_STREQ(fmi3_import_get_terminal_name(term), "terminalB");

        term = fmi2_import_get_terminal_by_name(xml, "terminalC");
        REQUIRE(term != nullptr);
        REQUIRE_STREQ(fmi3_import_get_terminal_name(term), "terminalC");

        term = fmi2_import_get_terminal_by_name(xml, "");
        REQUIRE(term != nullptr);
        REQUIRE_STREQ(fmi3_import_get_terminal_name(term), "");
    }

    SECTION("Testing edge cases of import functions; should not crash") {
        REQUIRE(fmi2_import_get_terminal_by_name(xml, "terminalD") == nullptr); // terminal does not exists
        REQUIRE(fmi2_import_get_terminal_by_name(nullptr, "terminalA") == nullptr); // NULL
        REQUIRE(fmi2_import_get_terminal_by_name(xml, NULL) == nullptr); // NULL

        REQUIRE(fmi3_import_get_terminal_name(nullptr) == nullptr);
    }

    // TODO: Current example xml includes elements/attributes not yet parsed
    REQUIRE(fmi2_testutil_get_num_problems(tfmu) == 11);
    fmi2_testutil_import_free(tfmu);
}

TEST_CASE("No terminalsAndIcons.xml, test log message") {
    const char* xmldir = FMI2_TEST_XML_DIR "/terminals_and_icons/valid/no_terminalsAndIcons";
    fmi2_testutil_import_t* tfmu = fmi2_testutil_parse_xml_with_log(xmldir);
    fmi2_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr); // successful parse of modelDescription
    int ret = fmi2_import_get_has_terminals_and_icons(fmu);
    REQUIRE(fmi2_import_get_has_terminals_and_icons(fmu) == 0); // failed parse of terminalsAndIcons

    // TODO: Should be FMI2XML
    REQUIRE(fmi2_testutil_log_contains(tfmu, "[INFO][FMI3XML] Could not find or open terminalsAndIcons.xml:"));
    REQUIRE(fmi2_testutil_get_num_problems(tfmu) == 0);
    fmi2_testutil_import_free(tfmu);
}

TEST_CASE("Error test; terminal with missing name") {
    /* Error check tests for terminalsAndIcons are done in FMI2, 
    single test here to check principle error handling works*/
    const char* xmldir = FMI2_TEST_XML_DIR "/terminals_and_icons/invalid/missing_terminal_name";
    fmi2_testutil_import_t* tfmu = fmi2_testutil_parse_xml_with_log(xmldir);
    fmi2_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr); // successful parse of modelDescription
    REQUIRE(fmi2_import_get_has_terminals_and_icons(fmu) == 1); // successful parse of terminalsAndIcons

    REQUIRE(fmi2_testutil_log_contains(tfmu, "Parsing XML element 'Terminal': required attribute 'name' not found"));

    REQUIRE(fmi2_testutil_get_num_problems(tfmu) == 1);
    fmi2_testutil_import_free(tfmu);
}
