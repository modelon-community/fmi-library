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



/** \file fmi1_import_variable.c
*  \brief Methods to handle fmi1_import_variable_t.
*/

#include <FMI1/fmi1_import_variable.h>
#include "fmi1_import_impl.h"
#include "fmi1_import_variable_list_impl.h"

fmi1_import_variable_t* fmi1_import_get_variable_by_name(fmi1_import_t* fmu, const char* name) {
	return fmi1_xml_get_variable_by_name(fmu->md, name);
}

fmi1_import_variable_t* fmi1_import_get_variable_by_vr(fmi1_import_t* fmu, fmi1_base_type_enu_t baseType, fmi1_value_reference_t vr) {
	return fmi1_xml_get_variable_by_vr(fmu->md, baseType, vr);
}

const char* fmi1_import_get_variable_name(fmi1_import_variable_t* v) {
	return fmi1_xml_get_variable_name(v); 
}

const char* fmi1_import_get_variable_description(fmi1_import_variable_t* v) {
	return fmi1_xml_get_variable_description(v);
}

fmi1_value_reference_t fmi1_import_get_variable_vr(fmi1_import_variable_t* v) {
	return fmi1_xml_get_variable_vr(v);
}

/*
    For scalar variable gives the type definition is present
*/
fmi1_import_variable_typedef_t* fmi1_import_get_variable_declared_type(fmi1_import_variable_t* v) {
	return fmi1_xml_get_variable_declared_type(v);
}

fmi1_base_type_enu_t fmi1_import_get_variable_base_type(fmi1_import_variable_t* v) {
	return fmi1_xml_get_variable_base_type(v);
}

int   fmi1_import_get_variable_has_start(fmi1_import_variable_t* v) {
	return fmi1_xml_get_variable_has_start(v);
}

int   fmi1_import_get_variable_is_fixed(fmi1_import_variable_t* v) {
	return fmi1_xml_get_variable_is_fixed(v);
}

fmi1_variability_enu_t fmi1_import_get_variability(fmi1_import_variable_t* v) {
	return fmi1_xml_get_variability(v);
}

fmi1_causality_enu_t fmi1_import_get_causality(fmi1_import_variable_t* v) {
	return fmi1_xml_get_causality(v);
}

/* DirectDependency is returned for variables with causality Output. Null pointer for others. */
fmi1_import_variable_list_t* fmi1_import_get_direct_dependency(fmi1_import_t* fmu, fmi1_import_variable_t* v) {
	fmi1_import_variable_list_t* list = fmi1_import_alloc_variable_list(fmu, fmi1_xml_get_direct_dependency_size(fmu->md,v));
	if(fmi1_xml_get_direct_dependency(fmu->md,v,&list->variables) != jm_status_success) {
		fmi1_import_free_variable_list(list);
		return 0;
	}
	return list;
}

fmi1_import_real_variable_t* fmi1_import_get_variable_as_real(fmi1_import_variable_t* v) {
	return fmi1_xml_get_variable_as_real(v);
}

fmi1_import_integer_variable_t* fmi1_import_get_variable_as_integer(fmi1_import_variable_t* v) {
	return fmi1_xml_get_variable_as_integer(v);
}

fmi1_import_enum_variable_t* fmi1_import_get_variable_as_enum(fmi1_import_variable_t* v) {
	return fmi1_xml_get_variable_as_enum(v);
}

fmi1_import_string_variable_t* fmi1_import_get_variable_as_string(fmi1_import_variable_t* v) {
	return fmi1_xml_get_variable_as_string(v);
}

fmi1_import_bool_variable_t* fmi1_import_get_variable_as_boolean(fmi1_import_variable_t* v) {
	return fmi1_xml_get_variable_as_boolean(v);
}

fmi1_real_t fmi1_import_get_real_variable_start(fmi1_import_real_variable_t* v) {
	return fmi1_xml_get_real_variable_start(v);
}

fmi1_real_t fmi1_import_get_real_variable_max(fmi1_import_real_variable_t* v) {
	return fmi1_xml_get_real_variable_max(v);
}

fmi1_real_t fmi1_import_get_real_variable_min(fmi1_import_real_variable_t* v) {
	return fmi1_xml_get_real_variable_min(v);
}

fmi1_real_t fmi1_import_get_real_variable_nominal(fmi1_import_real_variable_t* v) {
	return fmi1_xml_get_real_variable_nominal(v);
}

fmi1_import_unit_t* fmi1_import_get_real_variable_unit(fmi1_import_real_variable_t* v) {
	return fmi1_xml_get_real_variable_unit(v);
}

fmi1_import_display_unit_t* fmi1_import_get_real_variable_display_unit(fmi1_import_real_variable_t* v){
	return fmi1_xml_get_real_variable_display_unit(v);
}

const char* fmi1_import_get_string_variable_start(fmi1_import_string_variable_t* v) {
	return fmi1_xml_get_string_variable_start(v);
}

fmi1_boolean_t fmi1_import_get_boolean_variable_start(fmi1_import_bool_variable_t* v){
	return fmi1_xml_get_boolean_variable_start(v);
}

int fmi1_import_get_integer_variable_start(fmi1_import_integer_variable_t* v) {
	return fmi1_xml_get_integer_variable_start(v);
}

int fmi1_import_get_integer_variable_min(fmi1_import_integer_variable_t* v) {
	return fmi1_xml_get_integer_variable_min(v);
}

int fmi1_import_get_integer_variable_max(fmi1_import_integer_variable_t* v) {
	return fmi1_xml_get_integer_variable_max(v);
}

int fmi1_import_get_enum_variable_start(fmi1_import_enum_variable_t* v) {
	return fmi1_xml_get_enum_variable_start(v);
}

int fmi1_import_get_enum_variable_min(fmi1_import_enum_variable_t* v) {
	return fmi1_xml_get_enum_variable_min(v);
}

int fmi1_import_get_enum_variable_max(fmi1_import_enum_variable_t* v) {
	return fmi1_xml_get_enum_variable_max(v);
}

fmi1_variable_alias_kind_enu_t fmi1_import_get_variable_alias_kind(fmi1_import_variable_t* v) {
	return fmi1_xml_get_variable_alias_kind(v);
}

fmi1_import_variable_t* fmi1_import_get_variable_alias_base(fmi1_import_t* fmu,fmi1_import_variable_t* v) {
	return fmi1_xml_get_variable_alias_base(fmu->md, v);
}

/*
    Return the list of all the variables aliased to the given one (including the base one.
    The list is ordered: base variable, aliases, negated aliases.
*/
fmi1_import_variable_list_t* fmi1_import_get_variable_aliases(fmi1_import_t* fmu,fmi1_import_variable_t* v) {
	fmi1_import_variable_list_t* list = fmi1_import_alloc_variable_list(fmu, 0);
	if(fmi1_xml_get_variable_aliases(fmu->md, v, &list->variables) != jm_status_success) {
		fmi1_import_free_variable_list(list);
		return 0;
	}
	return list;
}

size_t fmi1_import_get_variable_original_order(fmi1_import_variable_t* v) {
	return fmi1_xml_get_variable_original_order((fmi1_xml_variable_t*)v);
}
