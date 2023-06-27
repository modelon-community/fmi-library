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
#include <stdarg.h>

#include "config_test.h"

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"
#include <JM/jm_portability.h>

#include "catch.hpp"

#define BUFFER 1000

void importlogger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message)
{
        printf("module = %s, log level = %s: %s\n", module, jm_log_level_to_string(log_level), message);
}

/* function implementing fmi3_intermediate_update_callback_ft */
fmi3_status_t dummy_intermediate_update_callback(
        fmi3_instance_environment_t instanceEnvironment,
        fmi3_float64_t intermediateUpdateTime,
        fmi3_boolean_t intermediateVariableSetRequested,
        fmi3_boolean_t intermediateVariableGetAllowed,
        fmi3_boolean_t intermediateStepFinished,
        fmi3_boolean_t canReturnEarly,
        fmi3_boolean_t* earlyReturnRequested,
        fmi3_float64_t* earlyReturnTime)
{
    return fmi3_status_ok;
}

/* just check that it's possible to call the dependency functions */
void call_dependency_functions(fmi3_import_t* fmu) {
    fmi3_value_reference_t dependent = 1; /* arbitrarily chosen */
    size_t nDeps;

    REQUIRE(fmi3_import_get_number_of_variable_dependencies(fmu, 1, &nDeps) == fmi3_status_ok); // populates nDeps

    size_t* elementIndicesOfDependent = (size_t*)malloc(nDeps * sizeof(size_t));
    fmi3_value_reference_t* independents =(fmi3_value_reference_t*)malloc(nDeps * sizeof(fmi3_value_reference_t));
    size_t* elementIndicesOfIndependents = (size_t*)malloc(nDeps * sizeof(size_t));
    fmi3_dependency_kind_t* dependencyKinds = (fmi3_dependency_kind_t*)malloc(nDeps * sizeof(fmi3_dependency_kind_t));

    // populates elementIndicesOfDependent, independents, elementIndicesOfIndependents, dependencyKinds
    REQUIRE(fmi3_import_get_variable_dependencies(fmu, dependent, elementIndicesOfDependent, independents,
            elementIndicesOfIndependents, dependencyKinds, nDeps) == fmi3_status_ok);

    REQUIRE(elementIndicesOfDependent != nullptr);
    REQUIRE(independents != nullptr);
    REQUIRE(elementIndicesOfIndependents != nullptr);
    REQUIRE(dependencyKinds != nullptr);

    free(elementIndicesOfDependent);
    free(independents);
    free(elementIndicesOfIndependents);
    free(dependencyKinds);
}

void test_simulate_cs(fmi3_import_t* fmu) {
    fmi3_string_t instanceName = "Test CS model instance";
    fmi3_string_t fmuInstantiationToken;
    fmi3_string_t resourcePath = "";
    fmi3_boolean_t visible = fmi3_false;
    fmi3_boolean_t loggingOn = fmi3_false;
    fmi3_boolean_t eventModeUsed = fmi3_false;
    fmi3_boolean_t earlyReturnAllowed = fmi3_false;
    const fmi3_value_reference_t requiredIntermediateVariables[] = {0};
    size_t nRequiredIntermediateVariables = 0;

    /* DoStep output args */
    fmi3_boolean_t eventHandlingNeeded;
    fmi3_boolean_t terminate;
    fmi3_boolean_t earlyReturn;
    fmi3_float64_t lastSuccessfulTime;

    fmi3_float64_t relativeTol = 1e-4;

    fmi3_float64_t simulation_results[2] = { 0.0143633,   -1.62417 };
    fmi3_value_reference_t compare_real_variables_vr[2] = { 0, 1 };

    fmi3_float64_t tstart = 0.0;
    fmi3_float64_t tcur = tstart;
    fmi3_float64_t hstep = 0.1;
    fmi3_float64_t tend = 2.0;
    fmi3_boolean_t StopTimeDefined = fmi3_false;

    REQUIRE(sizeof(compare_real_variables_vr) / sizeof(fmi3_value_reference_t) == sizeof(simulation_results) / sizeof(fmi3_float64_t));

    INFO("Instantiation");
    REQUIRE_STREQ(fmi3_import_get_version(fmu), "3.0");
    REQUIRE_STREQ(fmi3_import_get_instantiation_token(fmu), "123");

    jm_status_enu_t jmstatus = fmi3_import_instantiate_co_simulation(
            fmu,
            instanceName,
            resourcePath,
            visible,
            loggingOn,
            eventModeUsed,
            earlyReturnAllowed,
            requiredIntermediateVariables,
            nRequiredIntermediateVariables,
            NULL,
            NULL,
            NULL);

    REQUIRE(jmstatus == jm_status_success);

    INFO("Initialization");
    REQUIRE(fmi3_import_enter_initialization_mode(fmu, fmi3_true,
        relativeTol, tstart, StopTimeDefined, tend) == fmi3_status_ok);
    REQUIRE(fmi3_import_exit_initialization_mode(fmu) == fmi3_status_ok);

    INFO("Testing dependecies");
    call_dependency_functions(fmu);

    INFO("Simulation");
    fmi3_float64_t rvalue;
    tcur = tstart;
    while (tcur < tend) {
        INFO("tcur = " << tcur);
        fmi3_boolean_t newStep = fmi3_true; /* noSetFMUStatePriorToCurrentPoint */
        /* The test FMU behaves "normally" for now, so not checking the new FMI 3 output args */
        REQUIRE(fmi3_import_do_step(fmu, tcur, hstep, newStep, &eventHandlingNeeded, &terminate, &earlyReturn, &lastSuccessfulTime) == fmi3_status_ok);
        
        for (size_t k = 0; k < sizeof(compare_real_variables_vr)/sizeof(fmi3_value_reference_t); k++) {
            INFO("k = " << k);
            fmi3_value_reference_t vr = compare_real_variables_vr[k];
            REQUIRE(fmi3_import_get_float64(fmu, &vr, 1, &rvalue, 1) == fmi3_status_ok);
        }
        const size_t nValue = 2;
        fmi3_float64_t val[nValue];
        REQUIRE(fmi3_import_get_float64(fmu, compare_real_variables_vr, nValue, val, nValue) == fmi3_status_ok);

        tcur += hstep;
    }
    // Simulation finished

    INFO("Validate scalar variables result");
    for (size_t k = 0; k < sizeof(compare_real_variables_vr)/sizeof(fmi3_value_reference_t); k++) {
        INFO("k = " << k);
        fmi3_value_reference_t vr = compare_real_variables_vr[k];
        fmi3_float64_t rvalue;
        REQUIRE(fmi3_import_get_float64(fmu, &vr, 1, &rvalue, 1) == fmi3_status_ok);
        fmi3_float64_t res = rvalue - simulation_results[k]; // error
        res = res > 0 ? res: -res; // abs
        REQUIRE(res <= 3e-3);
    }

    INFO("Validate array variables result");
    {
        fmi3_value_reference_t vr = 12;
        const size_t nValue = 4;
        fmi3_float64_t arrValues[nValue];
        fmi3_float64_t diff;
        fmi3_float64_t tol = 3e-3; /* absolute tolerance */
        fmi3_float64_t ref_res[] = {simulation_results[0], simulation_results[1], simulation_results[1], -9.81};

        /* check result */
        REQUIRE(fmi3_import_get_float64(fmu, &vr, 1, (fmi3_float64_t*)&arrValues, nValue) == fmi3_status_ok);
        for (size_t k = 0; k < nValue; k++) {
            INFO("k = " << k);
            diff = ref_res[k] - arrValues[k]; // error
            diff = diff > 0 ? diff : -diff; // abs
            REQUIRE(diff <= tol);
        }
    }

    INFO("Terminate and free instance");
    REQUIRE(fmi3_import_terminate(fmu) == fmi3_status_ok);
    fmi3_import_free_instance(fmu);
}

/*****************************************
 *** instanceEnvironment and callbacks ***
 *****************************************/

/* Structure and callbacks for counting number of allocs/frees, so we can verify that they are equal in number */

typedef struct {
    fmi3_import_t* fmu; /* fmi3_log_forwarding is based on the fmu instance */
} fmi3_inst_env_count_calls_t;

void fmi3_test_log_forwarding_wrap(fmi3_instance_environment_t instEnv, fmi3_status_t status,
        fmi3_string_t category, fmi3_string_t message) {
    fmi3_log_forwarding(((fmi3_inst_env_count_calls_t*)instEnv)->fmu, status, category, message);
}

TEST_CASE("Co-Simulation FMU example") {
    fmi3_inst_env_count_calls_t instEnv;
    jm_callbacks callbacks;

    callbacks.malloc = malloc;
    callbacks.calloc = calloc;
    callbacks.realloc = realloc;
    callbacks.free = free;
    callbacks.logger = importlogger;
    callbacks.log_level = jm_log_level_debug;
    callbacks.context = 0;

#ifdef FMILIB_GENERATE_BUILD_STAMP
    printf("Library build stamp:\n%s\n", fmilib_get_build_stamp());
#endif

    const char* tmpPath = fmi_import_mk_temp_dir(&callbacks, FMU_UNPACK_DIR, NULL);
    REQUIRE(tmpPath != nullptr);

    fmi_import_context_t* context = fmi_import_allocate_context(&callbacks);
    REQUIRE(fmi_import_get_fmi_version(context, FMU3_CS_PATH, tmpPath) == fmi_version_3_0_enu);

    fmi3_import_t* fmu = fmi3_import_parse_xml(context, tmpPath, 0);
    REQUIRE(fmu != nullptr);
    REQUIRE((fmi3_import_get_fmu_kind(fmu) & fmi3_fmu_kind_cs) == fmi3_fmu_kind_cs); // is CS FMU

    /* Test custom callbacks - .._me_test tests default */
    instEnv.fmu = fmu;

    REQUIRE(fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_cs, &instEnv, fmi3_test_log_forwarding_wrap) == jm_status_success);

    test_simulate_cs(fmu);

    fmi3_import_destroy_dllfmu(fmu);
    fmi3_import_free(fmu);
    fmi_import_free_context(context);
    REQUIRE(fmi_import_rmdir(&callbacks, tmpPath) == jm_status_success);
    callbacks.free((void*)tmpPath);
}
