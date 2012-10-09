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

#include "fmi2_import_impl.h"

fmi2_import_display_unit_t* fmi2_import_get_type_display_unit(fmi2_import_real_typedef_t* t) {
	return fmi2_xml_get_type_display_unit(t);
}

unsigned int fmi2_import_get_type_definition_number(fmi2_import_type_definitions_t* td) {
	return fmi2_xml_get_type_definition_number(td);
}

fmi2_import_variable_typedef_t* fmi2_import_get_typedef(fmi2_import_type_definitions_t* td, unsigned int  index) {
	return fmi2_xml_get_typedef(td, index);
}

const char* fmi2_import_get_type_name(fmi2_import_variable_typedef_t* t) {   
	return fmi2_xml_get_type_name(t);
}

/* Note that NULL pointer is returned if the attribute is not present in the XML.*/
const char* fmi2_import_get_type_description(fmi2_import_variable_typedef_t* t) {
	return fmi2_xml_get_type_description(t);
}

fmi2_base_type_enu_t fmi2_import_get_base_type(fmi2_import_variable_typedef_t* t) {
	return fmi2_xml_get_base_type(t);
}

fmi2_import_real_typedef_t* fmi2_import_get_type_as_real(fmi2_import_variable_typedef_t* t) {
	return fmi2_xml_get_type_as_real(t);
}

fmi2_import_integer_typedef_t* fmi2_import_get_type_as_int(fmi2_import_variable_typedef_t* t) {
	return fmi2_xml_get_type_as_int(t);
}

fmi2_import_enumeration_typedef_t* fmi2_import_get_type_as_enum(fmi2_import_variable_typedef_t* t) {
	return fmi2_xml_get_type_as_enum(t);
}

/* Note that 0-pointer is returned for strings and booleans, empty string quantity if not defined*/
const char* fmi2_import_get_type_quantity(fmi2_import_variable_typedef_t* t) {
	return fmi2_xml_get_type_quantity(t);
}

fmi2_real_t fmi2_import_get_real_type_min(fmi2_import_real_typedef_t* t) {
	return fmi2_xml_get_real_type_min(t);
}

fmi2_real_t fmi2_import_get_real_type_max(fmi2_import_real_typedef_t* t) {
	return fmi2_xml_get_real_type_max(t);
}

fmi2_real_t fmi2_import_get_real_type_nominal(fmi2_import_real_typedef_t* t) {
	return fmi2_xml_get_real_type_nominal(t);
}

fmi2_import_unit_t* fmi2_import_get_real_type_unit(fmi2_import_real_typedef_t* t) {    
	return fmi2_xml_get_real_type_unit(t);
}

int fmi2_import_get_real_type_is_relative_quantity(fmi2_import_real_typedef_t* t) {
	return fmi2_xml_get_real_type_is_relative_quantity(t);
}

int fmi2_import_get_real_type_is_unbounded(fmi2_import_real_typedef_t* t) {
	return fmi2_xml_get_real_type_is_unbounded(t);
}

fmi2_integer_t fmi2_import_get_integer_type_min(fmi2_import_integer_typedef_t* t) {
	return fmi2_xml_get_integer_type_min(t);
}

fmi2_integer_t fmi2_import_get_integer_type_max(fmi2_import_integer_typedef_t* t){
	return fmi2_xml_get_integer_type_max(t);
}

unsigned int fmi2_import_get_enum_type_min(fmi2_import_enumeration_typedef_t* t){
	return fmi2_xml_get_enum_type_min(t);
}

 unsigned int fmi2_import_get_enum_type_max(fmi2_import_enumeration_typedef_t* t){
	return fmi2_xml_get_enum_type_max(t);
}

unsigned int  fmi2_import_get_enum_type_size(fmi2_import_enumeration_typedef_t* t) {
	return fmi2_xml_get_enum_type_size(t);
}

const char* fmi2_import_get_enum_type_item_name(fmi2_import_enumeration_typedef_t* t, unsigned int  item) {
	return fmi2_xml_get_enum_type_item_name(t, item);
}

const char* fmi2_import_get_enum_type_item_description(fmi2_import_enumeration_typedef_t* t, unsigned int  item){
	return fmi2_xml_get_enum_type_item_description(t, item);
}

int fmi2_import_get_enum_type_item_value(fmi2_import_enumeration_typedef_t* t, unsigned int  item) {
	return fmi2_xml_get_enum_type_item_value(t, item);
}

const char* fmi2_import_get_enum_type_value_name(fmi2_import_enumeration_typedef_t* t, int value) {
	return fmi2_xml_get_enum_type_value_name(t, value);
}

