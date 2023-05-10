#include <stdio.h>
#include <stdlib.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

static void test_fmi3_import_get_outputs_list(fmi3_import_t* fmu) {

    fmi3_import_variable_list_t* varList = fmi3_import_get_outputs_list(fmu);
    REQUIRE(fmi3_import_get_variable_list_size(varList) == 1);

    fmi3_import_variable_t* var = fmi3_import_get_variable(varList, 0);
    REQUIRE(fmi3_import_get_variable_vr(var) == 2);

    fmi3_import_free_variable_list(varList);
}

static void test_fmi3_import_get_continuous_state_derivatives_list(fmi3_import_t* fmu) {

    fmi3_import_variable_list_t* varList = fmi3_import_get_continuous_state_derivatives_list(fmu);
    REQUIRE(fmi3_import_get_variable_list_size(varList) == 1);

    fmi3_import_variable_t* var = fmi3_import_get_variable(varList, 0);
    REQUIRE(fmi3_import_get_variable_vr(var) == 1);

    fmi3_import_free_variable_list(varList);
}

static void test_fmi3_import_get_initial_unknowns_list(fmi3_import_t* fmu) {

    fmi3_import_variable_list_t* varList = fmi3_import_get_initial_unknowns_list(fmu);
    REQUIRE(fmi3_import_get_variable_list_size(varList) == 1);

    fmi3_import_variable_t* var = fmi3_import_get_variable(varList, 0);
    REQUIRE(fmi3_import_get_variable_vr(var) == 1);

    fmi3_import_free_variable_list(varList);
}

TEST_CASE("Valid ModelStructure parsing") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/valid";

    fmi3_import_t* fmu = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(fmu != nullptr);

    SECTION("Get Outputs") {
        test_fmi3_import_get_outputs_list(fmu);
    }

    SECTION("Get ContinuousStateDerivatives") {
        test_fmi3_import_get_continuous_state_derivatives_list(fmu);
    }

    SECTION("Get InitialUnknowns") {
        test_fmi3_import_get_initial_unknowns_list(fmu);
    }
    
    fmi3_import_free(fmu);
}

TEST_CASE("Error check: ModelStructure; ContinuousStateDerivative missing derivative reference") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/invalid/derivative_reference";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu == nullptr);

    const char* err_msg = "The continuous state derivative 'state_var' does not specify the state variable that it is a derivative of.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, err_msg));

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Error check: ModelStructure; Incorrect order of elements") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/invalid/incorrect_order";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu == nullptr);

    const char* err_msg = "XML element 'Output' cannot be placed after element 'ContinuousStateDerivative', skipping";
    REQUIRE(fmi3_testutil_log_contains(tfmu, err_msg));

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Warning check: ModelStructure; FMI2 style lists") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/invalid/fmi2_style_list";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu != nullptr); // Parsing is successful

    const char* err_msg = "Unknown element 'Derivatives' in XML, skipping";
    REQUIRE(fmi3_testutil_log_contains(tfmu, err_msg));

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Error check: ModelStructure; Output incorrect causality") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/invalid/output_wrong_causality";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu == nullptr);

    const char* err_msg = "The Output 'output_var' does not have causality='output'.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, err_msg));

    fmi3_testutil_import_free(tfmu);
}
