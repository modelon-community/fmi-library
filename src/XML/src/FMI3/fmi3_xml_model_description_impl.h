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

#ifndef fmi3_xml_model_decription_impl_h_
#define fmi3_xml_model_decription_impl_h_

#include <stdarg.h>

#include <JM/jm_callbacks.h>
#include <JM/jm_vector.h>
#include <JM/jm_named_ptr.h>
#include <JM/jm_string_set.h>
#include <FMI3/fmi3_xml_model_description.h>

#include "fmi3_xml_unit_impl.h"
#include "fmi3_xml_type_impl.h"
#include "fmi3_xml_variable_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FMI3_DEFAULT_EXPERIMENT_TOLERANCE 1e-4
#define FMI3_DEFAULT_EXPERIMENT_STEPSIZE 1e-2
typedef struct fmi3_xml_default_experiment {
    double  startTime;
    int     startTimeDefined;

    double  stopTime;
    int     stopTimeDefined;

    double  tolerance;
    int     toleranceDefined;

    double  stepSize;
    int     stepSizeDefined;
} fmi3_xml_default_experiment;

typedef struct fmi3_xml_model_exchange {
    jm_vector(char) modelIdentifier;
} fmi3_xml_model_exchange;

typedef struct fmi3_xml_co_simulation {
    jm_vector(char) modelIdentifier;
    int hasFixedInternalStepSize;
    double fixedInternalStepSize;
    int hasRecommendedIntermediateInputSmoothness;
    int recommendedIntermediateInputSmoothness;
} fmi3_xml_co_simulation;

typedef struct fmi3_xml_scheduled_execution {
    jm_vector(char) modelIdentifier;
} fmi3_xml_scheduled_execution;

/*  ModelDescription is the entry point for the package*/
struct fmi3_xml_model_description_t {

    int isValid;
    int latestVariableValid; // for skipping the postprocessing of an invalid variable

    jm_callbacks* callbacks;

    jm_vector(char) fmi3_xml_standard_version;

    jm_vector(char) modelName;

    jm_vector(char) instantiationToken;

    jm_vector(char) description;

    jm_vector(char) author;
    jm_vector(char) copyright;
    jm_vector(char) license;

    jm_vector(char) version;
    jm_vector(char) generationTool;
    jm_vector(char) generationDateAndTime;

    fmi3_variable_naming_convension_enu_t namingConvension;

    fmi3_xml_default_experiment defaultExperiment;
    
    fmi3_xml_model_exchange modelExchange;
    fmi3_xml_co_simulation coSimulation;
    fmi3_xml_scheduled_execution scheduledExecution;

    jm_vector(jm_string) sourceFilesME;
    jm_vector(jm_string) sourceFilesCS;

    jm_vector(jm_string) logCategories;
    jm_vector(jm_string) logCategoryDescriptions;

    jm_vector(jm_string) vendorList;

    jm_vector(jm_named_ptr) unitDefinitions;
    jm_vector(jm_named_ptr) displayUnitDefinitions;

    fmi3_xml_type_definition_list_t typeDefinitions;

    jm_string_set descriptions;

    jm_vector(jm_named_ptr) variablesByName;
    jm_vector(jm_voidp) variablesOrigOrder;
    jm_vector(jm_voidp) variablesByVR;

    fmi3_fmu_kind_enu_t fmuKind;

    unsigned int capabilities[fmi3_capabilities_num];

    fmi3_xml_model_structure_t* modelStructure;
};

#ifdef __cplusplus
}
#endif

#endif
