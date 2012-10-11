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
#include <minizip.h>

#include <JM/jm_types.h>
#include <JM/jm_callbacks.h>
#include <JM/jm_portability.h>

jm_status_enu_t fmi_zip_zip(const char* zip_file_path, int n_files_to_zip, const char** files_to_zip, jm_callbacks* callbacks)
{
	/* A call to minizip may change the current directory and therefore we must change it back */
	char cd[FILENAME_MAX];

#define N_BASIC_ARGS 4
	int argc;
	char** argv;
	int k;
	int status;

	/* Temporary save the current directory */
	if (jm_portability_get_current_working_directory(cd, sizeof(cd) / sizeof(char)) == jm_status_error) {
		jm_log(callbacks, "UNZIP", jm_log_level_error, "Could not get Current Directory");
		return jm_status_error;
	}

	argc = N_BASIC_ARGS + n_files_to_zip;
	argv = callbacks->calloc(sizeof(char*), argc);	
	/* Failed to allocate memory, return error */
	if (argv == NULL) {
		callbacks->logger(NULL, "FMIZIP", jm_log_level_error, "Failed to allocate memory.");
		return jm_status_error;	
	}

	/* Input arguments to the corresponding minizip main() function call */
	/*
	Usage : minizip [-o] [-a] [-0 to -9] [-p password] [-j] file.zip [files_to_add]

	-o  Overwrite existing file.zip
	-a  Append to existing file.zip
	-0  Store only
	-1  Compress faster
	-9  Compress better

	-j  exclude path. store only the file name.
	*/
	argv[0]="minizip";
	argv[1]="-o";
	argv[2]="-1";
	argv[3]=(char*)zip_file_path;

	/* Append the input argument list with the files to unzip */
	jm_log_info(callbacks, "FMIZIP", "Will compress following files: \n");
	for (k = 0; k < n_files_to_zip; k++) {
		jm_log_info(callbacks, "FMIZIP", "\t%s\n", files_to_zip[k]);
		argv[N_BASIC_ARGS + k] = (char*)files_to_zip[k];
	}

	/* Zip */
	status = minizip(argc, (char**)argv);

	/* Free allocated memory */
	callbacks->free(argv);

	/* Reset the current directory */
	if (jm_portability_set_current_working_directory(cd) == jm_status_error) {
		jm_log(callbacks, "UNZIP", jm_log_level_warning, "Could not change back Current Directory");
		return jm_status_warning;
	}

	/* Return error status */
	if (status == 0) {
		return jm_status_success;
	} else {
		return jm_status_error;	
	}
}
