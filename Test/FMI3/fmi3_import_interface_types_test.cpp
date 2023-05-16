#include <stdio.h>
#include <stdint.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("FMU interface types: Default attributes") {
    const char* xmldir = FMI3_TEST_XML_DIR "/interface_types/valid/default_attrs";

    fmi3_import_t* fmu = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(fmu != nullptr);

    SECTION("ME") {
        REQUIRE_STREQ(fmi3_import_get_model_identifier_ME(fmu), "id_me");
    }
    SECTION("CS") {
        REQUIRE_STREQ(fmi3_import_get_model_identifier_CS(fmu), "id_cs");
    }
    SECTION("SE") {
        REQUIRE_STREQ(fmi3_import_get_model_identifier_SE(fmu), "id_se");
    }

    fmi3_import_free(fmu);
}

//TEST_CASE("FMU interface types: All attributes") {
//}