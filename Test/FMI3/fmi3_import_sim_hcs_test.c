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

void importlogger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message)
{
        printf("module = %s, log level = %s: %s\n", module, jm_log_level_to_string(log_level), message);
}

/* function implementing fmi3_callback_intermediate_update_ft */
fmi3_status_t dummy_callback_intermediate_update(
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
    return fmi3_status_ok; /* NYI */
}

void do_exit(int code)
{
	printf("Press 'Enter' to exit\n");
	/* getchar(); */
	exit(code);
}

/* Dummy struct used to represent binary value for set/get-binary API */
typedef struct {
	char ch;
	double dbl;
} dummy_binary_t;

int dummy_binary_equals(dummy_binary_t* b1, dummy_binary_t* b2)
{
	return b1->ch == b2->ch && b1->dbl == b2->dbl;
}


void test_capi_set_get_internal_fmu_state(fmi3_import_t* fmu)
{
	jm_status_enu_t jmstatus;

	jmstatus = fmi3_import_get_fmu_state(fmu, NULL);
	if (jmstatus == jm_status_error) {
		printf("fmi3_import_get_fmu_state failed\n");
		do_exit(CTEST_RETURN_FAIL);
	}

	jmstatus = fmi3_import_set_fmu_state(fmu, NULL);
	if (jmstatus == jm_status_error) {
		printf("fmi3_import_set_fmu_state failed\n");
		do_exit(CTEST_RETURN_FAIL);
	}

	jmstatus = fmi3_import_free_fmu_state(fmu, NULL);
	if (jmstatus == jm_status_error) {
		printf("fmi3_import_free_fmu_state failed\n");
		do_exit(CTEST_RETURN_FAIL);
	}

	jmstatus = fmi3_import_serialized_fmu_state_size(fmu, NULL, NULL);
	if (jmstatus == jm_status_error) {
		printf("fmi3_import_serialized_fmu_state_size failed\n");
		do_exit(CTEST_RETURN_FAIL);
	}

	jmstatus = fmi3_import_serialize_fmu_state(fmu, NULL, NULL, 0);
	if (jmstatus == jm_status_error) {
		printf("fmi3_import_serialize_fmu_state failed\n");
		do_exit(CTEST_RETURN_FAIL);
	}

	jmstatus = fmi3_import_de_serialize_fmu_state(fmu, NULL, 0, NULL);
	if (jmstatus == jm_status_error) {
		printf("fmi3_import_de_serialize_fmu_state failed\n");
		do_exit(CTEST_RETURN_FAIL);
	}
}

/* check that fmi3EnterEventMode is loaded for CS FMUs */
void test_capi_enter_event_mode(fmi3_import_t* fmu)
{
	jm_status_enu_t jmstatus;

	jmstatus = fmi3_import_enter_event_mode(
            fmu,
            fmi3_false,
            fmi3_false,
            NULL,
            0,
            fmi3_false);
	if (jmstatus == jm_status_error) {
		printf("fmi3_import_enter_event_mode failed\n");
		do_exit(CTEST_RETURN_FAIL);
	}
}

/* check get and set binary (not checking start value - XML parsing NYI) */
void test_capi_get_set_binary(fmi3_import_t* fmu)
{
	jm_status_enu_t jmstatus;
    const fmi3_value_reference_t vrs[] = {0}; /* VRs are not yet global, so just match FMU buffer index */
    size_t nvr = sizeof(vrs)/sizeof(fmi3_value_reference_t);
    dummy_binary_t binary_set;
    dummy_binary_t* binary_get;
    fmi3_binary_t values_set[] = { (char*)&binary_set };
    fmi3_binary_t values_get[1];
    size_t sizes_set[] = { sizeof(dummy_binary_t) };
    size_t sizes_get[1];
    size_t nValues = 1;

    binary_set.ch = 'a';
    binary_set.dbl = 1.0;

    jmstatus = fmi3_import_set_binary(fmu, vrs, nvr, sizes_set, values_set, nValues);
    if (jmstatus == jm_status_error) {
        printf("fmi3_import_set_binary failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    jmstatus = fmi3_import_get_binary(fmu, vrs, nvr, sizes_get, values_get, nValues);
    if (jmstatus == jm_status_error) {
        printf("fmi3_import_get_binary failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    /* Verify that we get same values as we set */
    binary_get = (dummy_binary_t*)values_get[0];
    if (sizes_get[0] != sizes_set[0] || !dummy_binary_equals(&binary_set, binary_get)) {
        printf("incorrect values from fmi3_import_get_binary\n");
        do_exit(CTEST_RETURN_FAIL);
    }
}

/* check that partial derivatives API work */
void test_capi_partial_derivatives(fmi3_import_t* fmu)
{
	jm_status_enu_t jmstatus;

    jmstatus = fmi3_import_get_directional_derivative(
            fmu,
            NULL, 0,
            NULL, 0,
            NULL, 0,
            NULL, 0);
    if (jmstatus == jm_status_error) {
        printf("fmi3_import_get_directional_derivative failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    jmstatus = fmi3_import_get_adjoint_derivative(
            fmu,
            NULL, 0,
            NULL, 0,
            NULL, 0,
            NULL, 0);
    if (jmstatus == jm_status_error) {
        printf("fmi3_import_get_adjoint_derivative failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }
}

void test_capi_enter_exit_config_mode(fmi3_import_t* fmu)
{
	jm_status_enu_t jmstatus;

    jmstatus = fmi3_import_enter_configuration_mode(fmu);
    if (jmstatus == jm_status_error) {
        printf("fmi3_import_enter_configuration_mode failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    jmstatus = fmi3_import_exit_configuration_mode(fmu);
    if (jmstatus == jm_status_error) {
        printf("fmi3_import_exit_configuration_mode failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }
}

void test_capi_clock_api(fmi3_import_t* fmu)
{
	jm_status_enu_t jmstatus;

    jmstatus = fmi3_import_get_clock(fmu, NULL, 0, NULL, 0);
    if (jmstatus == jm_status_error) {
        printf("fmi3_import_get_clock failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    jmstatus = fmi3_import_set_clock(fmu, NULL, 0, NULL, NULL, 0);
    if (jmstatus == jm_status_error) {
        printf("fmi3_import_set_clock failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    jmstatus = fmi3_import_get_interval_decimal(fmu, NULL, 0, NULL, 0);
    if (jmstatus == jm_status_error) {
        printf("fmi3_import_get_interval_decimal failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    jmstatus = fmi3_import_get_interval_fraction(fmu, NULL, 0, NULL, NULL, 0);
    if (jmstatus == jm_status_error) {
        printf("fmi3_import_get_interval_decimal failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    jmstatus = fmi3_import_set_interval_decimal(fmu, NULL, 0, NULL, 0);
    if (jmstatus == jm_status_error) {
        printf("fmi3_import_set_interval_decimal failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    jmstatus = fmi3_import_set_interval_fraction(fmu, NULL, 0, NULL, NULL, 0);
    if (jmstatus == jm_status_error) {
        printf("fmi3_import_set_interval_fraction failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }
}

/* check that NewDiscreteStates is loaded for CS as well */
void test_capi_new_discrete_states(fmi3_import_t* fmu)
{
	jm_status_enu_t jmstatus;
    fmi3_boolean_t newDiscreteStatesNeeded;
    fmi3_boolean_t terminateSimulation;
    fmi3_boolean_t nominalsOfContinuousStatesChanged;
    fmi3_boolean_t valuesOfContinuousStatesChanged;
    fmi3_boolean_t nextEventTimeDefined;
    fmi3_float64_t nextEventTime;

    jmstatus = fmi3_import_new_discrete_states(
            fmu,
            &newDiscreteStatesNeeded,
            &terminateSimulation,
            &nominalsOfContinuousStatesChanged,
            &valuesOfContinuousStatesChanged,
            &nextEventTimeDefined,
            &nextEventTime);
    if (jmstatus == jm_status_error) {
        printf("fmi3_import_new_discrete_states failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }
}

/* check that NewDiscreteStates is loaded for CS as well */
void test_capi_cs_specific_api(fmi3_import_t* fmu)
{
	jm_status_enu_t jmstatus;

    jmstatus = fmi3_import_enter_step_mode(fmu);
    if (jmstatus == jm_status_error) {
        printf("fmi3_import_enter_step_mode failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    jmstatus = fmi3_import_get_output_derivatives(fmu, NULL, 0, NULL, NULL, 0);
    if (jmstatus == jm_status_error) {
        printf("fmi3_import_get_output_derivatives failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }
}

/* Check that CAPI is loaded and can be called - not so much that the FMU
 * handles it correctly (only that API is exported) */
int test_capi_wrappers_hcs(fmi3_import_t* fmu)
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


	jmstatus = fmi3_import_instantiate_hybrid_co_simulation(
            fmu,
            instanceName,
            resourceLocation,
            visible,
            intermediateVariableGetRequired,
            intermediateInternalVariableGetRequired,
            intermediateVariableSetRequired,
            &dummy_callback_intermediate_update);
	if (jmstatus == jm_status_error) {
		printf("fmi3_import_instantiate_hybrid_co_simulation failed\n");
		do_exit(CTEST_RETURN_FAIL);
	}

    test_capi_set_get_internal_fmu_state(fmu);
    test_capi_enter_event_mode(fmu);
    test_capi_get_set_binary(fmu);
    test_capi_partial_derivatives(fmu);
    test_capi_enter_exit_config_mode(fmu);
    test_capi_clock_api(fmu);
    test_capi_new_discrete_states(fmu);
    test_capi_cs_specific_api(fmu);

	/* TODO: add simulation */

	fmistatus = fmi3_import_terminate(fmu);
	if (fmistatus != fmi3_status_ok) {
		printf("fmi3_import_terminate failed\n");
		do_exit(CTEST_RETURN_FAIL);
	}

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
	
    /* TODO: update when parsing of HCS element is available */
	if(fmi3_import_get_fmu_kind(fmu) == fmi3_fmu_kind_me) {
		printf("Only CS 3.0 is supported by this code\n");
		do_exit(CTEST_RETURN_FAIL);
	}

	/* Test custom callbacks - .._me_test tests default */
	instEnv.fmu	= fmu;

	status = fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_hcs, &instEnv, fmi3_test_log_forwarding_wrap);
	if (status == jm_status_error) {
		printf("Could not create the DLL loading mechanism(C-API) (error: %s).\n", fmi3_import_get_last_error(fmu));
		do_exit(CTEST_RETURN_FAIL);
	}

	test_capi_wrappers_hcs(fmu);

	fmi3_import_destroy_dllfmu(fmu);

	fmi3_import_free(fmu);
	fmi_import_free_context(context);
	
	printf("Everything seems to be OK since you got this far=)!\n");

	do_exit(CTEST_RETURN_SUCCESS);

	return 0;
}

