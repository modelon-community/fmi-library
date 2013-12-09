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

#ifndef FMI2_XML_VARIABLEIMPL_H
#define FMI2_XML_VARIABLEIMPL_H

#include <JM/jm_vector.h>

#include <FMI2/fmi2_xml_model_description.h>

#include "fmi2_xml_type_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* General variable type is convenien to unify all the variable list operations */
struct fmi2_xml_variable_t {
    fmi2_xml_variable_type_base_t* typeBase; /** \brief Type information of the variable */

    const char* description;				 /** \brief Associate description */

	size_t originalIndex;					/** \brief Index in the model description */

    /* NB: before parsing of <ModelVariables> has finished,
           derivativeOf and previous are stored as integer indices cast to pointers,
           until they can be looked up */
    fmi2_xml_variable_t *derivativeOf;      /** \brief Only for continuous Real variables. If non-NULL, the variable that this is the derivative of. */
    fmi2_xml_variable_t *previous;          /** \brief If non-NULL, the variable that holds the value of this variable at the previous super-dense time instant. */

    fmi2_value_reference_t vr;				/** \brief Value reference */
    char aliasKind;
    char initial;
    char variability;
    char causality;
    char reinit; /** \brief Only for continuous Real variables */
    char canHandleMultipleSetPerTimeInstant;

    char name[1];
};

static int fmi2_xml_compare_vr (const void* first, const void* second) {
    fmi2_xml_variable_t* a = *(fmi2_xml_variable_t**)first;
    fmi2_xml_variable_t* b = *(fmi2_xml_variable_t**)second;
    fmi2_base_type_enu_t at = fmi2_xml_get_variable_base_type(a);
    fmi2_base_type_enu_t bt = fmi2_xml_get_variable_base_type(b);
	if(at == fmi2_base_type_enum) at = fmi2_base_type_int;
	if(bt == fmi2_base_type_enum) bt = fmi2_base_type_int;
    if(at!=bt) return at - bt;
    if(a->vr < b->vr) return -1;
    if(a->vr > b->vr) return 1;
    return ((int)a->aliasKind - (int)b->aliasKind);
}

#ifdef __cplusplus
}
#endif

#endif /* FMI2_XML_VARIABLEIMPL_H */
