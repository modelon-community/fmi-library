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

#include <JM/jm_types.h>
#include <JM/jm_portability.h>
#include <FMI1/fmi1_types.h>
#include <FMI1/fmi1_functions.h>
#include <FMI1/fmi1_capi.h>
#include <JM/jm_callbacks.h>
#include <fmu_dummy/fmu1_model_defines.h>
#include "config_test.h" /* Defines path to the DLL file */


#define MODEL_IDENTIFIER FMU_DUMMY_CS_MODEL_IDENTIFIER

/* #define PRINT_VERBOSE */
#define INSTANCE_NAME		"Test Model"

fmi1_capi_t* fmu; /* Pointer to the C-API struct that is used in all tests */

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
	len=jm_vsnprintf(msg, BUFFER, message, argp);
	printf("fmiStatus = %d;  %s (%s): %s\n", status, instanceName, category, msg);
    if(len > BUFFER) {
        printf("Warning: message was trancated");
    }
}

/* Pause and exit function. Useally called when an error occured */
void do_exit(int code)
{
	printf("Press any key to exit\n");
	/* getchar(); */
	exit(code);
}

jm_callbacks* callbacks = 0;

/**
 * \brief Tests fmi1_capi_create_dllfmu
 *
 */
int test_create_dllfmu()
{
	fmi1_callback_functions_t callBackFunctions;
	fmi1_fmu_kind_enu_t standard = fmi1_fmu_kind_enu_cs_standalone;	

	callbacks = calloc(1, sizeof(jm_callbacks));

	callbacks->malloc = malloc;
    callbacks->calloc = calloc;
    callbacks->realloc = realloc;
    callbacks->free = free;
    callbacks->logger = importlogger;
    callbacks->log_level = jm_log_level_debug;
    callbacks->context = 0;

	callBackFunctions.logger = fmilogger;
	callBackFunctions.allocateMemory = calloc;
	callBackFunctions.freeMemory = free;

	printf("fmi1_capi_create_dllfmu:                 ");
	fmu = fmi1_capi_create_dllfmu(callbacks, FMU1_DLL_CS_PATH, MODEL_IDENTIFIER_STR, callBackFunctions, standard);
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
		printf("Error in fmi1_capi_load_dll: %s\n", jm_get_last_error(callbacks));
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
 * \brief Tests fmi1_capi_get_types_platform
 *
 */
int test_get_types_platform()
{
	const char* platformtype = fmi1_capi_get_types_platform(fmu);

	printf("fmi1_capi_get_types_platform:            ");
	if (strcmp(FMI_PLATFORM_TYPE, platformtype) != 0) {
		printf("Expected \"%s\" but returned \"%s\"", FMI_PLATFORM_TYPE, platformtype);
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("Success\n");
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_instantiate_slave
 *
 */
int test_instantiate_slave()
{
	fmi1_string_t fmuLocation;
	fmi1_string_t mimeType;
	fmi1_real_t timeout;
	fmi1_boolean_t visible;
	fmi1_boolean_t interactive;
	fmi1_boolean_t loggingOn;

	fmuLocation = "";
	mimeType = "";
	timeout = 0;
	visible = fmi1_false;
	interactive = fmi1_false;
	loggingOn = fmi1_true;

	if (fmi1_capi_instantiate_slave(fmu, INSTANCE_NAME, FMI_GUID, fmuLocation, mimeType, timeout, visible, interactive, loggingOn) == NULL) {		
		printf("fmi1_capi_instantiate_slave:             Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_instantiate_slave:             Success\n");
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_initialize_slave
 *
 */
int test_initialize_slave()
{
        fmi1_status_t status;
	fmi1_real_t tStart;
	fmi1_real_t tStop;
	fmi1_boolean_t StopTimeDefined;

	tStart = 0;
	tStop = 10;
	StopTimeDefined = fmi1_false;

	status = fmi1_capi_initialize_slave(fmu, tStart, StopTimeDefined, tStop);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_initialize_slave:              Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_initialize_slave:              Success\n");
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
 * \brief Tests fmi1_capi_cancel_step
 *
 */
int test_cancel_step()
{
        fmi1_status_t status;
	status = fmi1_capi_cancel_step(fmu);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_cancel_step:                   Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_cancel_step:                   Success\n");
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_do_step
 *
 */
int test_do_step()
{
        fmi1_status_t status;
	fmi1_real_t currentCommunicationPoint;
	fmi1_real_t communicationStepSize; 
	fmi1_boolean_t newStep;

	currentCommunicationPoint = 0;
	communicationStepSize = 0.1;
	newStep = fmi1_true;
	status = fmi1_capi_do_step(fmu, currentCommunicationPoint, communicationStepSize, newStep);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_do_step:                       Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_do_step:                       Success\n");
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_get_status
 *
 */
int test_get_status()
{
        fmi1_status_t status;
	fmi1_status_kind_t statuskind;
	fmi1_status_t  statusvalue;

	statuskind = fmi1_do_step_status;
	status = fmi1_capi_get_status(fmu, statuskind, &statusvalue);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_get_status:                    Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_get_status:                    Success\n");
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_get_real_status
 *
 */
int test_get_real_status()
{
        fmi1_status_t status;
	fmi1_status_kind_t statuskind = fmi1_last_successful_time;
	fmi1_real_t real;

	status = fmi1_capi_get_real_status(fmu, statuskind, &real);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_get_real_status:               Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_get_real_status:               Success\n");
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_get_integer_status
 *
 */
int test_get_integer_status()
{
        fmi1_status_t status;
	fmi1_status_kind_t statuskind = fmi1_last_successful_time;
	fmi1_integer_t integer;

	status = fmi1_capi_get_integer_status(fmu, statuskind, &integer);
	if (status != fmi1_status_discard) {
		printf("fmi1_capi_get_integer_status:            Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_get_integer_status:            Success\n");
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_get_boolean_status
 *
 */
int test_get_boolean_status()
{
        fmi1_status_t status;
	fmi1_status_kind_t statuskind = fmi1_last_successful_time;
	fmi1_boolean_t boolean;

	status = fmi1_capi_get_boolean_status(fmu, statuskind, &boolean);
	if (status != fmi1_status_discard) {
		printf("fmi1_capi_get_boolean_status:            Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_get_boolean_status:            Success\n");
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_get_string_status
 *
 */
int test_get_string_status()
{
        fmi1_status_t status;
	fmi1_status_kind_t statuskind = fmi1_pending_status;
	fmi1_string_t string;

	status = fmi1_capi_get_string_status(fmu, statuskind, &string);
	if (status != fmi1_status_discard) {
		printf("fmi1_capi_get_string_status:             Failed\n");
		do_exit(CTEST_RETURN_FAIL);;
	} else {
		printf("fmi1_capi_get_string_status:             Success\n");
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
		do_exit(CTEST_RETURN_FAIL);
	} else {
		for (k = 0; k < N_INTEGER; k++) {
			if (values_ref[k] != values[k]) {
				printf("fmi1_capi_get_integer returned values[%d] = \"%d\" expected \"%d\"\n", k, values[k], values_ref[k]);
				do_exit(CTEST_RETURN_FAIL);
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
		do_exit(CTEST_RETURN_FAIL);
	} else {
		printf("fmi1_capi_set_boolean:                   Success\n");
	}

	/* Test fmi1_capi_get_boolean */
	status = fmi1_capi_get_boolean(fmu, vrs, N_BOOLEAN, values);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_get_boolean:                   Failed\n");
		do_exit(CTEST_RETURN_FAIL);
	} else {
		for (k = 0; k < N_BOOLEAN; k++) {
			if (values_ref[k] != values[k]) {
				printf("fmi1_capi_get_boolean returned values[%u] = \"%s\" expected \"%s\"\n", (unsigned)k, values[k] ? "fmiTrue" : "fmiFalse", values_ref[k] ? "fmiTrue" : "fmiFalse");
				do_exit(CTEST_RETURN_FAIL);
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
		do_exit(CTEST_RETURN_FAIL);
	} else {
		printf("fmi1_capi_set_real:                      Success\n");
	}

	/* Test fmi1_capi_get_real */
	status = fmi1_capi_get_real(fmu, vrs, N_REAL, values);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_get_real:                      Failed\n");
		do_exit(CTEST_RETURN_FAIL);
	} else {
		for (k = 0; k < N_REAL; k++) {
			if (values_ref[k] != values[k]) {
				printf("fmi1_capi_get_real returned values[%u] = \"%f\" expected \"%f\"\n", (unsigned)k, values[k], values_ref[k]);
				do_exit(CTEST_RETURN_FAIL);
			}
		}
		printf("fmi1_capi_get_real:                      Success\n");
	}

	return 0;
}

/**
 * \brief Tests fmi1_capi_reset_slave
 * 
 */
int test_reset_slave()
{
        fmi1_status_t status;
	status = fmi1_capi_reset_slave(fmu);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_reset_slave:                   Failed\n");
		do_exit(CTEST_RETURN_FAIL);
	} else {
		printf("fmi1_capi_reset_slave:                   Success\n");
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_set_real_input_derivatives
 * fmi1_capi_set_real_input_derivatives returns fmiError if wrong values are set. The values that are set are tested inside the DLL.
 */
int test_set_real_input_derivatives()
{
        fmi1_status_t status;
	fmi1_value_reference_t	vrs		[N_INPUT_REAL * N_INPUT_REAL_MAX_ORDER];
	fmi1_real_t				values	[N_INPUT_REAL * N_INPUT_REAL_MAX_ORDER];
	fmi1_integer_t			order	[N_INPUT_REAL * N_INPUT_REAL_MAX_ORDER];
	int k, p;

	for (k = 0; k < N_INPUT_REAL_MAX_ORDER; k++) {
		for (p = 0; p < N_INPUT_REAL; p++)
		{
			vrs		[p + k * N_INPUT_REAL] = (fmi1_value_reference_t)p;
			values	[p + k * N_INPUT_REAL] = MAGIC_TEST_VALUE; /* This value is tested in the DLL to be exakt MAGIC_TEST_VALUE */
			order	[p + k * N_INPUT_REAL] = k + 1;
		}
	}

	status = fmi1_capi_set_real_input_derivatives(fmu, vrs, N_INPUT_REAL * N_INPUT_REAL_MAX_ORDER, order, values);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_set_real_input_derivatives:    Failed\n");
		do_exit(CTEST_RETURN_FAIL);
	} else {
		printf("fmi1_capi_set_real_input_derivatives:    Success\n");
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_get_real_output_derivatives
 * The output values from fmi1_capi_get_real_output_derivatives is expected to have a special value that is coded in the DLL. 
 */
int test_get_real_output_derivatives()
{
        fmi1_status_t status;
	fmi1_value_reference_t	vrs		[N_OUTPUT_REAL * N_OUTPUT_REAL_MAX_ORDER];
	fmi1_real_t				values	[N_OUTPUT_REAL * N_OUTPUT_REAL_MAX_ORDER];
	fmi1_integer_t			order	[N_OUTPUT_REAL * N_OUTPUT_REAL_MAX_ORDER];
	size_t k, p;

	for (k = 0; k < N_OUTPUT_REAL_MAX_ORDER; k++) {
		for (p = 0; p < N_OUTPUT_REAL; p++)
		{
			vrs		[p + k * N_OUTPUT_REAL] = (fmi1_value_reference_t)p;
			values	[p + k * N_OUTPUT_REAL] = -1;
			order	[p + k * N_OUTPUT_REAL] = (fmi1_integer_t)k + 1;
		}
	}

	status = fmi1_capi_get_real_output_derivatives(fmu, vrs, N_INPUT_REAL * N_INPUT_REAL_MAX_ORDER, order, values);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_get_real_output_derivatives:   Failed\n");
		do_exit(CTEST_RETURN_FAIL);
	} else {
		printf("fmi1_capi_get_real_output_derivatives:   Success\n");

		for (k = 0; k < N_OUTPUT_REAL_MAX_ORDER; k++) {
			for (p = 0; p < N_OUTPUT_REAL; p++)
			{
				fmi1_real_t value = values[p + k * N_OUTPUT_REAL];
				if (value != MAGIC_TEST_VALUE) { /* This value is set in DLL to be exakt MAGIC_TEST_VALUE */
					printf("fmi1_capi_get_real_output_derivatives:   Failed\n");
					do_exit(CTEST_RETURN_FAIL);
				}

#ifdef PRINT_VERBOSE
				fmi1_integer_t ord = k + 1;
				printf("value[%d][%d] = %f \n", p, ord, value);
#endif
			}
		}
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_terminate_slave
 *
 */
int test_terminate_slave()
{
        fmi1_status_t status;
	status = fmi1_capi_terminate_slave(fmu);
	if (status == fmi1_status_error || status == fmi1_status_fatal) {
		printf("fmi1_capi_terminate_slave:               Failed\n");
		do_exit(CTEST_RETURN_FAIL);
	} else {
		printf("fmi1_capi_terminate_slave:               Success\n");
	}
	return 0;
}

/**
 * \brief Tests fmi1_capi_free_slave_instance
 *
 */
int test_free_slave_instance()
{
	fmi1_capi_free_slave_instance(fmu);
	printf("fmi1_capi_free_slave_instance:           Success\n");		
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
 * \brief Tests the C-API for FMI 1.0 Co-Simulation.
 * The tests are performed using a test-dll. The functions are called and the values are set or returned are validated either in the test function(output functions) or inside the dll(input functions). If any error occures, the program exits. 
 *
 */
int main(int argc, char *argv[])
{
	/* Test CAPI constructor functions */
	test_create_dllfmu();
	test_load_dll();
	test_load_dll_fcn();

	/* FMI CS 1.0 functions */
	test_instantiate_slave();
	test_get_types_platform();	
	test_initialize_slave();
	test_cancel_step();
	test_do_step();
	test_get_status();
	test_get_real_status();
	test_get_integer_status();
	test_get_boolean_status();
	test_get_string_status();
	test_reset_slave();
	test_set_real_input_derivatives();
	test_get_real_output_derivatives();

	test_fmi_get_version();
	test_set_get_string();
	test_set_get_boolean();
	test_set_get_integer();
	test_set_get_real();
	test_set_debug_logging();

	test_terminate_slave();
	test_free_slave_instance();

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


