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

#include <fmu_dummy/fmu2_model.h>

/* Model calculation functions */
static int calc_initialize(component_ptr_t comp)
{
	comp->states[VAR_R_HIGHT]		= 1.0;
	comp->states[VAR_R_HIGHT_SPEED] = 4;
	comp->reals	[VAR_R_GRATIVY]		= -9.81;
	comp->reals	[VAR_R_BOUNCE_CONF]	= 0.5;
	if(comp->loggingOn) {
		comp->functions.logger(comp->functions.componentEnvironment, comp->instanceName, fmiOK, "INFO", "###### Initializing component ######");
		comp->functions.logger(comp->functions.componentEnvironment, comp->instanceName, fmiOK, "INFO", "Init #r%d#=%g", VAR_R_HIGHT, comp->states[VAR_R_HIGHT]);
		comp->functions.logger(comp->functions.componentEnvironment, comp->instanceName, fmiOK, "INFO", "Init #r%d#=%g",VAR_R_HIGHT_SPEED, comp->states[VAR_R_HIGHT_SPEED]);
		comp->functions.logger(comp->functions.componentEnvironment, comp->instanceName, fmiOK, "INFO", "Init #r%d#=%g",VAR_R_GRATIVY, comp->reals	[VAR_R_GRATIVY]);
		comp->functions.logger(comp->functions.componentEnvironment, comp->instanceName, fmiOK, "INFO", "Init #r%d#=%g",VAR_R_BOUNCE_CONF, comp->reals	[VAR_R_BOUNCE_CONF]);
		comp->functions.logger(comp->functions.componentEnvironment, comp->instanceName, fmiOK, "ERROR", "Bad reference: #r-1#");
		comp->functions.logger(comp->functions.componentEnvironment, comp->instanceName, fmiOK, "ERROR", "Bad reference: #r1");
		comp->functions.logger(comp->functions.componentEnvironment, comp->instanceName, fmiOK, "ERROR", "Bad reference: #t1#");
		comp->functions.logger(comp->functions.componentEnvironment, comp->instanceName, fmiOK, "ERROR", "Bad reference: #r10#");
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
	fmiReal event_tol = 1e-16;
	comp->event_indicators[EVENT_HIGHT]		= comp->states[VAR_R_HIGHT] + (comp->states[VAR_R_HIGHT] >= 0 ? event_tol : -event_tol);
	return 0;
}

static int calc_event_update(component_ptr_t comp)
{	
	if (comp->states[VAR_R_HIGHT] < 0) {
		comp->states[VAR_R_HIGHT_SPEED] = - comp->reals[VAR_R_BOUNCE_CONF] * comp->states[VAR_R_HIGHT_SPEED];
		comp->states[VAR_R_HIGHT] = 0;

		comp->eventInfo.newDiscreteStatesNeeded			= fmiFalse;
		comp->eventInfo.terminateSimulation			= fmiFalse;
		comp->eventInfo.nominalsOfContinuousStatesChanged	= fmiFalse;
		comp->eventInfo.valuesOfContinuousStatesChanged		= fmiTrue;
		comp->eventInfo.nextEventTimeDefined			= fmiFalse;
		comp->eventInfo.nextEventTime				= -0.0;
		return 0;
	} else {
		return 1; /* Should not call the event update */
	}
}


/* FMI 2.0 Common Functions */
const char* fmi_get_version()
{
	return FMI_VERSION;
}

fmiStatus fmi_set_debug_logging(fmiComponent c, fmiBoolean loggingOn)
{
	component_ptr_t comp = (fmiComponent)c;
	if (comp == NULL) {
		return fmiFatal;
	} else {
		comp->loggingOn = loggingOn;
		return fmiOK;
	}
}

fmiStatus fmi_get_real(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiReal value[])
{
	component_ptr_t comp = (fmiComponent)c;
	if (comp == NULL) {
		return fmiFatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {
			fmiValueReference cvr = vr[k];
			if (cvr < N_STATES) {
				value[k] = comp->states[cvr];
			} 
			else if(cvr == 4) {
				calc_get_derivatives(comp);
				value[k] = comp->states_der[1];
			}
			else {
				value[k] = comp->reals[cvr];
			}	
		}
		return fmiOK;
	}
}

fmiStatus fmi_get_integer(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiInteger value[])
{
	component_ptr_t comp = (fmiComponent)c;
	if (comp == NULL) {
		return fmiFatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {
			value[k] = comp->integers[vr[k]];
		}
		return fmiOK;
	}
}

fmiStatus fmi_get_boolean(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiBoolean value[])
{
	component_ptr_t comp = (fmiComponent)c;
	if (comp == NULL) {
		return fmiFatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {
			value[k] = comp->booleans[vr[k]];
		}
		return fmiOK;
	}
}

fmiStatus fmi_get_string(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiString  value[])
{
	component_ptr_t comp = (fmiComponent)c;
	if (comp == NULL) {
		return fmiFatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {
			value[k] = comp->strings[vr[k]];
		}
		return fmiOK;
	}
}

fmiStatus fmi_set_real(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiReal value[])
{
	component_ptr_t comp = (fmiComponent)c;
	if (comp == NULL) {
		return fmiFatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {
			fmiValueReference cvr = vr[k];
			if (cvr < N_STATES) {
				comp->states[cvr] = value[k]; 
			} 
			else if(cvr == 4) {
				comp->functions.logger(c, comp->instanceName,fmiWarning, "WARNING", "Cannot set acceleration value (calculated)");
				return fmiError;
			}
			else {
				comp->reals[cvr] = value[k]; 
			}			
		}
		return fmiOK;
	}
}

fmiStatus fmi_set_integer(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiInteger value[])
{
	component_ptr_t comp = (fmiComponent)c;
	if (comp == NULL) {
		return fmiFatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {
			comp->integers[vr[k]] = value[k]; 
		}
		return fmiOK;
	}
}

fmiStatus fmi_set_boolean(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiBoolean value[])
{
	component_ptr_t comp = (fmiComponent)c;
	if (comp == NULL) {
		return fmiFatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {
			comp->booleans[vr[k]] = value[k]; 
		}
		return fmiOK;
	}
}

fmiStatus fmi_set_string(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiString  value[])
{
	component_ptr_t comp = (fmiComponent)c;
	if (comp == NULL) {
		return fmiFatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {			
			size_t len;
			fmiString s_dist;
			fmiString s_src = value[k];

			len = strlen((char*)s_src) + 1;
			s_dist = comp->functions.allocateMemory(len, sizeof(char));
			if (s_dist == NULL) {
				return fmiFatal;
			}			
			strcpy((char*)s_dist, (char*)s_src);
			if(comp->strings[vr[k]]) {
				comp->functions.freeMemory((void*)comp->strings[vr[k]]);
			}
			comp->strings[vr[k]] = s_dist;
		}

		/******* Logger test *******/
		if(comp->loggingOn == fmiTrue) {
			for (k = 0; k < nvr; k++) {
				fmiValueReference cvr = vr[k];
				if (cvr == VAR_S_LOGGER_TEST) {
					comp->functions.logger(comp->functions.componentEnvironment, comp->instanceName, fmiFatal, "INFO", "%s",value[k]);
				}
			}
		}
		/******* End of logger test *******/
		return fmiOK;
	}
}

/* FMI 2.0 ME Functions */
const char* fmi_get_model_types_platform()
{
	return fmiTypesPlatform;
}

/* static FILE* find_string(FILE* fp, char* str, int len) {

} */

fmiComponent fmi_instantiate(fmiString instanceName, fmiType fmuType,
  fmiString fmuGUID, fmiString fmuLocation,
  const fmiCallbackFunctions *functions, fmiBoolean visible,
  fmiBoolean loggingOn)
{
	component_ptr_t comp;
	int k, p;

	comp = (component_ptr_t)functions->allocateMemory(1, sizeof(component_t));
	if (comp == NULL) {
		return NULL;
	} else if (strcmp(fmuGUID, FMI_GUID) != 0) {
		return NULL;
	} else {	
		sprintf(comp->instanceName, "%s", instanceName);
		sprintf(comp->GUID, "%s",fmuGUID);
		comp->functions		= *functions;
		comp->loggingOn		= loggingOn;

		comp->callEventUpdate = fmiFalse;

		/* Set default values */
		for (k = 0; k < N_STATES;			k++) comp->states[k]			= 0.0;
		for (k = 0; k < N_STATES;			k++) comp->states_prev[k]		= 0.0; /* Used in CS only */
		for (k = 0; k < N_STATES;			k++) comp->states_nom[k]		= 1.0;
		for (k = 0; k < N_STATES;			k++) comp->states_vr[k]			= k;
		for (k = 0; k < N_STATES;			k++) comp->states_der[k]		= 0.0;
		for (k = 0; k < N_EVENT_INDICATORS; k++) comp->event_indicators[k]	= 1e10;
		for (k = 0; k < N_REAL;				k++) comp->reals[k]				= 0.0;
		for (k = 0; k < N_INTEGER;			k++) comp->integers[k]			= 0;
		for (k = 0; k < N_BOOLEAN;			k++) comp->booleans[k]			= fmiFalse;
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

void fmi_free_instance(fmiComponent c)
{
	int i;
	component_ptr_t comp = (fmiComponent)c;
	for(i = 0; i < N_STRING; i++) {
		comp->functions.freeMemory((void*)(comp->strings[i]));
		comp->strings[i] = 0;
	}
	comp->functions.freeMemory(c);
}

fmiStatus fmi_setup_experiment(fmiComponent c, fmiBoolean toleranceDefined,
                               fmiReal tolerance, fmiReal startTime,
                               fmiBoolean stopTimeDefined,
                               fmiReal stopTime)
{
    component_ptr_t comp = (fmiComponent)c;

    if (comp == NULL) {
        return fmiFatal;
    } else {
        comp->toleranceControlled = toleranceDefined;
        comp->relativeTolerance = tolerance;

        comp->tStart = startTime;
        comp->StopTimeDefined = stopTimeDefined;
        comp->tStop = stopTime;

        return fmiOK;
    }
}

fmiStatus fmi_enter_initialization_mode(fmiComponent c)
{
    if (c == NULL) {
        return fmiFatal;
    } else {
        calc_initialize(c);
        return fmiOK;
    }
}

fmiStatus fmi_exit_initialization_mode(fmiComponent c)
{
    return fmiOK;
}

fmiStatus fmi_enter_event_mode(fmiComponent c)
{
    /* Dummy implementation. TODO: implement */
    return fmiOK;
}

fmiStatus fmi_new_discrete_states(fmiComponent c, fmiEventInfo* eventInfo)
{
    /* Dummy implementation. TODO: implement */
    return fmiOK;
}

fmiStatus fmi_enter_continuous_time_mode(fmiComponent c)
{
    /* Dummy implementation. TODO: implement */
    return fmiOK;
}

fmiStatus fmi_set_time(fmiComponent c, fmiReal fmitime)
{
	component_ptr_t comp = (fmiComponent)c;
	if (comp == NULL) {
		return fmiFatal;
	} else {
		comp->fmitime = fmitime;
		return fmiOK;
	}
}

fmiStatus fmi_set_continuous_states(fmiComponent c, const fmiReal x[], size_t nx)
{
	component_ptr_t comp = (fmiComponent)c;
	if (comp == NULL) {
		return fmiFatal;
	} else {
		size_t k;
		for (k = 0; k < nx; k++) {
			comp->states[k] = x[k];
		}
		return fmiOK;
	}
}

fmiStatus fmi_completed_integrator_step(fmiComponent c,
  fmiBoolean noSetFMUStatePriorToCurrentPoint,
  fmiBoolean* enterEventMode, fmiBoolean* terminateSimulation)
{
	component_ptr_t comp = (fmiComponent)c;
	if (comp == NULL) {
		return fmiFatal;
	} else {
		*enterEventMode = comp->callEventUpdate;
		return fmiOK;
	}
}

fmiStatus fmi_get_derivatives(fmiComponent c, fmiReal derivatives[] , size_t nx)
{
	component_ptr_t comp = (fmiComponent)c;
	if (comp == NULL) {
		return fmiFatal;
	} else {
		size_t k;

		calc_get_derivatives(comp);

		for (k = 0; k < nx; k++) {
			derivatives[k] = comp->states_der[k];
		}
		return fmiOK;
	}
}

fmiStatus fmi_get_event_indicators(fmiComponent c, fmiReal eventIndicators[], size_t ni)
{
	component_ptr_t comp = (fmiComponent)c;
	if (comp == NULL) {
		return fmiFatal;
	} else {
		size_t k;

		calc_get_event_indicators(comp);

		for (k = 0; k < ni; k++) {
			eventIndicators[k] = comp->event_indicators[k];
		}
		return fmiOK;
	}
}

fmiStatus fmi_event_update(fmiComponent c, fmiBoolean intermediateResults, fmiEventInfo* eventInfo)
{
	component_ptr_t comp = (fmiComponent)c;
	if (comp == NULL) {
		return fmiFatal;
	} else {
		calc_event_update(comp);

		*eventInfo = comp->eventInfo;
		return fmiOK;
	}
}

fmiStatus fmi_get_continuous_states(fmiComponent c, fmiReal states[], size_t nx)
{
	component_ptr_t comp = (fmiComponent)c;
	if (comp == NULL) {
		return fmiFatal;
	} else {
		size_t k;

		for (k = 0; k < nx; k++) {
			states[k] = comp->states[k];
		}
		return fmiOK;
	}
}

fmiStatus fmi_get_nominals_of_continuousstates(fmiComponent c, fmiReal x_nominal[], size_t nx)
{
	component_ptr_t comp = (fmiComponent)c;
	if (comp == NULL) {
		return fmiFatal;
	} else {
		size_t k;
		for (k = 0; k < nx; k++) {
			x_nominal[k] = comp->states_nom[k];
		}
		return fmiOK;
	}
}

fmiStatus fmi_get_state_value_references(fmiComponent c, fmiValueReference vrx[], size_t nx)
{
	component_ptr_t comp = (fmiComponent)c;
	if (comp == NULL) {
		return fmiFatal;
	} else {
		size_t k;
		for (k = 0; k < nx; k++) {
			vrx[k] = comp->states_vr[k];
		}
		return fmiOK;
	}
}

fmiStatus fmi_terminate(fmiComponent c)
{
	component_ptr_t comp = (fmiComponent)c;
	if (comp == NULL) {
		return fmiFatal;
	} else {
		return fmiOK;
	}
}

/* FMI 2.0 CS Functions */
const char* fmi_get_types_platform()
{
	return fmiTypesPlatform;
}

fmiStatus fmi_reset(fmiComponent c)
{
	return fmiOK;
}

fmiStatus fmi_set_real_input_derivatives(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiInteger order[], const fmiReal value[])
{

	component_ptr_t comp	= (fmiComponent)c;
	size_t k;

	for (k = 0; k < nvr; k++) {
		comp->input_real[vr[k]][order[k]] = value[k];
		if (value[k] != MAGIC_TEST_VALUE) {/* Tests that the value is set to MAGIC_TEST_VALUE */
			return fmiFatal;
		}
	}

	return fmiOK;
}

fmiStatus fmi_get_real_output_derivatives(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiInteger order[], fmiReal value[])
{
	component_ptr_t comp	= (fmiComponent)c;
	size_t k;

	for (k = 0; k < nvr; k++) {
		value[k] = comp->output_real[vr[k]][order[k]];
	}

	return fmiOK;
}

fmiStatus fmi_cancel_step(fmiComponent c)
{
	return fmiOK;
}

fmiStatus fmi_do_step(fmiComponent c, fmiReal currentCommunicationPoint, fmiReal communicationStepSize, fmiBoolean newStep)
{
	component_ptr_t comp	= (fmiComponent)c;

	if (comp == NULL) {
		return fmiFatal;
	} else {
		fmiReal tstart = currentCommunicationPoint;
		fmiReal tcur;
		fmiReal tend = currentCommunicationPoint + communicationStepSize;
		fmiReal hcur; 
		fmiReal hdef = 0.01;	/* Default time step length */
		fmiReal z_cur[N_EVENT_INDICATORS];
		fmiReal z_pre[N_EVENT_INDICATORS];
		fmiReal states[N_STATES];
		fmiReal states_der[N_STATES];
		fmiEventInfo eventInfo;
		fmiBoolean callEventUpdate;
		fmiBoolean terminateSimulation;
		fmiBoolean intermediateResults = fmiFalse;
		fmiStatus fmistatus;	
		size_t k;
		size_t counter = 0;

		fmi_get_continuous_states(comp, states, N_STATES);
		fmi_get_event_indicators(comp, z_pre, N_EVENT_INDICATORS);

		tcur = tstart;
		hcur = hdef;
		callEventUpdate = fmiFalse;
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
				fmistatus = fmi_event_update(comp, intermediateResults, &eventInfo);
				fmistatus = fmi_get_continuous_states(comp, states, N_STATES);
				fmistatus = fmi_get_event_indicators(comp, z_cur, N_EVENT_INDICATORS);
				fmistatus = fmi_get_event_indicators(comp, z_pre, N_EVENT_INDICATORS);
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
			fmistatus = fmi_get_derivatives(comp, states_der, N_STATES);
			for (k = 0; k < N_STATES; k++) {
				states[k] = states[k] + hcur*states_der[k];	
				/* if (k == 0) printf("states[%u] = %f states_der[k] = %f hcur =%f\n", k, states[k], states_der[k], hcur); */
			}

			/* Set states */
			fmistatus = fmi_set_continuous_states(comp, states, N_STATES);
			/* Step is complete */
			fmistatus = fmi_completed_integrator_step(comp, fmiTrue,
                            &callEventUpdate, &terminateSimulation);
            
            if(fmistatus != fmiOK) break;

		}
		for (k = 0; k < N_STATES; k++) { /* Update states */
			comp->reals[k] = comp->states[k];
		}
		return fmiOK;
	}
}

fmiStatus fmi_get_status(fmiComponent c, const fmiStatusKind s, fmiStatus*  value)
{
	switch (s) {
		case fmiDoStepStatus:
			/* Return fmiPending if we are waiting. Otherwise the result from fmiDoStep */
			*value = fmiOK;
			return fmiOK;
		default: /* Not defined for status for this function */
			return fmiDiscard;
	}
}

fmiStatus fmi_get_real_status(fmiComponent c, const fmiStatusKind s, fmiReal*    value)
{
	switch (s) {
		case fmiLastSuccessfulTime:
			/* Return fmiPending if we are waiting. Otherwise return end time for last call to fmiDoStep */
			*value = 0.01;
			return fmiOK;
		default: /* Not defined for status for this function */
			return fmiDiscard;
	}
}

fmiStatus fmi_get_integer_status(fmiComponent c, const fmiStatusKind s, fmiInteger* value)
{
	switch (s) {
		default: /* Not defined for status for this function */
			return fmiDiscard;
	}
}

fmiStatus fmi_get_boolean_status(fmiComponent c, const fmiStatusKind s, fmiBoolean* value)
{
	switch (s) {
		default: /* Not defined for status for this function */
			return fmiDiscard;
	}
}

fmiStatus fmi_get_string_status(fmiComponent c, const fmiStatusKind s, fmiString*  value)
{
	switch (s) {
		case fmiPendingStatus:
			*value = "Did fmiDoStep really return with fmiPending? Then its time to implement this function";
			return fmiDiscard;
		default: /* Not defined for status for this function */
			return fmiDiscard;
	}
}
