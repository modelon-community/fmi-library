/*
    Copyright (C) 2023 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#include <string.h>

#if __GNUC__ >= 4
    #pragma GCC visibility push(default)
#endif
/* Standard FMI 3.0 */
#include <FMI3/fmi3PlatformTypes.h>
#include <FMI3/fmi3Functions.h>

#include <fmu_dummy/fmu3_model.h>
#include "config_test.h"

#include "fmu3_model.c"

/* #define MODEL_IDENTIFIER FMU_DUMMY_ME_MODEL_IDENTIFIER*/

/* FMI 3.0 Common Functions */
FMI3_Export const char* fmi3GetVersion()
{
    return fmi_get_version();
}

FMI3_Export fmi3Status fmi3SetDebugLogging(fmi3Instance instance, fmi3Boolean loggingOn, size_t n , const fmi3String cat[])
{
    return fmi_set_debug_logging(instance, loggingOn);
}

FMI3_Export fmi3Instance fmi3InstantiateModelExchange(
        fmi3String              instanceName,
        fmi3String              instantiationToken,
        fmi3String              resourcePath,
        fmi3Boolean             visible,
        fmi3Boolean             loggingOn,
        fmi3InstanceEnvironment instanceEnvironment,
        fmi3LogMessageCallback  logMessage)
{
    // For testing:
    char msg[4096] = "TEST_RESOURCE_PATH=";
    strcpy(msg + strlen(msg), resourcePath);
    logMessage(instanceEnvironment, fmi3OK, "Debug", msg);

    return fmi_instantiate(fmu_type_me,
                           instanceName,
                           instantiationToken,
                           resourcePath,
                           visible,
                           loggingOn,
                           fmi3False, /* eventModeUsed:                  CS only */
                           fmi3False, /* earlyReturnAllowed:             CS only */
                           fmi3False, /* requiredIntermediateVariables:  CS only */
                           0,         /* nRequiredIntermediateVariables: CS only */
                           instanceEnvironment,
                           logMessage,
                           NULL,      /* intermediateUpdate:             CS only */
                           NULL,      /* clockUpdate:                    SE only */
                           NULL,      /* lockPreemption:                 SE only */
                           NULL);     /* unlockPreemption:               SE only */
}

/* Not supported for ME */
FMI3_Export fmi3Instance fmi3InstantiateCoSimulation(
    fmi3String                     instanceName,
    fmi3String                     instantiationToken,
    fmi3String                     resourcePath,
    fmi3Boolean                    visible,
    fmi3Boolean                    loggingOn,
    fmi3Boolean                    eventModeUsed,
    fmi3Boolean                    earlyReturnAllowed,
    const fmi3ValueReference       requiredIntermediateVariables[],
    size_t                         nRequiredIntermediateVariables,
    fmi3InstanceEnvironment        instanceEnvironment,
    fmi3LogMessageCallback         logMessage,
    fmi3IntermediateUpdateCallback intermediateUpdate)
{
    return NULL;
}

/* Not supported for ME */
FMI3_Export fmi3Instance fmi3InstantiateScheduledExecution(
    fmi3String                     instanceName,
    fmi3String                     instantiationToken,
    fmi3String                     resourcePath,
    fmi3Boolean                    visible,
    fmi3Boolean                    loggingOn,
    fmi3InstanceEnvironment        instanceEnvironment,
    fmi3LogMessageCallback         logMessage,
    fmi3ClockUpdateCallback        clockUpdate,
    fmi3LockPreemptionCallback     lockPreemption,
    fmi3UnlockPreemptionCallback   unlockPreemption)
{
    return NULL;
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

FMI3_Export fmi3Status fmi3ExitInitializationMode(fmi3Instance instance)
{
    return fmi_exit_initialization_mode(instance);
}

FMI3_Export fmi3Status fmi3EnterEventMode(fmi3Instance instance)
{
    return fmi_enter_event_mode(instance);
}

FMI3_Export fmi3Status fmi3GetFloat64(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Float64 values[], size_t nValues)
{
    return fmi_get_float64(instance, valueReferences, nValueReferences, values, nValues);
}

FMI3_Export fmi3Status fmi3GetFloat32(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Float32 values[], size_t nValues)
{
    return fmi3Error; /* NYI, need to impl. real -> floatXX parametrization first */
}

FMI3_Export fmi3Status fmi3GetInt64(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int64 values[], size_t nValues)
{
    return fmi_get_int64(instance, valueReferences, nValueReferences, values, nValues);
}

FMI3_Export fmi3Status fmi3GetInt32(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int32 values[], size_t nValues)
{
    return fmi_get_int32(instance, valueReferences, nValueReferences, values, nValues);
}

FMI3_Export fmi3Status fmi3GetInt16(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int16 values[], size_t nValues)
{
    return fmi_get_int16(instance, valueReferences, nValueReferences, values, nValues);
}

FMI3_Export fmi3Status fmi3GetInt8(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int8 values[], size_t nValues)
{
    return fmi_get_int8(instance, valueReferences, nValueReferences, values, nValues);
}

FMI3_Export fmi3Status fmi3GetUInt64(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt64 values[], size_t nValues)
{
    return fmi_get_uint64(instance, valueReferences, nValueReferences, values, nValues);
}

FMI3_Export fmi3Status fmi3GetUInt32(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt32 values[], size_t nValues)
{
    return fmi_get_uint32(instance, valueReferences, nValueReferences, values, nValues);
}

FMI3_Export fmi3Status fmi3GetUInt16(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt16 values[], size_t nValues)
{
    return fmi_get_uint16(instance, valueReferences, nValueReferences, values, nValues);
}

FMI3_Export fmi3Status fmi3GetUInt8(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt8 values[], size_t nValues)
{
    return fmi_get_uint8(instance, valueReferences, nValueReferences, values, nValues);
}

FMI3_Export fmi3Status fmi3GetBoolean(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences,
        fmi3Boolean values[], size_t nValues)
{
    return fmi_get_boolean(instance, valueReferences, nValueReferences, values, nValues);
}

FMI3_Export fmi3Status fmi3GetString(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences,
        fmi3String values[], size_t nValues)
{
    return fmi_get_string(instance, valueReferences, nValueReferences, values, nValues);
}

FMI3_Export fmi3Status fmi3GetBinary(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences,
        size_t sizes[], fmi3Binary values[], size_t nValues)
{
    return fmi_get_binary(instance, valueReferences, nValueReferences, sizes, values, nValues);
}

FMI3_Export fmi3Status fmi3SetFloat64(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Float64 values[], size_t nValues)
{
    return fmi_set_float64(instance, valueReferences, nValueReferences, values, nValues);
}

FMI3_Export fmi3Status fmi3SetFloat32(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Float32 values[], size_t nValues)
{
    return fmi3Error; /* NYI, need to impl. real -> floatXX parametrization first */
}

FMI3_Export fmi3Status fmi3SetInt64(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int64 values[], size_t nValues)
{
    return fmi_set_int64(instance, valueReferences, nValueReferences, values, nValues);
}

FMI3_Export fmi3Status fmi3SetInt32(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int32 values[], size_t nValues)
{
    return fmi_set_int32(instance, valueReferences, nValueReferences, values, nValues);
}

FMI3_Export fmi3Status fmi3SetInt16(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int16 values[], size_t nValues)
{
    return fmi_set_int16(instance, valueReferences, nValueReferences, values, nValues);
}

FMI3_Export fmi3Status fmi3SetInt8(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int8 values[], size_t nValues)
{
    return fmi_set_int8(instance, valueReferences, nValueReferences, values, nValues);
}

FMI3_Export fmi3Status fmi3SetUInt64(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt64 values[], size_t nValues)
{
    return fmi_set_uint64(instance, valueReferences, nValueReferences, values, nValues);
}

FMI3_Export fmi3Status fmi3SetUInt32(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt32 values[], size_t nValues)
{
    return fmi_set_uint32(instance, valueReferences, nValueReferences, values, nValues);
}

FMI3_Export fmi3Status fmi3SetUInt16(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt16 values[], size_t nValues)
{
    return fmi_set_uint16(instance, valueReferences, nValueReferences, values, nValues);
}

FMI3_Export fmi3Status fmi3SetUInt8(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt8 values[], size_t nValues)
{
    return fmi_set_uint8(instance, valueReferences, nValueReferences, values, nValues);
}

FMI3_Export fmi3Status fmi3SetBoolean(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences,
        const fmi3Boolean values[], size_t nValues)
{
    return fmi_set_boolean(instance, valueReferences, nValueReferences, values, nValues);
}

FMI3_Export fmi3Status fmi3SetString(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences,
        const fmi3String values[], size_t nValues)
{
    return fmi_set_string(instance, valueReferences, nValueReferences, values, nValues);
}

FMI3_Export fmi3Status fmi3SetBinary(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences,
        const size_t sizes[], const fmi3Binary values[], size_t nValues)
{
    return fmi_set_binary(instance, valueReferences, nValueReferences, sizes, values, nValues);
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

FMI3_Export fmi3Status fmi3DeserializeFMUState(fmi3Instance instance,
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
        fmi3Clock values[])
{
    return fmi_get_clock(instance, valueReferences, nValueReferences, values);
}

FMI3_Export fmi3Status fmi3SetClock(
        fmi3Instance instance,
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3Clock values[])
{
    return fmi_set_clock(instance, valueReferences, nValueReferences, values);
}

FMI3_Export fmi3Status fmi3GetIntervalDecimal(
        fmi3Instance instance,
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3Float64 intervals[],
        fmi3IntervalQualifier qualifiers[])
{
    return fmi_get_interval_decimal(instance, valueReferences, nValueReferences, intervals, qualifiers);
}


FMI3_Export fmi3Status fmi3GetShiftDecimal(
        fmi3Instance instance,
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3Float64 shifts[])
{
    return fmi_get_shift_decimal(instance, valueReferences, nValueReferences, shifts);
}

FMI3_Export fmi3Status fmi3GetShiftFraction(
        fmi3Instance instance,
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3UInt64 counters[],
        fmi3UInt64 resolutions[])
{
    return fmi_get_shift_fraction(instance, valueReferences, nValueReferences, counters, resolutions);
}

FMI3_Export fmi3Status fmi3GetIntervalFraction(
        fmi3Instance instance,
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        fmi3UInt64 counters[],
        fmi3UInt64 resolutions[],
        fmi3IntervalQualifier qualifiers[])
{
    return fmi_get_interval_fraction(
        instance,
        valueReferences,
        nValueReferences,
        counters,
        resolutions,
        qualifiers);
}

FMI3_Export fmi3Status fmi3SetIntervalDecimal(
        fmi3Instance instance,
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3Float64 intervals[])
{
    return fmi_set_interval_decimal(instance, valueReferences, nValueReferences, intervals);
}

FMI3_Export fmi3Status fmi3SetIntervalFraction(
        fmi3Instance instance,
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3UInt64 counters[],
        const fmi3UInt64 resolutions[])
{
    return fmi_set_interval_fraction(instance, valueReferences, nValueReferences, counters, resolutions);
}

FMI3_Export fmi3Status fmi3SetShiftDecimal(
        fmi3Instance instance,
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3Float64 shifts[])
{
    return fmi_set_shift_decimal(instance, valueReferences, nValueReferences, shifts);
}

FMI3_Export fmi3Status fmi3SetShiftFraction(
        fmi3Instance instance,
        const fmi3ValueReference valueReferences[],
        size_t nValueReferences,
        const fmi3UInt64 counters[],
        const fmi3UInt64 resolutions[])
{
    return fmi_set_shift_fraction(instance, valueReferences, nValueReferences, counters, resolutions);
}

FMI3_Export fmi3Status fmi3EvaluateDiscreteStates(
        fmi3Instance instance)
{
    return fmi_evaluate_discrete_states(instance);
}

FMI3_Export fmi3Status fmi3UpdateDiscreteStates(
        fmi3Instance instance,
        fmi3Boolean *discreteStatesNeedUpdate,
        fmi3Boolean *terminateSimulation,
        fmi3Boolean *nominalsOfContinuousStatesChanged,
        fmi3Boolean *valuesOfContinuousStatesChanged,
        fmi3Boolean *nextEventTimeDefined,
        fmi3Float64 *nextEventTime)
{
    return fmi_update_discrete_states(instance, discreteStatesNeedUpdate, terminateSimulation,
            nominalsOfContinuousStatesChanged, valuesOfContinuousStatesChanged, nextEventTimeDefined,
            nextEventTime);
}
/* FMI 3.0 ME Functions */
FMI3_Export fmi3Status fmi3EnterContinuousTimeMode(fmi3Instance instance)
{
    return fmi_enter_continuous_time_mode(instance);
}

FMI3_Export fmi3Status fmi3SetTime(fmi3Instance instance, fmi3Float64 fmitime)
{
    return fmi_set_time(instance, fmitime);
}

FMI3_Export fmi3Status fmi3SetContinuousStates(fmi3Instance instance, const fmi3Float64 x[], size_t nx)
{
    return fmi_set_continuous_states(instance, x, nx);
}

FMI3_Export fmi3Status fmi3CompletedIntegratorStep(fmi3Instance instance,
    fmi3Boolean noSetFMUStatePriorToCurrentPoint,
    fmi3Boolean* enterEventMode, fmi3Boolean* terminateSimulation)
{
    return fmi_completed_integrator_step(instance, noSetFMUStatePriorToCurrentPoint,
                                         enterEventMode, terminateSimulation);
}

FMI3_Export fmi3Status fmi3GetContinuousStateDerivatives(fmi3Instance instance, fmi3Float64 derivatives[] , size_t nx)
{
    return fmi_get_derivatives(instance, derivatives, nx);
}

FMI3_Export fmi3Status fmi3GetEventIndicators(fmi3Instance instance, fmi3Float64 eventIndicators[], size_t ni)
{
    return fmi_get_event_indicators(instance, eventIndicators, ni);
}

FMI3_Export fmi3Status fmi3GetContinuousStates(fmi3Instance instance, fmi3Float64 x[], size_t nx)
{
    return fmi_get_continuous_states(instance, x, nx);
}

FMI3_Export fmi3Status fmi3GetNominalsOfContinuousStates(fmi3Instance instance, fmi3Float64 nominals[], size_t nx)
{
    return fmi_get_nominals_of_continuous_states(instance, nominals, nx);
}

FMI3_Export fmi3Status fmi3GetNumberOfEventIndicators(fmi3Instance instance, size_t* nz)
{
    return fmi_get_number_of_event_indicators(instance, nz);
}

FMI3_Export fmi3Status fmi3GetNumberOfContinuousStates(fmi3Instance instance, size_t* nx)
{
    return fmi_get_number_of_continuous_states(instance, nx);
}

FMI3_Export fmi3Status fmi3Terminate(fmi3Instance instance)
{
    return fmi_terminate(instance);
}

FMI3_Export fmi3Status fmi3Reset(fmi3Instance instance)
{
    return fmi_reset(instance);
}
