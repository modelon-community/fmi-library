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

#ifdef __cplusplus 
extern "C" {
#endif
#include <assert.h>
#include <FMI1/fmi1_capi.h>
#include <FMI1/fmi1_capi_impl.h>


fmi1_component_t fmi1_capi_instantiate_model(fmi1_capi_t* fmu, fmi1_string_t instanceName, fmi1_string_t GUID, fmi1_boolean_t loggingOn)
{
	fmi1_me_callback_functions_t cb;
	assert(fmu);
	cb.logger = fmu->callBackFunctions.logger;
    cb.allocateMemory = fmu->callBackFunctions.allocateMemory;
    cb.freeMemory = fmu->callBackFunctions.freeMemory;
	jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiInstantiateModel");
	return fmu->c = fmu->fmiInstantiateModel(instanceName, GUID, cb, loggingOn);
}

void fmi1_capi_free_model_instance(fmi1_capi_t* fmu)
{
	jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiFreeModelInstance");
	fmu->fmiFreeModelInstance(fmu->c);
}

fmi1_status_t fmi1_capi_initialize(fmi1_capi_t* fmu, fmi1_boolean_t toleranceControlled, fmi1_real_t relativeTolerance, fmi1_event_info_t* eventInfo)
{
	assert(fmu);
	jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiInitialize");
	return fmu->fmiInitialize(fmu->c, toleranceControlled, relativeTolerance, eventInfo);	
}

const char* fmi1_capi_get_model_types_platform(fmi1_capi_t* fmu)
{
	assert(fmu);
	jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiGetModelTypesPlatform");
	return fmu->fmiGetModelTypesPlatform();
}

fmi1_status_t fmi1_capi_set_time(fmi1_capi_t* fmu, fmi1_real_t time)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiGetModelTypesPlatform");
	return fmu->fmiSetTime(fmu->c, time);
}

fmi1_status_t fmi1_capi_set_continuous_states(fmi1_capi_t* fmu, const fmi1_real_t x[], size_t nx)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiSetContinuousStates");
	return fmu->fmiSetContinuousStates(fmu->c, x, nx);
}

fmi1_status_t fmi1_capi_completed_integrator_step(fmi1_capi_t* fmu, fmi1_boolean_t* callEventUpdate)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiCompletedIntegratorStep");
	return fmu->fmiCompletedIntegratorStep(fmu->c, callEventUpdate);
}

fmi1_status_t fmi1_capi_get_derivatives(fmi1_capi_t* fmu, fmi1_real_t derivatives[], size_t nx)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiGetDerivatives");
	return fmu->fmiGetDerivatives(fmu->c, derivatives, nx);
}

fmi1_status_t fmi1_capi_get_event_indicators(fmi1_capi_t* fmu, fmi1_real_t eventIndicators[], size_t ni)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiGetEventIndicators");
	return fmu->fmiGetEventIndicators(fmu->c, eventIndicators, ni);
}

fmi1_status_t fmi1_capi_eventUpdate(fmi1_capi_t* fmu, fmi1_boolean_t intermediateResults, fmi1_event_info_t* eventInfo)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiEventUpdate");
	return fmu->fmiEventUpdate(fmu->c, intermediateResults, eventInfo);
}

fmi1_status_t fmi1_capi_get_continuous_states(fmi1_capi_t* fmu, fmi1_real_t states[], size_t nx)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiGetContinuousStates");
	return fmu->fmiGetContinuousStates(fmu->c, states, nx);
}

fmi1_status_t fmi1_capi_get_nominal_continuous_states(fmi1_capi_t* fmu, fmi1_real_t x_nominal[], size_t nx)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiGetNominalContinuousStates");
	return fmu->fmiGetNominalContinuousStates(fmu->c, x_nominal, nx);
}

fmi1_status_t fmi1_capi_get_state_value_references(fmi1_capi_t* fmu, fmi1_value_reference_t vrx[], size_t nx)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiGetStateValueReferences");
	return fmu->fmiGetStateValueReferences(fmu->c, vrx, nx);
}

fmi1_status_t fmi1_capi_terminate(fmi1_capi_t* fmu)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiTerminate");
	return fmu->fmiTerminate(fmu->c);
}

#ifdef __cplusplus
}
#endif
