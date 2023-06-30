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

#ifndef FMI3_IMPORT_DIMENSION_LIST_IMPL_H
#define FMI3_IMPORT_DIMENSION_LIST_IMPL_H

#include "JM/jm_vector.h"

#include "FMI3/fmi3_import.h"
#include "FMI3/fmi3_import_dimension_list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef jm_vector(fmi3_xml_dimension_t)* fmi3_import_dimension_impl_list_t;

#ifdef __cplusplus
}
#endif

#endif /* FMI3_IMPORT_DIMENSION_LIST_IMPL_H */
