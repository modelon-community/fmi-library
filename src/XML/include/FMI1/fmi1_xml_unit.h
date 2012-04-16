/*
    Copyright (C) 2012 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/** \file fmi1_xml_unit.h
*  \brief Public interface to the FMI XML C-library. Handling of variable units.
*/

#ifndef FMI1_XML_UNIT_H_
#define FMI1_XML_UNIT_H_

#include "fmi1_xml_model_description.h"

#ifdef __cplusplus
extern "C" {
#endif
		
	/**
	\addtogroup fmi1_xml
	@{
	\addtogroup fmi1_xml_units Functions for handling unit definitions.
	@}
	\addtogroup fmi1_xml_units Functions for handling unit definitions.
	@{
	*/

/** \brief Get a list of all the unit definitions */
fmi1_xml_unit_definitions_t* fmi1_xml_get_unit_definitions(fmi1_xml_model_description_t* md);
unsigned int  fmi1_xml_get_unit_definitions_number(fmi1_xml_unit_definitions_t*);
fmi1_xml_unit_t* fmi1_xml_get_unit(fmi1_xml_unit_definitions_t*, unsigned int  index);
const char* fmi1_xml_get_unit_name(fmi1_xml_unit_t*);
unsigned int fmi1_xml_get_unit_display_unit_number(fmi1_xml_unit_t*);
fmi1_xml_display_unit_t* fmi1_xml_get_unit_display_unit(fmi1_xml_unit_t*, size_t index);

fmi1_xml_display_unit_t* fmi1_xml_get_type_display_unit(fmi1_xml_real_typedef_t*);
fmi1_xml_unit_t* fmi1_xml_get_base_unit(fmi1_xml_display_unit_t*);
const char* fmi1_xml_get_display_unit_name(fmi1_xml_display_unit_t*);
double fmi1_xml_get_display_unit_gain(fmi1_xml_display_unit_t*);
double fmi1_xml_get_display_unit_offset(fmi1_xml_display_unit_t*);

double fmi1_xml_convert_to_display_unit(double, fmi1_xml_display_unit_t*, int isRelativeQuantity);
double fmi1_xml_convert_from_display_unit(double, fmi1_xml_display_unit_t*, int isRelativeQuantity);
/**
@}
*/
#ifdef __cplusplus
}
#endif
#endif
