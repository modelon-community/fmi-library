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

/** \file fmi1_import.h
*  \brief Public interface to the FMI import C-library.
*/

#ifndef FMI1_IMPORT_H_
#define FMI1_IMPORT_H_

#include <stddef.h>
#include <fmilib_config.h>
#include <JM/jm_callbacks.h>
#include <FMI/fmi_import_util.h>
#include <FMI/fmi_import_context.h>
/* #include <FMI1/fmi1_xml_model_description.h> */

#include <FMI1/fmi1_types.h>
#include <FMI1/fmi1_functions.h>
#include <FMI1/fmi1_enums.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup  fmi1_import FMI 1.0 import interface
 *  All the structures used in the interfaces are intended to
 *  be treated as opaque objects by the client code.
 @{ 
 */
/** 
\name Vendor annotation supporting structures
*/
/**@{ */
	/** \brief  Opaque list of vendor annotations. */
typedef struct fmi1_xml_vendor_list_t fmi1_import_vendor_list_t;
	/** \brief Opaque vendor object. */
typedef struct fmi1_xml_vendor_t fmi1_import_vendor_t;
	/** \brief Opaque annotation object. */
typedef struct fmi1_xml_annotation_t fmi1_import_annotation_t;
/**@} */

/**@name   Type definitions supporting structures*/
/**@{ */
/** \brief Opaque type definition object. */
typedef struct fmi1_xml_real_typedef_t fmi1_import_real_typedef_t;
/** \brief Opaque integer type definition object. */
typedef struct fmi1_xml_integer_typedef_t fmi1_import_integer_typedef_t;
/** \brief Opaque enumeration type definition object. */
typedef struct fmi1_xml_enumeration_typedef_t fmi1_import_enumeration_typedef_t;
/** \brief Opaque general variable type definition object. */
typedef struct fmi1_xml_variable_typedef_t fmi1_import_variable_typedef_t;
/** \brief Opaque list of the type definitions in the model */
typedef struct fmi1_xml_type_definitions_t fmi1_import_type_definitions_t;
/**@} */

/**@name Scalar variable types */
/**@{ */
/** \brief General variable type. 
*
* This type is convenient to unify all the variable list operations. 
* 	However, typed variables are needed to support specific attributes.
*/
typedef struct fmi1_xml_variable_t fmi1_import_variable_t;
/** \brief  Opaque real variable */
typedef struct fmi1_xml_real_variable_t fmi1_import_real_variable_t;
/** \brief Opaque integer variable */
typedef struct fmi1_xml_integer_variable_t fmi1_import_integer_variable_t;
/** \brief Opaque string variable */
typedef struct fmi1_xml_string_variable_t fmi1_import_string_variable_t;
/** \brief Opaque enumeration variable */
typedef struct fmi1_xml_enum_variable_t fmi1_import_enum_variable_t;
/** \brief Opaque boolean variable */
typedef struct fmi1_xml_bool_variable_t fmi1_import_bool_variable_t;
/** \brief List of variables */
typedef struct fmi1_import_variable_list_t fmi1_import_variable_list_t;
/**@} */

/**\name Structures encapsulating unit information */
/**@{ */
/** \brief A variable unit defined with a unit defition */
typedef struct fmi1_xml_unit_t fmi1_import_unit_t;
/** \brief A display unit. */
typedef struct fmi1_xml_display_unit_t fmi1_import_display_unit_t;
/** \brief The list of all the unit definitions in the model */
typedef struct fmi1_xml_unit_definitions_t fmi1_import_unit_definitions_t;
/**@} */

/**\name FMU capabilities flags */
/**@{ */
/** \brief A container for all the capability flags */
typedef struct fmi1_xml_capabilities_t fmi1_import_capabilities_t;
/** @} */
/**	\addtogroup fmi1_import_init Constuction, destruction and error handling
 * 	\addtogroup fmi1_import_gen General information retrieval
 *	\addtogroup fmi1_import_capi Interface to the standard FMI 1.0 "C" API
 *  \brief Convenient functions for calling the FMI functions. This interface wrappes the "C" API. 
 */
 /** @} */
 /** @} */

/** \addtogroup fmi1_import_init Constuction, destruction and error handling
@{
*/
/**
   \brief Create ::fmi1_import_t structure and parse the XML file.

    @param context A context data strucutre is used to propagate the callbacks for memory handling and logging.
    @param dirPath A directory name (full path) of a directory where the FMU was unzipped.
    @return The new structure if parsing was successfull. 0-pointer is returned on error.
*/
FMILIB_EXPORT fmi1_import_t* fmi1_import_parse_xml( fmi_import_context_t* context, const char* dirPath);

/**
* \brief Retrieve the last error message.
*
* Error handling:
*
*  Many functions in the library return pointers to struct. An error is indicated by returning NULL/0-pointer.
*  If error is returned than fmi_import_get_last_error() functions can be used to retrieve the error message.
*  If logging callbacks were specified then the same information is reported via logger.
*  Memory for the error string is allocated and deallocated in the module.
*  Client code should not store the pointer to the string since it can become invalid.
*    @param fmu An FMU object as returned by fmi1_import_parse_xml().
*    @return NULL-terminated string with an error message.
*/
FMILIB_EXPORT const char* fmi1_import_get_last_error(fmi1_import_t* fmu);

/**
\brief Clear the error message.
* @param fmu An FMU object as returned by fmi1_import_parse_xml().
* @return 0 if further processing is possible. If it returns 1 then the 
*	error was not recoverable. The fmu object should then be freed and recreated.
*/
FMILIB_EXPORT int fmi1_import_clear_last_error(fmi1_import_t* fmu);

/**
\brief Release the memory allocated
@param fmu An fmu object as returned by fmi1_import_parse_xml().
*/
FMILIB_EXPORT void fmi1_import_free(fmi1_import_t* fmu);
/** @}
\addtogroup fmi1_import_gen
 * \brief Functions for retrieving general model information. Memory for the strings is allocated and deallocated in the module.
 *   All the functions take an FMU object as returned by fmi1_import_parse_xml() as a parameter. 
 *   The information is retrieved from the XML file.
 * @{
*/
/** 
\brief Get model name. 
@param fmu An fmu object as returned by fmi1_import_parse_xml().
*/
FMILIB_EXPORT const char* fmi1_import_get_model_name(fmi1_import_t* fmu);

/** 
\brief Get model identifier. 
@param fmu An fmu object as returned by fmi1_import_parse_xml().
*/
FMILIB_EXPORT const char* fmi1_import_get_model_identifier(fmi1_import_t* fmu);

/** 
\brief Get FMU GUID. 
@param fmu An fmu object as returned by fmi1_import_parse_xml().
*/
FMILIB_EXPORT const char* fmi1_import_get_GUID(fmi1_import_t* fmu);

/** 
\brief Get FMU description.
@param fmu An fmu object as returned by fmi1_import_parse_xml().
*/
FMILIB_EXPORT const char* fmi1_import_get_description(fmi1_import_t* fmu);

/** 
\brief Get FMU author. 
@param fmu An fmu object as returned by fmi1_import_parse_xml().
*/
FMILIB_EXPORT const char* fmi1_import_get_author(fmi1_import_t* fmu);

/** \brief Get FMU version.
@param fmu An fmu object as returned by fmi1_import_parse_xml().
*/
FMILIB_EXPORT const char* fmi1_import_get_model_version(fmi1_import_t* fmu);

/** \brief Get FMI standard version (always 1.0). 
@param fmu An fmu object as returned by fmi1_import_parse_xml().
*/
FMILIB_EXPORT const char* fmi1_import_get_model_standard_version(fmi1_import_t* fmu);

/** \brief Get FMU generation tool. 
@param fmu An fmu object as returned by fmi1_import_parse_xml().
*/
FMILIB_EXPORT const char* fmi1_import_get_generation_tool(fmi1_import_t* fmu);

/** \brief Get FMU generation date and time. 
@param fmu An fmu object as returned by fmi1_import_parse_xml().
*/
FMILIB_EXPORT const char* fmi1_import_get_generation_date_and_time(fmi1_import_t* fmu);

/** \brief Get variable naming convention used. 
@param fmu An fmu object as returned by fmi1_import_parse_xml().
*/
FMILIB_EXPORT fmi1_variable_naming_convension_enu_t fmi1_import_get_naming_convention(fmi1_import_t* fmu);

/** \brief Get the number of contnuous states. */
FMILIB_EXPORT unsigned int fmi1_import_get_number_of_continuous_states(fmi1_import_t* fmu);

/** \brief Get the number of event indicators. */
FMILIB_EXPORT unsigned int fmi1_import_get_number_of_event_indicators(fmi1_import_t* fmu);

/** \brief Get the start time for default experiment  as specified in the XML file. */
FMILIB_EXPORT double fmi1_import_get_default_experiment_start(fmi1_import_t* fmu);

/** \brief Get the stop time for default experiment  as specified in the XML file. */
FMILIB_EXPORT double fmi1_import_get_default_experiment_stop(fmi1_import_t* fmu);

/** \brief Get the tolerance default experiment as specified in the XML file. */
FMILIB_EXPORT double fmi1_import_get_default_experiment_tolerance(fmi1_import_t* fmu);

/** \brief Get the type of the FMU (model exchange or co-simulation) */
FMILIB_EXPORT fmi1_fmu_kind_enu_t fmi1_import_get_fmu_kind(fmi1_import_t* fmu);

/** \brief Get the structure with capability flags.
	@return A pointer to the fmi1_import_capabilities_t allocated within the library. 
			Note that for model exchange FMUs the values of all the flags are always default.
*/
FMILIB_EXPORT fmi1_import_capabilities_t* fmi1_import_get_capabilities(fmi1_import_t* fmu);

/**@} */

#ifdef __cplusplus
}
#endif
#include "fmi1_import_type.h"
#include "fmi1_import_unit.h"
#include "fmi1_import_variable.h"
#include "fmi1_import_vendor_annotations.h"
#include "fmi1_import_capabilities.h"
#include "fmi1_import_cosim.h"
#include "fmi1_import_variable_list.h"
#include "fmi1_import_capi.h"
#include "fmi1_import_convenience.h"
#endif
