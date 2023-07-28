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
#include <stdlib.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

// Collection of various tests on valid combinations & defaults for causality, variablity and initial

TEST_CASE("Default valid variability") {
    // Float
    // constants: fixed
    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_structural_parameter, 1) == fmi3_variability_enu_fixed);
    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_parameter, 1)            == fmi3_variability_enu_fixed);
    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_calculated_parameter, 1) == fmi3_variability_enu_fixed);
    // non-constants: continuous
    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_input, 1)                == fmi3_variability_enu_continuous);
    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_output, 1)               == fmi3_variability_enu_continuous);
    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_local, 1)                == fmi3_variability_enu_continuous);
    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_independent, 1)          == fmi3_variability_enu_continuous);

    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_unknown, 1)              == fmi3_variability_enu_unknown);

    // Non-float
    // constants: fixed
    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_structural_parameter, 0) == fmi3_variability_enu_fixed);
    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_parameter, 0)            == fmi3_variability_enu_fixed);
    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_calculated_parameter, 0) == fmi3_variability_enu_fixed);
    // non-constants: discrete
    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_input, 0)                == fmi3_variability_enu_discrete);
    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_output, 0)               == fmi3_variability_enu_discrete);
    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_local, 0)                == fmi3_variability_enu_discrete);
    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_independent, 0)          == fmi3_variability_enu_continuous);

    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_unknown, 0)              == fmi3_variability_enu_unknown);

}

#define N_VARIABILITIES (6)

static void validate_initial(fmi3_causality_enu_t c, fmi3_initial_enu_t expected[]) {
    REQUIRE(expected[0] == fmi3_get_default_initial(fmi3_variability_enu_constant,   c));
    REQUIRE(expected[1] == fmi3_get_default_initial(fmi3_variability_enu_fixed,      c));
    REQUIRE(expected[2] == fmi3_get_default_initial(fmi3_variability_enu_tunable,    c));
    REQUIRE(expected[3] == fmi3_get_default_initial(fmi3_variability_enu_discrete,   c));
    REQUIRE(expected[4] == fmi3_get_default_initial(fmi3_variability_enu_continuous, c));
    REQUIRE(expected[5] == fmi3_get_default_initial(fmi3_variability_enu_unknown,    c));
}

TEST_CASE("Default initial for causality = structuralParameter") {
    fmi3_initial_enu_t expected[N_VARIABILITIES] = {
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_exact,
        fmi3_initial_enu_exact,
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_unknown
    };
    validate_initial(fmi3_causality_enu_structural_parameter, expected);
}

TEST_CASE("Default initial for causality = parameter") {
    fmi3_initial_enu_t expected[N_VARIABILITIES] = {
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_exact,
        fmi3_initial_enu_exact,
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_unknown
    };
    validate_initial(fmi3_causality_enu_parameter, expected);
}

TEST_CASE("Default initial for causality = calculatedParameter") {
    fmi3_initial_enu_t expected[N_VARIABILITIES] = {
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_calculated,
        fmi3_initial_enu_calculated,
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_unknown
    };
    validate_initial(fmi3_causality_enu_calculated_parameter, expected);
}

TEST_CASE("Default initial for causality = input") {
    fmi3_initial_enu_t expected[N_VARIABILITIES] = {
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_exact,
        fmi3_initial_enu_exact,
        fmi3_initial_enu_unknown
    };
    validate_initial(fmi3_causality_enu_input, expected);
}

TEST_CASE("Default initial for causality = output") {
    fmi3_initial_enu_t expected[N_VARIABILITIES] = {
        fmi3_initial_enu_exact,
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_calculated,
        fmi3_initial_enu_calculated,
        fmi3_initial_enu_unknown,
    };
    validate_initial(fmi3_causality_enu_output, expected);
}

TEST_CASE("Default initial for causality = local") {
    fmi3_initial_enu_t expected[N_VARIABILITIES] = {
        fmi3_initial_enu_exact,
        fmi3_initial_enu_calculated,
        fmi3_initial_enu_calculated,
        fmi3_initial_enu_calculated,
        fmi3_initial_enu_calculated,
        fmi3_initial_enu_unknown,
    };
    return validate_initial(fmi3_causality_enu_local, expected);
}

TEST_CASE("Default initial for causality = independent") {
    fmi3_initial_enu_t expected[N_VARIABILITIES] = {
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_unknown
    };
    validate_initial(fmi3_causality_enu_independent, expected);
}

TEST_CASE("Default initial for causality = unknown") {
    fmi3_initial_enu_t expected[N_VARIABILITIES] = {
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_unknown
    };
    validate_initial(fmi3_causality_enu_unknown, expected);
}

static void validate_variability_causality(fmi3_causality_enu_t c, int expected[]) {
    REQUIRE(expected[0] == fmi3_is_valid_variability_causality(fmi3_variability_enu_constant,   c));
    REQUIRE(expected[1] == fmi3_is_valid_variability_causality(fmi3_variability_enu_fixed,      c));
    REQUIRE(expected[2] == fmi3_is_valid_variability_causality(fmi3_variability_enu_tunable,    c));
    REQUIRE(expected[3] == fmi3_is_valid_variability_causality(fmi3_variability_enu_discrete,   c));
    REQUIRE(expected[4] == fmi3_is_valid_variability_causality(fmi3_variability_enu_continuous, c));
    REQUIRE(expected[5] == fmi3_is_valid_variability_causality(fmi3_variability_enu_unknown,    c));
}

TEST_CASE("Default valid combinations for causality = structuralParameter") {
    int expected[N_VARIABILITIES] = {0, 1, 1, 0, 0, 0};
    validate_variability_causality(fmi3_causality_enu_structural_parameter, expected);
}

TEST_CASE("Default valid combinations for causality = parameter") {
    int expected[N_VARIABILITIES] = {0, 1, 1, 0, 0, 0};
    validate_variability_causality(fmi3_causality_enu_parameter, expected);
}

TEST_CASE("Default valid combinations for causality = calculatedParameter") {
    int expected[N_VARIABILITIES] = {0, 1, 1, 0, 0, 0};
    validate_variability_causality(fmi3_causality_enu_calculated_parameter, expected);
}

TEST_CASE("Default valid combinations for causality = input") {
    int expected[N_VARIABILITIES] = {0, 0, 0, 1, 1, 0};
    validate_variability_causality(fmi3_causality_enu_input, expected);
}

TEST_CASE("Default valid combinations for causality = output") {
    int expected[N_VARIABILITIES] = {1, 0, 0, 1, 1, 0};
    validate_variability_causality(fmi3_causality_enu_output, expected);
}

TEST_CASE("Default valid combinations for causality = local") {
    int expected[N_VARIABILITIES] = {1, 1, 1, 1, 1, 0};
    validate_variability_causality(fmi3_causality_enu_local, expected);
}

TEST_CASE("Default valid combinations for causality = independent") {
    int expected[N_VARIABILITIES] = {0, 0, 0, 0, 1, 0};
    validate_variability_causality(fmi3_causality_enu_independent, expected);
}

TEST_CASE("Default valid combinations for causality = unknown") {
    int expected[N_VARIABILITIES] = {0, 0, 0, 0, 0, 0};
    validate_variability_causality(fmi3_causality_enu_unknown, expected);
}

#define N_VALID_VCI (12)
#define IDX_CAUSALITY   (0)
#define IDX_VARIABILITY (1)
#define IDX_INITIAL     (2)

// All non-default valid causality-variability-initial combinations
int g_nonDefaultValidVCI[N_VALID_VCI][3] = {
    // causality                              variability                      initial
    {fmi3_causality_enu_calculated_parameter, fmi3_variability_enu_fixed,      fmi3_initial_enu_approx},
    {fmi3_causality_enu_calculated_parameter, fmi3_variability_enu_tunable,    fmi3_initial_enu_approx},

    {fmi3_causality_enu_output,               fmi3_variability_enu_discrete,   fmi3_initial_enu_exact},
    {fmi3_causality_enu_output,               fmi3_variability_enu_discrete,   fmi3_initial_enu_approx},
    {fmi3_causality_enu_output,               fmi3_variability_enu_continuous, fmi3_initial_enu_exact},
    {fmi3_causality_enu_output,               fmi3_variability_enu_continuous, fmi3_initial_enu_approx},

    {fmi3_causality_enu_local,                fmi3_variability_enu_fixed,      fmi3_initial_enu_approx},
    {fmi3_causality_enu_local,                fmi3_variability_enu_tunable,    fmi3_initial_enu_approx},
    {fmi3_causality_enu_local,                fmi3_variability_enu_discrete,   fmi3_initial_enu_exact},
    {fmi3_causality_enu_local,                fmi3_variability_enu_discrete,   fmi3_initial_enu_approx},
    {fmi3_causality_enu_local,                fmi3_variability_enu_continuous, fmi3_initial_enu_exact},
    {fmi3_causality_enu_local,                fmi3_variability_enu_continuous, fmi3_initial_enu_approx},
};

static bool is_non_default_valid_combination(fmi3_variability_enu_t v, fmi3_causality_enu_t c, fmi3_initial_enu_t init) {
    // check if v, c, init combination is in g_nonDefaultValidVCI
    for (int i = 0; i < N_VALID_VCI; i++) {
        if (   (c  == g_nonDefaultValidVCI[i][IDX_CAUSALITY]) 
            && (v    == g_nonDefaultValidVCI[i][IDX_VARIABILITY])
            && (init == g_nonDefaultValidVCI[i][IDX_INITIAL])
            ) 
        {
            return true;
        }
    }
    return false;
}

static void test_valid_vci(fmi3_variability_enu_t v, fmi3_causality_enu_t c, fmi3_initial_enu_t defaultInitial, fmi3_initial_enu_t testInitial) {
    fmi3_initial_enu_t res = fmi3_get_valid_initial(v, c, testInitial); // res = testInitial if valid
    if (defaultInitial == fmi3_initial_enu_unknown) { // Unknown = no valid initials
        REQUIRE(res == fmi3_initial_enu_unknown);
    } else { // there are valid initials 
        // valid initials: default + g_nonDefaultValidVCI array combinations
        // Correct defaults are already tested separately
        bool valid = (res == defaultInitial) || is_non_default_valid_combination(v, c, testInitial);
        REQUIRE(valid == true);
    }
}

static void test_valid_vci_cv_fixed(fmi3_variability_enu_t v, fmi3_causality_enu_t c) {
    fmi3_initial_enu_t defaultInitial = fmi3_get_default_initial(v, c);
    test_valid_vci(v, c, defaultInitial, fmi3_initial_enu_exact);
    test_valid_vci(v, c, defaultInitial, fmi3_initial_enu_approx);
    test_valid_vci(v, c, defaultInitial, fmi3_initial_enu_calculated);
}

static void test_valid_vci_v_fixed(fmi3_variability_enu_t v) {
    test_valid_vci_cv_fixed(v, fmi3_causality_enu_parameter);
    test_valid_vci_cv_fixed(v, fmi3_causality_enu_calculated_parameter);
    test_valid_vci_cv_fixed(v, fmi3_causality_enu_input);
    test_valid_vci_cv_fixed(v, fmi3_causality_enu_output);
    test_valid_vci_cv_fixed(v, fmi3_causality_enu_local);
    test_valid_vci_cv_fixed(v, fmi3_causality_enu_independent);
    test_valid_vci_cv_fixed(v, fmi3_causality_enu_structural_parameter);
}

TEST_CASE("Test all causality - variability - intial combinations for validity") {
    test_valid_vci_v_fixed(fmi3_variability_enu_constant);
    test_valid_vci_v_fixed(fmi3_variability_enu_fixed);
    test_valid_vci_v_fixed(fmi3_variability_enu_tunable);
    test_valid_vci_v_fixed(fmi3_variability_enu_discrete);
    test_valid_vci_v_fixed(fmi3_variability_enu_continuous);
    test_valid_vci_v_fixed(fmi3_variability_enu_unknown);
}

TEST_CASE("Error check bad variability causality combination fallback") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variability_causality_initial/invalid/bad_variability_causality_fallback";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu != nullptr);

    const char* expectedError;
    fmi3_import_variable_t* v;

    // check Float32 type variables
    expectedError = "Invalid combination of variability 'continuous' and "
        "causality 'parameter' for variable 'f32_parameter'. Setting variability to 'fixed'";
    fmi3_testutil_log_contains(tfmu, expectedError);
    v = fmi3_import_get_variable_by_vr(tfmu->fmu, 0);
    REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_fixed);

    expectedError = "Invalid combination of variability 'fixed' and "
        "causality 'output' for variable 'f32_non_parameter'. Setting variability to 'continuous'";
    fmi3_testutil_log_contains(tfmu, expectedError);
    v = fmi3_import_get_variable_by_vr(tfmu->fmu, 1);
    REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_continuous);

    // check Float64 type variables
    expectedError = "Invalid combination of variability 'continuous' and "
        "causality 'parameter' for variable 'f64_parameter'. Setting variability to 'fixed'";
    fmi3_testutil_log_contains(tfmu, expectedError);
    v = fmi3_import_get_variable_by_vr(tfmu->fmu, 10);
    REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_fixed);

    expectedError = "Invalid combination of variability 'fixed' and "
        "causality 'output' for variable 'f64_non_parameter'. Setting variability to 'continuous'";
    fmi3_testutil_log_contains(tfmu, expectedError);
    v = fmi3_import_get_variable_by_vr(tfmu->fmu, 11);
    REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_continuous);

    // check non-Float type variables
    expectedError = "Invalid combination of variability 'continuous' and "
        "causality 'parameter' for variable 'i64_parameter'. Setting variability to 'fixed'";
    fmi3_testutil_log_contains(tfmu, expectedError);
    v = fmi3_import_get_variable_by_vr(tfmu->fmu, 20);
    REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_fixed);

    expectedError = "Invalid combination of variability 'fixed' and "
        "causality 'output' for variable 'i64_non_parameter'. Setting variability to 'discrete'";
    fmi3_testutil_log_contains(tfmu, expectedError);
    v = fmi3_import_get_variable_by_vr(tfmu->fmu, 21);
    REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_discrete);

    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Test default variabilities") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variability_causality_initial/valid/default_variabilities";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    REQUIRE(tfmu->fmu != nullptr);

    fmi3_import_variable_t* v;

    SECTION("Float64") {
        v = fmi3_import_get_variable_by_vr(tfmu->fmu, 1);
        REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_fixed);

        v = fmi3_import_get_variable_by_vr(tfmu->fmu, 2);
        REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_fixed);

        v = fmi3_import_get_variable_by_vr(tfmu->fmu, 3);
        REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_fixed);

        v = fmi3_import_get_variable_by_vr(tfmu->fmu, 4);
        REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_continuous);

        v = fmi3_import_get_variable_by_vr(tfmu->fmu, 5);
        REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_continuous);

        v = fmi3_import_get_variable_by_vr(tfmu->fmu, 6);
        REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_continuous);

        v = fmi3_import_get_variable_by_vr(tfmu->fmu, 7);
        REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_continuous);
    }

    SECTION("Int64") {
        v = fmi3_import_get_variable_by_vr(tfmu->fmu, 11);
        REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_fixed);

        v = fmi3_import_get_variable_by_vr(tfmu->fmu, 12);
        REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_fixed);

        v = fmi3_import_get_variable_by_vr(tfmu->fmu, 13);
        REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_fixed);

        v = fmi3_import_get_variable_by_vr(tfmu->fmu, 14);
        REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_discrete);

        v = fmi3_import_get_variable_by_vr(tfmu->fmu, 15);
        REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_discrete);

        v = fmi3_import_get_variable_by_vr(tfmu->fmu, 16);
        REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_discrete);

        v = fmi3_import_get_variable_by_vr(tfmu->fmu, 17);
        REQUIRE(fmi3_import_get_variable_variability(v) == fmi3_variability_enu_continuous);
    }
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("Test missing start values") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variability_causality_initial/invalid/missing_start_values";
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    REQUIRE(tfmu != nullptr);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Variable 'f64_input': start value required for input variables"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Variable 'f64_parameter': start value required for parameter variables"));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Variable 'f64_structural_parameter': start value required for structuralParameter variables"));

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Variable 'f64_constant': start value required for variables with constant variability"));

    REQUIRE(fmi3_testutil_log_contains(tfmu, "Variable 'f64_exact': start value required for variables with initial == \"exact\""));
    REQUIRE(fmi3_testutil_log_contains(tfmu, "Variable 'f64_approx': start value required for variables with initial == \"approx\""));

    fmi3_testutil_import_free(tfmu);
}
