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

fmi3_status_t fmi3_capi_enter_step_mode(fmi3_capi_t* fmu)
{
	return fmu->fmi3EnterStepMode(fmu->inst);
}

fmi3_status_t fmi3_capi_get_output_derivatives(
        fmi3_capi_t* fmu,
        const fmi3_value_reference_t valueReferences[],
        size_t nValueReferences,
        const fmi3_int32_t orders[],
        fmi3_float64_t values[],
        size_t nValues)
{
	return fmu->fmi3GetOutputDerivatives(fmu->inst, valueReferences, nValueReferences, orders, values, nValues);
}

fmi3_status_t fmi3_capi_do_step(
        fmi3_capi_t* fmu,
        fmi3_float64_t currentCommunicationPoint,
        fmi3_float64_t communicationStepSize,
        fmi3_boolean_t noSetFMUStatePriorToCurrentPoint,
        fmi3_boolean_t* terminate,
        fmi3_boolean_t* earlyReturn,
        fmi3_float64_t* lastSuccessfulTime)
{
	return fmu->fmi3DoStep(fmu->inst, currentCommunicationPoint, communicationStepSize,
            noSetFMUStatePriorToCurrentPoint, terminate, earlyReturn, lastSuccessfulTime);
}

fmi3_status_t fmi3_capi_activate_model_partition(
        fmi3_capi_t* fmu,
        fmi3_value_reference_t clockReference,
        size_t clockElementIndex,
        fmi3_float64_t activationTime)
{
    return fmu->fmi3ActivateModelPartition(fmu->inst, clockReference, clockElementIndex, activationTime);
}

#ifdef __cplusplus
}
#endif
