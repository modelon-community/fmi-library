/*
Copyright (C) 2012 Modelon AB

This program is free software: you can redistribute it and/or modify
it under the terms of the BSD style license.

the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
FMILIB_License.txt file for more details.

You should have received a copy of the FMILIB_License.txt file
along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#include <string.h>

#if __GNUC__ >= 4
    #pragma GCC visibility push(default)
#endif
/* Standard FMI 2.0 ME and CS types */
#include <FMI2/fmi2TypesPlatform.h>
#include <FMI2/fmi2Functions.h>

#include <fmu_dummy/fmu2_model.h>
#include "config_test.h"

#include "fmu2_model.c"

/* #define MODEL_IDENTIFIER FMU_DUMMY_ME_MODEL_IDENTIFIER*/

/* FMI 2.0 Common Functions */
FMI2_Export const char* fmi2GetVersion()
{
	return fmi_get_version();
}

FMI2_Export fmi2Status fmi2SetDebugLogging(fmi2Component c, fmi2Boolean loggingOn, size_t n , const fmi2String cat[])
{
	return fmi_set_debug_logging(c, loggingOn);
}

FMI2_Export fmi2Component fmi2Instantiate(fmi2String instanceName,
  fmi2Type fmuType, fmi2String GUID, fmi2String location,
  const fmi2CallbackFunctions* functions, fmi2Boolean visible,
  fmi2Boolean loggingOn)
{
    return fmi_instantiate(instanceName, fmuType, GUID, location, functions,
                           visible, loggingOn);
}

FMI2_Export void fmi2FreeInstance(fmi2Component c)
{
	fmi_free_instance(c);
}

FMI2_Export fmi2Status fmi2SetupExperiment(fmi2Component c, 
    fmi2Boolean toleranceDefined, fmi2Real tolerance,
    fmi2Real startTime, fmi2Boolean stopTimeDefined,
    fmi2Real stopTime)
{
    return fmi_setup_experiment(c, toleranceDefined, tolerance, startTime,
                                stopTimeDefined, stopTime);
}

FMI2_Export fmi2Status fmi2EnterInitializationMode(fmi2Component c)
{
    return fmi_enter_initialization_mode(c);
}

FMI2_Export fmi2Status fmi2ExitInitializationMode(fmi2Component c)
{
    return fmi_exit_initialization_mode(c);
}

FMI2_Export fmi2Status fmi2GetReal(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Real value[])
{
	return fmi_get_real(c, vr, nvr, value);
}

FMI2_Export fmi2Status fmi2GetInteger(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Integer value[])
{
	return fmi_get_integer(c, vr, nvr, value);
}

FMI2_Export fmi2Status fmi2GetBoolean(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[])
{
	return fmi_get_boolean(c, vr, nvr, value);
}

FMI2_Export fmi2Status fmi2GetString(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2String  value[])
{
	return fmi_get_string(c, vr, nvr, value);
}

FMI2_Export fmi2Status fmi2SetReal(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[])
{
	return fmi_set_real(c, vr, nvr, value);
}

FMI2_Export fmi2Status fmi2SetInteger(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Integer value[])
{
	return fmi_set_integer(c, vr, nvr, value);
}

FMI2_Export fmi2Status fmi2SetBoolean(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[])
{
	return fmi_set_boolean(c, vr, nvr, value);
}

FMI2_Export fmi2Status fmi2SetString(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2String  value[])
{
	return fmi_set_string(c, vr, nvr, value);
}

/* FMI 2.0 ME Functions */
FMI2_Export const char* fmi2GetTypesPlatform()
{
	return fmi_get_model_types_platform();
}

FMI2_Export fmi2Status fmi2EnterEventMode(fmi2Component c)
{
    return fmi_enter_event_mode(c);
}

FMI2_Export fmi2Status fmi2NewDiscreteStates(fmi2Component c, fmi2EventInfo* eventInfo)
{
    return fmi_new_discrete_states(c, eventInfo);
}

FMI2_Export fmi2Status fmi2EnterContinuousTimeMode(fmi2Component c)
{
    return fmi_enter_continuous_time_mode(c);
}

FMI2_Export fmi2Status fmi2SetTime(fmi2Component c, fmi2Real fmitime)
{
	return fmi_set_time(c, fmitime);
}

FMI2_Export fmi2Status fmi2SetContinuousStates(fmi2Component c, const fmi2Real x[], size_t nx)
{
	return fmi_set_continuous_states(c, x, nx);
}

FMI2_Export fmi2Status fmi2CompletedIntegratorStep(fmi2Component c,
    fmi2Boolean noSetFMUStatePriorToCurrentPoint,
    fmi2Boolean* enterEventMode, fmi2Boolean* terminateSimulation)
{
    return fmi_completed_integrator_step(c, noSetFMUStatePriorToCurrentPoint,
                                         enterEventMode, terminateSimulation);
}

FMI2_Export fmi2Status fmi2GetDerivatives(fmi2Component c, fmi2Real derivatives[] , size_t nx)
{
	return fmi_get_derivatives(c, derivatives, nx);
}

FMI2_Export fmi2Status fmi2GetEventIndicators(fmi2Component c, fmi2Real eventIndicators[], size_t ni)
{
	return fmi_get_event_indicators(c, eventIndicators, ni);
}

FMI2_Export fmi2Status fmi2GetContinuousStates(fmi2Component c, fmi2Real states[], size_t nx)
{
	return fmi_get_continuous_states(c, states, nx);
}

FMI2_Export fmi2Status fmi2GetNominalsOfContinuousStates(fmi2Component c, fmi2Real x_nominal[], size_t nx)
{
	return fmi_get_nominals_of_continuousstates(c, x_nominal, nx);
}

FMI2_Export fmi2Status fmi2Terminate(fmi2Component c)
{
	return fmi_terminate(c);
}

FMI2_Export fmi2Status fmi2Reset(fmi2Component c)
{
	return fmi_reset(c);
}
