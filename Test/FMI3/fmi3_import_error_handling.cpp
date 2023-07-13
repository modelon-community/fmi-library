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
 * This file contains tests specifically to verify correctness of error handling
 */

// TODO: Possibly move these to variable tests or similar, as appropriate

// TEST_CASE("Invalid VR, missing name") {
//     const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/invalid/missing_req_attributes";
//     fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
//     fmi3_import_t* fmu = tfmu->fmu;
//     REQUIRE(fmu == nullptr);

//     // <Float64 valueReference="VR"/>
//     REQUIRE(fmi3_testutil_log_contains(tfmu, "XML element 'Float64': failed to parse attribute valueReference='VR'"));
//     REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Float64': required attribute 'name' not found"));

//     // <Float64 name="floaty"/>
//     REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Float64': required attribute 'valueReference' not found"));
//     fmi3_testutil_import_free(tfmu);
// }

TEST_CASE("Missing VRs, duplicate names") {
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/invalid/missing_VR_duplicate_name";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);

    // <Float64 name="floaty"/>
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Float64': required attribute 'valueReference' not found"));

    // duplicate VRs from valid ones
    REQUIRE(fmi3_testutil_log_contains(tfmu, "The following variables have the same valueReference: floaty3, floaty4"));

    // duplicate names from valid ones
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Two variables with the same name 'floaty' found. This is not allowed."));
    fmi3_testutil_import_free(tfmu);
}


