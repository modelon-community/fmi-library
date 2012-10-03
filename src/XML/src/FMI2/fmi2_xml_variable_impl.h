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

	/*
		Note: in principle a variable can correspond to two different input/states/ders/outputs indices.
		So when going from a variable to its structure information having one index is not always enough.
	*/
	size_t inputIndex;						/** \brief Index in the list of inputs (1-based, 0 if not an input) */
	size_t stateIndex;						/** \brief Index in the list of states (1-based, 0 if not a state) */
	size_t derivativeIndex;					/** \brief Index in the list of derivatives (1-based, 0 if not a derivative) */
	size_t outputIndex;						/** \brief Index in the list of outputs (1-based, 0 if not an ouput) */

    fmi2_value_reference_t vr;				/** \brief Value reference */
    char aliasKind;
    char initial;
    char variability;
    char causality;

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
