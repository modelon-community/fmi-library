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
*/
jm_vector(jm_voidp)* fmi2_xml_get_outputs(fmi2_xml_model_structure_t* ms);

/** \brief Get the list of all the derivative variables in the model.
* @param ms A model structure pointer (returned by fmi2_xml_get_model_structure)
* @return a variable list with all the continuous state derivatives in the model.
*/
jm_vector(jm_voidp)* fmi2_xml_get_derivatives(fmi2_xml_model_structure_t* ms);

/** \brief Get the list of all the discrete state variables in the model.
* @param ms A model structure pointer (returned by fmi2_xml_get_model_structure)
* @return a variable list with all the discrete state variables in the model.
*/
jm_vector(jm_voidp)* fmi2_xml_get_discrete_states(fmi2_xml_model_structure_t* ms);

/** \brief Get the list of all the initial unknown variables in the model.
* @param ms A model structure pointer (returned by fmi2_xml_get_model_structure)
* @return a variable list with all the initial unknowns in the model.
*/
jm_vector(jm_voidp)* fmi2_xml_get_initial_unknowns(fmi2_xml_model_structure_t* ms);

#ifdef __cplusplus
}
#endif
#endif
