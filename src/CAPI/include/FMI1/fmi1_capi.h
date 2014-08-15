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


#ifndef FMI1_CAPI_H_
#define FMI1_CAPI_H_

#include <FMI1/fmi1_types.h>
#include <FMI1/fmi1_functions.h>
#include <FMI1/fmi1_enums.h>
#include <JM/jm_portability.h>
#include <JM/jm_callbacks.h>

typedef struct fmi1_capi_t fmi1_capi_t;

#ifdef __cplusplus 
extern "C" {
#endif

/** \file fmi1_capi.h
	\brief Public interfaces for the FMI CAPI library. 
	*/

/** \addtogroup fmi1_capi Standard FMI 1.0 "C" API
 * \brief The "C" API loads and frees the FMI functions and it is through theise functions all the communcation with the FMU occures. The FMI import library wrappes these functions in a more convenient way.
 *  @{
 */

/**	\addtogroup fmi1_capi_const_destroy FMI 1.0 Utility functions
 *		\brief Utility functions used to load and free the FMI functions.
 *	\addtogroup fmi1_capi_me FMI 1.0 (ME) Model Exchange functions
 *		\brief List of Model Exchange wrapper functions. Common functions are not listed.
 *	\addtogroup fmi1_capi_cs FMI 1.0 (CS) Co-Simulation functions 
 *		\brief List of Co-Simulation wrapper functions. Common functions are not listed.
 *	\addtogroup fmi1_capi_common FMI 1.0 (ME & CS) Common functions
 *		\brief List of wrapper functions that are in common for both Model Exchange and Co-Simulation.
 */


/** \addtogroup fmi1_capi_const_destroy
 *  @{
 */

/**
 * \brief Free a C-API struct. All memory allocated since the struct was created is freed.
 * 
 * @param fmu A model description object returned by fmi1_import_allocate.
 */
void fmi1_capi_destroy_dllfmu(fmi1_capi_t* fmu);

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
fmi1_capi_t* fmi1_capi_create_dllfmu(jm_callbacks* callbacks, const char* dllPath, const char* modelIdentifier, fmi1_callback_functions_t callBackFunctions, fmi1_fmu_kind_enu_t standard);

/**
 * \brief Loads the FMI functions from the shared library. The shared library must be loaded before this function can be called, see fmi1_import_create_dllfmu().
 * 
 * @param fmu A model description object returned by fmi1_import_allocate.
 * @return Error status. If the function returns with an error, no other C-API functions than fmi1_import_free_dll and fmi1_import_destroy_dllfmu are allowed to be called.
 */
jm_status_enu_t fmi1_capi_load_fcn(fmi1_capi_t* fmu);

/**
 * \brief Loads the FMU´s shared library. The shared library functions are not loaded in this call, see fmi1_import_create_dllfmu.
 * 
 * @param fmu A model description object returned by fmi1_import_allocate.
 * @return Error status. If the function returns with an error, no other C-API functions than fmi1_import_destroy_dllfmu are allowed to be called.
 */
jm_status_enu_t fmi1_capi_load_dll(fmi1_capi_t* fmu);

/**
 * \brief Frees the handle to the FMU´s shared library. After this function returnes, no other C-API functions than fmi1_import_destroy_dllfmu and fmi1_import_create_dllfmu are allowed to be called.
 * 
 * @param fmu A model description object returned by fmi1_import_allocate that has loaded the FMU´s shared library, see fmi1_import_load_dll.
 * @return Error status.
 */
jm_status_enu_t fmi1_capi_free_dll(fmi1_capi_t* fmu);

/**
 * \brief Set CAPI debug mode flag. Setting to non-zero prevents DLL unloading in fmi1_capi_free_dll
 *  while all the memory is deallocated. This is to support valgrind debugging. 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param mode The debug mode to set.
 */
void fmi1_capi_set_debug_mode(fmi1_capi_t* fmu, int mode);

/**
 * \brief Get CAPI debug mode flag that was set with fmi1_capi_set_debug_mode()
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function. */
int fmi1_capi_get_debug_mode(fmi1_capi_t* fmu);


/**@} */

/** \addtogroup fmi1_capi_common
 *  @{
 */

/**
 * \brief Calls the FMI function fmiGetVersion() 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @return FMI version.
 */
const char* fmi1_capi_get_version(fmi1_capi_t* fmu);

/**
 * \brief Calls the FMI function fmiSetDebugLogging(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param loggingOn Enable or disable the debug logger.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_set_debug_logging(fmi1_capi_t* fmu, fmi1_boolean_t loggingOn);

/**
 * \brief Calls the FMI function fmiSetReal(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value Array of variable values.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_set_real(fmi1_capi_t* fmu, const fmi1_value_reference_t vr[], size_t nvr, const fmi1_real_t    value[]);

/**
 * \brief Calls the FMI function fmiSetInteger(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value Array of variable values.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_set_integer(fmi1_capi_t* fmu, const fmi1_value_reference_t vr[], size_t nvr, const fmi1_integer_t value[]);

/**
 * \brief Calls the FMI function fmiSetBoolean(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value Array of variable values.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_set_boolean(fmi1_capi_t* fmu, const fmi1_value_reference_t vr[], size_t nvr, const fmi1_boolean_t value[]);

/**
 * \brief Calls the FMI function fmiSetString(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value Array of variable values.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_set_string(fmi1_capi_t* fmu, const fmi1_value_reference_t vr[], size_t nvr, const fmi1_string_t  value[]);

/**
 * \brief Calls the FMI function fmiGetReal(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value (Output)Array of variable values.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_get_real(fmi1_capi_t* fmu, const fmi1_value_reference_t vr[], size_t nvr, fmi1_real_t    value[]);

/**
 * \brief Calls the FMI function fmiGetInteger(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value (Output)Array of variable values.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_get_integer(fmi1_capi_t* fmu, const fmi1_value_reference_t vr[], size_t nvr, fmi1_integer_t value[]);

/**
 * \brief Calls the FMI function fmiGetBoolean(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value (Output)Array of variable values.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_get_boolean(fmi1_capi_t* fmu, const fmi1_value_reference_t vr[], size_t nvr, fmi1_boolean_t value[]);

/**
 * \brief Calls the FMI function fmiGetString(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value (Output)Array of variable values.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_get_string(fmi1_capi_t* fmu, const fmi1_value_reference_t vr[], size_t nvr, fmi1_string_t  value[]);

/**@} */

/** \addtogroup fmi1_capi_me
 *  @{
 */

/**
 * \brief Calls the FMI function fmiGetModelTypesPlatform(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @return The platform the FMU was compiled for.
 */
const char* fmi1_capi_get_model_types_platform(fmi1_capi_t* fmu);

/**
 * \brief Calls the FMI function fmiInstantiateModel(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param instanceName The name of the instance.
 * @param GUID The GUID identifier.
 * @param loggingOn Enable or disable the debug logger.
 * @return An instance of a model.
 */
fmi1_component_t fmi1_capi_instantiate_model(fmi1_capi_t* fmu, fmi1_string_t instanceName, fmi1_string_t GUID, fmi1_boolean_t loggingOn);

/**
 * \brief Calls the FMI function fmiFreeModelInstance(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 */
void fmi1_capi_free_model_instance(fmi1_capi_t* fmu);

/**
 * \brief Calls the FMI function fmiSetTime(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param time Set the current time.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_set_time(fmi1_capi_t* fmu, fmi1_real_t time);

/**
 * \brief Calls the FMI function fmiSetContinuousStates(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param x Array of state values.
 * @param nx Number of states.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_set_continuous_states(fmi1_capi_t* fmu, const fmi1_real_t x[], size_t nx);

/**
 * \brief Calls the FMI function fmiCompletedIntegratorStep(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param callEventUpdate (Output) Call fmiEventUpdate indicator.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_completed_integrator_step(fmi1_capi_t* fmu, fmi1_boolean_t* callEventUpdate);

/**
 * \brief Calls the FMI function fmiInitialize(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param toleranceControlled Enable or disable the use of relativeTolerance in the FMU.
 * @param relativeTolerance A relative tolerance used in the FMU.
 * @param eventInfo (Output) fmiEventInfo struct.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_initialize(fmi1_capi_t* fmu, fmi1_boolean_t toleranceControlled, fmi1_real_t relativeTolerance, fmi1_event_info_t* eventInfo);

/**
 * \brief Calls the FMI function fmiGetDerivatives(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param derivatives (Output) Array of the derivatives.
 * @param nx Number of derivatives.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_get_derivatives(fmi1_capi_t* fmu, fmi1_real_t derivatives[]    , size_t nx);

/**
 * \brief Calls the FMI function fmiGetEventIndicators(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param eventIndicators (Output) The event indicators.
 * @param ni Number of event indicators.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_get_event_indicators(fmi1_capi_t* fmu, fmi1_real_t eventIndicators[], size_t ni);

/**
 * \brief Calls the FMI function fmiEventUpdate(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param intermediateResults Indicate whether or not the fmiEventUpdate shall return after every internal event interation.
 * @param eventInfo (Output) An fmiEventInfo struct.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_eventUpdate(fmi1_capi_t* fmu, fmi1_boolean_t intermediateResults, fmi1_event_info_t* eventInfo);

/**
 * \brief Calls the FMI function fmiGetContinuousStates(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param states (Output) Array of state values.
 * @param nx Number of states.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_get_continuous_states(fmi1_capi_t* fmu, fmi1_real_t states[], size_t nx);

/**
 * \brief Calls the FMI function fmiGetNominalContinuousStates(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param x_nominal (Output) The nominal values.
 * @param nx Number of nominal values.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_get_nominal_continuous_states(fmi1_capi_t* fmu, fmi1_real_t x_nominal[], size_t nx);

/**
 * \brief Calls the FMI function fmiGetStateValueReferences(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param vrx (Output) The value-references of the states.
 * @param nx Number of value-references.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_get_state_value_references(fmi1_capi_t* fmu, fmi1_value_reference_t vrx[], size_t nx);

/**
 * \brief Calls the FMI function fmiTerminate(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_terminate(fmi1_capi_t* fmu);

/**@} */

/** \addtogroup fmi1_capi_cs
 *  @{
 */

/**
 * \brief Calls the FMI function fmiGetTypesPlatform(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @return The platform the FMU was compiled for.
 */
const char* fmi1_capi_get_types_platform(fmi1_capi_t* fmu);
/**
 * \brief Calls the FMI function fmiInstantiateSlave(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param instanceName The name of the instance.
 * @param fmuGUID The GUID identifier.
 * @param fmuLocation Access path to the FMU archive.
 * @param mimeType MIME type.
 * @param timeout Communication timeout value in milli-seconds.
 * @param visible Indicates whether or not the simulator application window shoule be visible.
 * @param interactive Indicates whether the simulator application must be manually started by the user.
 * @param loggingOn Enable or disable the debug logger.
 * @return An instance of a model.
 */
fmi1_component_t fmi1_capi_instantiate_slave(fmi1_capi_t* fmu, fmi1_string_t instanceName, fmi1_string_t fmuGUID, fmi1_string_t fmuLocation, fmi1_string_t mimeType,
																 fmi1_real_t timeout, fmi1_boolean_t visible, fmi1_boolean_t interactive, fmi1_boolean_t loggingOn);
/**
 * \brief Calls the FMI function fmiInitializeSlave(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param tStart Start time of the simulation
 * @param StopTimeDefined Indicates whether or not the stop time is used.
 * @param tStop The stop time of the simulation.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_initialize_slave(fmi1_capi_t* fmu, fmi1_real_t tStart, fmi1_boolean_t StopTimeDefined, fmi1_real_t tStop);

/**
 * \brief Calls the FMI function fmiTerminateSlave(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_terminate_slave(fmi1_capi_t* fmu);

/**
 * \brief Calls the FMI function fmiResetSlave(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_reset_slave(fmi1_capi_t* fmu);

/**
 * \brief Calls the FMI function fmiFreeSlaveInstance(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 */
void fmi1_capi_free_slave_instance(fmi1_capi_t* fmu);

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
fmi1_status_t fmi1_capi_set_real_input_derivatives(fmi1_capi_t* fmu, const fmi1_value_reference_t vr[], size_t nvr, const fmi1_integer_t order[], const  fmi1_real_t value[]);                                                  

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
fmi1_status_t fmi1_capi_get_real_output_derivatives(fmi1_capi_t* fmu, const fmi1_value_reference_t vr[], size_t nvr, const fmi1_integer_t order[], fmi1_real_t value[]);                                              

/**
 * \brief Calls the FMI function fmiCancelStep(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_cancel_step(fmi1_capi_t* fmu);

/**
 * \brief Calls the FMI function fmiDoStep(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param currentCommunicationPoint Current communication point of the master.
 * @param communicationStepSize Communication step size.
 * @param newStep Indicates whether or not the last communication step was accepted by the master.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_do_step(fmi1_capi_t* fmu, fmi1_real_t currentCommunicationPoint, fmi1_real_t communicationStepSize, fmi1_boolean_t newStep);

/**
 * \brief Calls the FMI function fmiGetStatus(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param s Kind of status to return the value for.
 * @param value (Output) FMI status value.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_get_status(fmi1_capi_t* fmu, const fmi1_status_kind_t s, fmi1_status_t*  value);

/**
 * \brief Calls the FMI function fmiGetRealStatus(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param s Kind of status to return the value for.
 * @param value (Output) FMI real value.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_get_real_status(fmi1_capi_t* fmu, const fmi1_status_kind_t s, fmi1_real_t*    value);

/**
 * \brief Calls the FMI function fmiGetIntegerStatus(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param s Kind of status to return the value for.
 * @param value (Output) FMI integer value.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_get_integer_status(fmi1_capi_t* fmu, const fmi1_status_kind_t s, fmi1_integer_t* value);

/**
 * \brief Calls the FMI function fmiGetBooleanStatus(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param s Kind of status to return the value for.
 * @param value (Output) FMI boolean value.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_get_boolean_status(fmi1_capi_t* fmu, const fmi1_status_kind_t s, fmi1_boolean_t* value);

/**
 * \brief Calls the FMI function fmiGetStringStatus(...) 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param s Kind of status to return the value for.
 * @param value (Output) FMI string value.
 * @return FMI status.
 */
fmi1_status_t fmi1_capi_get_string_status(fmi1_capi_t* fmu, const fmi1_status_kind_t s, fmi1_string_t*  value);

/** @}*/
/** @}*/

#ifdef __cplusplus 
}
#endif

#endif /* End of header file FMI1_CAPI_H_ */
