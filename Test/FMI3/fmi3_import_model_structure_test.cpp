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
    REQUIRE(dependency == nullptr);
    REQUIRE(factorKind == nullptr);

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

    idx = 0; // <ContinuousStateDerivative valueReference="1" dependencies="0" dependenciesKind="fixed"/>
    var = fmi3_import_get_variable(varList, idx);
    REQUIRE(fmi3_import_get_variable_vr(var) == 1);
    REQUIRE(fmi3_import_get_continuous_state_derivative_dependencies(fmu, var, &numDependencies, &dependency, &factorKind) == 0);
    REQUIRE(numDependencies == 1);
    REQUIRE(dependency[0] == 0);
    REQUIRE(factorKind[0] == fmi3_dependency_factor_kind_fixed);

    idx = 1; // <ContinuousStateDerivative valueReference="300" dependencies="200 201 202" dependenciesKind="fixed constant tunable"/>
    var = fmi3_import_get_variable(varList, idx);
    REQUIRE(fmi3_import_get_variable_vr(var) == 300);
    REQUIRE(fmi3_import_get_continuous_state_derivative_dependencies(fmu, var, &numDependencies, &dependency, &factorKind) == 0);
    REQUIRE(numDependencies == 3);
    REQUIRE(dependency[0] == 200);
    REQUIRE(dependency[1] == 201);
    REQUIRE(dependency[2] == 202);
    REQUIRE(factorKind[0] == fmi3_dependency_factor_kind_fixed);
    REQUIRE(factorKind[1] == fmi3_dependency_factor_kind_constant);
    REQUIRE(factorKind[2] == fmi3_dependency_factor_kind_tunable);

    idx = 2; // <ContinuousStateDerivative valueReference="301" dependencies="201 202" dependenciesKind="fixed constant"/>
    var = fmi3_import_get_variable(varList, idx);
    REQUIRE(fmi3_import_get_variable_vr(var) == 301);
    REQUIRE(fmi3_import_get_continuous_state_derivative_dependencies(fmu, var, &numDependencies, &dependency, &factorKind) == 0);
    REQUIRE(numDependencies == 2);
    REQUIRE(dependency[0] == 201);
    REQUIRE(dependency[1] == 202);
    REQUIRE(factorKind[0] == fmi3_dependency_factor_kind_fixed);
    REQUIRE(factorKind[1] == fmi3_dependency_factor_kind_constant);

    idx = 3; // <ContinuousStateDerivative valueReference="302" dependencies="202" dependenciesKind="constant"/>
    var = fmi3_import_get_variable(varList, idx);
    REQUIRE(fmi3_import_get_variable_vr(var) == 302);
    REQUIRE(fmi3_import_get_continuous_state_derivative_dependencies(fmu, var, &numDependencies, &dependency, &factorKind) == 0);
    REQUIRE(numDependencies == 1);
    REQUIRE(dependency[0] == 202);
    REQUIRE(factorKind[0] == fmi3_dependency_factor_kind_constant);

    idx = 4; // <ContinuousStateDerivative valueReference="303"/>
    var = fmi3_import_get_variable(varList, idx);
    REQUIRE(fmi3_import_get_variable_vr(var) == 303);
    REQUIRE(fmi3_import_get_continuous_state_derivative_dependencies(fmu, var, &numDependencies, &dependency, &factorKind) == 0);
    REQUIRE(numDependencies == SIZE_MAX); // depends on all
    REQUIRE(dependency == nullptr);
    REQUIRE(factorKind == nullptr);

    idx = 5; // <ContinuousStateDerivative valueReference="304" dependencies="" dependenciesKind=""/>
    var = fmi3_import_get_variable(varList, idx);
    REQUIRE(fmi3_import_get_variable_vr(var) == 304);
    REQUIRE(fmi3_import_get_continuous_state_derivative_dependencies(fmu, var, &numDependencies, &dependency, &factorKind) == 0);
    REQUIRE(numDependencies == 0); // no dependencies
    REQUIRE(dependency == nullptr);
    REQUIRE(factorKind == nullptr);

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
    REQUIRE(dependency == nullptr);
    REQUIRE(factorKind == nullptr);

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
    REQUIRE(dependency == nullptr);
    REQUIRE(factorKind == nullptr);

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
    REQUIRE(dependency == nullptr);
    REQUIRE(factorKind == nullptr);

    fmi3_import_free_variable_list(varList);
}

static void test_fmi3_import_get_dependencies_invalid_API_calls(fmi3_import_t* fmu) {

    REQUIRE(fmi3_import_get_continuous_state_derivatives_list(nullptr) == nullptr); // invalid fmu input

    fmi3_import_variable_list_t* varListCont = fmi3_import_get_continuous_state_derivatives_list(fmu);
    REQUIRE(fmi3_import_get_variable_list_size(varListCont) > 0); // make sure there are ContinuousStateDerivatives

    fmi3_import_variable_list_t* varListEvent = fmi3_import_get_event_indicators_list(fmu);
    REQUIRE(fmi3_import_get_variable_list_size(varListEvent) > 0); // make sure there are EventIndicators

    size_t numDependencies;
    size_t* dependency;
    char* factorKind;
    fmi3_import_variable_t* var = fmi3_import_get_variable(varListEvent, 0);

    // no FMU
    REQUIRE(fmi3_import_get_continuous_state_derivative_dependencies(nullptr, var, &numDependencies, &dependency, &factorKind) < 0);
    // no variable
    REQUIRE(fmi3_import_get_continuous_state_derivative_dependencies(fmu, nullptr, &numDependencies, &dependency, &factorKind) < 0);
    // variable not a ContinuousStateDerivative
    REQUIRE(fmi3_import_get_continuous_state_derivative_dependencies(fmu, var, &numDependencies, &dependency, &factorKind) > 0);

    fmi3_import_free_variable_list(varListCont);
    fmi3_import_free_variable_list(varListEvent);
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

    SECTION("Invalid API calls") {
        test_fmi3_import_get_dependencies_invalid_API_calls(fmu);
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

TEST_CASE("Error check: ModelStructure; Negative dependency value") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/invalid/dependencies_invalid_VR";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu == nullptr);

    const char* logMsg = "XML element 'Unknown': item 0=-1 is negative in the list for attribute 'dependencies'";
    REQUIRE(fmi3_testutil_log_contains(tfmu, logMsg));

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Error check: ModelStructure; Invalid dependency value; not a number") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/invalid/dependencies_invalid_VR2";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu == nullptr);

    const char* logMsg = "XML element 'Unknown': could not parse item 0 in the list for attribute 'dependencies'";
    REQUIRE(fmi3_testutil_log_contains(tfmu, logMsg));

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Error check: ModelStructure; Invalid dependency value; no variable with that VR") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/invalid/dependencies_invalid_VR_no_var";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    // TODO: Extra error check required to make this invalid
    REQUIRE(tfmu->fmu != nullptr);

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Error check: ModelStructure; Mismatched number of dependencies and dependenciesKind") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/invalid/dependencies_mismatched_list_lengthes";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu == nullptr);

    const char* logMsg = "XML element 'Unknown': different number of items (1 and 2) in the lists for 'dependencies' and 'dependenciesKind'";
    REQUIRE(fmi3_testutil_log_contains(tfmu, logMsg));

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Error check: ModelStructure; Invalid dependenciesKind item in list") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/invalid/dependencies_invalid_kind";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu == nullptr);

    const char* logMsg = "XML element 'Unknown': could not parse item 0 in the list for attribute 'dependenciesKind'";
    REQUIRE(fmi3_testutil_log_contains(tfmu, logMsg));

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Error check: ModelStructure; dependencies missing but dependenciesKind present") {
    const char* xmldir = FMI3_TEST_XML_DIR "/model_structure/invalid/dependencies_missing_kind_present";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu == nullptr);

    const char* logMsg = "XML element 'Unknown': if `dependenciesKind` attribute is present then the `dependencies` attribute must be present also.";
    REQUIRE(fmi3_testutil_log_contains(tfmu, logMsg));

    fmi3_testutil_import_free(tfmu);
}
