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

#ifndef FMI2_IMPORT_CAPI_H_
#define FMI2_IMPORT_CAPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <JM/jm_callbacks.h>
#include <FMI/fmi_import_util.h>
#include <FMI/fmi_import_context.h>
/* #include <FMI2/fmi2_xml_model_description.h>*/

#include <FMI2/fmi2_types.h>
#include <FMI2/fmi2_functions.h>
#include <FMI2/fmi2_enums.h>
/* #include <FMI2/fmi2_capi.h> */

/**
\file fmi2_import_capi.h
Wrapper functions for the FMI 1.0 functions
*/

/**
 * \addtogroup fmi2_import_capi
 * @{
 */

/**	\addtogroup fmi2_import_capi_const_destroy FMI 1.0 Constructor and Destructor	
 * \brief Functions for instantiating and freeing the container of the struct that is responsible for the FMI functions.
 *
 *	Before any of the FMI functions may be called, the construction function must instantiate a fmi_import_t module.
 *	After the fmi_import_t module has been succesfully instantiated, all the FMI functions can be called. To unload
 *	the FMI functions, the destroy functions shall be called.
 *
 * 	\addtogroup fmi2_import_capi_me FMI 1.0 (ME) Model Exchange functions
 * \brief List of Model Exchange wrapper functions. Common functions are not listed.
 *	\addtogroup fmi2_import_capi_cs FMI 1.0 (CS) Co-Simulation functions 
 * \brief List of Co-Simulation wrapper functions. Common functions are not listed.
 *	\addtogroup fmi2_import_capi_common FMI 1.0 (ME & CS) Common functions
 * \brief List of wrapper functions that are in common for both Model Exchange and Co-Simulation.
 */

/**
 * \addtogroup fmi2_import_capi_const_destroy
 * @{
 */

/**
 * \brief Create a C-API struct. The C-API struct is a placeholder for the FMI DLL functions.
 *
 * This function may only be called once if it returned succesfully. fmi2_import_destroy_dllfmu 
 * must be called before this function can be called again. 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml().
 * @param fmuKind Specifies if ModelExchange or CoSimulation binary should be loaded.
 * @param callBackFunctions Callback functions used by the FMI functions internally.
 * @return Error status. If the function returns with an error, it is not allowed to call any of the other C-API functions.
 */
FMILIB_EXPORT jm_status_enu_t fmi2_import_create_dllfmu(fmi2_import_t* fmu, fmi2_fmu_kind_enu_t fmuKind, const fmi2_callback_functions_t* callBackFunctions);

/** \brief Free a C-API struct. All memory allocated since the struct was created is freed.
 * 
 * @param fmu A model description object returned from fmi2_import_parse_xml().
 */
FMILIB_EXPORT void fmi2_import_destroy_dllfmu(fmi2_import_t* fmu);

/**
 * \brief Set CAPI debug mode flag. Setting to non-zero prevents DLL unloading in fmi2_import_destroy_dllfmu
 *  while all the memory is deallocated. This is to support valgrind debugging. 
 * 
 * @param fmu C-API struct that has succesfully loaded the FMI function.
 * @param mode The debug mode to set.
 */
FMILIB_EXPORT void fmi2_import_set_debug_mode(fmi2_import_t* fmu, int mode);
/**@} */

/**
 * \addtogroup fmi2_import_capi_common
 * @{
 */

/**
 * \brief Wrapper for the FMI function fmiGetVersion() 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @return FMI version.
 */
FMILIB_EXPORT const char* fmi2_import_get_version(fmi2_import_t* fmu);

/**
 * \brief Wrapper for the FMI function fmiSetDebugLogging(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param loggingOn Enable or disable the debug logger.
 * @param nCategories Number of categories to log.
 * @param categories Which categories to log.
* @return FMI status.
 */
 FMILIB_EXPORT fmi2_status_t fmi2_import_set_debug_logging(fmi2_import_t* fmu, fmi2_boolean_t loggingOn, size_t nCategories, fmi2_string_t categories[]);

/**
 * \brief Wrapper for the FMI function fmiSetReal(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value Array of variable values.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_set_real(fmi2_import_t* fmu, const fmi2_value_reference_t vr[], size_t nvr, const fmi2_real_t    value[]);

/**
 * \brief Wrapper for the FMI function fmiSetInteger(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value Array of variable values.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_set_integer(fmi2_import_t* fmu, const fmi2_value_reference_t vr[], size_t nvr, const fmi2_integer_t value[]);

/**
 * \brief Wrapper for the FMI function fmiSetBoolean(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value Array of variable values.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_set_boolean(fmi2_import_t* fmu, const fmi2_value_reference_t vr[], size_t nvr, const fmi2_boolean_t value[]);

/**
 * \brief Wrapper for the FMI function fmiSetString(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value Array of variable values.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_set_string(fmi2_import_t* fmu, const fmi2_value_reference_t vr[], size_t nvr, const fmi2_string_t  value[]);


/**
 * \brief Wrapper for the FMI function fmiGetReal(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value (Output)Array of variable values.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_get_real(fmi2_import_t* fmu, const fmi2_value_reference_t vr[], size_t nvr, fmi2_real_t    value[]);

/**
 * \brief Wrapper for the FMI function fmiGetInteger(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value (Output)Array of variable values.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_get_integer(fmi2_import_t* fmu, const fmi2_value_reference_t vr[], size_t nvr, fmi2_integer_t value[]);

/**
 * \brief Wrapper for the FMI function fmiGetBoolean(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value (Output)Array of variable values.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_get_boolean(fmi2_import_t* fmu, const fmi2_value_reference_t vr[], size_t nvr, fmi2_boolean_t value[]);

/**
 * \brief Wrapper for the FMI function fmiGetString(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param value (Output)Array of variable values.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_get_string(fmi2_import_t* fmu, const fmi2_value_reference_t vr[], size_t nvr, fmi2_string_t  value[]);

/**
 * \brief Wrapper for the FMI function fmiGetTypesPlatform(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @return The platform the FMU was compiled for.
 */
FMILIB_EXPORT const char* fmi2_import_get_types_platform(fmi2_import_t* fmu);

FMILIB_EXPORT fmi2_status_t fmi2_import_get_fmu_state           (fmi2_import_t* fmu, fmi2_FMU_state_t*);
FMILIB_EXPORT fmi2_status_t fmi2_import_set_fmu_state           (fmi2_import_t* fmu, fmi2_FMU_state_t);
FMILIB_EXPORT fmi2_status_t fmi2_import_free_fmu_state          (fmi2_import_t* fmu, fmi2_FMU_state_t*);
FMILIB_EXPORT fmi2_status_t fmi2_import_serialized_fmu_state_size(fmi2_import_t* fmu, fmi2_FMU_state_t, size_t*);
FMILIB_EXPORT fmi2_status_t fmi2_import_serialize_fmu_state     (fmi2_import_t* fmu, fmi2_FMU_state_t, fmi2_byte_t[], size_t);
FMILIB_EXPORT fmi2_status_t fmi2_import_de_serialize_fmu_state  (fmi2_import_t* fmu, const fmi2_byte_t[], size_t, fmi2_FMU_state_t*);

/* Getting partial derivatives */
/*   typedef fmi2_status_t (*fmi2_get_partial_derivatives_ft)   (fmi2_component_t, fmi2_set_matrix_element_ft, void*, void*, void*, void*); */
FMILIB_EXPORT fmi2_status_t fmi2_import_get_directional_derivative(fmi2_import_t* fmu, const fmi2_value_reference_t[], size_t,
                                                                   const fmi2_value_reference_t[], size_t,
                                                                   const fmi2_real_t[], fmi2_real_t[]);

/**@} */

/**
 * \addtogroup fmi2_import_capi_me
 * @{
 */


/**
 * \brief Wrapper for the FMI function fmiInstantiateModel(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param instanceName The name of the instance.
 * @param fmuResourceLocation Access path URI to the FMU archive resources. If this is NULL pointer the FMU will get the path to the unzipped location.
 * @param visible Indicates whether or not the simulator application window shoule be visible.
 * @return Error status. Returnes jm_status_error if fmiInstantiateModel returned NULL, otherwise jm_status_success.
 */
FMILIB_EXPORT jm_status_enu_t fmi2_import_instantiate_model(fmi2_import_t* fmu, fmi2_string_t instanceName, fmi2_string_t fmuResourceLocation, fmi2_boolean_t visible);

/**
 * \brief Wrapper for the FMI function fmiFreeModelInstance(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 */
FMILIB_EXPORT void fmi2_import_free_model_instance(fmi2_import_t* fmu);

/**
 * \brief Wrapper for the FMI function fmiSetTime(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param time Set the current time.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_set_time(fmi2_import_t* fmu, fmi2_real_t time);

/**
 * \brief Wrapper for the FMI function fmiSetContinuousStates(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param x Array of state values.
 * @param nx Number of states.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_set_continuous_states(fmi2_import_t* fmu, const fmi2_real_t x[], size_t nx);

/**
 * \brief Wrapper for the FMI function fmiCompletedIntegratorStep(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param callEventUpdate (Output) Call fmiEventUpdate indicator.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_completed_integrator_step(fmi2_import_t* fmu, fmi2_boolean_t* callEventUpdate);

/**
 * \brief Wrapper for the FMI function fmiInitialize(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param toleranceControlled Enable or disable the use of relativeTolerance in the FMU.
 * @param relativeTolerance A relative tolerance used in the FMU.
 * @param eventInfo (Output) fmiEventInfo struct.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_initialize_model(fmi2_import_t* fmu, fmi2_boolean_t toleranceControlled, fmi2_real_t relativeTolerance, fmi2_event_info_t* eventInfo);

/**
 * \brief Wrapper for the FMI function fmiGetDerivatives(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param derivatives (Output) Array of the derivatives.
 * @param nx Number of derivatives.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_get_derivatives(fmi2_import_t* fmu, fmi2_real_t derivatives[], size_t nx);

/**
 * \brief Wrapper for the FMI function fmiGetEventIndicators(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param eventIndicators (Output) The event indicators.
 * @param ni Number of event indicators.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_get_event_indicators(fmi2_import_t* fmu, fmi2_real_t eventIndicators[], size_t ni);

/**
 * \brief Wrapper for the FMI function fmiEventUpdate(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param intermediateResults Indicate whether or not the fmiEventUpdate shall return after every internal event interation.
 * @param eventInfo (Output) An fmiEventInfo struct.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_eventUpdate(fmi2_import_t* fmu, fmi2_boolean_t intermediateResults, fmi2_event_info_t* eventInfo);

/**
 * \brief Wrapper for the FMI function fmiCompletedEventIteration(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_completed_event_iteration(fmi2_import_t* fmu);

/**
 * \brief Wrapper for the FMI function fmiGetContinuousStates(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param states (Output) Array of state values.
 * @param nx Number of states.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_get_continuous_states(fmi2_import_t* fmu, fmi2_real_t states[], size_t nx);

/**
 * \brief Wrapper for the FMI function fmiGetNominalContinuousStates(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param x_nominal (Output) The nominal values.
 * @param nx Number of nominal values.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_get_nominal_continuous_states(fmi2_import_t* fmu, fmi2_real_t x_nominal[], size_t nx);

/**
 * \brief Wrapper for the FMI function fmiGetStateValueReferences(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param vrx (Output) The value-references of the states.
 * @param nx Number of value-references.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_get_state_value_references(fmi2_import_t* fmu, fmi2_value_reference_t vrx[], size_t nx);

/**
 * \brief Wrapper for the FMI function fmiTerminate(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_terminate(fmi2_import_t* fmu);

/**@} */

/**
 * \addtogroup fmi2_import_capi_cs
 * @{
 */


/**
 * \brief Wrapper for the FMI function fmiInstantiateSlave(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param instanceName The name of the instance.
 * @param fmuResourceLocation Access path to the resource subdir in the FMU archive.
 * @param visible Indicates whether or not the simulator application window shoule be visible.
 * @return Error status. Returnes jm_status_error if fmiInstantiateSlave returned NULL, otherwise jm_status_success.
 */
FMILIB_EXPORT jm_status_enu_t fmi2_import_instantiate_slave(fmi2_import_t* fmu, fmi2_string_t instanceName, fmi2_string_t fmuResourceLocation, fmi2_boolean_t visible);

/**
 * \brief Wrapper for the FMI function fmiInitializeSlave(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param relativeTolerance suggests a relative (local) tolerance in case the slave utilizes a numerical integrator with variable step size and error estimation.
 * @param tStart Start time of the simulation
 * @param StopTimeDefined Indicates whether or not the stop time is used.
 * @param tStop The stop time of the simulation.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_initialize_slave(fmi2_import_t* fmu,  fmi2_real_t  relativeTolerance, 
					fmi2_real_t tStart, fmi2_boolean_t StopTimeDefined, fmi2_real_t tStop);

/**
 * \brief Wrapper for the FMI function fmiTerminateSlave(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_terminate_slave(fmi2_import_t* fmu);

/**
 * \brief Wrapper for the FMI function fmiResetSlave(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_reset_slave(fmi2_import_t* fmu);

/**
 * \brief Wrapper for the FMI function fmiFreeSlaveInstance(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 */
FMILIB_EXPORT void fmi2_import_free_slave_instance(fmi2_import_t* fmu);

/**
 * \brief Wrapper for the FMI function fmiSetRealInputDerivatives(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param order	Array of derivative orders.
 * @param value Array of variable values.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_set_real_input_derivatives(fmi2_import_t* fmu, const fmi2_value_reference_t vr[], size_t nvr, const fmi2_integer_t order[], const  fmi2_real_t value[]);                                                  

/**
 * \brief Wrapper for the FMI function fmiGetOutputDerivatives(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param vr Array of value references.
 * @param nvr Number of array elements.
 * @param order	Array of derivative orders.
 * @param value (Output) Array of variable values.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_get_real_output_derivatives(fmi2_import_t* fmu, const fmi2_value_reference_t vr[], size_t nvr, const fmi2_integer_t order[], fmi2_real_t value[]);                                              

/**
 * \brief Wrapper for the FMI function fmiCancelStep(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_cancel_step(fmi2_import_t* fmu);

/**
 * \brief Wrapper for the FMI function fmiDoStep(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param currentCommunicationPoint Current communication point of the master.
 * @param communicationStepSize Communication step size.
 * @param newStep Indicates whether or not the last communication step was accepted by the master.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_do_step(fmi2_import_t* fmu, fmi2_real_t currentCommunicationPoint, fmi2_real_t communicationStepSize, fmi2_boolean_t newStep);

/**
 * \brief Wrapper for the FMI function fmiGetStatus(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param s Kind of status to return the value for.
 * @param value (Output) FMI status value.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_get_status(fmi2_import_t* fmu, const fmi2_status_kind_t s, fmi2_status_t*  value);

/**
 * \brief Wrapper for the FMI function fmiGetRealStatus(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param s Kind of status to return the value for.
 * @param value (Output) FMI real value.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_get_real_status(fmi2_import_t* fmu, const fmi2_status_kind_t s, fmi2_real_t*    value);

/**
 * \brief Wrapper for the FMI function fmiGetIntegerStatus(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn.
 * @param s Kind of status to return the value for.
 * @param value (Output) FMI integer value.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_get_integer_status(fmi2_import_t* fmu, const fmi2_status_kind_t s, fmi2_integer_t* value);

/**
 * \brief Wrapper for the FMI function fmiGetBooleanStatus(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn.
 * @param s Kind of status to return the value for.
 * @param value (Output) FMI boolean value.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_get_boolean_status(fmi2_import_t* fmu, const fmi2_status_kind_t s, fmi2_boolean_t* value);

/**
 * \brief Wrapper for the FMI function fmiGetStringStatus(...) 
 * 
 * @param fmu A model description object returned by fmi2_import_parse_xml() that has loaded the FMI functions, see fmi2_import_load_fcn().
 * @param s Kind of status to return the value for.
 * @param value (Output) FMI string value.
 * @return FMI status.
 */
FMILIB_EXPORT fmi2_status_t fmi2_import_get_string_status(fmi2_import_t* fmu, const fmi2_status_kind_t s, fmi2_string_t*  value);

/**@} */


/**@} */

#ifdef __cplusplus
}
#endif
#endif /* End of header FMI2_IMPORT_CAPI_H_ */
