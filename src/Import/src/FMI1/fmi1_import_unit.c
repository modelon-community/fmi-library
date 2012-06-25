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

#include "fmi1_import_impl.h"

fmi1_import_unit_t* fmi1_import_get_unit(fmi1_import_unit_definitions_t* ud, unsigned int  index) {
	return fmi1_xml_get_unit(ud, index);
}

const char* fmi1_import_get_unit_name(fmi1_import_unit_t* u) {
	return fmi1_xml_get_unit_name(u);
}

unsigned int fmi1_import_get_unit_display_unit_number(fmi1_import_unit_t* u) {
	return fmi1_xml_get_unit_display_unit_number(u);
}

fmi1_import_display_unit_t* fmi1_import_get_unit_display_unit(fmi1_import_unit_t* u, size_t index) {
	return fmi1_xml_get_unit_display_unit(u, index);
}

fmi1_import_unit_t* fmi1_import_get_base_unit(fmi1_import_display_unit_t* du) {
	return fmi1_xml_get_base_unit(du);
}

const char* fmi1_import_get_display_unit_name(fmi1_import_display_unit_t* du) {
	return fmi1_xml_get_display_unit_name(du);
}

fmi1_real_t fmi1_import_get_display_unit_gain(fmi1_import_display_unit_t* du) {
	return fmi1_xml_get_display_unit_gain(du);
}

fmi1_real_t fmi1_import_get_display_unit_offset(fmi1_import_display_unit_t* du) {
    return fmi1_xml_get_display_unit_offset(du);
}

fmi1_real_t fmi1_import_convert_to_display_unit(fmi1_real_t val , fmi1_import_display_unit_t* du, int isRelativeQuantity) {
    double gain = fmi1_import_get_display_unit_gain(du);
    double offset = fmi1_import_get_display_unit_offset(du);
    if(isRelativeQuantity)
        return val *gain;
    else
        return (val*gain + offset);
}

fmi1_real_t fmi1_import_convert_from_display_unit(fmi1_real_t val, fmi1_import_display_unit_t* du, int isRelativeQuantity) {
    double gain = fmi1_import_get_display_unit_gain(du);
    double offset = fmi1_import_get_display_unit_offset(du);
    if(isRelativeQuantity)
        return val/gain;
    else
        return (val - offset)/gain;
}
