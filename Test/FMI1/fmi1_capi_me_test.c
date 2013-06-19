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
#include <string.h>
#include <errno.h>

#include <fmilib.h>
#include "config_test.h"
/*#include <JM/jm_types.h>
#include <JM/jm_portability.h>
#include <FMI1/fmi1_types.h>
#include <FMI1/fmi1_functions.h>
#include <FMI1/fmi1_capi.h>
#include <JM/jm_callbacks.h> */
#include <FMI1/fmi1_capi.h>
#include <fmu_dummy/fmu1_model_defines.h>


#define MODEL_IDENTIFIER FMU_DUMMY_ME_MODEL_IDENTIFIER

/* #define PRINT_VERBOSE */
#define INSTANCE_NAME		"Test Model"

fmi1_capi_t* fmu;  /* Pointer to the C-API struct that is used in all tests */

/* Logger function used by the C-API */
void importlogger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message)
{
        printf("module = %s, log level = %d: %s\n", module, log_level, message);
}

/* Logger function used by the FMU internally */
void fmilogger(fmi1_component_t c, fmi1_string_t instanceName, fmi1_status_t status, fmi1_string_t category, fmi1_string_t message, ...)
{
	char msg[BUFFER];
    int len;
	va_list argp;	
	va_start(argp, message);
	len = jm_vsnprintf(msg, BUFFER, message, argp);
	printf("fmiStatus = %d;  %s (%s): %s\n", status, instanceName, category, msg);
    if(len > BUFFER) {
          printf("Warning: Message was truncated");
    }
}

/* Pause and exit function. Useally called when an error occured */
void do_exit(int code)
{
	printf("Press any key to exit\n");
	/* getchar(); */
	exit(code);
}

jm_callbacks* callbacks;

/**
 * \brief Tests fmi1_capi_create_dllfmu
 *
 */
int test_create_dllfmu()
{
	fmi1_callback_functions_t callBackFunctions;
	fmi1_fmu_kind_enu_t standard = fmi1_fmu_kind_enu_me;

	callbacks = calloc(1, sizeof(jm_callbacks));

	callbacks->malloc = malloc;
    callbacks->calloc = calloc;
    callbacks->realloc = realloc;
    callbacks->free = free;
    callbacks->logger = importlogger;
    callbacks->context = 0;

	callBackFunctions.logger = fmilogger;
	callBackFunctions.allocateMemory = calloc;
	callBackFunctions.freeMemory = free;

	printf("fmi1_capi_create_dllfmu:                 ");
	fmu = fmi1_capi_create_dllfmu(callbacks, FMU1_DLL_ME_PATH, MODEL_IDENTIFIER_STR, callBackFunctions, standard);
	if (fmu == NULL) {
		printf("An error occured while fmi1_capi_create_dllfmu was called, an error message should been printed.\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("Success\n");
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_load_dll
 *
 */
int test_load_dll()
{
	jm_status_enu_t status;

	printf("fmi1_capi_load_dll:                      ");
	status = fmi1_capi_load_dll(fmu);
	if (status == jm_status_error) {
		printf("Error in fmi1_capi_load_dll: %s\n", "fmi1_capi_get_last_error(fmu)");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("Success\n");
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_load_fcn
 *
 */
int test_load_dll_fcn()
{
	jm_status_enu_t status;

	printf("fmi1_capi_load_fcn:                      ");
	status = fmi1_capi_load_fcn(fmu);
	if (status == jm_status_error) {
		printf("\n");
		printf("Error in fmi1_capi_load_fcn: %s\n", "fmi1_capi_get_last_error(fmu)");
		do_exit(CTEST_RETURN_FAIL);;
	} else {		
		printf("Success\n");
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_get_version
 *
 */
int test_fmi_get_version()
{
	const char* version = fmi1_capi_get_version(fmu);

	printf("fmi1_capi_get_version:                   ");
	if (strcmp(FMI_VERSION, version) != 0) {
		printf("Expected \"%s\" but returned \"%s\"", FMI_VERSION, version);
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("Success\n");
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_get_model_types_platform
 *
 */
int test_fmi_get_model_types_platform()
{
	const char* platformtype = fmi1_capi_get_model_types_platform(fmu);

	printf("fmi1_capi_get_model_types_platform:      ");
	if (strcmp(FMI_PLATFORM_TYPE, platformtype) != 0) {
		printf("Expected \"%s\" but returned \"%s\"", FMI_PLATFORM_TYPE, platformtype);
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("Success\n");
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_instantiate_model
 *
 */
int test_instantiate_model()
{
	fmi1_boolean_t loggingOn = fmi1_true;

	if (fmi1_capi_instantiate_model(fmu, INSTANCE_NAME, FMI_GUID, loggingOn) == NULL) {		
		printf("fmi1_capi_instantiate_model:	         Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_instantiate_model:	         Success\n");
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_set_time
 *
 */
int test_fmi_set_time()
{
	fmi1_status_t status;
	status = fmi1_capi_set_time(fmu, 0.1);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_set_time:                      Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_set_time:                      Success\n");
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_set_continuous_states
 *
 */
int test_set_continuous_states()
{
	fmi1_status_t status;
	size_t k;
	fmi1_real_t states[N_STATES];

	for (k = 0; k < N_STATES; k++) {
		states[k] = (fmi1_real_t)(k + 1) * 12;
	}

	status = fmi1_capi_set_continuous_states(fmu, states, N_STATES);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_set_continuous_states:        Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_set_continuous_states:         Success\n");
#ifdef PRINT_VERBOSE
		for (k=0; k < N_STATES; k++) {
			printf("\t x[%d] = %f\n",k, states[k]);
		}
#endif
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_initialize
 *
 */
int test_initialize()
{
	fmi1_status_t status;
	fmi1_boolean_t toleranceControlled;
	fmi1_real_t relativeTolerance;
	fmi1_event_info_t eventInfo;

	relativeTolerance = 1e-5;
	toleranceControlled = fmi1_true;
	status = fmi1_capi_initialize(fmu, toleranceControlled, relativeTolerance, &eventInfo);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_initialize:                    Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_initialize:                    Success\n");
#ifdef PRINT_VERBOSE
		printf("\t fmiEventInfo.iterationConverged =          %s\n",  eventInfo.iterationConverged ?			"True" : "False");
		printf("\t fmiEventInfo.stateValueReferencesChanged = %s\n",  eventInfo.stateValueReferencesChanged ?	"True" : "False");
		printf("\t fmiEventInfo.stateValuesChanged =          %s\n",  eventInfo.stateValuesChanged ?			"True" : "False");
		printf("\t fmiEventInfo.terminateSimulation =         %s\n",  eventInfo.terminateSimulation ?			"True" : "False");
		printf("\t fmiEventInfo.upcomingTimeEvent =           %s\n",  eventInfo.upcomingTimeEvent ?				"True" : "False");
		printf("\t fmiEventInfo.nextEventTime =               %f\n", eventInfo.nextEventTime);
#endif
	}	
	return 0;
}

/**
 * \brief Tests fmi1_capi_completed_integrator_step
 *
 */
int test_completed_integrator_step()
{
	fmi1_status_t status;
	fmi1_boolean_t callEventUpdate;

	status = fmi1_capi_completed_integrator_step(fmu, &callEventUpdate);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_completed_integrator_step:     Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_completed_integrator_step:     Success\n");
#ifdef PRINT_VERBOSE
		printf("\t callEventUpdate = %s\n", callEventUpdate ? "True" : "False");
#endif
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_get_derivatives
 *
 */
int test_get_derivatives()
{
	fmi1_status_t status;

	fmi1_real_t dstates[N_STATES];

	status = fmi1_capi_get_derivatives(fmu, dstates, N_STATES);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_get_derivatives:              Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_get_derivatives:               Success\n");
#ifdef PRINT_VERBOSE
		for (k=0; k < N_STATES; k++) {
			printf("\t dx[%d] = %f\n", k, dstates[k]);
		}
#endif
	}
	return 0;
}

int test_get_event_indicators()
{
	fmi1_status_t status;

	fmi1_real_t zerocrossing[N_EVENT_INDICATORS];

	status = fmi1_capi_get_event_indicators(fmu, zerocrossing, N_EVENT_INDICATORS);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_get_event_indicators:          Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_get_event_indicators:          Success\n");
#ifdef PRINT_VERBOSE
		for (k=0; k < N_EVENT_INDICATORS; k++) {
			printf("\t nz[%d] = %f\n",k, zerocrossing[k]);
		}
#endif
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_eventUpdate
 *
 */
int test_event_update()
{
	fmi1_status_t status;
	fmi1_boolean_t intermediateResults = fmi1_false;
	fmi1_event_info_t eventInfo;

	status = fmi1_capi_eventUpdate(fmu, intermediateResults, &eventInfo);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_eventUpdate:                   Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_eventUpdate:                   Success\n");
#ifdef PRINT_VERBOSE
		printf("\t fmiEventInfo.iterationConverged =          %s\n",  eventInfo.iterationConverged ?			"True" : "False");
		printf("\t fmiEventInfo.stateValueReferencesChanged = %s\n",  eventInfo.stateValueReferencesChanged ?	"True" : "False");
		printf("\t fmiEventInfo.stateValuesChanged =          %s\n",  eventInfo.stateValuesChanged ?			"True" : "False");
		printf("\t fmiEventInfo.terminateSimulation =         %s\n",  eventInfo.terminateSimulation ?			"True" : "False");
		printf("\t fmiEventInfo.upcomingTimeEvent =           %s\n",  eventInfo.upcomingTimeEvent ?				"True" : "False");
		printf("\t fmiEventInfo.nextEventTime =               %f\n", eventInfo.nextEventTime);
#endif
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_get_continuous_states
 *
 */
int test_get_continuous_states()
{
	fmi1_status_t status;
	fmi1_real_t states[N_STATES];


	status = fmi1_capi_get_continuous_states(fmu, states, N_STATES);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_get_continuous_states:         Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_get_continuous_states:         Success\n");
#ifdef PRINT_VERBOSE
		for (k = 0; k < N_STATES; k++) {
			printf("\t x[%d] = %f\n",k, states[k]);
		}
#endif
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_get_nominal_continuous_states
 *
 */
int test_get_nominal_continuous_states()
{
	fmi1_status_t status;
	
	fmi1_real_t states[N_STATES];

	status = fmi1_capi_get_nominal_continuous_states(fmu, states, N_STATES);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_get_nominal_continuous_states: Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_get_nominal_continuous_states: Success\n");
#ifdef PRINT_VERBOSE
		for (k = 0; k < N_STATES; k++) {
			printf("\t x[%d] = %f\n",k, states[k]);
		}
#endif
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_get_state_value_references
 *
 */
int test_get_state_value_references()
{
	fmi1_status_t status;
	
	fmi1_value_reference_t vrs[N_STATES];

	status = fmi1_capi_get_state_value_references(fmu, vrs, N_STATES);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_get_state_value_references:    Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_get_state_value_references:    Success\n");
#ifdef PRINT_VERBOSE
		for (k=0; k < N_STATES; k++) {
			printf("\t vrs[%u] = %u\n", k, vrs[k]);
		}
#endif
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_set_debug_logging
 *
 */
int test_set_debug_logging()
{
	fmi1_status_t status;
	status = fmi1_capi_set_debug_logging(fmu, fmi1_true);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_set_debug_logging:             Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_set_debug_logging:             Success\n");
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_set_string and fmi1_capi_get_string
 * Some values are set with fmi1_capi_set_string. The same values are retrived with fmi1_capi_get_string and tested to be the same as thoughs that were set.
 */
int test_set_get_string()
{
	fmi1_status_t status;
	fmi1_value_reference_t vrs[N_STRING];
	fmi1_string_t values[N_STRING];
	fmi1_string_t values_ref[N_STRING];
	size_t k;

	for (k = 0; k < N_STRING; k++) {
		vrs[k] = (fmi1_value_reference_t)k;
		values[k] = "hej";
		values_ref[k] = values[k];
	}

	/* Test fmi1_capi_set_string */
	status = fmi1_capi_set_string(fmu, vrs, N_STRING, values);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_set_string:                    Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_set_string:                    Success\n");
	}

	/* Test fmi1_capi_get_string */
	status = fmi1_capi_get_string(fmu, vrs, N_STRING, values);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_get_string:                    Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		for (k = 0; k < N_STRING; k++) {
			if (strcmp(values_ref[k], values[k]) != 0) {
				printf("fmi1_capi_get_string returned values[%u] = \"%s\" expected \"%s\"\n", (unsigned)k, values[k], values_ref[k]);
				do_exit(CTEST_RETURN_FAIL);;
			}
		}
		printf("fmi1_capi_get_string:                    Success\n");
	}

	return 0;
}

/**
 * \brief Tests fmi1_capi_set_integer and fmi1_capi_get_integer
 * Some values are set with fmi1_capi_set_integer. The same values are retrived with fmi1_capi_get_integer and tested to be the same as thoughs that were set.
 */
int test_set_get_integer()
{
	fmi1_status_t status;
	fmi1_value_reference_t vrs[N_INTEGER];
	fmi1_integer_t values[N_INTEGER];
	fmi1_integer_t values_ref[N_INTEGER];
	int k;

	for (k = 0; k < N_INTEGER; k++) {
		vrs[k] = (fmi1_value_reference_t)k;
		values[k] = (k + 1) * 12;
		values_ref[k] = values[k];
	}

	/* Test fmi1_capi_set_integer */
	status = fmi1_capi_set_integer(fmu, vrs, N_INTEGER, values);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_set_integer:                   Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_set_integer:                   Success\n");
	}

	/* Test fmi1_capi_get_integer */
	status = fmi1_capi_get_integer(fmu, vrs, N_INTEGER, values);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_get_integer:                   Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		for (k = 0; k < N_INTEGER; k++) {
			if (values_ref[k] != values[k]) {
				printf("fmi1_capi_get_integer returned values[%d] = \"%d\" expected \"%d\"\n", k, values[k], values_ref[k]);
				do_exit(CTEST_RETURN_FAIL);;
			}
		}
		printf("fmi1_capi_get_integer:                   Success\n");
	}

	return 0;
}

/**
 * \brief Tests fmi1_capi_set_boolean and fmi1_capi_get_boolean
 * Some values are set with fmi1_capi_set_boolean. The same values are retrived with fmi1_capi_get_boolean and tested to be the same as thoughs that were set.
 */
int test_set_get_boolean()
{
	fmi1_status_t status;
	fmi1_value_reference_t vrs[N_BOOLEAN];
	fmi1_boolean_t values[N_BOOLEAN];
	fmi1_boolean_t values_ref[N_BOOLEAN];
	size_t k;

	for (k = 0; k < N_INTEGER; k++) {
		vrs[k] = (fmi1_value_reference_t)k;
		values[k] = fmi1_true;
		values_ref[k] = values[k];
	}

	/* Test fmi1_capi_set_boolean */
	status = fmi1_capi_set_boolean(fmu, vrs, N_BOOLEAN, values);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_set_boolean:                   Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_set_boolean:                   Success\n");
	}

	/* Test fmi1_capi_get_boolean */
	status = fmi1_capi_get_boolean(fmu, vrs, N_BOOLEAN, values);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_get_boolean:                   Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		for (k = 0; k < N_BOOLEAN; k++) {
			if (values_ref[k] != values[k]) {
				printf("fmi1_capi_get_boolean returned values[%u] = \"%s\" expected \"%s\"\n", (unsigned)k, values[k] ? "fmiTrue" : "fmiFalse", values_ref[k] ? "fmiTrue" : "fmiFalse");
				do_exit(CTEST_RETURN_FAIL);;
			}
		}
		printf("fmi1_capi_get_boolean:                   Success\n");
	}

	return 0;
}

/**
 * \brief Tests fmi1_capi_set_real and fmi1_capi_get_real
 * Some values are set with fmi1_capi_set_real. The same values are retrived with fmi1_capi_get_real and tested to be the same as thoughs that were set.
 */
int test_set_get_real()
{
	fmi1_status_t status;
	fmi1_value_reference_t vrs[N_REAL];
	fmi1_real_t values[N_REAL];
	fmi1_real_t values_ref[N_REAL];
	size_t k;

	for (k = 0; k < N_REAL; k++) {
		vrs[k] = (fmi1_value_reference_t)(N_STATES + k);
		values[k] = (fmi1_real_t)(k + 1) * 12;
		values_ref[k] = values[k];
	}

	/* Test fmi1_capi_set_real */
	status = fmi1_capi_set_real(fmu, vrs, N_REAL, values);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_set_real:                      Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_set_real:                      Success\n");
	}

	/* Test fmi1_capi_get_real */
	status = fmi1_capi_get_real(fmu, vrs, N_REAL, values);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_get_real:                      Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		for (k = 0; k < N_REAL; k++) {
			if (values_ref[k] != values[k]) {
				printf("%f\n",12.0);
				printf("fmi1_capi_get_real returned values[%u] = \"%f\" expected \"%f\"\n", (unsigned)k, (double)values[k], (double)values_ref[k]);
				do_exit(CTEST_RETURN_FAIL);;
			}
		}
		printf("fmi1_capi_get_real:                      Success\n");
	}

	return 0;
}

/**
 * \brief Tests fmi1_capi_terminate
 *
 */
int test_terminate()
{
	fmi1_status_t status;

	status = fmi1_capi_terminate(fmu);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_terminate:                     Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_terminate:                     Success\n");
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_free_model_instance
 *
 */
int test_free_model_instance()
{
	fmi1_capi_free_model_instance(fmu);
	printf("fmi1_capi_instantiate_model:             Success\n");		
	return 0;
}

/**
 * \brief Tests fmi1_capi_free_dll
 *
 */
int test_free_dll()
{
	fmi1_capi_free_dll(fmu);
	printf("fmi1_capi_free_dll:                      Success\n");
	return 0;
}

/**
 * \brief Tests fmi1_capi_destroy_dllfmu
 *
 */
int test_destroy_dllfmu()
{
	fmi1_capi_destroy_dllfmu(fmu);
	printf("fmi1_capi_destroy_dllfmu:                Success\n");
	return 0;
}

/**
 * \brief Tests the C-API for FMI 1.0 Model Exchange.
 * The tests are performed using a test-dll. The functions are called and the values are set or returned are validated either in the test function(output functions) or inside the dll(input functions). If any error occures, the program exits. 
 *
 */
int main(int argc, char *argv[])
{
	/* Test CAPI constructor functions */
	test_create_dllfmu();
	test_load_dll();
	test_load_dll_fcn();

	/* FMI ME 1.0 functions */
	test_fmi_get_model_types_platform();
	test_instantiate_model();
	test_fmi_set_time();
	test_set_continuous_states();
	test_initialize();
	test_completed_integrator_step();
	test_get_derivatives();
	test_get_event_indicators();
	test_event_update();
	test_get_continuous_states();
	test_get_nominal_continuous_states();
	test_get_state_value_references();

	test_fmi_get_version();
	test_set_get_string();
	test_set_get_boolean();
	test_set_get_integer();
	test_set_get_real();
	test_set_debug_logging();

	test_terminate();
	test_free_model_instance();

	/* Test CAPI destructor functions */
	test_free_dll();
	test_destroy_dllfmu();
	free(callbacks);
	printf("\n");
	printf("Everything seems to be ok!\n");
	printf("\n");
	do_exit(CTEST_RETURN_SUCCESS);
	return 0;
}


