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

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#include "catch.hpp"

#define BUFFER 1000

void do_exit(int code)
{
    printf("Press 'Enter' to exit\n");
    /* getchar(); */
    exit(code);
}

typedef struct {
    fmi3_boolean_t discreteStatesNeedUpdate;
    fmi3_boolean_t terminateSimulation;
    fmi3_boolean_t nominalsOfContinuousStatesChanged;
    fmi3_boolean_t valuesOfContinuousStatesChanged;
    fmi3_boolean_t nextEventTimeDefined;
    fmi3_float64_t nextEventTime;
} test_event_info_t;

void do_event_iteration(fmi3_import_t *fmu, test_event_info_t* eventInfo)
{
    eventInfo->discreteStatesNeedUpdate = fmi3_true;
    eventInfo->terminateSimulation     = fmi3_false;
    while (eventInfo->discreteStatesNeedUpdate && !eventInfo->terminateSimulation) {
        fmi3_import_update_discrete_states(
                fmu,
                &eventInfo->discreteStatesNeedUpdate,
                &eventInfo->terminateSimulation,
                &eventInfo->nominalsOfContinuousStatesChanged,
                &eventInfo->valuesOfContinuousStatesChanged,
                &eventInfo->nextEventTimeDefined,
                &eventInfo->nextEventTime);
    }
}

/* All ME common + ME specific functions must always exist, this function tests
 * what is not already being called in the simulation test function. */
void test_capi_wrappers_me(fmi3_import_t* fmu)
{
    jm_status_enu_t jmstatus;
    fmi3_status_t fmistatus;
    size_t n_states_xml;
    size_t n_event_indicators_xml;
    size_t n_states_capi;
    size_t n_event_indicators_capi;

    /* free instance if possible so we can verify that number of continuous
     * states are taken from XML in the first call */
    fmi3_import_free_instance(fmu);

    fmi3_import_get_number_of_continuous_states(fmu, &n_states_xml);
    fmi3_import_get_number_of_event_indicators(fmu, &n_event_indicators_xml);

    jmstatus = fmi3_import_instantiate_model_exchange(
        fmu,
        "Test ME model instance",
        NULL,
        fmi3_false,
        fmi3_false,
        NULL,
        NULL
    );
    if (jmstatus == jm_status_error) {
        printf("fmi3_import_instantiate_model_exchange failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    /* verify that XML and CAPI give same result */
    fmi3_import_get_number_of_continuous_states(fmu, &n_states_capi);
    if (n_states_xml != n_states_capi) {
        printf("fmi3_import_get_number_of_continuous_states: capi and xml variants gave different results\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    fmi3_import_get_number_of_event_indicators(fmu, &n_event_indicators_capi);
    if (n_event_indicators_xml != n_event_indicators_capi) {
        printf("fmi3_import_get_number_of_event_indicators: capi and xml variants gave different results\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    /* test that get_nominals API is loaded */
    fmistatus = fmi3_import_get_nominals_of_continuous_states(fmu, NULL, 0);
    if (fmistatus == fmi3_status_error) {
        printf("fmi3_import_get_nominals_of_continuous_states failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    fmistatus = fmi3_import_terminate(fmu);
    if (fmistatus != fmi3_status_ok) {
        printf("fmi3_import_terminate failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    fmi3_import_free_instance(fmu);
}

int test_simulate_me(fmi3_import_t* fmu)
{
    fmi3_status_t fmistatus;
    jm_status_enu_t jmstatus;
    fmi3_float64_t tstart = 0.0;
    fmi3_float64_t tcur;
    fmi3_float64_t hcur;
    fmi3_float64_t hdef = 0.1;
    fmi3_float64_t tend = 2.0;
    size_t n_states;
    size_t n_event_indicators;
    fmi3_float64_t* states;
    fmi3_float64_t states_end_results[] = {0.362000, -3.962000};
    fmi3_float64_t* states_der;
    fmi3_float64_t* event_indicators;
    fmi3_float64_t* event_indicators_prev;
    fmi3_int32_t* roots_found; /* any root found = state_event = zero_crossing_event */
    fmi3_boolean_t terminateSimulation = fmi3_false;
    fmi3_boolean_t toleranceControlled = fmi3_true;
    fmi3_float64_t relativeTolerance = 0.001;
    test_event_info_t eventInfo;
    fmi3_boolean_t step_event = fmi3_false;
    fmi3_boolean_t time_event = fmi3_false;
    size_t k;

    printf("Version returned from FMU:   %s\n", fmi3_import_get_version(fmu));
    fmi3_import_get_number_of_continuous_states(fmu, &n_states);
    fmi3_import_get_number_of_event_indicators(fmu, &n_event_indicators);

    if (sizeof(states_end_results)/sizeof(fmi3_float64_t) != n_states) {
        printf("Number of states and results have different length n_states = %u n_results = %u\n", (unsigned)n_states, (unsigned)sizeof(states_end_results));
        do_exit(CTEST_RETURN_FAIL);
    }

    states = (fmi3_float64_t*)calloc(n_states, sizeof(double));
    states_der = (fmi3_float64_t*)calloc(n_states, sizeof(double));
    event_indicators = (fmi3_float64_t*)calloc(n_event_indicators, sizeof(double));
    event_indicators_prev = (fmi3_float64_t*)calloc(n_event_indicators, sizeof(double));
    roots_found = (fmi3_int32_t*)calloc(n_event_indicators, sizeof(fmi3_int32_t));

    jmstatus = fmi3_import_instantiate_model_exchange(
        fmu,
        "Test ME model instance",
        NULL,
        fmi3_false,
        fmi3_false,
        NULL,
        NULL
    );
    if (jmstatus == jm_status_error) {
        printf("fmi3_import_instantiate_model_exchange failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    fmistatus = fmi3_import_set_debug_logging(fmu, fmi3_false,0,0);
    printf("fmi3_import_set_debug_logging:  %s\n", fmi3_status_to_string(fmistatus));
    fmi3_import_set_debug_logging(fmu, fmi3_true, 0, 0);

    fmistatus = fmi3_import_enter_initialization_mode(fmu, toleranceControlled,
        relativeTolerance, tstart, fmi3_false, 0.0);
    fmistatus = fmi3_import_exit_initialization_mode(fmu);

    tcur = tstart;
    hcur = hdef;
    step_event = fmi3_false;

    eventInfo.discreteStatesNeedUpdate          = fmi3_false;
    eventInfo.terminateSimulation               = fmi3_false;
    eventInfo.nominalsOfContinuousStatesChanged = fmi3_false;
    eventInfo.valuesOfContinuousStatesChanged   = fmi3_true;
    eventInfo.nextEventTimeDefined              = fmi3_false;
    eventInfo.nextEventTime                     = -0.0;

    /* fmiExitInitializationMode leaves FMU in event mode */
    do_event_iteration(fmu, &eventInfo);
    fmi3_import_enter_continuous_time_mode(fmu);

    fmistatus = fmi3_import_get_continuous_states(fmu, states, n_states);
    fmistatus = fmi3_import_get_event_indicators(fmu, event_indicators, n_event_indicators);

    while ((tcur < tend) && (!(eventInfo.terminateSimulation || terminateSimulation))) {
        size_t k;
        fmi3_float64_t tlast;
        int zero_crossing_event = 0;

        fmistatus = fmi3_import_set_time(fmu, tcur);

        { /* Swap event_indicators and event_indicators_prev so that we can get new indicators */
            fmi3_float64_t *temp = event_indicators;
            event_indicators = event_indicators_prev;
            event_indicators_prev = temp;
        }
        fmistatus = fmi3_import_get_event_indicators(fmu, event_indicators, n_event_indicators);

        /* Check if an event indicator has triggered, and get direction of event indicator */
        for (k = 0; k < n_event_indicators; k++) {
            if ((event_indicators[k] >= 0) && (event_indicators_prev[k] < 0)) {
                zero_crossing_event = 1;
                roots_found[k] = 1;
            } else if ((event_indicators[k] <= 0) && (event_indicators_prev[k] > 0)) {
                zero_crossing_event = 1;
                roots_found[k] = -1;
            } else {
                roots_found[k] = 0;
            }
        }

        /* Handle any events */
        time_event = eventInfo.nextEventTimeDefined && tcur == eventInfo.nextEventTime;
        if (step_event || zero_crossing_event || time_event) {
            fmistatus = fmi3_import_enter_event_mode(fmu);
            do_event_iteration(fmu, &eventInfo);
            fmistatus = fmi3_import_enter_continuous_time_mode(fmu);

            fmistatus = fmi3_import_get_continuous_states(fmu, states, n_states);
            fmistatus = fmi3_import_get_event_indicators(fmu, event_indicators, n_event_indicators);
        }

        /* Calculate next time step */
        tlast = tcur;
        tcur += hdef;
        if (eventInfo.nextEventTimeDefined && (tcur >= eventInfo.nextEventTime)) {
            tcur = eventInfo.nextEventTime;
        }
        hcur = tcur - tlast;
        if(tcur > tend - hcur/1e16) {
            tcur = tend;
            hcur = tcur - tlast;
        }

        /* Integrate a step */
        fmistatus = fmi3_import_get_derivatives(fmu, states_der, n_states);
        for (k = 0; k < n_states; k++) {
            states[k] = states[k] + hcur*states_der[k];
            if (k == 0) printf("Ball height state[%u] = %f\n", (unsigned)k, states[k]);
        }

        /* Set states */
        fmistatus = fmi3_import_set_continuous_states(fmu, states, n_states);
        /* Step is complete */
        fmistatus = fmi3_import_completed_integrator_step(fmu, fmi3_true, &step_event,
                                                          &terminateSimulation);
    }

    /* Validate result */
    for (k = 0; k < n_states; k++) {
        fmi3_float64_t diff = states[k] - states_end_results[k];
        diff = diff > 0 ? diff: -diff; /* Take abs */
        if (diff > 1e-10) {
            printf("Simulation results is wrong  states[%u] %f != %f, |res| = %f\n", (unsigned)k, states[k], states_end_results[k], diff);
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
        fmi3_float64_t ref_res[] = { states_end_results[0], states_end_results[1], states_end_results[1], -9.81 };

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

    free(states);
    free(states_der);
    free(event_indicators);
    free(event_indicators_prev);
    free(roots_found);

    return 0;
}

TEST_CASE("main") {
    const char* tmpPath;
    jm_callbacks callbacks;
    fmi_import_context_t* context;
    fmi_version_enu_t version;
    jm_status_enu_t status;

    fmi3_import_t* fmu;
    const char* FMUPath = FMU3_ME_PATH;

    callbacks.malloc = malloc;
    callbacks.calloc = calloc;
    callbacks.realloc = realloc;
    callbacks.free = free;
    callbacks.logger = jm_default_logger;
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
    version = fmi_import_get_fmi_version(context, FMU3_ME_PATH, tmpPath);
    if(version != fmi_version_3_0_enu) {
        printf("Only version 3.0 is supported by this code\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    fmu = fmi3_import_parse_xml(context, tmpPath, 0);
    if(!fmu) {
        printf("Error parsing XML, exiting\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    if (!(fmi3_fmu_kind_me & fmi3_import_get_fmu_kind(fmu))) { /* compare bitmasks */
        printf("Only ME 3.0 is supported by this code\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    /* Test default callbacks - .._cs_test tests custom */
    status = fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_me, NULL, NULL);
    if (status == jm_status_error) {
        printf("Could not create the DLL loading mechanism(C-API test).\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    test_capi_wrappers_me(fmu);
    test_simulate_me(fmu);

    fmi3_import_destroy_dllfmu(fmu);
    fmi3_import_free(fmu);
    fmi_import_free_context(context);
    if (fmi_import_rmdir(&callbacks, tmpPath)) {
        printf("Problem when deleting FMU unpack directory.\n");
        do_exit(CTEST_RETURN_FAIL);
    }
    callbacks.free((void*)tmpPath);

    printf("Everything seems to be OK since you got this far=)!\n");
}