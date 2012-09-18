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

	/* FMI common */
	fmi2_get_version_ft					fmiGetVersion;
	fmi2_set_debug_logging_ft			fmiSetDebugLogging;
    fmi2_set_real_ft					fmiSetReal;
    fmi2_set_integer_ft					fmiSetInteger;
    fmi2_set_boolean_ft					fmiSetBoolean;
    fmi2_set_string_ft					fmiSetString;
	fmi2_get_real_ft					fmiGetReal;
    fmi2_get_integer_ft					fmiGetInteger;
    fmi2_get_boolean_ft					fmiGetBoolean;
    fmi2_get_string_ft					fmiGetString;

	/* FMI ME */
    fmi2_get_model_typesPlatform_ft		fmiGetModelTypesPlatform;    
    fmi2_instantiate_model_ft			fmiInstantiateModel;
    fmi2_free_model_instance_ft			fmiFreeModelInstance;    
    fmi2_set_time_ft					fmiSetTime;
    fmi2_set_continuous_states_ft		fmiSetContinuousStates;
    fmi2_completed_integrator_step_ft	fmiCompletedIntegratorStep;
    fmi2_initialize_ft			 		fmiInitialize;
    fmi2_get_derivatives_ft				fmiGetDerivatives;
    fmi2_get_event_indicators_ft		fmiGetEventIndicators;
    fmi2_event_update_ft				fmiEventUpdate;
    fmi2_get_continuous_states_ft		fmiGetContinuousStates;
    fmi2_get_nominal_continuousStates_ft fmiGetNominalContinuousStates;
    fmi2_get_state_valueReferences_ft	fmiGetStateValueReferences;
    fmi2_terminate_ft					fmiTerminate;

	/* FMI CS */
	fmi2_get_types_platform_ft			fmiGetTypesPlatform;   
    fmi2_instantiate_slave_ft			fmiInstantiateSlave;
    fmi2_initialize_slave_ft			fmiInitializeSlave;
    fmi2_terminate_slave_ft				fmiTerminateSlave;
    fmi2_reset_slave_ft					fmiResetSlave;
    fmi2_free_slave_instance_ft			fmiFreeSlaveInstance;
    fmi2_set_real_inputDerivatives_ft	fmiSetRealInputDerivatives;
    fmi2_get_real_outputDerivatives_ft	fmiGetRealOutputDerivatives;
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
