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

#include <stdio.h>
#include <stdarg.h>

#include <FMI2/fmi2_import_impl.h>
#include <FMI3/fmi3_import_impl.h>

int fmi2_import_get_has_terminals_and_icons(fmi2_import_t* fmu) {
    return fmu ? fmi2_xml_get_has_terminals_and_icons(fmu->termIcon) : 0;
}

int fmi3_import_get_has_terminals_and_icons(fmi3_import_t* fmu) {
    return fmu ? fmi3_xml_get_has_terminals_and_icons(fmu->termIcon) : 0;
}

fmi3_import_terminal_t* fmi2_import_get_terminal_by_name(fmi2_import_t* fmu, const char* name){
    return fmu ? fmi3_xml_get_terminal_by_name(fmu->termIcon, name) : NULL;
}

fmi3_import_terminal_t* fmi3_import_get_terminal_by_name(fmi3_import_t* fmu, const char* name){
    return fmu ? fmi3_xml_get_terminal_by_name(fmu->termIcon, name) : NULL;
}

const char* fmi3_import_get_terminal_name(fmi3_import_terminal_t* term){
    return term ? fmi3_xml_get_terminal_name(term) : NULL;
}
