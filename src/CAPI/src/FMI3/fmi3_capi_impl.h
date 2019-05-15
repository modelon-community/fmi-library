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
#include <FMI3/fmi3_functions.h>
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
	fmi3_callback_functions_t callBackFunctions;

	jm_callbacks* callbacks;

	DLL_HANDLE dllHandle;

	fmi3_fmu_kind_enu_t standard;

	fmi3_component_t					c;

	int debugMode;

	/* FMI common */
	fmi3_get_version_ft					fmi3GetVersion;
	fmi3_set_debug_logging_ft			fmi3SetDebugLogging;
    fmi3_instantiate_ft			fmi3Instantiate;
    fmi3_free_instance_ft			fmi3FreeInstance;    

    fmi3_setup_experiment_ft 				fmi3SetupExperiment;
    fmi3_enter_initialization_mode_ft 			fmi3EnterInitializationMode;
    fmi3_exit_initialization_mode_ft 			fmi3ExitInitializationMode;

    fmi3_terminate_ft					fmi3Terminate;
    fmi3_reset_ft					fmi3Reset;

    fmi3_set_real_ft					fmi3SetReal;
    fmi3_set_integer_ft					fmi3SetInteger;
    fmi3_set_boolean_ft					fmi3SetBoolean;
    fmi3_set_string_ft					fmi3SetString;
	fmi3_get_real_ft					fmi3GetReal;
    fmi3_get_integer_ft					fmi3GetInteger;
    fmi3_get_boolean_ft					fmi3GetBoolean;
    fmi3_get_string_ft					fmi3GetString;

	fmi3_get_fmu_state_ft				fmi3GetFMUstate;
	fmi3_set_fmu_state_ft				fmi3SetFMUstate;
	fmi3_free_fmu_state_ft				fmi3FreeFMUstate;
	
	fmi3_serialized_fmu_state_size_ft	fmi3SerializedFMUstateSize;
	fmi3_serialize_fmu_state_ft			fmi3SerializeFMUstate;
    fmi3_de_serialize_fmu_state_ft		fmi3DeSerializeFMUstate;

/* Getting partial derivatives */
	fmi3_get_directional_derivative_ft			fmi3GetDirectionalDerivative;

	/* FMI ME */
    fmi3_enter_event_mode_ft		fmi3EnterEventMode;
    fmi3_new_discrete_states_ft		fmi3NewDiscreteStates;
    fmi3_enter_continuous_time_mode_ft	fmi3EnterContinuousTimeMode;
    fmi3_completed_integrator_step_ft	fmi3CompletedIntegratorStep;

    fmi3_get_types_platform_ft			fmi3GetTypesPlatform;    

    fmi3_set_time_ft					fmi3SetTime;
    fmi3_set_continuous_states_ft		fmi3SetContinuousStates;
    fmi3_get_derivatives_ft				fmi3GetDerivatives;
    fmi3_get_event_indicators_ft		fmi3GetEventIndicators;
    fmi3_get_continuous_states_ft		fmi3GetContinuousStates;
    fmi3_get_nominals_of_continuous_states_ft fmi3GetNominalsOfContinuousStates;

	/* FMI CS */
    fmi3_set_real_input_derivatives_ft	fmi3SetRealInputDerivatives;
    fmi3_get_real_output_derivatives_ft	fmi3GetRealOutputDerivatives;
    fmi3_do_step_ft						fmi3DoStep;
    fmi3_cancel_step_ft					fmi3CancelStep;
    fmi3_get_status_ft					fmi3GetStatus;
    fmi3_get_real_status_ft				fmi3GetRealStatus;
    fmi3_get_integer_status_ft			fmi3GetIntegerStatus;
    fmi3_get_boolean_status_ft			fmi3GetBooleanStatus;
    fmi3_get_string_status_ft			fmi3GetStringStatus;

};

#ifdef __cplusplus 
}
#endif

#endif /* End of header file FMI3_CAPI_IMPL_H_ */
