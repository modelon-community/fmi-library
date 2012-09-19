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

/** \file fmi2_import.h
*  \brief Public interface to the FMI import C-library.
*/

#ifndef FMI2_IMPORT_H_
#define FMI2_IMPORT_H_

#include <stddef.h>
#include <fmilib_config.h>
#include <JM/jm_callbacks.h>
#include <FMI/fmi_import_util.h>
#include <FMI/fmi_import_context.h>
/* #include <FMI2/fmi2_xml_model_description.h> */

#include <FMI2/fmi2_types.h>
#include <FMI2/fmi2_functions.h>
#include <FMI2/fmi2_enums.h>

#include "fmi2_import_type.h"
#include "fmi2_import_unit.h"
#include "fmi2_import_variable.h"
#include "fmi2_import_vendor_annotations.h"
#include "fmi2_import_variable_list.h"

#include "fmi2_import_capi.h"
#include "fmi2_import_convenience.h"
#include "fmi2_import_cosim.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup  fmi2_import FMI 1.0 import interface
 *  All the structures used in the interfaces are intended to
 *  be treated as opaque objects by the client code.
 @{ 
 */

/**	\addtogroup fmi2_import_init Constuction, destruction and error handling
 * 	\addtogroup fmi2_import_gen General information retrieval
 *	\addtogroup fmi2_import_capi Interface to the standard FMI 1.0 "C" API
 *  \brief Convenient functions for calling the FMI functions. This interface wrappes the "C" API. 
 */
 /** @} */
 /** @} */

/** \addtogroup fmi2_import_init Constuction, destruction and error handling
@{
*/
/**
   \brief Create ::fmi2_import_t structure and parse the XML file.

    @param context A context data strucutre is used to propagate the callbacks for memory handling and logging.
    @param dirPath A directory name (full path) of a directory where the FMU was unzipped.
    @return The new structure if parsing was successfull. 0-pointer is returned on error.
*/
FMILIB_EXPORT fmi2_import_t* fmi2_import_parse_xml( fmi_import_context_t* context, const char* dirPath);

/**
* \brief Retrieve the last error message.
*
* Error handling:
*
*  Many functions in the library return pointers to struct. An error is indicated by returning NULL/0-pointer.
*  If error is returned than fmi2_import_get_last_error() functions can be used to retrieve the error message.
*  If logging callbacks were specified then the same information is reported via logger.
*  Memory for the error string is allocated and deallocated in the module.
*  Client code should not store the pointer to the string since it can become invalid.
*    @param fmu An FMU object as returned by fmi2_import_parse_xml().
*    @return NULL-terminated string with an error message.
*/
FMILIB_EXPORT const char* fmi2_import_get_last_error(fmi2_import_t* fmu);

/**
\brief Clear the error message.
* @param fmu An FMU object as returned by fmi2_import_parse_xml().
* @return 0 if further processing is possible. If it returns 1 then the 
*	error was not recoverable. The \p fmu object should then be freed and recreated.
*/
FMILIB_EXPORT int fmi2_import_clear_last_error(fmi2_import_t* fmu);

/**
\brief Release the memory allocated
@param fmu An fmu object as returned by fmi2_import_parse_xml().
*/
FMILIB_EXPORT void fmi2_import_free(fmi2_import_t* fmu);
/** @}
\addtogroup fmi2_import_gen
 * \brief Functions for retrieving general model information. Memory for the strings is allocated and deallocated in the module.
 *   All the functions take an FMU object as returned by fmi2_import_parse_xml() as a parameter. 
 *   The information is retrieved from the XML file.
 * @{
*/
/** 
\brief Get model name. 
@param fmu An fmu object as returned by fmi2_import_parse_xml().
*/
FMILIB_EXPORT const char* fmi2_import_get_model_name(fmi2_import_t* fmu);

/** \brief Retrieve capability flags by ID. */
FMILIB_EXPORT unsigned int fmi2_import_get_capability(fmi2_import_t* , fmi2_capabilities_enu_t id);

/** 
\brief Get model identifier for ModelExchange. 
@param fmu An fmu object as returned by fmi2_import_parse_xml().
*/
FMILIB_EXPORT const char* fmi2_import_get_model_identifier_ME(fmi2_import_t* fmu);

/** 
\brief Get model identifier for CoSimulation. 
@param fmu An fmu object as returned by fmi2_import_parse_xml().
*/
FMILIB_EXPORT const char* fmi2_import_get_model_identifier_CS(fmi2_import_t* fmu);

/** 
\brief Get FMU GUID. 
@param fmu An fmu object as returned by fmi2_import_parse_xml().
*/
FMILIB_EXPORT const char* fmi2_import_get_GUID(fmi2_import_t* fmu);

/** 
\brief Get FMU description.
@param fmu An fmu object as returned by fmi2_import_parse_xml().
*/
FMILIB_EXPORT const char* fmi2_import_get_description(fmi2_import_t* fmu);

/** 
\brief Get FMU author. 
@param fmu An fmu object as returned by fmi2_import_parse_xml().
*/
FMILIB_EXPORT const char* fmi2_import_get_author(fmi2_import_t* fmu);

/** \brief Get FMU version.
@param fmu An fmu object as returned by fmi2_import_parse_xml().
*/
FMILIB_EXPORT const char* fmi2_import_get_model_version(fmi2_import_t* fmu);

/** \brief Get FMI standard version (always 1.0). 
@param fmu An fmu object as returned by fmi2_import_parse_xml().
*/
FMILIB_EXPORT const char* fmi2_import_get_model_standard_version(fmi2_import_t* fmu);

/** \brief Get FMU generation tool. 
@param fmu An fmu object as returned by fmi2_import_parse_xml().
*/
FMILIB_EXPORT const char* fmi2_import_get_generation_tool(fmi2_import_t* fmu);

/** \brief Get FMU generation date and time. 
@param fmu An fmu object as returned by fmi2_import_parse_xml().
*/
FMILIB_EXPORT const char* fmi2_import_get_generation_date_and_time(fmi2_import_t* fmu);

/** \brief Get variable naming convention used. 
@param fmu An fmu object as returned by fmi2_import_parse_xml().
*/
FMILIB_EXPORT fmi2_variable_naming_convension_enu_t fmi2_import_get_naming_convention(fmi2_import_t* fmu);

/** \brief Get the number of contnuous states. */
FMILIB_EXPORT unsigned int fmi2_import_get_number_of_continuous_states(fmi2_import_t* fmu);

/** \brief Get the number of event indicators. */
FMILIB_EXPORT unsigned int fmi2_import_get_number_of_event_indicators(fmi2_import_t* fmu);

/** \brief Get the start time for default experiment  as specified in the XML file. */
FMILIB_EXPORT double fmi2_import_get_default_experiment_start(fmi2_import_t* fmu);

/** \brief Get the stop time for default experiment  as specified in the XML file. */
FMILIB_EXPORT double fmi2_import_get_default_experiment_stop(fmi2_import_t* fmu);

/** \brief Get the tolerance default experiment as specified in the XML file. */
FMILIB_EXPORT double fmi2_import_get_default_experiment_tolerance(fmi2_import_t* fmu);

/** \brief Get the type of the FMU (model exchange or co-simulation) */
FMILIB_EXPORT fmi2_fmu_kind_enu_t fmi2_import_get_fmu_kind(fmi2_import_t* fmu);

/** \brief Get the list of all the type definitions in the model*/
FMILIB_EXPORT fmi2_import_type_definitions_t* fmi2_import_get_type_definitions(fmi2_import_t* );

/** \brief Get a list of all the unit definitions in the model. */
FMILIB_EXPORT fmi2_import_unit_definitions_t* fmi2_import_get_unit_definitions(fmi2_import_t* fmu);

/** 
	\brief Get the direct dependency information

	@return A variable list is returned for variables with causality Output. Null pointer for others. */
FMILIB_EXPORT fmi2_import_variable_list_t* fmi2_import_get_direct_dependency(fmi2_import_t* fmu, fmi2_import_variable_t*);

/** \brief Get the variable with the same value reference that is not an alias*/
FMILIB_EXPORT fmi2_import_variable_t* fmi2_import_get_variable_alias_base(fmi2_import_t* fmu,fmi2_import_variable_t*);

/**
    Get the list of all the variables aliased to the given one (including the base one).

    Note that the list is ordered: base variable, aliases, negated aliases.
*/
FMILIB_EXPORT fmi2_import_variable_list_t* fmi2_import_get_variable_aliases(fmi2_import_t* fmu,fmi2_import_variable_t*);

/** \brief Get the list of all the variables in the model.
* @param fmu An FMU object as returned by fmi2_import_parse_xml().
* @return a variable list with all the variables in the model.
*
* Note that variable lists are allocated dynamically and must be freed when not needed any longer.
*/
FMILIB_EXPORT fmi2_import_variable_list_t* fmi2_import_get_variable_list(fmi2_import_t* fmu);

/** \brief Create a variable list with a single variable.
  
\param fmu An FMU object that this variable list will reference.
\param v A variable.
*/
FMILIB_EXPORT fmi2_import_variable_list_t* fmi2_import_create_var_list(fmi2_import_t* fmu,fmi2_import_variable_t* v);

/**@} */

#ifdef __cplusplus
}
#endif

#endif