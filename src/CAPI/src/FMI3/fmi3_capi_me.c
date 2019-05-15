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
#include <FMI3/fmi3_capi.h>
#include <FMI3/fmi3_capi_impl.h>


fmi3_status_t fmi3_capi_enter_event_mode(fmi3_capi_t* fmu)
{
    assert(fmu); assert(fmu->c);
    jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3EnterEventMode");
    return fmu->fmi3EnterEventMode(fmu->c);
}

fmi3_status_t fmi3_capi_new_discrete_states(fmi3_capi_t* fmu, fmi3_event_info_t* eventInfo)
{
    assert(fmu); assert(fmu->c);
    jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3NewDiscreteStates");
    return fmu->fmi3NewDiscreteStates(fmu->c, eventInfo);
}

fmi3_status_t fmi3_capi_enter_continuous_time_mode(fmi3_capi_t* fmu)
{
    assert(fmu); assert(fmu->c);
    jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3EnterContinuousTimeMode");
    return fmu->fmi3EnterContinuousTimeMode(fmu->c);
}

fmi3_status_t fmi3_capi_set_time(fmi3_capi_t* fmu, fmi3_real_t time)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3SetTime");
	return fmu->fmi3SetTime(fmu->c, time);
}

fmi3_status_t fmi3_capi_set_continuous_states(fmi3_capi_t* fmu, const fmi3_real_t x[], size_t nx)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3SetContinuousStates");
	return fmu->fmi3SetContinuousStates(fmu->c, x, nx);
}

fmi3_status_t fmi3_capi_completed_integrator_step(fmi3_capi_t* fmu,
  fmi3_boolean_t noSetFMUStatePriorToCurrentPoint,
  fmi3_boolean_t* enterEventMode, fmi3_boolean_t* terminateSimulation)
{
    assert(fmu);
    jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3CompletedIntegratorStep");
    return fmu->fmi3CompletedIntegratorStep(fmu->c, noSetFMUStatePriorToCurrentPoint,
                                           enterEventMode, terminateSimulation);
}

fmi3_status_t fmi3_capi_get_derivatives(fmi3_capi_t* fmu, fmi3_real_t derivatives[], size_t nx)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3GetDerivatives");
	return fmu->fmi3GetDerivatives(fmu->c, derivatives, nx);
}

fmi3_status_t fmi3_capi_get_event_indicators(fmi3_capi_t* fmu, fmi3_real_t eventIndicators[], size_t ni)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3GetEventIndicators");
	return fmu->fmi3GetEventIndicators(fmu->c, eventIndicators, ni);
}

fmi3_status_t fmi3_capi_get_continuous_states(fmi3_capi_t* fmu, fmi3_real_t states[], size_t nx)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3GetContinuousStates");
	return fmu->fmi3GetContinuousStates(fmu->c, states, nx);
}

fmi3_status_t fmi3_capi_get_nominals_of_continuous_states(fmi3_capi_t* fmu, fmi3_real_t x_nominal[], size_t nx)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3GetNominalsOfContinuousStates");
	return fmu->fmi3GetNominalsOfContinuousStates(fmu->c, x_nominal, nx);
}
