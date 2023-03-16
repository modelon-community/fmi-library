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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "JM/jm_portability.h"

#include <fmu_dummy/fmu3_model.h>

#if defined (_MSC_VER) && _MSC_VER >= 1600
    /* MSVC is not yet full C99 compliant, but offers snprintf via alternative
	 * name.
	 * NOTE: this function also exists in jm_portability, but I'm having trouble
	 * with linking on MSVC since jm_snprintf is declared with
	 * __declspec(export) in a static lib, and we're trying to link it with a
	 * a target that is a dll, the export symbols are prefixed with __imp__.
	 */
    #define snprintf _snprintf
#endif


/* Model calculation functions */
static int calc_initialize(instance_ptr_t inst)
{
	inst->states[VAR_R_HIGHT]		= 1.0;
	inst->states[VAR_R_HIGHT_SPEED] = 4;
	inst->reals	[VAR_R_GRATIVY]		= -9.81;
	inst->reals	[VAR_R_BOUNCE_CONF]	= 0.5;
	if(inst->loggingOn) {
		char msg[1000];
		size_t msg_sz = sizeof(msg) / sizeof(char);

		inst->cb.logMessage(inst->cb.instanceEnvironment, inst->instanceName, fmi3OK, "INFO", "###### Initializing instance ######");

		snprintf(msg, msg_sz, "Init #r%d#=%g", VAR_R_HIGHT, inst->states[VAR_R_HIGHT]);
		inst->cb.logMessage(inst->cb.instanceEnvironment, inst->instanceName, fmi3OK, "INFO", msg);

		snprintf(msg, msg_sz, "Init #r%d#=%g", VAR_R_HIGHT, "Init #r%d#=%g",VAR_R_HIGHT_SPEED, inst->states[VAR_R_HIGHT_SPEED]);
		inst->cb.logMessage(inst->cb.instanceEnvironment, inst->instanceName, fmi3OK, "INFO", msg);

		snprintf(msg, msg_sz, "Init #r%d#=%g", VAR_R_HIGHT, "Init #r%d#=%g",VAR_R_HIGHT_SPEED, "Init #r%d#=%g",VAR_R_GRATIVY, inst->reals[VAR_R_GRATIVY]);
		inst->cb.logMessage(inst->cb.instanceEnvironment, inst->instanceName, fmi3OK, "INFO", msg);

		snprintf(msg, msg_sz, "Init #r%d#=%g", VAR_R_HIGHT, "Init #r%d#=%g",VAR_R_HIGHT_SPEED, "Init #r%d#=%g",VAR_R_BOUNCE_CONF, inst->reals[VAR_R_BOUNCE_CONF]);
		inst->cb.logMessage(inst->cb.instanceEnvironment, inst->instanceName, fmi3OK, "INFO", msg);
	}
	return 0;
}

static int calc_get_derivatives(instance_ptr_t inst)
{
	inst->states_der[VAR_R_HIGHT]		= inst->states[VAR_R_HIGHT_SPEED];
	inst->states_der[VAR_R_HIGHT_SPEED] = inst->reals[VAR_R_GRATIVY];
	return 0;
}

static int calc_get_event_indicators(instance_ptr_t inst)
{	
	fmi3Float64 event_tol = 1e-16;
	inst->event_indicators[EVENT_HIGHT]		= inst->states[VAR_R_HIGHT] + (inst->states[VAR_R_HIGHT] >= 0 ? event_tol : -event_tol);
	return 0;
}

static int calc_event_update(instance_ptr_t inst)
{	
    inst->newDiscreteStatesNeeded           = fmi3False;
    inst->terminateSimulation               = fmi3False;
    inst->nominalsOfContinuousStatesChanged = fmi3False;
    inst->nextEventTimeDefined              = fmi3False;
    inst->nextEventTime                     = -0.0;
	if ((inst->states[VAR_R_HIGHT] < 0) && (inst->states[VAR_R_HIGHT_SPEED] < 0)) {
		inst->states[VAR_R_HIGHT_SPEED] = - inst->reals[VAR_R_BOUNCE_CONF] * inst->states[VAR_R_HIGHT_SPEED];
		inst->states[VAR_R_HIGHT] = 0;

        inst->valuesOfContinuousStatesChanged = fmi3True;
		return 0;
	} else {
        inst->valuesOfContinuousStatesChanged = fmi3False;
		return 1; /* Should not call the event update */
	}
}


/* FMI 3.0 Common Functions */
const char* fmi_get_version()
{
	return FMI_VERSION;
}

fmi3Status fmi_set_debug_logging(fmi3Instance instance, fmi3Boolean loggingOn)
{
	instance_ptr_t inst = instance;
	if (inst == NULL) {
		return fmi3Fatal;
	} else {
		inst->loggingOn = loggingOn;
		return fmi3OK;
	}
}

fmi3Status fmi_get_float64(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3Float64 value[], size_t nValues)
{
	instance_ptr_t inst = instance;
	if (inst == NULL) {
		return fmi3Fatal;
	} else {
		size_t k;
		size_t m = 0; /* index in value, considering that arrays will require many indices */
		for (k = 0; k < nvr; k++) {
			fmi3ValueReference cvr = vr[k];
			if (cvr < N_STATES) {
				value[m] = inst->states[cvr];
			} 
			else if(cvr == 4) {
				calc_get_derivatives(inst);
				value[m] = inst->states_der[1];
			}
			else if (cvr == 12) { /* special case: array */
				calc_get_derivatives(inst);
				value[m++] = inst->states[0];
				value[m++] = inst->states_der[0];
				value[m++] = inst->states[1];
				value[m++] = inst->states_der[1];
			}
			else {
				value[m] = inst->reals[cvr];
			}
			m++;
		}

		if (m - 1 != nValues) {
            return fmi3Fatal;
		}

		return fmi3OK;
	}
}
/* these functions are not used, but need to be implemented according to standard */
fmi3Status fmi_get_float32(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3Float32 value[], size_t nValues) { return fmi3Fatal; }
fmi3Status fmi_get_int64(  fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3Int64 value[],   size_t nValues) { return fmi3Fatal; }
/* fmi_get_int32 has a real implementation */
fmi3Status fmi_get_int16(  fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3Int16 value[],   size_t nValues) { return fmi3Fatal; }
fmi3Status fmi_get_int8(   fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3Int8 value[],    size_t nValues) { return fmi3Fatal; }
fmi3Status fmi_get_uint64( fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3UInt64 value[],  size_t nValues) { return fmi3Fatal; }
fmi3Status fmi_get_uint32( fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3UInt32 value[],  size_t nValues) { return fmi3Fatal; }
fmi3Status fmi_get_uint16( fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3UInt16 value[],  size_t nValues) { return fmi3Fatal; }
fmi3Status fmi_get_uint8(  fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3UInt8 value[],   size_t nValues) { return fmi3Fatal; }

fmi3Status fmi_get_int32(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3Int32 value[], size_t nValues)
{
	instance_ptr_t inst = instance;
    size_t k;

	if (inst == NULL) {
		return fmi3Fatal;
	}

    for (k = 0; k < nvr; k++) {
        value[k] = inst->integers[vr[k]];
    }
    return fmi3OK;
}

fmi3Status fmi_get_boolean(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3Boolean value[],
        size_t nValues)
{
	instance_ptr_t inst = instance;
    size_t k;

	if (inst == NULL) {
		return fmi3Fatal;
	}

    for (k = 0; k < nvr; k++) {
        value[k] = inst->booleans[vr[k]];
    }
    return fmi3OK;
}

fmi3Status fmi_get_string(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3String value[], size_t nValues)
{
	instance_ptr_t inst = instance;
    size_t k;

	if (inst == NULL) {
		return fmi3Fatal;
	}

    for (k = 0; k < nvr; k++) {
        value[k] = inst->strings[vr[k]];
    }
    return fmi3OK;
}

fmi3Status fmi_get_binary(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, size_t sizes[],
        fmi3Binary value[], size_t nValues)
{
	instance_ptr_t inst = instance;
    size_t k;

	if (inst == NULL) {
		return fmi3Fatal;
	}

    for (k = 0; k < nvr; k++) {
        value[k] = inst->binaries[vr[k]];
        sizes[k] = inst->binaries_sz[k];
    }
    return fmi3OK;
}

/* TODO: nValues is just used for verification, i.e. to check that sum(lenght(vr[i])) == nValues;
 * currently it seems it's not decided whether this parameter will remain, so not implementing anything
 * for now
 * https://github.com/modelica/fmi-standard/issues/512
 */
fmi3Status fmi_set_float64(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3Float64 value[], size_t nValues)
{
	instance_ptr_t inst = instance;
	if (inst == NULL) {
		return fmi3Fatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {
			fmi3ValueReference cvr = vr[k];
			if (cvr < N_STATES) {
				inst->states[cvr] = value[k]; 
			} 
			else if(cvr == 4) {
				inst->cb.logMessage(instance, inst->instanceName, fmi3Warning, "WARNING", "Cannot set acceleration value (calculated)");
				return fmi3Error;
			}
			else {
				inst->reals[cvr] = value[k]; 
			}
		}
		return fmi3OK;
	}
}

/* these functions are not used, but need to be implemented according to standard */
fmi3Status fmi_set_int64( fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3Int64 value[],  size_t nValues) { return fmi3Fatal; }
/* fmi_set_int32 has a real implementation */
fmi3Status fmi_set_int16( fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3Int16 value[],  size_t nValues) { return fmi3Fatal; }
fmi3Status fmi_set_int8(  fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3Int8 value[],   size_t nValues) { return fmi3Fatal; }
fmi3Status fmi_set_uint64(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3UInt64 value[], size_t nValues) { return fmi3Fatal; }
fmi3Status fmi_set_uint32(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3UInt32 value[], size_t nValues) { return fmi3Fatal; }
fmi3Status fmi_set_uint16(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3UInt16 value[], size_t nValues) { return fmi3Fatal; }
fmi3Status fmi_set_uint8( fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3UInt8 value[],  size_t nValues) { return fmi3Fatal; }

fmi3Status fmi_set_int32(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3Int32 value[], size_t nValues)
{
	instance_ptr_t inst = instance;
    size_t k;

	if (inst == NULL) {
		return fmi3Fatal;
	}

    for (k = 0; k < nvr; k++) {
        inst->integers[vr[k]] = value[k]; 
    }
    return fmi3OK;
}

fmi3Status fmi_set_boolean(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3Boolean value[],
        size_t nValues)
{
	instance_ptr_t inst = instance;
    size_t k;

	if (inst == NULL) {
		return fmi3Fatal;
	}

    for (k = 0; k < nvr; k++) {
        inst->booleans[vr[k]] = value[k]; 
    }
    return fmi3OK;
}

fmi3Status fmi_set_string(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3String value[],
        size_t nValues)
{
	instance_ptr_t inst = instance;
    size_t k;

	if (inst == NULL) {
		return fmi3Fatal;
	}

    for (k = 0; k < nvr; k++) {			
        size_t len;
        fmi3String s_dist;
        fmi3String s_src = value[k];

        len = strlen((char*)s_src) + 1;
        s_dist = malloc(len * sizeof(char));
        if (s_dist == NULL) {
            return fmi3Fatal;
        }			
        strcpy((char*)s_dist, (char*)s_src);
        if (inst->strings[vr[k]]) {
            free((void*)inst->strings[vr[k]]);
        }
        inst->strings[vr[k]] = s_dist;
    }

    /******* Logger test *******/
    if (inst->loggingOn == fmi3True) {
        for (k = 0; k < nvr; k++) {
            fmi3ValueReference cvr = vr[k];
            if (cvr == VAR_S_LOGGER_TEST) {
                inst->cb.logMessage(inst->cb.instanceEnvironment, inst->instanceName, fmi3Fatal, "INFO", value[k]);
            }
        }
    }
    /******* End of logger test *******/
    return fmi3OK;
}

fmi3Status fmi_set_binary(fmi3Instance instance, const fmi3ValueReference vrs[], size_t nvr, const size_t sizes[],
        const fmi3Binary values[], size_t nValues)
{
	instance_ptr_t inst = instance;
    size_t k;

	if (inst == NULL) {
		return fmi3Fatal;
	}

    for (k = 0; k < nvr; k++) {
        fmi3ValueReference vr = vrs[k];

        /* realloc */
        if (inst->binaries[vr]) {
            free((void*)inst->binaries[vr]);
        }
        inst->binaries[vr] = malloc(sizes[k]);
        if (!inst->binaries[vr]) {
            return fmi3Fatal;
        }

        /* save size */
        inst->binaries_sz[k] = sizes[k];

        /* set value */
        memcpy(inst->binaries[k], values[k], sizes[k]);
    }

    return fmi3OK;
}

/* Getting Variable Dependency Information */
fmi3Status fmi_get_number_of_variable_dependencies(fmi3Instance instance,
                                                   fmi3ValueReference valueReference,
                                                   size_t* nDependencies)
{
    /* NYI */
    if (!nDependencies) {
        return fmi3Error;
    }
    *nDependencies = 0;
    return fmi3OK;
}

fmi3Status fmi_get_variable_dependencies(fmi3Instance instance,
                                         fmi3ValueReference dependent,
                                         size_t elementIndicesOfDependent[],
                                         fmi3ValueReference independents[],
                                         size_t elementIndicesOfIndependents[],
                                         fmi3DependencyKind dependencyKinds[],
                                         size_t nDependencies)
{
    /* NYI */
    if (nDependencies != 0) {
        return fmi3Error;
    }
    return fmi3OK;
}

fmi3Status fmi_get_FMU_state(fmi3Instance instance, fmi3FMUState* FMUState)
{
    return fmi3Error;
}


fmi3Status fmi_set_FMU_state(fmi3Instance instance, fmi3FMUState FMUState)
{
    return fmi3Error;
}

fmi3Status fmi_free_FMU_state(fmi3Instance instance, fmi3FMUState* FMUState)
{
    return fmi3Error;
}

fmi3Status fmi_serialized_FMU_state_size(fmi3Instance instance,
                                         fmi3FMUState FMUState,
                                         size_t* size)
{
    return fmi3Error;
}

fmi3Status fmi_serialize_FMU_state(fmi3Instance instance,
                                   fmi3FMUState  FMUState,
                                   fmi3Byte serializedState[],
                                   size_t size)
{
    return fmi3Error;
}

fmi3Status fmi_de_serialize_FMU_state(fmi3Instance instance,
                                      const fmi3Byte serializedState[],
                                      size_t size,
                                      fmi3FMUState* FMUState)
{
    return fmi3Error;
}

fmi3Status fmi_get_directional_derivative(
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
    return fmi3OK;
}

fmi3Status fmi_get_adjoint_derivative(
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
    return fmi3OK;
}

/* Entering and exiting the Configuration or Reconfiguration Mode */
fmi3Status fmi_enter_configuration_mode(fmi3Instance instance)
{
    return fmi3OK;
}

fmi3Status fmi_exit_configuration_mode(fmi3Instance instance)
{
    return fmi3OK;
}

/* Clock related functions */
fmi3Status fmi_get_clock(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         fmi3Clock values[],
                         size_t nValues)
{
    return fmi3OK;
}

fmi3Status fmi_set_clock(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         const fmi3Clock values[],
                         const fmi3Boolean subactive[],
                         size_t nValues)
{
    return fmi3OK;
}

fmi3Status fmi_get_interval_decimal(fmi3Instance instance,
                                    const fmi3ValueReference valueReferences[],
                                    size_t nValueReferences,
                                    fmi3Float64 interval[],
                                    size_t nValues)
{
    return fmi3OK;
}

fmi3Status fmi_get_interval_fraction(fmi3Instance instance,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     fmi3UInt64 intervalCounter[],
                                     fmi3UInt64 resolution[],
                                     size_t nValues)
{
    return fmi3OK;
}

fmi3Status fmi_set_interval_decimal(fmi3Instance instance,
                                    const fmi3ValueReference valueReferences[],
                                    size_t nValueReferences,
                                    const fmi3Float64 interval[],
                                    size_t nValues)
{
    return fmi3OK;
}

fmi3Status fmi_set_interval_fraction(fmi3Instance instance,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     const fmi3UInt64 intervalCounter[],
                                     const fmi3UInt64 resolution[],
                                     size_t nValues)
{
    return fmi3OK;
}

/* FMI 3.0 ME Functions */
fmi3Instance fmi_instantiate(
        fmu_type                       fmuType,
        fmi3String                     instanceName,
        fmi3String                     instantiationToken,
        fmi3String                     resourceLocation,
        fmi3Boolean                    visible,
        fmi3Boolean                    loggingOn,
        fmi3Boolean                    intermediateVariableGetRequired,         /* CS only */
        fmi3Boolean                    intermediateInternalVariableGetRequired, /* CS only */
        fmi3Boolean                    intermediateVariableSetRequired,         /* CS only */
        fmi3InstanceEnvironment        instanceEnvironment,
        fmi3CallbackLogMessage         logMessage,
        fmi3CallbackIntermediateUpdate intermediateUpdate,                      /* CS only */
        fmi3CallbackLockPreemption     lockPreemption,                          /* SCS only */
        fmi3CallbackUnlockPreemption   unlockPreemption)                        /* SCS only */
{
	instance_ptr_t inst;
	int k, p;

    inst = (instance_ptr_t)malloc(sizeof(instance_t));

	if (inst == NULL) {
		return NULL;
	} else if (strcmp(instantiationToken, FMI_INSTANTIATION_TOKEN) != 0) {
		return NULL;
	} else {	
        inst->fmu_type = fmuType;

		sprintf(inst->instanceName, "%s", instanceName);
		sprintf(inst->instantiationToken, "%s", instantiationToken);

        /* Set callbacks */
        inst->cb.logMessage          = logMessage;
        inst->cb.intermediateUpdate  = intermediateUpdate;
        inst->cb.lockPreemption      = lockPreemption;
        inst->cb.unlockPreemption    = unlockPreemption;
        inst->cb.instanceEnvironment = instanceEnvironment;
		
		inst->loggingOn = loggingOn;

		/* Set default values */
		for (k = 0; k < N_STATES;			k++) inst->states[k]			= 0.0;
		for (k = 0; k < N_STATES;			k++) inst->states_prev[k]		= 0.0; /* Used in CS only */
		for (k = 0; k < N_STATES;			k++) inst->states_nom[k]		= 1.0;
		for (k = 0; k < N_STATES;			k++) inst->states_der[k]		= 0.0;
		for (k = 0; k < N_EVENT_INDICATORS; k++) inst->event_indicators[k]	= 1e10;
		for (k = 0; k < N_REAL;				k++) inst->reals[k]				= 0.0;
		for (k = 0; k < N_INTEGER;			k++) inst->integers[k]			= 0;
		for (k = 0; k < N_BOOLEAN;			k++) inst->booleans[k]			= fmi3False;
		for (k = 0; k < N_STRING;			k++) inst->strings[k]			= NULL;
		for (k = 0; k < N_BINARY;			k++) inst->binaries[k]			= NULL;

        /* Set default event info */
        inst->newDiscreteStatesNeeded           = fmi3False;
        inst->terminateSimulation               = fmi3False;
        inst->nominalsOfContinuousStatesChanged = fmi3False;
        inst->valuesOfContinuousStatesChanged   = fmi3False;
        inst->nextEventTimeDefined              = fmi3False;
        inst->nextEventTime                     = 0.0;

		/* Used in CS only */
		for (k = 0; k < N_INPUT_REAL; k++) {
			for (p = 0; p < N_INPUT_REAL_MAX_ORDER + 1; p++) {
				inst->input_real[k][p] = 0.0;
			}
		}

		/* Used in CS only */
		for (k = 0; k < N_OUTPUT_REAL; k++) {
			for (p = 0; p < N_OUTPUT_REAL_MAX_ORDER + 1; p++) {
				inst->output_real[k][p] = MAGIC_TEST_VALUE;
			}
		}
	
		sprintf(inst->resourceLocation, "%s", resourceLocation);
		inst->visible		= visible;
		return inst;
	}
}

void fmi_free_instance(fmi3Instance instance)
{
	int i;
	instance_ptr_t inst = instance;
	for(i = 0; i < N_STRING; i++) {
		fmi3String str = inst->strings[i];
		if (str) {
			free((void*)str);
			str = NULL;
		}
	}
	for(i = 0; i < N_BINARY; i++) {
		fmi3String bin = inst->binaries[i];
		if (bin) {
			free((void*)bin);
			bin = NULL;
		}
	}

	free(instance);
}

fmi3Status fmi_enter_initialization_mode(
        fmi3Instance instance,
        fmi3Boolean  toleranceDefined,
        fmi3Float64  tolerance,
        fmi3Float64  startTime,
        fmi3Boolean  stopTimeDefined,
        fmi3Float64  stopTime)
{
    if (instance == NULL) {
        return fmi3Fatal;
    } else {
        instance_ptr_t inst = instance;

        inst->toleranceControlled = toleranceDefined;
        inst->relativeTolerance   = tolerance;
        inst->tStart              = startTime;
        inst->StopTimeDefined     = stopTimeDefined;
        inst->tStop               = stopTime;

        calc_initialize(instance);
        return fmi3OK;
    }
}

fmi3Status fmi_exit_initialization_mode(fmi3Instance instance)
{
    return fmi3OK;
}

fmi3Status fmi_enter_event_mode(fmi3Instance instance,
                                fmi3Boolean inputEvent,
                                fmi3Boolean stepEvent,
                                const fmi3Int32 rootsFound[],
                                size_t nEventIndicators,
                                fmi3Boolean timeEvent)
{
    return fmi3OK;
}

fmi3Status fmi_new_discrete_states(fmi3Instance instance,
                                   fmi3Boolean *newDiscreteStatesNeeded,
                                   fmi3Boolean *terminateSimulation,
                                   fmi3Boolean *nominalsOfContinuousStatesChanged,
                                   fmi3Boolean *valuesOfContinuousStatesChanged,
                                   fmi3Boolean *nextEventTimeDefined,
                                   fmi3Float64 *nextEventTime)
{
	instance_ptr_t inst = instance;
	if (inst == NULL) {
		return fmi3Fatal;
	} else {
		calc_event_update(inst);

        *newDiscreteStatesNeeded           = inst->newDiscreteStatesNeeded;
        *terminateSimulation               = inst->terminateSimulation;
        *nominalsOfContinuousStatesChanged = inst->nominalsOfContinuousStatesChanged;
        *valuesOfContinuousStatesChanged   = inst->valuesOfContinuousStatesChanged;
        *nextEventTimeDefined              = inst->nextEventTimeDefined;
        *nextEventTime                     = inst->nextEventTime;
		return fmi3OK;
	}
}

fmi3Status fmi_enter_continuous_time_mode(fmi3Instance instance)
{
    return fmi3OK;
}

fmi3Status fmi_set_time(fmi3Instance instance, fmi3Float64 fmitime)
{
	instance_ptr_t inst = instance;
	if (inst == NULL) {
		return fmi3Fatal;
	} else {
		inst->fmitime = fmitime;
		return fmi3OK;
	}
}

fmi3Status fmi_set_continuous_states(fmi3Instance instance, const fmi3Float64 x[], size_t nx)
{
	instance_ptr_t inst = instance;
	if (inst == NULL) {
		return fmi3Fatal;
	} else {
		size_t k;
		for (k = 0; k < nx; k++) {
			inst->states[k] = x[k];
		}
		return fmi3OK;
	}
}

fmi3Status fmi_completed_integrator_step(fmi3Instance instance,
  fmi3Boolean noSetFMUStatePriorToCurrentPoint,
  fmi3Boolean* enterEventMode, fmi3Boolean* terminateSimulation)
{
	instance_ptr_t inst = instance;
	if (inst == NULL) {
		return fmi3Fatal;
	} else {
		*enterEventMode = fmi3False;
		return fmi3OK;
	}
}

fmi3Status fmi_get_derivatives(fmi3Instance instance, fmi3Float64 derivatives[] , size_t nx)
{
	instance_ptr_t inst = instance;
	if (inst == NULL) {
		return fmi3Fatal;
	} else {
		size_t k;

		calc_get_derivatives(inst);

		for (k = 0; k < nx; k++) {
			derivatives[k] = inst->states_der[k];
		}
		return fmi3OK;
	}
}

fmi3Status fmi_get_event_indicators(fmi3Instance instance, fmi3Float64 eventIndicators[], size_t ni)
{
	instance_ptr_t inst = instance;
	if (inst == NULL) {
		return fmi3Fatal;
	} else {
		size_t k;

		calc_get_event_indicators(inst);

		for (k = 0; k < ni; k++) {
			eventIndicators[k] = inst->event_indicators[k];
		}
		return fmi3OK;
	}
}

fmi3Status fmi_get_continuous_states(fmi3Instance instance, fmi3Float64 x[], size_t nx)
{
	instance_ptr_t inst = instance;
	if (inst == NULL) {
		return fmi3Fatal;
	} else {
		size_t k;

		for (k = 0; k < nx; k++) {
			x[k] = inst->states[k];
		}
		return fmi3OK;
	}
}

fmi3Status fmi_get_nominals_of_continuous_states(fmi3Instance instance, fmi3Float64 nominals[], size_t nx)
{
	instance_ptr_t inst = instance;
	if (inst == NULL) {
		return fmi3Fatal;
	} else {
		size_t k;
		for (k = 0; k < nx; k++) {
			nominals[k] = inst->states_nom[k];
		}
		return fmi3OK;
	}
}

fmi3Status fmi_get_number_of_event_indicators(fmi3Instance instance, size_t* nz)
{
	instance_ptr_t inst = instance;
	if (inst == NULL) {
		return fmi3Fatal;
	}

    *nz = N_EVENT_INDICATORS;

    return fmi3OK;
}

fmi3Status fmi_get_number_of_continuous_states(fmi3Instance instance, size_t* nx)
{
	instance_ptr_t inst = instance;
	if (inst == NULL) {
		return fmi3Fatal;
	}

    *nx = N_STATES;

    return fmi3OK;
}

fmi3Status fmi_terminate(fmi3Instance instance)
{
	instance_ptr_t inst = instance;
	if (inst == NULL) {
		return fmi3Fatal;
	} else {
		return fmi3OK;
	}
}

/* FMI 3.0 CS Functions */
fmi3Status fmi_enter_step_mode(fmi3Instance instance)
{
	return fmi3OK;
}

fmi3Status fmi_reset(fmi3Instance instance)
{
	return fmi3OK;
}

fmi3Status fmi_get_output_derivatives(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const fmi3Int32 orders[],
                                      fmi3Float64 values[],
                                      size_t nValues)
{
	instance_ptr_t inst	= instance;
	size_t k;

	for (k = 0; k < nValueReferences; k++) {
		values[k] = inst->output_real[valueReferences[k]][orders[k]];
	}

	return fmi3OK;
}

fmi3Status fmi_do_step(fmi3Instance instance,
                       fmi3Float64 currentCommunicationPoint,
                       fmi3Float64 communicationStepSize,
                       fmi3Boolean noSetFMUStatePriorToCurrentPoint,
                       fmi3Boolean* terminate,
                       fmi3Boolean* earlyReturn,
                       fmi3Float64* lastSuccessfulTime)
{
	instance_ptr_t inst	= instance;

	if (inst == NULL) {
		return fmi3Fatal;
	} else {
		fmi3Float64 tstart = currentCommunicationPoint;
		fmi3Float64 tcur;
		fmi3Float64 tend = currentCommunicationPoint + communicationStepSize;
		fmi3Float64 hcur; 
		fmi3Float64 hdef = 0.01;	/* Default time step length */
		fmi3Float64 z_cur[N_EVENT_INDICATORS];
		fmi3Float64 z_pre[N_EVENT_INDICATORS];
		fmi3Float64 states[N_STATES];
		fmi3Float64 states_der[N_STATES];
		fmi3Boolean callEventUpdate;
		fmi3Boolean terminateSimulation;
		fmi3Status fmi3Status;	
		size_t k;
		size_t counter = 0;

		fmi_get_continuous_states(inst, states, N_STATES);
		fmi_get_event_indicators(inst, z_pre, N_EVENT_INDICATORS);

		tcur = tstart;
		hcur = hdef;
		callEventUpdate = fmi3False;

		while (tcur < tend && counter < 100) {
			size_t k;
			int zero_crossning_event = 0;
			counter++;

			fmi_set_time(inst, tcur);
			fmi_get_event_indicators(inst, z_cur, N_EVENT_INDICATORS);

			/* Check if an event inidcator has triggered */
			for (k = 0; k < N_EVENT_INDICATORS; k++) {
				if (z_cur[k]*z_pre[k] < 0) {
					zero_crossning_event = 1;
					break;
				}
			}

			/* Handle any events */
			if (callEventUpdate || zero_crossning_event ||
			  (inst->nextEventTimeDefined && tcur == inst->nextEventTime)) {
				fmi3Status = fmi_new_discrete_states(
                        inst,
                        &inst->newDiscreteStatesNeeded,
                        &inst->terminateSimulation,
                        &inst->nominalsOfContinuousStatesChanged,
                        &inst->valuesOfContinuousStatesChanged,
                        &inst->nextEventTimeDefined,
                        &inst->nextEventTime);
				fmi3Status = fmi_get_continuous_states(inst, states, N_STATES);
				fmi3Status = fmi_get_event_indicators(inst, z_cur, N_EVENT_INDICATORS);
				fmi3Status = fmi_get_event_indicators(inst, z_pre, N_EVENT_INDICATORS);
			}

			/* Updated next time step */
			if (inst->nextEventTimeDefined) {
				if (tcur + hdef < inst->nextEventTime) {
					hcur = hdef;
				} else {
					hcur = inst->nextEventTime - tcur;
				}
			} else {
				hcur = hdef;
			}

			{ 
				double t_full = tcur + hcur;
				if(t_full > tend) {
					hcur = (tend - tcur);
					tcur = tend;				
				}
				else
					tcur = t_full;
			}

			/* Integrate a step */
			fmi3Status = fmi_get_derivatives(inst, states_der, N_STATES);
			for (k = 0; k < N_STATES; k++) {
				states[k] = states[k] + hcur*states_der[k];	
				/* if (k == 0) printf("states[%u] = %f states_der[k] = %f hcur =%f\n", k, states[k], states_der[k], hcur); */
			}

			/* Set states */
			fmi3Status = fmi_set_continuous_states(inst, states, N_STATES);
			/* Step is complete */
			fmi3Status = fmi_completed_integrator_step(inst, fmi3True,
                            &callEventUpdate, &terminateSimulation);
            
            if(fmi3Status != fmi3OK) break;

		}
		for (k = 0; k < N_STATES; k++) { /* Update states */
			inst->reals[k] = inst->states[k];
		}
		return fmi3OK;
	}
}

fmi3Status fmi_activate_model_partition(fmi3Instance instance,
                                        fmi3ValueReference clockReference,
                                        size_t clockElementIndex,
                                        fmi3Float64 activationTime)
{
	instance_ptr_t inst	= instance;

	if (inst == NULL) {
		return fmi3Fatal;
    }

    /* just test that fmi3CallbackIntermediateUpdate can be called */
    {
        fmi3Boolean earlyReturnRequested;
        fmi3Float64 earlyReturnTime;

        inst->cb.intermediateUpdate(
                inst->cb.instanceEnvironment,
                inst->fmitime, /* intermediateUpdateTime */
                fmi3False,    /* eventOccurred */
                fmi3False,    /* clocksTicked */
                fmi3False,    /* intermediateVariableSetAllowed */
                fmi3False,    /* intermediateVariableGetAllowed */
                fmi3False,    /* intermediateStepFinished */
                fmi3False,    /* canReturnEarly */
                &earlyReturnRequested,
                &earlyReturnTime);
    }

    return fmi3OK;
}
