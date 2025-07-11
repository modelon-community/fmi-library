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

#include <stdio.h>


#include "JM/jm_named_ptr.h"
#include "fmi3_xml_model_description_impl.h"
#include "fmi3_xml_model_structure_impl.h"
#include "fmi3_xml_parser.h"
#include "FMI3/fmi3_enums.h"

static const char* module = "FMI3XML";

fmi3_xml_model_description_t* fmi3_xml_allocate_model_description(jm_callbacks* callbacks) {
    jm_callbacks* cb;
    fmi3_xml_model_description_t* md;

    if(callbacks) {
        cb = callbacks;
    }
    else {
        cb = jm_get_default_callbacks();
    }
    md = (fmi3_xml_model_description_t*)cb->calloc(1, sizeof(fmi3_xml_model_description_t));
    if (!md) {
        jm_log_fatal(cb, module, "Could not allocate memory");
        return 0;
    }

    md->isValid = 1;
    md->latestVariableIsValid = 1;

    md->callbacks = cb;

    jm_vector_init(char)(&md->fmi3_xml_standard_version, 0, cb);
    jm_vector_init(char)(&md->modelName,                 0, cb);
    jm_vector_init(char)(&md->instantiationToken,        0, cb);
    jm_vector_init(char)(&md->description,               0, cb);
    jm_vector_init(char)(&md->author,                    0, cb);
    jm_vector_init(char)(&md->license,                   0, cb);
    jm_vector_init(char)(&md->copyright,                 0, cb);
    jm_vector_init(char)(&md->version,                   0, cb);
    jm_vector_init(char)(&md->generationTool,            0, cb);
    jm_vector_init(char)(&md->generationDateAndTime,     0, cb);


    md->namingConvension = fmi3_naming_enu_flat;

    /* DefaultExperiment: we must initialize '*Defined' variables since the element is optional */
    md->defaultExperiment.startTimeDefined = 0;
    md->defaultExperiment.stopTimeDefined  = 0;
    md->defaultExperiment.toleranceDefined = 0;
    md->defaultExperiment.stepSizeDefined  = 0;

    md->defaultExperiment.startTime = 0;
    md->defaultExperiment.stopTime  = 1.0;
    md->defaultExperiment.tolerance = FMI3_DEFAULT_EXPERIMENT_TOLERANCE;
    md->defaultExperiment.stepSize  = FMI3_DEFAULT_EXPERIMENT_STEPSIZE;

    jm_vector_init(char)(&md->modelExchange.modelIdentifier, 0, cb);

    jm_vector_init(char)(&md->coSimulation.modelIdentifier, 0, cb);
    md->coSimulation.hasFixedInternalStepSize = 0;
    md->coSimulation.fixedInternalStepSize    = 0.0;
    md->coSimulation.hasRecommendedIntermediateInputSmoothness = 0;
    md->coSimulation.recommendedIntermediateInputSmoothness    = 0;

    jm_vector_init(char)(&md->scheduledExecution.modelIdentifier, 0, cb);

    jm_vector_init(jm_string)(&md->sourceFilesME, 0, cb);
    jm_vector_init(jm_string)(&md->sourceFilesCS, 0, cb);

    jm_vector_init(jm_string)(&md->vendorList, 0, cb);
    jm_vector_init(jm_string)(&md->logCategories, 0, cb);
    jm_vector_init(jm_string)(&md->logCategoryDescriptions, 0, cb);

    jm_vector_init(jm_named_ptr)(&md->unitDefinitions, 0, cb);
    jm_vector_init(jm_named_ptr)(&md->displayUnitDefinitions, 0, cb);

    fmi3_xml_init_type_definitions(&md->typeDefinitions, cb);

    jm_vector_init(jm_named_ptr)(&md->variablesByName, 0, cb);
    jm_vector_init(jm_voidp)(&md->variablesOrigOrder, 0, cb);
    jm_vector_init(jm_voidp)(&md->variablesByVR, 0, cb);

    jm_vector_init(jm_string)(&md->descriptions, 0, cb);

    md->fmuKind = fmi3_fmu_kind_unknown;

    for (size_t i = 0; i < fmi3_capabilities_num; i++) {
        md->capabilities[i] = 0;
    }

    return md;
}

void fmi3_xml_clear_model_description(fmi3_xml_model_description_t* md) {

    void(*cb_free)(const char*) = (void(*)(const char*))md->callbacks->free;
    size_t i; /* loop variable */

    jm_vector_free_data(char)(&md->fmi3_xml_standard_version);
    jm_vector_free_data(char)(&md->modelName);
    jm_vector_free_data(char)(&md->instantiationToken);
    jm_vector_free_data(char)(&md->description);
    jm_vector_free_data(char)(&md->author);
    jm_vector_free_data(char)(&md->license);
    jm_vector_free_data(char)(&md->copyright);
    jm_vector_free_data(char)(&md->version);
    jm_vector_free_data(char)(&md->generationTool);
    jm_vector_free_data(char)(&md->generationDateAndTime);

    md->namingConvension = fmi3_naming_enu_flat;

    md->defaultExperiment.startTime = 0;
    md->defaultExperiment.stopTime = 0;
    md->defaultExperiment.tolerance = 0;
    md->defaultExperiment.stepSize = 0;

    jm_vector_free_data(char)(&md->modelExchange.modelIdentifier);
    jm_vector_free_data(char)(&md->coSimulation.modelIdentifier);
    jm_vector_free_data(char)(&md->scheduledExecution.modelIdentifier);

    jm_vector_foreach(jm_string)(&md->sourceFilesME, cb_free);
    jm_vector_free_data(jm_string)(&md->sourceFilesME);

    jm_vector_foreach(jm_string)(&md->sourceFilesCS, cb_free);
    jm_vector_free_data(jm_string)(&md->sourceFilesCS);

    jm_vector_foreach(jm_string)(&md->vendorList, cb_free);
    jm_vector_free_data(jm_string)(&md->vendorList);

    jm_vector_foreach(jm_string)(&md->logCategories, cb_free);
    jm_vector_free_data(jm_string)(&md->logCategories);

    jm_vector_foreach(jm_string)(&md->logCategoryDescriptions, cb_free);
    jm_vector_free_data(jm_string)(&md->logCategoryDescriptions);

    jm_named_vector_free_data(&md->unitDefinitions);
    jm_named_vector_free_data(&md->displayUnitDefinitions);

    /* --- variables --- */
    {
        /* free memory inside the variables */
        for (i = 0; i < jm_vector_get_size(jm_voidp)(&md->variablesOrigOrder); i++) {
            fmi3_xml_variable_t* var = (fmi3_xml_variable_t*)jm_vector_get_item(jm_voidp)(&md->variablesOrigOrder, i);
            fmi3_xml_free_variable(md->callbacks, var);
        }
        /* free other vector data  */
        jm_vector_free_data(jm_voidp)(&md->variablesOrigOrder);
        jm_vector_free_data(jm_voidp)(&md->variablesByVR);
        /* free original pointers and variable structs */
        jm_vector_free_data(jm_named_ptr)(&md->variablesByName);
    }

    /* type definition must be freed after variables, because arrays check for number of start values  */
    fmi3_xml_free_type_definitions_data(&md->typeDefinitions);

    jm_vector_foreach(jm_string)(&md->descriptions, cb_free);
    jm_vector_free_data(jm_string)(&md->descriptions);

    fmi3_xml_free_model_structure(md->modelStructure);
    md->modelStructure = 0;
}

const char* fmi3_xml_get_last_error(fmi3_xml_model_description_t* md) {
    return jm_get_last_error(md->callbacks);
}

void fmi3_xml_clear_last_error(fmi3_xml_model_description_t* md) {
    jm_clear_last_error(md->callbacks);
}

void fmi3_xml_free_model_description(fmi3_xml_model_description_t* md) {
    jm_callbacks* cb;
    if(!md) return;
    cb = md->callbacks;
    fmi3_xml_clear_model_description(md);
    cb->free(md);
}

const char* fmi3_xml_get_model_name(fmi3_xml_model_description_t* md) {
    return jm_vector_char2string(&md->modelName);
}

const char* fmi3_xml_get_model_identifier_ME(fmi3_xml_model_description_t* md){
    return jm_vector_char2string(&md->modelExchange.modelIdentifier);
}

const char* fmi3_xml_get_model_identifier_CS(fmi3_xml_model_description_t* md){
    return jm_vector_char2string(&md->coSimulation.modelIdentifier);
}

const char* fmi3_xml_get_model_identifier_SE(fmi3_xml_model_description_t* md){
    return jm_vector_char2string(&md->scheduledExecution.modelIdentifier);
}

const char* fmi3_xml_get_instantiation_token(fmi3_xml_model_description_t* md){
    return jm_vector_char2string(&md->instantiationToken);
}

const char* fmi3_xml_get_description(fmi3_xml_model_description_t* md){
    return jm_vector_char2string(&md->description);
}

const char* fmi3_xml_get_author(fmi3_xml_model_description_t* md){
    return jm_vector_char2string(&md->author);
}

const char* fmi3_xml_get_license(fmi3_xml_model_description_t* md){
    return jm_vector_char2string(&md->license);
}

const char* fmi3_xml_get_copyright(fmi3_xml_model_description_t* md){
    return jm_vector_char2string(&md->copyright);
}

const char* fmi3_xml_get_model_standard_version(fmi3_xml_model_description_t* md){
    return jm_vector_char2string(&md->fmi3_xml_standard_version);
}

const char* fmi3_xml_get_model_version(fmi3_xml_model_description_t* md){
    return jm_vector_char2string(&md->version);
}

const char* fmi3_xml_get_generation_tool(fmi3_xml_model_description_t* md){
    return jm_vector_char2string(&md->generationTool);
}

const char* fmi3_xml_get_generation_date_and_time(fmi3_xml_model_description_t* md){
    return jm_vector_char2string(&md->generationDateAndTime);
}

fmi3_variable_naming_convension_enu_t fmi3_xml_get_naming_convention(fmi3_xml_model_description_t* md) {
    return md->namingConvension;
}


size_t fmi3_xml_get_number_of_continuous_states(fmi3_xml_model_description_t* md) {
    return jm_vector_get_size(jm_voidp)(&md->modelStructure->continuousStateDerivatives);
}

size_t fmi3_xml_get_number_of_event_indicators(fmi3_xml_model_description_t* md) {
    return jm_vector_get_size(jm_voidp)(&md->modelStructure->eventIndicators);
}

int fmi3_xml_get_default_experiment_has_start(fmi3_xml_model_description_t* md) {
    return md->defaultExperiment.startTimeDefined;
}

int fmi3_xml_get_default_experiment_has_stop(fmi3_xml_model_description_t* md) {
    return md->defaultExperiment.stopTimeDefined;
}

int fmi3_xml_get_default_experiment_has_tolerance(fmi3_xml_model_description_t* md) {
    return md->defaultExperiment.toleranceDefined;
}

int fmi3_xml_get_default_experiment_has_step_size(fmi3_xml_model_description_t* md) {
    return md->defaultExperiment.stepSizeDefined;
}

#define LOG_WARN_IF_ATTR_NOT_DEFINED(ATTRIBUTE) \
    if (!fmi3_xml_get_default_experiment_has_##ATTRIBUTE (md)) { \
        jm_log(md->callbacks, module, jm_log_level_warning, "fmi3_xml_get_default_experiment_" #ATTRIBUTE ": returning default value, since no attribute was defined in modelDescription"); \
    }

double fmi3_xml_get_default_experiment_start(fmi3_xml_model_description_t* md) {
    LOG_WARN_IF_ATTR_NOT_DEFINED(start);

    return md->defaultExperiment.startTime;
}

double fmi3_xml_get_default_experiment_stop(fmi3_xml_model_description_t* md) {
    LOG_WARN_IF_ATTR_NOT_DEFINED(stop);

    return md->defaultExperiment.stopTime;
}

double fmi3_xml_get_default_experiment_tolerance(fmi3_xml_model_description_t* md) {
    LOG_WARN_IF_ATTR_NOT_DEFINED(tolerance);

    return md->defaultExperiment.tolerance;
}

double fmi3_xml_get_default_experiment_step_size(fmi3_xml_model_description_t* md) {
    LOG_WARN_IF_ATTR_NOT_DEFINED(step_size);

    return md->defaultExperiment.stepSize;
}
#undef LOG_WARN_IF_ATTR_NOT_DEFINED

fmi3_fmu_kind_enu_t fmi3_xml_get_fmu_kind(fmi3_xml_model_description_t* md) {
    return md->fmuKind;
}

int fmi3_xml_get_cs_has_fixed_internal_step_size(fmi3_xml_model_description_t* md) {
    return md->coSimulation.hasFixedInternalStepSize;
}

double fmi3_xml_get_cs_fixed_internal_step_size(fmi3_xml_model_description_t* md) {
    return md->coSimulation.fixedInternalStepSize;
}

int fmi3_xml_get_cs_recommended_intermediate_input_smoothness(fmi3_xml_model_description_t* md) {
    return md->coSimulation.recommendedIntermediateInputSmoothness;
}

void fmi3_xml_set_model_description_invalid(fmi3_xml_model_description_t* md){
    md->isValid = 0;
}

unsigned int* fmi3_xml_get_capabilities(fmi3_xml_model_description_t* md) {
    return md->capabilities;
}

unsigned int fmi3_xml_get_capability(fmi3_xml_model_description_t* md, fmi3_capabilities_enu_t id) {
    assert((unsigned)id < (unsigned)fmi3_capabilities_num);
    return md->capabilities[id];
}

fmi3_xml_unit_definition_list_t* fmi3_xml_get_unit_definition_list(fmi3_xml_model_description_t* md) {
    return (fmi3_xml_unit_definition_list_t*)(&md->unitDefinitions);
}

size_t fmi3_xml_get_unit_definition_list_size(fmi3_xml_unit_definition_list_t* ud) {
    if(!ud) {
        assert(ud && "Unit definitions cannot be NULL");
        return 0;
    }
    return (unsigned int)jm_vector_get_size(jm_named_ptr)(&ud->definitions);
}

fmi3_xml_type_definition_list_t* fmi3_xml_get_type_definitions(fmi3_xml_model_description_t* md) {
    assert(md);
    return &md->typeDefinitions;
}

/** \brief Get the number of vendors that had annotations in the XML*/
size_t fmi3_xml_get_vendors_num(fmi3_xml_model_description_t* md) {
    assert(md);
    return jm_vector_get_size(jm_string)(&md->vendorList);
}

jm_vector(jm_string)* fmi3_xml_get_log_categories(fmi3_xml_model_description_t* md) {
    assert(md);
    return &md->logCategories;
}

jm_vector(jm_string)* fmi3_xml_get_log_category_descriptions(fmi3_xml_model_description_t* md) {
    assert(md);
    return &md->logCategoryDescriptions;
}

jm_vector(jm_string)* fmi3_xml_get_source_files_me(fmi3_xml_model_description_t* md) {
    assert(md);
    return &md->sourceFilesME;
}

jm_vector(jm_string)* fmi3_xml_get_source_files_cs(fmi3_xml_model_description_t* md) {
    assert(md);
    return &md->sourceFilesCS;
}

fmi3_xml_model_structure_t* fmi3_xml_get_model_structure(fmi3_xml_model_description_t* md) {
    assert(md);
    return md->modelStructure;
}


/** \brief Get the name of the vendor with that had annotations in the XML by index */
const char* fmi3_xml_get_vendor_name(fmi3_xml_model_description_t* md, size_t  index) {
    assert(fmi3_xml_get_vendors_num(md) > index);

    return jm_vector_get_item(jm_string)(&md->vendorList,index);
}

int fmi3_xml_is_valid_model_ID_char(char ch) {
    return ((( ch >= 'A') && (ch <= 'Z')) ||
            (( ch >= 'a') && (ch <= 'z')) ||
            (ch == '_'));
}

int fmi3_xml_is_valid_model_ID(const char* str) {
    size_t i, len = strlen(str);
    char ch = str[0];
    if( !fmi3_xml_is_valid_model_ID_char(ch)) return 0;
    for(i = 1; i < len; i++) {
        ch = str[i];
        if( !fmi3_xml_is_valid_model_ID_char(ch) && (ch < '0') && (ch > '9')) return 0;
    }
    return 1;
}

int fmi3_xml_handle_fmiModelDescription(fmi3_xml_parser_context_t* context, const char* data) {
    jm_name_ID_map_t namingConventionMap[] = {{"flat", fmi3_naming_enu_flat}, {"structured", fmi3_naming_enu_structured}, {NULL, 0}};
    fmi3_xml_model_description_t* md = context->modelDescription;
    if (!data) {
        int ret;
        if (context->currentElmID.modelDescription != fmi3_xml_elmID_none) {
            fmi3_xml_parse_fatal(context, "fmi3_xml_model_description must be the root XML element");
            return -1;
        }
        jm_log_verbose(context->callbacks, module, "Parsing XML element fmiModelDescription");
        md->fmuKind = fmi3_fmu_kind_unknown;
        /* process the attributes */
        int attrExists = 0; /* unused for now */
        ret =   fmi3_xml_parse_attr_as_string(context, FMI3_ELM(fmi3_xml_elmID_fmiModelDescription), FMI3_ATTR(fmi_attr_id_fmiVersion), 1, &attrExists, &(md->fmi3_xml_standard_version)) ||
                fmi3_xml_parse_attr_as_string(context, FMI3_ELM(fmi3_xml_elmID_fmiModelDescription), FMI3_ATTR(fmi_attr_id_modelName), 1, &attrExists, &(md->modelName)) ||
                fmi3_xml_parse_attr_as_string(context, FMI3_ELM(fmi3_xml_elmID_fmiModelDescription), FMI3_ATTR(fmi_attr_id_instantiationToken), 1, &attrExists, &(md->instantiationToken)) ||
                fmi3_xml_parse_attr_as_string(context, FMI3_ELM(fmi3_xml_elmID_fmiModelDescription), FMI3_ATTR(fmi_attr_id_description), 0, &attrExists, &(md->description)) ||
                fmi3_xml_parse_attr_as_string(context, FMI3_ELM(fmi3_xml_elmID_fmiModelDescription), FMI3_ATTR(fmi_attr_id_author), 0, &attrExists, &(md->author)) ||
                fmi3_xml_parse_attr_as_string(context, FMI3_ELM(fmi3_xml_elmID_fmiModelDescription), FMI3_ATTR(fmi_attr_id_version), 0, &attrExists, &(md->version)) ||
                fmi3_xml_parse_attr_as_string(context, FMI3_ELM(fmi3_xml_elmID_fmiModelDescription), FMI3_ATTR(fmi_attr_id_copyright), 0, &attrExists, &(md->copyright)) ||
                fmi3_xml_parse_attr_as_string(context, FMI3_ELM(fmi3_xml_elmID_fmiModelDescription), FMI3_ATTR(fmi_attr_id_license), 0, &attrExists, &(md->license)) ||
                fmi3_xml_parse_attr_as_string(context, FMI3_ELM(fmi3_xml_elmID_fmiModelDescription), FMI3_ATTR(fmi_attr_id_generationTool), 0, &attrExists, &(md->generationTool)) ||
                fmi3_xml_parse_attr_as_string(context, FMI3_ELM(fmi3_xml_elmID_fmiModelDescription), FMI3_ATTR(fmi_attr_id_generationDateAndTime), 0, &attrExists, &(md->generationDateAndTime)) ||
                fmi3_xml_parse_attr_as_enum  (context, FMI3_ELM(fmi3_xml_elmID_fmiModelDescription), FMI3_ATTR(fmi_attr_id_variableNamingConvention), 0, (unsigned*)&(md->namingConvension),
                                       fmi3_naming_enu_flat, namingConventionMap);
        return ret;
    }
    else {
        /* check that fmuKind is defined and that model identifiers are valid */
        if (md->fmuKind == fmi3_fmu_kind_unknown) {
            fmi3_xml_parse_fatal(
                context,
                "Neither ModelExchange, CoSimulation nor ScheduledExecution element were parsed correctly. FMU kind not known.");
            return -1;
        }
        if ((md->fmuKind & fmi3_fmu_kind_me) && !fmi3_xml_is_valid_model_ID(fmi3_xml_get_model_identifier_ME(md))) {
            fmi3_xml_parse_error(context,
                    "ModelExchange: Model identifier '%s' is not valid (must be a valid C-identifier)",
                    fmi3_xml_get_model_identifier_ME(md));
            return -1;
        }
        if ((md->fmuKind & fmi3_fmu_kind_cs) && !fmi3_xml_is_valid_model_ID(fmi3_xml_get_model_identifier_CS(md))) {
            fmi3_xml_parse_error(context,
                    "CoSimulation: Model identifier '%s' is not valid (must be a valid C-identifier)",
                    fmi3_xml_get_model_identifier_CS(md));
            return -1;
        }
        if ((md->fmuKind & fmi3_fmu_kind_se) && !fmi3_xml_is_valid_model_ID(fmi3_xml_get_model_identifier_SE(md))) {
            fmi3_xml_parse_error(context,
                    "ScheduledExecution: Model identifier '%s' is not valid (must be a valid C-identifier)",
                    fmi3_xml_get_model_identifier_SE(md));
            return -1;
        }
        if (!md->modelStructure) {
            fmi3_xml_parse_fatal(context, "No model structure information available. Cannot continue.");
            return -1;
        }
        return 0;
    }
}

// Helper for expanding lists, when only number of elements in the list matter.
#define FMI3_XML_REPLACE_WITH_CH(S) '_',

static int fmi3_xml_process_interface_type_common_attrs(fmi3_xml_parser_context_t *context,
        fmi3_fmu_kind_enu_t fmuKind)
{
    fmi3_xml_model_description_t* md = context->modelDescription;

    fmi3_capabilities_enu_t providesDirDerId    = fmi_attr_id_providesDirectionalDerivatives;
    fmi3_capabilities_enu_t providesDirDerIdOld = fmi_attr_id_providesDirectionalDerivative;
    if (fmi3_xml_is_attr_defined(context, FMI3_ATTR(providesDirDerIdOld))) {
        fmi3_xml_parse_error(context,
                "Attribute 'providesDirectionalDerivative' has been renamed to 'providesDirectionalDerivatives'.");
        providesDirDerId = providesDirDerIdOld;
    }

    // The offset from the corresponding ME capability. It's calculation makes use of that the
    // common capabilities for every FMU kind have the same ordering in the enum.
    size_t offset;

    // Helpers for finding num capabilities for each FMU-kind. (Just expands to one dummy char per capability.)
    char capArrME[] = { FMI3_ME_CAPABILITIES(FMI3_XML_REPLACE_WITH_CH) };
    char capArrCS[] = { FMI3_CS_CAPABILITIES(FMI3_XML_REPLACE_WITH_CH) };
    size_t nCapME = sizeof(capArrME);
    size_t nCapCS = sizeof(capArrCS);

    fmi3_xml_elm_t elmID;
    jm_vector(char)* modelIdentifierPtr;
    switch (fmuKind) {
        case fmi3_fmu_kind_me:
            elmID = FMI3_ELM(fmi3_xml_elmID_ModelExchange);
            modelIdentifierPtr = &md->modelExchange.modelIdentifier;
            offset = 0;
            break;
        case fmi3_fmu_kind_cs:
            elmID = FMI3_ELM(fmi3_xml_elmID_CoSimulation);
            modelIdentifierPtr = &md->coSimulation.modelIdentifier;
            offset = nCapME;  // CS comes after ME
            break;
        case fmi3_fmu_kind_se:
            elmID = FMI3_ELM(fmi3_xml_elmID_ScheduledExecution);
            modelIdentifierPtr = &md->scheduledExecution.modelIdentifier;
            offset = nCapME + nCapCS;  // SE comes after CS
            break;
        default:
            fmi3_xml_parse_fatal(context, "Invalid FMU kind");
            return -1;
    }

    int modelIdentifierExists = 0;
    if (fmi3_xml_parse_attr_as_string(context, elmID, FMI3_ATTR(fmi_attr_id_modelIdentifier), 1, &modelIdentifierExists, modelIdentifierPtr)
        ||
        fmi3_xml_parse_attr_as_boolean(context, elmID, FMI3_ATTR(fmi_attr_id_needsExecutionTool), 0,
                &md->capabilities[fmi3_me_needsExecutionTool + offset], 0)
        ||
        fmi3_xml_parse_attr_as_boolean(context, elmID, FMI3_ATTR(fmi_attr_id_canBeInstantiatedOnlyOncePerProcess), 0,
                &md->capabilities[fmi3_me_canBeInstantiatedOnlyOncePerProcess + offset], 0)
        ||
        fmi3_xml_parse_attr_as_boolean(context, elmID, FMI3_ATTR(fmi_attr_id_canGetAndSetFMUState), 0,
                &md->capabilities[fmi3_me_canGetAndSetFMUState + offset], 0)
        ||
        fmi3_xml_parse_attr_as_boolean(context, elmID, FMI3_ATTR(fmi_attr_id_canSerializeFMUState), 0,
                &md->capabilities[fmi3_me_canSerializeFMUState + offset], 0)
        ||
        fmi3_xml_parse_attr_as_boolean(context, elmID, FMI3_ATTR(providesDirDerId), 0,
                &md->capabilities[fmi3_me_providesDirectionalDerivatives + offset], 0)
        ||
        fmi3_xml_parse_attr_as_boolean(context, elmID, FMI3_ATTR(fmi_attr_id_providesAdjointDerivatives), 0,
                &md->capabilities[fmi3_me_providesAdjointDerivatives + offset], 0)
        ||
        fmi3_xml_parse_attr_as_boolean(context, elmID, FMI3_ATTR(fmi_attr_id_providesPerElementDependencies), 0,
                &md->capabilities[fmi3_me_providesPerElementDependencies + offset], 0))
    {
        return -1;
    }
    return 0;
}

int fmi3_xml_handle_ModelExchange(fmi3_xml_parser_context_t *context, const char* data) {
    fmi3_xml_model_description_t* md = context->modelDescription;
    if (!data) {
        jm_log_verbose(context->callbacks, module, "Parsing XML element ModelExchange");

        // Reset handles for the elements that are specific under ModelExchange
        fmi3_xml_set_element_handle(context, "SourceFiles", FMI3_ELM(FMI3_XML_ELM_ID(SourceFiles)));
        fmi3_xml_set_element_handle(context, "File", FMI3_ELM(FMI3_XML_ELM_ID(File)));

        // Update fmuKind
        if (md->fmuKind == fmi3_fmu_kind_unknown) {
            md->fmuKind = fmi3_fmu_kind_me;
        } else {
            // Handle multi-kind FMUs
            md->fmuKind |= fmi3_fmu_kind_me;
        }

        // Process attributes
        if (fmi3_xml_process_interface_type_common_attrs(context, fmi3_fmu_kind_me)) {
            return -1;
        }

        fmi3_xml_elm_t elmID = FMI3_ELM(fmi3_xml_elmID_ModelExchange);
        if (fmi3_xml_parse_attr_as_boolean(context, elmID, FMI3_ATTR(fmi_attr_id_needsCompletedIntegratorStep), 0,
                    &md->capabilities[fmi3_me_needsCompletedIntegratorStep], 0)
            ||
            fmi3_xml_parse_attr_as_boolean(context, elmID, FMI3_ATTR(fmi_attr_id_providesEvaluateDiscreteStates), 0,
                    &md->capabilities[fmi3_me_providesEvaluateDiscreteStates], 0))
        {
            return -1;
        }
    }
    return 0;
}

int fmi3_xml_handle_CoSimulation(fmi3_xml_parser_context_t *context, const char* data) {
    fmi3_xml_model_description_t* md = context->modelDescription;
    if(!data) {
        jm_log_verbose(context->callbacks, module, "Parsing XML element CoSimulation");

        //  Reset handles for the elements that are specific under CoSimulation
        fmi3_xml_set_element_handle(context, "SourceFiles", FMI3_ELM(FMI3_XML_ELM_ID(SourceFilesCS)));
        fmi3_xml_set_element_handle(context, "File", FMI3_ELM(FMI3_XML_ELM_ID(FileCS)));

        // Update fmuKind
        if (md->fmuKind == fmi3_fmu_kind_unknown) {
            md->fmuKind = fmi3_fmu_kind_cs;
        } else {
            // Add CS to found kinds (i.e. for multi-types)
            md->fmuKind |= fmi3_fmu_kind_cs;
        }

        // Process attributes
        if (fmi3_xml_process_interface_type_common_attrs(context, fmi3_fmu_kind_cs)) {
            return -1;
        }

        fmi3_xml_elm_t elmID = FMI3_ELM(fmi3_xml_elmID_CoSimulation);
        if (fmi3_xml_parse_attr_as_boolean(context, elmID, FMI3_ATTR(fmi_attr_id_canHandleVariableCommunicationStepSize), 0,
                    &md->capabilities[fmi3_cs_canHandleVariableCommunicationStepSize], 0)
            ||
            fmi3_xml_parse_attr_as_boolean(context, elmID, FMI3_ATTR(fmi_attr_id_providesIntermediateUpdate), 0,
                    &md->capabilities[fmi3_cs_providesIntermediateUpdate], 0)
            ||
            fmi3_xml_parse_attr_as_boolean(context, elmID, FMI3_ATTR(fmi_attr_id_mightReturnEarlyFromDoStep), 0,
                    &md->capabilities[fmi3_cs_mightReturnEarlyFromDoStep], 0)
            ||
            fmi3_xml_parse_attr_as_boolean(context, elmID, FMI3_ATTR(fmi_attr_id_canReturnEarlyAfterIntermediateUpdate), 0,
                    &md->capabilities[fmi3_cs_canReturnEarlyAfterIntermediateUpdate], 0)
            ||
            fmi3_xml_parse_attr_as_boolean(context, elmID, FMI3_ATTR(fmi_attr_id_hasEventMode), 0,
                    &md->capabilities[fmi3_cs_hasEventMode], 0)
            ||
            fmi3_xml_parse_attr_as_boolean(context, elmID, FMI3_ATTR(fmi_attr_id_providesEvaluateDiscreteStates), 0,
                    &md->capabilities[fmi3_cs_providesEvaluateDiscreteStates], 0))
        {
            return -1;
        }


        // NOTE: This attribute is unsigned int, so not really a capability. However, in
        // FMI2 it's part of the capabilities enum, so keeping it the same for FMI3.
        if (fmi3_xml_parse_attr_as_uint32(context, elmID, FMI3_ATTR(fmi_attr_id_maxOutputDerivativeOrder), 0,
                &md->capabilities[fmi3_cs_maxOutputDerivativeOrder], 0))
        {
            return -1;
        }

        // Other non-capability attributes:
        if (fmi3_xml_is_attr_defined(context, FMI3_ATTR(fmi_attr_id_fixedInternalStepSize))) {
            md->coSimulation.hasFixedInternalStepSize = 1;
            if (fmi3_xml_parse_attr_as_float64(context, elmID, FMI3_ATTR(fmi_attr_id_fixedInternalStepSize), 0,
                    &md->coSimulation.fixedInternalStepSize, 0))
            {
                return -1;
            }
        }
        if (fmi3_xml_is_attr_defined(context, FMI3_ATTR(fmi_attr_id_recommendedIntermediateInputSmoothness))) {
            md->coSimulation.hasRecommendedIntermediateInputSmoothness = 1;
            if (fmi3_xml_parse_attr_as_int32(context, elmID, FMI3_ATTR(fmi_attr_id_recommendedIntermediateInputSmoothness), 0,
                    &md->coSimulation.recommendedIntermediateInputSmoothness, 0))
            {
                return -1;
            }
        }
    }
    return 0;
}


int fmi3_xml_handle_ScheduledExecution(fmi3_xml_parser_context_t *context, const char* data) {
    fmi3_xml_model_description_t* md = context->modelDescription;
    if(!data) {
        jm_log_verbose(context->callbacks, module, "Parsing XML element ScheduledExecution");

        // Reset handles for the elements that are specific under ScheduledExecution TODO: Remove:
        /*  fmi3_xml_set_element_handle(context, "SourceFiles", FMI3_XML_ELM_ID(SourceFilesSE)); */
        /*  fmi3_xml_set_element_handle(context, "File", FMI3_XML_ELM_ID(FileSE)); */

        // Update fmuKind
        if (md->fmuKind == fmi3_fmu_kind_unknown) {
            md->fmuKind = fmi3_fmu_kind_se;
        } else {
            // Handle multi-kind FMU
            md->fmuKind |= fmi3_fmu_kind_se;
        }

        // Process attributes
        if (fmi3_xml_process_interface_type_common_attrs(context, fmi3_fmu_kind_se)) {
            return -1;
        }
    }
    return 0;
}

int fmi3_xml_handle_SourceFiles(fmi3_xml_parser_context_t *context, const char* data) {
    return 0;
}

static int push_back_jm_string(fmi3_xml_parser_context_t *context, jm_vector(jm_string) *stringvector, jm_vector(char)* buf) {
    size_t len;
    char* string = 0;
    jm_string *pstring;

    pstring = jm_vector_push_back(jm_string)(stringvector, string);
    len = jm_vector_get_size(char)(buf);
    if(pstring )
        *pstring = string = (char*)(context->callbacks->malloc(len + 1));
    if(!pstring || !string) {
        fmi3_xml_parse_fatal(context, "Could not allocate memory");
        return -1;
    }
    if (len != 0) { /* No need to memcpy empty strings (gives assetion error) */
        memcpy(string, jm_vector_get_itemp(char)(buf,0), len);
    }
    string[len] = 0;
    return 0;
}


int fmi3_xml_handle_File(fmi3_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        jm_vector(char)* bufName = fmi3_xml_reserve_parse_buffer(context,1,100);

        if (!bufName) { return -1;}
        /* <xs:attribute name="name" type="xs:normalizedString" use="required"> */
        int nameExists = 0;
        if (fmi3_xml_parse_attr_as_string(context, FMI3_ELM(fmi3_xml_elmID_File), FMI3_ATTR(fmi_attr_id_name), 1, &nameExists, bufName)) {
            return -1;
        }
        return push_back_jm_string(context, &md->sourceFilesME, bufName);
    } else {
        return 0;
    }
}

int fmi3_xml_handle_SourceFilesCS(fmi3_xml_parser_context_t *context, const char* data) {
    return 0;
}

int fmi3_xml_handle_FileCS(fmi3_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        jm_vector(char)* bufName = fmi3_xml_reserve_parse_buffer(context,1,100);

        if (!bufName) { return -1;}
        /* <xs:attribute name="name" type="xs:normalizedString" use="required"> */
        int nameExists = 0;
        if (fmi3_xml_parse_attr_as_string(context, FMI3_ELM(fmi3_xml_elmID_File), FMI3_ATTR(fmi_attr_id_name), 1, &nameExists, bufName)) {
            return -1;
        }
        return push_back_jm_string(context, &md->sourceFilesCS, bufName);
    } else {
        return 0;
    }
}

int fmi3_xml_handle_LogCategories(fmi3_xml_parser_context_t *context, const char* data) {
/*    fmi3_xml_model_description_t* md = context->modelDescription; */
    if(!data) {
        jm_log_verbose(context->callbacks, module, "Parsing XML element LogCategories");
        /* process the attributes */
        return (0);
    } else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
}

int fmi3_xml_handle_Category(fmi3_xml_parser_context_t *context, const char* data) {
    if(!data) {
        /* process the attributes */
        fmi3_xml_model_description_t* md = context->modelDescription;
        jm_vector(char)* bufName = fmi3_xml_reserve_parse_buffer(context,1,100);

        if (!bufName) return -1;
        /* <xs:attribute name="name" type="xs:normalizedString"> */
        int nameExists = 0;
        if (fmi3_xml_parse_attr_as_string(context, FMI3_ELM(fmi3_xml_elmID_Category), FMI3_ATTR(fmi_attr_id_name), 1, &nameExists, bufName)) {
            return -1;
        }
        if (push_back_jm_string(context, &md->logCategories, bufName) < 0) { return -1;}

        int descriptionExists = 0;
        if (fmi3_xml_parse_attr_as_string(context, FMI3_ELM(fmi3_xml_elmID_Category), FMI3_ATTR(fmi_attr_id_description), 0, &descriptionExists, bufName) < 0) { return -1;}
        if (push_back_jm_string(context, &md->logCategoryDescriptions, bufName) < 0) { return -1;}
        return 0;
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
}

int fmi3_xml_handle_DefaultExperiment(fmi3_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;

        /* process the attributes */

        /* save if attributes are defined */
        md->defaultExperiment.startTimeDefined  = fmi3_xml_is_attr_defined(context, FMI3_ATTR(fmi_attr_id_startTime));
        md->defaultExperiment.stopTimeDefined   = fmi3_xml_is_attr_defined(context, FMI3_ATTR(fmi_attr_id_stopTime));
        md->defaultExperiment.toleranceDefined  = fmi3_xml_is_attr_defined(context, FMI3_ATTR(fmi_attr_id_tolerance));
        md->defaultExperiment.stepSizeDefined   = fmi3_xml_is_attr_defined(context, FMI3_ATTR(fmi_attr_id_stepSize));

        /* save attribute values: from XML if exists else defaults */
        /* NOTE: the default values are hard-coded in the API documentation as well */
        return (
                fmi3_xml_parse_attr_as_float64(context, FMI3_ELM(fmi3_xml_elmID_DefaultExperiment), FMI3_ATTR(fmi_attr_id_startTime), 0, &md->defaultExperiment.startTime, 0)                                 ||
                fmi3_xml_parse_attr_as_float64(context, FMI3_ELM(fmi3_xml_elmID_DefaultExperiment), FMI3_ATTR(fmi_attr_id_stopTime),  0, &md->defaultExperiment.stopTime,  1)                                 ||
                fmi3_xml_parse_attr_as_float64(context, FMI3_ELM(fmi3_xml_elmID_DefaultExperiment), FMI3_ATTR(fmi_attr_id_tolerance), 0, &md->defaultExperiment.tolerance, FMI3_DEFAULT_EXPERIMENT_TOLERANCE) ||
                fmi3_xml_parse_attr_as_float64(context, FMI3_ELM(fmi3_xml_elmID_DefaultExperiment), FMI3_ATTR(fmi_attr_id_stepSize),  0, &md->defaultExperiment.stepSize,  FMI3_DEFAULT_EXPERIMENT_STEPSIZE)
        );
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

jm_vector(jm_voidp)* fmi3_xml_get_variables_original_order(fmi3_xml_model_description_t* md) {
    return &md->variablesOrigOrder;
}

jm_vector(jm_named_ptr)* fmi3_xml_get_variables_alphabetical_order(fmi3_xml_model_description_t* md){
    return &md->variablesByName;
}

jm_vector(jm_voidp)* fmi3_xml_get_variables_vr_order(fmi3_xml_model_description_t* md) {
    return &md->variablesByVR;
}

fmi3_xml_variable_t* fmi3_xml_get_variable_by_name(fmi3_xml_model_description_t* md, const char* name) {
    if (!name) return NULL;
    jm_named_ptr key, *found;
    key.name = name;
    found = jm_vector_bsearch(jm_named_ptr)(&md->variablesByName, &key, jm_compare_named);
    if (!found) return NULL;
    return found->ptr;
}

fmi3_xml_variable_t* fmi3_xml_get_variable_by_vr(fmi3_xml_model_description_t* md, fmi3_value_reference_t vr) {
    fmi3_xml_variable_t key;
    fmi3_xml_variable_t *pkey = &key;
    void** found;

    key.vr = vr;
    found = jm_vector_bsearch(jm_voidp)(&md->variablesByVR, (void**)&pkey, fmi3_xml_compare_vr);
    if (!found) return NULL;
    return (fmi3_xml_variable_t*)(*found);
}

const char* fmi3_xml_get_variable_description_by_name(fmi3_xml_model_description_t* md, const char* name) {
    fmi3_xml_variable_t* v = fmi3_xml_get_variable_by_name(md, name);
    if (v == NULL) {
        return NULL;
    }
    if (strcmp(name, fmi3_xml_get_variable_name(v)) == 0) {
        return fmi3_xml_get_variable_description(v);
    }
    fmi3_xml_alias_variable_t* alias_var = fmi3_xml_get_alias_variable_by_name(v, name);
    return fmi3_xml_get_alias_variable_description(alias_var);
}

fmi3_xml_display_unit_t* fmi3_xml_get_variable_display_unit_by_name(fmi3_xml_model_description_t* md, const char* name) {
    fmi3_xml_variable_t* v = fmi3_xml_get_variable_by_name(md, name);
    if (v == NULL) {
        return NULL;
    }
    if (strcmp(name, fmi3_xml_get_variable_name(v)) == 0) { /* no alias */
        fmi3_xml_float32_variable_t* v32 = fmi3_xml_get_variable_as_float32(v);
        if (v32) {
            return fmi3_xml_get_float32_variable_display_unit(v32);
        }
        fmi3_xml_float64_variable_t* v64 = fmi3_xml_get_variable_as_float64(v);
        if (v64) {
            return fmi3_xml_get_float64_variable_display_unit(v64);
        }
        return NULL; /* non-float variable */
    } else { /* alias */
        fmi3_xml_alias_variable_t* alias_var = fmi3_xml_get_alias_variable_by_name(v, name);
        if (!alias_var) {
            return NULL; /* safety*/
        }
        return fmi3_xml_get_alias_variable_display_unit(alias_var);
    }
}
