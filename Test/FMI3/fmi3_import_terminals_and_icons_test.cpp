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

    SECTION("Testing graphicalRepresentation") {
        // TODO: FMI2
        // TODO
        ;
        SECTION("Testing coordinateSystem") {
            // TODO:
            ;
        }
        SECTION("Testing Icon") {
            // TODO:
            ;
        }
    }

    SECTION("Testing getting terminal by name") {
        fmi_import_terminal_t* term;

        // test the existing ones
        term = fmi3_import_get_terminal_by_name(xml, "terminalA");
        REQUIRE(term != nullptr);
        REQUIRE_STREQ(fmi_import_get_terminal_name(term), "terminalA");

        term = fmi3_import_get_terminal_by_name(xml, "terminalB");
        REQUIRE(term != nullptr);
        REQUIRE_STREQ(fmi_import_get_terminal_name(term), "terminalB");

        term = fmi3_import_get_terminal_by_name(xml, "terminalC");
        REQUIRE(term != nullptr);
        REQUIRE_STREQ(fmi_import_get_terminal_name(term), "terminalC");

        term = fmi3_import_get_terminal_by_name(xml, "");
        REQUIRE(term != nullptr);
        REQUIRE_STREQ(fmi_import_get_terminal_name(term), "");
    }

    SECTION("Testing edge cases of import functions; should not crash") {
        REQUIRE(fmi3_import_get_terminal_by_name(xml, "terminalD") == nullptr); // terminal does not exists
        REQUIRE(fmi3_import_get_terminal_by_name(nullptr, "terminalA") == nullptr); // NULL
        REQUIRE(fmi3_import_get_terminal_by_name(xml, NULL) == nullptr); // NULL

        REQUIRE(fmi_import_get_terminal_name(nullptr) == nullptr);
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

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Could not find or open terminalsAndIcons.xml:"));
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

TEST_CASE("Error test; terminal with missing name") {
    const char* xmldir = FMI3_TEST_XML_DIR "/terminals_and_icons/invalid/missing_terminal_name";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr); // successful parse of modelDescription
    REQUIRE(fmi3_import_get_has_terminals_and_icons(fmu) == 1); // successful parse of terminalsAndIcons

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Terminal': required attribute 'name' not found"));

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 1);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Test clearing of attribute buffer with invalid elements") {
    const char* xmldir = FMI3_TEST_XML_DIR "/terminals_and_icons/invalid/attr_buffer_clearing";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr); // successful parse of modelDescription
    REQUIRE(fmi3_import_get_has_terminals_and_icons(fmu) == 1);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Terminal': required attribute 'name' not found"));

    fmi_import_terminal_t* term = fmi3_import_get_terminal_by_name(fmu, "terminalA");
    REQUIRE(term != nullptr);
    REQUIRE_STREQ(fmi_import_get_terminal_name(term), "terminalA");
    // TODO: Check that optional attributes from Terminal without name are not present in this one

    // TODO: Current example xml includes elements/attributes not yet parsed  
    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 3);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Empty graphical representation: Takes default") {
    const char* xmldir = FMI3_TEST_XML_DIR "/terminals_and_icons/valid/empty_graphicalRepresentation";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr); // successful parse of modelDescription
    REQUIRE(fmi3_import_get_has_terminals_and_icons(fmu) == 1);

    // TODO: Verify default coordinate system + scaling + no icon

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 0);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Incomplete graphicaRepresentation->coordinateSystem: Takes default") {
    const char* xmldir = FMI3_TEST_XML_DIR "/terminals_and_icons/invalid/incomplete_coordinateSystem";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr); // successful parse of modelDescription
    REQUIRE(fmi3_import_get_has_terminals_and_icons(fmu) == 1);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'CoordinateSystem': required attribute 'x2' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'CoordinateSystem': required attribute 'y2' not found"));

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Failed to parse complete CoordinateSystem, using default system (-100, -100), (100, 100)."));
    
    // TODO: Verify default coordinate system + no icon

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 3);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Not well-defined graphicaRepresentation->coordinateSystem") {
    const char* xmldir = FMI3_TEST_XML_DIR "/terminals_and_icons/invalid/flipped_coordinateSystem";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr); // successful parse of modelDescription
    REQUIRE(fmi3_import_get_has_terminals_and_icons(fmu) == 1);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "'CoordinateSystem' not well-defined, requires x1 = 100.000000 < x2 = -100.000000."));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "'CoordinateSystem' not well-defined, requires y1 = 100.000000 < y2 = -100.000000."));
    
    // TODO: Verify coordinates are stored anyways

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 2);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Incomplete graphicalRepresentation->Icon") {
    const char* xmldir = FMI3_TEST_XML_DIR "/terminals_and_icons/invalid/incomplete_icon";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr); // successful parse of modelDescription
    REQUIRE(fmi3_import_get_has_terminals_and_icons(fmu) == 1);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Icon': required attribute 'x2' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Icon': required attribute 'y2' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Failed to parse complete Icon."));
    
    // TODO: Verify there is no icon

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 3);
    fmi3_testutil_import_free(tfmu);
}
