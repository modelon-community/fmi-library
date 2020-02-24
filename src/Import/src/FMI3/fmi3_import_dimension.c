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

#include "fmi3_import_impl.h"
#include "FMI3/fmi3_import_dimension.h"

int fmi3_import_get_dimension_has_vr(fmi3_import_dimension_t* dim) {
    return fmi3_xml_get_dimension_has_vr(dim);
}

int fmi3_import_get_dimension_has_start(fmi3_import_dimension_t* dim) {
    return fmi3_xml_get_dimension_has_start(dim);
}

int fmi3_import_get_dimension_start(fmi3_import_dimension_t* dim) {
    return fmi3_xml_get_dimension_start(dim);
}

fmi3_value_reference_t fmi3_import_get_dimension_vr(fmi3_import_dimension_t* dim) {
    return fmi3_xml_get_dimension_vr(dim);
}

