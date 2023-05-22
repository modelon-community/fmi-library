/*
    Copyright (C) 2012-2023 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "config_test.h"

#include <fmilib.h>
#include <JM/jm_portability.h>


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
    return fmi3_status_ok; /* NYI */
}

void do_exit(int code)
{
    printf("Press 'Enter' to exit\n");
    /* getchar(); */
    exit(code);
}

/* just check that it's possible to call the dependency functions */
int call_dependency_functions(fmi3_import_t* fmu)
{
    fmi3_status_t fmistatus;
    fmi3_value_reference_t dependent = 1; /* arbitrarily chosen */
    size_t* elementIndicesOfDependent;
    fmi3_value_reference_t* independents;
    size_t* elementIndicesOfIndependents;
    fmi3_dependency_kind_t* dependencyKinds;
    size_t nDeps;

    fmistatus = fmi3_import_get_number_of_variable_dependencies(fmu, 1, &nDeps);
    if (fmistatus != fmi3_status_ok) {
        printf("fmi3_import_get_number_of_variable_dependencies failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    elementIndicesOfDependent = malloc(nDeps * sizeof(size_t));
    independents = malloc(nDeps * sizeof(fmi3_value_reference_t));
    elementIndicesOfIndependents = malloc(nDeps * sizeof(size_t));
    dependencyKinds = malloc(nDeps * sizeof(fmi3_dependency_kind_t));

    fmistatus = fmi3_import_get_variable_dependencies(fmu, dependent, elementIndicesOfDependent, independents,
            elementIndicesOfIndependents, dependencyKinds, nDeps);
    if (fmistatus != fmi3_status_ok) {
        printf("fmi3_import_get_number_of_variable_dependencies failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    /* note: ignoring cleanup on test failure to keep it simple */
    free(elementIndicesOfDependent);
    free(independents);
    free(elementIndicesOfIndependents);
    free(dependencyKinds);

    return 0;
}


int test_simulate_cs(fmi3_import_t * fmu)
{
    fmi3_status_t fmistatus;
    jm_status_enu_t jmstatus;

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


    /* fmi3_float64_t simulation_results[] = {-0.001878, -1.722275}; */
    fmi3_float64_t simulation_results[] = { 0.0143633,   -1.62417 };
    fmi3_value_reference_t compare_real_variables_vr[] = { 0, 1 };
    size_t k;

    fmi3_float64_t tstart = 0.0;
    fmi3_float64_t tcur = tstart;
    fmi3_float64_t hstep = 0.1;
    fmi3_float64_t tend = 2.0;
    fmi3_boolean_t StopTimeDefined = fmi3_false;

    size_t nValues = 1;

    if (sizeof(compare_real_variables_vr) / sizeof(fmi3_value_reference_t) != sizeof(simulation_results) / sizeof(fmi3_float64_t)) {
        printf("Number of simulation values and reference values are different\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    printf("Version returned from FMU:   %s\n", fmi3_import_get_version(fmu));

    fmuInstantiationToken = fmi3_import_get_instantiation_token(fmu);
    printf("instantiationToken:      %s\n", fmuInstantiationToken);


    /* TODO */
    jmstatus = fmi3_import_instantiate_co_simulation(
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

    if (jmstatus == jm_status_error) {
        printf("fmi3_import_instantiate_co_simulation failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    fmistatus = fmi3_import_enter_initialization_mode(fmu, fmi3_true,
        relativeTol, tstart, StopTimeDefined, tend);
    if (fmistatus != fmi3_status_ok) {
        printf("fmi3_import_enter_initialization_mode failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    fmistatus = fmi3_import_exit_initialization_mode(fmu);
    if (fmistatus != fmi3_status_ok) {
        printf("fmi3_import_exit_initialization_mode failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    if (call_dependency_functions(fmu)) {
        printf("call_dependency_functions failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    tcur = tstart;
    printf("%10s %10s\n", "Ball height", "Ball speed");
    while (tcur < tend) {
        fmi3_boolean_t newStep = fmi3_true; /* noSetFMUStatePriorToCurrentPoint */
#if 0 /* Prints a real value.. */
        fmi3_float64_t rvalue;
        fmi3_value_reference_t vr = 0;

        fmistatus = fmi3_import_get_real(fmu, &vr, 1, &rvalue);
        printf("rvalue = %f\n", rvalue);
#endif
        /* The test FMU behaves "normally" for now, so not checking the new FMI 3 output args */
        fmistatus = fmi3_import_do_step(fmu, tcur, hstep, newStep, &eventHandlingNeeded, &terminate, &earlyReturn, &lastSuccessfulTime);

        for (k = 0; k < sizeof(compare_real_variables_vr)/sizeof(fmi3_value_reference_t); k++) {
            fmi3_value_reference_t vr = compare_real_variables_vr[k];
            fmi3_float64_t rvalue;
            fmistatus = fmi3_import_get_float64(fmu, &vr, 1, &rvalue, nValues);
        }
        {
            fmi3_float64_t val[2];
            fmi3_import_get_float64(fmu, compare_real_variables_vr, 2, val, nValues);
            printf("%10g %10g\n", val[0],val[1]);
        }

        tcur += hstep;
    }

    printf("Simulation finished. Checking results\n");

    /* Validate result */
    for (k = 0; k < sizeof(compare_real_variables_vr)/sizeof(fmi3_value_reference_t); k++) {
        fmi3_value_reference_t vr = compare_real_variables_vr[k];
        fmi3_float64_t rvalue;
        fmi3_float64_t res;
        fmistatus = fmi3_import_get_float64(fmu, &vr, 1, &rvalue, nValues);
        res = rvalue - simulation_results[k];
        res = res > 0 ? res: -res; /* Take abs */
        if (res > 3e-3) {
            printf("Simulation results is wrong!\n");
            printf("State [%u]  %g != %g, |res| = %g\n", (unsigned)k, rvalue, simulation_results[k], res);
            printf("\n");
            do_exit(CTEST_RETURN_FAIL);
        }
    }

    /* Validate array variable results */
    {
        fmi3_value_reference_t vr = 12;
        fmi3_float64_t arrValues[4];
        fmi3_float64_t diff;
        fmi3_float64_t tol = 3e-3; /* absolute tolerance */
        size_t nValues = sizeof(arrValues)/sizeof(fmi3_float64_t);
        fmi3_float64_t ref_res[] = {simulation_results[0], simulation_results[1], simulation_results[1], -9.81};

        /* get result */
        fmistatus = fmi3_import_get_float64(fmu, &vr, 1, (fmi3_float64_t*)&arrValues, nValues);
        if (fmistatus != fmi3_status_ok) {
            printf("error: get values for array failed\n");
            do_exit(CTEST_RETURN_FAIL);
        }

        /* check result */
        for (k = 0; k < nValues; k++) {
            diff = ref_res[k] - arrValues[k];
            diff = diff > 0 ? diff : -diff;
            if (diff > tol) {
                printf("error: incorrect final result for array idx: '%zu', exp: '%f', act: '%f', diff: '%f', tol (abs.): '%f'\n", k, ref_res[k], arrValues[k], diff, tol);
                do_exit(CTEST_RETURN_FAIL);
            }
        }
    }

    fmistatus = fmi3_import_terminate(fmu);

    fmi3_import_free_instance(fmu);

    return 0;
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

/************
 *** main ***
 ************/

int main(int argc, char *argv[])
{
    fmi3_inst_env_count_calls_t instEnv;
    const char* tmpPath;
    jm_callbacks callbacks;
    fmi_import_context_t* context;
    fmi_version_enu_t version;
    jm_status_enu_t status;

    fmi3_import_t* fmu;

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

    tmpPath = fmi_import_mk_temp_dir(&callbacks, FMU_UNPACK_DIR, NULL);
    if (!tmpPath) {
        printf("Failed to create temporary directory in: " FMU_UNPACK_DIR "\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    context = fmi_import_allocate_context(&callbacks);
    version = fmi_import_get_fmi_version(context, FMU3_CS_PATH, tmpPath);
    if(version != fmi_version_3_0_enu) {
        printf("The code only supports version 3.0\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    fmu = fmi3_import_parse_xml(context, tmpPath, 0);
    if(!fmu) {
        printf("Error parsing XML, exiting\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    if (!(fmi3_import_get_fmu_kind(fmu) & fmi3_fmu_kind_cs)) {
        printf("Only CS 3.0 is supported by this code\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    /* Test custom callbacks - .._me_test tests default */
    instEnv.fmu    = fmu;

    status = fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_cs, &instEnv, fmi3_test_log_forwarding_wrap);
    if (status == jm_status_error) {
        printf("Could not create the DLL loading mechanism(C-API) (error: %s).\n", fmi3_import_get_last_error(fmu));
        do_exit(CTEST_RETURN_FAIL);
    }

    test_simulate_cs(fmu);

    fmi3_import_destroy_dllfmu(fmu);
    fmi3_import_free(fmu);
    fmi_import_free_context(context);
    if (fmi_import_rmdir(&callbacks, tmpPath)) {
        printf("Problem when deleting FMU unpack directory.\n");
        do_exit(CTEST_RETURN_FAIL);
    }
    callbacks.free((void*)tmpPath);

    printf("Everything seems to be OK since you got this far=)!\n");
    return 0;
}
