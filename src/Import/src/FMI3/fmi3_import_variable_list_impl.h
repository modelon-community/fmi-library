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

#ifndef FMI3_IMPORT_VARIABLELISTIMPL_H
#define FMI3_IMPORT_VARIABLELISTIMPL_H

#include <JM/jm_vector.h>
#include <FMI3/fmi3_import.h>
#include <FMI3/fmi3_import_variable_list.h>

#ifdef __cplusplus
extern "C" {
#endif

struct fmi3_import_variable_list_t {
    fmi3_import_t* fmu;
    jm_vector(jm_voidp) variables;
    fmi3_value_reference_t* vr;
};

#ifdef __cplusplus
}
#endif

#endif /* FMI3_XML_VARIABLELISTIMPL_H */
