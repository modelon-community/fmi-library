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



/** \file fmi2_xml_type.h
*  \brief Public interface to the FMI XML C-library: variable types handling.
*/

#ifndef FMI2_XML_TYPE_H_
#define FMI2_XML_TYPE_H_

#include "FMI2/fmi2_xml_model_description.h"

#ifdef __cplusplus
extern "C" {
#endif

	/**
	\addtogroup fmi2_xml
	@{
	\addtogroup fmi2_xml_types Support for processing variable types
	@}
	\addtogroup fmi2_xml_types Support for processing variable types
	@{
	*/

fmi2_xml_display_unit_t* fmi2_xml_get_type_display_unit(fmi2_xml_real_typedef_t*);

fmi2_xml_type_definitions_t* fmi2_xml_get_type_definitions(fmi2_xml_model_description_t* md);

/* Convert base type constant to string */

unsigned int fmi2_xml_get_type_definition_number(fmi2_xml_type_definitions_t* td);

fmi2_xml_variable_typedef_t* fmi2_xml_get_typedef(fmi2_xml_type_definitions_t* td, unsigned int  index);

const char* fmi2_xml_get_type_name(fmi2_xml_variable_typedef_t*);

/* Note that NULL pointer is returned if the attribute is not present in the XML.*/
const char* fmi2_xml_get_type_description(fmi2_xml_variable_typedef_t*);

fmi2_base_type_enu_t fmi2_xml_get_base_type(fmi2_xml_variable_typedef_t*);

/* Boolean and String has no extra attributes -> not needed*/

fmi2_xml_real_typedef_t* fmi2_xml_get_type_as_real(fmi2_xml_variable_typedef_t*);
fmi2_xml_integer_typedef_t* fmi2_xml_get_type_as_int(fmi2_xml_variable_typedef_t*);
fmi2_xml_enumeration_typedef_t* fmi2_xml_get_type_as_enum(fmi2_xml_variable_typedef_t*);

/* Note that NULL-pointer is always returned for strings and booleans */
const char* fmi2_xml_get_type_quantity(fmi2_xml_variable_typedef_t*);

double fmi2_xml_get_real_type_min(fmi2_xml_real_typedef_t*);
double fmi2_xml_get_real_type_max(fmi2_xml_real_typedef_t*);
double fmi2_xml_get_real_type_nominal(fmi2_xml_real_typedef_t*);
fmi2_xml_unit_t* fmi2_xml_get_real_type_unit(fmi2_xml_real_typedef_t*);
int fmi2_xml_get_real_type_is_relative_quantity(fmi2_xml_real_typedef_t*);
int fmi2_xml_get_real_type_is_unbounded(fmi2_xml_real_typedef_t*);

fmi2_integer_t fmi2_xml_get_integer_type_min(fmi2_xml_integer_typedef_t*);
fmi2_integer_t fmi2_xml_get_integer_type_max(fmi2_xml_integer_typedef_t*);

int fmi2_xml_get_enum_type_min(fmi2_xml_enumeration_typedef_t*);
int fmi2_xml_get_enum_type_max(fmi2_xml_enumeration_typedef_t*);
unsigned int  fmi2_xml_get_enum_type_size(fmi2_xml_enumeration_typedef_t*);
const char* fmi2_xml_get_enum_type_item_name(fmi2_xml_enumeration_typedef_t*, unsigned int  item);
int fmi2_xml_get_enum_type_item_value(fmi2_xml_enumeration_typedef_t*, unsigned int  item);
const char* fmi2_xml_get_enum_type_item_description(fmi2_xml_enumeration_typedef_t*, unsigned int  item);
const char* fmi2_xml_get_enum_type_value_name(fmi2_xml_enumeration_typedef_t* t, int value);

/*
*  @}
*/
#ifdef __cplusplus
}
#endif
#endif

