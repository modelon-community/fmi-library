#include <stdio.h>
#include <stdlib.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

// Collection of various tests on valid combinations & defaults for causality, variablity and initial

TEST_CASE("Default valid variability") {
    // constants: fixed
    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_parameter)            == fmi3_variability_enu_fixed);
    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_calculated_parameter) == fmi3_variability_enu_fixed);
    // TODO
    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_structural_parameter) == fmi3_variability_enu_continuous);
    // non-constants: continuous
    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_input)                == fmi3_variability_enu_continuous);
    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_output)               == fmi3_variability_enu_continuous);
    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_local)                == fmi3_variability_enu_continuous);
    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_independent)          == fmi3_variability_enu_continuous);
    // TOOD: Should probably be unknown?
    REQUIRE(fmi3_get_default_valid_variability(fmi3_causality_enu_unknown)              == fmi3_variability_enu_continuous);
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
        fmi3_initial_enu_unknown,
        fmi3_initial_enu_unknown,
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

TEST_CASE("Default initial for causality = structuralParameter") {
; // TODO
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

TEST_CASE("Default valid combinations for causality = structuralParameter") {
    ; // TODO
}

TEST_CASE("Default valid combinations for causality = unknown") {
    int expected[N_VARIABILITIES] = {0, 0, 0, 0, 0, 0};
    validate_variability_causality(fmi3_causality_enu_unknown, expected);
}

TEST_CASE("Error check bad variability causality combination") {
    const char* xmldir = FMI3_TEST_XML_DIR "/causality_variability_initial/invalid/variable_bad_variability_causality";

    fmi3_import_t* fmu = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(fmu != nullptr);

    // verify parsing error/warning
    const char *expectedErr = "Invalid combination of variability constant and "
    "causality input for variable 'dummy1'. Setting variability to 'continuous'";
    const char* errMsg = fmi3_import_get_last_error(fmu);
    REQUIRE(strcmp(errMsg, expectedErr) == 0);

    // check variablity has been set correctly
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_vr(fmu, 0);
    REQUIRE(fmi3_import_get_variability(v) == fmi3_variability_enu_continuous);

    fmi3_import_free(fmu);
}
