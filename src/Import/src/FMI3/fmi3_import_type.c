
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

#include "fmi3_import_impl.h"

unsigned int fmi3_import_get_type_definition_number(fmi3_import_type_definitions_t* td) {
	return fmi3_xml_get_type_definition_number(td);
}

fmi3_import_variable_typedef_t* fmi3_import_get_typedef(fmi3_import_type_definitions_t* td, unsigned int  index) {
	return fmi3_xml_get_typedef(td, index);
}

const char* fmi3_import_get_type_name(fmi3_import_variable_typedef_t* t) {
	return fmi3_xml_get_type_name(t);
}

/* Note that NULL pointer is returned if the attribute is not present in the XML.*/
const char* fmi3_import_get_type_description(fmi3_import_variable_typedef_t* t) {
	return fmi3_xml_get_type_description(t);
}

fmi3_base_type_enu_t fmi3_import_get_base_type(fmi3_import_variable_typedef_t* t) {
	return fmi3_xml_get_base_type(t);
}

fmi3_import_float_typedef_t* fmi3_import_get_type_as_float(fmi3_import_variable_typedef_t* t) {
	return fmi3_xml_get_type_as_float(t);
}

fmi3_import_int_typedef_t* fmi3_import_get_type_as_int(fmi3_import_variable_typedef_t* t) {
	return fmi3_xml_get_type_as_int(t);
}

fmi3_import_enumeration_typedef_t* fmi3_import_get_type_as_enum(fmi3_import_variable_typedef_t* t) {
	return fmi3_xml_get_type_as_enum(t);
}

const char* fmi3_import_get_type_quantity(fmi3_import_variable_typedef_t* t) {
	return fmi3_xml_get_type_quantity(t);
}

fmi3_float64_t fmi3_import_get_float64_type_min(fmi3_import_float_typedef_t* t) {
    return fmi3_xml_get_float64_type_min(t);
}

fmi3_float64_t fmi3_import_get_float64_type_max(fmi3_import_float_typedef_t* t) {
    return fmi3_xml_get_float64_type_max(t);
}

fmi3_float64_t fmi3_import_get_float64_type_nominal(fmi3_import_float_typedef_t* t) {
    return fmi3_xml_get_float64_type_nominal(t);
}

fmi3_import_unit_t* fmi3_import_get_float64_type_unit(fmi3_import_float_typedef_t* t) {
    return fmi3_xml_get_float64_type_unit(t);
}

int fmi3_import_get_float64_type_is_relative_quantity(fmi3_import_float_typedef_t* t) {
    return fmi3_xml_get_float64_type_is_relative_quantity(t);
}

int fmi3_import_get_float64_type_is_unbounded(fmi3_import_float_typedef_t* t) {
    return fmi3_xml_get_float64_type_is_unbounded(t);
}

fmi3_import_display_unit_t* fmi3_import_get_float64_type_display_unit(fmi3_import_float_typedef_t* t) {
    return fmi3_xml_get_float64_type_display_unit(t);
}

fmi3_float32_t fmi3_import_get_float32_type_min(fmi3_import_float_typedef_t* t) {
    return fmi3_xml_get_float32_type_min(t);
}

fmi3_float32_t fmi3_import_get_float32_type_max(fmi3_import_float_typedef_t* t) {
    return fmi3_xml_get_float32_type_max(t);
}

fmi3_float32_t fmi3_import_get_float32_type_nominal(fmi3_import_float_typedef_t* t) {
    return fmi3_xml_get_float32_type_nominal(t);
}

fmi3_import_unit_t* fmi3_import_get_float32_type_unit(fmi3_import_float_typedef_t* t) {
    return fmi3_xml_get_float32_type_unit(t);
}

int fmi3_import_get_float32_type_is_relative_quantity(fmi3_import_float_typedef_t* t) {
    return fmi3_xml_get_float32_type_is_relative_quantity(t);
}

int fmi3_import_get_float32_type_is_unbounded(fmi3_import_float_typedef_t* t) {
    return fmi3_xml_get_float32_type_is_unbounded(t);
}

fmi3_import_display_unit_t* fmi3_import_get_float32_type_display_unit(fmi3_import_float_typedef_t* t) {
    return fmi3_xml_get_float32_type_display_unit(t);
}

fmi3_int64_t fmi3_import_get_int64_type_min(fmi3_import_int_typedef_t* t) {
    return fmi3_xml_get_int64_type_min(t);
}

fmi3_int64_t fmi3_import_get_int64_type_max(fmi3_import_int_typedef_t* t) {
    return fmi3_xml_get_int64_type_max(t);
}

fmi3_int32_t fmi3_import_get_int32_type_min(fmi3_import_int_typedef_t* t) {
    return fmi3_xml_get_int32_type_min(t);
}

fmi3_int32_t fmi3_import_get_int32_type_max(fmi3_import_int_typedef_t* t) {
    return fmi3_xml_get_int32_type_max(t);
}

fmi3_int16_t fmi3_import_get_int16_type_min(fmi3_import_int_typedef_t* t) {
    return fmi3_xml_get_int16_type_min(t);
}

fmi3_int16_t fmi3_import_get_int16_type_max(fmi3_import_int_typedef_t* t) {
    return fmi3_xml_get_int16_type_max(t);
}

fmi3_int8_t fmi3_import_get_int8_type_min(fmi3_import_int_typedef_t* t) {
    return fmi3_xml_get_int8_type_min(t);
}

fmi3_int8_t fmi3_import_get_int8_type_max(fmi3_import_int_typedef_t* t) {
    return fmi3_xml_get_int8_type_max(t);
}

fmi3_uint64_t fmi3_import_get_uint64_type_min(fmi3_import_int_typedef_t* t) {
    return fmi3_xml_get_uint64_type_min(t);
}

fmi3_uint64_t fmi3_import_get_uint64_type_max(fmi3_import_int_typedef_t* t) {
    return fmi3_xml_get_uint64_type_max(t);
}

fmi3_uint32_t fmi3_import_get_uint32_type_min(fmi3_import_int_typedef_t* t) {
    return fmi3_xml_get_uint32_type_min(t);
}

fmi3_uint32_t fmi3_import_get_uint32_type_max(fmi3_import_int_typedef_t* t) {
    return fmi3_xml_get_uint32_type_max(t);
}

fmi3_uint16_t fmi3_import_get_uint16_type_min(fmi3_import_int_typedef_t* t) {
    return fmi3_xml_get_uint16_type_min(t);
}

fmi3_uint16_t fmi3_import_get_uint16_type_max(fmi3_import_int_typedef_t* t) {
    return fmi3_xml_get_uint16_type_max(t);
}

fmi3_uint8_t fmi3_import_get_uint8_type_min(fmi3_import_int_typedef_t* t) {
    return fmi3_xml_get_uint8_type_min(t);
}

fmi3_uint8_t fmi3_import_get_uint8_type_max(fmi3_import_int_typedef_t* t) {
    return fmi3_xml_get_uint8_type_max(t);
}

unsigned int fmi3_import_get_enum_type_min(fmi3_import_enumeration_typedef_t* t){
	return fmi3_xml_get_enum_type_min(t);
}

 unsigned int fmi3_import_get_enum_type_max(fmi3_import_enumeration_typedef_t* t){
	return fmi3_xml_get_enum_type_max(t);
}

unsigned int  fmi3_import_get_enum_type_size(fmi3_import_enumeration_typedef_t* t) {
	return fmi3_xml_get_enum_type_size(t);
}

const char* fmi3_import_get_enum_type_item_name(fmi3_import_enumeration_typedef_t* t, unsigned int  item) {
	return fmi3_xml_get_enum_type_item_name(t, item);
}

const char* fmi3_import_get_enum_type_item_description(fmi3_import_enumeration_typedef_t* t, unsigned int  item){
	return fmi3_xml_get_enum_type_item_description(t, item);
}

int fmi3_import_get_enum_type_item_value(fmi3_import_enumeration_typedef_t* t, unsigned int  item) {
	return fmi3_xml_get_enum_type_item_value(t, item);
}

const char* fmi3_import_get_enum_type_value_name(fmi3_import_enumeration_typedef_t* t, int value) {
	return fmi3_xml_get_enum_type_value_name(t, value);
}
