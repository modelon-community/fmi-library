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
/* Standard FMI 1.0 ME and CS types */
#include <FMI1/fmiModelTypes.h>
#include <FMI1/fmiModelFunctions.h>

#include <fmu_dummy/fmu1_model.h>
#include "config_test.h"

   typedef enum {fmiDoStepStatus,
                 fmiPendingStatus,
                 fmiLastSuccessfulTime} fmiStatusKind;

#include "fmu1_model.c"

#define MODEL_IDENTIFIER FMU_DUMMY_ME_MODEL_IDENTIFIER

/* FMI 1.0 Common Functions */
DllExport const char* fmiGetVersion()
{
	return fmi_get_version();
}

DllExport fmiStatus fmiSetDebugLogging(fmiComponent c, fmiBoolean loggingOn)
{
	return fmi_set_debug_logging(c, loggingOn);
}

DllExport fmiStatus fmiGetReal(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiReal value[])
{
	return fmi_get_real(c, vr, nvr, value);
}

DllExport fmiStatus fmiGetInteger(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiInteger value[])
{
	return fmi_get_integer(c, vr, nvr, value);
}

DllExport fmiStatus fmiGetBoolean(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiBoolean value[])
{
	return fmi_get_boolean(c, vr, nvr, value);
}

DllExport fmiStatus fmiGetString(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiString  value[])
{
	return fmi_get_string(c, vr, nvr, value);
}

DllExport fmiStatus fmiSetReal(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiReal value[])
{
	return fmi_set_real(c, vr, nvr, value);
}

DllExport fmiStatus fmiSetInteger(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiInteger value[])
{
	return fmi_set_integer(c, vr, nvr, value);
}

DllExport fmiStatus fmiSetBoolean(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiBoolean value[])
{
	return fmi_set_boolean(c, vr, nvr, value);
}

DllExport fmiStatus fmiSetString(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiString  value[])
{
	return fmi_set_string(c, vr, nvr, value);
}

/* FMI 1.0 ME Functions */
DllExport const char* fmiGetModelTypesPlatform()
{
	return fmi_get_model_types_platform();
}

DllExport fmiComponent fmiInstantiateModel(fmiString instanceName, fmiString GUID, fmiCallbackFunctions functions, fmiBoolean loggingOn)
{
	return fmi_instantiate_model(instanceName, GUID, functions, loggingOn);
}

DllExport void fmiFreeModelInstance(fmiComponent c)
{
	fmi_free_model_instance(c);
}

DllExport fmiStatus fmiSetTime(fmiComponent c, fmiReal fmitime)
{
	return fmi_set_time(c, fmitime);
}

DllExport fmiStatus fmiSetContinuousStates(fmiComponent c, const fmiReal x[], size_t nx)
{
	return fmi_set_continuous_states(c, x, nx);
}

DllExport fmiStatus fmiCompletedIntegratorStep(fmiComponent c, fmiBoolean* callEventUpdate)
{
	return fmi_completed_integrator_step(c, callEventUpdate);
}

DllExport fmiStatus fmiInitialize(fmiComponent c, fmiBoolean toleranceControlled, fmiReal relativeTolerance, fmiEventInfo* eventInfo)
{
	return fmi_initialize(c, toleranceControlled, relativeTolerance, eventInfo);
}

DllExport fmiStatus fmiGetDerivatives(fmiComponent c, fmiReal derivatives[] , size_t nx)
{
	return fmi_get_derivatives(c, derivatives, nx);
}

DllExport fmiStatus fmiGetEventIndicators(fmiComponent c, fmiReal eventIndicators[], size_t ni)
{
	return fmi_get_event_indicators(c, eventIndicators, ni);
}

DllExport fmiStatus fmiEventUpdate(fmiComponent c, fmiBoolean intermediateResults, fmiEventInfo* eventInfo)
{
	return fmi_event_update(c, intermediateResults, eventInfo);
}

DllExport fmiStatus fmiGetContinuousStates(fmiComponent c, fmiReal states[], size_t nx)
{
	return fmi_get_continuous_states(c, states, nx);
}

DllExport fmiStatus fmiGetNominalContinuousStates(fmiComponent c, fmiReal x_nominal[], size_t nx)
{
	return fmi_get_nominal_continuousstates(c, x_nominal, nx);
}

DllExport fmiStatus fmiGetStateValueReferences(fmiComponent c, fmiValueReference vrx[], size_t nx)
{
	return fmi_get_state_value_references(c, vrx, nx);
}

DllExport fmiStatus fmiTerminate(fmiComponent c)
{
	return fmi_terminate(c);
}
