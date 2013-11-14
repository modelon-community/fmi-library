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
 * \brief C-API struct used as a placeholder for the FMI funktions and shared library handler. 
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
	fmi2_get_version_ft					fmiGetVersion;
	fmi2_set_debug_logging_ft			fmiSetDebugLogging;
    fmi2_instantiate_ft			fmiInstantiate;
    fmi2_free_instance_ft			fmiFreeInstance;    
    fmi2_set_real_ft					fmiSetReal;
    fmi2_set_integer_ft					fmiSetInteger;
    fmi2_set_boolean_ft					fmiSetBoolean;
    fmi2_set_string_ft					fmiSetString;
	fmi2_get_real_ft					fmiGetReal;
    fmi2_get_integer_ft					fmiGetInteger;
    fmi2_get_boolean_ft					fmiGetBoolean;
    fmi2_get_string_ft					fmiGetString;

	fmi2_get_fmu_state_ft				fmiGetFMUstate;
	fmi2_set_fmu_state_ft				fmiSetFMUstate;
	fmi2_free_fmu_state_ft				fmiFreeFMUstate;
	
	fmi2_serialized_fmu_state_size_ft	fmiSerializedFMUstateSize;
	fmi2_serialize_fmu_state_ft			fmiSerializeFMUstate;
    fmi2_de_serialize_fmu_state_ft		fmiDeSerializeFMUstate;

/* Getting partial derivatives */
/*  fmi2_get_partial_derivatives_ft     fmiGetPartialDerivatives; */
	fmi2_get_directional_derivative_ft			fmiGetDirectionalDerivative;

	/* FMI ME */
    fmi2_get_types_platform_ft			fmiGetTypesPlatform;    
    fmi2_completed_event_iteration_ft   fmiCompletedEventIteration;

    fmi2_set_time_ft					fmiSetTime;
    fmi2_set_continuous_states_ft		fmiSetContinuousStates;
    fmi2_completed_integrator_step_ft	fmiCompletedIntegratorStep;
    fmi2_initialize_model_ft			fmiInitializeModel;
    fmi2_get_derivatives_ft				fmiGetDerivatives;
    fmi2_get_event_indicators_ft		fmiGetEventIndicators;
    fmi2_event_update_ft				fmiEventUpdate;
    fmi2_get_continuous_states_ft		fmiGetContinuousStates;
    fmi2_get_nominal_continuous_states_ft fmiGetNominalContinuousStates;
    fmi2_terminate_ft					fmiTerminate;

	/* FMI CS */
    fmi2_initialize_slave_ft			fmiInitializeSlave;
    fmi2_terminate_slave_ft				fmiTerminateSlave;
    fmi2_reset_slave_ft					fmiResetSlave;
    fmi2_set_real_input_derivatives_ft	fmiSetRealInputDerivatives;
    fmi2_get_real_output_derivatives_ft	fmiGetRealOutputDerivatives;
    fmi2_do_step_ft						fmiDoStep;
    fmi2_cancel_step_ft					fmiCancelStep;
    fmi2_get_status_ft					fmiGetStatus;
    fmi2_get_real_status_ft				fmiGetRealStatus;
    fmi2_get_integer_status_ft			fmiGetIntegerStatus;
    fmi2_get_boolean_status_ft			fmiGetBooleanStatus;
    fmi2_get_string_status_ft			fmiGetStringStatus;

};

#ifdef __cplusplus 
}
#endif

#endif /* End of header file FMI2_CAPI_IMPL_H_ */
