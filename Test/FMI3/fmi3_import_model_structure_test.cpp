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

static void test_fmi3_import_get_clocked_states_list(fmi3_import_t* fmu) {

    fmi3_import_variable_list_t* varList = fmi3_import_get_clocked_states_list(fmu);
    REQUIRE(fmi3_import_get_variable_list_size(varList) == 1);

    fmi3_import_variable_t* var = fmi3_import_get_variable(varList, 0);
    REQUIRE(fmi3_import_get_variable_vr(var) == 11);

    fmi3_import_free_variable_list(varList);
}

static void test_fmi3_import_get_initial_unknowns_list(fmi3_import_t* fmu) {

    fmi3_import_variable_list_t* varList = fmi3_import_get_initial_unknowns_list(fmu);
    REQUIRE(fmi3_import_get_variable_list_size(varList) == 1);

    fmi3_import_variable_t* var = fmi3_import_get_variable(varList, 0);
    REQUIRE(fmi3_import_get_variable_vr(var) == 1);

    fmi3_import_free_variable_list(varList);
}

static void test_fmi3_import_get_event_indicators_list(fmi3_import_t* fmu) {

    fmi3_import_variable_list_t* varList = fmi3_import_get_event_indicators_list(fmu);
    REQUIRE(fmi3_import_get_variable_list_size(varList) == 1);

    fmi3_import_variable_t* var = fmi3_import_get_variable(varList, 0);
    REQUIRE(fmi3_import_get_variable_vr(var) == 100);

    fmi3_import_free_variable_list(varList);
}

TEST_CASE("Valid ModelStructure parsing") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/valid/basic";

    fmi3_import_t* fmu = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(fmu != nullptr);

    SECTION("Get Outputs") {
        test_fmi3_import_get_outputs_list(fmu);
    }

    SECTION("Get ContinuousStateDerivatives") {
        test_fmi3_import_get_continuous_state_derivatives_list(fmu);
    }

    SECTION("Get ClockedStates") {
        test_fmi3_import_get_clocked_states_list(fmu);
    }

    SECTION("Get InitialUnknowns") {
        test_fmi3_import_get_initial_unknowns_list(fmu);
    }

    SECTION("Get EventIndicators") {
        test_fmi3_import_get_event_indicators_list(fmu);
    }
    
    fmi3_import_free(fmu);
}

TEST_CASE("Error check: ModelStructure; ContinuousStateDerivative missing derivative reference") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/invalid/derivative_reference";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu == nullptr);

    const char* errMsg = "The variable 'state_var' is a ContinuousStateDerivative, but does not specify the state variable it is a derivative of.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, errMsg));

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Error check: ModelStructure; Incorrect order of elements") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/invalid/incorrect_order";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu == nullptr);

    const char* errMsg = "XML element 'Output' cannot be placed after element 'ContinuousStateDerivative', skipping";
    REQUIRE(fmi3_testutil_log_contains(tfmu, errMsg));

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Warning check: ModelStructure; FMI2 style lists") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/invalid/fmi2_style_list";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu != nullptr); // Parsing is successful

    const char* errMsg = "Unknown element 'Derivatives' in XML, skipping";
    REQUIRE(fmi3_testutil_log_contains(tfmu, errMsg));

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Error check: ModelStructure; Output incorrect causality") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/invalid/output_wrong_causality";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu == nullptr);

    const char* errMsg = "The variable 'output_var' is an Output, but does not have causality='output'.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, errMsg));

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Error check: ModelStructure; EventIndicator variability") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/invalid/event_indicator_variability";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu == nullptr);

    const char* errMsg = "The variable 'event_ind' is an EventIndicator, but does not have variability='continuous'.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, errMsg));

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Error check: ModelStructure; EventIndicator base type") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/invalid/event_indicator_base_type";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu == nullptr);

    const char* errMsg = "The variable 'event_ind' is an EventIndicator, but does not have the base type 'Float32' or 'Float64'.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, errMsg));

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Info check: ModelStructure; EventIndicator ignored for Co-Simulation") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/valid/event_indictor_CS";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu != nullptr); // Parsing successful

    const char* logMsg = "EventIndicator ignored since FMU kind is Co-Simulation or Scheduled Excecution.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, logMsg));

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Error check: ModelStructure; ClockedState without clocks attribute") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/invalid/clocked_state_no_clocks";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu == nullptr);

    const char* logMsg = "The variable 'clocked_var' is a ClockedState, but does not define the attribute 'clocks'.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, logMsg));

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Error check: ModelStructure; ClockedState without previous attribute") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/invalid/clocked_state_no_previous";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu == nullptr);

    const char* logMsg = "The variable 'clocked_var' is a ClockedState, but does not define the attribute 'previous'.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, logMsg));

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Error check: ModelStructure; ClockedState with invalid variability, not discrete") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/invalid/clocked_state_not_discrete";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu == nullptr);

    const char* logMsg = "The variable 'clocked_var' is a ClockedState, but does not have variability='discrete'.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, logMsg));

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Error check: ModelStructure; ClockedState has Clock base type") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/invalid/clocked_state_is_clock";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu == nullptr);

    const char* logMsg = "The variable 'clock_01' is a ClockedState, but has the base type 'fmi3Clock'.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, logMsg));

    fmi3_testutil_import_free(tfmu);
}
