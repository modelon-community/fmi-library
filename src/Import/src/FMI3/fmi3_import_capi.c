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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <errno.h>
#include <FMI3/fmi3_types.h>
#include <FMI3/fmi3_function_types.h>
#include <FMI3/fmi3_enums.h>
#include <FMI3/fmi3_capi.h>
#include <FMI/fmi_util.h>
#include "fmi3_import_impl.h"
#include "fmi3_import_priv.h"

static const char * module = "FMILIB";

/* Load and destroy functions */
jm_status_enu_t fmi3_import_create_dllfmu(fmi3_import_t* fmu, fmi3_fmu_kind_enu_t fmuKind,
        const fmi3_instance_environment_t instanceEnvironment, const fmi3_log_message_callback_ft logMessage) {

    char curDir[FILENAME_MAX + 2];
    char* dllDirPath = 0;
    char* dllFileName = 0;
    const char* modelIdentifier;

    /* Callback functions and environment - either default or user specified */
    fmi3_instance_environment_t instanceEnvironmentFinal = instanceEnvironment;
    fmi3_log_message_callback_ft logMessageFinal = logMessage;

    if (fmu == NULL) {
        assert(0);
        return jm_status_error;
    }

    if(fmu->capi) {
        if(fmi3_capi_get_fmu_kind(fmu->capi) == fmuKind) {
            jm_log_warning(fmu->callbacks, module, "FMU binary is already loaded");
            return jm_status_success;
        }
        else
            fmi3_import_destroy_dllfmu(fmu);
    }

    if(fmuKind == fmi3_fmu_kind_me)
        modelIdentifier = fmi3_import_get_model_identifier_ME(fmu);
    else if(fmuKind == fmi3_fmu_kind_cs)
        modelIdentifier = fmi3_import_get_model_identifier_CS(fmu);
    else if(fmuKind == fmi3_fmu_kind_se)
        modelIdentifier = fmi3_import_get_model_identifier_SE(fmu);
    else {
        assert(0);
        return jm_status_error;
    }

    if (modelIdentifier == NULL) {
        jm_log_error(fmu->callbacks, module, "No model identifier given");
        return jm_status_error;
    }

    if( jm_portability_get_current_working_directory(curDir, FILENAME_MAX+1) != jm_status_success) {
        jm_log_warning(fmu->callbacks, module, "Could not get current working directory (%s)", strerror(errno));
        curDir[0] = 0;
    };

    dllDirPath = fmi_construct_dll_dir_name(fmu->callbacks, fmu->dirPath);
    dllFileName = fmi_construct_dll_file_name(fmu->callbacks, dllDirPath, modelIdentifier);

    if (!dllDirPath ||!dllFileName) {
        fmu->callbacks->free(dllDirPath);
        return jm_status_error;
    }

    if(!instanceEnvironment && !logMessage) {
        /* Use default callbacks */
        instanceEnvironmentFinal = fmu;
        logMessageFinal    = fmi3_log_forwarding;
    }

    if(jm_portability_set_current_working_directory(dllDirPath) != jm_status_success) {
        jm_log_fatal(fmu->callbacks, module, "Could not change to the DLL directory %s", dllDirPath);
        if(ENOENT == errno)
            jm_log_fatal(fmu->callbacks, module, "The FMU contains no binary for this platform.");
        else
            jm_log_fatal(fmu->callbacks, module, "System error: %s", strerror(errno));
    }
    else {
        /* Allocate memory for the C-API struct */
        fmu->capi = fmi3_capi_create_dllfmu(fmu->callbacks, dllFileName, modelIdentifier, instanceEnvironmentFinal,
                logMessageFinal, fmuKind);
    }


    /* Load the DLL handle */
    if (fmu->capi) {
        jm_log_info(fmu->callbacks, module, "Loading '" FMI_PLATFORM "' binary");

        if(fmi3_capi_load_dll(fmu->capi) == jm_status_error) {
            fmi3_capi_destroy_dllfmu(fmu->capi);
            fmu->capi = NULL;
        }
    }

    if(curDir[0] && (jm_portability_set_current_working_directory(curDir) != jm_status_success)) {
        jm_log_error(fmu->callbacks, module, "Could not restore current working directory (%s)", strerror(errno));
    }

    fmu->callbacks->free((jm_voidp)dllDirPath);
    fmu->callbacks->free((jm_voidp)dllFileName);

    if (fmu->capi == NULL) {
        return jm_status_error;
    }

    /* Load the DLL functions */
    if (fmi3_capi_load_fcn(fmu->capi, fmi3_xml_get_capabilities(fmu->md)) == jm_status_error) {
        fmi3_capi_free_dll(fmu->capi);
        fmi3_capi_destroy_dllfmu(fmu->capi);
        fmu->capi = NULL;
        return jm_status_error;
    }
    jm_log_verbose(fmu->callbacks, module, "Successfully loaded all the interface functions");

    return jm_status_success;
}

void fmi3_import_set_debug_mode(fmi3_import_t* fmu, int mode) {
    if (fmu == NULL) {
        return;
    }
    fmi3_capi_set_debug_mode(fmu->capi, mode);
}

void fmi3_import_destroy_dllfmu(fmi3_import_t* fmu) {

    if (fmu == NULL) {
        return;
    }


    if(fmu->capi) {
        jm_log_verbose(fmu->callbacks, module, "Releasing FMU CAPI interface");

        /* Free DLL handle */
        fmi3_capi_free_dll(fmu->capi);

        /* Destroy the C-API struct */
        fmi3_capi_destroy_dllfmu(fmu->capi);

        fmu->capi = NULL;
    }
}

/* FMI 3.0 Common functions */
const char* fmi3_import_get_version(fmi3_import_t* fmu) {
    if(!fmu->capi) {
        jm_log_error(fmu->callbacks, module,"FMU CAPI is not loaded");
        return 0;
    }
    return fmi3_capi_get_version(fmu->capi);
}

fmi3_status_t fmi3_import_set_debug_logging(fmi3_import_t* fmu, fmi3_boolean_t loggingOn, size_t nCategories, fmi3_string_t categories[]) {
    if(!fmu->capi) {
        jm_log_error(fmu->callbacks, module,"FMU CAPI is not loaded");
        return fmi3_status_fatal;
    }
    return fmi3_capi_set_debug_logging(fmu->capi, loggingOn, nCategories, categories);
}


jm_status_enu_t fmi3_import_instantiate_model_exchange(fmi3_import_t* fmu,
                                                       fmi3_string_t  instanceName,
                                                       fmi3_string_t  resourcePath,
                                                       fmi3_boolean_t visible,
                                                       fmi3_boolean_t loggingOn,
                                                       fmi3_instance_environment_t instanceEnvironment,
                                                       fmi3_log_message_callback_ft logMessage)
{
    fmi3_string_t instantiationToken = fmi3_import_get_instantiation_token(fmu);
    fmi3_instance_t c;
    if (!resourcePath)
        resourcePath = fmu->resourcePath;

    c = fmi3_capi_instantiate_model_exchange(
            fmu->capi,
            instanceName,
            instantiationToken,
            resourcePath,
            visible,
            loggingOn,
            instanceEnvironment,
            logMessage);
    if (c == NULL) {
        return jm_status_error;
    } else {
        return jm_status_success;
    }
}

jm_status_enu_t fmi3_import_instantiate_co_simulation(
        fmi3_import_t*                       fmu,
        fmi3_string_t                        instanceName,
        fmi3_string_t                        resourcePath,
        fmi3_boolean_t                       visible,
        fmi3_boolean_t                       loggingOn,
        fmi3_boolean_t                       eventModeUsed,
        fmi3_boolean_t                       earlyReturnAllowed,
        const fmi3_value_reference_t         requiredIntermediateVariables[],
        size_t                               nRequiredIntermediateVariables,
        fmi3_instance_environment_t          instanceEnvironment,
        fmi3_log_message_callback_ft         logMessage,
        fmi3_intermediate_update_callback_ft intermediateUpdate)
{
    fmi3_string_t instantiationToken = fmi3_import_get_instantiation_token(fmu);
    fmi3_instance_t c;
    if (!resourcePath)
        resourcePath = fmu->resourcePath;

    c = fmi3_capi_instantiate_co_simulation(
           fmu->capi,
           instanceName,
           instantiationToken,
           resourcePath,
           visible,
           loggingOn,
           eventModeUsed,
           earlyReturnAllowed,
           requiredIntermediateVariables,
           nRequiredIntermediateVariables,
           instanceEnvironment,
           logMessage,
           intermediateUpdate);
    if (c == NULL) {
        return jm_status_error;
    } else {
        return jm_status_success;
    }
}

jm_status_enu_t fmi3_import_instantiate_scheduled_execution(
        fmi3_import_t*                       fmu,
        fmi3_string_t                        instanceName,
        fmi3_string_t                        resourcePath,
        fmi3_boolean_t                       visible,
        fmi3_boolean_t                       loggingOn,
        fmi3_instance_environment_t          instanceEnvironment,
        fmi3_log_message_callback_ft         logMessage,
        fmi3_clock_update_callback_ft        clockUpdate,
        fmi3_lock_preemption_callback_ft     lockPreemption,
        fmi3_unlock_preemption_callback_ft   unlockPreemption)
{
    fmi3_string_t instantiationToken = fmi3_import_get_instantiation_token(fmu);
    fmi3_instance_t c;
    if (!resourcePath)
        resourcePath = fmu->resourcePath;

    c = fmi3_capi_instantiate_scheduled_execution(
           fmu->capi,
           instanceName,
           instantiationToken,
           resourcePath,
           visible,
           loggingOn,
           instanceEnvironment,
           logMessage,
           clockUpdate,
           lockPreemption,
           unlockPreemption);
    if (c == NULL) {
        return jm_status_error;
    } else {
        return jm_status_success;
    }
}

void fmi3_import_free_instance(fmi3_import_t* fmu) {
    if (fmu != NULL) {
        fmi3_capi_free_instance(fmu->capi);
    }
}

fmi3_status_t fmi3_import_enter_initialization_mode(
        fmi3_import_t* fmu,
        fmi3_boolean_t tolerance_defined,
        fmi3_float64_t tolerance,
        fmi3_float64_t start_time,
        fmi3_boolean_t stop_time_defined,
        fmi3_float64_t stop_time)
{
    assert(fmu);
    return fmi3_capi_enter_initialization_mode(fmu->capi, tolerance_defined, tolerance,
                                      start_time, stop_time_defined, stop_time);
}

fmi3_status_t fmi3_import_exit_initialization_mode(fmi3_import_t* fmu)
{
    assert(fmu);
    return fmi3_capi_exit_initialization_mode(fmu->capi);
}

fmi3_status_t fmi3_import_enter_event_mode(fmi3_import_t* fmu)
{
    return fmi3_capi_enter_event_mode(fmu->capi);
}

fmi3_status_t fmi3_import_terminate(fmi3_import_t* fmu) {
    return fmi3_capi_terminate(fmu->capi);
}

fmi3_status_t fmi3_import_reset(fmi3_import_t* fmu) {
    return fmi3_capi_reset(fmu->capi);
}


fmi3_status_t fmi3_import_set_float64(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences, const fmi3_float64_t values[], size_t nValues) {
    return fmi3_capi_set_float64(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_set_float32(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences, const fmi3_float32_t values[], size_t nValues) {
    return fmi3_capi_set_float32(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_set_int64(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences, const fmi3_int64_t values[], size_t nValues) {
    return fmi3_capi_set_int64(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_set_int32(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences, const fmi3_int32_t values[], size_t nValues) {
    return fmi3_capi_set_int32(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_set_int16(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences, const fmi3_int16_t values[], size_t nValues) {
    return fmi3_capi_set_int16(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_set_int8(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences, const fmi3_int8_t values[], size_t nValues) {
    return fmi3_capi_set_int8(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_set_uint64(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences, const fmi3_uint64_t values[], size_t nValues) {
    return fmi3_capi_set_uint64(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_set_uint32(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences, const fmi3_uint32_t values[], size_t nValues) {
    return fmi3_capi_set_uint32(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_set_uint16(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences, const fmi3_uint16_t values[], size_t nValues) {
    return fmi3_capi_set_uint16(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_set_uint8(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences, const fmi3_uint8_t values[], size_t nValues) {
    return fmi3_capi_set_uint8(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_set_boolean(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        const fmi3_boolean_t values[], size_t nValues) {
    return fmi3_capi_set_boolean(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_set_string(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        const fmi3_string_t values[], size_t nValues) {
    return fmi3_capi_set_string(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_set_binary(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        const size_t sizes[], const fmi3_binary_t values[], size_t nValues) {
    return fmi3_capi_set_binary(fmu->capi, valueReferences, nValueReferences, sizes, values, nValues);
}

fmi3_status_t fmi3_import_get_float64(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences, fmi3_float64_t values[], size_t nValues) {
    return fmi3_capi_get_float64(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_get_float32(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences, fmi3_float32_t values[], size_t nValues) {
    return fmi3_capi_get_float32(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_get_int64(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences, fmi3_int64_t values[], size_t nValues) {
    return fmi3_capi_get_int64(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_get_int32(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences, fmi3_int32_t values[], size_t nValues) {
    return fmi3_capi_get_int32(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_get_int16(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences, fmi3_int16_t values[], size_t nValues) {
    return fmi3_capi_get_int16(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_get_int8(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences, fmi3_int8_t values[], size_t nValues) {
    return fmi3_capi_get_int8(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_get_uint64(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences, fmi3_uint64_t values[], size_t nValues) {
    return fmi3_capi_get_uint64(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_get_uint32(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences, fmi3_uint32_t values[], size_t nValues) {
    return fmi3_capi_get_uint32(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_get_uint16(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences, fmi3_uint16_t values[], size_t nValues) {
    return fmi3_capi_get_uint16(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_get_uint8(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences, fmi3_uint8_t values[], size_t nValues) {
    return fmi3_capi_get_uint8(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_get_boolean(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences, fmi3_boolean_t values[], size_t nValues) {
    return fmi3_capi_get_boolean(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_get_string(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences, fmi3_string_t values[], size_t nValues) {
    return fmi3_capi_get_string(fmu->capi, valueReferences, nValueReferences, values, nValues);
}

fmi3_status_t fmi3_import_get_binary(fmi3_import_t* fmu, const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences, size_t sizes[], fmi3_binary_t values[], size_t nValues) {
    return fmi3_capi_get_binary(fmu->capi, valueReferences, nValueReferences, sizes, values, nValues);
}

/* Getting Variable Dependency Information */
fmi3_status_t fmi3_import_get_number_of_variable_dependencies(
        fmi3_import_t*         fmu,
        fmi3_value_reference_t valueReferences,
        size_t*                nDeps)
{
    return fmi3_capi_get_number_of_variable_dependencies(fmu->capi, valueReferences, nDeps);
}

fmi3_status_t fmi3_import_get_variable_dependencies(
        fmi3_import_t*          fmu,
        fmi3_value_reference_t  dependent,
        size_t                  elementIndicesOfDependent[],
        fmi3_value_reference_t  independents[],
        size_t                  elementIndicesOfIndependents[],
        fmi3_dependency_kind_t  dependencyKinds[],
        size_t                  nDeps)
{
    return fmi3_capi_get_variable_dependencies(fmu->capi, dependent, elementIndicesOfDependent,
            independents, elementIndicesOfIndependents, dependencyKinds, nDeps);
}

/* Getting and setting the internal FMU state */
fmi3_status_t fmi3_import_get_fmu_state(fmi3_import_t* fmu, fmi3_FMU_state_t* s) {
    return fmi3_capi_get_fmu_state(fmu->capi, s);
}
fmi3_status_t fmi3_import_set_fmu_state(fmi3_import_t* fmu, fmi3_FMU_state_t s) {
    return fmi3_capi_set_fmu_state(fmu->capi, s);
}
fmi3_status_t fmi3_import_free_fmu_state(fmi3_import_t* fmu, fmi3_FMU_state_t* s) {
    return fmi3_capi_free_fmu_state(fmu->capi, s);
}
fmi3_status_t fmi3_import_serialized_fmu_state_size(fmi3_import_t* fmu, fmi3_FMU_state_t s, size_t* sz) {
    return fmi3_capi_serialized_fmu_state_size(fmu->capi, s, sz);
}
fmi3_status_t fmi3_import_serialize_fmu_state(fmi3_import_t* fmu, fmi3_FMU_state_t s , fmi3_byte_t data[], size_t sz) {
    return fmi3_capi_serialize_fmu_state(fmu->capi, s, data, sz);
}
fmi3_status_t fmi3_import_de_serialize_fmu_state(fmi3_import_t* fmu, const fmi3_byte_t data[], size_t sz, fmi3_FMU_state_t* s) {
    return fmi3_capi_de_serialize_fmu_state(fmu->capi, data, sz, s);
}

fmi3_status_t fmi3_import_get_directional_derivative(
        fmi3_import_t* fmu,
        const fmi3_value_reference_t unknowns[],
        size_t nUnknowns,
        const fmi3_value_reference_t knowns[],
        size_t nKnowns,
        const fmi3_float64_t seed[],
        size_t nSeed,
        fmi3_float64_t sensitivity[],
        size_t nSensitivity)
{
    return fmi3_capi_get_directional_derivative(fmu->capi, unknowns, nUnknowns, knowns, nKnowns, seed, nSeed,
            sensitivity, nSensitivity);
}

fmi3_status_t fmi3_import_get_adjoint_derivative(
        fmi3_import_t* fmu,
        const fmi3_value_reference_t unknowns[],
        size_t nUnknowns,
        const fmi3_value_reference_t knowns[],
        size_t nKnowns,
        const fmi3_float64_t seed[],
        size_t nSeed,
        fmi3_float64_t sensitivity[],
        size_t nSensitivity)
{
    return fmi3_capi_get_adjoint_derivative(fmu->capi, unknowns, nUnknowns, knowns, nKnowns, seed, nSeed,
            sensitivity, nSensitivity);
}

fmi3_status_t fmi3_import_enter_configuration_mode(fmi3_import_t* fmu)
{
    return fmi3_capi_enter_configuration_mode(fmu->capi);
}

fmi3_status_t fmi3_import_exit_configuration_mode(fmi3_import_t* fmu)
{
    return fmi3_capi_exit_configuration_mode(fmu->capi);
}

fmi3_status_t fmi3_import_get_clock(
        fmi3_import_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        fmi3_clock_t values[])
{
    return fmi3_capi_get_clock(fmu->capi, valueReferences, nValueReferences, values);
}

fmi3_status_t fmi3_import_set_clock(
        fmi3_import_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        const fmi3_clock_t values[])
{
    return fmi3_capi_set_clock(fmu->capi, valueReferences, nValueReferences, values);
}

fmi3_status_t fmi3_import_get_interval_decimal(
        fmi3_import_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        fmi3_float64_t intervals[],
        fmi3_interval_qualifier_t qualifiers[])
{
    return fmi3_capi_get_interval_decimal(fmu->capi, valueReferences, nValueReferences, intervals, qualifiers);
}

fmi3_status_t fmi3_import_get_interval_fraction(
        fmi3_import_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        fmi3_uint64_t counters[],
        fmi3_uint64_t resolutions[],
        fmi3_interval_qualifier_t qualifiers[])
{
    return fmi3_capi_get_interval_fraction(fmu->capi, valueReferences, nValueReferences, counters, resolutions, qualifiers);
}

fmi3_status_t fmi3_import_set_interval_decimal(
        fmi3_import_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        const fmi3_float64_t intervals[])
{
    return fmi3_capi_set_interval_decimal(fmu->capi, valueReferences, nValueReferences, intervals);
}

fmi3_status_t fmi3_import_set_interval_fraction(
        fmi3_import_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        const fmi3_uint64_t counters[],
        const fmi3_uint64_t resolutions[])
{
    return fmi3_capi_set_interval_fraction(fmu->capi, valueReferences, nValueReferences, counters, resolutions);
}

fmi3_status_t fmi3_import_evaluate_discrete_states(
        fmi3_import_t*  fmu)
{
    return fmi3_capi_evaluate_discrete_states(fmu->capi);
}

fmi3_status_t fmi3_import_update_discrete_states(
        fmi3_import_t*  fmu,
        fmi3_boolean_t *discreteStatesNeedUpdate,
        fmi3_boolean_t *terminateSimulation,
        fmi3_boolean_t *nominalsOfContinuousStatesChanged,
        fmi3_boolean_t *valuesOfContinuousStatesChanged,
        fmi3_boolean_t *nextEventTimeDefined,
        fmi3_float64_t *nextEventTime)
{
    return fmi3_capi_update_discrete_states(fmu->capi, discreteStatesNeedUpdate, terminateSimulation,
            nominalsOfContinuousStatesChanged, valuesOfContinuousStatesChanged, nextEventTimeDefined,
            nextEventTime);
}

/* FMI 3.0 ME functions */

fmi3_status_t fmi3_import_enter_continuous_time_mode(fmi3_import_t* fmu) {
    return fmi3_capi_enter_continuous_time_mode(fmu->capi);
}

fmi3_status_t fmi3_import_set_time(fmi3_import_t* fmu, fmi3_float64_t time) {
    return fmi3_capi_set_time(fmu->capi, time);
}

fmi3_status_t fmi3_import_set_continuous_states(fmi3_import_t* fmu, const fmi3_float64_t x[], size_t nx) {
    return fmi3_capi_set_continuous_states(fmu->capi, x, nx);
}

fmi3_status_t fmi3_import_completed_integrator_step(fmi3_import_t* fmu,
  fmi3_boolean_t noSetFMUStatePriorToCurrentPoint,
  fmi3_boolean_t* enterEventMode, fmi3_boolean_t* terminateSimulation) {
    return fmi3_capi_completed_integrator_step(fmu->capi, noSetFMUStatePriorToCurrentPoint,
                                               enterEventMode, terminateSimulation);
}

fmi3_status_t fmi3_import_get_derivatives(fmi3_import_t* fmu, fmi3_float64_t derivatives[], size_t nx) {
    return fmi3_capi_get_derivatives(fmu->capi, derivatives, nx);
}

fmi3_status_t fmi3_import_get_event_indicators(fmi3_import_t* fmu, fmi3_float64_t eventIndicators[], size_t ni) {
    return fmi3_capi_get_event_indicators(fmu->capi, eventIndicators, ni);
}

fmi3_status_t fmi3_import_get_continuous_states(fmi3_import_t* fmu, fmi3_float64_t x[], size_t nx) {
    return fmi3_capi_get_continuous_states(fmu->capi, x, nx);
}

fmi3_status_t fmi3_import_get_nominals_of_continuous_states(fmi3_import_t* fmu, fmi3_float64_t nominals[], size_t nx) {
    return fmi3_capi_get_nominals_of_continuous_states(fmu->capi, nominals, nx);
}

fmi3_status_t fmi3_import_get_number_of_event_indicators(fmi3_import_t* fmu, size_t* nz)
{
    if (!fmi3_import_check_has_FMU(fmu)) return fmi3_status_error;

    /* Since this CAPI is redundant, the values can be taken from XML if the FMU is not yet instantiated */
    if (!fmu->capi || !fmu->capi->inst) {
        *nz = fmi3_xml_get_number_of_event_indicators(fmu->md);
        return fmi3_status_ok;
    }

    return fmi3_capi_get_number_of_event_indicators(fmu->capi, nz);
}

fmi3_status_t fmi3_import_get_number_of_continuous_states(fmi3_import_t* fmu, size_t* nx)
{
    if (!fmi3_import_check_has_FMU(fmu)) return fmi3_status_error;

    /* Since this CAPI is redundant, the values can be taken from XML if the FMU is not yet instantiated */
    if (!fmu->capi || !fmu->capi->inst) {
        *nx = fmi3_xml_get_number_of_continuous_states(fmu->md);
        return fmi3_status_ok;
    }

    return fmi3_capi_get_number_of_continuous_states(fmu->capi, nx);
}

/* FMI 3.0 CS functions */

fmi3_status_t fmi3_import_enter_step_mode(fmi3_import_t* fmu)
{
    return fmi3_capi_enter_step_mode(fmu->capi);
}

fmi3_status_t fmi3_import_get_output_derivatives(
        fmi3_import_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        const fmi3_int32_t orders[],
        fmi3_float64_t values[],
        size_t nValues)
{
    return fmi3_capi_get_output_derivatives(fmu->capi, valueReferences, nValueReferences, orders, values, nValues);
}

fmi3_status_t fmi3_import_do_step(
        fmi3_import_t* fmu,
        fmi3_float64_t currentCommunicationPoint,
        fmi3_float64_t communicationStepSize,
        fmi3_boolean_t noSetFMUStatePriorToCurrentPoint,
        fmi3_boolean_t* eventHandlingNeeded,
        fmi3_boolean_t* terminate,
        fmi3_boolean_t* earlyReturn,
        fmi3_float64_t* lastSuccessfulTime)
{
    return fmi3_capi_do_step(fmu->capi, currentCommunicationPoint, communicationStepSize,
            noSetFMUStatePriorToCurrentPoint, eventHandlingNeeded, terminate, earlyReturn, lastSuccessfulTime);
}

fmi3_status_t fmi3_import_activate_model_partition(
        fmi3_import_t* fmu,
        fmi3_value_reference_t clockReference,
        fmi3_float64_t activationTime)
{
    return fmi3_capi_activate_model_partition(fmu->capi, clockReference, activationTime);
}

#ifdef __cplusplus
}
#endif
