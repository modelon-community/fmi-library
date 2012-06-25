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

#include <limits.h>
#include <float.h>
#include <string.h>

#include "fmi1_import_impl.h"

fmi1_import_display_unit_t* fmi1_import_get_type_display_unit(fmi1_import_real_typedef_t* t) {
	return fmi1_xml_get_type_display_unit(t);
}

size_t fmi1_import_get_type_definition_number(fmi1_import_type_definitions_t* td) {
	return fmi1_xml_get_type_definition_number(td);
}

fmi1_import_variable_typedef_t* fmi1_import_get_typedef(fmi1_import_type_definitions_t* td, unsigned int  index) {
	return fmi1_xml_get_typedef(td, index);
}

const char* fmi1_import_get_type_name(fmi1_import_variable_typedef_t* t) {   
	return fmi1_xml_get_type_name(t);
}

/* Note that NULL pointer is returned if the attribute is not present in the XML.*/
const char* fmi1_import_get_type_description(fmi1_import_variable_typedef_t* t) {
	return fmi1_xml_get_type_description(t);
}

fmi1_base_type_enu_t fmi1_import_get_base_type(fmi1_import_variable_typedef_t* t) {
	return fmi1_xml_get_base_type(t);
}

fmi1_import_real_typedef_t* fmi1_import_get_type_as_real(fmi1_import_variable_typedef_t* t) {
	return fmi1_xml_get_type_as_real(t);
}

fmi1_import_integer_typedef_t* fmi1_import_get_type_as_int(fmi1_import_variable_typedef_t* t) {
	return fmi1_xml_get_type_as_int(t);
}

fmi1_import_enumeration_typedef_t* fmi1_import_get_type_as_enum(fmi1_import_variable_typedef_t* t) {
	return fmi1_xml_get_type_as_enum(t);
}

/* Note that 0-pointer is returned for strings and booleans, empty string quantity if not defined*/
const char* fmi1_import_get_type_quantity(fmi1_import_variable_typedef_t* t) {
	return fmi1_xml_get_type_quantity(t);
}

fmi1_real_t fmi1_import_get_real_type_min(fmi1_import_real_typedef_t* t) {
	return fmi1_xml_get_real_type_min(t);
}

fmi1_real_t fmi1_import_get_real_type_max(fmi1_import_real_typedef_t* t) {
	return fmi1_xml_get_real_type_max(t);
}

fmi1_real_t fmi1_import_get_real_type_nominal(fmi1_import_real_typedef_t* t) {
	return fmi1_xml_get_real_type_nominal(t);
}

fmi1_import_unit_t* fmi1_import_get_real_type_unit(fmi1_import_real_typedef_t* t) {    
	return fmi1_xml_get_real_type_unit(t);
}

int fmi1_import_get_real_type_is_relative_quantity(fmi1_import_real_typedef_t* t) {
	return fmi1_xml_get_real_type_is_relative_quantity(t);
}

fmi1_integer_t fmi1_import_get_integer_type_min(fmi1_import_integer_typedef_t* t) {
	return fmi1_xml_get_integer_type_min(t);
}

fmi1_integer_t fmi1_import_get_integer_type_max(fmi1_import_integer_typedef_t* t){
	return fmi1_xml_get_integer_type_max(t);
}

unsigned int fmi1_import_get_enum_type_min(fmi1_import_enumeration_typedef_t* t){
	return fmi1_xml_get_enum_type_min(t);
}

 unsigned int fmi1_import_get_enum_type_max(fmi1_import_enumeration_typedef_t* t){
	return fmi1_xml_get_enum_type_max(t);
}

unsigned int  fmi1_import_get_enum_type_size(fmi1_import_enumeration_typedef_t* t) {
	return fmi1_xml_get_enum_type_size(t);
}

const char* fmi1_import_get_enum_type_item_name(fmi1_import_enumeration_typedef_t* t, unsigned int  item) {
	return fmi1_xml_get_enum_type_item_name(t, item);
}

const char* fmi1_import_get_enum_type_item_description(fmi1_import_enumeration_typedef_t* t, unsigned int  item){
	return fmi1_xml_get_enum_type_item_description(t, item);
}
