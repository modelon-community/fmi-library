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

typedef struct {
    fmi3_boolean_t discreteStatesNeedUpdate;
    fmi3_boolean_t terminateSimulation;
    fmi3_boolean_t nominalsOfContinuousStatesChanged;
    fmi3_boolean_t valuesOfContinuousStatesChanged;
    fmi3_boolean_t nextEventTimeDefined;
    fmi3_float64_t nextEventTime;
} test_event_info_t;

void do_event_iteration(fmi3_import_t* fmu, test_event_info_t* eventInfo) {
    eventInfo->discreteStatesNeedUpdate = fmi3_true;
    eventInfo->terminateSimulation      = fmi3_false;
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
void test_capi_wrappers_me(fmi3_import_t* fmu) {
    size_t n_states_xml;
    size_t n_event_indicators_xml;
    size_t n_states_capi;
    size_t n_event_indicators_capi;

    /* free instance if possible so we can verify that number of continuous
     * states are taken from XML in the first call */
    fmi3_import_free_instance(fmu);

    REQUIRE(fmi3_import_get_number_of_continuous_states(fmu, &n_states_xml) == fmi3_status_ok);
    REQUIRE(fmi3_import_get_number_of_event_indicators(fmu, &n_event_indicators_xml) == fmi3_status_ok);

    INFO("Instantiation");
    jm_status_enu_t jmstatus = fmi3_import_instantiate_model_exchange(
        fmu,
        "Test ME model instance",
        NULL,
        fmi3_false,
        fmi3_false
    );
    REQUIRE(jmstatus == jm_status_success);

    INFO("Verify that XML and CAPI give same result");
    REQUIRE(fmi3_import_get_number_of_continuous_states(fmu, &n_states_capi) == fmi3_status_ok);
    REQUIRE(n_states_capi == n_states_xml);

    REQUIRE(fmi3_import_get_number_of_event_indicators(fmu, &n_event_indicators_capi) == fmi3_status_ok);
    REQUIRE(n_event_indicators_capi == n_event_indicators_xml);

    INFO("Test that get_nominals API is loaded");
    REQUIRE(fmi3_import_get_nominals_of_continuous_states(fmu, NULL, 0) == fmi3_status_ok);

    INFO("Testing Clocks CAPI wrappers");
    REQUIRE(fmi3_import_set_clock(fmu, nullptr, 0, nullptr) == fmi3_status_ok);
    REQUIRE(fmi3_import_set_interval_decimal(fmu, nullptr, 0, nullptr) == fmi3_status_ok);
    REQUIRE(fmi3_import_set_interval_fraction(fmu, nullptr, 0, nullptr, nullptr) == fmi3_status_ok);
    REQUIRE(fmi3_import_set_shift_decimal(fmu, nullptr, 0, nullptr) == fmi3_status_ok);
    REQUIRE(fmi3_import_set_shift_fraction(fmu, nullptr, 0, nullptr, nullptr) == fmi3_status_ok);

    REQUIRE(fmi3_import_get_clock(fmu, nullptr, 0, nullptr) == fmi3_status_ok);
    REQUIRE(fmi3_import_get_interval_decimal(fmu, nullptr, 0, nullptr, nullptr) == fmi3_status_ok);
    REQUIRE(fmi3_import_get_interval_fraction(fmu, nullptr, 0, nullptr, nullptr, nullptr) == fmi3_status_ok);
    REQUIRE(fmi3_import_get_shift_decimal(fmu, nullptr, 0, nullptr) == fmi3_status_ok);
    REQUIRE(fmi3_import_get_shift_fraction(fmu, nullptr, 0, nullptr, nullptr) == fmi3_status_ok);

    REQUIRE(fmi3_import_terminate(fmu) == fmi3_status_ok);
    fmi3_import_free_instance(fmu);
}

void test_simulate_me(fmi3_import_t* fmu) {
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

    REQUIRE_STREQ(fmi3_import_get_version(fmu), "3.0");
    REQUIRE(fmi3_import_get_number_of_continuous_states(fmu, &n_states) == fmi3_status_ok); // populates n_states
    REQUIRE(fmi3_import_get_number_of_event_indicators(fmu, &n_event_indicators) == fmi3_status_ok); // populates n_event_indicators

    REQUIRE(sizeof(states_end_results)/sizeof(fmi3_float64_t) == n_states); // sanity check on reference result data size

    states = (fmi3_float64_t*)calloc(n_states, sizeof(double));
    states_der = (fmi3_float64_t*)calloc(n_states, sizeof(double));
    event_indicators = (fmi3_float64_t*)calloc(n_event_indicators, sizeof(double));
    event_indicators_prev = (fmi3_float64_t*)calloc(n_event_indicators, sizeof(double));
    roots_found = (fmi3_int32_t*)calloc(n_event_indicators, sizeof(fmi3_int32_t));

    INFO("Instantiate");
    jmstatus = fmi3_import_instantiate_model_exchange(
        fmu,
        "Test ME model instance",
        NULL,
        fmi3_false,
        fmi3_false
    );
    REQUIRE(jmstatus == jm_status_success);

    // Try toggling debug logging
    REQUIRE(fmi3_import_set_debug_logging(fmu, fmi3_false, 0, 0) == fmi3_status_ok);
    REQUIRE(fmi3_import_set_debug_logging(fmu, fmi3_true, 0, 0) == fmi3_status_ok);

    INFO("Initialization");
    REQUIRE(fmi3_import_enter_initialization_mode(fmu, toleranceControlled,
        relativeTolerance, tstart, fmi3_false, 0.0) == fmi3_status_ok);
    REQUIRE(fmi3_import_exit_initialization_mode(fmu) == fmi3_status_ok);

    INFO("Simulation; continuous time mode");
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
    REQUIRE(fmi3_import_enter_continuous_time_mode(fmu) == fmi3_status_ok);

    REQUIRE(fmi3_import_get_continuous_states(fmu, states, n_states) == fmi3_status_ok);
    REQUIRE(fmi3_import_get_event_indicators(fmu, event_indicators, n_event_indicators) == fmi3_status_ok);

    while ((tcur < tend) && (!(eventInfo.terminateSimulation || terminateSimulation))) {
        INFO("tcur " << tcur);
        size_t k;
        fmi3_float64_t tlast;
        int zero_crossing_event = 0;

        REQUIRE(fmi3_import_set_time(fmu, tcur) == fmi3_status_ok);

        { /* Swap event_indicators and event_indicators_prev so that we can get new indicators */
            fmi3_float64_t* temp = event_indicators;
            event_indicators = event_indicators_prev;
            event_indicators_prev = temp;
        }
        REQUIRE(fmi3_import_get_event_indicators(fmu, event_indicators, n_event_indicators) == fmi3_status_ok);

        INFO("Check if an event indicator has triggered, and get direction of event indicator");
        for (size_t k = 0; k < n_event_indicators; k++) {
            INFO("k = " << k);
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

        INFO("Handle any events");
        time_event = eventInfo.nextEventTimeDefined && tcur == eventInfo.nextEventTime;
        if (step_event || zero_crossing_event || time_event) {
            REQUIRE(fmi3_import_enter_event_mode(fmu) == fmi3_status_ok);
            do_event_iteration(fmu, &eventInfo);
            REQUIRE(fmi3_import_enter_continuous_time_mode(fmu) == fmi3_status_ok);

            REQUIRE(fmi3_import_get_continuous_states(fmu, states, n_states) == fmi3_status_ok);
            REQUIRE(fmi3_import_get_event_indicators(fmu, event_indicators, n_event_indicators) == fmi3_status_ok);
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

        INFO("Integrate a step");
        REQUIRE(fmi3_import_get_derivatives(fmu, states_der, n_states) == fmi3_status_ok);
        for (size_t k = 0; k < n_states; k++) {
            states[k] = states[k] + hcur*states_der[k];
        }

        /* Set states */
        REQUIRE(fmi3_import_set_continuous_states(fmu, states, n_states) == fmi3_status_ok);
        /* Step is complete */
        REQUIRE(fmi3_import_completed_integrator_step(fmu, fmi3_true, &step_event,
                &terminateSimulation) == fmi3_status_ok);
    }

    INFO("Validate result of scalar variables");
    for (size_t k = 0; k < n_states; k++) {
        INFO("k = " << k);
        fmi3_float64_t diff = states[k] - states_end_results[k];
        diff = diff > 0 ? diff: -diff; /* Take abs */
        REQUIRE(diff <= 1e-10);
    }

    INFO("Validate array variable results");
    {
        fmi3_value_reference_t vr = 12;
        fmi3_float64_t arrValues[4];
        fmi3_float64_t diff;
        fmi3_float64_t tol = 3e-3; /* absolute tolerance */
        size_t nValues = sizeof(arrValues)/sizeof(fmi3_float64_t);
        fmi3_float64_t ref_res[] = { states_end_results[0], states_end_results[1], states_end_results[1], -9.81 };

        /* get result */
        REQUIRE(fmi3_import_get_float64(fmu, &vr, 1, (fmi3_float64_t*)&arrValues, nValues) == fmi3_status_ok);

        /* check result */
        for (size_t k = 0; k < nValues; k++) {
            INFO("k = " << k);
            diff = ref_res[k] - arrValues[k];
            diff = diff > 0 ? diff : -diff;
            REQUIRE(diff <= tol);
        }
    }

    INFO("Cleanup");
    REQUIRE(fmi3_import_terminate(fmu) == fmi3_status_ok);

    fmi3_import_free_instance(fmu);
    free(states);
    free(states_der);
    free(event_indicators);
    free(event_indicators_prev);
    free(roots_found);
}

TEST_CASE("Model-Exchange FMU example") {
    const char* tmpPath;
    jm_callbacks callbacks;
    fmi_import_context_t* context;
    fmi_version_enu_t version;

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
    REQUIRE(tmpPath != nullptr);

    context = fmi_import_allocate_context(&callbacks);
    REQUIRE(context != nullptr);
    REQUIRE(fmi_import_get_fmi_version(context, FMU3_ME_PATH, tmpPath) == fmi_version_3_0_enu);

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(tmpPath);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    REQUIRE((fmi3_import_get_fmu_kind(fmu) & fmi3_fmu_kind_me) == fmi3_fmu_kind_me);

    REQUIRE(fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_me, NULL, NULL) == jm_status_success);

    test_capi_wrappers_me(fmu);
    test_simulate_me(fmu);

    fmi3_import_destroy_dllfmu(fmu);

    // TODO: Various issues related to parsing of Annotations
    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 5);
    fmi3_testutil_import_free(tfmu);

    fmi_import_free_context(context);
    REQUIRE(fmi_import_rmdir(&callbacks, tmpPath) == jm_status_success);
    callbacks.free((void*)tmpPath);
}

static char loggedResourcePath[4096];

// If a log message is output on this format: <PREFIX>=<VALUE>, then copy value to testedResourcePath.
static void test_logger_fmi3Instantiate (fmi3_instance_environment_t instanceEnvironment,
        fmi3_status_t status, fmi3_string_t category, fmi3_string_t message)
{
    char prefix[20] = "TEST_RESOURCE_PATH=";
    if (strncmp(message, "TEST_RESOURCE_PATH=", strlen(prefix)) == 0) {
        strcpy(loggedResourcePath, message + strlen(prefix));        
    }
}

TEST_CASE("Test fallback/default value(s) passed to fmi3Instantiate") {
    const char* tmpPath;
    jm_callbacks callbacks;
    fmi_import_context_t* context;
    fmi_version_enu_t version;
    jm_status_enu_t jmstatus;

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
    REQUIRE(tmpPath != nullptr);

    context = fmi_import_allocate_context(&callbacks);
    REQUIRE(context != nullptr);
    REQUIRE(fmi_import_get_fmi_version(context, FMU3_ME_PATH, tmpPath) == fmi_version_3_0_enu);

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(tmpPath);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    // TODO: Various issues related to parsing of Annotations
    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 5);

    REQUIRE((fmi3_import_get_fmu_kind(fmu) & fmi3_fmu_kind_me) == fmi3_fmu_kind_me);

    // Set logger:
    REQUIRE(fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_me, NULL, test_logger_fmi3Instantiate) == jm_status_success);

    jmstatus = fmi3_import_instantiate_model_exchange(
        fmu,
        "InstanceName",
        NULL,
        fmi3_false,
        fmi3_false
    );
    REQUIRE(jmstatus == jm_status_success);

    // Main part of this test:
    char expected[4096];
    strcpy(expected, tmpPath);
#ifdef WIN32
    strcpy(expected + strlen(expected), "\\resources\\");
#else
    strcpy(expected + strlen(expected), "/resources/");
#endif
    REQUIRE(strcmp(expected, loggedResourcePath) == 0);

    // Clean up
    REQUIRE(fmi3_import_terminate(fmu) == fmi3_status_ok);
    fmi3_import_free_instance(fmu);
    fmi3_testutil_import_free(tfmu);
    fmi_import_free_context(context);
    REQUIRE(fmi_import_rmdir(&callbacks, tmpPath) == jm_status_success);
    callbacks.free((void*)tmpPath);
}