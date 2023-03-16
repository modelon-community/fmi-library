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

#ifndef FMU3_MODEL_H_
#define FMU3_MODEL_H_
#include <FMI3/fmi3Functions.h>

#include <fmu_dummy/fmu3_model_defines.h>
#ifndef FMI3_Export
	#define FMI3_Export DllExport
#endif

typedef struct {
    fmi3CallbackLogMessage         logMessage;
    fmi3CallbackIntermediateUpdate intermediateUpdate;
    fmi3CallbackLockPreemption     lockPreemption;
    fmi3CallbackUnlockPreemption   unlockPreemption;

    fmi3InstanceEnvironment instanceEnvironment;
} callbacks_t;

typedef enum {
    fmu_type_me = 1,
    fmu_type_bcs,
    fmu_type_hcs,
    fmu_type_scs
} fmu_type;

/**
 * 'fmu_type' Type of the FMU, e.g. one of ME, BCS, HCS, SCS.
 */
typedef struct {
	/*************** FMI ME 3.0 ****************/
	fmi3Float64					states			[N_STATES];
	fmi3Float64					states_nom		[N_STATES];
	fmi3Float64					states_der		[N_STATES];
	fmi3Float64					event_indicators[N_EVENT_INDICATORS];
	fmi3Float64					reals			[N_REAL];
	fmi3Int32				integers		[N_INTEGER];
	fmi3Boolean				booleans		[N_BOOLEAN];
	fmi3String				strings			[N_STRING];
	char*				binaries			[N_BINARY]; /* not using fmi3Binary because it's const,
														   which gives warnings with memcpy */

	size_t				binaries_sz			[N_BINARY];

	/* fmiInstantiateModel */
	fmi3Boolean				loggingOn;
	char					instanceName	[BUFFER];
	char					instantiationToken			[BUFFER];

	/* fmiSetTime */
	fmi3Float64					fmitime;

	/* fmiInitializeModel */
	fmi3Boolean				toleranceControlled;
	fmi3Float64					relativeTolerance;

    /* Event info */
    fmi3Boolean newDiscreteStatesNeeded;
    fmi3Boolean terminateSimulation;
    fmi3Boolean nominalsOfContinuousStatesChanged;
    fmi3Boolean valuesOfContinuousStatesChanged;
    fmi3Boolean nextEventTimeDefined;
    fmi3Float64 nextEventTime;

	/*************** FMI CS 3.0. Depends on the ME fields above and functions ****************/
	fmi3Float64					states_prev		[N_STATES];

	/* fmiInstantiateSlave */
	char					resourceLocation		[BUFFER];
	fmi3Boolean				visible;

	/* fmiInitializeSlave */
	fmi3Float64					tStart;
	fmi3Boolean				StopTimeDefined;
	fmi3Float64					tStop;

	/* fmiSetInputDerivatives */
	fmi3Float64					input_real		[N_INPUT_REAL][N_INPUT_REAL_MAX_ORDER + 1];

	/* fmiGetOutputDerivatives */
	fmi3Float64					output_real		[N_OUTPUT_REAL][N_OUTPUT_REAL_MAX_ORDER + 1];

    callbacks_t cb;

    fmu_type fmu_type;
} instance_t;

typedef instance_t* instance_ptr_t;

/* FMI 3.0 Common Functions */
const char*		fmi_get_version();

fmi3Status		fmi_set_debug_logging(
													fmi3Instance instance,
													fmi3Boolean loggingOn);

fmi3Instance fmi_instantiate(
        fmu_type                       fmuType,
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
        fmi3CallbackUnlockPreemption   unlockPreemption);

void fmi_free_instance(fmi3Instance instance);

fmi3Status fmi_enter_initialization_mode(
        fmi3Instance instance,
        fmi3Boolean  toleranceDefined,
        fmi3Float64  tolerance,
        fmi3Float64  startTime,
        fmi3Boolean  stopTimeDefined,
        fmi3Float64  stopTime);

fmi3Status fmi_exit_initialization_mode(fmi3Instance instance);

fmi3Status fmi_enter_event_mode(fmi3Instance instance,
                                fmi3Boolean inputEvent,
                                fmi3Boolean stepEvent,
                                const fmi3Int32  rootsFound[],
                                size_t nEventIndicators,
                                fmi3Boolean timeEvent);

fmi3Status		fmi_terminate(fmi3Instance instance);

fmi3Status		fmi_reset(
													fmi3Instance instance);


fmi3Status		fmi_get_float64(			
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr, fmi3Float64 value[],
													size_t nValues);

fmi3Status		fmi_get_float32(			
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr, fmi3Float32 value[],
													size_t nValues);

fmi3Status		fmi_get_int64(	
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
													fmi3Int64 value[],
													size_t nValues);

fmi3Status		fmi_get_int32(	
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
													fmi3Int32 value[],
													size_t nValues);

fmi3Status		fmi_get_int16(	
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
													fmi3Int16 value[],
													size_t nValues);

fmi3Status		fmi_get_int8(	
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
													fmi3Int8 value[],
													size_t nValues);

fmi3Status		fmi_get_uint64(	
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
													fmi3UInt64 value[],
													size_t nValues);

fmi3Status		fmi_get_uint32(	
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
													fmi3UInt32 value[],
													size_t nValues);

fmi3Status		fmi_get_uint16(	
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
													fmi3UInt16 value[],
													size_t nValues);

fmi3Status		fmi_get_uint8(	
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
													fmi3UInt8 value[],
													size_t nValues);

fmi3Status		fmi_get_boolean(
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
													fmi3Boolean value[],
													size_t nValues);

fmi3Status		fmi_get_string(
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
													fmi3String value[],
													size_t nValues);

fmi3Status		fmi_get_binary(
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
													size_t sizes[],
													fmi3Binary value[],
													size_t nValues);

fmi3Status		fmi_set_float64(
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
													const fmi3Float64 value[],
													size_t nValues);

fmi3Status		fmi_set_float32(
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
													const fmi3Float32 value[],
													size_t nValues);

fmi3Status		fmi_set_int64(
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
													const fmi3Int64 value[],
													size_t nValues);

fmi3Status		fmi_set_int32(
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
													const fmi3Int32 value[],
													size_t nValues);

fmi3Status		fmi_set_int16(
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
													const fmi3Int16 value[],
													size_t nValues);

fmi3Status		fmi_set_int8(
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
													const fmi3Int8 value[],
													size_t nValues);

fmi3Status		fmi_set_uint64(
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
													const fmi3UInt64 value[],
													size_t nValues);

fmi3Status		fmi_set_uint32(
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
													const fmi3UInt32 value[],
													size_t nValues);

fmi3Status		fmi_set_uint16(
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
													const fmi3UInt16 value[],
													size_t nValues);

fmi3Status		fmi_set_uint8(
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
													const fmi3UInt8 value[],
													size_t nValues);

fmi3Status		fmi_set_boolean(
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
													const fmi3Boolean value[],
                                                    size_t nValues);

fmi3Status		fmi_set_string(
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
													const fmi3String value[],
                                                    size_t nValues);

fmi3Status		fmi_set_binary(
													fmi3Instance instance,
													const fmi3ValueReference vr[],
													size_t nvr,
                                                    const size_t sizes[],
													const fmi3Binary value[],
                                                    size_t nValues);

/* Getting Variable Dependency Information */
fmi3Status fmi_get_number_of_variable_dependencies(fmi3Instance instance,
                                                   fmi3ValueReference valueReference,
                                                   size_t* nDependencies);

fmi3Status fmi_get_variable_dependencies(fmi3Instance instance,
                                         fmi3ValueReference dependent,
                                         size_t elementIndicesOfDependent[],
                                         fmi3ValueReference independents[],
                                         size_t elementIndicesOfIndependents[],
                                         fmi3DependencyKind dependencyKinds[],
                                         size_t nDependencies);

fmi3Status fmi_get_FMU_state(fmi3Instance instance, fmi3FMUState* FMUState);

fmi3Status fmi_set_FMU_state(fmi3Instance instance, fmi3FMUState FMUState);

fmi3Status fmi_free_FMU_state(fmi3Instance instance, fmi3FMUState* FMUState);

fmi3Status fmi_serialized_FMU_state_size(fmi3Instance instance,
                                         fmi3FMUState  FMUState,
                                         size_t* size);

fmi3Status fmi_serialize_FMU_state(fmi3Instance instance,
                                   fmi3FMUState  FMUState,
                                   fmi3Byte serializedState[],
                                   size_t size);

fmi3Status fmi_de_serialize_FMU_state(fmi3Instance instance,
                                      const fmi3Byte serializedState[],
                                      size_t size,
                                      fmi3FMUState* FMUState);

fmi3Status fmi_get_directional_derivative(
        fmi3Instance instance,
        const fmi3ValueReference unknowns[],
        size_t nUnknowns,
        const fmi3ValueReference knowns[],
        size_t nKnowns,
        const fmi3Float64 seed[],
        size_t nSeed,
        fmi3Float64 sensitivity[],
        size_t nSensitivity);

fmi3Status fmi_get_adjoint_derivative(
        fmi3Instance instance,
        const fmi3ValueReference unknowns[],
        size_t nUnknowns,
        const fmi3ValueReference knowns[],
        size_t nKnowns,
        const fmi3Float64 seed[],
        size_t nSeed,
        fmi3Float64 sensitivity[],
        size_t nSensitivity);

/* Entering and exiting the Configuration or Reconfiguration Mode */
fmi3Status fmi_enter_configuration_mode(fmi3Instance instance);

fmi3Status fmi_exit_configuration_mode(fmi3Instance instance);

/* Clock related functions */
fmi3Status fmi_get_clock(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         fmi3Clock values[],
                         size_t nValues);

fmi3Status fmi_set_clock(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         const fmi3Clock values[],
                         const fmi3Boolean subactive[],
                         size_t nValues);

fmi3Status fmi_get_interval_decimal(fmi3Instance instance,
                                    const fmi3ValueReference valueReferences[],
                                    size_t nValueReferences,
                                    fmi3Float64 interval[],
                                    size_t nValues);

fmi3Status fmi_get_interval_fraction(fmi3Instance instance,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     fmi3UInt64 intervalCounter[],
                                     fmi3UInt64 resolution[],
                                     size_t nValues);

fmi3Status fmi_set_interval_decimal(fmi3Instance instance,
                                    const fmi3ValueReference valueReferences[],
                                    size_t nValueReferences,
                                    const fmi3Float64 interval[],
                                    size_t nValues);

fmi3Status fmi_set_interval_fraction(fmi3Instance instance,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     const fmi3UInt64 intervalCounter[],
                                     const fmi3UInt64 resolution[],
                                     size_t nValues);

fmi3Status fmi_new_discrete_states(fmi3Instance instance,
                                   fmi3Boolean *newDiscreteStatesNeeded,
                                   fmi3Boolean *terminateSimulation,
                                   fmi3Boolean *nominalsOfContinuousStatesChanged,
                                   fmi3Boolean *valuesOfContinuousStatesChanged,
                                   fmi3Boolean *nextEventTimeDefined,
                                   fmi3Float64 *nextEventTime);

/* FMI 3.0 ME Functions */
fmi3Status		fmi_enter_continuous_time_mode(fmi3Instance instance);

fmi3Status		fmi_set_time(
													fmi3Instance instance,
													fmi3Float64 fmitime);

fmi3Status		fmi_set_continuous_states(
													fmi3Instance instance,
													const fmi3Float64 x[],
													size_t nx);

fmi3Status fmi_completed_integrator_step(
    fmi3Instance instance,
    fmi3Boolean noSetFMUStatePriorToCurrentPoint,
    fmi3Boolean* enterEventMode, fmi3Boolean* terminateSimulation);

fmi3Status		fmi_get_derivatives(
													fmi3Instance instance,
													fmi3Float64 derivatives[],
													size_t nx);

fmi3Status		fmi_get_event_indicators(
													fmi3Instance instance,
													fmi3Float64 eventIndicators[],
													size_t ni);

fmi3Status		fmi_get_continuous_states(
													fmi3Instance instance,
													fmi3Float64 x[],
													size_t nx);

fmi3Status		fmi_get_nominals_of_continuous_states(	
													fmi3Instance instance,
													fmi3Float64 nominals[],
													size_t nx);

fmi3Status fmi_get_number_of_event_indicators(fmi3Instance instance, size_t* nz);

fmi3Status fmi_get_number_of_continuous_states(fmi3Instance instance, size_t* nx);

/* FMI 3.0 CS Functions */
#ifdef fmi3Functions_h

fmi3Status fmi_enter_step_mode(fmi3Instance instance);

fmi3Status fmi_get_output_derivatives(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const fmi3Int32 orders[],
                                      fmi3Float64 values[],
                                      size_t nValues);

void			fmi_free_slave_instance(
													fmi3Instance instance);

fmi3Status fmi_do_step(fmi3Instance instance,
                       fmi3Float64 currentCommunicationPoint,
                       fmi3Float64 communicationStepSize,
                       fmi3Boolean noSetFMUStatePriorToCurrentPoint,
                       fmi3Boolean* terminate,
                       fmi3Boolean* earlyReturn,
                       fmi3Float64* lastSuccessfulTime);

fmi3Status fmi_activate_model_partition(fmi3Instance instance,
                                        fmi3ValueReference clockReference,
                                        size_t clockElementIndex,
                                        fmi3Float64 activationTime);

#endif /* End of fmi3Functions_h */
#endif /* End of header FMU3_MODEL_H_ */
