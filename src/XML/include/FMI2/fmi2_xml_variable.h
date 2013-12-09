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



/** \file fmi2_xml_variable.h
*  \brief Public interface to the FMI XML C-library. Handling of model variables.
*/

#ifndef FMI2_XML_VARIABLE_H_
#define FMI2_XML_VARIABLE_H_

#include "fmi2_xml_model_description.h"
#include "fmi2_xml_type.h"


#ifdef __cplusplus
extern "C" {
#endif
		/**
	\addtogroup fmi2_xml
	@{
	\addtogroup fmi2_xml_variables Functions for handling variable definitions.
	@}
	\addtogroup fmi2_xml_variables Functions for handling variable definitions.
	@{
	*/
const char* fmi2_xml_get_variable_name(fmi2_xml_variable_t*);
const char* fmi2_xml_get_variable_description(fmi2_xml_variable_t*);

size_t fmi2_xml_get_variable_original_order(fmi2_xml_variable_t* v);

fmi2_value_reference_t fmi2_xml_get_variable_vr(fmi2_xml_variable_t*);

/*
    For scalar variable gives the type definition is present
*/
fmi2_xml_variable_typedef_t* fmi2_xml_get_variable_declared_type(fmi2_xml_variable_t*);
fmi2_base_type_enu_t fmi2_xml_get_variable_base_type(fmi2_xml_variable_t*);

int   fmi2_xml_get_variable_has_start(fmi2_xml_variable_t*);

fmi2_variability_enu_t fmi2_xml_get_variability(fmi2_xml_variable_t*);
fmi2_causality_enu_t fmi2_xml_get_causality(fmi2_xml_variable_t*);
fmi2_initial_enu_t fmi2_xml_get_initial(fmi2_xml_variable_t*);

fmi2_xml_variable_t* fmi2_xml_get_previous(fmi2_xml_variable_t* v);
fmi2_boolean_t fmi2_xml_get_canHandleMultipleSetPerTimeInstant(fmi2_xml_variable_t* v);

fmi2_xml_real_variable_t* fmi2_xml_get_variable_as_real(fmi2_xml_variable_t*);
fmi2_xml_integer_variable_t* fmi2_xml_get_variable_as_integer(fmi2_xml_variable_t*);
fmi2_xml_enum_variable_t* fmi2_xml_get_variable_as_enum(fmi2_xml_variable_t*);
fmi2_xml_string_variable_t* fmi2_xml_get_variable_as_string(fmi2_xml_variable_t*);
fmi2_xml_bool_variable_t* fmi2_xml_get_variable_as_boolean(fmi2_xml_variable_t*);

double fmi2_xml_get_real_variable_start(fmi2_xml_real_variable_t* v);
fmi2_xml_real_variable_t* fmi2_xml_get_real_variable_derivative_of(fmi2_xml_real_variable_t* v);
fmi2_boolean_t fmi2_xml_get_real_variable_reinit(fmi2_xml_real_variable_t* v);
double fmi2_xml_get_real_variable_max(fmi2_xml_real_variable_t* v);
double fmi2_xml_get_real_variable_min(fmi2_xml_real_variable_t* v);
double fmi2_xml_get_real_variable_nominal(fmi2_xml_real_variable_t* v);
fmi2_xml_unit_t* fmi2_xml_get_real_variable_unit(fmi2_xml_real_variable_t* v);
fmi2_xml_display_unit_t* fmi2_xml_get_real_variable_display_unit(fmi2_xml_real_variable_t* v);

const char* fmi2_xml_get_string_variable_start(fmi2_xml_string_variable_t* v);
fmi2_boolean_t fmi2_xml_get_boolean_variable_start(fmi2_xml_bool_variable_t* v);

int fmi2_xml_get_integer_variable_start(fmi2_xml_integer_variable_t* v);
int fmi2_xml_get_integer_variable_min(fmi2_xml_integer_variable_t* v);
int fmi2_xml_get_integer_variable_max(fmi2_xml_integer_variable_t* v);

int fmi2_xml_get_enum_variable_start(fmi2_xml_enum_variable_t* v);
int fmi2_xml_get_enum_variable_min(fmi2_xml_enum_variable_t* v);
int fmi2_xml_get_enum_variable_max(fmi2_xml_enum_variable_t* v);


fmi2_variable_alias_kind_enu_t fmi2_xml_get_variable_alias_kind(fmi2_xml_variable_t*);
fmi2_xml_variable_t* fmi2_xml_get_variable_alias_base(fmi2_xml_model_description_t* md,fmi2_xml_variable_t*);

/**
    Return the list of all the variables aliased to the given one (including the base one.
    The list is ordered: base variable, aliases, negated aliases.
*/
jm_status_enu_t fmi2_xml_get_variable_aliases(fmi2_xml_model_description_t* md, fmi2_xml_variable_t*, jm_vector(jm_voidp)*);
/* fmi2_xml_variable_list_t* fmi2_xml_get_variable_aliases(fmi2_xml_model_description_t* md,fmi2_xml_variable_t*); */

/**
@}
*/
#ifdef __cplusplus
}
#endif
#endif
