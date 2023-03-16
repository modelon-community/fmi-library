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
#include <string.h>

#include "FMI3/fmi3_import_variable.h"

#include "fmi3_import_impl.h"
#include "fmi3_import_dimension_list_impl.h"
#include "../XML/src/FMI3/fmi3_xml_dimension_impl.h" /* TODO: we need access to jm_vector(fmi3_xml_dimension_t), but we shouldn't include any xml impl. files */

fmi3_import_dimension_list_t* fmi3_import_alloc_dimension_list(fmi3_import_t* fmu, fmi3_import_variable_t* v) {
	jm_callbacks* cb = fmu->callbacks;
	fmi3_import_dimension_list_t* dl = (fmi3_import_dimension_list_t*)cb->malloc(sizeof(fmi3_import_dimension_list_t));
    if (!dl)
        return 0;
	dl->fmu = fmu;
    dl->vec = fmi3_xml_get_variable_dimension_vector(v);
    return dl;
}

void fmi3_import_free_dimension_list(fmi3_import_dimension_list_t* dl) {
	if (!dl)
        return;
	dl->vec->callbacks->free(dl);
}

/* get number of dimensions in list */
size_t fmi3_import_get_dimension_list_size(fmi3_import_dimension_list_t* dl) {
	if (!dl)
		return 0;

    return jm_vector_get_size(fmi3_xml_dimension_t)(dl->vec);
}

fmi3_import_dimension_t* fmi3_import_get_dimension_list_item(fmi3_import_dimension_list_t* dl, size_t idx) {
    if (!dl)
        return NULL;

    return jm_vector_get_itemp(fmi3_xml_dimension_t)(dl->vec, idx);
}

