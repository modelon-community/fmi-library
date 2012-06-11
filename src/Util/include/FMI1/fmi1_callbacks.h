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

#ifndef FMI1_CALLBACKS_H
#define FMI1_CALLBACKS_H

#include <JM/jm_callbacks.h>
#include "fmi1_functions.h"

#ifdef __cplusplus
extern "C" {
#endif
/** \file fmi1_callbacks.h
	Support for FMI callbacks.
*/

/**	\addtogroup fmi1_utils
	@{
*/
/** 
\name Replacing system malloc/calloc/realloc/free

	NOTE: THIS IS NOT FULLY IMPLEMENTED
	
    In order to replace the system malloc/calloc/realloc/free an FMU must compile with -DFMU_REPLACE_SYSTEM_MALLOC
    and execute the following call sequence:
    1. During inititalization in fmi_xml_instantiate_model allocate fmiCallbacks struct and call fmi_xml_init_callbacks on it:
        fmiCallbacks cb;
        fmi_export_init_callbacks(&cb);
       The allocated structure must be preserved between entries into the DLL (must be part of the FMU context)
    2. On each entry into the FMU DLL where malloc/calloc (not callbacks) are expected to be called call
    jm_set_default_callbacks(&cb) to set memory handling functions to point to the correct context.
    Note that realloc and free get the correct context from heap memory directly if present.
@{

Mapping of FMI callbacks to ::jm_callbacks*/
typedef struct fmi1_callbacks_t {
    jm_callbacks jmFunctions;
    fmi1_callback_functions_t fmiFunctions;
} fmi1_callbacks_t;

/** 
\brief ::fmi_memory_header_t is used to enable replacement of system memory handling functions with the functions provided in FMI callbacks
*/
typedef struct fmi_memory_header_t {
    fmi1_callbacks_t* callbacks;
    size_t size;
} fmi1_memory_header_t;

/** Initialize ::fmi1_callback_functions_t structure to map jm library callback functions onto fmi callbacks */
void fmi1_export_init_callbacks(fmi1_callbacks_t* callbacks, fmi1_callback_functions_t* fmiFunctions);
/** @}
*/

/** Default logger may be used when instantiating FMUs */
void  fmi1_default_callback_logger(fmi1_component_t c, fmi1_string_t instanceName, fmi1_status_t status, fmi1_string_t category, fmi1_string_t message, ...);

/** Given fmi1_callbacks_t logger (fmi1_logger), the jm_callbacks logger (jm_logger) may redirect the messages to the fmi1_logger.
    The functions sets up the redirection.
*/
void fmi1_import_init_logger(jm_callbacks*, fmi1_callbacks_t*);
/**	@}
*/

#ifdef __cplusplus
}
#endif
#endif /* FMI1_CALLBACKS_H */
