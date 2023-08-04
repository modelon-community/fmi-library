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

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#include "catch.hpp"

TEST_CASE("Retrieving default experiment values; values defined") {
    const char* xmldir = FMI3_TEST_XML_DIR "/default_experiment/defined";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* xml = tfmu->fmu;
    REQUIRE(xml != nullptr);

    /* test defined */
    REQUIRE(fmi3_import_get_default_experiment_has_start(xml) == 1);
    REQUIRE(fmi3_import_get_default_experiment_has_stop(xml) == 1);
    REQUIRE(fmi3_import_get_default_experiment_has_tolerance(xml) == 1);
    REQUIRE(fmi3_import_get_default_experiment_has_step_size(xml) == 1);

    /* test value */
    REQUIRE(fmi3_import_get_default_experiment_start(xml) == 2.3);
    REQUIRE(fmi3_import_get_default_experiment_stop(xml) == 3.55);
    REQUIRE(fmi3_import_get_default_experiment_tolerance(xml) == 1e-6);
    REQUIRE(fmi3_import_get_default_experiment_step_size(xml) == 2e-3);

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 0);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Retrieving default experiment values; values undefined") {
    const char* xmldir = FMI3_TEST_XML_DIR "/default_experiment/undefined";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* xml = tfmu->fmu;
    REQUIRE(xml != nullptr);

    /* test defined */
    REQUIRE(fmi3_import_get_default_experiment_has_start(xml) == 0);
    REQUIRE(fmi3_import_get_default_experiment_has_stop(xml) == 0);
    REQUIRE(fmi3_import_get_default_experiment_has_tolerance(xml) == 0);
    REQUIRE(fmi3_import_get_default_experiment_has_step_size(xml) == 0);

    /* test value */
    REQUIRE(fmi3_import_get_default_experiment_start(xml) == 0.0);
    REQUIRE(fmi3_import_get_default_experiment_stop(xml) == 1.0);
    REQUIRE(fmi3_import_get_default_experiment_tolerance(xml) == 1e-4);
    REQUIRE(fmi3_import_get_default_experiment_step_size(xml) == 1e-2);

    // These will give warnings, since attributes are not set; TODO: Is this the intended behavior?
    REQUIRE(fmi3_testutil_log_contains(tfmu, "fmi3_xml_get_default_experiment_start: returning default value, since no attribute was defined in modelDescription"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "fmi3_xml_get_default_experiment_stop: returning default value, since no attribute was defined in modelDescription"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "fmi3_xml_get_default_experiment_tolerance: returning default value, since no attribute was defined in modelDescription"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "fmi3_xml_get_default_experiment_step_size: returning default value, since no attribute was defined in modelDescription"));

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 4);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Retrieving default experiment values; mixed defined/undefined") {
    const char* xmldir = FMI3_TEST_XML_DIR "/default_experiment/mixed";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* xml = tfmu->fmu;
    REQUIRE(xml != nullptr);

    /* test defined */
    REQUIRE(fmi3_import_get_default_experiment_has_start(xml) == 1);
    REQUIRE(fmi3_import_get_default_experiment_has_stop(xml) == 0);
    REQUIRE(fmi3_import_get_default_experiment_has_tolerance(xml) == 1);
    REQUIRE(fmi3_import_get_default_experiment_has_step_size(xml) == 0);

    /* test value */
    REQUIRE(fmi3_import_get_default_experiment_start(xml) == 2.3);
    REQUIRE(fmi3_import_get_default_experiment_stop(xml) == 1.0);
    REQUIRE(fmi3_import_get_default_experiment_tolerance(xml) == 1e-6);
    REQUIRE(fmi3_import_get_default_experiment_step_size(xml) == 1e-2);

    // These will give warnings, since attributes are not set; TODO: Is this the intended behavior?
    REQUIRE(fmi3_testutil_log_contains(tfmu, "fmi3_xml_get_default_experiment_stop: returning default value, since no attribute was defined in modelDescription"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "fmi3_xml_get_default_experiment_step_size: returning default value, since no attribute was defined in modelDescription"));

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 2);
    fmi3_testutil_import_free(tfmu);
}
