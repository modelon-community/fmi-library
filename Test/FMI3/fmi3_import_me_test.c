/*
    Copyright (C) 2012 Modelon AB

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

#define BUFFER 1000

void do_exit(int code)
{
	printf("Press 'Enter' to exit\n");
	/* getchar(); */
	exit(code);
}

void do_event_iteration(fmi3_import_t *fmu, fmi3_event_info_t *eventInfo)
{
    eventInfo->newDiscreteStatesNeeded = fmi3_true;
    eventInfo->terminateSimulation     = fmi3_false;
    while (eventInfo->newDiscreteStatesNeeded && !eventInfo->terminateSimulation) {
        fmi3_import_new_discrete_states(fmu, eventInfo);
    }
}

int test_parsed_all_varialbes(fmi3_import_t* fmu)
{ /* Test that all variables where parsed */
    fmi3_import_model_counts_t  mc;
    unsigned int                n_total;
    
    fmi3_import_collect_model_counts(fmu, &mc);
    n_total = mc.num_constants
            + mc.num_fixed
            + mc.num_tunable
            + mc.num_discrete
            + mc.num_continuous
            + mc.num_independent;
    
    
    if (n_total != 12) {
        do_exit(CTEST_RETURN_FAIL);
    }

    return 0;
}
	   
int test_simulate_me(fmi3_import_t* fmu)
{	
	fmi3_status_t fmistatus;
	jm_status_enu_t jmstatus;
	fmi3_real_t tstart = 0.0;
	fmi3_real_t tcur;
	fmi3_real_t hcur;
	fmi3_real_t hdef = 0.1;
	fmi3_real_t tend = 2.0;
	size_t n_states;
	size_t n_event_indicators;
	fmi3_real_t* states;
	fmi3_real_t states_end_results[] = {0.362000, -3.962000};
	fmi3_real_t* states_der;
	fmi3_real_t* event_indicators;
	fmi3_real_t* event_indicators_prev;
	fmi3_boolean_t callEventUpdate;
	fmi3_boolean_t terminateSimulation = fmi3_false;
	fmi3_boolean_t toleranceControlled = fmi3_true;
	fmi3_real_t relativeTolerance = 0.001;
	fmi3_event_info_t eventInfo;
	size_t k;

	printf("Version returned from FMU:   %s\n", fmi3_import_get_version(fmu));
	printf("Platform type returned:      %s\n", fmi3_import_get_types_platform(fmu));

	n_states = fmi3_import_get_number_of_continuous_states(fmu);
	n_event_indicators = fmi3_import_get_number_of_event_indicators(fmu);

	if (sizeof(states_end_results)/sizeof(fmi3_real_t) != n_states) {
		printf("Number of states and results have different length n_states = %u n_results = %u\n", (unsigned)n_states, (unsigned)sizeof(states_end_results));
		do_exit(CTEST_RETURN_FAIL);
	}

	states = calloc(n_states, sizeof(double));
	states_der = calloc(n_states, sizeof(double));
	event_indicators = calloc(n_event_indicators, sizeof(double));
	event_indicators_prev = calloc(n_event_indicators, sizeof(double));

	jmstatus = fmi3_import_instantiate(fmu, "Test ME model instance",fmi3_model_exchange,0,0);
	if (jmstatus == jm_status_error) {
		printf("fmi3_import_instantiate failed\n");
		do_exit(CTEST_RETURN_FAIL);
	}

	fmistatus = fmi3_import_set_debug_logging(fmu, fmi3_false,0,0);
	printf("fmi3_import_set_debug_logging:  %s\n", fmi3_status_to_string(fmistatus));	
	fmi3_import_set_debug_logging(fmu, fmi3_true, 0, 0);

    fmistatus = fmi3_import_setup_experiment(fmu, toleranceControlled,
        relativeTolerance, tstart, fmi3_false, 0.0);

    fmistatus = fmi3_import_enter_initialization_mode(fmu);
    fmistatus = fmi3_import_exit_initialization_mode(fmu);

	tcur = tstart;
	hcur = hdef;
	callEventUpdate = fmi3_false;

	eventInfo.newDiscreteStatesNeeded           = fmi3_false;
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
        fmi3_real_t tlast;
		int zero_crossing_event = 0;

		fmistatus = fmi3_import_set_time(fmu, tcur);

        { /* Swap event_indicators and event_indicators_prev so that we can get new indicators */
            fmi3_real_t *temp = event_indicators;
            event_indicators = event_indicators_prev;
            event_indicators_prev = temp;
        }
		fmistatus = fmi3_import_get_event_indicators(fmu, event_indicators, n_event_indicators);

		/* Check if an event indicator has triggered */
		for (k = 0; k < n_event_indicators; k++) {
			if ((event_indicators[k] > 0) != (event_indicators_prev[k] > 0)) {
				zero_crossing_event = 1;
				break;
			}
		}

		/* Handle any events */
		if (callEventUpdate || zero_crossing_event ||
		  (eventInfo.nextEventTimeDefined && tcur == eventInfo.nextEventTime)) {
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
		fmistatus = fmi3_import_completed_integrator_step(fmu, fmi3_true, &callEventUpdate,
                                                          &terminateSimulation);
	}	

	/* Validate result */
	for (k = 0; k < n_states; k++) {
		fmi3_real_t res = states[k] - states_end_results[k];
		res = res > 0 ? res: -res; /* Take abs */
		if (res > 1e-10) {
			printf("Simulation results is wrong  states[%u] %f != %f, |res| = %f\n", (unsigned)k, states[k], states_end_results[k], res);
			do_exit(CTEST_RETURN_FAIL);
		}
	}
	

	fmistatus = fmi3_import_terminate(fmu);

	fmi3_import_free_instance(fmu);

	free(states);
	free(states_der);
	free(event_indicators);
	free(event_indicators_prev);

	return 0;
}

int main(int argc, char *argv[])
{
	fmi3_callback_functions_t callBackFunctions;
	const char* FMUPath;
	const char* tmpPath;
	jm_callbacks callbacks;
	fmi_import_context_t* context;
	fmi_version_enu_t version;
	jm_status_enu_t status;

	fmi3_import_t* fmu;	

	if(argc < 3) {
		printf("Usage: %s <fmu_file> <temporary_dir>\n", argv[0]);
		do_exit(CTEST_RETURN_FAIL);
	}

	FMUPath = argv[1];
	tmpPath = argv[2];


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


	context = fmi_import_allocate_context(&callbacks);

	version = fmi_import_get_fmi_version(context, FMUPath, tmpPath);

	if(version != fmi_version_3_0_enu) {
		printf("Only version 3.0 is supported by this code\n");
		do_exit(CTEST_RETURN_FAIL);
	}

	fmu = fmi3_import_parse_xml(context, tmpPath, 0);

	if(!fmu) {
		printf("Error parsing XML, exiting\n");
		do_exit(CTEST_RETURN_FAIL);
	}	

	if(fmi3_import_get_fmu_kind(fmu) == fmi3_fmu_kind_cs) {
		printf("Only ME 3.0 is supported by this code\n");
		do_exit(CTEST_RETURN_FAIL);
	}

	callBackFunctions.logger = fmi3_log_forwarding;
	callBackFunctions.allocateMemory = calloc;
	callBackFunctions.freeMemory = free;
	callBackFunctions.componentEnvironment = fmu;

	status = fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_me, &callBackFunctions);
	if (status == jm_status_error) {
		printf("Could not create the DLL loading mechanism(C-API test).\n");
		do_exit(CTEST_RETURN_FAIL);
	}
    
    test_parsed_all_varialbes(fmu);
    
	test_simulate_me(fmu);

	fmi3_import_destroy_dllfmu(fmu);

	fmi3_import_free(fmu);
	fmi_import_free_context(context);
	
	printf("Everything seems to be OK since you got this far=)!\n");

	do_exit(CTEST_RETURN_SUCCESS);

	return 0;
}


