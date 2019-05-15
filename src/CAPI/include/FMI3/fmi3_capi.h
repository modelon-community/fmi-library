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


#ifndef FMI3_CAPI_H_
#define FMI3_CAPI_H_

#include <FMI3/fmi3_types.h>
#include <FMI3/fmi3_functions.h>
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

/** \addtogroup fmi3_capi Standard FMI 2.0 "C" API
 * \brief The "C" API loads and frees the FMI functions and it is through these functions all the communication with the FMU occurs. The FMI import library wraps these functions in a more convenient way.
 *  @{
 */

/**	\addtogroup fmi3_capi_const_destroy FMI 2.0 Utility functions
 *		\brief Utility functions used to load and free the FMI functions.
 *	\addtogroup fmi3_capi_me FMI 2.0 (ME) Model Exchange functions
 *		\brief List of Model Exchange wrapper functions. Common functions are not listed.
 *	\addtogroup fmi3_capi_cs FMI 2.0 (CS) Co-Simulation functions 
 *		\brief List of Co-Simulation wrapper functions. Common functions are not listed.
 *	\addtogroup fmi3_capi_common FMI 2.0 (ME & CS) Common functions
 *		\brief List of wrapper functions that are in common for both Model Exchange and Co-Simulation.
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
 * @param callBackFunctions callbacks passed to the FMU.
 * @return Error status. If the function returns with an error, it is not allowed to call any of the other C-API functions.
 */
fmi3_capi_t* fmi3_capi_create_dllfmu(jm_callbacks* callbacks, const char* dllPath, const char* modelIdentifier, const fmi3_callback_functions_t* callBackFunctions, fmi3_fmu_kind_enu_t standard);

/**
 * \brief Loads the FMI functions from the shared library. The shared library must be loaded before this function can be called, see fmi3_import_create_dllfmu.
 * 
 * @param fmu A model description object returned by fmi3_import_allocate.
 * @param capabilities An array of capability flags according to fmi3_capabilities_enu_t order.
 * @return Error status. If the function returns with an error, no other C-API functions than fmi3_import_free_dll and fmi3_import_destroy_dllfmu are allowed to be called.
 */
jm_status_enu_t fmi3_capi_load_fcn(fmi3_capi_t* fmu, unsigned int capabilities[]);

/**
 * \brief Loads the FMU´s shared library. The shared library functions are not loaded in this call, see fmi3_import_create_dllfmu.
 * 
 * @param fmu A model description object returned by fmi3_import_allocate.
 * @return Error status. If the function returns with an error, no other C-API functions than fmi3_import_destroy_dllfmu are allowed to be called.
 */
jm_status_enu_t fmi3_capi_load_dll(fmi3_capi_t* fmu);

/**
 * \brief Frees the handle to the FMU´s shared library. After this function returnes, no other C-API functions than fmi3_import_destroy_dllfmu and fmi3_import_create_dllfmu are allowed to be called.
 * 
 * @param fmu A model description object returned by fmi3_import_allocate that has loaded the FMU´s shared library, see fmi3_import_create_dllfmu.
 * @return Error status.
 */
jm_status_enu_t fmi3_capi_free_dll(fmi3_capi_t* fmu);

/**
 * \brief Set CAPI debug mode flag. Setting to non-zero prevents DLL unloading in fmi1_capi_free_dll
 *  while all the memory is deallocated. This is to support valgrind debugging. 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param mode The debug mode to set.
 */
void fmi3_capi_set_debug_mode(fmi3_capi_t* fmu, int mode);

/**
 * \brief Get CAPI debug mode flag that was set with fmi1_capi_set_debug_mode()
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
 * \brief Calls the FMI function fmiInstantiate(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function. 
 * @param instanceName The name of the instance.
 * @param fmuType fmi3_model_exchange or fmi3_cosimulation.
 * @param fmuGUID The GUID identifier.
 * @param fmuResourceLocation Access path to the FMU archive resources.
 * @param visible Indicates whether or not the simulator application window shoule be visible.
 * @param loggingOn Enable or disable the debug logger.
 * @return An instance of a model.
 */
fmi3_component_t fmi3_capi_instantiate(fmi3_capi_t* fmu,
    fmi3_string_t instanceName, fmi3_type_t fmuType, fmi3_string_t fmuGUID,
    fmi3_string_t fmuResourceLocation, fmi3_boolean_t visible,
    fmi3_boolean_t loggingOn);

/**
 * \brief Calls the FMI function fmiFreeInstance(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 */
void fmi3_capi_free_instance(fmi3_capi_t* fmu);


/**
 * \brief Calls the FMI function fmiSetupExperiment(...)
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param toleranceDefined True if the @p tolerance argument is to be used
 * @param tolerance Solvers internal to the FMU should use this tolerance or finer, if @p toleranceDefined is true
 * @param startTime Start time of the experiment
 * @param stopTimeDefined True if the @p stopTime argument is to be used
 * @param stopTime Stop time of the experiment, if @p stopTimeDefined is true
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_setup_experiment(fmi3_capi_t* fmu,
    fmi3_boolean_t toleranceDefined, fmi3_real_t tolerance,
    fmi3_real_t startTime, fmi3_boolean_t stopTimeDefined,
    fmi3_real_t stopTime);

/**
 * \brief Calls the FMI function fmiEnterInitializationMode(...)
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_enter_initialization_mode(fmi3_capi_t* fmu);

/**
 * \brief Calls the FMI function fmiExitInitializationMode(...)
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_exit_initialization_mode(fmi3_capi_t* fmu);

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
 * \brief Calls the FMI function fmiSetReal(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value Array of variable values.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_set_real(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const fmi3_real_t    value[]);

/**
 * \brief Calls the FMI function fmiSetInteger(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value Array of variable values.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_set_integer(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const fmi3_integer_t value[]);

/**
 * \brief Calls the FMI function fmiSetBoolean(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value Array of variable values.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_set_boolean(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const fmi3_boolean_t value[]);

/**
 * \brief Calls the FMI function fmiSetString(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value Array of variable values.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_set_string(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const fmi3_string_t  value[]);

/**
 * \brief Calls the FMI function fmiGetReal(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value (Output)Array of variable values.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_real(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, fmi3_real_t    value[]);

/**
 * \brief Calls the FMI function fmiGetInteger(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value (Output)Array of variable values.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_integer(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, fmi3_integer_t value[]);

/**
 * \brief Calls the FMI function fmiGetBoolean(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value (Output)Array of variable values.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_boolean(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, fmi3_boolean_t value[]);

/**
 * \brief Calls the FMI function fmiGetString(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value (Output)Array of variable values.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_string(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, fmi3_string_t  value[]);


/**
 * \brief Calls the FMI function fmiGetTypesPlatform(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @return The platform the FMU was compiled for.
 */
const char* fmi3_capi_get_types_platform(fmi3_capi_t* fmu);

fmi3_status_t fmi3_capi_get_fmu_state           (fmi3_capi_t* fmu, fmi3_FMU_state_t* s);
fmi3_status_t fmi3_capi_set_fmu_state           (fmi3_capi_t* fmu, fmi3_FMU_state_t s);
fmi3_status_t fmi3_capi_free_fmu_state          (fmi3_capi_t* fmu, fmi3_FMU_state_t* s);
fmi3_status_t fmi3_capi_serialized_fmu_state_size(fmi3_capi_t* fmu, fmi3_FMU_state_t s, size_t* sz);
fmi3_status_t fmi3_capi_serialize_fmu_state     (fmi3_capi_t* fmu, fmi3_FMU_state_t s , fmi3_byte_t data[], size_t sz);
fmi3_status_t fmi3_capi_de_serialize_fmu_state  (fmi3_capi_t* fmu, const fmi3_byte_t data[], size_t sz, fmi3_FMU_state_t* s);

/* Getting directional derivatives */
fmi3_status_t fmi3_capi_get_directional_derivative(fmi3_capi_t* fmu, const fmi3_value_reference_t v_ref[], size_t nv,
                                                                   const fmi3_value_reference_t z_ref[], size_t nz,
                                                                   const fmi3_real_t dv[], fmi3_real_t dz[]);
/**@} */

/** \addtogroup fmi3_capi_me
 *  @{
 */

/**
 * \brief Calls the FMI function fmiEnterEventMode(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_enter_event_mode(fmi3_capi_t* fmu);

/**
 * \brief Calls the FMI function fmiNewDiscreteStates(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param eventInfo Pointer to fmi3_event_info_t structure that will be filled in.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_new_discrete_states(fmi3_capi_t* fmu, fmi3_event_info_t* eventInfo);

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
fmi3_status_t fmi3_capi_set_time(fmi3_capi_t* fmu, fmi3_real_t time);

/**
 * \brief Calls the FMI function fmiSetContinuousStates(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param x Array of state values.
 * @param nx Number of states.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_set_continuous_states(fmi3_capi_t* fmu, const fmi3_real_t x[], size_t nx);

/**
 * \brief Calls the FMI function fmiCompletedIntegratorStep(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param noSetFMUStatePriorToCurrentPoint True if fmiSetFMUState will no
          longer be called for time instants prior to current time in this
          simulation run.
 * @param enterEventMode (Output) Call fmiEnterEventMode indicator.
 * @param terminateSimulation (Output) Terminate simulation indicator.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_completed_integrator_step(fmi3_capi_t* fmu,
    fmi3_boolean_t noSetFMUStatePriorToCurrentPoint,
    fmi3_boolean_t* enterEventMode, fmi3_boolean_t* terminateSimulation);

/**
 * \brief Calls the FMI function fmiGetDerivatives(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param derivatives (Output) Array of the derivatives.
 * @param nx Number of derivatives.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_derivatives(fmi3_capi_t* fmu, fmi3_real_t derivatives[]    , size_t nx);

/**
 * \brief Calls the FMI function fmiGetEventIndicators(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param eventIndicators (Output) The event indicators.
 * @param ni Number of event indicators.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_event_indicators(fmi3_capi_t* fmu, fmi3_real_t eventIndicators[], size_t ni);

/**
 * \brief Calls the FMI function fmiGetContinuousStates(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param states (Output) Array of state values.
 * @param nx Number of states.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_continuous_states(fmi3_capi_t* fmu, fmi3_real_t states[], size_t nx);

/**
 * \brief Calls the FMI function fmiGetNominalsOfContinuousStates(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param x_nominal (Output) The nominal values.
 * @param nx Number of nominal values.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_nominals_of_continuous_states(fmi3_capi_t* fmu, fmi3_real_t x_nominal[], size_t nx);

/**@} */

/** \addtogroup fmi3_capi_cs
 *  @{
 */

/**
 * \brief Calls the FMI function fmiSetRealInputDerivatives(...) 
 * 
 * @param fmu C-API struct that has succesfully load the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param order	Array of derivative orders.
 * @param value Array of variable values.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_set_real_input_derivatives(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const fmi3_integer_t order[], const  fmi3_real_t value[]);                                                  

/**
 * \brief Calls the FMI function fmiGetOutputDerivatives(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param order	Array of derivative orders.
 * @param value (Output) Array of variable values.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_real_output_derivatives(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const fmi3_integer_t order[], fmi3_real_t value[]);                                              

/**
 * \brief Calls the FMI function fmiCancelStep(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_cancel_step(fmi3_capi_t* fmu);

/**
 * \brief Calls the FMI function fmiDoStep(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param currentCommunicationPoint Current communication point of the master.
 * @param communicationStepSize Communication step size.
 * @param newStep Indicates whether or not the last communication step was accepted by the master.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_do_step(fmi3_capi_t* fmu, fmi3_real_t currentCommunicationPoint, fmi3_real_t communicationStepSize, fmi3_boolean_t newStep);

/**
 * \brief Calls the FMI function fmiGetStatus(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param s Kind of status to return the value for.
 * @param value (Output) FMI status value.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_status(fmi3_capi_t* fmu, const fmi3_status_kind_t s, fmi3_status_t*  value);

/**
 * \brief Calls the FMI function fmiGetRealStatus(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param s Kind of status to return the value for.
 * @param value (Output) FMI real value.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_real_status(fmi3_capi_t* fmu, const fmi3_status_kind_t s, fmi3_real_t*    value);

/**
 * \brief Calls the FMI function fmiGetIntegerStatus(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param s Kind of status to return the value for.
 * @param value (Output) FMI integer value.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_integer_status(fmi3_capi_t* fmu, const fmi3_status_kind_t s, fmi3_integer_t* value);

/**
 * \brief Calls the FMI function fmiGetBooleanStatus(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param s Kind of status to return the value for.
 * @param value (Output) FMI boolean value.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_boolean_status(fmi3_capi_t* fmu, const fmi3_status_kind_t s, fmi3_boolean_t* value);

/**
 * \brief Calls the FMI function fmiGetStringStatus(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param s Kind of status to return the value for.
 * @param value (Output) FMI string value.
 * @return FMI status.
 */
fmi3_status_t fmi3_capi_get_string_status(fmi3_capi_t* fmu, const fmi3_status_kind_t s, fmi3_string_t*  value);

/** @}*/
/** @}*/

#ifdef __cplusplus 
}
#endif

#endif /* End of header file FMI3_CAPI_H_ */
