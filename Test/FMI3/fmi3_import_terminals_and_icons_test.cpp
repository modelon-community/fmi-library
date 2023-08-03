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
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include "catch.hpp"
#include "config_test.h"
#include "fmi_testutil.h"
#include "fmilib.h"

TEST_CASE("Test parse terminals and icons") {
    const char* xmldir = FMI3_TEST_XML_DIR "/terminals_and_icons/valid/basic";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* xml = tfmu->fmu;
    REQUIRE(xml != nullptr);
    REQUIRE(fmi3_import_get_has_terminals_and_icons(xml) != 0);

    SECTION("Testing getting terminal by name") {
        fmi3_import_terminal_t* term;

        // test the existing ones
        term = fmi3_import_get_terminal_by_name(xml, "terminalA");
        REQUIRE(term != nullptr);
        REQUIRE_STREQ(fmi3_import_get_terminal_name(term), "terminalA");

        term = fmi3_import_get_terminal_by_name(xml, "terminalB");
        REQUIRE(term != nullptr);
        REQUIRE_STREQ(fmi3_import_get_terminal_name(term), "terminalB");

        term = fmi3_import_get_terminal_by_name(xml, "terminalC");
        REQUIRE(term != nullptr);
        REQUIRE_STREQ(fmi3_import_get_terminal_name(term), "terminalC");

        term = fmi3_import_get_terminal_by_name(xml, "");
        REQUIRE(term != nullptr);
        REQUIRE_STREQ(fmi3_import_get_terminal_name(term), "");
    }

    SECTION("Testing edge cases of import functions; should not crash") {
        REQUIRE(fmi3_import_get_terminal_by_name(xml, "terminalD") == nullptr); // terminal does not exists
        REQUIRE(fmi3_import_get_terminal_by_name(nullptr, "terminalA") == nullptr); // NULL
        REQUIRE(fmi3_import_get_terminal_by_name(xml, NULL) == nullptr); // NULL

        REQUIRE(fmi3_import_get_terminal_name(nullptr) == nullptr);
    }

    // TODO: Current example xml includes elements/attributes not yet parsed
    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 11); 
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("No terminalsAndIcons.xml, test log message") {
    const char* xmldir = FMI3_TEST_XML_DIR "/terminals_and_icons/valid/no_terminalsAndIcons";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr); // successful parse of modelDescription
    int ret = fmi3_import_get_has_terminals_and_icons(fmu);
    REQUIRE(fmi3_import_get_has_terminals_and_icons(fmu) == 0); // failed parse of terminalsAndIcons

    REQUIRE(fmi3_testutil_log_contains(tfmu, "[INFO][FMI3XML] Could not find or open terminalsAndIcons.xml:"));
    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 0);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Error check; Mismatching fmiVersions of modelDescription.xml and terminalsAndIcons.xml") {
    const char* xmldir = FMI3_TEST_XML_DIR "/terminals_and_icons/invalid/mismatching_fmiVersion";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr); // successful parse of modelDescription
    REQUIRE(fmi3_import_get_has_terminals_and_icons(fmu) == 0); // failed parse of terminalsAndIcons

    const char* logMsg = "Mismatch of attribute 'fmiVersion' in modelDescription.xml: '3.0-alpha' and terminalsAndIcons.xml: '3.0-beta'.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, logMsg)); // fatal, counts as 2

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 2);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Error check; Terminals with duplicate names") {
    const char* xmldir = FMI3_TEST_XML_DIR "/terminals_and_icons/invalid/duplicate_terminal_name";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr); // successful parse of modelDescription
    REQUIRE(fmi3_import_get_has_terminals_and_icons(fmu) == 0); // failed parse of terminalsAndIcons

    const char* logMsg = "Two terminals with the same name 'terminalA' found. This is not allowed by the specification.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, logMsg)); // fatal, counts as 2

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 2);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Error check; Terminals with duplicate names; edge case of empty name") {
    const char* xmldir = FMI3_TEST_XML_DIR "/terminals_and_icons/invalid/duplicate_terminal_empty_name";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr); // successful parse of modelDescription
    REQUIRE(fmi3_import_get_has_terminals_and_icons(fmu) == 0); // failed parse of terminalsAndIcons

    const char* logMsg = "Two terminals with the same name '' found. This is not allowed by the specification.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, logMsg)); // fatal, counts as 2

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 2);
    fmi3_testutil_import_free(tfmu);
}
