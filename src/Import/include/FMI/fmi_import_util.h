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

#ifndef FMI1_IMPORT_UTIL_H_
#define FMI1_IMPORT_UTIL_H_

#include <Common/jm_callbacks.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
\addtogroup  fmi_import
@{
\name Utility functions
*/
/** Given directory name fmu_unzipped_path and model identifier consturct Dll/so name
	@return Pointer to a string with the file name. Caller is responsible for freeing the memory.
*/
FMILIB_EXPORT char* fmi_import_get_dll_path(const char* fmu_unzipped_path, const char* model_identifier, jm_callbacks* callBackFunctions);

/** Given directory name fmu_unzipped_path and model identifier consturct XML file name
	@return Pointer to a string with the file name. Caller is responsible for freeing the memory.
*/
FMILIB_EXPORT char* fmi_import_get_model_description_path(const char* fmu_unzipped_path, jm_callbacks* callBackFunctions);
/**
@}
@}
*/
#ifdef __cplusplus
}
#endif

#endif /* End of header file FMI1_IMPORT_UTIL_H_ */