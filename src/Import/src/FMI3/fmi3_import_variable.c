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



/** \file fmi3_import_variable.c
*  \brief Methods to handle fmi3_import_variable_t.
*/

#include <FMI3/fmi3_import_variable.h>
#include "fmi3_import_impl.h"
#include "fmi3_import_variable_list_impl.h"

fmi3_import_variable_t* fmi3_import_get_variable_by_name(fmi3_import_t* fmu, const char* name) {
	return fmi3_xml_get_variable_by_name(fmu->md, name);
}

fmi3_import_variable_t* fmi3_import_get_variable_by_vr(fmi3_import_t* fmu, fmi3_base_type_enu_t baseType, fmi3_value_reference_t vr) {
	return fmi3_xml_get_variable_by_vr(fmu->md, baseType, vr);
}

const char* fmi3_import_get_variable_name(fmi3_import_variable_t* v) {
	return fmi3_xml_get_variable_name(v); 
}

const char* fmi3_import_get_variable_description(fmi3_import_variable_t* v) {
	return fmi3_xml_get_variable_description(v);
}

fmi3_value_reference_t fmi3_import_get_variable_vr(fmi3_import_variable_t* v) {
	return fmi3_xml_get_variable_vr(v);
}

/*
    For scalar variable gives the type definition is present
*/
fmi3_import_variable_typedef_t* fmi3_import_get_variable_declared_type(fmi3_import_variable_t* v) {
	return fmi3_xml_get_variable_declared_type(v);
}

fmi3_base_type_enu_t fmi3_import_get_variable_base_type(fmi3_import_variable_t* v) {
	return fmi3_xml_get_variable_base_type(v);
}

int   fmi3_import_get_variable_has_start(fmi3_import_variable_t* v) {
	return fmi3_xml_get_variable_has_start(v);
}

fmi3_variability_enu_t fmi3_import_get_variability(fmi3_import_variable_t* v) {
	return fmi3_xml_get_variability(v);
}

fmi3_causality_enu_t fmi3_import_get_causality(fmi3_import_variable_t* v) {
	return fmi3_xml_get_causality(v);
}

fmi3_initial_enu_t fmi3_import_get_initial(fmi3_import_variable_t* v) {
	return fmi3_xml_get_initial(v);
}

fmi3_import_variable_t* fmi3_import_get_previous(fmi3_import_variable_t* v) {
	return fmi3_xml_get_previous(v);    
}

fmi3_boolean_t fmi3_import_get_canHandleMultipleSetPerTimeInstant(fmi3_import_variable_t* v) {
	return fmi3_xml_get_canHandleMultipleSetPerTimeInstant(v);
}

fmi3_import_float64_variable_t* fmi3_import_get_variable_as_float64(fmi3_import_variable_t* v) {
	return fmi3_xml_get_variable_as_float64(v);
}

fmi3_import_float32_variable_t* fmi3_import_get_variable_as_float32(fmi3_import_variable_t* v) {
	return fmi3_xml_get_variable_as_float32(v);
}

fmi3_import_real_variable_t* fmi3_import_get_variable_as_real(fmi3_import_variable_t* v) {
	return fmi3_xml_get_variable_as_real(v);
}

fmi3_import_integer_variable_t* fmi3_import_get_variable_as_integer(fmi3_import_variable_t* v) {
	return fmi3_xml_get_variable_as_integer(v);
}

fmi3_import_enum_variable_t* fmi3_import_get_variable_as_enum(fmi3_import_variable_t* v) {
	return fmi3_xml_get_variable_as_enum(v);
}

fmi3_import_string_variable_t* fmi3_import_get_variable_as_string(fmi3_import_variable_t* v) {
	return fmi3_xml_get_variable_as_string(v);
}

fmi3_import_bool_variable_t* fmi3_import_get_variable_as_boolean(fmi3_import_variable_t* v) {
	return fmi3_xml_get_variable_as_boolean(v);
}

/* float */

/* Macro for generating variable value getters. Use leading underscore for variable value param, */
/* since otherwise there's a risk of using the 'min' or 'max' macros. */
#define def_import_get_variable_XX(XX, TYPE) \
    fmi3_##TYPE##_t fmi3_import_get_##TYPE##_variable##XX(fmi3_import_##TYPE##_variable_t* v) { \
        return fmi3_xml_get_##TYPE##_variable##XX(v); \
    }

def_import_get_variable_XX(_start,      float32)
def_import_get_variable_XX(_min,        float32)
def_import_get_variable_XX(_max,        float32)
def_import_get_variable_XX(_nominal,    float32)

def_import_get_variable_XX(_start,      float64)
def_import_get_variable_XX(_min,        float64)
def_import_get_variable_XX(_max,        float64)
def_import_get_variable_XX(_nominal,    float64)

#undef def_import_get_variable_XX

fmi3_real_t fmi3_import_get_real_variable_start(fmi3_import_real_variable_t* v) {
	return fmi3_xml_get_real_variable_start(v);
}

fmi3_import_real_variable_t* fmi3_import_get_real_variable_derivative_of(fmi3_import_real_variable_t* v) {
    return fmi3_xml_get_real_variable_derivative_of(v);
}

fmi3_boolean_t fmi3_import_get_real_variable_reinit(fmi3_import_real_variable_t* v) {
    return fmi3_xml_get_real_variable_reinit(v);
}

fmi3_real_t fmi3_import_get_real_variable_max(fmi3_import_real_variable_t* v) {
	return fmi3_xml_get_real_variable_max(v);
}

fmi3_real_t fmi3_import_get_real_variable_min(fmi3_import_real_variable_t* v) {
	return fmi3_xml_get_real_variable_min(v);
}

fmi3_real_t fmi3_import_get_real_variable_nominal(fmi3_import_real_variable_t* v) {
	return fmi3_xml_get_real_variable_nominal(v);
}

fmi3_import_unit_t* fmi3_import_get_real_variable_unit(fmi3_import_real_variable_t* v) {
	return fmi3_xml_get_real_variable_unit(v);
}

fmi3_import_display_unit_t* fmi3_import_get_real_variable_display_unit(fmi3_import_real_variable_t* v){
	return fmi3_xml_get_real_variable_display_unit(v);
}

const char* fmi3_import_get_string_variable_start(fmi3_import_string_variable_t* v) {
	return fmi3_xml_get_string_variable_start(v);
}

fmi3_boolean_t fmi3_import_get_boolean_variable_start(fmi3_import_bool_variable_t* v){
	return fmi3_xml_get_boolean_variable_start(v);
}

int fmi3_import_get_integer_variable_start(fmi3_import_integer_variable_t* v) {
	return fmi3_xml_get_integer_variable_start(v);
}

int fmi3_import_get_integer_variable_min(fmi3_import_integer_variable_t* v) {
	return fmi3_xml_get_integer_variable_min(v);
}

int fmi3_import_get_integer_variable_max(fmi3_import_integer_variable_t* v) {
	return fmi3_xml_get_integer_variable_max(v);
}

int fmi3_import_get_enum_variable_start(fmi3_import_enum_variable_t* v) {
	return fmi3_xml_get_enum_variable_start(v);
}

int fmi3_import_get_enum_variable_min(fmi3_import_enum_variable_t* v) {
	return fmi3_xml_get_enum_variable_min(v);
}

int fmi3_import_get_enum_variable_max(fmi3_import_enum_variable_t* v) {
	return fmi3_xml_get_enum_variable_max(v);
}

fmi3_variable_alias_kind_enu_t fmi3_import_get_variable_alias_kind(fmi3_import_variable_t* v) {
	return fmi3_xml_get_variable_alias_kind(v);
}

fmi3_import_variable_t* fmi3_import_get_variable_alias_base(fmi3_import_t* fmu,fmi3_import_variable_t* v) {
	return fmi3_xml_get_variable_alias_base(fmu->md, v);
}

/*
    Return the list of all the variables aliased to the given one (including the base one.
    The list is ordered: base variable, aliases, negated aliases.
*/
fmi3_import_variable_list_t* fmi3_import_get_variable_aliases(fmi3_import_t* fmu,fmi3_import_variable_t* v) {
	fmi3_import_variable_list_t* list = fmi3_import_alloc_variable_list(fmu, 0);
	if(fmi3_xml_get_variable_aliases(fmu->md, v, &list->variables) != jm_status_success) {
		fmi3_import_free_variable_list(list);
		return 0;
	}
	return list;
}

size_t fmi3_import_get_variable_original_order(fmi3_import_variable_t* v) {
	return fmi3_xml_get_variable_original_order((fmi3_xml_variable_t*)v);
}
