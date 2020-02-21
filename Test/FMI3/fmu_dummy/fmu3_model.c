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

#include <fmu_dummy/fmu3_model.h>

/* Model calculation functions */
static int calc_initialize(component_ptr_t comp)
{
	comp->states[VAR_R_HIGHT]		= 1.0;
	comp->states[VAR_R_HIGHT_SPEED] = 4;
	comp->reals	[VAR_R_GRATIVY]		= -9.81;
	comp->reals	[VAR_R_BOUNCE_CONF]	= 0.5;
	if(comp->loggingOn) {
		comp->functions->logger(comp->functions->componentEnvironment, comp->instanceName, fmi3OK, "INFO", "###### Initializing component ######");
		comp->functions->logger(comp->functions->componentEnvironment, comp->instanceName, fmi3OK, "INFO", "Init #r%d#=%g", VAR_R_HIGHT, comp->states[VAR_R_HIGHT]);
		comp->functions->logger(comp->functions->componentEnvironment, comp->instanceName, fmi3OK, "INFO", "Init #r%d#=%g",VAR_R_HIGHT_SPEED, comp->states[VAR_R_HIGHT_SPEED]);
		comp->functions->logger(comp->functions->componentEnvironment, comp->instanceName, fmi3OK, "INFO", "Init #r%d#=%g",VAR_R_GRATIVY, comp->reals	[VAR_R_GRATIVY]);
		comp->functions->logger(comp->functions->componentEnvironment, comp->instanceName, fmi3OK, "INFO", "Init #r%d#=%g",VAR_R_BOUNCE_CONF, comp->reals	[VAR_R_BOUNCE_CONF]);
/*		comp->functions->logger(comp->functions->componentEnvironment, comp->instanceName, fmi3OK, "ERROR", "Bad reference: #r-1#");
		comp->functions->logger(comp->functions->componentEnvironment, comp->instanceName, fmi3OK, "ERROR", "Bad reference: #r1");
		comp->functions->logger(comp->functions->componentEnvironment, comp->instanceName, fmi3OK, "ERROR", "Bad reference: #t1#");
		comp->functions->logger(comp->functions->componentEnvironment, comp->instanceName, fmi3OK, "ERROR", "Bad reference: #r10#");*/
	}
	return 0;
}

static int calc_get_derivatives(component_ptr_t comp)
{
	comp->states_der[VAR_R_HIGHT]		= comp->states[VAR_R_HIGHT_SPEED];
	comp->states_der[VAR_R_HIGHT_SPEED] = comp->reals[VAR_R_GRATIVY];
	return 0;
}

static int calc_get_event_indicators(component_ptr_t comp)
{	
	fmi3Float64 event_tol = 1e-16;
	comp->event_indicators[EVENT_HIGHT]		= comp->states[VAR_R_HIGHT] + (comp->states[VAR_R_HIGHT] >= 0 ? event_tol : -event_tol);
	return 0;
}

static int calc_event_update(component_ptr_t comp)
{	
    comp->eventInfo.newDiscreteStatesNeeded           = fmi3False;
    comp->eventInfo.terminateSimulation               = fmi3False;
    comp->eventInfo.nominalsOfContinuousStatesChanged = fmi3False;
    comp->eventInfo.nextEventTimeDefined              = fmi3False;
    comp->eventInfo.nextEventTime                     = -0.0;
	if ((comp->states[VAR_R_HIGHT] < 0) && (comp->states[VAR_R_HIGHT_SPEED] < 0)) {
		comp->states[VAR_R_HIGHT_SPEED] = - comp->reals[VAR_R_BOUNCE_CONF] * comp->states[VAR_R_HIGHT_SPEED];
		comp->states[VAR_R_HIGHT] = 0;

        comp->eventInfo.valuesOfContinuousStatesChanged = fmi3True;
		return 0;
	} else {
        comp->eventInfo.valuesOfContinuousStatesChanged = fmi3False;
		return 1; /* Should not call the event update */
	}
}


/* FMI 2.0 Common Functions */
const char* fmi_get_version()
{
	return FMI_VERSION;
}

fmi3Status fmi_set_debug_logging(fmi3Component c, fmi3Boolean loggingOn)
{
	component_ptr_t comp = (fmi3Component)c;
	if (comp == NULL) {
		return fmi3Fatal;
	} else {
		comp->loggingOn = loggingOn;
		return fmi3OK;
	}
}

fmi3Status fmi_get_float64(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, fmi3Float64 value[], size_t nValues)
{
	component_ptr_t comp = (fmi3Component)c;
	if (comp == NULL) {
		return fmi3Fatal;
	} else {
		size_t k;
		size_t m = 0; /* index in value, considering that arrays will require many indices */
		for (k = 0; k < nvr; k++) {
			fmi3ValueReference cvr = vr[k];
			if (cvr < N_STATES) {
				value[m] = comp->states[cvr];
			} 
			else if(cvr == 4) {
				calc_get_derivatives(comp);
				value[m] = comp->states_der[1];
			}
			else if (cvr == 12) { /* special case: array */
				calc_get_derivatives(comp);
				value[m++] = comp->states[0];
				value[m++] = comp->states_der[0];
				value[m++] = comp->states[1];
				value[m++] = comp->states_der[1];
			}
			else {
				value[m] = comp->reals[cvr];
			}
			m++;
		}

		if (m - 1 != nValues) {
            return fmi3Fatal;
		}

		return fmi3OK;
	}
}

fmi3Status fmi_get_integer(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, fmi3Integer value[])
{
	component_ptr_t comp = (fmi3Component)c;
	if (comp == NULL) {
		return fmi3Fatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {
			value[k] = comp->integers[vr[k]];
		}
		return fmi3OK;
	}
}

fmi3Status fmi_get_boolean(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, fmi3Boolean value[])
{
	component_ptr_t comp = (fmi3Component)c;
	if (comp == NULL) {
		return fmi3Fatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {
			value[k] = comp->booleans[vr[k]];
		}
		return fmi3OK;
	}
}

fmi3Status fmi_get_string(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, fmi3String  value[])
{
	component_ptr_t comp = (fmi3Component)c;
	if (comp == NULL) {
		return fmi3Fatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {
			value[k] = comp->strings[vr[k]];
		}
		return fmi3OK;
	}
}

/* TODO: nValues is just used for verification, i.e. to check that sum(lenght(vr[i])) == nValues;
 * currently it seems it's not decided whether this parameter will remain, so not implementing anything
 * for now
 * https://github.com/modelica/fmi-standard/issues/512
 */
fmi3Status fmi_set_float64(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, const fmi3Float64 value[], size_t nValues)
{
	component_ptr_t comp = (fmi3Component)c;
	if (comp == NULL) {
		return fmi3Fatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {
			fmi3ValueReference cvr = vr[k];
			if (cvr < N_STATES) {
				comp->states[cvr] = value[k]; 
			} 
			else if(cvr == 4) {
				comp->functions->logger(c, comp->instanceName, fmi3Warning, "WARNING", "Cannot set acceleration value (calculated)");
				return fmi3Error;
			}
			else {
				comp->reals[cvr] = value[k]; 
			}
		}
		return fmi3OK;
	}
}

fmi3Status fmi_set_integer(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, const fmi3Integer value[])
{
	component_ptr_t comp = (fmi3Component)c;
	if (comp == NULL) {
		return fmi3Fatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {
			comp->integers[vr[k]] = value[k]; 
		}
		return fmi3OK;
	}
}

fmi3Status fmi_set_boolean(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, const fmi3Boolean value[])
{
	component_ptr_t comp = (fmi3Component)c;
	if (comp == NULL) {
		return fmi3Fatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {
			comp->booleans[vr[k]] = value[k]; 
		}
		return fmi3OK;
	}
}

fmi3Status fmi_set_string(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, const fmi3String  value[])
{
	component_ptr_t comp = (fmi3Component)c;
	if (comp == NULL) {
		return fmi3Fatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {			
			size_t len;
			fmi3String s_dist;
			fmi3String s_src = value[k];

			len = strlen((char*)s_src) + 1;
			s_dist = comp->functions->allocateMemory(len, sizeof(char));
			if (s_dist == NULL) {
				return fmi3Fatal;
			}			
			strcpy((char*)s_dist, (char*)s_src);
			if(comp->strings[vr[k]]) {
				comp->functions->freeMemory((void*)comp->strings[vr[k]]);
			}
			comp->strings[vr[k]] = s_dist;
		}

		/******* Logger test *******/
		if(comp->loggingOn == fmi3True) {
			for (k = 0; k < nvr; k++) {
				fmi3ValueReference cvr = vr[k];
				if (cvr == VAR_S_LOGGER_TEST) {
					comp->functions->logger(comp->functions->componentEnvironment, comp->instanceName, fmi3Fatal, "INFO", "%s",value[k]);
				}
			}
		}
		/******* End of logger test *******/
		return fmi3OK;
	}
}

/* FMI 2.0 ME Functions */
const char* fmi_get_model_types_platform()
{
	return fmi3TypesPlatform;
}

/* static FILE* find_string(FILE* fp, char* str, int len) {

} */

fmi3Component fmi_instantiate(fmi3String instanceName, fmi3Type fmuType,
  fmi3String fmuInstantiationToken, fmi3String fmuLocation,
  const fmi3CallbackFunctions *functions, fmi3Boolean visible,
  fmi3Boolean loggingOn)
{
	component_ptr_t comp;
	int k, p;

	comp = (component_ptr_t)functions->allocateMemory(1, sizeof(component_t));
	if (comp == NULL) {
		return NULL;
	} else if (strcmp(fmuInstantiationToken, FMI_INSTANTIATION_TOKEN) != 0) {
		return NULL;
	} else {	
		sprintf(comp->instanceName, "%s", instanceName);
		sprintf(comp->instantiationToken, "%s",fmuInstantiationToken);
		comp->functions		= functions;
		/*comp->functions->allocateMemory = functions->allocateMemory;*/
		
		comp->loggingOn		= loggingOn;

		/* Set default values */
		for (k = 0; k < N_STATES;			k++) comp->states[k]			= 0.0;
		for (k = 0; k < N_STATES;			k++) comp->states_prev[k]		= 0.0; /* Used in CS only */
		for (k = 0; k < N_STATES;			k++) comp->states_nom[k]		= 1.0;
		for (k = 0; k < N_STATES;			k++) comp->states_der[k]		= 0.0;
		for (k = 0; k < N_EVENT_INDICATORS; k++) comp->event_indicators[k]	= 1e10;
		for (k = 0; k < N_REAL;				k++) comp->reals[k]				= 0.0;
		for (k = 0; k < N_INTEGER;			k++) comp->integers[k]			= 0;
		for (k = 0; k < N_BOOLEAN;			k++) comp->booleans[k]			= fmi3False;
		for (k = 0; k < N_STRING;			k++) comp->strings[k]			= NULL;

		/* Used in CS only */
		for (k = 0; k < N_INPUT_REAL; k++) {
			for (p = 0; p < N_INPUT_REAL_MAX_ORDER + 1; p++) {
				comp->input_real[k][p] = 0.0;
			}
		}

		/* Used in CS only */
		for (k = 0; k < N_OUTPUT_REAL; k++) {
			for (p = 0; p < N_OUTPUT_REAL_MAX_ORDER + 1; p++) {
				comp->output_real[k][p] = MAGIC_TEST_VALUE;
			}
		}
	
		sprintf(comp->fmuLocation, "%s",fmuLocation);
		comp->visible		= visible;
		return comp;
	}
}

void fmi_free_instance(fmi3Component c)
{
	int i;
	component_ptr_t comp = (fmi3Component)c;
	for(i = 0; i < N_STRING; i++) {
		comp->functions->freeMemory((void*)(comp->strings[i]));
		comp->strings[i] = 0;
	}
	comp->functions->freeMemory(c);
}

fmi3Status fmi_setup_experiment(fmi3Component c, fmi3Boolean toleranceDefined,
                               fmi3Float64 tolerance, fmi3Float64 startTime,
                               fmi3Boolean stopTimeDefined,
                               fmi3Float64 stopTime)
{
    component_ptr_t comp = (fmi3Component)c;

    if (comp == NULL) {
        return fmi3Fatal;
    } else {
        comp->toleranceControlled = toleranceDefined;
        comp->relativeTolerance = tolerance;

        comp->tStart = startTime;
        comp->StopTimeDefined = stopTimeDefined;
        comp->tStop = stopTime;

        return fmi3OK;
    }
}

fmi3Status fmi_enter_initialization_mode(fmi3Component c)
{
    if (c == NULL) {
        return fmi3Fatal;
    } else {
        calc_initialize(c);
        return fmi3OK;
    }
}

fmi3Status fmi_exit_initialization_mode(fmi3Component c)
{
    return fmi3OK;
}

fmi3Status fmi_enter_event_mode(fmi3Component c)
{
    return fmi3OK;
}

fmi3Status fmi_new_discrete_states(fmi3Component c, fmi3EventInfo* eventInfo)
{
	component_ptr_t comp = (fmi3Component)c;
	if (comp == NULL) {
		return fmi3Fatal;
	} else {
		calc_event_update(comp);

		*eventInfo = comp->eventInfo;
		return fmi3OK;
	}
}

fmi3Status fmi_enter_continuous_time_mode(fmi3Component c)
{
    return fmi3OK;
}

fmi3Status fmi_set_time(fmi3Component c, fmi3Float64 fmitime)
{
	component_ptr_t comp = (fmi3Component)c;
	if (comp == NULL) {
		return fmi3Fatal;
	} else {
		comp->fmitime = fmitime;
		return fmi3OK;
	}
}

fmi3Status fmi_set_continuous_states(fmi3Component c, const fmi3Float64 x[], size_t nx)
{
	component_ptr_t comp = (fmi3Component)c;
	if (comp == NULL) {
		return fmi3Fatal;
	} else {
		size_t k;
		for (k = 0; k < nx; k++) {
			comp->states[k] = x[k];
		}
		return fmi3OK;
	}
}

fmi3Status fmi_completed_integrator_step(fmi3Component c,
  fmi3Boolean noSetFMUStatePriorToCurrentPoint,
  fmi3Boolean* enterEventMode, fmi3Boolean* terminateSimulation)
{
	component_ptr_t comp = (fmi3Component)c;
	if (comp == NULL) {
		return fmi3Fatal;
	} else {
		*enterEventMode = fmi3False;
		return fmi3OK;
	}
}

fmi3Status fmi_get_derivatives(fmi3Component c, fmi3Float64 derivatives[] , size_t nx)
{
	component_ptr_t comp = (fmi3Component)c;
	if (comp == NULL) {
		return fmi3Fatal;
	} else {
		size_t k;

		calc_get_derivatives(comp);

		for (k = 0; k < nx; k++) {
			derivatives[k] = comp->states_der[k];
		}
		return fmi3OK;
	}
}

fmi3Status fmi_get_event_indicators(fmi3Component c, fmi3Float64 eventIndicators[], size_t ni)
{
	component_ptr_t comp = (fmi3Component)c;
	if (comp == NULL) {
		return fmi3Fatal;
	} else {
		size_t k;

		calc_get_event_indicators(comp);

		for (k = 0; k < ni; k++) {
			eventIndicators[k] = comp->event_indicators[k];
		}
		return fmi3OK;
	}
}

fmi3Status fmi_get_continuous_states(fmi3Component c, fmi3Float64 states[], size_t nx)
{
	component_ptr_t comp = (fmi3Component)c;
	if (comp == NULL) {
		return fmi3Fatal;
	} else {
		size_t k;

		for (k = 0; k < nx; k++) {
			states[k] = comp->states[k];
		}
		return fmi3OK;
	}
}

fmi3Status fmi_get_nominals_of_continuousstates(fmi3Component c, fmi3Float64 x_nominal[], size_t nx)
{
	component_ptr_t comp = (fmi3Component)c;
	if (comp == NULL) {
		return fmi3Fatal;
	} else {
		size_t k;
		for (k = 0; k < nx; k++) {
			x_nominal[k] = comp->states_nom[k];
		}
		return fmi3OK;
	}
}

fmi3Status fmi_terminate(fmi3Component c)
{
	component_ptr_t comp = (fmi3Component)c;
	if (comp == NULL) {
		return fmi3Fatal;
	} else {
		return fmi3OK;
	}
}

/* FMI 2.0 CS Functions */
const char* fmi_get_types_platform()
{
	return fmi3TypesPlatform;
}

fmi3Status fmi_reset(fmi3Component c)
{
	return fmi3OK;
}

fmi3Status fmi_set_real_input_derivatives(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, const fmi3Integer order[], const fmi3Float64 value[])
{

	component_ptr_t comp	= (fmi3Component)c;
	size_t k;

	for (k = 0; k < nvr; k++) {
		comp->input_real[vr[k]][order[k]] = value[k];
		if (value[k] != MAGIC_TEST_VALUE) {/* Tests that the value is set to MAGIC_TEST_VALUE */
			return fmi3Fatal;
		}
	}

	return fmi3OK;
}

fmi3Status fmi_get_real_output_derivatives(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, const fmi3Integer order[], fmi3Float64 value[])
{
	component_ptr_t comp	= (fmi3Component)c;
	size_t k;

	for (k = 0; k < nvr; k++) {
		value[k] = comp->output_real[vr[k]][order[k]];
	}

	return fmi3OK;
}

fmi3Status fmi_cancel_step(fmi3Component c)
{
	return fmi3OK;
}

fmi3Status fmi_do_step(fmi3Component c, fmi3Float64 currentCommunicationPoint, fmi3Float64 communicationStepSize, fmi3Boolean newStep, fmi3Boolean* earlyReturn)
{
	component_ptr_t comp	= (fmi3Component)c;

	if (comp == NULL) {
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
		fmi3EventInfo eventInfo;
		fmi3Boolean callEventUpdate;
		fmi3Boolean terminateSimulation;
		fmi3Status fmi3Status;	
		size_t k;
		size_t counter = 0;

		fmi_get_continuous_states(comp, states, N_STATES);
		fmi_get_event_indicators(comp, z_pre, N_EVENT_INDICATORS);

		tcur = tstart;
		hcur = hdef;
		callEventUpdate = fmi3False;
		eventInfo = comp->eventInfo;

		while (tcur < tend && counter < 100) {
			size_t k;
			int zero_crossning_event = 0;
			counter++;

			fmi_set_time(comp, tcur);
			fmi_get_event_indicators(comp, z_cur, N_EVENT_INDICATORS);

			/* Check if an event inidcator has triggered */
			for (k = 0; k < N_EVENT_INDICATORS; k++) {
				if (z_cur[k]*z_pre[k] < 0) {
					zero_crossning_event = 1;
					break;
				}
			}

			/* Handle any events */
			if (callEventUpdate || zero_crossning_event ||
			  (eventInfo.nextEventTimeDefined && tcur == eventInfo.nextEventTime)) {
				fmi3Status = fmi_new_discrete_states(comp, &eventInfo);
				fmi3Status = fmi_get_continuous_states(comp, states, N_STATES);
				fmi3Status = fmi_get_event_indicators(comp, z_cur, N_EVENT_INDICATORS);
				fmi3Status = fmi_get_event_indicators(comp, z_pre, N_EVENT_INDICATORS);
			}

			/* Updated next time step */
			if (eventInfo.nextEventTimeDefined) {
				if (tcur + hdef < eventInfo.nextEventTime) {
					hcur = hdef;
				} else {
					hcur = eventInfo.nextEventTime - tcur;
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
			fmi3Status = fmi_get_derivatives(comp, states_der, N_STATES);
			for (k = 0; k < N_STATES; k++) {
				states[k] = states[k] + hcur*states_der[k];	
				/* if (k == 0) printf("states[%u] = %f states_der[k] = %f hcur =%f\n", k, states[k], states_der[k], hcur); */
			}

			/* Set states */
			fmi3Status = fmi_set_continuous_states(comp, states, N_STATES);
			/* Step is complete */
			fmi3Status = fmi_completed_integrator_step(comp, fmi3True,
                            &callEventUpdate, &terminateSimulation);
            
            if(fmi3Status != fmi3OK) break;

		}
		for (k = 0; k < N_STATES; k++) { /* Update states */
			comp->reals[k] = comp->states[k];
		}
		return fmi3OK;
	}
}

fmi3Status fmi_get_status(fmi3Component c, const fmi3StatusKind s, fmi3Status*  value)
{
	switch (s) {
		case fmi3DoStepStatus:
			/* Return fmiPending if we are waiting. Otherwise the result from fmiDoStep */
			*value = fmi3OK;
			return fmi3OK;
		default: /* Not defined for status for this function */
			return fmi3Discard;
	}
}

fmi3Status fmi_get_real_status(fmi3Component c, const fmi3StatusKind s, fmi3Float64*    value)
{
	switch (s) {
		case fmi3LastSuccessfulTime:
			/* Return fmiPending if we are waiting. Otherwise return end time for last call to fmiDoStep */
			*value = 0.01;
			return fmi3OK;
		default: /* Not defined for status for this function */
			return fmi3Discard;
	}
}

fmi3Status fmi_get_integer_status(fmi3Component c, const fmi3StatusKind s, fmi3Integer* value)
{
	switch (s) {
		default: /* Not defined for status for this function */
			return fmi3Discard;
	}
}

fmi3Status fmi_get_boolean_status(fmi3Component c, const fmi3StatusKind s, fmi3Boolean* value)
{
	switch (s) {
		default: /* Not defined for status for this function */
			return fmi3Discard;
	}
}

fmi3Status fmi_get_string_status(fmi3Component c, const fmi3StatusKind s, fmi3String*  value)
{
	switch (s) {
		case fmi3PendingStatus:
			*value = "Did fmi3DoStep really return with fmi3Pending? Then its time to implement this function";
			return fmi3Discard;
		default: /* Not defined for status for this function */
			return fmi3Discard;
	}
}
