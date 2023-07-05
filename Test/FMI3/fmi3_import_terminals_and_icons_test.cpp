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
    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);
    REQUIRE(fmi3_import_get_has_terminals_and_icons(xml) != 0);

    fmi3_import_free(xml);
}

// TODO: How to properly distinguish successful parsing of modelDescription vs terminalsAndIcons?
TEST_CASE("No terminalsAndIcons.xml, test log message") {
    const char* xmldir = FMI3_TEST_XML_DIR "/terminals_and_icons/valid/no_terminalsAndIcons";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr); // successful parse of modelDescription
    int ret = fmi3_import_get_has_terminals_and_icons(fmu);
    REQUIRE(fmi3_import_get_has_terminals_and_icons(fmu) == 0); // failed parse of terminalsAndIcons

    REQUIRE(fmi3_testutil_log_contains(tfmu, "[INFO][FMI3XML] Could not find or open terminalsAndIcons.xmxl:"));
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Error check; Mismatching fmiVersions of modelDescription.xml and terminalsAndIcons.xml") {
    const char* xmldir = FMI3_TEST_XML_DIR "/terminals_and_icons/invalid/mismatching_fmiVersion";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr); // successful parse of modelDescription
    REQUIRE(fmi3_import_get_has_terminals_and_icons(fmu) == 0); // failed parse of terminalsAndIcons

    const char* logMsg = "Mismatch of attribute 'fmiVersion' in modelDescription.xml: '3.0-alpha' and terminalsAndIcons.xml: '3.0-beta'.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, logMsg));
    fmi3_testutil_import_free(tfmu);
}
