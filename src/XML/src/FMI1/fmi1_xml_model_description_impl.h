#ifndef fmi1_xml_model_decription_impl_h_
#define fmi1_xml_model_decription_impl_h_

#include <stdarg.h>

#include <JM/jm_callbacks.h>
#include <JM/jm_vector.h>
#include <JM/jm_named_ptr.h>
#include <JM/jm_string_set.h>
#include <FMI1/fmi1_xml_model_description.h>

#include "fmi1_xml_unit_impl.h"
#include "fmi1_xml_type_impl.h"
#include "fmi1_xml_variable_impl.h"
#include "fmi1_xml_capabilities_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum fmi1_xml_model_description_status_enu_t {
    fmi1_xml_model_description_enu_empty,
    fmi1_xml_model_description_enu_ok,
    fmi1_xml_model_description_enu_error
} fmi1_xml_model_description_status_enu_t;

/*  ModelDescription is the entry point for the package*/
struct fmi1_xml_model_description_t {

    jm_callbacks* callbacks;

    fmi1_xml_model_description_status_enu_t status;

    jm_vector(char) fmi1_xml_standard_version;

    jm_vector(char) modelName;

    jm_vector(char) modelIdentifier;

    jm_vector(char) GUID;

    jm_vector(char) description;

    jm_vector(char) author;

    jm_vector(char) version;
    jm_vector(char) generationTool;
    jm_vector(char) generationDateAndTime;

    fmi1_variable_naming_convension_enu_t namingConvension;

    unsigned int numberOfContinuousStates;

    unsigned int numberOfEventIndicators;

    double defaultExperimentStartTime;

    double defaultExperimentStopTime;

#define FMI1_DEFAULT_EXPERIMENT_TOLERANCE 1e-4

    double defaultExperimentTolerance;

    jm_vector(jm_voidp) vendorList;

    jm_vector(jm_named_ptr) unitDefinitions;
    jm_vector(jm_named_ptr) displayUnitDefinitions;

    fmi1_xml_type_definitions_t typeDefinitions;

    jm_string_set descriptions;

	jm_vector(jm_named_ptr) variablesByName;

    jm_vector(jm_voidp)* variablesOrigOrder;

	jm_vector(jm_voidp)* variablesByVR;

	jm_vector(jm_voidp)* inputVariables;

	jm_vector(jm_voidp)* outputVariables;

    fmi1_fmu_kind_enu_t fmuKind;

    fmi1_xml_capabilities_t *capabilities;

    jm_vector(char) entryPoint;
    jm_vector(char) mimeType;
    int manual_start;
    jm_vector(jm_string) additionalModels;
};

void fmi1_xml_report_error(fmi1_xml_model_description_t* md, const char* module, const char* fmt, ...);

void fmi1_xml_report_error_v(fmi1_xml_model_description_t* md, const char* module, const char* fmt, va_list ap);

void fmi1_xml_report_warning(fmi1_xml_model_description_t* md, const char* module, const char* fmt, ...);

void fmi1_xml_report_warning_v(fmi1_xml_model_description_t* md, const char* module, const char* fmt, va_list ap);


#ifdef __cplusplus
}
#endif

#endif

