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
#include <stdarg.h>

#include <JM/jm_named_ptr.h>
#include "FMI/fmi_util.h"
#include <FMI3/fmi3_types.h>
#include <FMI3/fmi3_function_types.h>
#include <FMI3/fmi3_enums.h>
#include <FMI3/fmi3_capi.h>

#include "fmi3_import_impl.h"
#include "fmi3_import_variable_list_impl.h"
#include "fmi3_import_priv.h"


static const char* module = "FMILIB";

char* fmi3_import_get_dll_path(const char* fmu_unzipped_path, const char* model_identifier, jm_callbacks* callbacks) {
    char* dllDir = fmi_construct_dll_dir_name(callbacks, fmu_unzipped_path, fmi_version_3_0_enu);
    if (!dllDir) {
        return NULL;
    }
    return fmi_construct_dll_file_name(callbacks, dllDir, model_identifier);
}

fmi3_import_t* fmi3_import_allocate(jm_callbacks* cb) {
    fmi3_import_t* fmu = (fmi3_import_t*)cb->calloc(1, sizeof(fmi3_import_t));

    if (!fmu || (jm_vector_init(char)(&fmu->logMessageBufferCoded,JM_MAX_ERROR_MESSAGE_SIZE,cb) < JM_MAX_ERROR_MESSAGE_SIZE)) {
        jm_log_fatal(cb, module, "Could not allocate memory");
        if (fmu) {
            cb->free(fmu);
        }
        return NULL;
    }
    fmu->dirPath = NULL;
    fmu->resourcePath = NULL;
    fmu->instanceName = NULL;
    fmu->callbacks = cb;
    fmu->capi = NULL;

    fmu->md = fmi3_xml_allocate_model_description(cb);
    if (!fmu->md) {
        cb->free(fmu);
        return NULL;
    }

    fmu->termIcon = fmi_xml_allocate_terminals_and_icons(cb);
    if (!fmu->termIcon) {
        fmi3_xml_free_model_description(fmu->md);
        cb->free(fmu);
        return NULL;
    }

    jm_vector_init(char)(&fmu->logMessageBufferExpanded, 0, cb);

    fmu->options = fmi_util_allocate_options(cb);
    if (!fmu->options) {
        fmi3_xml_free_model_description(fmu->md);
        fmi_xml_free_terminals_and_icons(fmu->termIcon);
        cb->free(fmu);
        return NULL;
     }

    return fmu;
}

const char* fmi3_import_get_last_error(fmi3_import_t* fmu) {
    return jm_get_last_error(fmu->callbacks);
}

fmi3_import_t* fmi3_import_parse_xml(
        fmi_import_context_t* context, const char* dirPath, fmi3_xml_callbacks_t* xml_callbacks) {
    char* xmlPath;
    char* terminalsAndIconsPath;
    size_t resourcePathLen;
    fmi3_import_t* fmu = NULL;

    if (strlen(dirPath) + 20 > FILENAME_MAX) {
        jm_log_fatal(context->callbacks, module, "Directory path for FMU is too long");
        return NULL;
    }

    xmlPath = fmi_import_get_model_description_path(dirPath, context->callbacks);
    terminalsAndIconsPath = fmi_import_get_terminals_and_icons_path(dirPath, context->callbacks);
    fmu = fmi3_import_allocate(context->callbacks);
    if (!fmu) {
        goto err1;
    }
    
    // Create the resourcePath
    fmu->resourcePath = context->callbacks->malloc(FILENAME_MAX + 2);
    if (!fmu->resourcePath) {
        jm_log_fatal(context->callbacks, "FMILIB", "Could not allocate memory");
        goto err2;
    }
    // Set fmu->resourcePath to abspath of dirPath
    if (!jm_get_dir_abspath(context->callbacks, dirPath, fmu->resourcePath, FILENAME_MAX + 2)) {
        goto err2;
    }
    // Append the resource dir. Compared to FMI 2, in FMI 3 the resourcePath:
    // - Is an absolute path (not a URI)
    // - Needs a trailing file separator
    resourcePathLen = strlen(fmu->resourcePath);
    if (resourcePathLen + 20 > FILENAME_MAX) {
        jm_log_fatal(context->callbacks, module, "Resource directory path for FMU is too long");
        goto err2;
    }
    strcpy(fmu->resourcePath + resourcePathLen, FMI_FILE_SEP "resources" FMI_FILE_SEP);

    // Create dirPath
    fmu->dirPath = context->callbacks->malloc(strlen(dirPath) + 1);
    if (!fmu->dirPath) {
        jm_log_fatal(context->callbacks, "FMILIB", "Could not allocate memory");
        goto err2;
    }
    strcpy(fmu->dirPath, dirPath);

    jm_log_verbose(context->callbacks, "FMILIB", "Parsing model description XML");
    if (fmi3_xml_parse_model_description(fmu->md, xmlPath, xml_callbacks)) {
        fmi3_import_free(fmu);
        fmu = NULL;
    }
    context->callbacks->free(xmlPath);

    // Only parse terminals and icons if parsing of model description did not fail
    if (fmu) {
        // terminalsAndIcons uses modelDescription for error checks
        if (fmi3_xml_terminals_and_icons_set_model_description(fmu->termIcon, fmu->md)) {
            fmi3_import_free(fmu);
            fmu = NULL;
        }
        // TODO: Callbacks
        if (fmi3_xml_parse_terminals_and_icons(fmu->termIcon, terminalsAndIconsPath, NULL)) {
            // failure to parse terminalsAndIcons does not constitute parsing failure
            fmi_xml_free_terminals_and_icons(fmu->termIcon);
            fmu->termIcon = NULL;
        }
    }
    context->callbacks->free(terminalsAndIconsPath);

    if (fmu) {
        jm_log_verbose(context->callbacks, "FMILIB", "Parsing finished successfully");
    }

    return fmu;
    
    // Error handling
err2:
    fmi3_import_free(fmu);
    fmu = NULL;
err1:
    context->callbacks->free(xmlPath);
    if (terminalsAndIconsPath) {
        context->callbacks->free(terminalsAndIconsPath);
    }
    return NULL;
        
}

void fmi3_import_free(fmi3_import_t* fmu) {
    jm_callbacks* cb;

    if (!fmu) return;
    cb = fmu->callbacks;
    jm_log_verbose(fmu->callbacks, "FMILIB", "Releasing allocated library resources");

    fmi3_import_destroy_dllfmu(fmu);
    fmi3_xml_free_model_description(fmu->md);
    fmi_xml_free_terminals_and_icons(fmu->termIcon);
    fmi_util_free_options(cb, fmu->options);
    jm_vector_free_data(char)(&fmu->logMessageBufferCoded);
    jm_vector_free_data(char)(&fmu->logMessageBufferExpanded);

    cb->free(fmu->resourcePath);
    cb->free(fmu->dirPath);
    cb->free(fmu);
}

const char* fmi3_import_get_model_name(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;

    return fmi3_xml_get_model_name(fmu->md);
}

unsigned int fmi3_import_get_capability(fmi3_import_t* fmu , fmi3_capabilities_enu_t id) {
    if (!fmi3_import_check_has_FMU(fmu)) return 0;

    return fmi3_xml_get_capability(fmu->md, id);
}

const char* fmi3_import_get_model_identifier_ME(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;

    return fmi3_xml_get_model_identifier_ME(fmu->md);
}

const char* fmi3_import_get_model_identifier_CS(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;

    return fmi3_xml_get_model_identifier_CS(fmu->md);
}

const char* fmi3_import_get_model_identifier_SE(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;

    return fmi3_xml_get_model_identifier_SE(fmu->md);
}

const char* fmi3_import_get_instantiation_token(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;

    return fmi3_xml_get_instantiation_token(fmu->md);
}

const char* fmi3_import_get_description(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;

    return fmi3_xml_get_description(fmu->md);
}

const char* fmi3_import_get_author(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;

    return fmi3_xml_get_author(fmu->md);
}

const char* fmi3_import_get_license(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;

    return fmi3_xml_get_license(fmu->md);
}
const char* fmi3_import_get_copyright(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;

    return fmi3_xml_get_copyright(fmu->md);
}

const char* fmi3_import_get_model_standard_version(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;

    return fmi3_xml_get_model_standard_version(fmu->md);
}

const char* fmi3_import_get_model_version(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;

    return fmi3_xml_get_model_version(fmu->md);
}

const char* fmi3_import_get_generation_tool(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;

    return fmi3_xml_get_generation_tool(fmu->md);
}

const char* fmi3_import_get_generation_date_and_time(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;

    return fmi3_xml_get_generation_date_and_time(fmu->md);
}

fmi3_variable_naming_convension_enu_t fmi3_import_get_naming_convention(fmi3_import_t* fmu) {
    if (!fmu->md) {
        jm_log_error(fmu->callbacks, module,"No FMU is loaded");
        return fmi3_naming_enu_unknown;
    }
    return fmi3_xml_get_naming_convention(fmu->md);
}

int fmi3_import_get_default_experiment_has_start(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return 0;

    return fmi3_xml_get_default_experiment_has_start(fmu->md);
}

int fmi3_import_get_default_experiment_has_stop(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return 0;

    return fmi3_xml_get_default_experiment_has_stop(fmu->md);
}

int fmi3_import_get_default_experiment_has_tolerance(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return 0;

    return fmi3_xml_get_default_experiment_has_tolerance(fmu->md);
}

int fmi3_import_get_default_experiment_has_step_size(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return 0;

    return fmi3_xml_get_default_experiment_has_step_size(fmu->md);
}

double fmi3_import_get_default_experiment_start(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return 0;

    return fmi3_xml_get_default_experiment_start(fmu->md);
}

double fmi3_import_get_default_experiment_stop(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return 0;

    return fmi3_xml_get_default_experiment_stop(fmu->md);
}

double fmi3_import_get_default_experiment_tolerance(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return 0;

    return fmi3_xml_get_default_experiment_tolerance(fmu->md);
}

double fmi3_import_get_default_experiment_step_size(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return 0;

    return fmi3_xml_get_default_experiment_step_size(fmu->md);
}

fmi3_import_unit_definition_list_t* fmi3_import_get_unit_definition_list(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;

    return fmi3_xml_get_unit_definition_list(fmu->md);
}

size_t fmi3_import_get_unit_definition_list_size(fmi3_import_unit_definition_list_t* ud) {
    return fmi3_xml_get_unit_definition_list_size(ud);
}

fmi3_import_type_definition_list_t* fmi3_import_get_type_definition_list(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;

    return fmi3_xml_get_type_definitions(fmu->md);
}

fmi3_import_variable_list_t* fmi3_import_vector_to_varlist(fmi3_import_t* fmu, jm_vector(jm_voidp)* vars) {
    fmi3_import_variable_list_t* vl;
    size_t nv, i;
    if (!vars) return NULL;

    nv = jm_vector_get_size(jm_voidp)(vars);
    vl = fmi3_import_alloc_variable_list(fmu, nv);
    if (!vl) return NULL;
    for (i = 0; i< nv; i++) {
        jm_vector_set_item(jm_voidp)(&vl->variables, i, jm_vector_get_item(jm_voidp)(vars, i));
    }
    return vl;

}

/* Get the list of all the variables in the model */
/* 0 - original order as found in the XML file; 1 - sorted alfabetically by variable name; 2 sorted by value references. */
fmi3_import_variable_list_t* fmi3_import_get_variable_list(fmi3_import_t* fmu, int sortOrder) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;
    switch(sortOrder) {
    case 0:
        return fmi3_import_vector_to_varlist(fmu, fmi3_xml_get_variables_original_order(fmu->md));
    case 1: {
        fmi3_import_variable_list_t* vl;
        size_t nv, i;
        jm_vector(jm_named_ptr)* vars = fmi3_xml_get_variables_alphabetical_order(fmu->md);
        if (!vars) return NULL;
        nv = jm_vector_get_size(jm_named_ptr)(vars);
        vl = fmi3_import_alloc_variable_list(fmu, nv);
        if (!vl) return NULL;
        for (i = 0; i< nv; i++) {
            jm_vector_set_item(jm_voidp)(&vl->variables, i, jm_vector_get_item(jm_named_ptr)(vars, i).ptr);
        }
        return vl;
    }
    case 2:
        return fmi3_import_vector_to_varlist(fmu, fmi3_xml_get_variables_vr_order(fmu->md));
    default:
        assert(0);
    }
    return NULL;
}

fmi3_fmu_kind_enu_t fmi3_import_get_fmu_kind(fmi3_import_t* fmu) {
    return fmi3_xml_get_fmu_kind(fmu->md);
}

int fmi3_import_get_cs_has_fixed_internal_step_size(fmi3_import_t* fmu) {
    return fmi3_xml_get_cs_has_fixed_internal_step_size(fmu->md);
}

double fmi3_import_get_cs_fixed_internal_step_size(fmi3_import_t* fmu) {
    return fmi3_xml_get_cs_fixed_internal_step_size(fmu->md);
}

int fmi3_import_get_cs_recommended_intermediate_input_smoothness(fmi3_import_t* fmu) {
    return fmi3_xml_get_cs_recommended_intermediate_input_smoothness(fmu->md);
}

size_t fmi3_import_get_vendors_num(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return 0;

    return fmi3_xml_get_vendors_num(fmu->md);
}

const char* fmi3_import_get_vendor_name(fmi3_import_t* fmu, size_t  index) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;

    return fmi3_xml_get_vendor_name(fmu->md, index);
}

/** \brief Get the number of log categories defined in the XML */
size_t fmi3_import_get_log_categories_num(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return 0;

    return jm_vector_get_size(jm_string)(fmi3_xml_get_log_categories(fmu->md));
}

/** \brief Get the log category by index */
const char* fmi3_import_get_log_category(fmi3_import_t* fmu, size_t  index) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;

    return jm_vector_get_item(jm_string)(fmi3_xml_get_log_categories(fmu->md), index);
}

/** \brief Get the log category description by index */
const char* fmi3_import_get_log_category_description(fmi3_import_t* fmu, size_t index) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;

    return jm_vector_get_item(jm_string)(fmi3_xml_get_log_category_descriptions(fmu->md), index);
}

/** \brief Get the number of source files for ME defined in the XML */
size_t fmi3_import_get_source_files_me_num(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return 0;

    return jm_vector_get_size(jm_string)(fmi3_xml_get_source_files_me(fmu->md));
}

/** \brief Get the ME source file by index */
const char* fmi3_import_get_source_file_me(fmi3_import_t* fmu, size_t index) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;

    return jm_vector_get_item(jm_string)(fmi3_xml_get_source_files_me(fmu->md), index);
}

/** \brief Get the number of source files for CS defined in the XML */
size_t fmi3_import_get_source_files_cs_num(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return 0;

    return jm_vector_get_size(jm_string)(fmi3_xml_get_source_files_cs(fmu->md));
}

/** \brief Get the CS source file by index */
const char* fmi3_import_get_source_file_cs(fmi3_import_t* fmu, size_t index) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;

    return jm_vector_get_item(jm_string)(fmi3_xml_get_source_files_cs(fmu->md), index);
}


fmi3_import_variable_list_t* fmi3_import_get_outputs_list(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;
    return fmi3_import_vector_to_varlist(fmu, fmi3_xml_get_outputs(fmi3_xml_get_model_structure(fmu->md)));
}

fmi3_import_variable_list_t* fmi3_import_get_continuous_state_derivatives_list(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;
    return fmi3_import_vector_to_varlist(fmu, fmi3_xml_get_continuous_state_derivatives(fmi3_xml_get_model_structure(fmu->md)));
}

fmi3_import_variable_list_t* fmi3_import_get_clocked_states_list(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;
    return fmi3_import_vector_to_varlist(fmu, fmi3_xml_get_clocked_states(fmi3_xml_get_model_structure(fmu->md)));
}

fmi3_import_variable_list_t* fmi3_import_get_initial_unknowns_list(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;
    return fmi3_import_vector_to_varlist(fmu, fmi3_xml_get_initial_unknowns(fmi3_xml_get_model_structure(fmu->md)));
}

fmi3_import_variable_list_t* fmi3_import_get_event_indicators_list(fmi3_import_t* fmu) {
    if (!fmi3_import_check_has_FMU(fmu)) return NULL;
    return fmi3_import_vector_to_varlist(fmu, fmi3_xml_get_event_indicators(fmi3_xml_get_model_structure(fmu->md)));
}

int fmi3_import_get_output_dependencies(fmi3_import_t* fmu, fmi3_import_variable_t* variable,
        size_t* numDependencies, int* dependsOnAll, size_t** dependencies, char** dependenciesKind)
{
    fmi3_xml_model_structure_t* ms;
    if (!fmi3_import_check_has_FMU(fmu)) return -1;

    ms = fmi3_xml_get_model_structure(fmu->md);
    assert(ms);
    return fmi3_xml_get_output_dependencies(ms, variable,
        numDependencies, dependsOnAll, dependencies, dependenciesKind);
}

int fmi3_import_get_continuous_state_derivative_dependencies(fmi3_import_t* fmu, fmi3_import_variable_t* variable,
        size_t* numDependencies, int* dependsOnAll, size_t** dependencies, char** dependenciesKind)
{
    fmi3_xml_model_structure_t* ms;
    if (!fmi3_import_check_has_FMU(fmu)) return -1;

    ms = fmi3_xml_get_model_structure(fmu->md);
    assert(ms);
    return fmi3_xml_get_continuous_state_derivative_dependencies(ms, variable,
        numDependencies, dependsOnAll, dependencies, dependenciesKind);
}

int fmi3_import_get_clocked_state_dependencies(fmi3_import_t* fmu, fmi3_import_variable_t* variable,
        size_t* numDependencies, int* dependsOnAll, size_t** dependencies, char** dependenciesKind)
{
    fmi3_xml_model_structure_t* ms;
    if (!fmi3_import_check_has_FMU(fmu)) return -1;

    ms = fmi3_xml_get_model_structure(fmu->md);
    assert(ms);
    return fmi3_xml_get_clocked_state_dependencies(ms, variable,
        numDependencies, dependsOnAll, dependencies, dependenciesKind);
}

int fmi3_import_get_initial_unknown_dependencies(fmi3_import_t* fmu, fmi3_import_variable_t* variable,
        size_t* numDependencies, int* dependsOnAll, size_t** dependencies, char** dependenciesKind)
{
    fmi3_xml_model_structure_t* ms;
    if (!fmi3_import_check_has_FMU(fmu)) return -1;

    ms = fmi3_xml_get_model_structure(fmu->md);
    assert(ms);
    return fmi3_xml_get_initial_unknown_dependencies(ms, variable,
        numDependencies, dependsOnAll, dependencies, dependenciesKind);
}

int fmi3_import_get_event_indicator_dependencies(fmi3_import_t* fmu, fmi3_import_variable_t* variable,
        size_t* numDependencies, int* dependsOnAll, size_t** dependencies, char** dependenciesKind)
{
    fmi3_xml_model_structure_t* ms;
    if (!fmi3_import_check_has_FMU(fmu)) return -1;

    ms = fmi3_xml_get_model_structure(fmu->md);
    assert(ms);
    return fmi3_xml_get_event_indicator_dependencies(ms, variable,
        numDependencies, dependsOnAll, dependencies, dependenciesKind);
}

fmi_import_options_t* fmi3_import_get_options(fmi3_import_t* fmu) {
    if (fmu->options) {
        return fmu->options;
    } else {
        /* Options ownership has been moved to CAPI */
        return fmu->capi->options;
    }
}
