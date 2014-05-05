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


#ifndef FMI2_CAPI_IMPL_H_
#define FMI2_CAPI_IMPL_H_

#ifdef __cplusplus 
extern "C" {
#endif

#include <FMI2/fmi2_types.h>
#include <FMI2/fmi2_functions.h>
#include <FMI2/fmi2_enums.h>
#include <FMI2/fmi2_capi.h>
#include <JM/jm_portability.h>
#include <JM/jm_callbacks.h>

#define FMI_CAPI_MODULE_NAME "FMICAPI"

/** 
 * \brief C-API struct used as a placeholder for the FMI functions and shared library handler. 
 */
struct fmi2_capi_t {
	const char* dllPath;
	const char* modelIdentifier;
	fmi2_callback_functions_t callBackFunctions;

	jm_callbacks* callbacks;

	DLL_HANDLE dllHandle;

	fmi2_fmu_kind_enu_t standard;

	fmi2_component_t					c;

	int debugMode;

	/* FMI common */
	fmi2_get_version_ft					fmi2GetVersion;
	fmi2_set_debug_logging_ft			fmi2SetDebugLogging;
    fmi2_instantiate_ft			fmi2Instantiate;
    fmi2_free_instance_ft			fmi2FreeInstance;    

    fmi2_setup_experiment_ft 				fmi2SetupExperiment;
    fmi2_enter_initialization_mode_ft 			fmi2EnterInitializationMode;
    fmi2_exit_initialization_mode_ft 			fmi2ExitInitializationMode;

    fmi2_terminate_ft					fmi2Terminate;
    fmi2_reset_ft					fmi2Reset;

    fmi2_set_real_ft					fmi2SetReal;
    fmi2_set_integer_ft					fmi2SetInteger;
    fmi2_set_boolean_ft					fmi2SetBoolean;
    fmi2_set_string_ft					fmi2SetString;
	fmi2_get_real_ft					fmi2GetReal;
    fmi2_get_integer_ft					fmi2GetInteger;
    fmi2_get_boolean_ft					fmi2GetBoolean;
    fmi2_get_string_ft					fmi2GetString;

	fmi2_get_fmu_state_ft				fmi2GetFMUstate;
	fmi2_set_fmu_state_ft				fmi2SetFMUstate;
	fmi2_free_fmu_state_ft				fmi2FreeFMUstate;
	
	fmi2_serialized_fmu_state_size_ft	fmi2SerializedFMUstateSize;
	fmi2_serialize_fmu_state_ft			fmi2SerializeFMUstate;
    fmi2_de_serialize_fmu_state_ft		fmi2DeSerializeFMUstate;

/* Getting partial derivatives */
	fmi2_get_directional_derivative_ft			fmi2GetDirectionalDerivative;

	/* FMI ME */
    fmi2_enter_event_mode_ft		fmi2EnterEventMode;
    fmi2_new_discrete_states_ft		fmi2NewDiscreteStates;
    fmi2_enter_continuous_time_mode_ft	fmi2EnterContinuousTimeMode;
    fmi2_completed_integrator_step_ft	fmi2CompletedIntegratorStep;

    fmi2_get_types_platform_ft			fmi2GetTypesPlatform;    

    fmi2_set_time_ft					fmi2SetTime;
    fmi2_set_continuous_states_ft		fmi2SetContinuousStates;
    fmi2_get_derivatives_ft				fmi2GetDerivatives;
    fmi2_get_event_indicators_ft		fmi2GetEventIndicators;
    fmi2_get_continuous_states_ft		fmi2GetContinuousStates;
    fmi2_get_nominals_of_continuous_states_ft fmi2GetNominalsOfContinuousStates;

	/* FMI CS */
    fmi2_set_real_input_derivatives_ft	fmi2SetRealInputDerivatives;
    fmi2_get_real_output_derivatives_ft	fmi2GetRealOutputDerivatives;
    fmi2_do_step_ft						fmi2DoStep;
    fmi2_cancel_step_ft					fmi2CancelStep;
    fmi2_get_status_ft					fmi2GetStatus;
    fmi2_get_real_status_ft				fmi2GetRealStatus;
    fmi2_get_integer_status_ft			fmi2GetIntegerStatus;
    fmi2_get_boolean_status_ft			fmi2GetBooleanStatus;
    fmi2_get_string_status_ft			fmi2GetStringStatus;

};

#ifdef __cplusplus 
}
#endif

#endif /* End of header file FMI2_CAPI_IMPL_H_ */
