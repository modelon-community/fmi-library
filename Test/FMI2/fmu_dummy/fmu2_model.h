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

#ifndef FM1_MODEL_H_
#define FM1_MODEL_H_
#include <FMI2/fmiFunctions.h>
#include <fmu_dummy/fmu2_model_defines.h>
#ifndef FMI_Export
	#define FMI_Export DllExport
#endif
typedef struct {
	/*************** FMI ME 2.0 ****************/
	fmiReal					states			[N_STATES];
	fmiReal					states_nom		[N_STATES];
	fmiValueReference		states_vr		[N_STATES];
	fmiReal					states_der		[N_STATES];
	fmiReal					event_indicators[N_EVENT_INDICATORS];
	fmiReal					reals			[N_REAL];
	fmiInteger				integers		[N_INTEGER];
	fmiBoolean				booleans		[N_BOOLEAN];
	fmiString				strings			[N_STRING];

	/* fmiInstantiateModel */
	fmiBoolean				loggingOn;
	char					instanceName	[BUFFER];
	char					GUID			[BUFFER];
	fmiCallbackFunctions	functions;

	/* fmiSetTime */
	fmiReal					fmitime;

	/* fmiCompletedIntegratorStep */
	fmiBoolean				callEventUpdate;

	/* fmiInitializeModel */
	fmiBoolean				toleranceControlled;
	fmiReal					relativeTolerance;
	fmiEventInfo			eventInfo;

	/*************** FMI CS 2.0. Depends on the ME fields above and functions ****************/
	fmiReal					states_prev		[N_STATES];

	/* fmiInstantiateSlave */
	char					fmuLocation		[BUFFER];
	fmiBoolean				visible;

	/* fmiInitializeSlave */
	fmiReal					tStart;
	fmiBoolean				StopTimeDefined;
	fmiReal					tStop;

	/* fmiSetRealInputDerivatives */
	fmiReal					input_real		[N_INPUT_REAL][N_INPUT_REAL_MAX_ORDER + 1];

	/* fmiGetRealOutputDerivatives */
	fmiReal					output_real		[N_OUTPUT_REAL][N_OUTPUT_REAL_MAX_ORDER + 1];

} component_t;

typedef component_t* component_ptr_t;

/* FMI 2.0 Common Functions */
const char*		fmi_get_version();

fmiStatus		fmi_set_debug_logging(
													fmiComponent c,
													fmiBoolean loggingOn);

fmiComponent fmi_instantiate (
    fmiString instanceName,
    fmiType fmuType,
    fmiString fmuGUID,
    fmiString fmuLocation,
    const fmiCallbackFunctions* functions,
    fmiBoolean visible,
    fmiBoolean loggingOn);

void fmi_free_instance(
    fmiComponent c);

fmiStatus fmi_setup_experiment(fmiComponent c,
    fmiBoolean toleranceDefined, fmiReal tolerance,
    fmiReal startTime, fmiBoolean stopTimeDefined,
    fmiReal stopTime);
fmiStatus		fmi_enter_initialization_mode(fmiComponent c);
fmiStatus		fmi_exit_initialization_mode(fmiComponent c);

fmiStatus		fmi_terminate(fmiComponent c);

fmiStatus		fmi_reset(
													fmiComponent c);


fmiStatus		fmi_get_real(			
													fmiComponent c,
													const fmiValueReference vr[],
													size_t nvr, fmiReal value[]);

fmiStatus		fmi_get_integer(	
													fmiComponent c,
													const fmiValueReference vr[],
													size_t nvr,
													fmiInteger value[]);
fmiStatus		fmi_get_boolean(
													fmiComponent c,
													const fmiValueReference vr[],
													size_t nvr,
													fmiBoolean value[]);

fmiStatus		fmi_get_string(
													fmiComponent c,
													const fmiValueReference vr[],
													size_t nvr,
													fmiString  value[]);

fmiStatus		fmi_set_real(
													fmiComponent c,
													const fmiValueReference vr[],
													size_t nvr,
													const fmiReal value[]);
fmiStatus		fmi_set_integer(
													fmiComponent c,
													const fmiValueReference vr[],
													size_t nvr,
													const fmiInteger value[]);

fmiStatus		fmi_set_boolean(
													fmiComponent c,
													const fmiValueReference vr[],
													size_t nvr,
													const fmiBoolean value[]);

fmiStatus		fmi_set_string(
													fmiComponent c,
													const fmiValueReference vr[],
													size_t nvr,
													const fmiString  value[]);

/* FMI 2.0 ME Functions */
const char*		fmi_get_model_types_platform();

fmiStatus		fmi_enter_event_mode(fmiComponent c);
fmiStatus		fmi_new_discrete_states(fmiComponent c, fmiEventInfo* eventInfo);
fmiStatus		fmi_enter_continuous_time_mode(fmiComponent c);

fmiStatus		fmi_set_time(
													fmiComponent c,
													fmiReal fmitime);

fmiStatus		fmi_set_continuous_states(
													fmiComponent c,
													const fmiReal x[],
													size_t nx);

fmiStatus fmi_completed_integrator_step(
    fmiComponent c,
    fmiBoolean noSetFMUStatePriorToCurrentPoint,
    fmiBoolean* enterEventMode, fmiBoolean* terminateSimulation);

fmiStatus		fmi_get_derivatives(
													fmiComponent c,
													fmiReal derivatives[],
													size_t nx);

fmiStatus		fmi_get_event_indicators(
													fmiComponent c,
													fmiReal eventIndicators[],
													size_t ni);

fmiStatus		fmi_event_update(
													fmiComponent c,
													fmiBoolean intermediateResults,
													fmiEventInfo* eventInfo);
fmiStatus		fmi_get_continuous_states(
													fmiComponent c,
													fmiReal states[],
													size_t nx);

fmiStatus		fmi_get_nominals_of_continuousstates(	
													fmiComponent c,
													fmiReal x_nominal[],
													size_t nx);

fmiStatus		fmi_get_state_value_references(
													fmiComponent c,
													fmiValueReference vrx[],
													size_t nx);


/* FMI 2.0 CS Functions */
#ifdef fmiFunctions_h

const char*		fmi_get_types_platform();

void			fmi_free_slave_instance(
													fmiComponent c);

fmiStatus		fmi_set_real_input_derivatives(
													fmiComponent c,
													const fmiValueReference vr[],
													size_t nvr,
													const fmiInteger order[],
													const fmiReal value[]);

fmiStatus		fmi_get_real_output_derivatives(
													fmiComponent c,
													const fmiValueReference vr[],
													size_t nvr,
													const fmiInteger order[],
													fmiReal value[]);

fmiStatus		fmi_cancel_step(
													fmiComponent c);
fmiStatus		fmi_do_step(
													fmiComponent c,
													fmiReal currentCommunicationPoint,
													fmiReal communicationStepSize,
													fmiBoolean newStep);

fmiStatus		fmi_get_status(
													fmiComponent c,
													const fmiStatusKind s,
													fmiStatus*  value);

fmiStatus		fmi_get_real_status(
													fmiComponent c,
													const fmiStatusKind s,
													fmiReal*    value);

fmiStatus		fmi_get_integer_status(
													fmiComponent c,
													const fmiStatusKind s,
													fmiInteger* value);

fmiStatus		fmi_get_boolean_status(
													fmiComponent c,
													const fmiStatusKind s,
													fmiBoolean* value);

fmiStatus		fmi_get_string_status(
													fmiComponent c,
													const fmiStatusKind s,
													fmiString*  value);

#endif /* End of fmiFunctions_h */
#endif /* End of header FMU2_MODEL_H_ */
