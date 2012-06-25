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
#include <JM/jm_types.h>
#include <JM/jm_callbacks.h>
#include <FMI/fmi_zip_zip.h>

static void importlogger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message)
{
        printf("module = %s, log level = %d: %s\n", module, log_level, message);
}

int main(int argc, char *argv[])
{
	jm_status_enu_t status;
	jm_callbacks callbacks;
	char* output_name;	
	const char** files_to_zip;
	int n_files_to_zip;

	callbacks.malloc = malloc;
    callbacks.calloc = calloc;
    callbacks.realloc = realloc;
    callbacks.free = free;
    callbacks.logger = importlogger;
	callbacks.log_level = jm_log_level_debug;

    callbacks.context = 0;

	if (argc < 3) {
		printf("Not enought input arguments\n");
		return 1;
	}

	output_name = argv[1];
	files_to_zip = &argv[2];
	n_files_to_zip = argc - 2;

	status = fmi_zip_zip(output_name, n_files_to_zip, files_to_zip, &callbacks);

	if (status == jm_status_error) {
		printf("Failed to compress the file\n");
		return 1;
	} else {
		printf("Succesfully compressed the file\n");
		return 0;
	}	
	
}