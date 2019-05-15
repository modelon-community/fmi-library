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
#include <FMI3/fmi3TypesPlatform.h>
#include <FMI3/fmi3Functions.h>

#include <fmu_dummy/fmu3_model.h>
#include "config_test.h"

#include "fmu3_model.c"

/* #define MODEL_IDENTIFIER FMU_DUMMY_ME_MODEL_IDENTIFIER*/

/* FMI 2.0 Common Functions */
FMI3_Export const char* fmi3GetVersion()
{
	return fmi_get_version();
}

FMI3_Export fmi3Status fmi3SetDebugLogging(fmi3Component c, fmi3Boolean loggingOn, size_t n , const fmi3String cat[])
{
	return fmi_set_debug_logging(c, loggingOn);
}

FMI3_Export fmi3Component fmi3Instantiate(fmi3String instanceName,
  fmi3Type fmuType, fmi3String GUID, fmi3String location,
  const fmi3CallbackFunctions* functions, fmi3Boolean visible,
  fmi3Boolean loggingOn)
{
    return fmi_instantiate(instanceName, fmuType, GUID, location, functions,
                           visible, loggingOn);
}

FMI3_Export void fmi3FreeInstance(fmi3Component c)
{
	fmi_free_instance(c);
}

FMI3_Export fmi3Status fmi3SetupExperiment(fmi3Component c, 
    fmi3Boolean toleranceDefined, fmi3Real tolerance,
    fmi3Real startTime, fmi3Boolean stopTimeDefined,
    fmi3Real stopTime)
{
    return fmi_setup_experiment(c, toleranceDefined, tolerance, startTime,
                                stopTimeDefined, stopTime);
}

FMI3_Export fmi3Status fmi3EnterInitializationMode(fmi3Component c)
{
    return fmi_enter_initialization_mode(c);
}

FMI3_Export fmi3Status fmi3ExitInitializationMode(fmi3Component c)
{
    return fmi_exit_initialization_mode(c);
}

FMI3_Export fmi3Status fmi3GetReal(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, fmi3Real value[])
{
	return fmi_get_real(c, vr, nvr, value);
}

FMI3_Export fmi3Status fmi3GetInteger(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, fmi3Integer value[])
{
	return fmi_get_integer(c, vr, nvr, value);
}

FMI3_Export fmi3Status fmi3GetBoolean(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, fmi3Boolean value[])
{
	return fmi_get_boolean(c, vr, nvr, value);
}

FMI3_Export fmi3Status fmi3GetString(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, fmi3String  value[])
{
	return fmi_get_string(c, vr, nvr, value);
}

FMI3_Export fmi3Status fmi3SetReal(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, const fmi3Real value[])
{
	return fmi_set_real(c, vr, nvr, value);
}

FMI3_Export fmi3Status fmi3SetInteger(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, const fmi3Integer value[])
{
	return fmi_set_integer(c, vr, nvr, value);
}

FMI3_Export fmi3Status fmi3SetBoolean(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, const fmi3Boolean value[])
{
	return fmi_set_boolean(c, vr, nvr, value);
}

FMI3_Export fmi3Status fmi3SetString(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, const fmi3String  value[])
{
	return fmi_set_string(c, vr, nvr, value);
}

/* FMI 2.0 ME Functions */
FMI3_Export const char* fmi3GetTypesPlatform()
{
	return fmi_get_model_types_platform();
}

FMI3_Export fmi3Status fmi3EnterEventMode(fmi3Component c)
{
    return fmi_enter_event_mode(c);
}

FMI3_Export fmi3Status fmi3NewDiscreteStates(fmi3Component c, fmi3EventInfo* eventInfo)
{
    return fmi_new_discrete_states(c, eventInfo);
}

FMI3_Export fmi3Status fmi3EnterContinuousTimeMode(fmi3Component c)
{
    return fmi_enter_continuous_time_mode(c);
}

FMI3_Export fmi3Status fmi3SetTime(fmi3Component c, fmi3Real fmitime)
{
	return fmi_set_time(c, fmitime);
}

FMI3_Export fmi3Status fmi3SetContinuousStates(fmi3Component c, const fmi3Real x[], size_t nx)
{
	return fmi_set_continuous_states(c, x, nx);
}

FMI3_Export fmi3Status fmi3CompletedIntegratorStep(fmi3Component c,
    fmi3Boolean noSetFMUStatePriorToCurrentPoint,
    fmi3Boolean* enterEventMode, fmi3Boolean* terminateSimulation)
{
    return fmi_completed_integrator_step(c, noSetFMUStatePriorToCurrentPoint,
                                         enterEventMode, terminateSimulation);
}

FMI3_Export fmi3Status fmi3GetDerivatives(fmi3Component c, fmi3Real derivatives[] , size_t nx)
{
	return fmi_get_derivatives(c, derivatives, nx);
}

FMI3_Export fmi3Status fmi3GetEventIndicators(fmi3Component c, fmi3Real eventIndicators[], size_t ni)
{
	return fmi_get_event_indicators(c, eventIndicators, ni);
}

FMI3_Export fmi3Status fmi3GetContinuousStates(fmi3Component c, fmi3Real states[], size_t nx)
{
	return fmi_get_continuous_states(c, states, nx);
}

FMI3_Export fmi3Status fmi3GetNominalsOfContinuousStates(fmi3Component c, fmi3Real x_nominal[], size_t nx)
{
	return fmi_get_nominals_of_continuousstates(c, x_nominal, nx);
}

FMI3_Export fmi3Status fmi3Terminate(fmi3Component c)
{
	return fmi_terminate(c);
}

FMI3_Export fmi3Status fmi3Reset(fmi3Component c)
{
	return fmi_reset(c);
}
