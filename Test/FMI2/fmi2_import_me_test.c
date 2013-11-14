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
	   
int test_simulate_me(fmi2_import_t* fmu)
{	
	fmi2_status_t fmistatus;
	jm_status_enu_t jmstatus;
	fmi2_real_t tstart = 0.0;
	fmi2_real_t tcur;
	fmi2_real_t hcur;
	fmi2_real_t hdef = 0.1;
	fmi2_real_t tend = 2.0;
	size_t n_states;
	size_t n_event_indicators;
	fmi2_real_t* states;
	fmi2_real_t states_end_results[] = {0.362000, -3.962000};
	fmi2_real_t* states_der;
	fmi2_real_t* event_indicators;
	fmi2_real_t* event_indicators_prev;
	fmi2_boolean_t callEventUpdate;
	fmi2_boolean_t terminateSimulation = fmi2_false;
	fmi2_boolean_t toleranceControlled = fmi2_true;
	fmi2_real_t relativeTolerance = 0.001;
	fmi2_event_info_t eventInfo;
	fmi2_boolean_t intermediateResults = fmi2_false;
	size_t k;

	printf("Version returned from FMU:   %s\n", fmi2_import_get_version(fmu));
	printf("Platform type returned:      %s\n", fmi2_import_get_types_platform(fmu));

	n_states = fmi2_import_get_number_of_continuous_states(fmu);
	n_event_indicators = fmi2_import_get_number_of_event_indicators(fmu);

	if (sizeof(states_end_results)/sizeof(fmi2_real_t) != n_states) {
		printf("Number of states and results have different length n_states = %u n_results = %u\n", (unsigned)n_states, (unsigned)sizeof(states_end_results));
		do_exit(CTEST_RETURN_FAIL);
	}

	states = calloc(n_states, sizeof(double));
	states_der = calloc(n_states, sizeof(double));
	event_indicators = calloc(n_event_indicators, sizeof(double));
	event_indicators_prev = calloc(n_event_indicators, sizeof(double));

	jmstatus = fmi2_import_instantiate(fmu, "Test ME model instance",fmi2_model_exchange,0,0);
	if (jmstatus == jm_status_error) {
		printf("fmi2_import_instantiate failed\n");
		do_exit(CTEST_RETURN_FAIL);
	}

	fmistatus = fmi2_import_set_time(fmu, tstart);

	fmistatus = fmi2_import_initialize_model(fmu, toleranceControlled, relativeTolerance, &eventInfo);

	fmistatus = fmi2_import_get_continuous_states(fmu, states, n_states);
	fmistatus = fmi2_import_get_event_indicators(fmu, event_indicators_prev, n_event_indicators);

	fmistatus = fmi2_import_set_debug_logging(fmu, fmi2_false,0,0);
	printf("fmi2_import_set_debug_logging:  %s\n", fmi2_status_to_string(fmistatus));	
	fmi2_import_set_debug_logging(fmu, fmi2_true, 0, 0);

	tcur = tstart;
	hcur = hdef;
	callEventUpdate = fmi2_false;

	while (tcur < tend) {
		size_t k;
		int zero_crossning_event = 0;

		fmistatus = fmi2_import_set_time(fmu, tcur);
		fmistatus = fmi2_import_get_event_indicators(fmu, event_indicators, n_event_indicators);

		/* Check if an event inidcator has triggered */
		for (k = 0; k < n_event_indicators; k++) {
			if (event_indicators[k]*event_indicators_prev[k] < 0) {
				zero_crossning_event = 1;
				break;
			}
		}

		/* Handle any events */
		if (callEventUpdate || zero_crossning_event ||
		  (eventInfo.nextEventTimeDefined && tcur == eventInfo.nextEventTime)) {
			fmistatus = fmi2_import_eventUpdate(fmu, intermediateResults, &eventInfo);
			fmistatus = fmi2_import_completed_event_iteration(fmu);
			fmistatus = fmi2_import_get_continuous_states(fmu, states, n_states);
			fmistatus = fmi2_import_get_event_indicators(fmu, event_indicators, n_event_indicators);
			fmistatus = fmi2_import_get_event_indicators(fmu, event_indicators_prev, n_event_indicators);
		}

		/* Updated next time step */
		if (eventInfo.nextEventTimeDefined) {
			if (tcur + hdef < eventInfo.nextEventTime) {
				hcur = hdef;
			} else {
				hcur = eventInfo.nextEventTime - tcur;
			}
		} else {
			hcur = hdef;
		}
		tcur += hcur;
		if(tcur > tend - hcur/1e16) {
			tcur -= hcur;
			hcur = (tend - tcur);
			tcur = tend;				
		}
		/* Integrate a step */
		fmistatus = fmi2_import_get_derivatives(fmu, states_der, n_states);
		for (k = 0; k < n_states; k++) {
			states[k] = states[k] + hcur*states_der[k];	
			if (k == 0) printf("Ball hight state[%u] = %f\n", (unsigned)k, states[k]);
		}

		/* Set states */
		fmistatus = fmi2_import_set_continuous_states(fmu, states, n_states);
		/* Step is complete */
		fmistatus = fmi2_import_completed_integrator_step(fmu, fmi2_true, &callEventUpdate,
                                                                  &terminateSimulation);
	}	

	/* Validate result */
	for (k = 0; k < n_states; k++) {
		fmi2_real_t res = states[k] - states_end_results[k];
		res = res > 0 ? res: -res; /* Take abs */
		if (res > 1e-10) {
			printf("Simulation results is wrong  states[%u] %f != %f, |res| = %f\n", (unsigned)k, states[k], states_end_results[k], res);
			do_exit(CTEST_RETURN_FAIL);
		}
	}
	

	fmistatus = fmi2_import_terminate(fmu);

	fmi2_import_free_instance(fmu);

	free(states);
	free(states_der);
	free(event_indicators);
	free(event_indicators_prev);

	return 0;
}

int main(int argc, char *argv[])
{
	fmi2_callback_functions_t callBackFunctions;
	const char* FMUPath;
	const char* tmpPath;
	jm_callbacks callbacks;
	fmi_import_context_t* context;
	fmi_version_enu_t version;
	jm_status_enu_t status;

	fmi2_import_t* fmu;	

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

	if(version != fmi_version_2_0_enu) {
		printf("Only version 2.0 is supported by this code\n");
		do_exit(CTEST_RETURN_FAIL);
	}

	fmu = fmi2_import_parse_xml(context, tmpPath,0);

	if(!fmu) {
		printf("Error parsing XML, exiting\n");
		do_exit(CTEST_RETURN_FAIL);
	}	

	if(fmi2_import_get_fmu_kind(fmu) == fmi2_fmu_kind_cs) {
		printf("Only ME 2.0 is supported by this code\n");
		do_exit(CTEST_RETURN_FAIL);
	}

	callBackFunctions.logger = fmi2_log_forwarding;
	callBackFunctions.allocateMemory = calloc;
	callBackFunctions.freeMemory = free;
	callBackFunctions.componentEnvironment = fmu;

	status = fmi2_import_create_dllfmu(fmu, fmi2_fmu_kind_me, &callBackFunctions);
	if (status == jm_status_error) {
		printf("Could not create the DLL loading mechanism(C-API test).\n");
		do_exit(CTEST_RETURN_FAIL);
	}
	
	test_simulate_me(fmu);

	fmi2_import_destroy_dllfmu(fmu);

	fmi2_import_free(fmu);
	fmi_import_free_context(context);
	
	printf("Everything seems to be OK since you got this far=)!\n");

	do_exit(CTEST_RETURN_SUCCESS);

	return 0;
}


