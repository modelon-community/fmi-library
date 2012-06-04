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

#ifdef __cplusplus 
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

#include <miniunz.h>

#include <Common/jm_types.h>
#include <Common/jm_callbacks.h>
#include <Common/jm_portability.h>

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

	argv[0]="miniunz";
	argv[1]="-x";
	argv[2]="-o";
	argv[3]=zip_file_path;
	argv[4]="-d";
	argv[5]=output_folder;
	

	/* Temporary save the current directory */
	if (jm_portability_get_current_working_directory(cd, sizeof(cd) / sizeof(char)) == jm_status_error) {
		jm_log(callbacks, "UNZIP", jm_log_level_error, "Could not get Current Directory");
		return jm_status_error;
	}

	/* Unzip */
	status = miniunz(argc, (char**)argv);

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

#ifdef __cplusplus 
}
#endif