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

/** \file fmi2_xml_model_structure_impl.h
*  \brief Private header file. Definitions for the model structure interface.
*/

#ifndef FMI2_XML_MODELSTRUCTURE_IMPL_H_
#define FMI2_XML_MODELSTRUCTURE_IMPL_H_

#include <FMI2/fmi2_xml_model_structure.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Structure for keeping information about variable dependencies.
*/
typedef struct fmi2_xml_dependencies_t {
	int isRowMajor;	/** Information is stored in row-major format flag */

	/** Start index in dependency data for the corresponding row (isRowMajor=1) or column (isRowMajor = 0) */
	jm_vector(size_t) startIndex;  

	/** Column indices (isRowMajor=1) or row indices (isRowMajor=0)
		Note that indices are 1-based. 0 has a special meaning - depends on all.
	*/
	jm_vector(size_t) dependencyIndex;
	jm_vector(char)   dependencyFactorKind;
} fmi2_xml_dependencies_t;

fmi2_xml_dependencies_t* fmi2_xml_allocate_dependencies(jm_callbacks* cb);
void fmi2_xml_free_dependencies(fmi2_xml_dependencies_t* dep);
	
struct fmi2_xml_model_structure_t {
	jm_vector(jm_voidp) outputs;
	jm_vector(jm_voidp) derivatives;
	jm_vector(jm_voidp) discreteStates;
	jm_vector(jm_voidp) initialUnknowns;

    fmi2_xml_dependencies_t* outputDeps;
    fmi2_xml_dependencies_t* derivativeDeps;
    fmi2_xml_dependencies_t* discreteStateDeps;
    fmi2_xml_dependencies_t* initialUnknownDeps;

	int isValidFlag;  /**\ brief The flag is used to signal if an error was discovered and the model structure is not usable */
};

#ifdef __cplusplus
}
#endif
#endif
