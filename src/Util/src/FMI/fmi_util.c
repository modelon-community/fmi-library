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

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <FMI/fmi_util.h>

char* fmi_construct_dll_dir_name(jm_callbacks* callbacks, const char* fmu_unzipped_path) {
	char* dir_path;
	int len;

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

	sprintf(dir_path, "%s%s%s%s%s%s", fmu_unzipped_path, FMI_FILE_SEP, FMI_BINARIES, FMI_FILE_SEP, FMI_PLATFORM, FMI_FILE_SEP);

	return dir_path;
}

FMILIB_EXPORT char* fmi_construct_dll_file_name(jm_callbacks* callbacks, const char* model_identifier) {
	char* fname;
	int len;
	assert(callbacks && model_identifier);
	len = 
		strlen(FMI_FILE_SEP) + 1 +
		strlen(model_identifier) 
		+ strlen(FMI_DLL_EXT) + 1;
	fname = (char*)callbacks->malloc(len);
	if (fname == NULL) {
		jm_log_fatal(callbacks, "FMIUT", "Failed to allocate memory.");
		return NULL;
	}
	sprintf(fname, ".%s%s%s", FMI_FILE_SEP, model_identifier, FMI_DLL_EXT);

	return fname;
}
