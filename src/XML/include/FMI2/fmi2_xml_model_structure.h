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

/** \file fmi2_xml_model_structure.h
*  \brief Public interface to the FMI XML C-library. Handling of vendor annotations.
*/

#ifndef FMI2_XML_MODELSTRUCTURE_H_
#define FMI2_XML_MODELSTRUCTURE_H_

#include "fmi2_xml_model_description.h"

#ifdef __cplusplus
extern "C" {
#endif

fmi2_xml_model_structure_t* fmi2_xml_allocate_model_structure(jm_callbacks* cb);

void fmi2_xml_free_model_structure(fmi2_xml_model_structure_t* ms);

/** \brief Get the list of all the output variables in the model.
* @param ms A model structure pointer (returned by fmi2_xml_get_model_structure)
* @return a variable list with all the output variables in the model.
*
* Note that variable lists are allocated dynamically and must be freed when not needed any longer.
* If the ModelStructure is available, then the list constructed from this information.
* Otherwise, causality property is used.
*/
    /*jm_vector(jm_voidp)* fmi2_xml_get_outputs(fmi2_xml_model_structure_t* ms);*/

/** \brief Get the list of all the input variables in the model.
* @param ms A model structure pointer (returned by fmi2_xml_get_model_structure)
* @return a variable list with all the input variables in the model.
*
* Note that variable lists are allocated dynamically and must be freed when not needed any longer.
*/
/*jm_vector(jm_voidp)* fmi2_xml_get_inputs(fmi2_xml_model_structure_t* ms);*/

/** \brief Get the list of all the state variables in the model.
* @param ms A model structure pointer (returned by fmi2_xml_get_model_structure)
* @return a variable list with all the continuous state variables in the model.
*
*     Note: variable lists are allocated dynamically and must be freed when not needed any longer.
*/
/*jm_vector(jm_voidp)* fmi2_xml_get_states(fmi2_xml_model_structure_t* ms);*/

/** \brief Get the list of all the derivative variables in the model.
* @param ms A model structure pointer (returned by fmi2_xml_get_model_structure)
* @return a variable list with all the continuous state derivatives in the model.
*
*     Note: variable lists are allocated dynamically and must be freed when not needed any longer.
*/
/*jm_vector(jm_voidp)* fmi2_xml_get_derivatives(fmi2_xml_model_structure_t* ms); */


#ifdef __cplusplus
}
#endif
#endif
