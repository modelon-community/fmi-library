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


#ifndef FMI3_CAPI_IMPL_H_
#define FMI3_CAPI_IMPL_H_

#ifdef __cplusplus 
extern "C" {
#endif

#include <FMI3/fmi3_types.h>
#include <FMI3/fmi3_function_types.h>
#include <FMI3/fmi3_enums.h>
#include <FMI3/fmi3_capi.h>
#include <JM/jm_portability.h>
#include <JM/jm_callbacks.h>

#define FMI_CAPI_MODULE_NAME "FMICAPI"

/** 
 * \brief C-API struct used as a placeholder for the FMI functions and shared library handler. 
 */
struct fmi3_capi_t {
	const char* dllPath;
	const char* modelIdentifier;

	jm_callbacks* callbacks;

	DLL_HANDLE dllHandle;

	fmi3_fmu_kind_enu_t standard;

	fmi3_instance_t					inst;

	int debugMode;

    /* Callbacks and callback environment */
    fmi3_instance_environment_t  instanceEnvironment;
    fmi3_callback_log_message_ft logMessage;

	/* FMI common */
	fmi3_get_version_ft					fmi3GetVersion;
	fmi3_set_debug_logging_ft			fmi3SetDebugLogging;

    fmi3_instantiate_model_exchange_ft          fmi3InstantiateModelExchange;
    fmi3_instantiate_basic_co_simulation_ft     fmi3InstantiateBasicCoSimulation;
    fmi3_instantiate_hybrid_co_simulation_ft    fmi3InstantiateHybridCoSimulation;
    fmi3_instantiate_scheduled_co_simulation_ft fmi3InstantiateScheduledCoSimulation;
    fmi3_free_instance_ft			            fmi3FreeInstance;    

    fmi3_enter_initialization_mode_ft 			fmi3EnterInitializationMode;
    fmi3_exit_initialization_mode_ft 			fmi3ExitInitializationMode;
    fmi3_enter_event_mode_ft		            fmi3EnterEventMode;

    fmi3_terminate_ft					fmi3Terminate;
    fmi3_reset_ft					fmi3Reset;

    fmi3_set_float64_ft					fmi3SetFloat64;
    fmi3_set_float32_ft					fmi3SetFloat32;
    fmi3_set_int64_ft					fmi3SetInt64;
    fmi3_set_int32_ft					fmi3SetInt32;
    fmi3_set_int16_ft					fmi3SetInt16;
    fmi3_set_int8_ft					fmi3SetInt8;
    fmi3_set_uint64_ft					fmi3SetUInt64;
    fmi3_set_uint32_ft					fmi3SetUInt32;
    fmi3_set_uint16_ft					fmi3SetUInt16;
    fmi3_set_uint8_ft					fmi3SetUInt8;
    fmi3_set_boolean_ft					fmi3SetBoolean;
    fmi3_set_string_ft					fmi3SetString;
    fmi3_set_binary_ft					fmi3SetBinary;
	fmi3_get_float64_ft					fmi3GetFloat64;
    fmi3_get_float32_ft					fmi3GetFloat32;
    fmi3_get_int64_ft					fmi3GetInt64;
    fmi3_get_int32_ft					fmi3GetInt32;
    fmi3_get_int16_ft					fmi3GetInt16;
    fmi3_get_int8_ft					fmi3GetInt8;
    fmi3_get_uint64_ft					fmi3GetUInt64;
    fmi3_get_uint32_ft					fmi3GetUInt32;
    fmi3_get_uint16_ft					fmi3GetUInt16;
    fmi3_get_uint8_ft					fmi3GetUInt8;
    fmi3_get_boolean_ft					fmi3GetBoolean;
    fmi3_get_string_ft					fmi3GetString;
    fmi3_get_binary_ft					fmi3GetBinary;

    fmi3_get_number_of_variable_dependencies_ft fmi3GetNumberOfVariableDependencies;
    fmi3_get_variable_dependencies_ft           fmi3GetVariableDependencies;

	fmi3_get_fmu_state_ft				fmi3GetFMUState;
	fmi3_set_fmu_state_ft				fmi3SetFMUState;
	fmi3_free_fmu_state_ft				fmi3FreeFMUState;
	
	fmi3_serialized_fmu_state_size_ft	fmi3SerializedFMUStateSize;
	fmi3_serialize_fmu_state_ft			fmi3SerializeFMUState;
    fmi3_de_serialize_fmu_state_ft		fmi3DeSerializeFMUState;

    /* Getting partial derivatives */
	fmi3_get_directional_derivative_ft		fmi3GetDirectionalDerivative;
	fmi3_get_adjoint_derivative_ft			fmi3GetAdjointDerivative;

    /* Entering and exiting the Configuration or Reconfiguration Mode */
    fmi3_enter_configuration_mode_ft fmi3EnterConfigurationMode;
    fmi3_exit_configuration_mode_ft  fmi3ExitConfigurationMode;

    /* Clock related functions */
    fmi3_get_clock_ft             fmi3GetClock;
    fmi3_set_clock_ft             fmi3SetClock;
    fmi3_get_interval_decimal_ft  fmi3GetIntervalDecimal;
    fmi3_get_interval_fraction_ft fmi3GetIntervalFraction;
    fmi3_set_interval_decimal_ft  fmi3SetIntervalDecimal;
    fmi3_set_interval_fraction_ft fmi3SetIntervalFraction;
    fmi3_new_discrete_states_ft   fmi3NewDiscreteStates;

	/* FMI ME */
    fmi3_enter_continuous_time_mode_ft	fmi3EnterContinuousTimeMode;
    fmi3_completed_integrator_step_ft	fmi3CompletedIntegratorStep;

    fmi3_set_time_ft                          fmi3SetTime;
    fmi3_set_continuous_states_ft             fmi3SetContinuousStates;
    fmi3_get_derivatives_ft                   fmi3GetDerivatives;
    fmi3_get_event_indicators_ft              fmi3GetEventIndicators;
    fmi3_get_continuous_states_ft             fmi3GetContinuousStates;
    fmi3_get_nominals_of_continuous_states_ft fmi3GetNominalsOfContinuousStates;
    fmi3_get_number_of_event_indicators_ft    fmi3GetNumberOfEventIndicators;
    fmi3_get_number_of_continuous_states_ft   fmi3GetNumberOfContinuousStates;

	/* FMI CS */
    fmi3_enter_step_mode_ft             fmi3EnterStepMode;
    fmi3_get_output_derivatives_ft	    fmi3GetOutputDerivatives;
    fmi3_do_step_ft						fmi3DoStep;
    fmi3_activate_model_partition_ft    fmi3ActivateModelPartition;
};

#ifdef __cplusplus 
}
#endif

#endif /* End of header file FMI3_CAPI_IMPL_H_ */
