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

#include <config_test.h>
#include <fmilib.h>
#include <JM/jm_portability.h>

#define BUFFER 1000

/* Logger function used by the FMU internally */
static void fmi2logger(fmi2_component_environment_t env, fmi2_string_t instanceName, fmi2_status_t status, fmi2_string_t category, fmi2_string_t message, ...)
{
    int len;
	char msg[BUFFER];
	va_list argp;	
	va_start(argp, message);
	len = jm_vsnprintf(msg, BUFFER, message, argp);
	printf("fmiStatus = %s;  %s (%s): %s\n", fmi2_status_to_string(status), instanceName, category, msg);
}

static void stepFinished(fmi2_component_environment_t env, fmi2_status_t status) {
	printf("stepFinished is called wiht fmiStatus = %s\n", fmi2_status_to_string(status));
}
	   
int fmi2_test(fmi_import_context_t* context, const char* dirPath)
{
	fmi2_callback_functions_t callBackFunctions;
	const char* modelIdentifier;
	const char* modelName;
	const char*  GUID;
	jm_status_enu_t status;

	fmi2_import_t* fmu;	
	fmi2_fmu_kind_enu_t fmukind;

	callBackFunctions.logger = fmi2logger;
	callBackFunctions.allocateMemory = calloc;
	callBackFunctions.freeMemory = free;
	callBackFunctions.stepFinished = stepFinished;
	callBackFunctions.componentEnvironment = 0;

	fmu = fmi2_import_parse_xml(context, dirPath, 0);

	if(!fmu) {
		printf("Error parsing XML, exiting\n");
		return (CTEST_RETURN_FAIL);
	}
	modelName = fmi2_import_get_model_name(fmu);
	GUID = fmi2_import_get_GUID(fmu);

	printf("Model name: %s\n", modelName);
	if(fmi2_import_get_fmu_kind(fmu) != fmi2_fmu_kind_cs) {
		modelIdentifier = fmi2_import_get_model_identifier_ME(fmu);
		printf("Model identifier for ME: %s\n", modelIdentifier);
		fmukind = fmi2_fmu_kind_me;
	}
	else if(fmi2_import_get_fmu_kind(fmu) != fmi2_fmu_kind_me) {
		modelIdentifier = fmi2_import_get_model_identifier_CS(fmu);
		printf("Model identifier for CS: %s\n", modelIdentifier);
		fmukind = fmi2_fmu_kind_cs;
	}
	else {
		printf("Unxepected FMU kind, exiting\n");
		return (CTEST_RETURN_FAIL);
	}
    printf("Model GUID: %s\n", GUID);

	status = fmi2_import_create_dllfmu(fmu, fmukind, &callBackFunctions);
	if (status == jm_status_error) {
		printf("Could not create the DLL loading mechanism(C-API).\n");
		return(CTEST_RETURN_FAIL);
	}

	printf("Version returned from FMU:   %s\n", fmi2_import_get_version(fmu));

	fmi2_import_destroy_dllfmu(fmu);

	fmi2_import_free(fmu);

	return (CTEST_RETURN_SUCCESS);
}


