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

typedef enum fmi3_xml_model_description_status_enu_t {
    fmi3_xml_model_description_enu_empty,
    fmi3_xml_model_description_enu_ok,
    fmi3_xml_model_description_enu_error
} fmi3_xml_model_description_status_enu_t;

/*  ModelDescription is the entry point for the package*/
struct fmi3_xml_model_description_t {

    jm_callbacks* callbacks;

    fmi3_xml_model_description_status_enu_t status;

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

    size_t numberOfContinuousStates;

    size_t numberOfEventIndicators;

    double defaultExperimentStartTime;

    double defaultExperimentStopTime;

#define FMI3_DEFAULT_EXPERIMENT_TOLERANCE 1e-4

    double defaultExperimentTolerance;

#define FMI3_DEFAULT_EXPERIMENT_STEPSIZE 1e-2

    double defaultExperimentStepSize;


    jm_vector(char) modelIdentifierME;

	jm_vector(char) modelIdentifierCS;

    jm_vector(jm_string) sourceFilesME;
    jm_vector(jm_string) sourceFilesCS;

    jm_vector(jm_string) logCategories;
    jm_vector(jm_string) logCategoryDescriptions;

	jm_vector(jm_string) vendorList;

    jm_vector(jm_named_ptr) unitDefinitions;
    jm_vector(jm_named_ptr) displayUnitDefinitions;

    fmi3_xml_type_definitions_t typeDefinitions;

    jm_string_set descriptions;

	jm_vector(jm_named_ptr) variablesByName;

    jm_vector(jm_voidp)* variablesOrigOrder;

	jm_vector(jm_voidp)* variablesByVR;

    fmi3_fmu_kind_enu_t fmuKind;

    unsigned int capabilities[fmi3_capabilities_Num];

	fmi3_xml_model_structure_t* modelStructure;
};

void fmi3_xml_report_error(fmi3_xml_model_description_t* md, const char* module, const char* fmt, ...);

void fmi3_xml_report_error_v(fmi3_xml_model_description_t* md, const char* module, const char* fmt, va_list ap);

void fmi3_xml_report_warning(fmi3_xml_model_description_t* md, const char* module, const char* fmt, ...);

void fmi3_xml_report_warning_v(fmi3_xml_model_description_t* md, const char* module, const char* fmt, va_list ap);


#ifdef __cplusplus
}
#endif

#endif

