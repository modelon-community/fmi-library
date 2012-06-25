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

#ifndef FMI1_XML_VARIABLEIMPL_H
#define FMI1_XML_VARIABLEIMPL_H

#include <JM/jm_vector.h>

#include <FMI1/fmi1_xml_model_description.h>

#include "fmi1_xml_type_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* General variable type is convenien to unify all the variable list operations */
struct fmi1_xml_variable_t {
    fmi1_xml_variable_type_base_t* typeBase;

    const char* description;
    jm_vector(jm_voidp)* directDependency;

	size_t originalIndex;
    fmi1_value_reference_t vr;
    char aliasKind;
    char variability;
    char causality;

    char name[1];
};

static int fmi1_xml_compare_vr (const void* first, const void* second) {
    fmi1_xml_variable_t* a = *(fmi1_xml_variable_t**)first;
    fmi1_xml_variable_t* b = *(fmi1_xml_variable_t**)second;
    fmi1_base_type_enu_t at = fmi1_xml_get_variable_base_type(a);
    fmi1_base_type_enu_t bt = fmi1_xml_get_variable_base_type(b);
	if(at == fmi1_base_type_enum) at = fmi1_base_type_int;
	if(bt == fmi1_base_type_enum) bt = fmi1_base_type_int;
    if(at!=bt) return at - bt;
    if(a->vr < b->vr) return -1;
    if(a->vr > b->vr) return 1;
    return ((int)a->aliasKind - (int)b->aliasKind);
}

void fmi1_xml_free_direct_dependencies(jm_named_ptr named);

#ifdef __cplusplus
}
#endif

#endif /* FMI1_XML_VARIABLEIMPL_H */
