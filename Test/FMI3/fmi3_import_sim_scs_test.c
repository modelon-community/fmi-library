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
#include <JM/jm_portability.h>

#define BUFFER 1000

/* globals */
fmi3_boolean_t G_dummy_callback_intermediate_update_called;


void importlogger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message)
{
        printf("module = %s, log level = %s: %s\n", module, jm_log_level_to_string(log_level), message);
}

/* function implementing fmi3_callback_intermediate_update_ft */
void dummy_callback_intermediate_update(
        fmi3_instance_environment_t instanceEnvironment,
        fmi3_float64_t intermediateUpdateTime,
        fmi3_boolean_t eventOccurred,
        fmi3_boolean_t clocksTicked,
        fmi3_boolean_t intermediateVariableSetAllowed,
        fmi3_boolean_t intermediateVariableGetAllowed,
        fmi3_boolean_t intermediateStepFinished,
        fmi3_boolean_t canReturnEarly,
        fmi3_boolean_t* earlyReturnRequested,
        fmi3_float64_t* earlyReturnTime)
{
    G_dummy_callback_intermediate_update_called = fmi3_true;
}

/* function implementing fmi3_callback_lock_preemption_ft */
void dummy_lock_preemption(void)
{
    return; /* NYI */
}

/* function implementing fmi3_callback_unlock_preemption_ft */
void dummy_unlock_preemption(void)
{
    return; /* NYI */
}

void do_exit(int code)
{
	printf("Press 'Enter' to exit\n");
	/* getchar(); */
	exit(code);
}

/* this function also tests that the intermediate update callback is called */
void test_capi_activate_model_partition(fmi3_import_t* fmu)
{
	jm_status_enu_t jmstatus;

    /* reset value that will be set by intermedate update callback */
    G_dummy_callback_intermediate_update_called = fmi3_false;

	jmstatus = fmi3_import_activate_model_partition(fmu, 0, 0, 0);
	if (jmstatus == jm_status_error) {
		printf("fmi3_import_activate_model_partition failed\n");
		do_exit(CTEST_RETURN_FAIL);
	}
	if (G_dummy_callback_intermediate_update_called != fmi3_true) {
		printf("fmi3CallbackIntermediateUpdate test failed\n");
		do_exit(CTEST_RETURN_FAIL);
	}
}

int test_capi_wrappers_scs(fmi3_import_t * fmu)
{
	fmi3_status_t fmistatus;
	jm_status_enu_t jmstatus;

	fmi3_string_t instanceName = "Test HCS model instance";
	fmi3_string_t fmuInstantiationToken;
	fmi3_string_t resourceLocation = "";
	fmi3_boolean_t visible = fmi3_false;
    fmi3_boolean_t intermediateVariableGetRequired = fmi3_false;
    fmi3_boolean_t intermediateInternalVariableGetRequired = fmi3_false;
    fmi3_boolean_t intermediateVariableSetRequired = fmi3_false;

	printf("Version returned from FMU:   %s\n", fmi3_import_get_version(fmu));

	fmuInstantiationToken = fmi3_import_get_instantiation_token(fmu);
	printf("instantiationToken:      %s\n", fmuInstantiationToken);

	jmstatus = fmi3_import_instantiate_scheduled_co_simulation(
            fmu,
            instanceName,
            resourceLocation,
            visible,
            intermediateVariableGetRequired,
            intermediateInternalVariableGetRequired,
            intermediateVariableSetRequired,
            &dummy_callback_intermediate_update,
			&dummy_lock_preemption,
			&dummy_unlock_preemption);
	if (jmstatus == jm_status_error) {
		printf("fmi3_import_instantiate_scheduled_co_simulation failed\n");
		do_exit(CTEST_RETURN_FAIL);
	}

    /* verify that the CAPI wrappers are loaded and have matching signatures */
    test_capi_activate_model_partition(fmu);

    /* clean up */
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

void fmi3_test_log_forwarding_wrap(fmi3_instance_environment_t instEnv, fmi3_string_t instanceName, fmi3_status_t status,
		fmi3_string_t category, fmi3_string_t message) {
	fmi3_log_forwarding(((fmi3_inst_env_count_calls_t*)instEnv)->fmu, instanceName, status, category, message);
}

/************
 *** main ***
 ************/

int main(int argc, char *argv[])
{
	fmi3_inst_env_count_calls_t instEnv;
	const char* FMUPath;
	const char* tmpPath;
	jm_callbacks callbacks;
	fmi_import_context_t* context;
	fmi_version_enu_t version;
	jm_status_enu_t status;
	int k;

	fmi3_import_t* fmu;	

	if(argc < 3) {
		printf("Usage: %s <fmu_file> <temporary_dir>\n", argv[0]);
		do_exit(CTEST_RETURN_FAIL);
	} 
	for (k = 0; k < argc; k ++)
		printf("argv[%d] = %s\n", k, argv[k]);

	FMUPath = argv[1];
	tmpPath = argv[2];


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

	context = fmi_import_allocate_context(&callbacks);

	version = fmi_import_get_fmi_version(context, FMUPath, tmpPath);
	if(version != fmi_version_3_0_enu) {
		printf("The code only supports version 3.0\n");
		do_exit(CTEST_RETURN_FAIL);
	}

	fmu = fmi3_import_parse_xml(context, tmpPath, 0);
	if(!fmu) {
		printf("Error parsing XML, exiting\n");
		do_exit(CTEST_RETURN_FAIL);
	}
	
    /* TODO: update when parsing of SCS element is available */
	if(fmi3_import_get_fmu_kind(fmu) == fmi3_fmu_kind_me) {
		printf("Only CS 3.0 is supported by this code\n");
		do_exit(CTEST_RETURN_FAIL);
	}

	/* Test custom callbacks - .._me_test tests default */
	instEnv.fmu	= fmu;

	status = fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_scs, &instEnv, fmi3_test_log_forwarding_wrap);
	if (status == jm_status_error) {
		printf("Could not create the DLL loading mechanism(C-API) (error: %s).\n", fmi3_import_get_last_error(fmu));
		do_exit(CTEST_RETURN_FAIL);
	}

	test_capi_wrappers_scs(fmu);

	/* TODO: add simulation */

	fmi3_import_destroy_dllfmu(fmu);

	fmi3_import_free(fmu);
	fmi_import_free_context(context);
	
	printf("Everything seems to be OK since you got this far=)!\n");

	do_exit(CTEST_RETURN_SUCCESS);

	return 0;
}

