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
#include <string.h>
#include <assert.h>

#include <JM/jm_types.h>
#include <JM/jm_portability.h>
#include "FMI/fmi_util_options.h"

#include <FMI3/fmi3_capi_impl.h>

#define FUNCTION_NAME_LENGTH_MAX 2048            /* Maximum length of FMI function name. Used in the load DLL function. */
#define STRINGIFY(str) #str

/* Loading shared library functions */
static jm_status_enu_t fmi3_capi_get_fcn(fmi3_capi_t* fmu, const char* function_name, jm_dll_function_ptr* dll_function_ptrptr, jm_status_enu_t* status )
{
        jm_status_enu_t jm_status = jm_portability_load_dll_function(fmu->dllHandle, (char*)function_name, dll_function_ptrptr);
        if (jm_status == jm_status_error) {
            jm_log_error(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Could not load the FMI function '%s'. %s", function_name, jm_portability_get_last_dll_error());
            *status = jm_status_error;
        }
        return jm_status;
}

/* Load FMI functions from DLL macro */
#define LOAD_DLL_FUNCTION(FMIFUNCTION) fmi3_capi_get_fcn(fmu, #FMIFUNCTION, (jm_dll_function_ptr*)&fmu->FMIFUNCTION, &jm_status)

static jm_status_enu_t fmi3_capi_load_common_fcn(fmi3_capi_t* fmu)
{
    jm_status_enu_t jm_status = jm_status_success;

    /* Inquire version numbers of header files and setting logging status */
    LOAD_DLL_FUNCTION(fmi3GetVersion);
    LOAD_DLL_FUNCTION(fmi3SetDebugLogging);

    /* Enter and exit initialization mode, terminate and reset */
    LOAD_DLL_FUNCTION(fmi3Terminate);
    LOAD_DLL_FUNCTION(fmi3Reset);

    /* Creation and destruction of instances and setting debug status */
    LOAD_DLL_FUNCTION(fmi3InstantiateModelExchange);
    LOAD_DLL_FUNCTION(fmi3InstantiateCoSimulation);
    LOAD_DLL_FUNCTION(fmi3InstantiateScheduledExecution);
    LOAD_DLL_FUNCTION(fmi3FreeInstance);

    /* Enter and exit initialization mode, terminate and reset */
    LOAD_DLL_FUNCTION(fmi3EnterInitializationMode);
    LOAD_DLL_FUNCTION(fmi3ExitInitializationMode);
    LOAD_DLL_FUNCTION(fmi3EnterEventMode);

    /* Getting and setting variable values */
    LOAD_DLL_FUNCTION(fmi3GetFloat64);
    LOAD_DLL_FUNCTION(fmi3GetFloat32);
    LOAD_DLL_FUNCTION(fmi3GetInt64);
    LOAD_DLL_FUNCTION(fmi3GetInt32);
    LOAD_DLL_FUNCTION(fmi3GetInt16);
    LOAD_DLL_FUNCTION(fmi3GetInt8);
    LOAD_DLL_FUNCTION(fmi3GetUInt64);
    LOAD_DLL_FUNCTION(fmi3GetUInt32);
    LOAD_DLL_FUNCTION(fmi3GetUInt16);
    LOAD_DLL_FUNCTION(fmi3GetUInt8);
    LOAD_DLL_FUNCTION(fmi3GetBoolean);
    LOAD_DLL_FUNCTION(fmi3GetString);
    LOAD_DLL_FUNCTION(fmi3GetBinary);

    LOAD_DLL_FUNCTION(fmi3SetFloat64);
    LOAD_DLL_FUNCTION(fmi3SetFloat32);
    LOAD_DLL_FUNCTION(fmi3SetInt64);
    LOAD_DLL_FUNCTION(fmi3SetInt32);
    LOAD_DLL_FUNCTION(fmi3SetInt16);
    LOAD_DLL_FUNCTION(fmi3SetInt8);
    LOAD_DLL_FUNCTION(fmi3SetUInt64);
    LOAD_DLL_FUNCTION(fmi3SetUInt32);
    LOAD_DLL_FUNCTION(fmi3SetUInt16);
    LOAD_DLL_FUNCTION(fmi3SetUInt8);
    LOAD_DLL_FUNCTION(fmi3SetBoolean);
    LOAD_DLL_FUNCTION(fmi3SetString);
    LOAD_DLL_FUNCTION(fmi3SetBinary);

    LOAD_DLL_FUNCTION(fmi3GetNumberOfVariableDependencies);
    LOAD_DLL_FUNCTION(fmi3GetVariableDependencies);

    /* Getting and setting the internal FMU state */
    LOAD_DLL_FUNCTION(fmi3GetFMUState);
    LOAD_DLL_FUNCTION(fmi3SetFMUState);
    LOAD_DLL_FUNCTION(fmi3FreeFMUState);
    LOAD_DLL_FUNCTION(fmi3SerializedFMUStateSize);
    LOAD_DLL_FUNCTION(fmi3SerializeFMUState);
    LOAD_DLL_FUNCTION(fmi3DeserializeFMUState);

    /* Getting partial derivatives */
    LOAD_DLL_FUNCTION(fmi3GetDirectionalDerivative);
    LOAD_DLL_FUNCTION(fmi3GetAdjointDerivative);

    /* Entering and exiting the Configuration or Reconfiguration Mode */
    LOAD_DLL_FUNCTION(fmi3EnterConfigurationMode);
    LOAD_DLL_FUNCTION(fmi3ExitConfigurationMode);

    /* Clock related functions */
    LOAD_DLL_FUNCTION(fmi3EvaluateDiscreteStates);
    LOAD_DLL_FUNCTION(fmi3UpdateDiscreteStates);

    LOAD_DLL_FUNCTION(fmi3GetClock);
    LOAD_DLL_FUNCTION(fmi3GetIntervalDecimal);
    LOAD_DLL_FUNCTION(fmi3GetIntervalFraction);
    LOAD_DLL_FUNCTION(fmi3GetShiftDecimal);
    LOAD_DLL_FUNCTION(fmi3GetShiftFraction);
    
    LOAD_DLL_FUNCTION(fmi3SetIntervalDecimal);
    LOAD_DLL_FUNCTION(fmi3SetIntervalFraction);
    LOAD_DLL_FUNCTION(fmi3SetShiftDecimal);
    LOAD_DLL_FUNCTION(fmi3SetShiftFraction);

    return jm_status;
}


/* Load FMI 3.0 CoSimulation functions */
static jm_status_enu_t fmi3_capi_load_cs_fcn(fmi3_capi_t* fmu)
{
    jm_status_enu_t jm_status = jm_status_success;

    jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Loading functions for the co-simulation interface");

    jm_status = fmi3_capi_load_common_fcn(fmu);

    /* Clock related functions */
    LOAD_DLL_FUNCTION(fmi3SetClock);

    /* Simulating the slave */
    LOAD_DLL_FUNCTION(fmi3EnterStepMode);
    LOAD_DLL_FUNCTION(fmi3GetOutputDerivatives);
    LOAD_DLL_FUNCTION(fmi3DoStep);

    return jm_status;
}

/* Load FMI 3.0 Scheduled Execution functions */
static jm_status_enu_t fmi3_capi_load_se_fcn(fmi3_capi_t* fmu)
{
    jm_status_enu_t jm_status = jm_status_success;

    jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Loading functions for the scheduled execution interface");

    jm_status = fmi3_capi_load_common_fcn(fmu);

    /* Simulating the slave */
    LOAD_DLL_FUNCTION(fmi3ActivateModelPartition);

    return jm_status;
}

/* Load FMI 3.0 Model Exchange functions */
static jm_status_enu_t fmi3_capi_load_me_fcn(fmi3_capi_t* fmu)
{
    jm_status_enu_t jm_status = jm_status_success;

    jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Loading functions for the model exchange interface");

    jm_status = fmi3_capi_load_common_fcn(fmu);

    /* Clock related functions */
    LOAD_DLL_FUNCTION(fmi3SetClock);

    /* Enter and exit the different modes */
    LOAD_DLL_FUNCTION(fmi3EnterContinuousTimeMode);
    LOAD_DLL_FUNCTION(fmi3CompletedIntegratorStep);

    /* Providing independent variables and re-initialization of caching */
    LOAD_DLL_FUNCTION(fmi3SetTime);
    LOAD_DLL_FUNCTION(fmi3SetContinuousStates);

    /* Evaluation of the model equations */
    LOAD_DLL_FUNCTION(fmi3GetContinuousStateDerivatives);
    LOAD_DLL_FUNCTION(fmi3GetEventIndicators);
    LOAD_DLL_FUNCTION(fmi3GetContinuousStates);
    LOAD_DLL_FUNCTION(fmi3GetNominalsOfContinuousStates);
    LOAD_DLL_FUNCTION(fmi3GetNumberOfEventIndicators);
    LOAD_DLL_FUNCTION(fmi3GetNumberOfContinuousStates);

    return jm_status;
}

void fmi3_capi_destroy_dllfmu(fmi3_capi_t* fmu)
{
    if (fmu == NULL) {
        return;
    }
    fmi3_capi_free_dll(fmu);
    jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Releasing allocated memory");
    fmi_util_free_options(fmu->callbacks, fmu->options);
    fmu->callbacks->free((void*)fmu->dllPath);
    fmu->callbacks->free((void*)fmu->modelIdentifier);
    fmu->callbacks->free((void*)fmu);
}

fmi3_capi_t* fmi3_capi_create_dllfmu(jm_callbacks* cb, const char* dllPath, const char* modelIdentifier,
        fmi3_instance_environment_t instanceEnvironment, fmi3_log_message_callback_ft logMessage,
        fmi3_fmu_kind_enu_t standard)
{
    fmi3_capi_t* fmu = NULL;

    jm_log_debug(cb, FMI_CAPI_MODULE_NAME, "Initializing data structures for FMICAPI.");

    /* Minor check for the callbacks */
    if (cb == NULL) {
        assert(0);
        return NULL;
    }

    /* Allocate memory for the FMU instance */
    fmu = (fmi3_capi_t*)cb->calloc(1, sizeof(fmi3_capi_t));
    if (fmu == NULL) { /* Could not allocate memory for the FMU struct */
        jm_log_fatal(cb, FMI_CAPI_MODULE_NAME, "Could not allocate memory for the FMU struct.");
        return NULL;
    }

    /* Set the import package callback functions */
    fmu->callbacks = cb;

    /* Set the FMI callback functions and environment */
    fmu->instanceEnvironment = instanceEnvironment;
    fmu->logMessage = logMessage;

    /* Set FMI standard to load */
    fmu->standard = standard;

    /* Set all memory alloated pointers to NULL */
    fmu->dllPath = NULL;
    fmu->modelIdentifier = NULL;

    /* Set to NULL so we can know that FMU has not yet been instantiated */
    fmu->inst = NULL;

    /* Create options */
    fmu->options = fmi_util_allocate_options(cb);

    /* Copy DLL path */
    fmu->dllPath = (char*)cb->calloc(sizeof(char), strlen(dllPath) + 1);
    if (fmu->dllPath == NULL) {
        jm_log_fatal(cb, FMI_CAPI_MODULE_NAME, "Could not allocate memory for the DLL path string.");
        fmi3_capi_destroy_dllfmu(fmu);
        return NULL;
    }
    strcpy((char*)fmu->dllPath, dllPath);

    /* Copy the modelIdentifier */
    fmu->modelIdentifier = (char*)cb->calloc(sizeof(char), strlen(modelIdentifier) + 1);
    if (fmu->modelIdentifier == NULL) {
        jm_log_fatal(cb, FMI_CAPI_MODULE_NAME, "Could not allocate memory for the modelIdentifier string.");
        fmi3_capi_destroy_dllfmu(fmu);
        return NULL;
    }
    strcpy((char*)fmu->modelIdentifier, modelIdentifier);

    jm_log_debug(cb, FMI_CAPI_MODULE_NAME, "Successfully initialized data structures for FMICAPI.");

    /* Everything was successful */
    return fmu;
}

jm_status_enu_t fmi3_capi_load_fcn(fmi3_capi_t* fmu)
{
    assert(fmu);
    if (fmu->standard == fmi3_fmu_kind_me) {
        return fmi3_capi_load_me_fcn(fmu);
    } else if (fmu->standard == fmi3_fmu_kind_cs) {
        return fmi3_capi_load_cs_fcn(fmu);
    } else if (fmu->standard == fmi3_fmu_kind_se) {
        return fmi3_capi_load_se_fcn(fmu);

    } else {
        jm_log_error(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Unexpected FMU kind in FMICAPI.");
        return jm_status_error;
    }
}

jm_status_enu_t fmi3_capi_load_dll(fmi3_capi_t* fmu)
{
    assert(fmu && fmu->dllPath);
    fmu->dllHandle = jm_portability_load_dll_handle_with_flag(fmu->dllPath, fmu->options->loadlibrary_flag); /* Load the shared library */
    if (fmu->dllHandle == NULL) {
        jm_log_fatal(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Could not load the FMU binary: %s", jm_portability_get_last_dll_error());
        return jm_status_error;
    } else {
        jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Loaded FMU binary from %s", fmu->dllPath);
        return jm_status_success;
    }
}

void fmi3_capi_set_debug_mode(fmi3_capi_t* fmu, int mode) {
    if(fmu)
        fmu->debugMode = mode;
}

int fmi3_capi_get_debug_mode(fmi3_capi_t* fmu) {
    if(fmu) return fmu->debugMode;
    return 0;
}

fmi3_fmu_kind_enu_t fmi3_capi_get_fmu_kind(fmi3_capi_t* fmu) {
    if(fmu) return fmu->standard;
    return fmi3_fmu_kind_unknown;
}

jm_status_enu_t fmi3_capi_free_dll(fmi3_capi_t* fmu)
{
    if (fmu == NULL) {
        return jm_status_error; /* Return without writing any log message */
    }

    if (fmu->dllHandle) {
        jm_status_enu_t status =
            (fmu->debugMode != 0) ?
                /* When running valgrind this may be convenient to track mem leaks */
                jm_status_success:
                jm_portability_free_dll_handle(fmu->dllHandle);
        fmu->dllHandle = 0;
        if (status == jm_status_error) { /* Free the library handle */
            jm_log(fmu->callbacks, FMI_CAPI_MODULE_NAME, jm_log_level_error, "Could not free the FMU binary: %s", jm_portability_get_last_dll_error());
            return jm_status_error;
        } else {
            jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Successfully unloaded FMU binary");
            return jm_status_success;
        }
    }
    return jm_status_success;
}

/* Common FMI 3.0 functions */

const char* fmi3_capi_get_version(fmi3_capi_t* fmu)
{
    assert(fmu);
    return fmu->fmi3GetVersion();
}

fmi3_status_t fmi3_capi_set_debug_logging(fmi3_capi_t* fmu, fmi3_boolean_t loggingOn, size_t nCategories, fmi3_string_t categories[])
{
    return fmu->fmi3SetDebugLogging(fmu->inst, loggingOn, nCategories, categories);
}

fmi3_instance_t fmi3_capi_instantiate_model_exchange(fmi3_capi_t*   fmu,
                                                     fmi3_string_t  instanceName,
                                                     fmi3_string_t  instantiationToken,
                                                     fmi3_string_t  resourcePath,
                                                     fmi3_boolean_t visible,
                                                     fmi3_boolean_t loggingOn)
{
    return fmu->inst = fmu->fmi3InstantiateModelExchange(instanceName,
                                                         instantiationToken,
                                                         resourcePath,
                                                         visible,
                                                         loggingOn,
                                                         fmu->instanceEnvironment,
                                                         fmu->logMessage);
}

fmi3_instance_t fmi3_capi_instantiate_co_simulation(
        fmi3_capi_t*                         fmu,
        fmi3_string_t                        instanceName,
        fmi3_string_t                        instantiationToken,
        fmi3_string_t                        resourcePath,
        fmi3_boolean_t                       visible,
        fmi3_boolean_t                       loggingOn,
        fmi3_boolean_t                       eventModeUsed,
        fmi3_boolean_t                       earlyReturnAllowed,
        const fmi3_value_reference_t         requiredIntermediateVariables[],
        size_t                               nRequiredIntermediateVariables,
        fmi3_intermediate_update_callback_ft intermediateUpdate)
{
    return fmu->inst = fmu->fmi3InstantiateCoSimulation(
            instanceName,
            instantiationToken,
            resourcePath,
            visible,
            loggingOn,
            eventModeUsed,
            earlyReturnAllowed,
            requiredIntermediateVariables,
            nRequiredIntermediateVariables,
            fmu->instanceEnvironment,
            fmu->logMessage,
            intermediateUpdate);
}

fmi3_instance_t fmi3_capi_instantiate_scheduled_execution(
    fmi3_capi_t*                         fmu,
    fmi3_string_t                        instanceName,
    fmi3_string_t                        instantiationToken,
    fmi3_string_t                        resourcePath,
    fmi3_boolean_t                       visible,
    fmi3_boolean_t                       loggingOn,
    fmi3_clock_update_callback_ft        clockUpdate,
    fmi3_lock_preemption_callback_ft     lockPreemption,
    fmi3_unlock_preemption_callback_ft   unlockPreemption)
{
    return fmu->inst = fmu->fmi3InstantiateScheduledExecution(
            instanceName,
            instantiationToken,
            resourcePath,
            visible,
            loggingOn,
            fmu->instanceEnvironment,
            fmu->logMessage,
            clockUpdate,
            lockPreemption,
            unlockPreemption);
}

void fmi3_capi_free_instance(fmi3_capi_t* fmu)
{
    if(fmu != NULL && fmu->inst != NULL) {
        fmu->fmi3FreeInstance(fmu->inst);
        fmu->inst = 0;
    }
}

fmi3_status_t fmi3_capi_enter_initialization_mode(fmi3_capi_t* fmu,
                                                  fmi3_boolean_t tolerance_defined,
                                                  fmi3_float64_t tolerance,
                                                  fmi3_float64_t start_time,
                                                  fmi3_boolean_t stop_time_defined,
                                                  fmi3_float64_t stop_time)
{
    assert(fmu); assert(fmu->inst);
    jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3EnterInitializationMode");
    return fmu->fmi3EnterInitializationMode(fmu->inst, tolerance_defined, tolerance,
                                   start_time, stop_time_defined, stop_time);
}

fmi3_status_t fmi3_capi_exit_initialization_mode(fmi3_capi_t* fmu)
{
    assert(fmu); assert(fmu->inst);
    jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3ExitInitializationMode");
    return fmu->fmi3ExitInitializationMode(fmu->inst);
}

fmi3_status_t fmi3_capi_enter_event_mode(fmi3_capi_t* fmu)
{
    assert(fmu); assert(fmu->inst);
    jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3EnterEventMode");
    return fmu->fmi3EnterEventMode(fmu->inst);
}

fmi3_status_t fmi3_capi_terminate(fmi3_capi_t* fmu)
{
    assert(fmu);
    jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3Terminate");
    return fmu->fmi3Terminate(fmu->inst);
}

fmi3_status_t fmi3_capi_reset(fmi3_capi_t* fmu)
{
    return fmu->fmi3Reset(fmu->inst);
}

fmi3_status_t fmi3_capi_get_number_of_variable_dependencies(
    fmi3_capi_t*           fmu,
    fmi3_value_reference_t vr,
    size_t*                nDeps)
{
    return fmu->fmi3GetNumberOfVariableDependencies(fmu->inst, vr, nDeps);
}

fmi3_status_t fmi3_capi_get_variable_dependencies(
    fmi3_capi_t* fmu,
    fmi3_value_reference_t  dependent,
    size_t                  elementIndicesOfDependent[],
    fmi3_value_reference_t  independents[],
    size_t                  elementIndicesOfIndependents[],
    fmi3_dependency_kind_t  dependencyKinds[],
    size_t                  nDeps)
{
    return fmu->fmi3GetVariableDependencies(fmu->inst, dependent, elementIndicesOfDependent,
        independents, elementIndicesOfIndependents, dependencyKinds, nDeps);
}

/* Getting and setting the internal FMU state */
fmi3_status_t fmi3_capi_get_fmu_state(fmi3_capi_t* fmu, fmi3_FMU_state_t* s) {
    return fmu->fmi3GetFMUState(fmu->inst, s);
}
fmi3_status_t fmi3_capi_set_fmu_state(fmi3_capi_t* fmu, fmi3_FMU_state_t s) {
    return fmu->fmi3SetFMUState(fmu->inst,s);
}
fmi3_status_t fmi3_capi_free_fmu_state(fmi3_capi_t* fmu, fmi3_FMU_state_t* s) {
    return fmu->fmi3FreeFMUState(fmu->inst,s);
}
fmi3_status_t fmi3_capi_serialized_fmu_state_size(fmi3_capi_t* fmu, fmi3_FMU_state_t s, size_t* sz) {
    return fmu->fmi3SerializedFMUStateSize(fmu->inst, s, sz);
}
fmi3_status_t fmi3_capi_serialize_fmu_state(fmi3_capi_t* fmu, fmi3_FMU_state_t s , fmi3_byte_t data[], size_t sz) {
    return fmu->fmi3SerializeFMUState(fmu->inst, s, data, sz);
}
fmi3_status_t fmi3_capi_de_serialize_fmu_state(fmi3_capi_t* fmu, const fmi3_byte_t data[], size_t sz, fmi3_FMU_state_t* s) {
    return fmu->fmi3DeserializeFMUState(fmu->inst, data, sz, s);
}

fmi3_status_t fmi3_capi_get_directional_derivative(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t unknowns[],
        size_t nUnknowns,
        const fmi3_value_reference_t knowns[],
        size_t nKnowns,
        const fmi3_float64_t seed[],
        size_t nSeed,
        fmi3_float64_t sensitivity[],
        size_t nSensitivity)
{
    return fmu->fmi3GetDirectionalDerivative(fmu->inst, unknowns, nUnknowns, knowns, nKnowns, seed, nSeed,
            sensitivity, nSensitivity);
}

fmi3_status_t fmi3_capi_get_adjoint_derivative(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t unknowns[],
        size_t nUnknowns,
        const fmi3_value_reference_t knowns[],
        size_t nKnowns,
        const fmi3_float64_t seed[],
        size_t nSeed,
        fmi3_float64_t sensitivity[],
        size_t nSensitivity)
{
    return fmu->fmi3GetAdjointDerivative(fmu->inst, unknowns, nUnknowns, knowns, nKnowns, seed, nSeed,
            sensitivity, nSensitivity);
}

fmi3_status_t fmi3_capi_enter_configuration_mode(fmi3_capi_t* fmu)
{
    return fmu->fmi3EnterConfigurationMode(fmu->inst);
}

fmi3_status_t fmi3_capi_exit_configuration_mode(fmi3_capi_t* fmu)
{
    return fmu->fmi3ExitConfigurationMode(fmu->inst);
}

fmi3_status_t fmi3_capi_get_clock(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        fmi3_clock_t values[])
{
    return fmu->fmi3GetClock(fmu->inst, valueReferences, nValueReferences, values);
}

fmi3_status_t fmi3_capi_set_clock(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        const fmi3_clock_t values[])
{
    return fmu->fmi3SetClock(fmu->inst, valueReferences, nValueReferences, values);
}

fmi3_status_t fmi3_capi_get_interval_decimal(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        fmi3_float64_t intervals[],
        fmi3_interval_qualifier_t qualifiers[])
{
    return fmu->fmi3GetIntervalDecimal(fmu->inst, valueReferences, nValueReferences, intervals, qualifiers);
}

fmi3_status_t fmi3_capi_get_shift_decimal(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        fmi3_float64_t shifts[])
{
    return fmu->fmi3GetShiftDecimal(fmu->inst, valueReferences, nValueReferences, shifts);
}

fmi3_status_t fmi3_capi_get_shift_fraction(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        fmi3_uint64_t counters[],
        fmi3_uint64_t resolutions[])
{
    return fmu->fmi3GetShiftFraction(fmu->inst, valueReferences, nValueReferences, counters, resolutions);
}

fmi3_status_t fmi3_capi_get_interval_fraction(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        fmi3_uint64_t counters[],
        fmi3_uint64_t resolutions[],
        fmi3_interval_qualifier_t qualifiers[])
{
    return fmu->fmi3GetIntervalFraction(
        fmu->inst,
        valueReferences,
        nValueReferences,
        counters,
        resolutions,
        qualifiers);
}

fmi3_status_t fmi3_capi_set_interval_decimal(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        const fmi3_float64_t intervals[])
{
    return fmu->fmi3SetIntervalDecimal(fmu->inst, valueReferences, nValueReferences, intervals);
}

fmi3_status_t fmi3_capi_set_interval_fraction(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        const fmi3_uint64_t counters[],
        const fmi3_uint64_t resolutions[])
{
    return fmu->fmi3SetIntervalFraction(fmu->inst, valueReferences, nValueReferences, counters, resolutions);
}

fmi3_status_t fmi3_capi_set_shift_decimal(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        const fmi3_float64_t shifts[])
{
    return fmu->fmi3SetShiftDecimal(fmu->inst, valueReferences, nValueReferences, shifts);
}

fmi3_status_t fmi3_capi_set_shift_fraction(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        const fmi3_uint64_t counters[],
        const fmi3_uint64_t resolutions[])
{
    return fmu->fmi3SetShiftFraction(fmu->inst, valueReferences, nValueReferences, counters, resolutions);
}

fmi3_status_t fmi3_capi_evaluate_discrete_states(
    fmi3_capi_t* fmu)
{
    return fmu->fmi3EvaluateDiscreteStates(fmu->inst);
}

fmi3_status_t fmi3_capi_update_discrete_states(
        fmi3_capi_t*    fmu,
        fmi3_boolean_t *discreteStatesNeedUpdate,
        fmi3_boolean_t *terminateSimulation,
        fmi3_boolean_t *nominalsOfContinuousStatesChanged,
        fmi3_boolean_t *valuesOfContinuousStatesChanged,
        fmi3_boolean_t *nextEventTimeDefined,
        fmi3_float64_t *nextEventTime)
{
    return fmu->fmi3UpdateDiscreteStates(fmu->inst, discreteStatesNeedUpdate, terminateSimulation,
            nominalsOfContinuousStatesChanged, valuesOfContinuousStatesChanged, nextEventTimeDefined,
            nextEventTime);
}

#define FMISETX(FNAME1, FNAME2, FTYPE) \
fmi3_status_t FNAME1(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const FTYPE value[], size_t nValues)    \
{ \
    return fmu->FNAME2(fmu->inst, vr, nvr, value, nValues); \
}

#define FMIGETX(FNAME1, FNAME2, FTYPE) \
fmi3_status_t FNAME1(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, FTYPE value[], size_t nValues) \
{ \
    return fmu->FNAME2(fmu->inst, vr, nvr, value, nValues); \
}

FMISETX(fmi3_capi_set_float64, fmi3SetFloat64, fmi3_float64_t)
FMISETX(fmi3_capi_set_float32, fmi3SetFloat32, fmi3_float32_t)
FMISETX(fmi3_capi_set_int64,   fmi3SetInt64,   fmi3_int64_t)
FMISETX(fmi3_capi_set_int32,   fmi3SetInt32,   fmi3_int32_t)
FMISETX(fmi3_capi_set_int16,   fmi3SetInt16,   fmi3_int16_t)
FMISETX(fmi3_capi_set_int8,    fmi3SetInt8,    fmi3_int8_t)
FMISETX(fmi3_capi_set_uint64,  fmi3SetUInt64,  fmi3_uint64_t)
FMISETX(fmi3_capi_set_uint32,  fmi3SetUInt32,  fmi3_uint32_t)
FMISETX(fmi3_capi_set_uint16,  fmi3SetUInt16,  fmi3_uint16_t)
FMISETX(fmi3_capi_set_uint8,   fmi3SetUInt8,   fmi3_uint8_t)
FMISETX(fmi3_capi_set_boolean, fmi3SetBoolean, fmi3_boolean_t)
FMISETX(fmi3_capi_set_string,  fmi3SetString,  fmi3_string_t)

FMIGETX(fmi3_capi_get_float64, fmi3GetFloat64, fmi3_float64_t)
FMIGETX(fmi3_capi_get_float32, fmi3GetFloat32, fmi3_float32_t)
FMIGETX(fmi3_capi_get_int64,   fmi3GetInt64,   fmi3_int64_t)
FMIGETX(fmi3_capi_get_int32,   fmi3GetInt32,   fmi3_int32_t)
FMIGETX(fmi3_capi_get_int16,   fmi3GetInt16,   fmi3_int16_t)
FMIGETX(fmi3_capi_get_int8,    fmi3GetInt8,    fmi3_int8_t)
FMIGETX(fmi3_capi_get_uint64,  fmi3GetUInt64,  fmi3_uint64_t)
FMIGETX(fmi3_capi_get_uint32,  fmi3GetUInt32,  fmi3_uint32_t)
FMIGETX(fmi3_capi_get_uint16,  fmi3GetUInt16,  fmi3_uint16_t)
FMIGETX(fmi3_capi_get_uint8,   fmi3GetUInt8,   fmi3_uint8_t)
FMIGETX(fmi3_capi_get_boolean, fmi3GetBoolean, fmi3_boolean_t)
FMIGETX(fmi3_capi_get_string,  fmi3GetString,  fmi3_string_t)

fmi3_status_t fmi3_capi_set_binary(
                              fmi3_capi_t*                 fmu,
                              const fmi3_value_reference_t vr[],
                              size_t                       nvr,
                              const size_t                 sizes[],
                              const fmi3_binary_t          values[],
                              size_t                       nValues)
{
    return fmu->fmi3SetBinary(fmu->inst, vr, nvr, sizes, values, nValues);
}

fmi3_status_t fmi3_capi_get_binary(
                              fmi3_capi_t*                 fmu,
                              const fmi3_value_reference_t vr[],
                              size_t                       nvr,
                              size_t                       sizes[],
                              fmi3_binary_t                values[],
                              size_t                       nValues)
{
    return fmu->fmi3GetBinary(fmu->inst, vr, nvr, sizes, values, nValues);
}
