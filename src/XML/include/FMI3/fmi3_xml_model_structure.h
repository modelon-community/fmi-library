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

/** \file fmi3_xml_model_structure.h
*  \brief Public interface to the FMI XML C-library. Handling of ModelStructure.
*/

#ifndef FMI3_XML_MODELSTRUCTURE_H_
#define FMI3_XML_MODELSTRUCTURE_H_

#include "fmi3_xml_model_description.h"

#ifdef __cplusplus
extern "C" {
#endif

fmi3_xml_model_structure_t* fmi3_xml_allocate_model_structure(jm_callbacks* cb);

void fmi3_xml_free_model_structure(fmi3_xml_model_structure_t* ms);

/** \brief Get the list of all the output variables in the model.
* @param ms A model structure pointer (returned by fmi3_xml_get_model_structure)
* @return a variable list with all the output variables in the model.
*/
jm_vector(jm_voidp)* fmi3_xml_get_outputs(fmi3_xml_model_structure_t* ms);

/** \brief Get the list of all the continuous state derivative variables in the model.
* @param ms A model structure pointer (returned by fmi3_xml_get_model_structure)
* @return a variable list with all the continuous state derivatives in the model.
*/
jm_vector(jm_voidp)* fmi3_xml_get_continuous_state_derivatives(fmi3_xml_model_structure_t* ms);

/** \brief Get the list of all the clocked states in the model.
* @param ms A model structure pointer (returned by fmi3_xml_get_model_structure)
* @return a variable list with all the clocked states in the model.
*/
jm_vector(jm_voidp)* fmi3_xml_get_clocked_states(fmi3_xml_model_structure_t* ms);

/** \brief Get the list of all the initial unknown variables in the model.
* @param ms A model structure pointer (returned by fmi3_xml_get_model_structure)
* @return a variable list with all the initial unknowns in the model.
*/
jm_vector(jm_voidp)* fmi3_xml_get_initial_unknowns(fmi3_xml_model_structure_t* ms);

/** \brief Get the list of all the event indicator variables in the model.
* @param ms A model structure pointer (returned by fmi3_xml_get_model_structure)
* @return a variable list with all the event indicator in the model.
*/
jm_vector(jm_voidp)* fmi3_xml_get_event_indicators(fmi3_xml_model_structure_t* ms);

/** \brief Get dependency information for Outputs. 
 * @param ms              - A model structure pointer (returned by fmi3_xml_get_model_structure)
 * @param variable        - A model variable, returned from fmi3_import_get_variable(varList, <index>), where 
 *                          varList = fmi3_import_get_outputs_list(...);
 * @param numDependencies - outputs number of dependencies, equals to SIZE_MAX in case of missing dependencies = depends on all
 * @param dependency      - outputs a pointer to the dependency valueReferences data.
 *                          NULL if numDependencies == 0 || numDependencies == <max(size_t)>
 * @param factorKind      - outputs a pointer to the factor kind data. The values can be converted to ::fmi3_dependency_factor_kind_enu_t 
 *                          NULL if numDependencies == 0 || numDependencies == <max(size_t)>
 * @return - non-zero if variable is not an EventIndicator or other unexpected issue
 */
int fmi3_xml_get_output_dependencies(fmi3_xml_model_structure_t* ms, fmi3_xml_variable_t* variable,
        size_t* numDependencies, size_t** dependency, char** factorKind);

/** \brief Get dependency information for EventIndicators. 
 * @param variable        - A model variable, returned from fmi3_import_get_variable(varList, <index>), where 
 *                          varList = fmi3_xml_get_event_indicators(...);
 * @param numDependencies - ouputs number of dependencies, equals to <max(size_t)> in case of missing dependencies = depends on all
 * @param dependency      - outputs a pointer to the dependency valueReferences data.
 *                          NULL if numDependencies == 0 || numDependencies == <max(size_t)>
 * @param factorKind      - outputs a pointer to the factor kind data. The values can be converted to ::fmi3_dependency_factor_kind_enu_t 
 *                          NULL if numDependencies == 0 || numDependencies == <max(size_t)>
 * @return - non-zero if variable is not an EventIndicator or other unexpected issue
 */
int fmi3_xml_get_continuous_state_derivative_dependencies(fmi3_xml_model_structure_t* ms, fmi3_xml_variable_t* variable,
        size_t* numDependencies, size_t** dependency, char** factorKind);

/** \brief Get dependency information for EventIndicators. 
 * @param variable        - A model variable, returned from fmi3_import_get_variable(varList, <index>), where 
 *                          varList = fmi3_xml_get_event_indicators(...);
 * @param numDependencies - ouputs number of dependencies, equals to <max(size_t)> in case of missing dependencies = depends on all
 * @param dependency      - outputs a pointer to the dependency valueReferences data.
 *                          NULL if numDependencies == 0 || numDependencies == <max(size_t)>
 * @param factorKind      - outputs a pointer to the factor kind data. The values can be converted to ::fmi3_dependency_factor_kind_enu_t 
 *                          NULL if numDependencies == 0 || numDependencies == <max(size_t)>
 * @return - non-zero if variable is not an EventIndicator or other unexpected issue
 */
int fmi3_xml_get_clocked_state_dependencies(fmi3_xml_model_structure_t* ms, fmi3_xml_variable_t* variable,
        size_t* numDependencies, size_t** dependency, char** factorKind);

/** \brief Get dependency information for EventIndicators. 
 * @param variable        - A model variable, returned from fmi3_import_get_variable(varList, <index>), where 
 *                          varList = fmi3_xml_get_event_indicators(...);
 * @param numDependencies - ouputs number of dependencies, equals to <max(size_t)> in case of missing dependencies = depends on all
 * @param dependency      - outputs a pointer to the dependency valueReferences data.
 *                          NULL if numDependencies == 0 || numDependencies == <max(size_t)>
 * @param factorKind      - outputs a pointer to the factor kind data. The values can be converted to ::fmi3_dependency_factor_kind_enu_t 
 *                          NULL if numDependencies == 0 || numDependencies == <max(size_t)>
 * @return - non-zero if variable is not an EventIndicator or other unexpected issue
 */
int fmi3_xml_get_initial_unknown_dependencies(fmi3_xml_model_structure_t* ms, fmi3_xml_variable_t* variable,
        size_t* numDependencies, size_t** dependency, char** factorKind);

/** \brief Get dependency information for EventIndicators. 
 * @param variable        - A model variable, returned from fmi3_import_get_variable(varList, <index>), where 
 *                          varList = fmi3_xml_get_event_indicators(...);
 * @param numDependencies - ouputs number of dependencies, equals to <max(size_t)> in case of missing dependencies = depends on all
 * @param dependency      - outputs a pointer to the dependency valueReferences data.
 *                          NULL if numDependencies == 0 || numDependencies == <max(size_t)>
 * @param factorKind      - outputs a pointer to the factor kind data. The values can be converted to ::fmi3_dependency_factor_kind_enu_t 
 *                          NULL if numDependencies == 0 || numDependencies == <max(size_t)>
 * @return - non-zero if variable is not an EventIndicator or other unexpected issue
 */
int fmi3_xml_get_event_indicator_dependencies(fmi3_xml_model_structure_t* ms, fmi3_xml_variable_t* variable,
        size_t* numDependencies, size_t** dependency, char** factorKind);

#ifdef __cplusplus
}
#endif
#endif
