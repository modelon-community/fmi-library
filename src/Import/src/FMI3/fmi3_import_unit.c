/*
    Copyright (C) 2023 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#include "fmi3_import_impl.h"

fmi3_import_unit_t* fmi3_import_get_unit(fmi3_import_unit_definitions_t* ud, unsigned int index) {
    return fmi3_xml_get_unit(ud, index);
}

const char* fmi3_import_get_unit_name(fmi3_import_unit_t* u) {
    return fmi3_xml_get_unit_name(u);
}

const int* fmi3_import_get_SI_unit_exponents(fmi3_import_unit_t* u) {
    return fmi3_xml_get_SI_unit_exponents(u);
}

/**
    \brief Get factor to the corresponding SI base units.
*/
double fmi3_import_get_SI_unit_factor(fmi3_import_unit_t* u) {
    return fmi3_xml_get_SI_unit_factor(u);
}

/**
    \brief Get offset to the corresponding SI base units.
*/
double fmi3_import_get_SI_unit_offset(fmi3_import_unit_t* u) {
    return fmi3_xml_get_SI_unit_offset(u);
}

/**
    \brief Convert a value with respect to the unit to the
    value with respect to the SI base unit.
*/
double fmi3_import_convert_to_SI_base_unit(double v, fmi3_import_unit_t* u) {
    return fmi3_xml_convert_to_SI_base_unit(v,u);
}

/**
    \brief Convert a value with respect to the SI base unit to the
    value with respect to the unit.
*/
double fmi3_import_convert_from_SI_base_unit(double v, fmi3_import_unit_t* u) {
    return fmi3_xml_convert_from_SI_base_unit( v, u);
}

unsigned int fmi3_import_get_unit_display_unit_number(fmi3_import_unit_t* u) {
    return fmi3_xml_get_unit_display_unit_number(u);
}

fmi3_import_display_unit_t* fmi3_import_get_unit_display_unit(fmi3_import_unit_t* u, size_t index) {
    return fmi3_xml_get_unit_display_unit(u, index);
}

fmi3_import_unit_t* fmi3_import_get_base_unit(fmi3_import_display_unit_t* du) {
    return fmi3_xml_get_base_unit(du);
}

const char* fmi3_import_get_display_unit_name(fmi3_import_display_unit_t* du) {
    return fmi3_xml_get_display_unit_name(du);
}

double fmi3_import_get_display_unit_factor(fmi3_import_display_unit_t* du) {
    return fmi3_xml_get_display_unit_factor(du);
}

double fmi3_import_get_display_unit_offset(fmi3_import_display_unit_t* du) {
    return fmi3_xml_get_display_unit_offset(du);
}

int fmi3_import_get_display_unit_inverse(fmi3_import_display_unit_t* du) {
    return (int)fmi3_xml_get_display_unit_inverse(du);
}

fmi3_float64_t fmi3_import_float64_convert_to_display_unit(fmi3_float64_t val, fmi3_import_display_unit_t* du, int isRelativeQuantity) {
    return fmi3_xml_float64_convert_to_display_unit(val, du, isRelativeQuantity);
}

fmi3_float64_t fmi3_import_float64_convert_from_display_unit(fmi3_float64_t val, fmi3_import_display_unit_t* du, int isRelativeQuantity) {
    return fmi3_xml_float64_convert_from_display_unit(val, du, isRelativeQuantity);
}

fmi3_float32_t fmi3_import_float32_convert_to_display_unit(fmi3_float32_t val, fmi3_import_display_unit_t* du, int isRelativeQuantity) {
    return fmi3_xml_float32_convert_to_display_unit(val, du, isRelativeQuantity);
}

fmi3_float32_t fmi3_import_float32_convert_from_display_unit(fmi3_float32_t val, fmi3_import_display_unit_t* du, int isRelativeQuantity) {
    return fmi3_xml_float32_convert_from_display_unit(val, du, isRelativeQuantity);
}
