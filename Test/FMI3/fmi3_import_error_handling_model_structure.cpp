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
 * This file contains tests specifically to verify correctness of error handling when parsing ModelStructure.
 * Such as:
 * Elements with multiple issues raise all appropriate errors warnings.
 * Invalid elements are discarded, but valid ones are still parsed
 * 
 * NOTE: No separation into valid/invalid, all tests contain errors/warnings
 */

TEST_CASE("Test missing VRs") {
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/model_structure/missing_VRs";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'Output': required attribute 'valueReference' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'ContinuousStateDerivative': required attribute 'valueReference' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'ClockedState': required attribute 'valueReference' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'InitialUnknown': required attribute 'valueReference' not found"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Parsing XML element 'EventIndicator': required attribute 'valueReference' not found"));

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Model structure is not valid due to detected errors. Cannot continue.")); // counts as 2

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 7);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Test invalid VRs") {
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/model_structure/invalid_VRs";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Failed to find variable for valueReference=1."));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Failed to find variable for valueReference=2."));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Failed to find variable for valueReference=3."));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Failed to find variable for valueReference=4."));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Failed to find variable for valueReference=5."));

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Model structure is not valid due to detected errors. Cannot continue.")); // counts as 2

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 7);
    fmi3_testutil_import_free(tfmu);
}

// Default dependency setup
static void test_fmi3_default_dependency(size_t numDependencies, int dependsOnAll, size_t* dependencies, char* dependenciesKind) {
    REQUIRE(numDependencies == 0);
    REQUIRE(dependsOnAll == 1);
    REQUIRE(dependencies == nullptr);
    REQUIRE(dependenciesKind == nullptr);
}

TEST_CASE("Test multiple invalid dependencies") {
    // Failed dependency parsing requires some cleanup in the parser
    // Test all invalid variants together with valid ones
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/model_structure/invalid_dependencies";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    // Test all dependencies
    size_t numDependencies;
    int dependsOnAll;
    size_t* dependencies;
    char* dependenciesKind;
    fmi3_import_variable_t* var;

    // number of test cases
    int numFirstValid = 3;
    int numInvalid = 5;
    int numSecondValid = 3;

    // Get ContinuousStateDerivative list, all ones should be valid
    fmi3_import_variable_list_t* varList = fmi3_import_get_continuous_state_derivatives_list(fmu);
    REQUIRE(fmi3_import_get_variable_list_size(varList) == numFirstValid + numInvalid + numSecondValid);

    SECTION("First set of valid ones") {
        size_t idx = 0;

        var = fmi3_import_get_variable(varList, 0);
        REQUIRE(fmi3_import_get_variable_vr(var) == 100);
        REQUIRE(fmi3_import_get_continuous_state_derivative_dependencies(fmu, var, &numDependencies, &dependsOnAll, &dependencies, &dependenciesKind) == 0);
        test_fmi3_default_dependency(numDependencies, dependsOnAll, dependencies, dependenciesKind);

        var = fmi3_import_get_variable(varList, 1);
        REQUIRE(fmi3_import_get_variable_vr(var) == 101);
        REQUIRE(fmi3_import_get_continuous_state_derivative_dependencies(fmu, var, &numDependencies, &dependsOnAll, &dependencies, &dependenciesKind) == 0);
        REQUIRE(numDependencies == 0);
        REQUIRE(dependsOnAll == 0); // no dependencies
        REQUIRE(dependencies == nullptr);
        REQUIRE(dependenciesKind == nullptr);

        var = fmi3_import_get_variable(varList, 2);
        REQUIRE(fmi3_import_get_variable_vr(var) == 102);
        REQUIRE(fmi3_import_get_continuous_state_derivative_dependencies(fmu, var, &numDependencies, &dependsOnAll, &dependencies, &dependenciesKind) == 0);
        REQUIRE(numDependencies == 3);
        REQUIRE(dependencies[0] == 0);
        REQUIRE(dependencies[1] == 1);
        REQUIRE(dependencies[2] == 2);
        REQUIRE(dependenciesKind[0] == fmi3_dependencies_kind_fixed);
        REQUIRE(dependenciesKind[1] == fmi3_dependencies_kind_constant);
        REQUIRE(dependenciesKind[2] == fmi3_dependencies_kind_tunable);
    }

    SECTION("Invalid ones") {
        size_t idx = numFirstValid;
        REQUIRE(fmi3_testutil_log_contains(tfmu, "'ContinuousStateDerivative': could not parse item 0 in the list for attribute 'dependenciesKind'"));
        REQUIRE(fmi3_testutil_log_contains(tfmu, "'ContinuousStateDerivative': Attribute 'dependencies' contains invalid value: -1."));
        REQUIRE(fmi3_testutil_log_contains(tfmu, "'ContinuousStateDerivative': could not parse item 0, character 'a' in the list for attribute 'dependencies'"));
        REQUIRE(fmi3_testutil_log_contains(tfmu, "'ContinuousStateDerivative': different number of items (3 and 4) in the lists for 'dependencies' and 'dependenciesKind'"));
        REQUIRE(fmi3_testutil_log_contains(tfmu, "'ContinuousStateDerivative': if `dependenciesKind` attribute is present then the `dependencies` attribute must also be present."));

        // loop since all should result in default dependencies
        for (size_t i = 0; i < numInvalid; i++) {
            fmi3_value_reference_t vr = 110 + i;
            INFO("Testing invalid dependencies for valueReference = " << vr);
            var = fmi3_import_get_variable(varList, idx++);
            REQUIRE(fmi3_import_get_variable_vr(var) == vr);
            REQUIRE(fmi3_import_get_continuous_state_derivative_dependencies(fmu, var, &numDependencies, &dependsOnAll, &dependencies, &dependenciesKind) == 0);
            test_fmi3_default_dependency(numDependencies, dependsOnAll, dependencies, dependenciesKind);
        }
    }

    // These are the essential ones, the invalid ones should not corrupt the internal data storage
    SECTION("Second set of valid ones") {
        size_t idx = numFirstValid + numInvalid;

        var = fmi3_import_get_variable(varList, idx++);
        REQUIRE(fmi3_import_get_variable_vr(var) == 120);
        REQUIRE(fmi3_import_get_continuous_state_derivative_dependencies(fmu, var, &numDependencies, &dependsOnAll, &dependencies, &dependenciesKind) == 0);
        test_fmi3_default_dependency(numDependencies, dependsOnAll, dependencies, dependenciesKind);

        var = fmi3_import_get_variable(varList, idx++);
        REQUIRE(fmi3_import_get_variable_vr(var) == 121);
        REQUIRE(fmi3_import_get_continuous_state_derivative_dependencies(fmu, var, &numDependencies, &dependsOnAll, &dependencies, &dependenciesKind) == 0);
        REQUIRE(numDependencies == 0);
        REQUIRE(dependsOnAll == 0); // no dependencies
        REQUIRE(dependencies == nullptr);
        REQUIRE(dependenciesKind == nullptr);

        var = fmi3_import_get_variable(varList, idx++);
        REQUIRE(fmi3_import_get_variable_vr(var) == 122);
        REQUIRE(fmi3_import_get_continuous_state_derivative_dependencies(fmu, var, &numDependencies, &dependsOnAll, &dependencies, &dependenciesKind) == 0);
        REQUIRE(numDependencies == 3);
        REQUIRE(dependencies[0] == 20);
        REQUIRE(dependencies[1] == 21);
        REQUIRE(dependencies[2] == 22);
        REQUIRE(dependenciesKind[0] == fmi3_dependencies_kind_fixed);
        REQUIRE(dependenciesKind[1] == fmi3_dependencies_kind_constant);
        REQUIRE(dependenciesKind[2] == fmi3_dependencies_kind_tunable);
    }

    fmi3_import_free_variable_list(varList);

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 5);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Clocked States; multiple attribute issues") {
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/model_structure/clocked_state_multiple_issues";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "The variable 'clock' is a ClockedState, but does not define the attribute 'previous'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "The variable 'clock' is a ClockedState, but has the base type 'fmi3Clock'"));

    fmi3_import_variable_list_t* varList = fmi3_import_get_clocked_states_list(fmu);
    REQUIRE(fmi3_import_get_variable_list_size(varList) == 1);
    fmi3_import_variable_t* var = fmi3_import_get_variable(varList, 0);
    REQUIRE(var != nullptr);

    // does not have previous
    REQUIRE(fmi3_import_get_variable_previous(var) == nullptr);

    // is Clock
    fmi3_import_clock_variable_t* clockVar = fmi3_import_get_variable_as_clock(var);
    REQUIRE(clockVar != nullptr);

    fmi3_import_free_variable_list(varList);  

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 2);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("EventIndicators; multiple attribute issues") {
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/model_structure/event_indicators_multiple_issues";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "The variable 'event_ind' is an EventIndicator, but does not have variability='continuous'"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "The variable 'event_ind' is an EventIndicator, but does not have the base type 'Float32' or 'Float64'"));

    fmi3_import_variable_list_t* varList = fmi3_import_get_event_indicators_list(fmu);
    REQUIRE(fmi3_import_get_variable_list_size(varList) == 1);
    fmi3_import_variable_t* var = fmi3_import_get_variable(varList, 0);
    REQUIRE(var != nullptr);

    // not continuous
    REQUIRE(fmi3_import_get_variable_variability(var) == fmi3_variability_enu_discrete);

    // not float
    fmi3_import_int64_variable_t* i64Var = fmi3_import_get_variable_as_int64(var);
    REQUIRE(i64Var != nullptr);

    fmi3_import_free_variable_list(varList);  

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 2);
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Attribute fuffer clearing; attributes from invalid elements need to be cleared") {
    const char* xmldir = FMI3_TEST_XML_DIR "/error_handling/model_structure/attr_buffer_clearing";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu == nullptr); // contains an invalid ContinuousStateDerivative

    REQUIRE(fmi3_testutil_log_contains(tfmu, "'ContinuousStateDerivative': required attribute 'valueReference' not found"));
    // No errors on invalid dependencies attribute values
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Model structure is not valid due to detected errors. Cannot continue.")); // counts as 2  

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 3);
    fmi3_testutil_import_free(tfmu);
}
