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



/** \file fmi2_import_variable.c
*  \brief Methods to handle fmi2_import_variable_t.
*/

#include <FMI2/fmi2_import_variable.h>
#include "fmi2_import_impl.h"
#include "fmi2_import_variable_list_impl.h"

fmi2_import_variable_t* fmi2_import_get_variable_by_name(fmi2_import_t* fmu, const char* name) {
	return fmi2_xml_get_variable_by_name(fmu->md, name);
}

fmi2_import_variable_t* fmi2_import_get_variable_by_vr(fmi2_import_t* fmu, fmi2_base_type_enu_t baseType, fmi2_value_reference_t vr) {
	return fmi2_xml_get_variable_by_vr(fmu->md, baseType, vr);
}

const char* fmi2_import_get_variable_name(fmi2_import_variable_t* v) {
	return fmi2_xml_get_variable_name(v); 
}

const char* fmi2_import_get_variable_description(fmi2_import_variable_t* v) {
	return fmi2_xml_get_variable_description(v);
}

fmi2_value_reference_t fmi2_import_get_variable_vr(fmi2_import_variable_t* v) {
	return fmi2_xml_get_variable_vr(v);
}

/*
    For scalar variable gives the type definition is present
*/
fmi2_import_variable_typedef_t* fmi2_import_get_variable_declared_type(fmi2_import_variable_t* v) {
	return fmi2_xml_get_variable_declared_type(v);
}

fmi2_base_type_enu_t fmi2_import_get_variable_base_type(fmi2_import_variable_t* v) {
	return fmi2_xml_get_variable_base_type(v);
}

int   fmi2_import_get_variable_has_start(fmi2_import_variable_t* v) {
	return fmi2_xml_get_variable_has_start(v);
}

fmi2_variability_enu_t fmi2_import_get_variability(fmi2_import_variable_t* v) {
	return fmi2_xml_get_variability(v);
}

fmi2_causality_enu_t fmi2_import_get_causality(fmi2_import_variable_t* v) {
	return fmi2_xml_get_causality(v);
}

fmi2_initial_enu_t fmi2_import_get_initial(fmi2_import_variable_t* v) {
	return fmi2_xml_get_initial(v);
}

fmi2_import_variable_t* fmi2_import_get_previous(fmi2_import_variable_t* v) {
	return fmi2_xml_get_previous(v);    
}

fmi2_boolean_t fmi2_import_get_canHandleMultipleSetPerTimeInstant(fmi2_import_variable_t* v) {
	return fmi2_xml_get_canHandleMultipleSetPerTimeInstant(v);
}


fmi2_import_real_variable_t* fmi2_import_get_variable_as_real(fmi2_import_variable_t* v) {
	return fmi2_xml_get_variable_as_real(v);
}

fmi2_import_integer_variable_t* fmi2_import_get_variable_as_integer(fmi2_import_variable_t* v) {
	return fmi2_xml_get_variable_as_integer(v);
}

fmi2_import_enum_variable_t* fmi2_import_get_variable_as_enum(fmi2_import_variable_t* v) {
	return fmi2_xml_get_variable_as_enum(v);
}

fmi2_import_string_variable_t* fmi2_import_get_variable_as_string(fmi2_import_variable_t* v) {
	return fmi2_xml_get_variable_as_string(v);
}

fmi2_import_bool_variable_t* fmi2_import_get_variable_as_boolean(fmi2_import_variable_t* v) {
	return fmi2_xml_get_variable_as_boolean(v);
}

fmi2_real_t fmi2_import_get_real_variable_start(fmi2_import_real_variable_t* v) {
	return fmi2_xml_get_real_variable_start(v);
}

fmi2_import_real_variable_t* fmi2_import_get_real_variable_derivative_of(fmi2_import_real_variable_t* v) {
    return fmi2_xml_get_real_variable_derivative_of(v);
}

fmi2_boolean_t fmi2_import_get_real_variable_reinit(fmi2_import_real_variable_t* v) {
    return fmi2_xml_get_real_variable_reinit(v);
}

fmi2_real_t fmi2_import_get_real_variable_max(fmi2_import_real_variable_t* v) {
	return fmi2_xml_get_real_variable_max(v);
}

fmi2_real_t fmi2_import_get_real_variable_min(fmi2_import_real_variable_t* v) {
	return fmi2_xml_get_real_variable_min(v);
}

fmi2_real_t fmi2_import_get_real_variable_nominal(fmi2_import_real_variable_t* v) {
	return fmi2_xml_get_real_variable_nominal(v);
}

fmi2_import_unit_t* fmi2_import_get_real_variable_unit(fmi2_import_real_variable_t* v) {
	return fmi2_xml_get_real_variable_unit(v);
}

fmi2_import_display_unit_t* fmi2_import_get_real_variable_display_unit(fmi2_import_real_variable_t* v){
	return fmi2_xml_get_real_variable_display_unit(v);
}

const char* fmi2_import_get_string_variable_start(fmi2_import_string_variable_t* v) {
	return fmi2_xml_get_string_variable_start(v);
}

fmi2_boolean_t fmi2_import_get_boolean_variable_start(fmi2_import_bool_variable_t* v){
	return fmi2_xml_get_boolean_variable_start(v);
}

int fmi2_import_get_integer_variable_start(fmi2_import_integer_variable_t* v) {
	return fmi2_xml_get_integer_variable_start(v);
}

int fmi2_import_get_integer_variable_min(fmi2_import_integer_variable_t* v) {
	return fmi2_xml_get_integer_variable_min(v);
}

int fmi2_import_get_integer_variable_max(fmi2_import_integer_variable_t* v) {
	return fmi2_xml_get_integer_variable_max(v);
}

int fmi2_import_get_enum_variable_start(fmi2_import_enum_variable_t* v) {
	return fmi2_xml_get_enum_variable_start(v);
}

int fmi2_import_get_enum_variable_min(fmi2_import_enum_variable_t* v) {
	return fmi2_xml_get_enum_variable_min(v);
}

int fmi2_import_get_enum_variable_max(fmi2_import_enum_variable_t* v) {
	return fmi2_xml_get_enum_variable_max(v);
}

fmi2_variable_alias_kind_enu_t fmi2_import_get_variable_alias_kind(fmi2_import_variable_t* v) {
	return fmi2_xml_get_variable_alias_kind(v);
}

fmi2_import_variable_t* fmi2_import_get_variable_alias_base(fmi2_import_t* fmu,fmi2_import_variable_t* v) {
	return fmi2_xml_get_variable_alias_base(fmu->md, v);
}

/*
    Return the list of all the variables aliased to the given one (including the base one.
    The list is ordered: base variable, aliases, negated aliases.
*/
fmi2_import_variable_list_t* fmi2_import_get_variable_aliases(fmi2_import_t* fmu,fmi2_import_variable_t* v) {
	fmi2_import_variable_list_t* list = fmi2_import_alloc_variable_list(fmu, 0);
	if(fmi2_xml_get_variable_aliases(fmu->md, v, &list->variables) != jm_status_success) {
		fmi2_import_free_variable_list(list);
		return 0;
	}
	return list;
}

size_t fmi2_import_get_variable_original_order(fmi2_import_variable_t* v) {
	return fmi2_xml_get_variable_original_order((fmi2_xml_variable_t*)v);
}
