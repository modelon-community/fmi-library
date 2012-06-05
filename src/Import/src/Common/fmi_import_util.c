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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <fmilib_config.h>
#include "fmi_import_util.h"



char* fmi_import_get_dll_path(const char* fmu_unzipped_path, const char* model_identifier, jm_callbacks* callbacks)
{
	char* dll_path;
	int len;

	if (model_identifier == NULL || fmu_unzipped_path == NULL) {
		assert(0);
		return NULL;
	}

	len = strlen(fmu_unzipped_path) + strlen(FMI_FILE_SEP) + strlen(FMI_BINARIES) + strlen(FMI_FILE_SEP) + strlen(FMI_PLATFORM) + strlen(FMI_FILE_SEP) + strlen(model_identifier) + strlen(FMI_DLL_EXT) + 1;

	dll_path = (char*)callbacks->calloc(len, sizeof(char));
	if (dll_path == NULL) {
		jm_log_fatal(callbacks, "FMILIB", "Failed to allocate memory.");
		return NULL;
	}

	sprintf(dll_path, "%s%s%s%s%s%s%s%s", fmu_unzipped_path, FMI_FILE_SEP, FMI_BINARIES, FMI_FILE_SEP, FMI_PLATFORM, FMI_FILE_SEP, model_identifier, FMI_DLL_EXT);

	return dll_path;
}

char* fmi_import_get_model_description_path(const char* fmu_unzipped_path, jm_callbacks* callbacks)
{
	char* model_description_path;
	int len;

	len = strlen(fmu_unzipped_path) + strlen(FMI_FILE_SEP) + strlen(FMI_MODEL_DESCRIPTION_XML) + 1;

	model_description_path = (char*)callbacks->calloc(len, sizeof(char));
	if (model_description_path == NULL) {
		callbacks->logger(NULL, "FMIIMPORTUTIL", jm_log_level_error, "Failed to allocate memory.");
		return NULL;
	}

	sprintf(model_description_path, "%s%s%s", fmu_unzipped_path, FMI_FILE_SEP, FMI_MODEL_DESCRIPTION_XML);

	return model_description_path;
}
