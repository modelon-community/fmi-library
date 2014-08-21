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



/** \file fmi2_xml_model_description.h
*  \brief Public interface to the FMI XML C-library.
*/

#ifndef FMI2_XML_MODELDESCRIPTION_H_
#define FMI2_XML_MODELDESCRIPTION_H_

#include <stddef.h>
#include <JM/jm_callbacks.h>
#include <JM/jm_named_ptr.h>
#include <FMI/fmi_xml_context.h>
#include <FMI2/fmi2_types.h>
#include <FMI2/fmi2_enums.h>
#include <FMI2/fmi2_xml_callbacks.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
\addtogroup fmi_xml
  @{
	\addtogroup fmi2_xml
  @}
*/
/**
  \addtogroup fmi2_xml FMI 2.0 XML parsing library.
   \brief The FMI 2.0 XML parsing library supports processing of model description XML files.
   @{
  \name Declarations of structs used in the interface.
  \brief All the structures used in the interfaces are intended to
   be treated as opaque objects by the client code.
  @{  */

/** \brief Model structure object */
typedef struct fmi2_xml_model_structure_t fmi2_xml_model_structure_t;

/**\name  Type definitions supporting structures
@{ */
typedef struct fmi2_xml_real_typedef_t fmi2_xml_real_typedef_t;
typedef struct fmi2_xml_integer_typedef_t fmi2_xml_integer_typedef_t;
typedef struct fmi2_xml_enumeration_typedef_t fmi2_xml_enumeration_typedef_t;
typedef struct fmi2_xml_variable_typedef_t fmi2_xml_variable_typedef_t;

typedef struct fmi2_xml_type_definitions_t fmi2_xml_type_definitions_t;
/** @} */

/**\name Scalar Variable types */
/** @{ */
/**General variable type is convenien to unify all the variable list operations */
typedef struct fmi2_xml_variable_t fmi2_xml_variable_t;

/**Typed variables are needed to support specific attributes */
typedef struct fmi2_xml_real_variable_t fmi2_xml_real_variable_t;
typedef struct fmi2_xml_integer_variable_t fmi2_xml_integer_variable_t;
typedef struct fmi2_xml_string_variable_t fmi2_xml_string_variable_t;
typedef struct fmi2_xml_enum_variable_t fmi2_xml_enum_variable_t;
typedef struct fmi2_xml_bool_variable_t fmi2_xml_bool_variable_t;
/** @} */

/**\name Structures encapsulating unit information */
/**@{ */
typedef struct fmi2_xml_unit_t fmi2_xml_unit_t;
typedef struct fmi2_xml_display_unit_t fmi2_xml_display_unit_t;
typedef struct fmi2_xml_unit_definitions_t fmi2_xml_unit_definitions_t;
/**@} */

/**\name FMU capabilities flags */
/**@{ */
typedef struct fmi2_xml_capabilities_t fmi2_xml_capabilities_t;
/**@} */
/**	\addtogroup fmi2_xml_gen General information retrieval*/
/**	\addtogroup fmi2_xml_init  Constuction, destruction and error checking */

/** @} */

/**	\addtogroup fmi2_xml_init
@{ */
/**
   \brief Allocate the ModelDescription structure and initialize as empty model.
   @return NULL pointer is returned if memory allocation fails.
   @param callbacks - Standard FMI callbacks may be sent into the module. The argument is optional (pointer can be zero).
*/
fmi2_xml_model_description_t* fmi2_xml_allocate_model_description( jm_callbacks* callbacks);

/**
   \brief Parse XML file
   Repeaded calls invalidate the data structures created with the previous call to fmiParseXML,
   i.e., fmiClearModelDescrition is automatically called before reading in the new file.

    @param md A model description object as returned by fmi2_xml_allocate_model_description.
    @param fileName A name (full path) of the XML file name with model definition.
	@param xml_callbacks Callbacks to use for processing annotations (may be NULL).
   @return 0 if parsing was successfull. Non-zero value indicates an error.
*/
int fmi2_xml_parse_model_description( fmi2_xml_model_description_t* md, const char* fileName, fmi2_xml_callbacks_t* xml_callbacks);

/**
   Clears the data associated with the model description. This is useful if the same object
   instance is used repeatedly to work with different XML files.
    @param md A model description object as returned by fmi2_xml_allocate_model_description.
*/
void fmi2_xml_clear_model_description( fmi2_xml_model_description_t* md);

/*
*    @param md A model description object as returned by fmi2_xml_allocate_model_description.
*    @return 1 if model description is empty and 0 if there is some content associated.
*/
int fmi2_xml_is_model_description_empty(fmi2_xml_model_description_t* md);

/**Error handling:
*  Many functions in the library return pointers to struct. An error is indicated by returning NULL/0-pointer.
*  If error is returned than fmiGetLastError() functions can be used to retrieve the error message.
*  If logging callbacks were specified then the same information is reported via logger.
*  Memory for the error string is allocated and deallocated in the module.
*  Client code should not store the pointer to the string since it can become invalid.
*    @param md A model description object as returned by fmi2_xml_allocate_model_description.
*    @return NULL-terminated string with an error message.
*/
const char* fmi2_xml_get_last_error(fmi2_xml_model_description_t* md);

/**
fmiClearLastError clears the error message .
*/
void fmi2_xml_clear_last_error(fmi2_xml_model_description_t* md);

/**Release the memory allocated
@param md A model description object as returned by fmi2_xml_allocate_model_description.
*/
void fmi2_xml_free_model_description(fmi2_xml_model_description_t* md);

/** @} */
/** \addtogroup fmi2_xml_gen
 * \brief Functions for retrieving general model information. Memory for the strings is allocated and deallocated in the module.
 *   All the functions take a model description object as returned by fmi2_xml_allocate_model_description() as a parameter. 
 *   The information is retrieved from the XML file.
 * @{
*/
const char* fmi2_xml_get_model_name(fmi2_xml_model_description_t* md);

const char* fmi2_xml_get_model_identifier_ME(fmi2_xml_model_description_t* md);

const char* fmi2_xml_get_model_identifier_CS(fmi2_xml_model_description_t* md);

const char* fmi2_xml_get_GUID(fmi2_xml_model_description_t* md);

const char* fmi2_xml_get_description(fmi2_xml_model_description_t* md);

const char* fmi2_xml_get_author(fmi2_xml_model_description_t* md);
const char* fmi2_xml_get_license(fmi2_xml_model_description_t* md);

const char* fmi2_xml_get_copyright(fmi2_xml_model_description_t* md);

const char* fmi2_xml_get_model_version(fmi2_xml_model_description_t* md);
const char* fmi2_xml_get_model_standard_version(fmi2_xml_model_description_t* md);
const char* fmi2_xml_get_generation_tool(fmi2_xml_model_description_t* md);
const char* fmi2_xml_get_generation_date_and_time(fmi2_xml_model_description_t* md);

fmi2_variable_naming_convension_enu_t fmi2_xml_get_naming_convention(fmi2_xml_model_description_t* md);

size_t fmi2_xml_get_number_of_continuous_states(fmi2_xml_model_description_t* md);

size_t fmi2_xml_get_number_of_event_indicators(fmi2_xml_model_description_t* md);

double fmi2_xml_get_default_experiment_start(fmi2_xml_model_description_t* md);

double fmi2_xml_get_default_experiment_stop(fmi2_xml_model_description_t* md);

double fmi2_xml_get_default_experiment_tolerance(fmi2_xml_model_description_t* md);

double fmi2_xml_get_default_experiment_step(fmi2_xml_model_description_t* md);

fmi2_fmu_kind_enu_t fmi2_xml_get_fmu_kind(fmi2_xml_model_description_t* md);

/** \brief Get a pointer to the internal capabilities array */
unsigned int* fmi2_xml_get_capabilities(fmi2_xml_model_description_t* md);

/** \brief Get a capability flag by ID */
unsigned int fmi2_xml_get_capability(fmi2_xml_model_description_t* , fmi2_capabilities_enu_t id);

jm_vector(jm_voidp)* fmi2_xml_get_variables_original_order(fmi2_xml_model_description_t* md);

jm_vector(jm_named_ptr)* fmi2_xml_get_variables_alphabetical_order(fmi2_xml_model_description_t* md);

jm_vector(jm_voidp)* fmi2_xml_get_variables_vr_order(fmi2_xml_model_description_t* md);

/**
	\brief Get variable by variable name.
	\param md - the model description
	\param name - variable name
	\return variable pointer.
*/
fmi2_xml_variable_t* fmi2_xml_get_variable_by_name(fmi2_xml_model_description_t* md, const char* name);

/**
	\brief Get variable by value reference.
	\param md - the model description
	\param baseType - basic data type
	\param vr - value reference
	\return variable pointer.
*/
fmi2_xml_variable_t* fmi2_xml_get_variable_by_vr(fmi2_xml_model_description_t* md, fmi2_base_type_enu_t baseType, fmi2_value_reference_t vr);

/** \brief Get the number of vendors that had annotations in the XML*/
size_t fmi2_xml_get_vendors_num(fmi2_xml_model_description_t* md);

/** \brief Get the name of the vendor with that had annotations in the XML by index */
const char* fmi2_xml_get_vendor_name(fmi2_xml_model_description_t* md, size_t  index);

/** \brief Get the log categories defined in the XML */
jm_vector(jm_string)* fmi2_xml_get_log_categories(fmi2_xml_model_description_t* md);

/** \brief Get descriptions for the log categories defined in the XML */
jm_vector(jm_string)* fmi2_xml_get_log_category_descriptions(fmi2_xml_model_description_t* md);

/** \brief Get the source files for ME defined in the XML */
jm_vector(jm_string)* fmi2_xml_get_source_files_me(fmi2_xml_model_description_t* md);

/** \brief Get the source files for CS defined in the XML */
jm_vector(jm_string)* fmi2_xml_get_source_files_cs(fmi2_xml_model_description_t* md);

/** \brief Get the model structure pointer. NULL pointer means there was no information present in the XML */
fmi2_xml_model_structure_t* fmi2_xml_get_model_structure(fmi2_xml_model_description_t* md);

/** @} */
#ifdef __cplusplus
}
#endif

#include "fmi2_xml_type.h"
#include "fmi2_xml_unit.h"
#include "fmi2_xml_variable.h"
#include "fmi2_xml_capabilities.h"
#include "fmi2_xml_cosim.h"
#include "fmi2_xml_model_structure.h"

#endif
