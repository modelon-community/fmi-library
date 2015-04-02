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

/*Include model specific definition e.g. number of states and values references, as well as all generic FMI function and type declarations -> fmu2_model.h*/
#include "Values_fmu2_model_defines.h"

/* Model calculation functions */
/*Set all default values for the model here. This needs to be separate from the initialization routine, as it might be overwritten altered by the master*/
static int set_default_values(component_ptr_t comp){
	
	/*values for dummies*/
	comp->states[VAR_R_dummy]			= 1.0;
	comp->event_indicators[0]			= 1.0;
	
	/*Set values according to values from xml*/
	comp->booleans	[VAR_B_boolean_input]	= fmi2False;
	comp->integers	[VAR_I_integer_input]	= 1;
	comp->reals		[VAR_R_real_input]		= 3.1426;

	comp->eventInfo.newDiscreteStatesNeeded				= fmi2False;
	comp->eventInfo.terminateSimulation					= fmi2False;
	comp->eventInfo.nominalsOfContinuousStatesChanged	= fmi2False;
	comp->eventInfo.valuesOfContinuousStatesChanged		= fmi2False;
	comp->eventInfo.nextEventTimeDefined				= fmi2False;
	comp->eventInfo.nextEventTime						= -0.0;
 
	return 0;
}

/*Initial settings,values that are calculated and eventually the first time event are to be set here.*/
static int calc_initialize(component_ptr_t comp)
{
	/*Write initial outputs*/
	comp->strings	[VAR_S_string_output]	= "String output deactivated.";
	comp->booleans	[VAR_B_boolean_output]	= comp->booleans[VAR_B_boolean_input];	
	comp->integers	[VAR_I_integer_output]	= comp->integers[VAR_I_integer_input];
	comp->reals		[VAR_R_real_output]		= comp->reals	[VAR_R_real_input];		
	
	if(comp->loggingOn) {
		comp->functions->logger(comp->functions->componentEnvironment, comp->instanceName, fmi2OK, "INFO", "###### Initializing component ######");
		comp->functions->logger(comp->functions->componentEnvironment, comp->instanceName, fmi2OK, "INFO", "Init #r%d#=%g", VAR_R_real_input,comp->reals			[VAR_R_real_input]);
		comp->functions->logger(comp->functions->componentEnvironment, comp->instanceName, fmi2OK, "INFO", "Init #r%d#=%g", VAR_R_real_output, comp->reals			[VAR_R_real_output]);
		comp->functions->logger(comp->functions->componentEnvironment, comp->instanceName, fmi2OK, "INFO", "Init #b%d#=%s", VAR_B_boolean_input, (comp->booleans	[VAR_B_boolean_input])?"fmiTrue":"fmiFalse");
		comp->functions->logger(comp->functions->componentEnvironment, comp->instanceName, fmi2OK, "INFO", "Init #b%d#=%s", VAR_B_boolean_output,(comp->booleans	[VAR_B_boolean_output])?"fmiTrue":"fmiFalse");
		comp->functions->logger(comp->functions->componentEnvironment, comp->instanceName, fmi2OK, "INFO", "Init #i%d#=%i", VAR_I_integer_input, comp->integers	[VAR_I_integer_input]);
		comp->functions->logger(comp->functions->componentEnvironment, comp->instanceName, fmi2OK, "INFO", "Init #i%d#=%i", VAR_I_integer_output, comp->integers	[VAR_I_integer_output]);
		comp->functions->logger(comp->functions->componentEnvironment, comp->instanceName, fmi2OK, "INFO", "Init #s%d#=%s", VAR_S_string_input, comp->strings		[VAR_S_string_input]);
		comp->functions->logger(comp->functions->componentEnvironment, comp->instanceName, fmi2OK, "INFO", "Init #s%d#=%s", VAR_S_string_output, comp->strings		[VAR_S_string_output]);
	}
	return 0;
}

/*Calculation of state derivatives*/
static int calc_get_derivatives(component_ptr_t comp)
{
	/*dummy derivative*/
	comp->states_der[VAR_R_dummy] = 0;
	return 0;
}

/*Calculation of event indicators to verify if state event has happened*/
static int calc_get_event_indicators(component_ptr_t comp)
{	
	/*no state events, therefor nothing to do here.*/

	return 0;
}


/*Calculations for handling an event, e.g. reinitialization of states*/
static int calc_event_update(component_ptr_t comp)
{	
	/*Write output if input has changed.*/
	int eventOn = 0;
	/*Set boolean output, if input has changed */
	if (comp->booleans	[VAR_B_boolean_input] != 	comp->booleans	[VAR_B_boolean_output]) {
		comp->booleans	[VAR_B_boolean_output] = 	comp->booleans	[VAR_B_boolean_input];
		eventOn = 1;
	}

	/*Set integer output, if input has changed */
	if (comp->integers	[VAR_I_integer_input] != comp->integers	[VAR_I_integer_output]){
		comp->integers	[VAR_I_integer_output] = comp->integers	[VAR_I_integer_input];
		eventOn = 1;
	}

	if (eventOn > 0) {
		if(comp->loggingOn) {
			comp->functions->logger(comp->functions->componentEnvironment, comp->instanceName, fmi2OK, "INFO", "Input change triggered event.");
		}
		comp->eventInfo.newDiscreteStatesNeeded           = fmi2False;
		comp->eventInfo.terminateSimulation               = fmi2False;
		comp->eventInfo.nominalsOfContinuousStatesChanged = fmi2False;
		comp->eventInfo.nextEventTimeDefined              = fmi2False;
		comp->eventInfo.nextEventTime                     = -0.0;
		
		return 0;

	} else {
		return 1; /* Should not call the event update */
	}

	return 0;
}


/* FMI 2.0 Common Functions */
const char* fmi_get_version()
{
	return FMI_VERSION;
}

/*Pass on logger switch*/
fmi2Status fmi_set_debug_logging(fmi2Component c, fmi2Boolean loggingOn)
{
	component_ptr_t comp = (fmi2Component)c;
	if (comp == NULL) {
		return fmi2Fatal;
	} else {
		comp->loggingOn = loggingOn;
		return fmi2OK;
	}
}

/*Read current real values*/
fmi2Status fmi_get_real(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Real value[])
{
	component_ptr_t comp = (fmi2Component)c;
	if (comp == NULL) {
		return fmi2Fatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {
			fmi2ValueReference cvr = vr[k];
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
		return fmi2OK;
	}
}

/*Read current integer values*/
fmi2Status fmi_get_integer(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Integer value[])
{
	component_ptr_t comp = (fmi2Component)c;
	if (comp == NULL) {
		return fmi2Fatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {
			value[k] = comp->integers[vr[k]];
		}
		return fmi2OK;
	}
}

/*Read current boolean values*/
fmi2Status fmi_get_boolean(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[])
{
	component_ptr_t comp = (fmi2Component)c;
	if (comp == NULL) {
		return fmi2Fatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {
			value[k] = comp->booleans[vr[k]];
		}
		return fmi2OK;
	}
}

/*Read current string values*/
fmi2Status fmi_get_string(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2String  value[])
{
	component_ptr_t comp = (fmi2Component)c;
	if (comp == NULL) {
		return fmi2Fatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {
			value[k] = comp->strings[vr[k]];
		}
		return fmi2OK;
	}
}

/*Write current real values*/
fmi2Status fmi_set_real(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[])
{
	component_ptr_t comp = (fmi2Component)c;
	if (comp == NULL) {
		return fmi2Fatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {
			fmi2ValueReference cvr = vr[k];
			if (cvr < N_STATES) {
				comp->states[cvr] = value[k]; 
			} 
			else if(cvr == 4) {
				comp->functions->logger(c, comp->instanceName,fmi2Warning, "WARNING", "Cannot set acceleration value (calculated)");
				return fmi2Error;
			}
			else {
				comp->reals[cvr] = value[k]; 
			}			
		}
		return fmi2OK;
	}
}

/*Write current integer values*/
fmi2Status fmi_set_integer(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Integer value[])
{
	component_ptr_t comp = (fmi2Component)c;
	if (comp == NULL) {
		return fmi2Fatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {
			comp->integers[vr[k]] = value[k]; 
		}
		return fmi2OK;
	}
}

/*Write current boolean values*/
fmi2Status fmi_set_boolean(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[])
{
	component_ptr_t comp = (fmi2Component)c;
	if (comp == NULL) {
		return fmi2Fatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {
			comp->booleans[vr[k]] = value[k]; 
		}
		return fmi2OK;
	}
}

/*Write current string values*/
fmi2Status fmi_set_string(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2String  value[])
{
	component_ptr_t comp = (fmi2Component)c;
	if (comp == NULL) {
		return fmi2Fatal;
	} else {
		size_t k;
		for (k = 0; k < nvr; k++) {			
			size_t len;
			fmi2String s_dist;
			fmi2String s_src = value[k];

			len = strlen((char*)s_src) + 1;
			s_dist = comp->functions->allocateMemory(len, sizeof(char));
			if (s_dist == NULL) {
				return fmi2Fatal;
			}			
			strcpy((char*)s_dist, (char*)s_src);
			if(comp->strings[vr[k]]) {
				comp->functions->freeMemory((void*)comp->strings[vr[k]]);
			}
			comp->strings[vr[k]] = s_dist;
		}

		/******* Logger test *******/
		if(comp->loggingOn == fmi2True) {
			for (k = 0; k < nvr; k++) {
				fmi2ValueReference cvr = vr[k];
				if (cvr == VAR_S_LOGGER_TEST) {
					comp->functions->logger(comp->functions->componentEnvironment, comp->instanceName, fmi2Fatal, "INFO", "%s",value[k]);
				}
			}
		}
		/******* End of logger test *******/
		return fmi2OK;
	}
}

/* FMI 2.0 ME Functions */
const char* fmi_get_model_types_platform()
{
	return fmi2TypesPlatform;
}

/* static FILE* find_string(FILE* fp, char* str, int len) {

} */

fmi2Component fmi_instantiate(fmi2String instanceName, fmi2Type fmuType,
  fmi2String fmuGUID, fmi2String fmuLocation,
  const fmi2CallbackFunctions *functions, fmi2Boolean visible,
  fmi2Boolean loggingOn)
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
		for (k = 0; k < N_BOOLEAN;			k++) comp->booleans[k]			= fmi2False;
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

		/*Set default inital values*/
		set_default_values(comp);

		return comp;
	}
}

void fmi_free_instance(fmi2Component c)
{
	int i;
	component_ptr_t comp = (fmi2Component)c;
	for(i = 0; i < N_STRING; i++) {
//		comp->functions->freeMemory((void*)(comp->strings[i]));
//		comp->strings[i] = 0;
	}
	comp->functions->freeMemory(c);
}

fmi2Status fmi_setup_experiment(fmi2Component c, fmi2Boolean toleranceDefined,
                               fmi2Real tolerance, fmi2Real startTime,
                               fmi2Boolean stopTimeDefined,
                               fmi2Real stopTime)
{
    component_ptr_t comp = (fmi2Component)c;

    if (comp == NULL) {
        return fmi2Fatal;
    } else {
        comp->toleranceControlled = toleranceDefined;
        comp->relativeTolerance = tolerance;

        comp->tStart = startTime;
        comp->StopTimeDefined = stopTimeDefined;
        comp->tStop = stopTime;

        return fmi2OK;
    }
}

fmi2Status fmi_enter_initialization_mode(fmi2Component c)
{
    if (c == NULL) {
        return fmi2Fatal;
    } else {
        calc_initialize(c);
        return fmi2OK;
    }
}

fmi2Status fmi_exit_initialization_mode(fmi2Component c)
{
    return fmi2OK;
}

fmi2Status fmi_enter_event_mode(fmi2Component c)
{
    return fmi2OK;
}

fmi2Status fmi_new_discrete_states(fmi2Component c, fmi2EventInfo* eventInfo)
{
	component_ptr_t comp = (fmi2Component)c;
	if (comp == NULL) {
		return fmi2Fatal;
	} else {
		calc_event_update(comp);

		*eventInfo = comp->eventInfo;
		return fmi2OK;
	}
}

fmi2Status fmi_enter_continuous_time_mode(fmi2Component c)
{
    return fmi2OK;
}

fmi2Status fmi_set_time(fmi2Component c, fmi2Real fmitime)
{
	component_ptr_t comp = (fmi2Component)c;
	if (comp == NULL) {
		return fmi2Fatal;
	} else {
		comp->fmitime = fmitime;
		return fmi2OK;
	}
}

fmi2Status fmi_set_continuous_states(fmi2Component c, const fmi2Real x[], size_t nx)
{
	component_ptr_t comp = (fmi2Component)c;
	if (comp == NULL) {
		return fmi2Fatal;
	} else {
		size_t k;
		for (k = 0; k < nx; k++) {
			comp->states[k] = x[k];
		}
		return fmi2OK;
	}
}

fmi2Status fmi_completed_integrator_step(fmi2Component c,
  fmi2Boolean noSetFMUStatePriorToCurrentPoint,
  fmi2Boolean* enterEventMode, fmi2Boolean* terminateSimulation)
{
	component_ptr_t comp = (fmi2Component)c;
	if (comp == NULL) {
		*terminateSimulation = fmi2True;
		return fmi2Fatal;
	} else {
		*enterEventMode = fmi2False;
		*terminateSimulation = comp->eventInfo.terminateSimulation;
		//*terminateSimulation = fmi2False;
		return fmi2OK;
	}
}

fmi2Status fmi_get_derivatives(fmi2Component c, fmi2Real derivatives[] , size_t nx)
{
	component_ptr_t comp = (fmi2Component)c;
	if (comp == NULL) {
		return fmi2Fatal;
	} else {
		size_t k;

		calc_get_derivatives(comp);

		for (k = 0; k < nx; k++) {
			derivatives[k] = comp->states_der[k];
		}
		return fmi2OK;
	}
}

fmi2Status fmi_get_event_indicators(fmi2Component c, fmi2Real eventIndicators[], size_t ni)
{
	component_ptr_t comp = (fmi2Component)c;
	if (comp == NULL) {
		return fmi2Fatal;
	} else {
		size_t k;

		calc_get_event_indicators(comp);

		for (k = 0; k < ni; k++) {
			eventIndicators[k] = comp->event_indicators[k];
		}
		return fmi2OK;
	}
}

fmi2Status fmi_get_continuous_states(fmi2Component c, fmi2Real states[], size_t nx)
{
	component_ptr_t comp = (fmi2Component)c;
	if (comp == NULL) {
		return fmi2Fatal;
	} else {
		size_t k;

		for (k = 0; k < nx; k++) {
			states[k] = comp->states[k];
		}
		return fmi2OK;
	}
}

fmi2Status fmi_get_nominals_of_continuousstates(fmi2Component c, fmi2Real x_nominal[], size_t nx)
{
	component_ptr_t comp = (fmi2Component)c;
	if (comp == NULL) {
		return fmi2Fatal;
	} else {
		size_t k;
		for (k = 0; k < nx; k++) {
			x_nominal[k] = comp->states_nom[k];
		}
		return fmi2OK;
	}
}

fmi2Status fmi_terminate(fmi2Component c)
{
	component_ptr_t comp = (fmi2Component)c;
	if (comp == NULL) {
		return fmi2Fatal;
	} else {
		return fmi2OK;
	}
}

/* FMI 2.0 CS Functions */
const char* fmi_get_types_platform()
{
	return fmi2TypesPlatform;
}

fmi2Status fmi_reset(fmi2Component c)
{
	/*Set values to default again*/
	set_default_values(c);
	return fmi2OK;
}

fmi2Status fmi_set_real_input_derivatives(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Integer order[], const fmi2Real value[])
{

	component_ptr_t comp	= (fmi2Component)c;
	size_t k;

	for (k = 0; k < nvr; k++) {
		comp->input_real[vr[k]][order[k]] = value[k];
		if (value[k] != MAGIC_TEST_VALUE) {/* Tests that the value is set to MAGIC_TEST_VALUE */
			return fmi2Fatal;
		}
	}

	return fmi2OK;
}

fmi2Status fmi_get_real_output_derivatives(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Integer order[], fmi2Real value[])
{
	component_ptr_t comp	= (fmi2Component)c;
	size_t k;

	for (k = 0; k < nvr; k++) {
		value[k] = comp->output_real[vr[k]][order[k]];
	}

	return fmi2OK;
}

fmi2Status fmi_cancel_step(fmi2Component c)
{
	return fmi2OK;
}

fmi2Status fmi_do_step(fmi2Component c, fmi2Real currentCommunicationPoint, fmi2Real communicationStepSize, fmi2Boolean newStep)
{
	component_ptr_t comp	= (fmi2Component)c;

	if (comp == NULL) {
		return fmi2Fatal;
	} else {
		fmi2Real tstart = currentCommunicationPoint;
		fmi2Real tcur;
		fmi2Real tend = currentCommunicationPoint + communicationStepSize;
		fmi2Real hcur; 
		fmi2Real hdef = 0.01;	/* Default time step length */
		fmi2Real z_cur[N_EVENT_INDICATORS];
		fmi2Real z_pre[N_EVENT_INDICATORS];
		fmi2Real states[N_STATES];
		fmi2Real states_der[N_STATES];
		fmi2EventInfo eventInfo;
		fmi2Boolean callEventUpdate;
		fmi2Boolean terminateSimulation;
		fmi2Status fmi2Status;	
		size_t k;
		size_t counter = 0;

		fmi_get_continuous_states(comp, states, N_STATES);
		fmi_get_event_indicators(comp, z_pre, N_EVENT_INDICATORS);

		tcur = tstart;
		hcur = hdef;
		callEventUpdate = fmi2False;
		eventInfo = comp->eventInfo;

		while (tcur < tend && counter < 100 && !comp->eventInfo.terminateSimulation) {
			size_t k;
			int zero_crossning_event = 0;
			counter++;

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

			fmi_set_time(comp, tcur);

			/* Integrate a step */
			fmi2Status = fmi_get_derivatives(comp, states_der, N_STATES);
			for (k = 0; k < N_STATES; k++) {
				states[k] = states[k] + hcur*states_der[k];	
				/* if (k == 0) printf("states[%u] = %f states_der[k] = %f hcur =%f\n", k, states[k], states_der[k], hcur); */
			}

			/* Set states */
			fmi2Status = fmi_set_continuous_states(comp, states, N_STATES);
			/* Step is complete */
			fmi2Status = fmi_completed_integrator_step(comp, fmi2True,
                            &callEventUpdate, &terminateSimulation);
			
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
				fmi2Status = fmi_new_discrete_states(comp, &eventInfo);
				fmi2Status = fmi_get_continuous_states(comp, states, N_STATES);
				fmi2Status = fmi_get_event_indicators(comp, z_cur, N_EVENT_INDICATORS);
				fmi2Status = fmi_get_event_indicators(comp, z_pre, N_EVENT_INDICATORS);
			}
	
			fmi_get_event_indicators(comp, z_pre, N_EVENT_INDICATORS);

            if(fmi2Status != fmi2OK) break;

		}
		for (k = 0; k < N_STATES; k++) { /* Update states */
			comp->reals[k] = comp->states[k];
		}
		return fmi2OK;
	}
}

fmi2Status fmi_get_status(fmi2Component c, const fmi2StatusKind s, fmi2Status*  value)
{
	switch (s) {
		case fmi2DoStepStatus:
			/* Return fmiPending if we are waiting. Otherwise the result from fmiDoStep */
			*value = fmi2OK;
			return fmi2OK;
		default: /* Not defined for status for this function */
			return fmi2Discard;
	}
}

fmi2Status fmi_get_real_status(fmi2Component c, const fmi2StatusKind s, fmi2Real*    value)
{
	switch (s) {
		case fmi2LastSuccessfulTime:
			/* Return fmiPending if we are waiting. Otherwise return end time for last call to fmiDoStep */
			*value = 0.01;
			return fmi2OK;
		default: /* Not defined for status for this function */
			return fmi2Discard;
	}
}

fmi2Status fmi_get_integer_status(fmi2Component c, const fmi2StatusKind s, fmi2Integer* value)
{
	switch (s) {
		default: /* Not defined for status for this function */
			return fmi2Discard;
	}
}

fmi2Status fmi_get_boolean_status(fmi2Component c, const fmi2StatusKind s, fmi2Boolean* value)
{
	switch (s) {
		default: /* Not defined for status for this function */
			return fmi2Discard;
	}
}

fmi2Status fmi_get_string_status(fmi2Component c, const fmi2StatusKind s, fmi2String*  value)
{
	switch (s) {
		case fmi2PendingStatus:
			*value = "Did fmi2DoStep really return with fmi2Pending? Then its time to implement this function";
			return fmi2Discard;
		default: /* Not defined for status for this function */
			return fmi2Discard;
	}
}

fmi2Status fmiFunction_not_supported(fmi2Component c, const char *functionName) {
    component_ptr_t comp	= (fmi2Component)c;
    if(!comp){
		return fmi2Error;
	}
	if(comp->loggingOn) {
		comp->functions->logger(comp->functions->componentEnvironment, comp->instanceName, fmi2OK, "Error", "###### Function %s is not implemented. ######", functionName);
	}
	return fmi2Error;
}
