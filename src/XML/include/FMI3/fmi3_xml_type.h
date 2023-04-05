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



/** \file fmi3_xml_type.h
*  \brief Public interface to the FMI XML C-library: variable types handling.
*/

#ifndef FMI3_XML_TYPE_H_
#define FMI3_XML_TYPE_H_

#include "FMI3/fmi3_xml_model_description.h"

#ifdef __cplusplus
extern "C" {
#endif

    /**
    \addtogroup fmi3_xml
    @{
    \addtogroup fmi3_xml_types Support for processing variable types
    @}
    \addtogroup fmi3_xml_types Support for processing variable types
    @{
    */

fmi3_xml_type_definitions_t* fmi3_xml_get_type_definitions(fmi3_xml_model_description_t* md);

/* Convert base type constant to string */

unsigned int fmi3_xml_get_type_definition_number(fmi3_xml_type_definitions_t* td);

fmi3_xml_variable_typedef_t* fmi3_xml_get_typedef(fmi3_xml_type_definitions_t* td, unsigned int  index);

const char* fmi3_xml_get_type_name(fmi3_xml_variable_typedef_t*);

/* Note that NULL pointer is returned if the attribute is not present in the XML.*/
const char* fmi3_xml_get_type_description(fmi3_xml_variable_typedef_t*);

fmi3_base_type_enu_t fmi3_xml_get_base_type(fmi3_xml_variable_typedef_t*);

/* Boolean and String has no extra attributes -> not needed*/

fmi3_xml_float_typedef_t* fmi3_xml_get_type_as_float(fmi3_xml_variable_typedef_t* t);
fmi3_xml_int_typedef_t* fmi3_xml_get_type_as_int(fmi3_xml_variable_typedef_t*);
fmi3_xml_enumeration_typedef_t* fmi3_xml_get_type_as_enum(fmi3_xml_variable_typedef_t*);

/* Note that NULL-pointer is always returned for strings and booleans */
const char* fmi3_xml_get_type_quantity(fmi3_xml_variable_typedef_t*);

int fmi3_xml_get_float64_type_is_relative_quantity(fmi3_xml_float_typedef_t*);
int fmi3_xml_get_float64_type_is_unbounded(fmi3_xml_float_typedef_t*);
int fmi3_xml_get_float32_type_is_relative_quantity(fmi3_xml_float_typedef_t*);
int fmi3_xml_get_float32_type_is_unbounded(fmi3_xml_float_typedef_t*);
fmi3_float64_t fmi3_xml_get_float64_type_min(fmi3_xml_float_typedef_t*);
fmi3_float64_t fmi3_xml_get_float64_type_max(fmi3_xml_float_typedef_t*);
fmi3_float64_t fmi3_xml_get_float64_type_nominal(fmi3_xml_float_typedef_t*);
fmi3_xml_unit_t* fmi3_xml_get_float64_type_unit(fmi3_xml_float_typedef_t*);
fmi3_xml_display_unit_t* fmi3_xml_get_float64_type_display_unit(fmi3_xml_float_typedef_t*);
fmi3_float32_t fmi3_xml_get_float32_type_min(fmi3_xml_float_typedef_t*);
fmi3_float32_t fmi3_xml_get_float32_type_max(fmi3_xml_float_typedef_t*);
fmi3_float32_t fmi3_xml_get_float32_type_nominal(fmi3_xml_float_typedef_t*);
fmi3_xml_unit_t* fmi3_xml_get_float32_type_unit(fmi3_xml_float_typedef_t*);
fmi3_xml_display_unit_t* fmi3_xml_get_float32_type_display_unit(fmi3_xml_float_typedef_t*);
fmi3_int64_t fmi3_xml_get_int64_type_min(fmi3_xml_int_typedef_t*);
fmi3_int64_t fmi3_xml_get_int64_type_max(fmi3_xml_int_typedef_t*);
fmi3_int32_t fmi3_xml_get_int32_type_min(fmi3_xml_int_typedef_t*);
fmi3_int32_t fmi3_xml_get_int32_type_max(fmi3_xml_int_typedef_t*);
fmi3_int16_t fmi3_xml_get_int16_type_min(fmi3_xml_int_typedef_t*);
fmi3_int16_t fmi3_xml_get_int16_type_max(fmi3_xml_int_typedef_t*);
fmi3_int8_t fmi3_xml_get_int8_type_min(fmi3_xml_int_typedef_t*);
fmi3_int8_t fmi3_xml_get_int8_type_max(fmi3_xml_int_typedef_t*);
fmi3_uint64_t fmi3_xml_get_uint64_type_min(fmi3_xml_int_typedef_t*);
fmi3_uint64_t fmi3_xml_get_uint64_type_max(fmi3_xml_int_typedef_t*);
fmi3_uint32_t fmi3_xml_get_uint32_type_min(fmi3_xml_int_typedef_t*);
fmi3_uint32_t fmi3_xml_get_uint32_type_max(fmi3_xml_int_typedef_t*);
fmi3_uint16_t fmi3_xml_get_uint16_type_min(fmi3_xml_int_typedef_t*);
fmi3_uint16_t fmi3_xml_get_uint16_type_max(fmi3_xml_int_typedef_t*);
fmi3_uint8_t fmi3_xml_get_uint8_type_min(fmi3_xml_int_typedef_t*);
fmi3_uint8_t fmi3_xml_get_uint8_type_max(fmi3_xml_int_typedef_t*);

int fmi3_xml_get_enum_type_min(fmi3_xml_enumeration_typedef_t*);
int fmi3_xml_get_enum_type_max(fmi3_xml_enumeration_typedef_t*);
unsigned int  fmi3_xml_get_enum_type_size(fmi3_xml_enumeration_typedef_t*);
const char* fmi3_xml_get_enum_type_item_name(fmi3_xml_enumeration_typedef_t*, unsigned int  item);
int fmi3_xml_get_enum_type_item_value(fmi3_xml_enumeration_typedef_t*, unsigned int  item);
const char* fmi3_xml_get_enum_type_item_description(fmi3_xml_enumeration_typedef_t*, unsigned int  item);
const char* fmi3_xml_get_enum_type_value_name(fmi3_xml_enumeration_typedef_t* t, int value);

/*
*  @}
*/
#ifdef __cplusplus
}
#endif
#endif
