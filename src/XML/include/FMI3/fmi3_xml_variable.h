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



/** \file fmi3_xml_variable.h
*  \brief Public interface to the FMI XML C-library. Handling of model variables.
*/

#ifndef FMI3_XML_VARIABLE_H_
#define FMI3_XML_VARIABLE_H_

#include "fmi3_xml_model_description.h"
#include "fmi3_xml_type.h"
#include "fmi3_xml_dimension.h"

#ifdef __cplusplus
extern "C" {
#endif
		/**
	\addtogroup fmi3_xml
	@{
	\addtogroup fmi3_xml_variables Functions for handling variable definitions.
	@}
	\addtogroup fmi3_xml_variables Functions for handling variable definitions.
	@{
	*/
const char* fmi3_xml_get_variable_name(fmi3_xml_variable_t*);
const char* fmi3_xml_get_variable_description(fmi3_xml_variable_t*);

size_t fmi3_xml_get_variable_original_order(fmi3_xml_variable_t* v);

fmi3_value_reference_t fmi3_xml_get_variable_vr(fmi3_xml_variable_t*);

/*
    For scalar variable gives the type definition is present
*/
fmi3_xml_variable_typedef_t* fmi3_xml_get_variable_declared_type(fmi3_xml_variable_t*);
fmi3_base_type_enu_t fmi3_xml_get_variable_base_type(fmi3_xml_variable_t*);

fmi3_float64_t* fmi3_xml_get_float64_variable_start_array(fmi3_xml_float64_variable_t* v);
fmi3_float32_t* fmi3_xml_get_float32_variable_start_array(fmi3_xml_float32_variable_t* v);

jm_vector(fmi3_xml_dimension_t)* fmi3_xml_get_variable_dimension_vector(fmi3_xml_variable_t* v);

int fmi3_xml_variable_is_array(fmi3_xml_variable_t* v);

int fmi3_xml_get_variable_has_start(fmi3_xml_variable_t*);

fmi3_variability_enu_t fmi3_xml_get_variability(fmi3_xml_variable_t*);
fmi3_causality_enu_t fmi3_xml_get_causality(fmi3_xml_variable_t*);
fmi3_initial_enu_t fmi3_xml_get_initial(fmi3_xml_variable_t*);

fmi3_xml_variable_t* fmi3_xml_get_previous(fmi3_xml_variable_t* v);
fmi3_boolean_t fmi3_xml_get_canHandleMultipleSetPerTimeInstant(fmi3_xml_variable_t* v);

fmi3_xml_float64_variable_t* fmi3_xml_get_variable_as_float64(fmi3_xml_variable_t*);
fmi3_xml_float32_variable_t* fmi3_xml_get_variable_as_float32(fmi3_xml_variable_t*);
fmi3_xml_real_variable_t* fmi3_xml_get_variable_as_real(fmi3_xml_variable_t*);
fmi3_xml_integer_variable_t* fmi3_xml_get_variable_as_integer(fmi3_xml_variable_t*);
fmi3_xml_enum_variable_t* fmi3_xml_get_variable_as_enum(fmi3_xml_variable_t*);
fmi3_xml_string_variable_t* fmi3_xml_get_variable_as_string(fmi3_xml_variable_t*);
fmi3_xml_bool_variable_t* fmi3_xml_get_variable_as_boolean(fmi3_xml_variable_t*);

 /* include generated prototypes */
#include "gen/FMI3/fmi3_xml_variable_types_h.c"

/**
 * real 
 */
fmi3_xml_real_variable_t* fmi3_xml_get_real_variable_derivative_of(fmi3_xml_real_variable_t* v); 
fmi3_boolean_t fmi3_xml_get_real_variable_reinit(fmi3_xml_real_variable_t* v); 
fmi3_xml_unit_t* fmi3_xml_get_real_variable_unit(fmi3_xml_real_variable_t* v); 
fmi3_xml_display_unit_t* fmi3_xml_get_real_variable_display_unit(fmi3_xml_real_variable_t* v); 
fmi3_real_t fmi3_xml_get_real_variable_start (fmi3_xml_real_variable_t* v); 
fmi3_real_t fmi3_xml_get_real_variable_min (fmi3_xml_real_variable_t* v); 
fmi3_real_t fmi3_xml_get_real_variable_max (fmi3_xml_real_variable_t* v); 
fmi3_real_t fmi3_xml_get_real_variable_nominal (fmi3_xml_real_variable_t* v); 

double fmi3_xml_get_real_variable_start(fmi3_xml_real_variable_t* v);
fmi3_xml_real_variable_t* fmi3_xml_get_real_variable_derivative_of(fmi3_xml_real_variable_t* v);
fmi3_boolean_t fmi3_xml_get_real_variable_reinit(fmi3_xml_real_variable_t* v);
double fmi3_xml_get_real_variable_max(fmi3_xml_real_variable_t* v);
double fmi3_xml_get_real_variable_min(fmi3_xml_real_variable_t* v);
double fmi3_xml_get_real_variable_nominal(fmi3_xml_real_variable_t* v);
fmi3_xml_unit_t* fmi3_xml_get_real_variable_unit(fmi3_xml_real_variable_t* v);
fmi3_xml_display_unit_t* fmi3_xml_get_real_variable_display_unit(fmi3_xml_real_variable_t* v);

const char* fmi3_xml_get_string_variable_start(fmi3_xml_string_variable_t* v);
fmi3_boolean_t fmi3_xml_get_boolean_variable_start(fmi3_xml_bool_variable_t* v);

int fmi3_xml_get_integer_variable_start(fmi3_xml_integer_variable_t* v);
int fmi3_xml_get_integer_variable_min(fmi3_xml_integer_variable_t* v);
int fmi3_xml_get_integer_variable_max(fmi3_xml_integer_variable_t* v);

int fmi3_xml_get_enum_variable_start(fmi3_xml_enum_variable_t* v);
int fmi3_xml_get_enum_variable_min(fmi3_xml_enum_variable_t* v);
int fmi3_xml_get_enum_variable_max(fmi3_xml_enum_variable_t* v);


fmi3_variable_alias_kind_enu_t fmi3_xml_get_variable_alias_kind(fmi3_xml_variable_t*);
fmi3_xml_variable_t* fmi3_xml_get_variable_alias_base(fmi3_xml_model_description_t* md,fmi3_xml_variable_t*);

void fmi3_xml_variable_free_internals(jm_callbacks* callbacks, fmi3_xml_variable_t* var);

/**
    Return the list of all the variables aliased to the given one (including the base one.
    The list is ordered: base variable, aliases, negated aliases.
*/
jm_status_enu_t fmi3_xml_get_variable_aliases(fmi3_xml_model_description_t* md, fmi3_xml_variable_t*, jm_vector(jm_voidp)*);
/* fmi3_xml_variable_list_t* fmi3_xml_get_variable_aliases(fmi3_xml_model_description_t* md,fmi3_xml_variable_t*); */

/**
@}
*/
#ifdef __cplusplus
}
#endif
#endif
