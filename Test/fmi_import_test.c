/*
    Copyright (C) 2012 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <Common/fmi_import_context.h>
#include <FMI1/fmi1_types.h>
#include <FMI1/fmi1_enums.h>
#include <FMI1/fmi1_functions.h>
#include <FMI1/fmi1_import.h>
#include "config_fmilib.h"

#define BUFFER 1000

void importlogger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message)
{
        printf("module = %s, log level = %d: %s\n", module, log_level, message);
}

/* Logger function used by the FMU internally */
void fmilogger(fmi1_component_t c, fmi1_string_t instanceName, fmi1_status_t status, fmi1_string_t category, fmi1_string_t message, ...)
{
	char msg[BUFFER];
	va_list argp;	
	va_start(argp, message);
	vsprintf(msg, message, argp);
	printf("fmiStatus = %d;  %s (%s): %s\n", status, instanceName, category, msg);
}

void do_exit(int code)
{
	printf("Press 'Enter' to exit\n");
	/* getchar(); */
	exit(code);
}
	   
int main(int argc, char *argv[])
{
	fmi1_callback_functions_t callBackFunctions;
	const char* FMUPath;
	const char* tmpPath;
	const char* modelIdentifier;
	const char* modelName;
	const char*  GUID;
	jm_callbacks callbacks;
	fmi_import_context_t* context;
	fmi_version_enu_t version;
	jm_status_enu_t status;

	fmi1_import_t* fmu;	

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
    callbacks.logger = importlogger;
    callbacks.context = 0;

	callBackFunctions.logger = fmilogger;
	callBackFunctions.allocateMemory = calloc;
	callBackFunctions.freeMemory = free;


	context = fmi_import_allocate_context(&callbacks);

	version = fmi_import_get_fmi_version(context, FMUPath, tmpPath);

	if(version != fmi_version_1_enu) {
		printf("Only version 1.0 is supported so far\n");
		do_exit(CTEST_RETURN_FAIL);
	}

	fmu = fmi1_import_parse_xml(context, tmpPath);

	if(!fmu) {
		printf("Error parsing XML, exiting\n");
		do_exit(CTEST_RETURN_FAIL);
	}
	modelIdentifier = fmi1_import_get_model_identifier(fmu);
	modelName = fmi1_import_get_model_name(fmu);
	GUID = fmi1_import_get_GUID(fmu);

	printf("Model name: %s\n", modelName);
    printf("Model identifier: %s\n", modelIdentifier);
    printf("Model GUID: %s\n", GUID);

	

	status = fmi1_import_create_dllfmu(fmu, callBackFunctions);
	if (status == jm_status_error) {
		printf("Could not create the DLL loading mechanism(C-API).\n");
		do_exit(CTEST_RETURN_FAIL);
	}

	printf("Version returned from FMU:   %s\n", fmi1_import_get_version(fmu));

	fmi1_import_destroy_dllfmu(fmu);

	fmi1_import_free(fmu);
	fmi_import_free_context(context);
	
	printf("Everything seems to be OK since you got this far=)!\n");

	do_exit(CTEST_RETURN_SUCCESS);
}


