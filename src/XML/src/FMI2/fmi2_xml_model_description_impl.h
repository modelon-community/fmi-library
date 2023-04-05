#ifndef fmi2_xml_model_decription_impl_h_
#define fmi2_xml_model_decription_impl_h_

#include <stdarg.h>

#include <JM/jm_callbacks.h>
#include <JM/jm_vector.h>
#include <JM/jm_named_ptr.h>
#include <JM/jm_string_set.h>
#include <FMI2/fmi2_xml_model_description.h>

#include "fmi2_xml_unit_impl.h"
#include "fmi2_xml_type_impl.h"
#include "fmi2_xml_variable_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum fmi2_xml_model_description_status_enu_t {
    fmi2_xml_model_description_enu_empty,
    fmi2_xml_model_description_enu_ok,
    fmi2_xml_model_description_enu_error
} fmi2_xml_model_description_status_enu_t;

#define FMI2_DEFAULT_EXPERIMENT_TOLERANCE 1e-4
#define FMI2_DEFAULT_EXPERIMENT_STEPSIZE 1e-2
typedef struct fmi2_xml_default_experiment {
    double  startTime;
    int     startTimeDefined;

    double  stopTime;
    int     stopTimeDefined;

    double  tolerance;
    int     toleranceDefined;

    double  stepSize;
    int     stepSizeDefined;
} fmi2_xml_default_experiment;

/*  ModelDescription is the entry point for the package*/
struct fmi2_xml_model_description_t {

    jm_callbacks* callbacks;

    fmi2_xml_model_description_status_enu_t status;

    jm_vector(char) fmi2_xml_standard_version;

    jm_vector(char) modelName;

    jm_vector(char) GUID;

    jm_vector(char) description;

    jm_vector(char) author;
    jm_vector(char) copyright;
    jm_vector(char) license;

    jm_vector(char) version;
    jm_vector(char) generationTool;
    jm_vector(char) generationDateAndTime;

    fmi2_variable_naming_convension_enu_t namingConvension;

    size_t numberOfContinuousStates;

    size_t numberOfEventIndicators;

    fmi2_xml_default_experiment defaultExperiment;

    jm_vector(char) modelIdentifierME;

    jm_vector(char) modelIdentifierCS;

    jm_vector(jm_string) sourceFilesME;
    jm_vector(jm_string) sourceFilesCS;

    jm_vector(jm_string) logCategories;
    jm_vector(jm_string) logCategoryDescriptions;

    jm_vector(jm_string) vendorList;

    jm_vector(jm_named_ptr) unitDefinitions;
    jm_vector(jm_named_ptr) displayUnitDefinitions;

    fmi2_xml_type_definitions_t typeDefinitions;

    jm_string_set descriptions;

    jm_vector(jm_named_ptr) variablesByName;

    jm_vector(jm_voidp)* variablesOrigOrder;

    jm_vector(jm_voidp)* variablesByVR;

    fmi2_fmu_kind_enu_t fmuKind;

    unsigned int capabilities[fmi2_capabilities_Num];

    fmi2_xml_model_structure_t* modelStructure;
};

#ifdef __cplusplus
}
#endif

#endif

