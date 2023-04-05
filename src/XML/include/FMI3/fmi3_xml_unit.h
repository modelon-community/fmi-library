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

/** \file fmi3_xml_unit.h
*  \brief Public interface to the FMI XML C-library. Handling of variable units.
*/

#ifndef FMI3_XML_UNIT_H_
#define FMI3_XML_UNIT_H_

#include "fmi3_xml_model_description.h"

#ifdef __cplusplus
extern "C" {
#endif

    /**
    \addtogroup fmi3_xml
    @{
    \addtogroup fmi3_xml_units Functions for handling unit definitions.
    @}
    \addtogroup fmi3_xml_units Functions for handling unit definitions.
    @{
    */

/** \brief Get a list of all the unit definitions */
fmi3_xml_unit_definitions_t* fmi3_xml_get_unit_definitions(fmi3_xml_model_description_t* md);
unsigned int  fmi3_xml_get_unit_definitions_number(fmi3_xml_unit_definitions_t*);
fmi3_xml_unit_t* fmi3_xml_get_unit(fmi3_xml_unit_definitions_t*, unsigned int  index);

const char* fmi3_xml_get_unit_name(fmi3_xml_unit_t*);
unsigned int fmi3_xml_get_unit_display_unit_number(fmi3_xml_unit_t*);
fmi3_xml_display_unit_t* fmi3_xml_get_unit_display_unit(fmi3_xml_unit_t*, size_t index);

/**
    \brief Get fmi3_SI_base_units_Num SI base units exponents associated with the unit.
*/
const int* fmi3_xml_get_SI_unit_exponents(fmi3_xml_unit_t*);

/**
    \brief Get factor to the corresponding SI base units.
*/
double fmi3_xml_get_SI_unit_factor(fmi3_xml_unit_t*);

/**
    \brief Get offset to the corresponding SI base units.
*/
double fmi3_xml_get_SI_unit_offset(fmi3_xml_unit_t*);

/**
    \brief Convert a value with respect to the unit to the
    value with respect to the SI base unit.
*/
double fmi3_xml_convert_to_SI_base_unit(double, fmi3_xml_unit_t*);

/**
    \brief Convert a value with respect to the SI base unit to the
    value with respect to the unit.
*/
double fmi3_xml_convert_from_SI_base_unit(double, fmi3_xml_unit_t*);

fmi3_xml_unit_t* fmi3_xml_get_base_unit(fmi3_xml_display_unit_t*);
const char* fmi3_xml_get_display_unit_name(fmi3_xml_display_unit_t*);
double fmi3_xml_get_display_unit_factor(fmi3_xml_display_unit_t*);
double fmi3_xml_get_display_unit_offset(fmi3_xml_display_unit_t*);

double fmi3_xml_convert_to_display_unit(double, fmi3_xml_display_unit_t*, int isRelativeQuantity);
double fmi3_xml_convert_from_display_unit(double, fmi3_xml_display_unit_t*, int isRelativeQuantity);
/**
@}
*/
#ifdef __cplusplus
}
#endif
#endif
