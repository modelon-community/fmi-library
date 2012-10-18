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
#include <FMI2/fmiTypesPlatform.h>
#include <FMI2/fmiFunctions.h>

#include <fmu_dummy/fmu2_model.h>
#include "config_test.h"

 #include "fmu2_model.c"

/* #define MODEL_IDENTIFIER FMU_DUMMY_ME_MODEL_IDENTIFIER*/

/* FMI 2.0 Common Functions */
FMIAPI const char* fmiGetVersion()
{
	return fmi_get_version();
}

FMIAPI fmiStatus fmiSetDebugLogging(fmiComponent c, fmiBoolean loggingOn, size_t n , const fmiString cat[])
{
	return fmi_set_debug_logging(c, loggingOn);
}

FMIAPI fmiStatus fmiGetReal(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiReal value[])
{
	return fmi_get_real(c, vr, nvr, value);
}

FMIAPI fmiStatus fmiGetInteger(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiInteger value[])
{
	return fmi_get_integer(c, vr, nvr, value);
}

FMIAPI fmiStatus fmiGetBoolean(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiBoolean value[])
{
	return fmi_get_boolean(c, vr, nvr, value);
}

FMIAPI fmiStatus fmiGetString(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiString  value[])
{
	return fmi_get_string(c, vr, nvr, value);
}

FMIAPI fmiStatus fmiSetReal(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiReal value[])
{
	return fmi_set_real(c, vr, nvr, value);
}

FMIAPI fmiStatus fmiSetInteger(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiInteger value[])
{
	return fmi_set_integer(c, vr, nvr, value);
}

FMIAPI fmiStatus fmiSetBoolean(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiBoolean value[])
{
	return fmi_set_boolean(c, vr, nvr, value);
}

FMIAPI fmiStatus fmiSetString(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiString  value[])
{
	return fmi_set_string(c, vr, nvr, value);
}

/* FMI 2.0 ME Functions */
FMIAPI const char* fmiGetTypesPlatform()
{
	return fmi_get_model_types_platform();
}

FMIAPI fmiComponent fmiInstantiateModel(fmiString instanceName, fmiString GUID, fmiString location, const fmiCallbackFunctions* functions, fmiBoolean visible, fmiBoolean loggingOn)
{
	return fmi_instantiate_model(instanceName, GUID, location, functions, visible, loggingOn);
}

FMIAPI void fmiFreeModelInstance(fmiComponent c)
{
	fmi_free_model_instance(c);
}

FMIAPI fmiStatus fmiSetTime(fmiComponent c, fmiReal fmitime)
{
	return fmi_set_time(c, fmitime);
}

FMIAPI fmiStatus fmiSetContinuousStates(fmiComponent c, const fmiReal x[], size_t nx)
{
	return fmi_set_continuous_states(c, x, nx);
}

FMIAPI fmiStatus fmiCompletedIntegratorStep(fmiComponent c, fmiBoolean* callEventUpdate)
{
	return fmi_completed_integrator_step(c, callEventUpdate);
}

FMIAPI fmiStatus fmiInitializeModel(fmiComponent c, fmiBoolean toleranceControlled, fmiReal relativeTolerance, fmiEventInfo* eventInfo)
{
	return fmi_initialize(c, toleranceControlled, relativeTolerance, eventInfo);
}

FMIAPI fmiStatus fmiGetDerivatives(fmiComponent c, fmiReal derivatives[] , size_t nx)
{
	return fmi_get_derivatives(c, derivatives, nx);
}

FMIAPI fmiStatus fmiGetEventIndicators(fmiComponent c, fmiReal eventIndicators[], size_t ni)
{
	return fmi_get_event_indicators(c, eventIndicators, ni);
}

FMIAPI fmiStatus fmiEventUpdate(fmiComponent c, fmiBoolean intermediateResults, fmiEventInfo* eventInfo)
{
	return fmi_event_update(c, intermediateResults, eventInfo);
}

FMIAPI fmiStatus fmiCompletedEventIteration(fmiComponent c) {
	if(c) return fmiOK;
	return fmiError;
}

FMIAPI fmiStatus fmiGetContinuousStates(fmiComponent c, fmiReal states[], size_t nx)
{
	return fmi_get_continuous_states(c, states, nx);
}

FMIAPI fmiStatus fmiGetNominalContinuousStates(fmiComponent c, fmiReal x_nominal[], size_t nx)
{
	return fmi_get_nominal_continuousstates(c, x_nominal, nx);
}

FMIAPI fmiStatus fmiGetStateValueReferences(fmiComponent c, fmiValueReference vrx[], size_t nx)
{
	return fmi_get_state_value_references(c, vrx, nx);
}

FMIAPI fmiStatus fmiTerminate(fmiComponent c)
{
	return fmi_terminate(c);
}
