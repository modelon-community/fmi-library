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

/* Standard FMI 3.0 ME and CS types */
#include <FMI3/fmi3Functions.h>

#include <fmu_dummy/fmu3_model.h>
#include "config_test.h"
/* #define MODEL_IDENTIFIER FMU_DUMMY_CS_MODEL_IDENTIFIER */

#include "fmu3_model.c"


/* FMI 3.0 Common Functions */
FMI3_Export const char* fmi3GetVersion()
{
	return fmi_get_version();
}

FMI3_Export fmi3Status fmi3SetDebugLogging(fmi3Instance instance, fmi3Boolean loggingOn, size_t n, const fmi3String cat[])
{
	return fmi_set_debug_logging(instance, loggingOn);
}

/* Not supported for CS */
FMI3_Export fmi3Instance fmi3InstantiateModelExchange(
        fmi3String              instanceName,
        fmi3String              instantiationToken,
        fmi3String              resourceLocation,
        fmi3Boolean             visible,
        fmi3Boolean             loggingOn,
        fmi3InstanceEnvironment instanceEnvironment,
        fmi3CallbackLogMessage  logMessage)
{
    return NULL;
}

FMI3_Export fmi3Instance fmi3InstantiateBasicCoSimulation(
    fmi3String                     instanceName,
    fmi3String                     instantiationToken,
    fmi3String                     resourceLocation,
    fmi3Boolean                    visible,
    fmi3Boolean                    loggingOn,
    fmi3Boolean                    intermediateVariableGetRequired,
    fmi3Boolean                    intermediateInternalVariableGetRequired,
    fmi3Boolean                    intermediateVariableSetRequired,
    fmi3InstanceEnvironment        instanceEnvironment,
    fmi3CallbackLogMessage         logMessage,
    fmi3CallbackIntermediateUpdate intermediateUpdate)
{
    return fmi_instantiate(fmu_type_bcs,
                           instanceName,
                           instantiationToken,
                           resourceLocation,
                           visible,
                           loggingOn,
                           intermediateVariableGetRequired,
                           intermediateInternalVariableGetRequired,
                           intermediateVariableSetRequired,
                           instanceEnvironment,
                           logMessage,
                           intermediateUpdate,
                           NULL,  /* SCS only */
                           NULL); /* SCS only */
}

FMI3_Export fmi3Instance fmi3InstantiateHybridCoSimulation(
    fmi3String                     instanceName,
    fmi3String                     instantiationToken,
    fmi3String                     resourceLocation,
    fmi3Boolean                    visible,
    fmi3Boolean                    loggingOn,
    fmi3Boolean                    intermediateVariableGetRequired,
    fmi3Boolean                    intermediateInternalVariableGetRequired,
    fmi3Boolean                    intermediateVariableSetRequired,
    fmi3InstanceEnvironment        instanceEnvironment,
    fmi3CallbackLogMessage         logMessage,
    fmi3CallbackIntermediateUpdate intermediateUpdate)
{
    return fmi_instantiate(fmu_type_hcs,
                           instanceName,
                           instantiationToken,
                           resourceLocation,
                           visible,
                           loggingOn,
                           intermediateVariableGetRequired,
                           intermediateInternalVariableGetRequired,
                           intermediateVariableSetRequired,
                           instanceEnvironment,
                           logMessage,
                           intermediateUpdate,
                           NULL,  /* SCS only */
                           NULL); /* SCS only */
}

FMI3_Export fmi3Instance fmi3InstantiateScheduledCoSimulation(
    fmi3String                     instanceName,
    fmi3String                     instantiationToken,
    fmi3String                     resourceLocation,
    fmi3Boolean                    visible,
    fmi3Boolean                    loggingOn,
    fmi3Boolean                    intermediateVariableGetRequired,
    fmi3Boolean                    intermediateInternalVariableGetRequired,
    fmi3Boolean                    intermediateVariableSetRequired,
    fmi3InstanceEnvironment        instanceEnvironment,
    fmi3CallbackLogMessage         logMessage,
    fmi3CallbackIntermediateUpdate intermediateUpdate,
    fmi3CallbackLockPreemption     lockPreemption,
    fmi3CallbackUnlockPreemption   unlockPreemption)
{
    return fmi_instantiate(fmu_type_hcs,
                           instanceName,
                           instantiationToken,
                           resourceLocation,
                           visible,
                           loggingOn,
                           intermediateVariableGetRequired,
                           intermediateInternalVariableGetRequired,
                           intermediateVariableSetRequired,
                           instanceEnvironment,
                           logMessage,
                           intermediateUpdate,
                           lockPreemption,
                           unlockPreemption);
}

FMI3_Export void fmi3FreeInstance(fmi3Instance instance)
{
	fmi_free_instance(instance);
}

FMI3_Export fmi3Status fmi3EnterInitializationMode(
        fmi3Instance instance,
        fmi3Boolean  toleranceDefined,
        fmi3Float64  tolerance,
        fmi3Float64  startTime,
        fmi3Boolean  stopTimeDefined,
        fmi3Float64  stopTime)
{
    return fmi_enter_initialization_mode(instance, toleranceDefined, toleranceDefined,
            startTime, stopTimeDefined, stopTimeDefined);
}

FMI3_Export fmi3Status fmi3EnterEventMode(
        fmi3Instance    instance,
        fmi3Boolean     inputEvent,
        fmi3Boolean     stepEvent,
        const fmi3Int32 rootsFound[],
        size_t          nEventIndicators,
        fmi3Boolean     timeEvent)
{
    return fmi_enter_event_mode(instance, inputEvent, stepEvent, rootsFound, nEventIndicators, timeEvent);
}

FMI3_Export fmi3Status fmi3ExitInitializationMode(fmi3Instance instance)
{
    return fmi_exit_initialization_mode(instance);
}

FMI3_Export fmi3Status fmi3GetFloat64(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3Float64 value[], size_t nValues)
{
	return fmi_get_float64(instance, vr, nvr, value, nValues);
}

FMI3_Export fmi3Status fmi3GetFloat32(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3Float32 value[], size_t nValues)
{
	return fmi3Error; /* NYI, need to impl. real -> floatXX parametrization first */
}

FMI3_Export fmi3Status fmi3GetInt64(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3Int64 value[], size_t nValues)
{
	return fmi_get_int64(instance, vr, nvr, value, nValues);
}

FMI3_Export fmi3Status fmi3GetInt32(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3Int32 value[], size_t nValues)
{
	return fmi_get_int32(instance, vr, nvr, value, nValues);
}

FMI3_Export fmi3Status fmi3GetInt16(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3Int16 value[], size_t nValues)
{
	return fmi_get_int16(instance, vr, nvr, value, nValues);
}

FMI3_Export fmi3Status fmi3GetInt8(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3Int8 value[], size_t nValues)
{
	return fmi_get_int8(instance, vr, nvr, value, nValues);
}

FMI3_Export fmi3Status fmi3GetUInt64(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3UInt64 value[], size_t nValues)
{
	return fmi_get_uint64(instance, vr, nvr, value, nValues);
}

FMI3_Export fmi3Status fmi3GetUInt32(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3UInt32 value[], size_t nValues)
{
	return fmi_get_uint32(instance, vr, nvr, value, nValues);
}

FMI3_Export fmi3Status fmi3GetUInt16(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3UInt16 value[], size_t nValues)
{
	return fmi_get_uint16(instance, vr, nvr, value, nValues);
}

FMI3_Export fmi3Status fmi3GetUInt8(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3UInt8 value[], size_t nValues)
{
	return fmi_get_uint8(instance, vr, nvr, value, nValues);
}

FMI3_Export fmi3Status fmi3GetBoolean(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr,
        fmi3Boolean value[], size_t nValues)
{
	return fmi_get_boolean(instance, vr, nvr, value, nValues);
}

FMI3_Export fmi3Status fmi3GetString(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr,
        fmi3String value[], size_t nValues)
{
	return fmi_get_string(instance, vr, nvr, value, nValues);
}

FMI3_Export fmi3Status fmi3GetBinary(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr,
        size_t sizes[], fmi3Binary value[], size_t nValues)
{
	return fmi_get_binary(instance, vr, nvr, sizes, value, nValues);
}

FMI3_Export fmi3Status fmi3SetFloat64(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3Float64 value[], size_t nValues)
{
	return fmi_set_float64(instance, vr, nvr, value, nValues);
}

FMI3_Export fmi3Status fmi3SetFloat32(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3Float32 value[], size_t nValues)
{
	return fmi3Error; /* NYI, need to impl. real -> floatXX parametrization first */
}

FMI3_Export fmi3Status fmi3SetInt64(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3Int64 value[], size_t nValues)
{
	return fmi_set_int64(instance, vr, nvr, value, nValues);
}

FMI3_Export fmi3Status fmi3SetInt32(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3Int32 value[], size_t nValues)
{
	return fmi_set_int32(instance, vr, nvr, value, nValues);
}

FMI3_Export fmi3Status fmi3SetInt16(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3Int16 value[], size_t nValues)
{
	return fmi_set_int16(instance, vr, nvr, value, nValues);
}

FMI3_Export fmi3Status fmi3SetInt8(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3Int8 value[], size_t nValues)
{
	return fmi_set_int8(instance, vr, nvr, value, nValues);
}

FMI3_Export fmi3Status fmi3SetUInt64(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3UInt64 value[], size_t nValues)
{
	return fmi_set_uint64(instance, vr, nvr, value, nValues);
}

FMI3_Export fmi3Status fmi3SetUInt32(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3UInt32 value[], size_t nValues)
{
	return fmi_set_uint32(instance, vr, nvr, value, nValues);
}

FMI3_Export fmi3Status fmi3SetUInt16(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3UInt16 value[], size_t nValues)
{
	return fmi_set_uint16(instance, vr, nvr, value, nValues);
}

FMI3_Export fmi3Status fmi3SetUInt8(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3UInt8 value[], size_t nValues)
{
	return fmi_set_uint8(instance, vr, nvr, value, nValues);
}

FMI3_Export fmi3Status fmi3SetBoolean(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr,
        const fmi3Boolean value[], size_t nValues)
{
	return fmi_set_boolean(instance, vr, nvr, value, nValues);
}

FMI3_Export fmi3Status fmi3SetString(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr,
        const fmi3String value[], size_t nValues)
{
	return fmi_set_string(instance, vr, nvr, value, nValues);
}

FMI3_Export fmi3Status fmi3SetBinary(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr,
        const size_t sizes[], const fmi3Binary value[], size_t nValues)
{
	return fmi_set_binary(instance, vr, nvr, sizes, value, nValues);
}

/* Getting Variable Dependency Information */
FMI3_Export fmi3Status fmi3GetNumberOfVariableDependencies(fmi3Instance instance,
                                                           fmi3ValueReference valueReference,
                                                           size_t* nDependencies)
{
    return fmi_get_number_of_variable_dependencies(instance, valueReference, nDependencies);
}

FMI3_Export fmi3Status fmi3GetVariableDependencies(fmi3Instance instance,
                                                   fmi3ValueReference dependent,
                                                   size_t elementIndicesOfDependent[],
                                                   fmi3ValueReference independents[],
                                                   size_t elementIndicesOfIndependents[],
                                                   fmi3DependencyKind dependencyKinds[],
                                                   size_t nDependencies)
{
    return fmi_get_variable_dependencies(instance, dependent, elementIndicesOfDependent, independents,
            elementIndicesOfIndependents, dependencyKinds, nDependencies);
}

/* Getting and setting the internal FMU state */
FMI3_Export fmi3Status fmi3GetFMUState(fmi3Instance instance, fmi3FMUState* FMUState)
{
    return fmi_get_FMU_state(instance, FMUState);
}

FMI3_Export fmi3Status fmi3SetFMUState(fmi3Instance instance, fmi3FMUState FMUState)
{
    return fmi_set_FMU_state(instance, FMUState);
}

FMI3_Export fmi3Status fmi3FreeFMUState(fmi3Instance instance, fmi3FMUState* FMUState)
{
    return fmi_free_FMU_state(instance, FMUState);
}

FMI3_Export fmi3Status fmi3SerializedFMUStateSize(fmi3Instance instance,
                                                  fmi3FMUState FMUState,
                                                  size_t* size)
{
    return fmi_serialized_FMU_state_size(instance, FMUState, size);
}

FMI3_Export fmi3Status fmi3SerializeFMUState(fmi3Instance instance,
                                             fmi3FMUState FMUState,
                                             fmi3Byte serializedState[],
                                             size_t size)
{
    return fmi_serialize_FMU_state(instance, FMUState, serializedState, size);
}

FMI3_Export fmi3Status fmi3DeSerializeFMUState(fmi3Instance instance,
                                               const fmi3Byte serializedState[],
                                               size_t size,
                                               fmi3FMUState* FMUState)
{
    return fmi_de_serialize_FMU_state(instance, serializedState, size, FMUState);
}

FMI3_Export fmi3Status fmi3GetDirectionalDerivative(
        fmi3Instance instance,
        const fmi3ValueReference unknowns[],
        size_t nUnknowns,
        const fmi3ValueReference knowns[],
        size_t nKnowns,
        const fmi3Float64 seed[],
        size_t nSeed,
        fmi3Float64 sensitivity[],
        size_t nSensitivity)
{
	return fmi_get_directional_derivative(instance, unknowns, nUnknowns, knowns, nKnowns, seed, nSeed,
            sensitivity, nSensitivity);
}

FMI3_Export fmi3Status fmi3GetAdjointDerivative(
        fmi3Instance instance,
        const fmi3ValueReference unknowns[],
        size_t nUnknowns,
        const fmi3ValueReference knowns[],
        size_t nKnowns,
        const fmi3Float64 seed[],
        size_t nSeed,
        fmi3Float64 sensitivity[],
        size_t nSensitivity)
{
	return fmi_get_adjoint_derivative(instance, unknowns, nUnknowns, knowns, nKnowns, seed, nSeed,
            sensitivity, nSensitivity);
}

FMI3_Export fmi3Status fmi3EnterConfigurationMode(fmi3Instance instance)
{
	return fmi_enter_configuration_mode(instance);
}

FMI3_Export fmi3Status fmi3ExitConfigurationMode(fmi3Instance instance)
{
	return fmi_exit_configuration_mode(instance);
}

/* Clock related functions */
FMI3_Export fmi3Status fmi3GetClock(
        fmi3Instance instance,
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3Clock values[],
        size_t nValues)
{
	return fmi_get_clock(instance, valueReferences, nValueReferences, values, nValues);
}

FMI3_Export fmi3Status fmi3SetClock(
        fmi3Instance instance,
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3Clock values[],
        const fmi3Boolean subactive[],
        size_t nValues)
{
	return fmi_set_clock(instance, valueReferences, nValueReferences, values, subactive, nValues);
}

FMI3_Export fmi3Status fmi3GetIntervalDecimal(
        fmi3Instance instance,
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3Float64 interval[],
        size_t nValues)
{
	return fmi_get_interval_decimal(instance, valueReferences, nValueReferences, interval, nValues);
}

FMI3_Export fmi3Status fmi3GetIntervalFraction(
        fmi3Instance instance,
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3UInt64 intervalCounter[],
        fmi3UInt64 resolution[],
        size_t nValues)
{
	return fmi_get_interval_fraction(instance, valueReferences, nValueReferences, intervalCounter, resolution, nValues);
}

FMI3_Export fmi3Status fmi3SetIntervalDecimal(
        fmi3Instance instance,
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3Float64 interval[],
        size_t nValues)
{
	return fmi_set_interval_decimal(instance, valueReferences, nValueReferences, interval, nValues);
}

FMI3_Export fmi3Status fmi3SetIntervalFraction(
        fmi3Instance instance,
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3UInt64 intervalCounter[],
        const fmi3UInt64 resolution[],
        size_t nValues)
{
	return fmi_set_interval_fraction(instance, valueReferences, nValueReferences, intervalCounter, resolution, nValues);
}

FMI3_Export fmi3Status fmi3NewDiscreteStates(
        fmi3Instance instance,
        fmi3Boolean *newDiscreteStatesNeeded,
        fmi3Boolean *terminateSimulation,
        fmi3Boolean *nominalsOfContinuousStatesChanged,
        fmi3Boolean *valuesOfContinuousStatesChanged,
        fmi3Boolean *nextEventTimeDefined,
        fmi3Float64 *nextEventTime)
{
	return fmi_new_discrete_states(instance, newDiscreteStatesNeeded, terminateSimulation,
            nominalsOfContinuousStatesChanged, valuesOfContinuousStatesChanged, nextEventTimeDefined,
            nextEventTime);
}

/* FMI 3.0 CS Functions */
FMI3_Export fmi3Status fmi3EnterStepMode(fmi3Instance instance)
{
	return fmi_enter_step_mode(instance);
}

FMI3_Export fmi3Status fmi3Terminate(fmi3Instance instance)
{
	return fmi_terminate(instance);
}

FMI3_Export fmi3Status fmi3Reset(fmi3Instance instance)
{
	return fmi_reset(instance);
}

FMI3_Export fmi3Status fmi3GetOutputDerivatives(fmi3Instance instance,
                                                const fmi3ValueReference valueReferences[],
                                                size_t nValueReferences,
                                                const fmi3Int32 orders[],
                                                fmi3Float64 values[],
                                                size_t nValues)
{
	return fmi_get_output_derivatives(instance, valueReferences, nValueReferences, orders, values, nValues);
}

FMI3_Export fmi3Status fmi3DoStep(fmi3Instance instance,
                                  fmi3Float64 currentCommunicationPoint,
                                  fmi3Float64 communicationStepSize,
                                  fmi3Boolean noSetFMUStatePriorToCurrentPoint,
                                  fmi3Boolean* terminate,
                                  fmi3Boolean* earlyReturn,
                                  fmi3Float64* lastSuccessfulTime)
{
	return fmi_do_step(instance, currentCommunicationPoint, communicationStepSize, noSetFMUStatePriorToCurrentPoint,
            terminate, earlyReturn, lastSuccessfulTime);
}

FMI3_Export fmi3Status fmi3ActivateModelPartition(fmi3Instance instance,
                                                  fmi3ValueReference clockReference,
                                                  size_t clockElementIndex,
                                                  fmi3Float64 activationTime)
{
    return fmi_activate_model_partition(instance, clockReference, clockElementIndex, activationTime);
}
