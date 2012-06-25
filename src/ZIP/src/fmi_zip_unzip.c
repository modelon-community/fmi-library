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

#ifdef __cplusplus 
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

#include <miniunz.h>

#include <JM/jm_types.h>
#include <JM/jm_callbacks.h>
#include <JM/jm_portability.h>

static const char* module = "FMIZIP";

jm_status_enu_t fmi_zip_unzip(const char* zip_file_path, const char* output_folder, jm_callbacks* callbacks)
{
	/*
	Usage : miniunz [-e] [-x] [-v] [-l] [-o] [-p password] file.zip [file_to_extr.] [-d extractdir]
	  -e  Extract without pathname (junk paths)
	  -x  Extract with pathname
	  -v  list files
	  -l  list files
	  -d  directory to extract into
	  -o  overwrite files without prompting
	  -p  extract crypted file using password
	*/

	/* A call to minunz may change the current directory and therefore we must change it back */
	char cd[FILENAME_MAX];

	int argc = 6;
	const char *argv[6];
	int status;

	jm_log_verbose(callbacks, module, "Unpacking FMU into %s", output_folder);

	argv[0]="miniunz";
	argv[1]="-x";
	argv[2]="-o";
	argv[3]=zip_file_path;
	argv[4]="-d";
	argv[5]=output_folder;
	

	/* Temporary save the current directory */
	if (jm_portability_get_current_working_directory(cd, sizeof(cd) / sizeof(char)) == jm_status_error) {
		jm_log_fatal(callbacks, module, "Could not get Current Directory");
		return jm_status_error;
	}

	/* Unzip */
	status = miniunz(argc, (char**)argv);

	/* Reset the current directory */
	if (jm_portability_set_current_working_directory(cd) == jm_status_error) {
		jm_log_warning(callbacks, module, "Could not restore Current Directory after unpacking");
		return jm_status_warning;
	}

	/* Return error status */
	if (status == 0) {
		return jm_status_success;
	} else {
		jm_log_fatal(callbacks, "FMIZIP", "Unpacking of FMU %s into %s failed", zip_file_path, output_folder);
		return jm_status_error;	
	}
}

#ifdef __cplusplus 
}
#endif