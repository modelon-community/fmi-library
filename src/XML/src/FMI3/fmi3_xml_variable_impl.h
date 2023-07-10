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

#ifndef FMI3_XML_VARIABLEIMPL_H
#define FMI3_XML_VARIABLEIMPL_H

#include <JM/jm_vector.h>

#include <FMI3/fmi3_xml_model_description.h>

#include "fmi3_xml_type_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Holds the VR until all Variables have been parsed. Then the variable
 * can be looked up.
 */
typedef union fmi3_xml_valueref_or_variable_union_t {
    fmi3_value_reference_t vr;
    fmi3_xml_variable_t* variable;
} fmi3_xml_valueref_or_variable_union_t;

struct fmi3_xml_alias_variable_list_t {
    jm_vector(jm_voidp) vec;
};

struct fmi3_xml_alias_variable_t {
    const char* description;
    fmi3_xml_display_unit_t* displayUnit;  // Only used for FLoatXX variables.
    char name[1];
};

struct fmi3_xml_dimension_t {
    int has_vr;
    fmi3_uint64_t start;           /* value of the start attribute if 'has_vr' is false, else unassigned */
    fmi3_uint32_t vr;              /* value of the valueReference attribute if 'has_vr' is true, else unassigned */
};

struct fmi3_xml_dimension_list_t {
    jm_vector(jm_voidp) vec;
};

/* General variable type is convenient to unify all the variable list operations */
struct fmi3_xml_variable_t {
    fmi3_xml_variable_type_base_t* type;     /** \brief Contains type-specific attributes. */

    const char* description;                 /** \brief Associated description */

    size_t originalIndex;                    /** \brief Index in the model description */

    /* NB: before parsing of <ModelVariables> has finished,
           derivativeOf and previous are stored as integer indices cast to pointers,
           until they can be looked up */
    fmi3_xml_valueref_or_variable_union_t derivativeOf; /** \brief Only for continuous FloatXX variables. The variable that this is the derivative of. */
    bool hasDerivativeOf;
    fmi3_xml_valueref_or_variable_union_t previous;      /** \brief The variable referenced in the previous attribute. */
    bool hasPrevious;

    fmi3_value_reference_t vr;                /** \brief Value reference */
    char initial;
    char variability;
    char causality;
    char reinit; /** \brief Only for continuous FloatXX variables */
    char canHandleMultipleSetPerTimeInstant;
    char intermediateUpdate;

    jm_vector(fmi3_value_reference_t)* clocks;   /* VRs in the clock attribute. NULL if attribute doesn't exist. */
    jm_vector(jm_voidp)* dimensions;
    jm_vector(jm_voidp)* aliases;

    /* temp fields during parsing - FIXME: If it's temp it should not be here (waste of memory), but on context */
    jm_string startAttr;

    /* 'name' field must be last, because its memory is allocated with jm_named_alloc[_v] */
    char name[1];
};

static int fmi3_xml_compare_vr(const void* first, const void* second) {
    fmi3_xml_variable_t* a = *(fmi3_xml_variable_t**)first;
    fmi3_xml_variable_t* b = *(fmi3_xml_variable_t**)second;
    if (a->vr == b->vr) return 0;
    return a->vr < b->vr ? -1 : 1;
}

#ifdef __cplusplus
}
#endif

#endif /* FMI3_XML_VARIABLEIMPL_H */
