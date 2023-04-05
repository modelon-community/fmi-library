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

fmi3_status_t fmi3_capi_activate_model_partition(
        fmi3_capi_t* fmu,
        fmi3_value_reference_t clockReference,
        fmi3_float64_t activationTime)
{
    return fmu->fmi3ActivateModelPartition(fmu->inst, clockReference, activationTime);
}

#ifdef __cplusplus
}
#endif
