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
#include <FMI3/fmi3Functions.h>

#include <fmu_dummy/fmu3_model.h>
#include "config_test.h"
/* #define MODEL_IDENTIFIER FMU_DUMMY_CS_MODEL_IDENTIFIER */

#include "fmu3_model.c"



/* FMI 2.0 Common Functions */
FMI3_Export const char* fmi3GetVersion()
{
	return fmi_get_version();
}

FMI3_Export fmi3Status fmi3SetDebugLogging(fmi3Component c, fmi3Boolean loggingOn, size_t n, const fmi3String cat[])
{
	return fmi_set_debug_logging(c, loggingOn);
}

FMI3_Export fmi3Component fmi3Instantiate(fmi3String instanceName,
  fmi3Type fmuType, fmi3String instantiationToken, fmi3String location,
  const fmi3CallbackFunctions* functions, fmi3Boolean visible,
  fmi3Boolean loggingOn)
{
    return fmi_instantiate(instanceName, fmuType, instantiationToken, location, functions,
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

/* FMI 2.0 CS Functions */
FMI3_Export const char* fmi3GetTypesPlatform()
{
	return fmi_get_types_platform();
}

FMI3_Export fmi3Status fmi3Terminate(fmi3Component c)
{
	return fmi_terminate(c);
}

FMI3_Export fmi3Status fmi3Reset(fmi3Component c)
{
	return fmi_reset(c);
}

FMI3_Export fmi3Status fmi3SetRealInputDerivatives(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, const fmi3Integer order[], const fmi3Real value[])
{
	return fmi_set_real_input_derivatives(c, vr, nvr, order, value);
}

FMI3_Export fmi3Status fmi3GetRealOutputDerivatives(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, const fmi3Integer order[], fmi3Real value[])
{
	return fmi_get_real_output_derivatives(c, vr, nvr, order, value);
}

FMI3_Export fmi3Status fmi3CancelStep(fmi3Component c)
{
	return fmi_cancel_step(c);
}

FMI3_Export fmi3Status fmi3DoStep(fmi3Component c, fmi3Real currentCommunicationPoint, fmi3Real communicationStepSize, fmi3Boolean newStep)
{
	return fmi_do_step(c, currentCommunicationPoint, communicationStepSize, newStep);
}

FMI3_Export fmi3Status fmi3GetStatus(fmi3Component c, const fmi3StatusKind s, fmi3Status*  value)
{
	return fmi_get_status(c, s, value);
}

FMI3_Export fmi3Status fmi3GetRealStatus(fmi3Component c, const fmi3StatusKind s, fmi3Real*    value)
{
	return fmi_get_real_status(c, s, value);
}

FMI3_Export fmi3Status fmi3GetIntegerStatus(fmi3Component c, const fmi3StatusKind s, fmi3Integer* value)
{
	return fmi_get_integer_status(c, s, value);
}

FMI3_Export fmi3Status fmi3GetBooleanStatus(fmi3Component c, const fmi3StatusKind s, fmi3Boolean* value)
{
	return fmi_get_boolean_status(c, s, value);
}

FMI3_Export fmi3Status fmi3GetStringStatus(fmi3Component c, const fmi3StatusKind s, fmi3String*  value)
{
	return fmi_get_string_status(c, s, value);
}
