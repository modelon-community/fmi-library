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


#ifndef FMI3_CAPI_H_
#define FMI3_CAPI_H_

#include <FMI3/fmi3_types.h>
#include <FMI3/fmi3_function_types.h>
#include <FMI3/fmi3_enums.h>
#include <JM/jm_portability.h>
#include <JM/jm_callbacks.h>

typedef struct fmi3_capi_t fmi3_capi_t;

#ifdef __cplusplus
extern "C" {
#endif

/** \file fmi3_capi.h
    \brief Public interfaces for the FMI CAPI library.
    */

/** \addtogroup fmi3_capi Standard FMI 3.0 "C" API
 * \brief The "C" API loads and frees the FMI functions and it is through these functions all the communication with the FMU occurs. The FMI import library wraps these functions in a more convenient way.
 *  @{
 */

/**    \addtogroup fmi3_capi_const_destroy FMI 3.0 Utility functions
 *        \brief Utility functions used to load and free the FMI functions.
 *    \addtogroup fmi3_capi_me FMI 3.0 (ME) Model Exchange functions
 *        \brief List of Model Exchange wrapper functions. Common functions are not listed.
 *    \addtogroup fmi3_capi_cs FMI 3.0 (CS) Co-Simulation functions
 *        \brief List of Co-Simulation wrapper functions. Common functions are not listed.
 *    \addtogroup fmi3_capi_se FMI 3.0 (CS) Scheduled-Execution functions
 *        \brief List of Scheduled-Execution wrapper functions. Common functions are not listed.
 *    \addtogroup fmi3_capi_common FMI 3.0 Common functions
 *        \brief List of wrapper functions that are in common for Model Exchange, Co-Simulation and Scheduled-Execution.
 */


/** \addtogroup fmi3_capi_const_destroy
 *  @{
 */

/**
 * \brief Free a C-API struct. All memory allocated since the struct was created is freed.
 *
 * @param fmu A model description object returned by fmi3_import_allocate.
 */
void fmi3_capi_destroy_dllfmu(fmi3_capi_t* fmu);

/**
 * \brief Create a C-API struct. The C-API struct is a placeholder for the FMI DLL functions.
 *
 * @param callbacks ::jm_callbacks used to construct library objects.
 * @param dllPath Full path to the FMU shared library.
 * @param modelIdentifier The model indentifier.
 * @param standard FMI standard that the function should load.
 * @param instanceEnvironment The instance environment that is used during callbacks.
 * @param logMessage The logging function the FMU will use.
 * @return Error status. If the function returns with an error, it is not allowed to call any of the other C-API functions.
 */
fmi3_capi_t* fmi3_capi_create_dllfmu(jm_callbacks* callbacks, const char* dllPath, const char* modelIdentifier,
        fmi3_instance_environment_t instanceEnvironment, fmi3_log_message_callback_ft logMessage,
        fmi3_fmu_kind_enu_t standard);

/**
 * \brief Loads the FMI functions from the shared library. The shared library must be loaded before this function can be called, see fmi3_import_create_dllfmu.
 *
 * @param fmu A model description object returned by fmi3_import_allocate.
 * @return Error status. If the function returns with an error, no other C-API functions than fmi3_import_free_dll and fmi3_import_destroy_dllfmu are allowed to be called.
 */
jm_status_enu_t fmi3_capi_load_fcn(fmi3_capi_t* fmu);

/**
 * \brief Loads the FMUs shared library. The shared library functions are not loaded in this call, see fmi3_import_create_dllfmu.
 *
 * @param fmu A model description object returned by fmi3_import_allocate.
 * @return Error status. If the function returns with an error, no other C-API functions than fmi3_import_destroy_dllfmu are allowed to be called.
 */
jm_status_enu_t fmi3_capi_load_dll(fmi3_capi_t* fmu);

/**
 * \brief Frees the handle to the FMUs shared library. After this function returnes, no other C-API functions than fmi3_import_destroy_dllfmu and fmi3_import_create_dllfmu are allowed to be called.
 *
 * @param fmu A model description object returned by fmi3_import_allocate that has loaded the FMUs shared library, see fmi3_import_create_dllfmu.
 * @return Error status.
 */
jm_status_enu_t fmi3_capi_free_dll(fmi3_capi_t* fmu);

/**
 * \brief Set CAPI debug mode flag. Setting this to a non-zero value will prevent the DLL from
 * unloading when fmi3_capi_free_dll is invoked. This is to support valgrind debugging.
 * Use with caution since this can cause memory leaks if debug mode is not set to 0
 * before deallocating all the resources.
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param mode The debug mode to set.
 */
void fmi3_capi_set_debug_mode(fmi3_capi_t* fmu, int mode);

/**
 * \brief Get CAPI debug mode flag that was set with fmi3_capi_set_debug_mode()
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function. */
int fmi3_capi_get_debug_mode(fmi3_capi_t* fmu);

/**
 * \brief Get the FMU kind loaded by the CAPI
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function. */
fmi3_fmu_kind_enu_t fmi3_capi_get_fmu_kind(fmi3_capi_t* fmu);


/**@} */

/** \addtogroup fmi3_capi_common
 *  @{
 */

/**
 * \brief Calls the FMI function fmiGetVersion()
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @return FMI version.
 */
const char* fmi3_capi_get_version(fmi3_capi_t* fmu);

/**
 * \brief Calls the FMI function fmiSetDebugLogging(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param loggingOn Enable or disable the debug logger.
 * @param nCategories Number of categories to log.
 * @param categories Which categories to log.
 *
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_set_debug_logging(fmi3_capi_t* fmu, fmi3_boolean_t loggingOn, size_t nCategories, fmi3_string_t categories[]);

/**
 * \brief Calls the FMI function fmi3InstantiateModelExchange(...)
 *
 * Arguments 'instanceEnvironment' and 'logMessage' are reused from #fmi3_capi_create_dllfmu.
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param instanceName The name of the instance.
 * @param instantiationToken The instantiation token.
 * @param resourcePath Absolute path to the FMU archive resources.
 * @param visible Indicates whether or not the simulator application window shoule be visible.
 * @param loggingOn Enable or disable the debug logger.
 * @return An instance of a model, or NULL if call failed.
 */
fmi3_instance_t fmi3_capi_instantiate_model_exchange(fmi3_capi_t*   fmu,
                                                     fmi3_string_t  instanceName,
                                                     fmi3_string_t  instantiationToken,
                                                     fmi3_string_t  resourcePath,
                                                     fmi3_boolean_t visible,
                                                     fmi3_boolean_t loggingOn);


/**
 * \brief Calls the FMI function fmi3InstantiateCoSimulation(...)
 *
 * Arguments 'instanceEnvironment' and 'logMessage' are reused from #fmi3_capi_create_dllfmu.
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param instanceName The name of the instance.
 * @param instantiationToken The instantiation token.
 * @param resourcePath Absolute path to the FMU archive resources.
 * @param visible Indicates whether or not the simulator application window shoule be visible.
 * @param loggingOn Enable or disable the debug logger.
 * @param eventModeUsed Indicates whether or not 'Event Mode' is supported.
 * @param earlyReturnAllowed Indicates whether early return is allowed.
 * @param requiredIntermediateVariables Array of value references of all input/output variables
 *        that the simulation algorithm intends to set/get during intermediate updates.
 * @param nRequiredIntermediateVariables Specifies the number of entries in array
 *        'requiredIntermediateVariables'.
 * @param intermediateUpdate Callback for performing intermediate updates.
 * @return An instance of a model, or NULL if call failed.
 */
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
        fmi3_intermediate_update_callback_ft intermediateUpdate);

/**
 * \brief Calls the FMI function fmi3InstantiateScheduledExecution(...)
 *
 * Arguments 'instanceEnvironment' and 'logMessage' are reused from #fmi3_capi_create_dllfmu.
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param instanceName The name of the instance.
 * @param instantiationToken The instantiation token.
 * @param resourcePath Absolute path to the FMU archive resources.
 * @param visible Indicates whether or not the simulator application window shoule be visible.
 * @param loggingOn Enable or disable the debug logger.
 * @param clockUpdate Callback for clock update.
 * @param lockPreemption Callback for locking preemption.
 * @param unlockPreemption Callback for unlocking preemption.
 * @return An instance of a model, or NULL if call failed.
 */
fmi3_instance_t fmi3_capi_instantiate_scheduled_execution(
        fmi3_capi_t*                         fmu,
        fmi3_string_t                        instanceName,
        fmi3_string_t                        instantiationToken,
        fmi3_string_t                        resourcePath,
        fmi3_boolean_t                       visible,
        fmi3_boolean_t                       loggingOn,
        fmi3_clock_update_callback_ft        clockUpdate,
        fmi3_lock_preemption_callback_ft     lockPreemption,
        fmi3_unlock_preemption_callback_ft   unlockPreemption);

/**
 * \brief Calls the FMI function fmiFreeInstance(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 */
void fmi3_capi_free_instance(fmi3_capi_t* fmu);

/**
 * \brief Calls the FMI function fmiEnterInitializationMode(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param toleranceDefined True if the @p tolerance argument is to be used
 * @param tolerance Solvers internal to the FMU should use this tolerance or finer, if @p toleranceDefined is true
 * @param startTime Start time of the experiment
 * @param stopTimeDefined True if the @p stopTime argument is to be used
 * @param stopTime Stop time of the experiment, if @p stopTimeDefined is true
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_enter_initialization_mode(fmi3_capi_t* fmu,
                                                  fmi3_boolean_t toleranceDefined,
                                                  fmi3_float64_t tolerance,
                                                  fmi3_float64_t startTime,
                                                  fmi3_boolean_t stopTimeDefined,
                                                  fmi3_float64_t stopTime);

/**
 * \brief Calls the FMI function fmiExitInitializationMode(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_exit_initialization_mode(fmi3_capi_t* fmu);

/**
 * \brief Calls the FMI function fmiEnterEventMode(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_enter_event_mode(fmi3_capi_t* fmu);

/**
 * \brief Calls the FMI function fmiTerminate(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_terminate(fmi3_capi_t* fmu);

/**
 * \brief Calls the FMI function fmiReset(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_reset(fmi3_capi_t* fmu);


/**
 * \brief Calls the FMI function fmiSetFloat64(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value Array of variable values.
 * @param nValues Total number of variable values, i.e. the number of elements in each array + the number of scalar variables.

 * @return FMI status.
 */
fmi3_status_t fmi3_capi_set_float64(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const fmi3_float64_t value[], size_t nValues);

/**
 * \brief Calls the FMI function fmiSetFloat32(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value Array of variable values.
 * @param nValues Total number of variable values, i.e. the number of elements in each array + the number of scalar variables.

 * @return FMI status.
 */
fmi3_status_t fmi3_capi_set_float32(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const fmi3_float32_t value[], size_t nValues);


/* fmi3_capi_set_intXX documentation: see Import wrapper */

fmi3_status_t fmi3_capi_set_int64( fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const fmi3_int64_t  value[], size_t nValues);
fmi3_status_t fmi3_capi_set_int32( fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const fmi3_int32_t  value[], size_t nValues);
fmi3_status_t fmi3_capi_set_int16( fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const fmi3_int16_t  value[], size_t nValues);
fmi3_status_t fmi3_capi_set_int8(  fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const fmi3_int8_t   value[], size_t nValues);
fmi3_status_t fmi3_capi_set_uint64(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const fmi3_uint64_t value[], size_t nValues);
fmi3_status_t fmi3_capi_set_uint32(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const fmi3_uint32_t value[], size_t nValues);
fmi3_status_t fmi3_capi_set_uint16(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const fmi3_uint16_t value[], size_t nValues);
fmi3_status_t fmi3_capi_set_uint8( fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const fmi3_uint8_t  value[], size_t nValues);

/**
 * \brief Calls the FMI function fmiSetBoolean(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value Array of variable values.
 * @param nValues Number of values in 'value' vector - might not equal 'nvr' if any variable is an array.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_set_boolean(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr,
        const fmi3_boolean_t value[], size_t nValues);

/**
 * \brief Calls the FMI function fmiSetString(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value Array of variable values.
 * @param nValues Number of values in 'value' vector - might not equal 'nvr' if any variable is an array.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_set_string(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr,
        const fmi3_string_t value[], size_t nValues);

/**
 * \brief Calls the FMI function fmiSetBinary(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param sizes Array with the actual sizes of the values for binary variables.
 * @param values Array of variable values.
 * @param nValues Number of values in 'value' vector - might not equal 'nvr' if any variable is an array.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_set_binary(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr,
        const size_t sizes[], const fmi3_binary_t values[], size_t nValues);

/**
 * \brief Calls the FMI function fmiGetFloat64(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param[out] value Array of variable values.
 * @param nValues Total number of variable values, i.e. the number of elements in each array + the number of scalar variables.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_float64(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, fmi3_float64_t value[], size_t nValues);

/**
 * \brief Calls the FMI function fmiGetFloat32(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param[out] value Array of variable values.
 * @param nValues Total number of variable values, i.e. the number of elements in each array + the number of scalar variables.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_float32(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, fmi3_float32_t value[], size_t nValues);


/* fmi3_capi_get_intXX documentation: see Import wrapper */

fmi3_status_t fmi3_capi_get_int64( fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, fmi3_int64_t  value[], size_t nValues);
fmi3_status_t fmi3_capi_get_int32( fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, fmi3_int32_t  value[], size_t nValues);
fmi3_status_t fmi3_capi_get_int16( fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, fmi3_int16_t  value[], size_t nValues);
fmi3_status_t fmi3_capi_get_int8(  fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, fmi3_int8_t   value[], size_t nValues);
fmi3_status_t fmi3_capi_get_uint64(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, fmi3_uint64_t value[], size_t nValues);
fmi3_status_t fmi3_capi_get_uint32(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, fmi3_uint32_t value[], size_t nValues);
fmi3_status_t fmi3_capi_get_uint16(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, fmi3_uint16_t value[], size_t nValues);
fmi3_status_t fmi3_capi_get_uint8( fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, fmi3_uint8_t  value[], size_t nValues);

/**
 * \brief Calls the FMI function fmiGetBoolean(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param[out] value Array of variable values.
 * @param nValues Number of values in 'value' vector - might not equal 'nvr' if any variable is an array.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_boolean(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, fmi3_boolean_t value[],
        size_t nValues);

/**
 * \brief Calls the FMI function fmiGetString(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param[out] value Array of variable values.
 * @param nValues Number of values in 'value' vector - might not equal 'nvr' if any variable is an array.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_string(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, fmi3_string_t value[],
        size_t nValues);

/**
 * \brief Calls the FMI function fmiGetBinary(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param sizes Array with the actual sizes of the values for binary variables.
 * @param[out] value Array of variable values.
 * @param nValues Number of values in 'value' vector - might not equal 'nvr' if any variable is an array.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_binary(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr,
        size_t sizes[], fmi3_binary_t value[], size_t nValues);

fmi3_status_t fmi3_capi_get_number_of_variable_dependencies(
        fmi3_capi_t*           fmu,
        fmi3_value_reference_t value_reference,
        size_t*                n_dependencies);

fmi3_status_t fmi3_capi_get_variable_dependencies(
        fmi3_capi_t* fmu,
        fmi3_value_reference_t  dependent,
        size_t                  elementIndicesOfDependent[],
        fmi3_value_reference_t  independents[],
        size_t                  elementIndicesOfIndependents[],
        fmi3_dependency_kind_t  dependencyKinds[],
        size_t                  nDeps);

fmi3_status_t fmi3_capi_get_fmu_state            (fmi3_capi_t* fmu, fmi3_FMU_state_t* s);
fmi3_status_t fmi3_capi_set_fmu_state            (fmi3_capi_t* fmu, fmi3_FMU_state_t s);
fmi3_status_t fmi3_capi_free_fmu_state           (fmi3_capi_t* fmu, fmi3_FMU_state_t* s);
fmi3_status_t fmi3_capi_serialized_fmu_state_size(fmi3_capi_t* fmu, fmi3_FMU_state_t s, size_t* sz);
fmi3_status_t fmi3_capi_serialize_fmu_state      (fmi3_capi_t* fmu, fmi3_FMU_state_t s ,fmi3_byte_t data[], size_t sz);
fmi3_status_t fmi3_capi_de_serialize_fmu_state   (fmi3_capi_t* fmu, const fmi3_byte_t data[], size_t sz, fmi3_FMU_state_t* s);

/* Getting partial derivatives */

/**
 * \brief Calls the FMI function fmiGetDirectionalDerivative(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param unknowns Value references for the derivatives/outputs to be processed
 * @param nUnknowns Size of 'nUnknowns'.
 * @param knowns Value references for the seed vector.
 * @param nKnowns Size of 'knowns'.
 * @param seed The seed vector.
 * @param nSeed Size of 'seed'.
 * @param sensitivity Calculated directional derivative on output.
 * @param nSensitivity Size of 'sensitivity'.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_directional_derivative(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t unknowns[],
        size_t nUnknowns,
        const fmi3_value_reference_t knowns[],
        size_t nKnowns,
        const fmi3_float64_t seed[],
        size_t nSeed,
        fmi3_float64_t sensitivity[],
        size_t nSensitivity);

/**
 * \brief Calls the FMI function fmiGetAdjointDerivative(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param unknowns Value references for the derivatives/outputs to be processed.
 * @param nUnknowns Size of 'nUnknowns'.
 * @param knowns Value references for the seed vector.
 * @param nKnowns Size of 'knowns'.
 * @param seed The seed vector.
 * @param nSeed Size of 'seed'.
 * @param sensitivity Calculated derivative on output.
 * @param nSensitivity Size of 'sensitivity'.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_adjoint_derivative(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t unknowns[],
        size_t nUnknowns,
        const fmi3_value_reference_t knowns[],
        size_t nKnowns,
        const fmi3_float64_t seed[],
        size_t nSeed,
        fmi3_float64_t sensitivity[],
        size_t nSensitivity);

/* Entering and exiting the Configuration or Reconfiguration Mode */

/**
 * \brief Calls the FMI function fmiEnterConfigurationMode(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_enter_configuration_mode(fmi3_capi_t* fmu);

/**
 * \brief Calls the FMI function fmiExitConfigurationMode(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_exit_configuration_mode(fmi3_capi_t* fmu);

/* Clock related functions */

/**
 * \brief Calls the FMI function fmiGetClock(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param valueReferences Array of value references to clock variables.
 * @param nValueReferences Number of elements in 'valueReferences' array.
 * @param values Output argument containing the values.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_clock(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        fmi3_clock_t values[]);

/**
 * \brief Calls the FMI function fmiSetClock(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param valueReferences Array of value references to clock variables.
 * @param nValueReferences Number of elements in 'valueReferences' array.
 * @param values Output argument containing the values.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_set_clock(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        const fmi3_clock_t values[]);

/**
 * \brief Calls the FMI function fmiGetIntervalDecimal(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param valueReferences Array of value references to clock variables.
 * @param nValueReferences Number of elements in 'valueReferences' array.
 * @param intervals Array of size nValueReferences to retrieve the Clock intervals.
 * @param qualifiers Array of size nValueReferences to retrieve the Clock qualifiers.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_interval_decimal(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        fmi3_float64_t intervals[],
        fmi3_interval_qualifier_t qualifiers[]);

/**
 * \brief Calls the FMI function fmiGetIntervalFraction(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param valueReferences Array of value references to clock variables.
 * @param nValueReferences Number of elements in 'valueReferences' array.
 * @param counters Array of size nValueReferences to retrieve the Clock intervals as fraction counters.
 * @param resolutions Array of size nValueReferences to retrieve the Clock intervals as fraction resolutions.
 * @param qualifiers Array of size nValueReferences to retrieve the Clock qualifiers.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_interval_fraction(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        fmi3_uint64_t counters[],
        fmi3_uint64_t resolutions[],
        fmi3_interval_qualifier_t qualifiers[]);

/**
 * \brief Calls the FMI function fmiGetShiftDecimal(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param valueReferences Array of value references to clock variables.
 * @param nValueReferences Number of elements in 'valueReferences' array.
 * @param shifts Array of size nValueReferences to retrieve the Clock shifts.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_shift_decimal(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        fmi3_float64_t shifts[]);

/**
 * \brief Calls the FMI function fmiGetShiftFraction(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param valueReferences Array of value references to clock variables.
 * @param nValueReferences Number of elements in 'valueReferences' array.
 * @param counters Array of size nValueReferences to retrieve the Clock shifts as fraction counters.
 * @param resolutions Array of size nValueReferences to retrieve the Clock shifts as fraction resolutions.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_shift_fraction(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        fmi3_uint64_t counters[],
        fmi3_uint64_t resolutions[]);

/**
 * \brief Calls the FMI function fmiSetIntervalDecimal(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param valueReferences Array of value references to clock variables.
 * @param nValueReferences Number of elements in 'valueReferences' array.
 * @param intervals Array of size nValueReferences holding the Clock intervals to be set.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_set_interval_decimal(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        const fmi3_float64_t intervals[]);

/**
 * \brief Calls the FMI function fmiGetIntervalFraction(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param valueReferences Array of value references to clock variables.
 * @param nValueReferences Number of elements in 'valueReferences' array.
 * @param counters Array of size nValueReferences that holds the Clock counters to be set.
 * @param resolutions Array of size nValueReferences that holds the Clock resolutions to be set.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_set_interval_fraction(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        const fmi3_uint64_t counters[],
        const fmi3_uint64_t resolutions[]);

/**
 * \brief Calls the FMI function fmiSetShiftDecimal(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param valueReferences Array of value references to clock variables.
 * @param nValueReferences Number of elements in 'valueReferences' array.
 * @param shifts Array of size nValueReferences holding the Clock shifts to be set.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_set_shift_decimal(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        const fmi3_float64_t shifts[]);

/**
 * \brief Calls the FMI function fmiSetShiftFraction(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param valueReferences Array of value references to clock variables.
 * @param nValueReferences Number of elements in 'valueReferences' array.
 * @param counters Array of size nValueReferences that holds the Clock shift counters to be set.
 * @param resolutions Array of size nValueReferences that holds the Clock shift resolutions to be set.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_set_shift_fraction(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        const fmi3_uint64_t counters[],
        const fmi3_uint64_t resolutions[]);

/**
 * \brief Calls the FMI function fmiEvaluateDiscreteStates(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_evaluate_discrete_states(
        fmi3_capi_t*    fmu);

/**
 * \brief Calls the FMI function fmiUpdateDiscreteStates(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param discreteStatesNeedUpdate Return arg: if the FMU needs to update the discrete states.
 * @param terminateSimulation Return arg: if the FMU wants to terminate the simulation.
 * @param nominalsOfContinuousStatesChanged Return arg: if the nominals of continuous states changed.
 * @param valuesOfContinuousStatesChanged Return arg: if the values of continuous states changed.
 * @param nextEventTimeDefined Return arg: if the value of the next time event (arg 'nextEventTime') is defined.
 * @param nextEventTime Return arg: time for next time event.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_update_discrete_states(
        fmi3_capi_t*    fmu,
        fmi3_boolean_t *discreteStatesNeedUpdate,
        fmi3_boolean_t *terminateSimulation,
        fmi3_boolean_t *nominalsOfContinuousStatesChanged,
        fmi3_boolean_t *valuesOfContinuousStatesChanged,
        fmi3_boolean_t *nextEventTimeDefined,
        fmi3_float64_t *nextEventTime);

/**@} */

/** \addtogroup fmi3_capi_me
 *  @{
 */

/**
 * \brief Calls the FMI function fmiEnterContinuousTimeMode(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_enter_continuous_time_mode(fmi3_capi_t* fmu);


/**
 * \brief Calls the FMI function fmiSetTime(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param time Set the current time.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_set_time(fmi3_capi_t* fmu, fmi3_float64_t time);

/**
 * \brief Calls the FMI function fmiSetContinuousStates(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param x Array of state values.
 * @param nx Number of states.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_set_continuous_states(fmi3_capi_t* fmu, const fmi3_float64_t x[], size_t nx);

/**
 * \brief Calls the FMI function fmiCompletedIntegratorStep(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param noSetFMUStatePriorToCurrentPoint True if fmiSetFMUState will no
          longer be called for time instants prior to current time in this
          simulation run.
 * @param[out] enterEventMode  Call fmiEnterEventMode indicator.
 * @param[out] terminateSimulation  Terminate simulation indicator.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_completed_integrator_step(fmi3_capi_t* fmu,
    fmi3_boolean_t noSetFMUStatePriorToCurrentPoint,
    fmi3_boolean_t* enterEventMode, fmi3_boolean_t* terminateSimulation);

/**
 * \brief Calls the FMI function fmiGetContinuousStateDerivatives(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param[out] derivatives  Array of the continuous state derivatives.
 * @param nx Number of continuous state derivatives.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_continuous_state_derivatives(fmi3_capi_t* fmu, fmi3_float64_t derivatives[], size_t nx);

/**
 * \brief Calls the FMI function fmiGetEventIndicators(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param[out] eventIndicators  The event indicators.
 * @param ni Number of event indicators.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_event_indicators(fmi3_capi_t* fmu, fmi3_float64_t eventIndicators[], size_t ni);

/**
 * \brief Calls the FMI function fmiGetContinuousStates(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param[out] x  Array of state values.
 * @param nx Number of states.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_continuous_states(fmi3_capi_t* fmu, fmi3_float64_t x[], size_t nx);

/**
 * \brief Calls the FMI function fmiGetNominalsOfContinuousStates(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param[out] nominals  The nominal values.
 * @param nx Number of nominal values.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_nominals_of_continuous_states(fmi3_capi_t* fmu, fmi3_float64_t nominals[], size_t nx);

/**
 * \brief Calls the FMI function fmi3GetNumberOfEventIndicators(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param[out] nz  Number of event indicators.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_number_of_event_indicators(fmi3_capi_t* fmu, size_t* nz);

/**
 * \brief Calls the FMI function fmi3GetNumberOfContinuousStates(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param[out] nx  Number of continuous states.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_number_of_continuous_states(fmi3_capi_t* fmu, size_t* nx);

/**@} */

/** \addtogroup fmi3_capi_cs
 *  @{
 */

/**
 * \brief Calls the FMI function fmiEnterStepMode(...)
 *
 * @param fmu C-API struct that has succesfully load the FMI function.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_enter_step_mode(fmi3_capi_t* fmu);

/**
 * \brief Calls the FMI function fmiGetOutputDerivatives(...)
 *
 * @param fmu C-API struct that has succesfully load the FMI function.
 * @param valueReferences Array of value references.
 * @param nValueReferences Number of array elements.
 * @param orders Array of derivative orders (same size as 'valueReferences').
 * @param values Array of variable values.
 * @param nValues Number of array elements.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_output_derivatives(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        const fmi3_int32_t orders[],
        fmi3_float64_t values[],
        size_t nValues);

/**
 * \brief Calls the FMI function fmiDoStep(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param currentCommunicationPoint Current communication point of the master.
 * @param communicationStepSize Communication step size.
 * @param noSetFMUStatePriorToCurrentPoint Indicates that the master will not cal SetFMUState to a time prior to
 *        currentCommunicationPoint.
 * @param eventHandlingNeeded Indicates that an event was encountered by the FMU at lastSuccessfulTime.
 * @param[out] terminate  If the FMU requests the simulation to be terminated (since the FMU reached end of
 *        simulation time - not due to internal error).
 * @param[out] earlyReturn  If the FMU returns early.
 * @param[out] lastSuccessfulTime  The internal FMU time when this function returned.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_do_step(
        fmi3_capi_t* fmu,
        fmi3_float64_t currentCommunicationPoint,
        fmi3_float64_t communicationStepSize,
        fmi3_boolean_t noSetFMUStatePriorToCurrentPoint,
        fmi3_boolean_t* eventHandlingNeeded,
        fmi3_boolean_t* terminate,
        fmi3_boolean_t* earlyReturn,
        fmi3_float64_t* lastSuccessfulTime);

/**
 * \brief Calls the FMI function fmiActivateModelPartition(...)
 *
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param clockReference Value reference of an inputClock that will be activated.
 * @param activationTime Simulation (virtual) time of the clock tick.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_activate_model_partition(
        fmi3_capi_t* fmu,
        fmi3_value_reference_t clockReference,
        fmi3_float64_t activationTime);

/** @}*/
/** @}*/

#ifdef __cplusplus
}
#endif

#endif /* End of header file FMI3_CAPI_H_ */
