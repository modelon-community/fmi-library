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

/** \file fmi3_xml_model_structure_impl.h
*  \brief Private header file. Definitions for the model structure interface.
*/

#ifndef FMI3_XML_MODELSTRUCTURE_IMPL_H_
#define FMI3_XML_MODELSTRUCTURE_IMPL_H_

#include <FMI3/fmi3_xml_model_structure.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Structure for keeping information about variable dependencies.
*/
typedef struct fmi3_xml_dependencies_t {
    /** Information is stored in the CSR format, with the extra speciality
     * that an empty row (i.e., startIndex[i] == startIndex[i + 1]) doubles for 
     * "depends on all" (missing dependencies attribute in the XML) and no dependencies,
     * in that case check dependencyOnAll[i]
    */

    /* Start index in dependency data, length = <number of variables> + 1 */
    jm_vector(size_t) startIndex;  
    /* Dependency data, valueReferences */
    jm_vector(size_t) dependencyVRs;
    /* Dependency types corresponding to dependencyVRs */
    jm_vector(char)   dependencyFactorKind;
    jm_vector(char)   dependencyOnAll;
} fmi3_xml_dependencies_t;

fmi3_xml_dependencies_t* fmi3_xml_allocate_dependencies(jm_callbacks* cb);
void fmi3_xml_free_dependencies(fmi3_xml_dependencies_t* dep);

struct fmi3_xml_model_structure_t {
    jm_vector(jm_voidp) outputs;
    jm_vector(jm_voidp) continuousStateDerivatives;
    jm_vector(jm_voidp) clockedStates;
    jm_vector(jm_voidp) initialUnknowns;
    jm_vector(jm_voidp) eventIndicators;

    fmi3_xml_dependencies_t* outputDeps;
    fmi3_xml_dependencies_t* continuousStateDerivativeDeps;
    fmi3_xml_dependencies_t* clockedStateDeps;
    fmi3_xml_dependencies_t* initialUnknownDeps;
    fmi3_xml_dependencies_t* eventIndicatorDeps;

    int isValidFlag;  /**\ brief The flag is used to signal if an error was discovered and the model structure is not usable */
};

#ifdef __cplusplus
}
#endif
#endif
