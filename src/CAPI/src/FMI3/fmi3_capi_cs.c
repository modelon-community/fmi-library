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

#include <FMI3/fmi3_capi.h>
#include <FMI3/fmi3_capi_impl.h>


fmi3_status_t fmi3_capi_set_real_input_derivatives(fmi3_capi_t* fmu, const  fmi3_value_reference_t vr[], size_t nvr, const fmi3_integer_t order[], const  fmi3_float64_t value[])  
{
	return fmu->fmi3SetRealInputDerivatives(fmu->c, vr, nvr, order, value);
}

fmi3_status_t fmi3_capi_get_real_output_derivatives(fmi3_capi_t* fmu, const  fmi3_value_reference_t vr[], size_t nvr, const fmi3_integer_t order[], fmi3_float64_t value[])   
{
	return fmu->fmi3GetRealOutputDerivatives(fmu->c, vr, nvr, order, value);
}

fmi3_status_t fmi3_capi_cancel_step(fmi3_capi_t* fmu)   
{
	return fmu->fmi3CancelStep(fmu->c);
}

fmi3_status_t fmi3_capi_do_step(fmi3_capi_t* fmu, fmi3_float64_t currentCommunicationPoint, fmi3_float64_t communicationStepSize, fmi3_boolean_t newStep)
{
	return fmu->fmi3DoStep(fmu->c, currentCommunicationPoint, communicationStepSize, newStep);
}

/* fmiGetStatus* */
#define FMIGETSTATUSX(FNAME1, FNAME2,FSTATUSTYPE) \
fmi3_status_t FNAME1(fmi3_capi_t* fmu, const fmi3_status_kind_t s, FSTATUSTYPE*  value) \
{ \
	return fmu->FNAME2(fmu->c, s, value); \
}

FMIGETSTATUSX(fmi3_capi_get_status,		fmi3GetStatus,		fmi3_status_t)
FMIGETSTATUSX(fmi3_capi_get_real_status,		fmi3GetRealStatus,	fmi3_float64_t)
FMIGETSTATUSX(fmi3_capi_get_integer_status,	fmi3GetIntegerStatus,	fmi3_integer_t)
FMIGETSTATUSX(fmi3_capi_get_boolean_status,	fmi3GetBooleanStatus,	fmi3_boolean_t)
FMIGETSTATUSX(fmi3_capi_get_string_status,		fmi3GetStringStatus,	fmi3_string_t)

#ifdef __cplusplus
}
#endif
