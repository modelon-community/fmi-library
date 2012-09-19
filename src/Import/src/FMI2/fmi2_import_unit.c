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

#include "fmi2_import_impl.h"

fmi2_import_unit_t* fmi2_import_get_unit(fmi2_import_unit_definitions_t* ud, unsigned int  index) {
	return fmi2_xml_get_unit(ud, index);
}

const char* fmi2_import_get_unit_name(fmi2_import_unit_t* u) {
	return fmi2_xml_get_unit_name(u);
}

const int* fmi2_import_get_SI_unit_exponents(fmi2_import_unit_t* u){
	return fmi2_xml_get_SI_unit_exponents(u);
}

/**
	\brief Get factor to the corresponding SI base units.
*/
double fmi2_import_get_SI_unit_factor(fmi2_import_unit_t* u) {
	return fmi2_xml_get_SI_unit_factor(u);
}

/**
	\brief Get offset to the corresponding SI base units.
*/
double fmi2_import_get_SI_unit_offset(fmi2_import_unit_t* u) {
	return fmi2_xml_get_SI_unit_offset(u);
}

/**
	\brief Convert a value with respect to the unit to the
	value with respect to the SI base unit.
*/
double fmi2_import_convert_to_SI_base_unit(double v, fmi2_import_unit_t* u) {
	return fmi2_xml_convert_to_SI_base_unit(v,u);
}

/**
	\brief Convert a value with respect to the SI base unit to the
	value with respect to the unit.
*/
double fmi2_import_convert_from_SI_base_unit(double v , fmi2_import_unit_t* u) {
	return fmi2_xml_convert_from_SI_base_unit( v, u);
}

unsigned int fmi2_import_get_unit_display_unit_number(fmi2_import_unit_t* u) {
	return fmi2_xml_get_unit_display_unit_number(u);
}

fmi2_import_display_unit_t* fmi2_import_get_unit_display_unit(fmi2_import_unit_t* u, size_t index) {
	return fmi2_xml_get_unit_display_unit(u, index);
}

fmi2_import_unit_t* fmi2_import_get_base_unit(fmi2_import_display_unit_t* du) {
	return fmi2_xml_get_base_unit(du);
}

const char* fmi2_import_get_display_unit_name(fmi2_import_display_unit_t* du) {
	return fmi2_xml_get_display_unit_name(du);
}

fmi2_real_t fmi2_import_get_display_unit_factor(fmi2_import_display_unit_t* du) {
	return fmi2_xml_get_display_unit_factor(du);
}

fmi2_real_t fmi2_import_get_display_unit_offset(fmi2_import_display_unit_t* du) {
    return fmi2_xml_get_display_unit_offset(du);
}

fmi2_real_t fmi2_import_convert_to_display_unit(fmi2_real_t val , fmi2_import_display_unit_t* du, int isRelativeQuantity) {
    double factor = fmi2_import_get_display_unit_factor(du);
    double offset = fmi2_import_get_display_unit_offset(du);
    if(isRelativeQuantity)
        return val *factor;
    else
        return (val*factor + offset);
}

fmi2_real_t fmi2_import_convert_from_display_unit(fmi2_real_t val, fmi2_import_display_unit_t* du, int isRelativeQuantity) {
    double factor = fmi2_import_get_display_unit_factor(du);
    double offset = fmi2_import_get_display_unit_offset(du);
    if(isRelativeQuantity)
        return val/factor;
    else
        return (val - offset)/factor;
}
