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

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <FMI/fmi_util.h>

char* fmi_construct_dll_dir_name(jm_callbacks* callbacks, const char* fmu_unzipped_path) {
	char* dir_path;
	size_t len;

	assert( fmu_unzipped_path && callbacks);

	len = 
		strlen(fmu_unzipped_path) + strlen(FMI_FILE_SEP)
		+ strlen(FMI_BINARIES) + strlen(FMI_FILE_SEP) 
		+ strlen(FMI_PLATFORM) + strlen(FMI_FILE_SEP) + 1;

	dir_path = (char*)callbacks->malloc(len);
	if (dir_path == NULL) {
		jm_log_fatal(callbacks, "FMIUT", "Failed to allocate memory.");
		return NULL;
	}

	sprintf(dir_path, "%s%s%s%s%s%s", fmu_unzipped_path, FMI_FILE_SEP, FMI_BINARIES, FMI_FILE_SEP, FMI_PLATFORM, FMI_FILE_SEP);/*safe */

	return dir_path;
}

char* fmi_construct_dll_file_name(jm_callbacks* callbacks, const char* dll_dir_name, const char* model_identifier) {
	char* fname;
	size_t len;
	assert(callbacks && model_identifier);
	len = 
		strlen(dll_dir_name) +
		strlen(model_identifier) 
		+ strlen(FMI_DLL_EXT) + 1;
	fname = (char*)callbacks->malloc(len);
	if (fname == NULL) {
		jm_log_fatal(callbacks, "FMIUT", "Failed to allocate memory.");
		return NULL;
	}
	sprintf(fname, "%s%s%s", dll_dir_name, model_identifier, FMI_DLL_EXT);/*safe */

	return fname;
}
