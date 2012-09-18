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

#include <FMI2/fmi2_capi.h>
#include <FMI2/fmi2_capi_impl.h>

fmi2_component_t fmi2_capi_instantiate_slave(fmi2_capi_t* fmu, fmi2_string_t instanceName, fmi2_string_t fmuGUID, fmi2_string_t fmuLocation, fmi2_string_t mimeType, fmi2_real_t timeout, fmi2_boolean_t visible, fmi2_boolean_t interactive, fmi2_boolean_t loggingOn)
{
	return fmu->c = fmu->fmiInstantiateSlave(instanceName, fmuGUID, fmuLocation, mimeType, timeout, visible, interactive, fmu->callBackFunctions, loggingOn);
}

void fmi2_capi_free_slave_instance(fmi2_capi_t* fmu)
{
	if(fmu->c) {
		fmu->fmiFreeSlaveInstance(fmu->c);
		fmu->c = 0;
	}
}

fmi2_status_t fmi2_capi_initialize_slave(fmi2_capi_t* fmu, fmi2_real_t tStart, fmi2_boolean_t StopTimeDefined, fmi2_real_t tStop)
{
	return fmu->fmiInitializeSlave(fmu->c, tStart, StopTimeDefined, tStop);
}


const char* fmi2_capi_get_types_platform(fmi2_capi_t* fmu)
{
	return fmu->fmiGetTypesPlatform();
}

fmi2_status_t fmi2_capi_terminate_slave(fmi2_capi_t* fmu)
{
	return fmu->fmiTerminateSlave(fmu->c);
}

fmi2_status_t fmi2_capi_reset_slave(fmi2_capi_t* fmu)
{
	return fmu->fmiResetSlave(fmu->c);
}

fmi2_status_t fmi2_capi_set_real_input_derivatives(fmi2_capi_t* fmu, const  fmi2_value_reference_t vr[], size_t nvr, const fmi2_integer_t order[], const  fmi2_real_t value[])  
{
	return fmu->fmiSetRealInputDerivatives(fmu->c, vr, nvr, order, value);
}

fmi2_status_t fmi2_capi_get_real_output_derivatives(fmi2_capi_t* fmu, const  fmi2_value_reference_t vr[], size_t nvr, const fmi2_integer_t order[], fmi2_real_t value[])   
{
	return fmu->fmiGetRealOutputDerivatives(fmu->c, vr, nvr, order, value);
}

fmi2_status_t fmi2_capi_cancel_step(fmi2_capi_t* fmu)   
{
	return fmu->fmiCancelStep(fmu->c);
}

fmi2_status_t fmi2_capi_do_step(fmi2_capi_t* fmu, fmi2_real_t currentCommunicationPoint, fmi2_real_t communicationStepSize, fmi2_boolean_t newStep)
{
	return fmu->fmiDoStep(fmu->c, currentCommunicationPoint, communicationStepSize, newStep);
}

/* fmiGetStatus* */
#define FMIGETSTATUSX(FNAME1, FNAME2,FSTATUSTYPE) \
fmi2_status_t FNAME1(fmi2_capi_t* fmu, const fmi2_status_kind_t s, FSTATUSTYPE*  value) \
{ \
	return fmu->FNAME2(fmu->c, s, value); \
}

FMIGETSTATUSX(fmi2_capi_get_status,		fmiGetStatus,		fmi2_status_t)
FMIGETSTATUSX(fmi2_capi_get_real_status,		fmiGetRealStatus,	fmi2_real_t)
FMIGETSTATUSX(fmi2_capi_get_integer_status,	fmiGetIntegerStatus,	fmi2_integer_t)
FMIGETSTATUSX(fmi2_capi_get_boolean_status,	fmiGetBooleanStatus,	fmi2_boolean_t)
FMIGETSTATUSX(fmi2_capi_get_string_status,		fmiGetStringStatus,	fmi2_string_t)

#ifdef __cplusplus
}
#endif