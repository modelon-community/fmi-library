#include <stdio.h>
#include <stdint.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("fmiModelDescription: FMI3 with FMI2 attribute numberOfEventIndicators") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_description/valid/ev_ind_1";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(tfmu != nullptr);
    REQUIRE(fmu  != nullptr);
    REQUIRE(fmi3_testutil_get_num_errors(tfmu) == 0);
    REQUIRE(fmi3_testutil_log_contains(tfmu, 
            "Attribute 'numberOfEventIndicators' not processed by element 'fmiModelDescription' handle"));

    size_t nEvInd;
    fmi3_import_get_number_of_event_indicators(fmu, &nEvInd);
    REQUIRE(nEvInd == 0);

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("fmiModelDescription: Get num event indicators from ModelStructure") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_description/valid/ev_ind_2";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(tfmu != nullptr);
    REQUIRE(fmu  != nullptr);

    size_t nEvInd;
    fmi3_import_get_number_of_event_indicators(fmu, &nEvInd);
    REQUIRE(nEvInd == 2);

    fmi3_testutil_import_free(tfmu);
}