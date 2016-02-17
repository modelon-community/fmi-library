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

#include "config_test.h"

#include <fmilib.h>


#define BUFFER 1000

void importlogger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message)
{
        printf("module = %s, log level = %s: %s\n", module, jm_log_level_to_string(log_level), message);
}

/* Logger function used by the FMU internally */

void fmilogger(fmi1_component_t c, fmi1_string_t instanceName, fmi1_status_t status, fmi1_string_t category, fmi1_string_t message, ...)
{
	/* char msg[BUFFER];*/
	va_list argp;
	va_start(argp, message);
	/* vsnprintf(msg, BUFFER, message, argp); */
	fmi1_log_forwarding_v(c, instanceName, status, category, message, argp);
	va_end(argp);
}

void do_exit(int code)
{
	printf("Press 'Enter' to exit\n");
	/* getchar(); */
	exit(code);
}

/* Testing "...Test\FMI1\fmu_dummy\modelDescription_cs_tc.xml" */
void test_xml_modelDescription_cs_tc(const char* xmlFileName, fmi1_import_t* fmu)
{
    fmi1_import_variable_t* v;

    { /* Test variable "INTEGER" */
        fmi1_import_integer_variable_t* vi;
        const char* name = "INTEGER";
        printf("Testing ScalarVariable %s\n", name);
        v = fmi1_import_get_variable_by_name(fmu, name);
        vi = fmi1_import_get_variable_as_integer(v);

        /* Test min attr */
        if (1 != fmi1_import_get_integer_variable_min(vi)) {
            printf("Test of XML file \"%s\" failed. min attribute value missmatch.\n", xmlFileName);
            do_exit(CTEST_RETURN_FAIL);
        }

        /* Test max attr */
        if (11 != fmi1_import_get_integer_variable_max(vi)) {
            printf("Test of XML file \"%s\" failed. max attribute value missmatch.\n", xmlFileName);
            do_exit(CTEST_RETURN_FAIL);
        }
    }

    { /* Test variable "INTEGER_DECLAREDTYPE" */
        fmi1_import_integer_variable_t* vi;
        const char* name = "INTEGER_DECLAREDTYPE";
        printf("Testing ScalarVariable %s\n", name);
        v = fmi1_import_get_variable_by_name(fmu, name);
        vi = fmi1_import_get_variable_as_integer(v);

        /* Test min attr */
        if (2 != fmi1_import_get_integer_variable_min(vi)) {
            printf("Test of XML file \"%s\" failed. min attribute value missmatch.\n", xmlFileName);
            do_exit(CTEST_RETURN_FAIL);
        }

        /* Test max attr */
        if (22 != fmi1_import_get_integer_variable_max(vi)) {
            printf("Test of XML file \"%s\" failed. max attribute value missmatch.\n", xmlFileName);
            do_exit(CTEST_RETURN_FAIL);
        }
    }

    { /* Test variable "INTEGER_DECLAREDTYPE_OVERWRITE" */
        fmi1_import_integer_variable_t* vi;
        const char* name = "INTEGER_DECLAREDTYPE_OVERWRITE";
        printf("Testing ScalarVariable %s\n", name);
        v = fmi1_import_get_variable_by_name(fmu, name);
        vi = fmi1_import_get_variable_as_integer(v);

        /* Test min attr */
        if (1 != fmi1_import_get_integer_variable_min(vi)) {
            printf("Test of XML file \"%s\" failed. min attribute value missmatch.\n", xmlFileName);
            do_exit(CTEST_RETURN_FAIL);
        }

        /* Test max attr */
        if (11 != fmi1_import_get_integer_variable_max(vi)) {
            printf("Test of XML file \"%s\" failed. max attribute value missmatch.\n", xmlFileName);
            do_exit(CTEST_RETURN_FAIL);
        }
    }

    { /* Test variable "ENUMERATION_DECLAREDTYPE" */
        fmi1_import_enum_variable_t* vi;
        const char* name = "ENUMERATION_DECLAREDTYPE";
        printf("Testing ScalarVariable %s\n", name);
        v = fmi1_import_get_variable_by_name(fmu, name);
        vi = fmi1_import_get_variable_as_enum(v);

        /* Test min attr */
        if (1 != fmi1_import_get_enum_variable_min(vi)) {
            printf("Test of XML file \"%s\" failed. min attribute value missmatch.\n", xmlFileName);
            do_exit(CTEST_RETURN_FAIL);
        }

        /* Test max attr */
        if (5 != fmi1_import_get_enum_variable_max(vi)) {
            printf("Test of XML file \"%s\" failed. max attribute value missmatch.\n", xmlFileName);
            do_exit(CTEST_RETURN_FAIL);
        }
    }

    { /* Test variable "ENUMERATION_DECLAREDTYPE_OVERWRITE" */
        fmi1_import_enum_variable_t* vi;
        const char* name = "ENUMERATION_DECLAREDTYPE_OVERWRITE";
        printf("Testing ScalarVariable %s\n", name);
        v = fmi1_import_get_variable_by_name(fmu, name);
        vi = fmi1_import_get_variable_as_enum(v);

        /* Test min attr */
        if (2 != fmi1_import_get_enum_variable_min(vi)) {
            printf("Test of XML file \"%s\" failed. min attribute value missmatch.\n", xmlFileName);
            do_exit(CTEST_RETURN_FAIL);
        }

        /* Test max attr */
        if (3 != fmi1_import_get_enum_variable_max(vi)) {
            printf("Test of XML file \"%s\" failed. max attribute value missmatch.\n", xmlFileName);
            do_exit(CTEST_RETURN_FAIL);
        }
    }
}

typedef struct {
	const char* filename;
	int performTest;
	void (*fcn)(const char* xmlFileName, fmi1_import_t* fmu);
} xml_test_files_t;

xml_test_files_t xml_test_files[] = {
	{"modelDescription_cs_tc.xml", 1, test_xml_modelDescription_cs_tc},
	{"modelDescription_cs.xml", 0, NULL}
};

void test_xml(const char* xmlFileName, fmi1_import_t* fmu)
{
    int k;
    int foundxml = 0;

    for (k = 0; k < sizeof(xml_test_files)/sizeof(*xml_test_files); k++) {
        foundxml = strcmp(xmlFileName, xml_test_files[k].filename) == 0 ? 1 : 0;
        if (foundxml) {
            if (xml_test_files[k].performTest) {
                xml_test_files[k].fcn(xmlFileName, fmu); /* Run specific file XML file test */
            }
            return;
        }
    }

    if (!foundxml) {
        printf("XML file test is not properly implemented in " __FILE__ " . Could not find the XML \"%s\" in the list of expected XML-files", xmlFileName);
        do_exit(CTEST_RETURN_FAIL);
    }
}


int test_simulate_cs(fmi1_import_t* fmu)
{
	fmi1_status_t fmistatus;
	jm_status_enu_t jmstatus;

	fmi1_string_t instanceName = "Test CS model instance";
	fmi1_string_t fmuGUID;
	fmi1_string_t fmuLocation = "";
	fmi1_string_t mimeType = "";
	fmi1_real_t timeout = 0.0;
	fmi1_boolean_t visible = fmi1_false;
	fmi1_boolean_t interactive = fmi1_false;
/*	fmi1_boolean_t loggingOn = fmi1_true; */

	/* fmi1_real_t simulation_results[] = {-0.001878, -1.722275}; */
	fmi1_real_t simulation_results[] = {0.0143633,   -1.62417};
	fmi1_value_reference_t compare_real_variables_vr[] = {0, 1};
	size_t k;

	fmi1_real_t tstart = 0.0;
	fmi1_real_t tcur = tstart;
	fmi1_real_t hstep = 0.1;
	fmi1_real_t tend = 2.0;
	fmi1_boolean_t StopTimeDefined = fmi1_false;

	if (sizeof(compare_real_variables_vr)/sizeof(fmi1_value_reference_t) != sizeof(simulation_results)/sizeof(fmi1_real_t)) {
		printf("Number of simulation values and reference values are different\n");
		do_exit(CTEST_RETURN_FAIL);
	}

	printf("Version returned from FMU:   %s\n", fmi1_import_get_version(fmu));
	printf("Platform type returned:      %s\n", fmi1_import_get_types_platform(fmu));

	fmuGUID = fmi1_import_get_GUID(fmu);
    printf("GUID:      %s\n", fmuGUID);


	jmstatus = fmi1_import_instantiate_slave(fmu, instanceName, fmuLocation, mimeType, timeout, visible, interactive);
	if (jmstatus == jm_status_error) {
		printf("fmi1_import_instantiate_model failed\n");
		do_exit(CTEST_RETURN_FAIL);
	}

	fmistatus = fmi1_import_initialize_slave(fmu, tstart, StopTimeDefined, tend);
    if(fmistatus != fmi1_status_ok) {
        printf("fmi1_import_initialize_slave failed\n");
		do_exit(CTEST_RETURN_FAIL);
    }

	tcur = tstart;
	printf("%10s %10s\n", "Ball height", "Ball speed");
	while (tcur < tend) {
		fmi1_boolean_t newStep = fmi1_true;
#if 0 /* Prints a real value.. */
		fmi1_real_t rvalue;
		fmi1_value_reference_t vr = 0;

		fmistatus = fmi1_import_get_real(fmu, &vr, 1, &rvalue);
		printf("rvalue = %f\n", rvalue);
#endif
		fmistatus = fmi1_import_do_step(fmu, tcur, hstep, newStep);

		for (k = 0; k < sizeof(compare_real_variables_vr)/sizeof(fmi1_value_reference_t); k++) {
			fmi1_value_reference_t vr = compare_real_variables_vr[k];
			fmi1_real_t rvalue;
			fmistatus = fmi1_import_get_real(fmu, &vr, 1, &rvalue);
		}
		{
			fmi1_real_t val[2];
			fmi1_import_get_real(fmu, compare_real_variables_vr, 2, val);
			printf("%10g %10g\n", val[0],val[1]);
		}

		tcur += hstep;
	}

	printf("Simulation finished. Checking results\n");

	/* Validate result */
	for (k = 0; k < sizeof(compare_real_variables_vr)/sizeof(fmi1_value_reference_t); k++) {
		fmi1_value_reference_t vr = compare_real_variables_vr[k];
		fmi1_real_t rvalue;
		fmi1_real_t res;
		fmistatus = fmi1_import_get_real(fmu, &vr, 1, &rvalue);
		res = rvalue - simulation_results[k];
		res = res > 0 ? res: -res; /* Take abs */
		if (res > 3e-3) {
			printf("Simulation results is wrong!\n");
			printf("State [%u]  %g != %g, |res| = %g\n", (unsigned)k, rvalue, simulation_results[k], res);
			printf("\n");
			do_exit(CTEST_RETURN_FAIL);
		}
	}

	fmistatus = fmi1_import_terminate_slave(fmu);

	fmi1_import_free_slave_instance(fmu);

	return 0;
}

int main(int argc, char *argv[])
{
	fmi1_callback_functions_t callBackFunctions;
	const char* FMUPath;
	const char* tmpPath;
	const char* xmlFileName;
	jm_callbacks callbacks;
	fmi_import_context_t* context;
	fmi_version_enu_t version;
	jm_status_enu_t status;
	int k;

	fmi1_import_t* fmu;

	if(argc < 4) {
		printf("Usage: %s <fmu_file> <temporary_dir> <modelDescription_file>\n", argv[0]);
		do_exit(CTEST_RETURN_FAIL);
	}
	for (k = 0; k < argc; k ++)
		printf("argv[%d] = %s\n", k, argv[k]);

	FMUPath = argv[1];
	tmpPath = argv[2];
	xmlFileName = argv[3];

	callbacks.malloc = malloc;
    callbacks.calloc = calloc;
    callbacks.realloc = realloc;
    callbacks.free = free;
    callbacks.logger = importlogger;
	callbacks.log_level = jm_log_level_debug;
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


	status = fmi1_import_create_dllfmu(fmu, callBackFunctions, 1);
	if (status == jm_status_error) {
		printf("Could not create the DLL loading mechanism(C-API) (error: %s).\n", fmi1_import_get_last_error(fmu));
		do_exit(CTEST_RETURN_FAIL);
	}

	test_simulate_cs(fmu);
	test_xml(xmlFileName, fmu);

	fmi1_import_destroy_dllfmu(fmu);

	fmi1_import_free(fmu);
	fmi_import_free_context(context);

	printf("Everything seems to be OK since you got this far=)!\n");

	do_exit(CTEST_RETURN_SUCCESS);

	return 0;
}


