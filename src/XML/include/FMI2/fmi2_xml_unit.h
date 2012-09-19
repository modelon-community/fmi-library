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

/** \file fmi2_xml_unit.h
*  \brief Public interface to the FMI XML C-library. Handling of variable units.
*/

#ifndef FMI2_XML_UNIT_H_
#define FMI2_XML_UNIT_H_

#include "fmi2_xml_model_description.h"

#ifdef __cplusplus
extern "C" {
#endif
		
	/**
	\addtogroup fmi2_xml
	@{
	\addtogroup fmi2_xml_units Functions for handling unit definitions.
	@}
	\addtogroup fmi2_xml_units Functions for handling unit definitions.
	@{
	*/

/** \brief Get a list of all the unit definitions */
fmi2_xml_unit_definitions_t* fmi2_xml_get_unit_definitions(fmi2_xml_model_description_t* md);
unsigned int  fmi2_xml_get_unit_definitions_number(fmi2_xml_unit_definitions_t*);
fmi2_xml_unit_t* fmi2_xml_get_unit(fmi2_xml_unit_definitions_t*, unsigned int  index);

const char* fmi2_xml_get_unit_name(fmi2_xml_unit_t*);
unsigned int fmi2_xml_get_unit_display_unit_number(fmi2_xml_unit_t*);
fmi2_xml_display_unit_t* fmi2_xml_get_unit_display_unit(fmi2_xml_unit_t*, size_t index);

/**
	\brief Get fmi2_SI_base_units_Num SI base units exponents associated with the unit.
*/
const int* fmi2_xml_get_SI_unit_exponents(fmi2_xml_unit_t*);

/**
	\brief Get factor to the corresponding SI base units.
*/
double fmi2_xml_get_SI_unit_factor(fmi2_xml_unit_t*);

/**
	\brief Get offset to the corresponding SI base units.
*/
double fmi2_xml_get_SI_unit_offset(fmi2_xml_unit_t*);

/**
	\brief Convert a value with respect to the unit to the
	value with respect to the SI base unit.
*/
double fmi2_xml_convert_to_SI_base_unit(double, fmi2_xml_unit_t*);

/**
	\brief Convert a value with respect to the SI base unit to the
	value with respect to the unit.
*/
double fmi2_xml_convert_from_SI_base_unit(double, fmi2_xml_unit_t*);

fmi2_xml_display_unit_t* fmi2_xml_get_type_display_unit(fmi2_xml_real_typedef_t*);
fmi2_xml_unit_t* fmi2_xml_get_base_unit(fmi2_xml_display_unit_t*);
const char* fmi2_xml_get_display_unit_name(fmi2_xml_display_unit_t*);
double fmi2_xml_get_display_unit_factor(fmi2_xml_display_unit_t*);
double fmi2_xml_get_display_unit_offset(fmi2_xml_display_unit_t*);

double fmi2_xml_convert_to_display_unit(double, fmi2_xml_display_unit_t*, int isRelativeQuantity);
double fmi2_xml_convert_from_display_unit(double, fmi2_xml_display_unit_t*, int isRelativeQuantity);
/**
@}
*/
#ifdef __cplusplus
}
#endif
#endif
