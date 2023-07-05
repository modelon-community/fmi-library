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

// TODO: Cleanup
// #include <JM/jm_named_ptr.h>
// #include <FMI3/fmi3_types.h>
// #include <FMI3/fmi3_function_types.h>
// #include <FMI3/fmi3_enums.h>
// #include <FMI3/fmi3_capi.h>

#include "fmi3_import_impl.h"
// #include "fmi3_import_variable_list_impl.h"
// #include "fmi3_import_priv.h"

// static const char* module = "FMILIB";

int fmi3_import_get_has_terminals_and_icons(fmi3_import_t* fmu) {
    return fmu ? fmi3_xml_get_has_terminals_and_icons(fmu->termIcon) : 0;
}
