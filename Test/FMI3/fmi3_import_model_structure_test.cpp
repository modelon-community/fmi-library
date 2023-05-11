#include <stdio.h>
#include <stdlib.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

static void test_fmi3_import_get_outputs(fmi3_import_t* fmu) {

    fmi3_import_variable_list_t* varList = fmi3_import_get_outputs_list(fmu);
    REQUIRE(fmi3_import_get_variable_list_size(varList) == 1);

    /* check VRs and dependencies of all Outputs */
    size_t numDependencies;
    size_t* dependency;
    char* factorKind;

    fmi3_import_variable_t* var = fmi3_import_get_variable(varList, 0);
    REQUIRE(fmi3_import_get_variable_vr(var) == 2);
    REQUIRE(fmi3_import_get_output_dependencies(fmu, var, &numDependencies, &dependency, &factorKind) == 0);
    REQUIRE(numDependencies == SIZE_MAX); // depends on all

    fmi3_import_free_variable_list(varList);
}

static void test_fmi3_import_get_continuous_state_derivatives(fmi3_import_t* fmu) {

    fmi3_import_variable_list_t* varList = fmi3_import_get_continuous_state_derivatives_list(fmu);
    REQUIRE(fmi3_import_get_variable_list_size(varList) == 6);

    /* check VRs and dependencies of all continuousStateDerivatives */
    size_t numDependencies;
    size_t* dependency;
    char* factorKind;
    fmi3_import_variable_t* var;
    size_t idx = 0;

    idx = 0; // <ContinuousStateDerivative valueReference="1" dependencies="0"/> 
    var = fmi3_import_get_variable(varList, idx);
    REQUIRE(fmi3_import_get_variable_vr(var) == 1);
    REQUIRE(fmi3_import_get_continuous_state_derivative_dependencies(fmu, var, &numDependencies, &dependency, &factorKind) == 0);
    REQUIRE(numDependencies == 1);
    REQUIRE(dependency[0] == 0);

    idx = 1; // <ContinuousStateDerivative valueReference="300" dependencies="200 201 202"/>
    var = fmi3_import_get_variable(varList, idx);
    REQUIRE(fmi3_import_get_variable_vr(var) == 300);
    REQUIRE(fmi3_import_get_continuous_state_derivative_dependencies(fmu, var, &numDependencies, &dependency, &factorKind) == 0);
    REQUIRE(numDependencies == 3);
    REQUIRE(dependency[0] == 200);
    REQUIRE(dependency[1] == 201);
    REQUIRE(dependency[2] == 202);

    idx = 2; // <ContinuousStateDerivative valueReference="301" dependencies="201 202"/>
    var = fmi3_import_get_variable(varList, idx);
    REQUIRE(fmi3_import_get_variable_vr(var) == 301);
    REQUIRE(fmi3_import_get_continuous_state_derivative_dependencies(fmu, var, &numDependencies, &dependency, &factorKind) == 0);
    REQUIRE(numDependencies == 2);
    REQUIRE(dependency[0] == 201);
    REQUIRE(dependency[1] == 202);

    idx = 3; // <ContinuousStateDerivative valueReference="302" dependencies="202"/>
    var = fmi3_import_get_variable(varList, idx);
    REQUIRE(fmi3_import_get_variable_vr(var) == 302);
    REQUIRE(fmi3_import_get_continuous_state_derivative_dependencies(fmu, var, &numDependencies, &dependency, &factorKind) == 0);
    REQUIRE(numDependencies == 1);
    REQUIRE(dependency[0] == 202);

    idx = 4; // <ContinuousStateDerivative valueReference="303"/>
    var = fmi3_import_get_variable(varList, idx);
    REQUIRE(fmi3_import_get_variable_vr(var) == 303);
    REQUIRE(fmi3_import_get_continuous_state_derivative_dependencies(fmu, var, &numDependencies, &dependency, &factorKind) == 0);
    REQUIRE(numDependencies == SIZE_MAX); // depends on all

    idx = 5; // <ContinuousStateDerivative valueReference="304" dependencies=""/>
    var = fmi3_import_get_variable(varList, idx);
    REQUIRE(fmi3_import_get_variable_vr(var) == 304);
    REQUIRE(fmi3_import_get_continuous_state_derivative_dependencies(fmu, var, &numDependencies, &dependency, &factorKind) == 0);
    REQUIRE(numDependencies == 0); // no dependencies

    fmi3_import_free_variable_list(varList);
}

static void test_fmi3_import_get_clocked_states(fmi3_import_t* fmu) {

    fmi3_import_variable_list_t* varList = fmi3_import_get_clocked_states_list(fmu);
    REQUIRE(fmi3_import_get_variable_list_size(varList) == 1);

    /* check VRs and dependencies of all ClockedStates */
    size_t numDependencies;
    size_t* dependency;
    char* factorKind;

    fmi3_import_variable_t* var = fmi3_import_get_variable(varList, 0);
    REQUIRE(fmi3_import_get_variable_vr(var) == 11);
    REQUIRE(fmi3_import_get_clocked_state_dependencies(fmu, var, &numDependencies, &dependency, &factorKind) == 0);
    REQUIRE(numDependencies == SIZE_MAX); // depends on all

    fmi3_import_free_variable_list(varList);
}

static void test_fmi3_import_get_initial_unknowns(fmi3_import_t* fmu) {

    fmi3_import_variable_list_t* varList = fmi3_import_get_initial_unknowns_list(fmu);
    REQUIRE(fmi3_import_get_variable_list_size(varList) == 1);

    /* check VRs and dependencies of all InitialUnknowns */
    size_t numDependencies;
    size_t* dependency;
    char* factorKind;

    fmi3_import_variable_t* var = fmi3_import_get_variable(varList, 0);
    REQUIRE(fmi3_import_get_variable_vr(var) == 1);
    REQUIRE(fmi3_import_get_initial_unknown_dependencies(fmu, var, &numDependencies, &dependency, &factorKind) == 0);
    REQUIRE(numDependencies == SIZE_MAX); // depends on all

    fmi3_import_free_variable_list(varList);
}

static void test_fmi3_import_get_event_indicators(fmi3_import_t* fmu) {

    fmi3_import_variable_list_t* varList = fmi3_import_get_event_indicators_list(fmu);
    REQUIRE(fmi3_import_get_variable_list_size(varList) == 1);

    /* check VRs and dependencies of all EventIndicators */
    size_t numDependencies;
    size_t* dependency;
    char* factorKind;

    fmi3_import_variable_t* var = fmi3_import_get_variable(varList, 0);
    REQUIRE(fmi3_import_get_variable_vr(var) == 100);
    REQUIRE(fmi3_import_get_event_indicator_dependencies(fmu, var, &numDependencies, &dependency, &factorKind) == 0);
    REQUIRE(numDependencies == SIZE_MAX); // depends on all

    fmi3_import_free_variable_list(varList);
}

TEST_CASE("Valid ModelStructure parsing") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/valid/basic";

    fmi3_import_t* fmu = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(fmu != nullptr);

    SECTION("Get Outputs") {
        test_fmi3_import_get_outputs(fmu);
    }

    SECTION("Get ContinuousStateDerivatives") {
        test_fmi3_import_get_continuous_state_derivatives(fmu);
    }

    SECTION("Get ClockedStates") {
        test_fmi3_import_get_clocked_states(fmu);
    }

    SECTION("Get InitialUnknowns") {
        test_fmi3_import_get_initial_unknowns(fmu);
    }

    SECTION("Get EventIndicators") {
        test_fmi3_import_get_event_indicators(fmu);
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
