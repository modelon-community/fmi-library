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

#ifdef _WIN32
#define FILE_SEP "\\"
#define DLL_EXT ".dll"
#else
#define FILE_SEP "/"
#define DLL_EXT ".so"
#endif

#define BINARIES "binaries"
#define MODEL_DESCRIPTION_XML "modelDescription.xml"

#ifdef _WIN64
#define PLATFORM "win64"
#elif defined _WIN32
#define PLATFORM "win32"
#elif (defined __LP64__ || defined __LLP64__)
#define PLATFORM "linux64"
#else
#define PLATFORM "linux32"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "fmi_import_util.h"

char* fmi_import_get_dll_path(const char* fmu_unzipped_path, const char* model_identifier, jm_callbacks* callbacks)
{
	char* dll_path;
	int len;

	if (model_identifier == NULL || fmu_unzipped_path == NULL) {
		return NULL;
	}

	len = strlen(fmu_unzipped_path) + strlen(FILE_SEP) + strlen(BINARIES) + strlen(FILE_SEP) + strlen(PLATFORM) + strlen(FILE_SEP) + strlen(model_identifier) + strlen(DLL_EXT) + 1;

	dll_path = (char*)callbacks->calloc(len, sizeof(char));
	if (dll_path == NULL) {
		callbacks->logger(NULL, "FMIIMPORTUTIL", jm_log_level_error, "Failed to allocate memory.");
		return NULL;
	}

	sprintf(dll_path, "%s%s%s%s%s%s%s%s", fmu_unzipped_path, FILE_SEP, BINARIES, FILE_SEP, PLATFORM, FILE_SEP, model_identifier, DLL_EXT);

	return dll_path;
}

char* fmi_import_get_model_description_path(const char* fmu_unzipped_path, jm_callbacks* callbacks)
{
	char* model_description_path;
	int len;

	len = strlen(fmu_unzipped_path) + strlen(FILE_SEP) + strlen(MODEL_DESCRIPTION_XML) + 1;

	model_description_path = (char*)callbacks->calloc(len, sizeof(char));
	if (model_description_path == NULL) {
		callbacks->logger(NULL, "FMIIMPORTUTIL", jm_log_level_error, "Failed to allocate memory.");
		return NULL;
	}

	sprintf(model_description_path, "%s%s%s", fmu_unzipped_path, FILE_SEP, MODEL_DESCRIPTION_XML);

	return model_description_path;
}
