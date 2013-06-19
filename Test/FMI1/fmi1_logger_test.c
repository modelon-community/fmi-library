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
#include <assert.h>

#include "config_test.h"
#include <fmilib.h>
#include <fmu_dummy/fmu1_model_defines.h>
#include <JM/jm_portability.h>

FILE* logFile;

void logger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message) {
	char buf[10000];
	const char* loadingstr = "Loading '";
	/* need to replace platform specific message with a standard one */
	if( (log_level == jm_log_level_info) && 
		(strcmp(module, "FMILIB") == 0) &&
		(strncmp(message, "Loading '", strlen(loadingstr)) == 0)
		)
	{
		jm_snprintf(buf, 10000, "[INFO][FMILIB] Loading '-----' binary with '------' platform types\n");
	}
	else
		jm_snprintf(buf, 10000, "[%s][%s] %s\n", jm_log_level_to_string(log_level), module, message);
	printf("%s", buf);
	fprintf(logFile, "%s", buf);
}

void do_exit(int code)
{
	/* getchar();*/
	exit(code);
}
	   
int test_logger(fmi1_import_t* fmu)
{	
	fmi1_status_t fmistatus; 
	jm_status_enu_t jmstatus;	

	jmstatus = fmi1_import_instantiate_model(fmu, "LoggerTesting");
	if (jmstatus == jm_status_error) {
		printf("fmi1_import_instantiate_model failed\n");
		do_exit(CTEST_RETURN_FAIL);
	}

	/* Logger message to print: */
	{
		fmi1_value_reference_t* vr;
		size_t n;
		size_t k;
		const char* str[] = {
			"###### Reals ######",
			"OK HIGHT = #r0#", /* HIGHT */
			"OK HIGHT_SPEED = #r1#", /* HIGHT_SPEED */
			"OK GRAVITY = #r2#", /* GRAVITY */
			"OK BOUNCE_COF = #r3#", /* BOUNCE_COF */
			"Bad reference #r0",
			"Bad reference #r-0",
			"Bad reference #r-1",
			"Bad reference #r100#",
			"###### Integers ######",
			"OK LOGGER_TEST_INTEGER = #i0#", /* LOGGER_TEST_INTEGER */			
			"Bad reference #i0",
			"Bad reference #i-0",
			"Bad reference #i-1",
			"Bad reference #i100#",
			"###### Booleans ######",
			"OK LOGGER_TEST_BOOLEAN = #b0#", /* LOGGER_TEST_BOOLEAN */	
			"Bad reference #b0",
			"Bad reference #b-0",
			"Bad reference #b-1",
			"Bad reference #b100#",
			"###### Strings ######",
			"OK LOGGER_TEST = #s0#", /* LOGGER_TEST */
			"Bad reference #s0",
			"Bad reference #s-0",
			"Bad reference #s-1",
			"Bad reference #s100#"
		};

		n = sizeof(str)/sizeof(*str);	
		vr = calloc(n, sizeof(fmi1_value_reference_t));
		for (k = 0; k < n; k++) {
			vr[k] = VAR_S_LOGGER_TEST;
		}

		fmistatus = fmi1_import_set_string(fmu, vr, n, str);
        if(fmistatus != fmi1_status_ok) {
            abort();
        }		

		{ /* Print a really big message */

#define MESSAGE_SIZE_TO_EXPAND_AND_PRINT 3000 /* Using fixed size since the log message is printed to a file and compared */
#if JM_MAX_ERROR_MESSAGE_SIZE + 200 > MESSAGE_SIZE_TO_EXPAND_AND_PRINT
#error This test triggers the logger function to allocate more memory than the default size JM_MAX_ERROR_MESSAGE_SIZE. If you change JM_MAX_ERROR_MESSAGE_SIZE, please update this test.
#endif
			char longmessage[MESSAGE_SIZE_TO_EXPAND_AND_PRINT];
			const char* str[1];

			str[0] = (const char*)&longmessage;
			{
				fmi1_value_reference_t vr = VAR_S_LOGGER_TEST;
				int k;
				char repmsg[] = "#r0# "; /* HIGHT */
				for (k = 0; k < sizeof(longmessage)/sizeof(*longmessage) - 1; k++) {
					longmessage[k] = repmsg[k%(sizeof(repmsg)/sizeof(*repmsg) - 1)];					
				}
				longmessage[k] = '\0';
				
				fmistatus = fmi1_import_set_string(fmu, &vr, 1, str);
				if(fmistatus != fmi1_status_ok) {
					abort();
				}
			}
		}

		free(vr);
	}


	fmi1_import_free_model_instance(fmu);
	return 0;
}

int main(int argc, char *argv[])
{
	fmi1_callback_functions_t callBackFunctions;
	const char* FMUPath;
	const char* tmpPath;
	jm_callbacks callbacks;
	fmi_import_context_t* context;
	fmi_version_enu_t version;
	jm_status_enu_t status;
	int register_active_fmu;
	fmi1_import_t* fmu;	
	char* outfile;

	if(argc < 4) {
		printf("Usage: %s <fmu_file> <temporary_dir> <output_file>\n", argv[0]);
		do_exit(CTEST_RETURN_FAIL);
	}

	FMUPath = argv[1];
	tmpPath = argv[2];
	outfile = argv[3];

	logFile = fopen(outfile, "wb");

	if(!logFile) {
		printf("Could not open output file %s\n", outfile);
		do_exit(CTEST_RETURN_FAIL);
	}

	callbacks.malloc = malloc;
    callbacks.calloc = calloc;
    callbacks.realloc = realloc;
    callbacks.free = free;
    callbacks.logger = logger;
	callbacks.log_level = jm_log_level_info;
    callbacks.context = 0;

	callBackFunctions.logger = fmi1_log_forwarding;
	callBackFunctions.allocateMemory = calloc;
	callBackFunctions.freeMemory = free;

#ifdef FMILIB_GENERATE_BUILD_STAMP
	printf("Library build stamp:\n%s\n", fmilib_get_build_stamp());
#endif

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

	register_active_fmu = 1; /* Must be used to use our logger. (jm standard prints to strerr which does not generate out put test file)  */
	status = fmi1_import_create_dllfmu(fmu, callBackFunctions, register_active_fmu);
	if (status == jm_status_error) {
		printf("Could not create the DLL loading mechanism(C-API test).\n");
		do_exit(CTEST_RETURN_FAIL);
	}
	
	test_logger(fmu);

	fmi1_import_destroy_dllfmu(fmu);

	fmi1_import_free(fmu);
	fmi_import_free_context(context);
	fclose(logFile);
	return 0;
}


