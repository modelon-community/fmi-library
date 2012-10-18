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



/** \file fmi1_xml_type.h
*  \brief Public interface to the FMI XML C-library: variable types handling.
*/

#ifndef FMI1_XML_TYPE_H_
#define FMI1_XML_TYPE_H_

#include "FMI1/fmi1_xml_model_description.h"

#ifdef __cplusplus
extern "C" {
#endif

	/**
	\addtogroup fmi1_xml
	@{
	\addtogroup fmi1_xml_types Support for processing variable types
	@}
	\addtogroup fmi1_xml_types Support for processing variable types
	@{
	*/

fmi1_xml_display_unit_t* fmi1_xml_get_type_display_unit(fmi1_xml_real_typedef_t*);

fmi1_xml_type_definitions_t* fmi1_xml_get_type_definitions(fmi1_xml_model_description_t* md);

/* Convert base type constant to string */

size_t fmi1_xml_get_type_definition_number(fmi1_xml_type_definitions_t* td);

fmi1_xml_variable_typedef_t* fmi1_xml_get_typedef(fmi1_xml_type_definitions_t* td, unsigned int  index);

const char* fmi1_xml_get_type_name(fmi1_xml_variable_typedef_t*);

/* Note that NULL pointer is returned if the attribute is not present in the XML.*/
const char* fmi1_xml_get_type_description(fmi1_xml_variable_typedef_t*);

fmi1_base_type_enu_t fmi1_xml_get_base_type(fmi1_xml_variable_typedef_t*);

/* Boolean and String has no extra attributes -> not needed*/

fmi1_xml_real_typedef_t* fmi1_xml_get_type_as_real(fmi1_xml_variable_typedef_t*);
fmi1_xml_integer_typedef_t* fmi1_xml_get_type_as_int(fmi1_xml_variable_typedef_t*);
fmi1_xml_enumeration_typedef_t* fmi1_xml_get_type_as_enum(fmi1_xml_variable_typedef_t*);

/* Note that NULL-pointer is always returned for strings and booleans */
const char* fmi1_xml_get_type_quantity(fmi1_xml_variable_typedef_t*);

double fmi1_xml_get_real_type_min(fmi1_xml_real_typedef_t*);
double fmi1_xml_get_real_type_max(fmi1_xml_real_typedef_t*);
double fmi1_xml_get_real_type_nominal(fmi1_xml_real_typedef_t*);
fmi1_xml_unit_t* fmi1_xml_get_real_type_unit(fmi1_xml_real_typedef_t*);
int fmi1_xml_get_real_type_is_relative_quantity(fmi1_xml_real_typedef_t*);

fmi1_integer_t fmi1_xml_get_integer_type_min(fmi1_xml_integer_typedef_t*);
fmi1_integer_t fmi1_xml_get_integer_type_max(fmi1_xml_integer_typedef_t*);

int fmi1_xml_get_enum_type_min(fmi1_xml_enumeration_typedef_t*);
int fmi1_xml_get_enum_type_max(fmi1_xml_enumeration_typedef_t*);
unsigned int  fmi1_xml_get_enum_type_size(fmi1_xml_enumeration_typedef_t*);
const char* fmi1_xml_get_enum_type_item_name(fmi1_xml_enumeration_typedef_t*, unsigned int  item);
const char* fmi1_xml_get_enum_type_item_description(fmi1_xml_enumeration_typedef_t*, unsigned int  item);

/*
*  @}
*/
#ifdef __cplusplus
}
#endif
#endif

