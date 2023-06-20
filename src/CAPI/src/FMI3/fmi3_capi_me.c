/*
    Copyright (C) 2023 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#include <assert.h>
#include <FMI3/fmi3_capi.h>
#include <FMI3/fmi3_capi_impl.h>

fmi3_status_t fmi3_capi_enter_continuous_time_mode(fmi3_capi_t* fmu)
{
    assert(fmu); assert(fmu->inst);
    jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3EnterContinuousTimeMode");
    return fmu->fmi3EnterContinuousTimeMode(fmu->inst);
}

fmi3_status_t fmi3_capi_set_time(fmi3_capi_t* fmu, fmi3_float64_t time)
{
    assert(fmu);
    jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3SetTime");
    return fmu->fmi3SetTime(fmu->inst, time);
}

fmi3_status_t fmi3_capi_set_continuous_states(fmi3_capi_t* fmu, const fmi3_float64_t x[], size_t nx)
{
    assert(fmu);
    jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3SetContinuousStates");
    return fmu->fmi3SetContinuousStates(fmu->inst, x, nx);
}

fmi3_status_t fmi3_capi_completed_integrator_step(fmi3_capi_t* fmu,
  fmi3_boolean_t noSetFMUStatePriorToCurrentPoint,
  fmi3_boolean_t* enterEventMode, fmi3_boolean_t* terminateSimulation)
{
    assert(fmu);
    jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3CompletedIntegratorStep");
    return fmu->fmi3CompletedIntegratorStep(fmu->inst, noSetFMUStatePriorToCurrentPoint,
                                           enterEventMode, terminateSimulation);
}

fmi3_status_t fmi3_capi_get_continuous_state_derivatives(fmi3_capi_t* fmu, fmi3_float64_t derivatives[], size_t nx)
{
    assert(fmu);
    jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3GetContinuousStateDerivatives");
    return fmu->fmi3GetContinuousStateDerivatives(fmu->inst, derivatives, nx);
}

fmi3_status_t fmi3_capi_get_event_indicators(fmi3_capi_t* fmu, fmi3_float64_t eventIndicators[], size_t ni)
{
    assert(fmu);
    jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3GetEventIndicators");
    return fmu->fmi3GetEventIndicators(fmu->inst, eventIndicators, ni);
}

fmi3_status_t fmi3_capi_get_continuous_states(fmi3_capi_t* fmu, fmi3_float64_t x[], size_t nx)
{
    assert(fmu);
    jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3GetContinuousStates");
    return fmu->fmi3GetContinuousStates(fmu->inst, x, nx);
}

fmi3_status_t fmi3_capi_get_nominals_of_continuous_states(fmi3_capi_t* fmu, fmi3_float64_t nominals[], size_t nx)
{
    assert(fmu);
    jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3GetNominalsOfContinuousStates");
    return fmu->fmi3GetNominalsOfContinuousStates(fmu->inst, nominals, nx);
}

fmi3_status_t fmi3_capi_get_number_of_event_indicators(fmi3_capi_t* fmu, size_t* nz)
{
    assert(fmu);
    jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3GetNumberOfEventIndicators");
    return fmu->fmi3GetNumberOfEventIndicators(fmu->inst, nz);
}

fmi3_status_t fmi3_capi_get_number_of_continuous_states(fmi3_capi_t* fmu, size_t* nx)
{
    assert(fmu);
    jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3GetNumberOfContinuousStates");
    return fmu->fmi3GetNumberOfContinuousStates(fmu->inst, nx);
}
