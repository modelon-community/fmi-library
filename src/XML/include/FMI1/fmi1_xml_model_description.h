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



/** \file fmi1_xml_model_description.h
*  \brief Public interface to the FMI XML C-library.
*/

#ifndef FMI1_XML_MODELDESCRIPTION_H_
#define FMI1_XML_MODELDESCRIPTION_H_

#include <stddef.h>
#include <JM/jm_callbacks.h>
#include <JM/jm_named_ptr.h>
#include <FMI/fmi_xml_context.h>
#include <FMI1/fmi1_types.h>
#include <FMI1/fmi1_enums.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
\addtogroup fmi_xml
  @{
	\addtogroup fmi1_xml
  @}
*/
/**
  \addtogroup fmi1_xml FMI 1.0 XML parsing library.
   \brief The FMI 1.0 XML parsing library supports processing of model description XML files.
   @{
  \name Declarations of structs used in the interface.
  \brief All the structures used in the interfaces are intended to
   be treated as opaque objects by the client code.
  @{  */

/**\name Vendor annotation supporting structures
 * @{ 
 */
typedef struct fmi1_xml_vendor_list_t fmi1_xml_vendor_list_t;
typedef struct fmi1_xml_vendor_t fmi1_xml_vendor_t;
typedef struct fmi1_xml_annotation_t fmi1_xml_annotation_t;
/** @} */

/**\name  Type definitions supporting structures
@{ */
typedef struct fmi1_xml_real_typedef_t fmi1_xml_real_typedef_t;
typedef struct fmi1_xml_integer_typedef_t fmi1_xml_integer_typedef_t;
typedef struct fmi1_xml_enumeration_typedef_t fmi1_xml_enumeration_typedef_t;
typedef struct fmi1_xml_variable_typedef_t fmi1_xml_variable_typedef_t;

typedef struct fmi1_xml_type_definitions_t fmi1_xml_type_definitions_t;
/** @} */

/**\name Scalar Variable types */
/** @{ */
/**General variable type is convenien to unify all the variable list operations */
typedef struct fmi1_xml_variable_t fmi1_xml_variable_t;

/**Typed variables are needed to support specific attributes */
typedef struct fmi1_xml_real_variable_t fmi1_xml_real_variable_t;
typedef struct fmi1_xml_integer_variable_t fmi1_xml_integer_variable_t;
typedef struct fmi1_xml_string_variable_t fmi1_xml_string_variable_t;
typedef struct fmi1_xml_enum_variable_t fmi1_xml_enum_variable_t;
typedef struct fmi1_xml_bool_variable_t fmi1_xml_bool_variable_t;
/** @} */

/**\name Structures encapsulating unit information */
/**@{ */
typedef struct fmi1_xml_unit_t fmi1_xml_unit_t;
typedef struct fmi1_xml_display_unit_t fmi1_xml_display_unit_t;
typedef struct fmi1_xml_unit_definitions_t fmi1_xml_unit_definitions_t;
/**@} */

/**\name FMU capabilities flags */
/**@{ */
typedef struct fmi1_xml_capabilities_t fmi1_xml_capabilities_t;
/**@} */
/**	\addtogroup fmi1_xml_gen General information retrieval*/
/**	\addtogroup fmi1_xml_init  Constuction, destruction and error checking */

/** @} */

/**	\addtogroup fmi1_xml_init
@{ */
/**
   \brief Allocate the ModelDescription structure and initialize as empty model.
   @return NULL pointer is returned if memory allocation fails.
   @param callbacks - Standard FMI callbacks may be sent into the module. The argument is optional (pointer can be zero).
*/
fmi1_xml_model_description_t* fmi1_xml_allocate_model_description( jm_callbacks* callbacks);

/**
   \brief Parse XML file
   Repeaded calls invalidate the data structures created with the previous call to fmiParseXML,
   i.e., fmiClearModelDescrition is automatically called before reading in the new file.

    @param md A model description object as returned by fmi1_xml_allocate_model_description.
    @param fileName A name (full path) of the XML file name with model definition.
   @return 0 if parsing was successfull. Non-zero value indicates an error.
*/
int fmi1_xml_parse_model_description( fmi1_xml_model_description_t* md, const char* fileName);

/**
   Clears the data associated with the model description. This is useful if the same object
   instance is used repeatedly to work with different XML files.
    @param md A model description object as returned by fmi1_xml_allocate_model_description.
*/
void fmi1_xml_clear_model_description( fmi1_xml_model_description_t* md);

/*
*    @param md A model description object as returned by fmi1_xml_allocate_model_description.
*    @return 1 if model description is empty and 0 if there is some content associated.
*/
int fmi1_xml_is_model_description_empty(fmi1_xml_model_description_t* md);

/**Error handling:
*  Many functions in the library return pointers to struct. An error is indicated by returning NULL/0-pointer.
*  If error is returned than fmiGetLastError() functions can be used to retrieve the error message.
*  If logging callbacks were specified then the same information is reported via logger.
*  Memory for the error string is allocated and deallocated in the module.
*  Client code should not store the pointer to the string since it can become invalid.
*    @param md A model description object as returned by fmi1_xml_allocate_model_description.
*    @return NULL-terminated string with an error message.
*/
const char* fmi1_xml_get_last_error(fmi1_xml_model_description_t* md);

/**
fmiClearLastError clears the error message .
*/
void fmi1_xml_clear_last_error(fmi1_xml_model_description_t* md);

/**Release the memory allocated
@param md A model description object as returned by fmi1_xml_allocate_model_description.
*/
void fmi1_xml_free_model_description(fmi1_xml_model_description_t* md);

/** @} */
/** \addtogroup fmi1_xml_gen
 * \brief Functions for retrieving general model information. Memory for the strings is allocated and deallocated in the module.
 *   All the functions take a model description object as returned by fmi1_xml_allocate_model_description() as a parameter. 
 *   The information is retrieved from the XML file.
 * @{
*/
const char* fmi1_xml_get_model_name(fmi1_xml_model_description_t* md);

const char* fmi1_xml_get_model_identifier(fmi1_xml_model_description_t* md);

const char* fmi1_xml_get_GUID(fmi1_xml_model_description_t* md);

const char* fmi1_xml_get_description(fmi1_xml_model_description_t* md);

const char* fmi1_xml_get_author(fmi1_xml_model_description_t* md);

const char* fmi1_xml_get_model_version(fmi1_xml_model_description_t* md);
const char* fmi1_xml_get_model_standard_version(fmi1_xml_model_description_t* md);
const char* fmi1_xml_get_generation_tool(fmi1_xml_model_description_t* md);
const char* fmi1_xml_get_generation_date_and_time(fmi1_xml_model_description_t* md);

fmi1_variable_naming_convension_enu_t fmi1_xml_get_naming_convention(fmi1_xml_model_description_t* md);

unsigned int fmi1_xml_get_number_of_continuous_states(fmi1_xml_model_description_t* md);

unsigned int fmi1_xml_get_number_of_event_indicators(fmi1_xml_model_description_t* md);

double fmi1_xml_get_default_experiment_start(fmi1_xml_model_description_t* md);

void fmi1_xml_set_default_experiment_start(fmi1_xml_model_description_t* md, double);

double fmi1_xml_get_default_experiment_stop(fmi1_xml_model_description_t* md);

void fmi1_xml_set_default_experiment_stop(fmi1_xml_model_description_t* md, double);

double fmi1_xml_get_default_experiment_tolerance(fmi1_xml_model_description_t* md);

void fmi1_xml_set_default_experiment_tolerance(fmi1_xml_model_description_t* md, double);

fmi1_fmu_kind_enu_t fmi1_xml_get_fmu_kind(fmi1_xml_model_description_t* md);

fmi1_xml_capabilities_t* fmi1_xml_get_capabilities(fmi1_xml_model_description_t* md);

jm_vector(jm_voidp)* fmi1_xml_get_variables_original_order(fmi1_xml_model_description_t* md);

jm_vector(jm_named_ptr)* fmi1_xml_get_variables_alphabetical_order(fmi1_xml_model_description_t* md);

jm_vector(jm_voidp)* fmi1_xml_get_variables_vr_order(fmi1_xml_model_description_t* md);

/**
	\brief Get variable by variable name.
	\param md - the model description
	\param name - variable name
	\return variable pointer.
*/
fmi1_xml_variable_t* fmi1_xml_get_variable_by_name(fmi1_xml_model_description_t* md, const char* name);

/**
	\brief Get variable by value reference.
	\param md - the model description
	\param baseType - basic data type
	\param vr - value reference
	\return variable pointer.
*/
fmi1_xml_variable_t* fmi1_xml_get_variable_by_vr(fmi1_xml_model_description_t* md, fmi1_base_type_enu_t baseType, fmi1_value_reference_t vr);

/** @} */
#ifdef __cplusplus
}
#endif

#include "fmi1_xml_type.h"
#include "fmi1_xml_unit.h"
#include "fmi1_xml_variable.h"
#include "fmi1_xml_vendor_annotations.h"
#include "fmi1_xml_capabilities.h"
#include "fmi1_xml_cosim.h"

#endif
