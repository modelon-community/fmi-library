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

#ifndef FMI3_XML_VARIABLEIMPL_H
#define FMI3_XML_VARIABLEIMPL_H

#include <JM/jm_vector.h>

#include <FMI3/fmi3_xml_model_description.h>

#include "fmi3_xml_type_impl.h"
#include "fmi3_xml_dimension_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* General variable type is convenient to unify all the variable list operations */
struct fmi3_xml_variable_t {
    fmi3_xml_variable_type_base_t* type;     /** \brief Type information of the variable, and start values, and maybe more. */

    const char* description;                 /** \brief Associated description */

    size_t originalIndex;                    /** \brief Index in the model description */

    /* NB: before parsing of <ModelVariables> has finished,
           derivativeOf and previous are stored as integer indices cast to pointers,
           until they can be looked up */
    fmi3_xml_variable_t *derivativeOf;      /** \brief Only for continuous FloatXX variables. If non-NULL, the variable that this is the derivative of. */
    fmi3_xml_variable_t *previous;          /** \brief If non-NULL, the variable that holds the value of this variable at the previous super-dense time instant. */

    fmi3_value_reference_t vr;                /** \brief Value reference */
    char aliasKind;
    char initial;
    char variability;
    char causality;
    char reinit; /** \brief Only for continuous FloatXX variables */
    char canHandleMultipleSetPerTimeInstant;

    /* array fields */
    jm_vector(fmi3_xml_dimension_t) dimensionsVector; /* stores the dimensions and their attributes */
    /*
     * Dynamic memory storage for resolved dimensions (i.e. vr's are dereferenced).
     * This field will be exposed to the user, but FMIL handles memory management,
     * and this is a convenient place to store it.
     */
    unsigned int* dimensionsArray; /* TODO: this var can probably be removed now, since API was restructured (reduced) */

    /* temp fields during parsing*/
    jm_string startAttr;

    /* 'name' field must be last, because its memory is allocated with jm_named_alloc[_v] */
    char name[1];
};

static int fmi3_xml_compare_vr(const void* first, const void* second) {
    fmi3_xml_variable_t* a = *(fmi3_xml_variable_t**)first;
    fmi3_xml_variable_t* b = *(fmi3_xml_variable_t**)second;
    fmi3_base_type_enu_t at = fmi3_xml_get_variable_base_type(a);
    fmi3_base_type_enu_t bt = fmi3_xml_get_variable_base_type(b);
    if(at == fmi3_base_type_enum) at = fmi3_base_type_int32;
    if(bt == fmi3_base_type_enum) bt = fmi3_base_type_int32;
    if(at!=bt) return at - bt;
    if(a->vr < b->vr) return -1;
    if(a->vr > b->vr) return 1;
    return ((int)a->aliasKind - (int)b->aliasKind);
}

#ifdef __cplusplus
}
#endif

#endif /* FMI3_XML_VARIABLEIMPL_H */
