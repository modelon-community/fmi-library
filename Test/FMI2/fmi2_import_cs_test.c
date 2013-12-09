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

/* Logger function used by the FMU internally */

void fmilogger(fmi2_component_t c, fmi2_string_t instanceName, fmi2_status_t status, fmi2_string_t category, fmi2_string_t message, ...)
{
    /* int len;
	char msg[BUFFER]; */
	va_list argp;	
	va_start(argp, message);
	/* len = jm_vsnprintf(msg, BUFFER, message, argp); */
	fmi2_log_forwarding_v(c, instanceName, status, category, message, argp);
	va_end(argp);
}

void do_exit(int code)
{
	printf("Press 'Enter' to exit\n");
	/* getchar(); */
	exit(code);
}

int test_simulate_cs(fmi2_import_t* fmu)
{
	fmi2_status_t fmistatus;
	jm_status_enu_t jmstatus;

	fmi2_string_t instanceName = "Test CS model instance";
	fmi2_string_t fmuGUID;
	fmi2_string_t fmuLocation = "";
	fmi2_boolean_t visible = fmi2_false;
	fmi2_real_t relativeTol = 1e-4;
/*	fmi2_boolean_t loggingOn = fmi2_true; */
	
	/* fmi2_real_t simulation_results[] = {-0.001878, -1.722275}; */
	fmi2_real_t simulation_results[] = {0.0143633,   -1.62417};
	fmi2_value_reference_t compare_real_variables_vr[] = {0, 1};
	size_t k;

	fmi2_real_t tstart = 0.0;
	fmi2_real_t tcur = tstart;
	fmi2_real_t hstep = 0.1;
	fmi2_real_t tend = 2.0;
	fmi2_boolean_t StopTimeDefined = fmi2_false;

	if (sizeof(compare_real_variables_vr)/sizeof(fmi2_value_reference_t) != sizeof(simulation_results)/sizeof(fmi2_real_t)) {
		printf("Number of simulation values and reference values are different\n");
		do_exit(CTEST_RETURN_FAIL);
	}

	printf("Version returned from FMU:   %s\n", fmi2_import_get_version(fmu));
	printf("Platform type returned:      %s\n", fmi2_import_get_types_platform(fmu));

	fmuGUID = fmi2_import_get_GUID(fmu);
    printf("GUID:      %s\n", fmuGUID);


    jmstatus = fmi2_import_instantiate(fmu, instanceName, fmi2_cosimulation, fmuLocation, visible);
	if (jmstatus == jm_status_error) {
		printf("fmi2_import_instantiate failed\n");
		do_exit(CTEST_RETURN_FAIL);
	}

        fmistatus = fmi2_import_setup_experiment(fmu, fmi2_true,
            relativeTol, tstart, StopTimeDefined, tend);
    if(fmistatus != fmi2_status_ok) {
        printf("fmi2_import_setup_experiment failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }

        fmistatus = fmi2_import_enter_initialization_mode(fmu);
    if(fmistatus != fmi2_status_ok) {
        printf("fmi2_import_enter_initialization_mode failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }

        fmistatus = fmi2_import_exit_initialization_mode(fmu);
    if(fmistatus != fmi2_status_ok) {
        printf("fmi2_import_exit_initialization_mode failed\n");
        do_exit(CTEST_RETURN_FAIL);
    }        

	tcur = tstart;
	printf("%10s %10s\n", "Ball height", "Ball speed");
	while (tcur < tend) {
		fmi2_boolean_t newStep = fmi2_true;
#if 0 /* Prints a real value.. */
		fmi2_real_t rvalue;
		fmi2_value_reference_t vr = 0;

		fmistatus = fmi2_import_get_real(fmu, &vr, 1, &rvalue);
		printf("rvalue = %f\n", rvalue);
#endif 
		fmistatus = fmi2_import_do_step(fmu, tcur, hstep, newStep);

		for (k = 0; k < sizeof(compare_real_variables_vr)/sizeof(fmi2_value_reference_t); k++) {
			fmi2_value_reference_t vr = compare_real_variables_vr[k];
			fmi2_real_t rvalue;
			fmistatus = fmi2_import_get_real(fmu, &vr, 1, &rvalue);
		}
		{
			fmi2_real_t val[2];
			fmi2_import_get_real(fmu, compare_real_variables_vr, 2, val);
			printf("%10g %10g\n", val[0],val[1]);
		}

		tcur += hstep;
	}

	printf("Simulation finished. Checking results\n");

	/* Validate result */
	for (k = 0; k < sizeof(compare_real_variables_vr)/sizeof(fmi2_value_reference_t); k++) {
		fmi2_value_reference_t vr = compare_real_variables_vr[k];
		fmi2_real_t rvalue;
		fmi2_real_t res;	
		fmistatus = fmi2_import_get_real(fmu, &vr, 1, &rvalue);
		res = rvalue - simulation_results[k];
		res = res > 0 ? res: -res; /* Take abs */
		if (res > 3e-3) {
			printf("Simulation results is wrong!\n");
			printf("State [%u]  %g != %g, |res| = %g\n", (unsigned)k, rvalue, simulation_results[k], res);
			printf("\n");
			do_exit(CTEST_RETURN_FAIL);
		}
	}

	fmistatus = fmi2_import_terminate(fmu);

	fmi2_import_free_instance(fmu);

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
	int k;

	fmi2_import_t* fmu;	

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

	if(version != fmi_version_2_0_enu) {
		printf("The code only supports version 2.0\n");
		do_exit(CTEST_RETURN_FAIL);
	}

	fmu = fmi2_import_parse_xml(context, tmpPath, 0);

	if(!fmu) {
		printf("Error parsing XML, exiting\n");
		do_exit(CTEST_RETURN_FAIL);
	}
	
	if(fmi2_import_get_fmu_kind(fmu) == fmi2_fmu_kind_me) {
		printf("Only CS 2.0 is supported by this code\n");
		do_exit(CTEST_RETURN_FAIL);
	}

	callBackFunctions.logger = fmi2_log_forwarding;
	callBackFunctions.allocateMemory = calloc;
	callBackFunctions.freeMemory = free;
	callBackFunctions.componentEnvironment = fmu;

	status = fmi2_import_create_dllfmu(fmu, fmi2_fmu_kind_cs, &callBackFunctions);
	if (status == jm_status_error) {
		printf("Could not create the DLL loading mechanism(C-API) (error: %s).\n", fmi2_import_get_last_error(fmu));
		do_exit(CTEST_RETURN_FAIL);
	}

	test_simulate_cs(fmu);

	fmi2_import_destroy_dllfmu(fmu);

	fmi2_import_free(fmu);
	fmi_import_free_context(context);
	
	printf("Everything seems to be OK since you got this far=)!\n");

	do_exit(CTEST_RETURN_SUCCESS);

	return 0;
}


