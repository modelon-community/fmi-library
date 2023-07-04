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
    const char* xmldir = FMI3_TEST_XML_DIR "/terminals_and_icons/valid/";
    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);

    SECTION("Test binary start array") {
        printf("hello there\n");
    }

    fmi3_import_free(xml);
}
