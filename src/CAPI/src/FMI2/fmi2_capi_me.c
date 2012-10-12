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

#include <assert.h>
#include <FMI2/fmi2_capi.h>
#include <FMI2/fmi2_capi_impl.h>


fmi2_component_t fmi2_capi_instantiate_model(fmi2_capi_t* fmu, fmi2_string_t instanceName, fmi2_string_t fmuGUID, fmi2_string_t fmuResourceLocation, 
																 fmi2_boolean_t visible, fmi2_boolean_t loggingOn)
{
	assert(fmu);
	jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiInstantiateModel");
	return fmu->c = fmu->fmiInstantiateModel(instanceName, fmuGUID,  fmuResourceLocation, &fmu->callBackFunctions, visible, loggingOn);
}

void fmi2_capi_free_model_instance(fmi2_capi_t* fmu)
{
	jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiFreeModelInstance");
	fmu->fmiFreeModelInstance(fmu->c);
}

fmi2_status_t fmi2_capi_initialize_model(fmi2_capi_t* fmu, fmi2_boolean_t toleranceControlled, fmi2_real_t relativeTolerance, fmi2_event_info_t* eventInfo)
{
	assert(fmu);
	jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiInitialize");
	return fmu->fmiInitializeModel(fmu->c, toleranceControlled, relativeTolerance, eventInfo);	
}


fmi2_status_t fmi2_capi_set_time(fmi2_capi_t* fmu, fmi2_real_t time)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiGetModelTypesPlatform");
	return fmu->fmiSetTime(fmu->c, time);
}

fmi2_status_t fmi2_capi_set_continuous_states(fmi2_capi_t* fmu, const fmi2_real_t x[], size_t nx)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiSetContinuousStates");
	return fmu->fmiSetContinuousStates(fmu->c, x, nx);
}

fmi2_status_t fmi2_capi_completed_integrator_step(fmi2_capi_t* fmu, fmi2_boolean_t* callEventUpdate)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiCompletedIntegratorStep");
	return fmu->fmiCompletedIntegratorStep(fmu->c, callEventUpdate);
}

fmi2_status_t fmi2_capi_get_derivatives(fmi2_capi_t* fmu, fmi2_real_t derivatives[], size_t nx)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiGetDerivatives");
	return fmu->fmiGetDerivatives(fmu->c, derivatives, nx);
}

fmi2_status_t fmi2_capi_get_event_indicators(fmi2_capi_t* fmu, fmi2_real_t eventIndicators[], size_t ni)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiGetEventIndicators");
	return fmu->fmiGetEventIndicators(fmu->c, eventIndicators, ni);
}

fmi2_status_t fmi2_capi_eventUpdate(fmi2_capi_t* fmu, fmi2_boolean_t intermediateResults, fmi2_event_info_t* eventInfo)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiEventUpdate");
	return fmu->fmiEventUpdate(fmu->c, intermediateResults, eventInfo);
}

fmi2_status_t fmi2_capi_completed_event_iteration(fmi2_capi_t* fmu) {
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiCompletedEventIteration");
	return fmu->fmiCompletedEventIteration(fmu->c);
}

fmi2_status_t fmi2_capi_get_continuous_states(fmi2_capi_t* fmu, fmi2_real_t states[], size_t nx)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiGetContinuousStates");
	return fmu->fmiGetContinuousStates(fmu->c, states, nx);
}

fmi2_status_t fmi2_capi_get_nominal_continuous_states(fmi2_capi_t* fmu, fmi2_real_t x_nominal[], size_t nx)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiGetNominalContinuousStates");
	return fmu->fmiGetNominalContinuousStates(fmu->c, x_nominal, nx);
}

fmi2_status_t fmi2_capi_get_state_value_references(fmi2_capi_t* fmu, fmi2_value_reference_t vrx[], size_t nx)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiGetStateValueReferences");
	return fmu->fmiGetStateValueReferences(fmu->c, vrx, nx);
}

fmi2_status_t fmi2_capi_terminate(fmi2_capi_t* fmu)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiTerminate");
	return fmu->fmiTerminate(fmu->c);
}
