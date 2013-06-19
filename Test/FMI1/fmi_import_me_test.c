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
	   
int test_simulate_me(fmi1_import_t* fmu)
{	
	fmi1_status_t fmistatus;
	jm_status_enu_t jmstatus;
	fmi1_real_t tstart = 0.0;
	fmi1_real_t tcur;
	fmi1_real_t hcur;
	fmi1_real_t hdef = 0.1;
	fmi1_real_t tend = 2.0;
	size_t n_states;
	size_t n_event_indicators;
	fmi1_real_t* states;
	fmi1_real_t states_end_results[] = {0.362000, -3.962000};
	fmi1_real_t* states_der;
	fmi1_real_t* event_indicators;
	fmi1_real_t* event_indicators_prev;
	fmi1_boolean_t callEventUpdate;
	fmi1_boolean_t toleranceControlled = fmi1_true;
	fmi1_real_t relativeTolerance = 0.001;
	fmi1_event_info_t eventInfo;
	fmi1_boolean_t intermediateResults = fmi1_false;
	size_t k;

	printf("Version returned from FMU:   %s\n", fmi1_import_get_version(fmu));
	printf("Platform type returned:      %s\n", fmi1_import_get_model_types_platform(fmu));

	n_states = fmi1_import_get_number_of_continuous_states(fmu);
	n_event_indicators = fmi1_import_get_number_of_event_indicators(fmu);

	if (sizeof(states_end_results)/sizeof(fmi1_real_t) != n_states) {
		printf("Number of states and results have different length n_states = %u n_results = %u\n", (unsigned)n_states, (unsigned)sizeof(states_end_results));
		do_exit(CTEST_RETURN_FAIL);
	}

	states = calloc(n_states, sizeof(double));
	states_der = calloc(n_states, sizeof(double));
	event_indicators = calloc(n_event_indicators, sizeof(double));
	event_indicators_prev = calloc(n_event_indicators, sizeof(double));

	jmstatus = fmi1_import_instantiate_model(fmu, "Test ME model instance");
	if (jmstatus == jm_status_error) {
		printf("fmi1_import_instantiate_model failed\n");
		do_exit(CTEST_RETURN_FAIL);
	}

	fmistatus = fmi1_import_set_time(fmu, tstart);

	fmistatus = fmi1_import_initialize(fmu, toleranceControlled, relativeTolerance, &eventInfo);

	fmistatus = fmi1_import_get_continuous_states(fmu, states, n_states);
	fmistatus = fmi1_import_get_event_indicators(fmu, event_indicators_prev, n_event_indicators);

	fmistatus = fmi1_import_set_debug_logging(fmu, fmi1_false);
	printf("fmi1_import_set_debug_logging:  %s\n", fmi1_status_to_string(fmistatus));	
	fmi1_import_set_debug_logging(fmu, fmi1_true);

	tcur = tstart;
	hcur = hdef;
	callEventUpdate = fmi1_false;

	while (tcur < tend) {
		size_t k;
		int zero_crossning_event = 0;

		fmistatus = fmi1_import_set_time(fmu, tcur);
		fmistatus = fmi1_import_get_event_indicators(fmu, event_indicators, n_event_indicators);

		/* Check if an event inidcator has triggered */
		for (k = 0; k < n_event_indicators; k++) {
			if (event_indicators[k]*event_indicators_prev[k] < 0) {
				zero_crossning_event = 1;
				break;
			}
		}

		/* Handle any events */
		if (callEventUpdate || zero_crossning_event || (eventInfo.upcomingTimeEvent && tcur == eventInfo.nextEventTime)) {
			fmistatus = fmi1_import_eventUpdate(fmu, intermediateResults, &eventInfo);
			fmistatus = fmi1_import_get_continuous_states(fmu, states, n_states);
			fmistatus = fmi1_import_get_event_indicators(fmu, event_indicators, n_event_indicators);
			fmistatus = fmi1_import_get_event_indicators(fmu, event_indicators_prev, n_event_indicators);
		}

		/* Updated next time step */
		if (eventInfo.upcomingTimeEvent) {
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
		fmistatus = fmi1_import_get_derivatives(fmu, states_der, n_states);
		for (k = 0; k < n_states; k++) {
			states[k] = states[k] + hcur*states_der[k];	
			if (k == 0) printf("Ball hight state[%u] = %f\n", (unsigned)k, states[k]);
		}

		/* Set states */
		fmistatus = fmi1_import_set_continuous_states(fmu, states, n_states);
		/* Step is complete */
		fmistatus = fmi1_import_completed_integrator_step(fmu, &callEventUpdate);
	}	

	/* Validate result */
	for (k = 0; k < n_states; k++) {
		fmi1_real_t res = states[k] - states_end_results[k];
		res = res > 0 ? res: -res; /* Take abs */
		if (res > 1e-10) {
			printf("Simulation results is wrong  states[%u] %f != %f, |res| = %f\n", (unsigned)k, states[k], states_end_results[k], res);
			do_exit(CTEST_RETURN_FAIL);
		}
	}
	

	fmistatus = fmi1_import_terminate(fmu);

	fmi1_import_free_model_instance(fmu);

	free(states);
	free(states_der);
	free(event_indicators);
	free(event_indicators_prev);

	return 0;
}

typedef struct {
	fmi1_import_t* fmu;
	fmi_import_context_t* context;
	jm_callbacks* callbacks;
	fmi1_callback_functions_t callBackFunctions;
} fmul_t;

fmul_t load(int argc, char *argv[])
{
	fmi1_callback_functions_t callBackFunctions;
	const char* FMUPath;
	const char* tmpPath;
	jm_callbacks* callbacks;
	fmi_import_context_t* context;
	fmi_version_enu_t version;
	jm_status_enu_t status;
	static int isunzipped;

	fmi1_import_t* fmu;	

	if(argc < 3) {
		printf("Usage: %s <fmu_file> <temporary_dir>\n", argv[0]);
		do_exit(CTEST_RETURN_FAIL);
	}

	FMUPath = argv[1];
	tmpPath = argv[2];


	callbacks = (jm_callbacks*)malloc(sizeof(jm_callbacks));
	callbacks->malloc = malloc;
    callbacks->calloc = calloc;
    callbacks->realloc = realloc;
    callbacks->free = free;
    callbacks->logger = jm_default_logger;
	callbacks->log_level = jm_log_level_debug;
    callbacks->context = 0;

	callBackFunctions.logger = fmi1_log_forwarding;
	callBackFunctions.allocateMemory = calloc;
	callBackFunctions.freeMemory = free;

#ifdef FMILIB_GENERATE_BUILD_STAMP
	printf("Library build stamp:\n%s\n", fmilib_get_build_stamp());
#endif


	context = fmi_import_allocate_context(callbacks);
	
	if (isunzipped == 0) { /* Unzip the FMU only once. Overwriting the dll/so file may cause a segfault. */
		version = fmi_import_get_fmi_version(context, FMUPath, tmpPath);
		if(version != fmi_version_1_enu) {
			printf("Only version 1.0 is supported so far\n");
			do_exit(CTEST_RETURN_FAIL);
		}
		isunzipped = 1;
	}

	fmu = fmi1_import_parse_xml(context, tmpPath);

	if(!fmu) {
		printf("Error parsing XML, exiting\n");
		do_exit(CTEST_RETURN_FAIL);
	}	

	status = fmi1_import_create_dllfmu(fmu, callBackFunctions, 1);
	if (status == jm_status_error) {
		printf("Could not create the DLL loading mechanism(C-API test).\n");
		do_exit(CTEST_RETURN_FAIL);
	}
	
	test_simulate_me(fmu);

	printf("Everything seems to be OK since you got this far=)!\n");
	{
		fmul_t fmus;
		fmus.callBackFunctions = callBackFunctions;
		fmus.callbacks = callbacks;
		fmus.context = context;
		fmus.fmu = fmu;
		return fmus;
	}
}

void destroy(fmul_t* fmus) {
	fmi1_import_destroy_dllfmu(fmus->fmu);
	fmi1_import_free(fmus->fmu);
	fmi_import_free_context(fmus->context);
	free(fmus->callbacks);
	memset(fmus, 0, sizeof(fmul_t));
}
 
/* Load and simulate 150 FMUs. Destroy and free all memory last. Usefull testing speciall for the registerGlobally functionality. */
#define NUMBER_OF_TESTS 150
int main(int argc, char *argv[])
{
	fmul_t fmul[NUMBER_OF_TESTS];
	int k;
	
	for (k=0;k<NUMBER_OF_TESTS;k++) {
		fmul[k] = load(argc, argv);
	}

	for (k=0;k<NUMBER_OF_TESTS;k++) {
		destroy(&fmul[k]);
	}

	do_exit(CTEST_RETURN_SUCCESS);
}
