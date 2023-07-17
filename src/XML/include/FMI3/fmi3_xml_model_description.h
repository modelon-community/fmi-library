/*
    Copyright (C) 2023 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

/** \file fmi3_xml_model_description.h
*  \brief Public interface to the FMI XML C-library.
*/

#ifndef FMI3_XML_MODELDESCRIPTION_H_
#define FMI3_XML_MODELDESCRIPTION_H_

#include <stddef.h>
#include <JM/jm_callbacks.h>
#include <JM/jm_named_ptr.h>
#include <FMI/fmi_xml_context.h>
#include <FMI3/fmi3_types.h>
#include <FMI3/fmi3_enums.h>
#include <FMI3/fmi3_xml_callbacks.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
\addtogroup fmi_xml
  @{
    \addtogroup fmi3_xml
  @}
*/
/**
  \addtogroup fmi3_xml FMI 3.0 XML parsing library.
   \brief The FMI 3.0 XML parsing library supports processing of model description XML files.
   @{
  \name Declarations of structs used in the interface.
  \brief All the structures used in the interfaces are intended to
   be treated as opaque objects by the client code.
  @{  */

/** \brief Model structure object */
typedef struct fmi3_xml_model_structure_t fmi3_xml_model_structure_t;

/**\name Type definitions supporting structures
@{ */
typedef struct fmi3_xml_float_typedef_t       fmi3_xml_float_typedef_t;
typedef struct fmi3_xml_int_typedef_t         fmi3_xml_int_typedef_t;
typedef struct fmi3_xml_enumeration_typedef_t fmi3_xml_enumeration_typedef_t;
typedef struct fmi3_xml_binary_typedef_t      fmi3_xml_binary_typedef_t;
typedef struct fmi3_xml_clock_typedef_t       fmi3_xml_clock_typedef_t;
typedef struct fmi3_xml_variable_typedef_t    fmi3_xml_variable_typedef_t;

typedef struct fmi3_xml_type_definition_list_t fmi3_xml_type_definition_list_t;
/** @} */

/**\name Scalar Variable types */
/** @{ */
/** General variable type is convenient to unify all the variable list operations */
typedef struct fmi3_xml_variable_t fmi3_xml_variable_t;

/** Typed variables are needed to support specific attributes */
typedef struct fmi3_xml_float_variable_t   fmi3_xml_float_variable_t;
typedef struct fmi3_xml_float64_variable_t fmi3_xml_float64_variable_t;
typedef struct fmi3_xml_float32_variable_t fmi3_xml_float32_variable_t;
typedef struct fmi3_xml_int_variable_t     fmi3_xml_int_variable_t;
typedef struct fmi3_xml_int64_variable_t   fmi3_xml_int64_variable_t;
typedef struct fmi3_xml_int32_variable_t   fmi3_xml_int32_variable_t;
typedef struct fmi3_xml_int16_variable_t   fmi3_xml_int16_variable_t;
typedef struct fmi3_xml_int8_variable_t    fmi3_xml_int8_variable_t;
typedef struct fmi3_xml_uint64_variable_t  fmi3_xml_uint64_variable_t;
typedef struct fmi3_xml_uint32_variable_t  fmi3_xml_uint32_variable_t;
typedef struct fmi3_xml_uint16_variable_t  fmi3_xml_uint16_variable_t;
typedef struct fmi3_xml_uint8_variable_t   fmi3_xml_uint8_variable_t;
typedef struct fmi3_xml_string_variable_t  fmi3_xml_string_variable_t;
typedef struct fmi3_xml_enum_variable_t    fmi3_xml_enum_variable_t;
typedef struct fmi3_xml_bool_variable_t    fmi3_xml_bool_variable_t;
typedef struct fmi3_xml_binary_variable_t  fmi3_xml_binary_variable_t;
typedef struct fmi3_xml_clock_variable_t   fmi3_xml_clock_variable_t;
/** @} */

/** \name Structure encapsulating alias variable information
 * @{
 */
typedef struct fmi3_xml_alias_variable_t fmi3_xml_alias_variable_t;
typedef struct fmi3_xml_alias_variable_list_t fmi3_xml_alias_variable_list_t;
/** @} */

/** \name Structure encapsulating variable dimension information
 * @{
 */
typedef struct fmi3_xml_dimension_t fmi3_xml_dimension_t;
typedef struct fmi3_xml_dimension_list_t fmi3_xml_dimension_list_t;
/** @} */

/** \name Structures encapsulating unit information */
/**@{ */
typedef struct fmi3_xml_unit_t fmi3_xml_unit_t;
typedef struct fmi3_xml_display_unit_t fmi3_xml_display_unit_t;
typedef struct fmi3_xml_unit_definition_list_t fmi3_xml_unit_definition_list_t;
/**@} */

/**\name FMU capabilities flags */
/**@{ */
typedef struct fmi3_xml_capabilities_t fmi3_xml_capabilities_t;
/**@} */
/**    \addtogroup fmi3_xml_gen General information retrieval*/
/**    \addtogroup fmi3_xml_init  Constuction, destruction and error checking */

/** @} */

/**    \addtogroup fmi3_xml_init
@{ */
/**
   \brief Allocate the ModelDescription structure and initialize as empty model.
   @return NULL pointer is returned if memory allocation fails.
   @param callbacks - Standard FMI callbacks may be sent into the module. The argument is optional (pointer can be zero).
*/
fmi3_xml_model_description_t* fmi3_xml_allocate_model_description(jm_callbacks* callbacks);

/**
   \brief Parse XML file
   Repeated calls invalidate the data structures created with the previous call to fmiParseXML,
   i.e., fmiClearModelDescrition is automatically called before reading in the new file.

    @param md A model description object as returned by fmi3_xml_allocate_model_description.
    @param fileName A name (full path) of the XML file name with model definition.
    @param xml_callbacks Callbacks to use for processing annotations (may be NULL).
   @return 0 if parsing was successful. Non-zero value indicates an error.
*/
int fmi3_xml_parse_model_description(fmi3_xml_model_description_t* md,
                                     const char* fileName,
                                     fmi3_xml_callbacks_t* xml_callbacks);

/**
   Clears the data associated with the model description. This is useful if the same object
   instance is used repeatedly to work with different XML files.
    @param md A model description object as returned by fmi3_xml_allocate_model_description.
*/
void fmi3_xml_clear_model_description(fmi3_xml_model_description_t* md);

/*
*    @param md A model description object as returned by fmi3_xml_allocate_model_description.
*    @return 1 if model description is empty and 0 if there is some content associated.
*/
int fmi3_xml_is_model_description_empty(fmi3_xml_model_description_t* md);

/**Error handling:
*  Many functions in the library return pointers to struct. An error is indicated by returning NULL/0-pointer.
*  If error is returned than fmiGetLastError() functions can be used to retrieve the error message.
*  If logging callbacks were specified then the same information is reported via logger.
*  Memory for the error string is allocated and deallocated in the module.
*  Client code should not store the pointer to the string since it can become invalid.
*    @param md A model description object as returned by fmi3_xml_allocate_model_description.
*    @return NULL-terminated string with an error message.
*/
const char* fmi3_xml_get_last_error(fmi3_xml_model_description_t* md);

/**
fmiClearLastError clears the error message .
*/
void fmi3_xml_clear_last_error(fmi3_xml_model_description_t* md);

/**Release the memory allocated
@param md A model description object as returned by fmi3_xml_allocate_model_description.
*/
void fmi3_xml_free_model_description(fmi3_xml_model_description_t* md);

/** @} */
/** \addtogroup fmi3_xml_gen
 * \brief Functions for retrieving general model information. Memory for the strings is allocated and deallocated in the module.
 *   All the functions take a model description object as returned by fmi3_xml_allocate_model_description() as a parameter.
 *   The information is retrieved from the XML file.
 * @{
*/
const char* fmi3_xml_get_model_name(fmi3_xml_model_description_t* md);

const char* fmi3_xml_get_model_identifier_ME(fmi3_xml_model_description_t* md);

const char* fmi3_xml_get_model_identifier_CS(fmi3_xml_model_description_t* md);

const char* fmi3_xml_get_model_identifier_SE(fmi3_xml_model_description_t* md);

const char* fmi3_xml_get_instantiation_token(fmi3_xml_model_description_t* md);

const char* fmi3_xml_get_description(fmi3_xml_model_description_t* md);

const char* fmi3_xml_get_author(fmi3_xml_model_description_t* md);
const char* fmi3_xml_get_license(fmi3_xml_model_description_t* md);

const char* fmi3_xml_get_copyright(fmi3_xml_model_description_t* md);

const char* fmi3_xml_get_model_version(fmi3_xml_model_description_t* md);
const char* fmi3_xml_get_model_standard_version(fmi3_xml_model_description_t* md);
const char* fmi3_xml_get_generation_tool(fmi3_xml_model_description_t* md);
const char* fmi3_xml_get_generation_date_and_time(fmi3_xml_model_description_t* md);

fmi3_variable_naming_convension_enu_t fmi3_xml_get_naming_convention(fmi3_xml_model_description_t* md);

size_t fmi3_xml_get_number_of_continuous_states(fmi3_xml_model_description_t* md);

size_t fmi3_xml_get_number_of_event_indicators(fmi3_xml_model_description_t* md);

int fmi3_xml_get_default_experiment_has_start(fmi3_xml_model_description_t* md);
int fmi3_xml_get_default_experiment_has_stop(fmi3_xml_model_description_t* md);
int fmi3_xml_get_default_experiment_has_tolerance(fmi3_xml_model_description_t* md);
int fmi3_xml_get_default_experiment_has_step_size(fmi3_xml_model_description_t* md);

double fmi3_xml_get_default_experiment_start(fmi3_xml_model_description_t* md);
double fmi3_xml_get_default_experiment_stop(fmi3_xml_model_description_t* md);
double fmi3_xml_get_default_experiment_tolerance(fmi3_xml_model_description_t* md);
double fmi3_xml_get_default_experiment_step_size(fmi3_xml_model_description_t* md);

fmi3_fmu_kind_enu_t fmi3_xml_get_fmu_kind(fmi3_xml_model_description_t* md);

int fmi3_xml_get_cs_has_fixed_internal_step_size(fmi3_xml_model_description_t* md);

double fmi3_xml_get_cs_fixed_internal_step_size(fmi3_xml_model_description_t* md);
int fmi3_xml_get_cs_recommended_intermediate_input_smoothness(fmi3_xml_model_description_t* md);

void fmi3_xml_set_model_description_invalid(fmi3_xml_model_description_t* md); // Use for traceability

/** \brief Get a pointer to the internal capabilities array */
unsigned int* fmi3_xml_get_capabilities(fmi3_xml_model_description_t* md);

/** \brief Get a capability flag by ID */
unsigned int fmi3_xml_get_capability(fmi3_xml_model_description_t* , fmi3_capabilities_enu_t id);

jm_vector(jm_voidp)* fmi3_xml_get_variables_original_order(fmi3_xml_model_description_t* md);

jm_vector(jm_named_ptr)* fmi3_xml_get_variables_alphabetical_order(fmi3_xml_model_description_t* md);

jm_vector(jm_voidp)* fmi3_xml_get_variables_vr_order(fmi3_xml_model_description_t* md);

/**
    \brief Get variable by variable name.
    @param md - the model description
    @param name - variable name
    @return variable pointer.
*/
fmi3_xml_variable_t* fmi3_xml_get_variable_by_name(fmi3_xml_model_description_t* md, const char* name);

/**
    \brief Get variable by value reference.
    @param md - the model description
    @param vr - value reference
    @return variable pointer.
*/
fmi3_xml_variable_t* fmi3_xml_get_variable_by_vr(fmi3_xml_model_description_t* md, fmi3_value_reference_t vr);

/** \brief Get the number of vendors that had annotations in the XML*/
size_t fmi3_xml_get_vendors_num(fmi3_xml_model_description_t* md);

/** \brief Get the name of the vendor with that had annotations in the XML by index */
const char* fmi3_xml_get_vendor_name(fmi3_xml_model_description_t* md, size_t index);

/** \brief Get the log categories defined in the XML */
jm_vector(jm_string)* fmi3_xml_get_log_categories(fmi3_xml_model_description_t* md);

/** \brief Get descriptions for the log categories defined in the XML */
jm_vector(jm_string)* fmi3_xml_get_log_category_descriptions(fmi3_xml_model_description_t* md);

/** \brief Get the source files for ME defined in the XML */
jm_vector(jm_string)* fmi3_xml_get_source_files_me(fmi3_xml_model_description_t* md);

/** \brief Get the source files for CS defined in the XML */
jm_vector(jm_string)* fmi3_xml_get_source_files_cs(fmi3_xml_model_description_t* md);

/** \brief Get the model structure pointer. NULL pointer means there was no information present in the XML */
fmi3_xml_model_structure_t* fmi3_xml_get_model_structure(fmi3_xml_model_description_t* md);

/** @} */
#ifdef __cplusplus
}
#endif

#include "fmi3_xml_type.h"
#include "fmi3_xml_unit.h"
#include "fmi3_xml_variable.h"
#include "fmi3_xml_capabilities.h"
#include "fmi3_xml_cosim.h"
#include "fmi3_xml_model_structure.h"

#endif
