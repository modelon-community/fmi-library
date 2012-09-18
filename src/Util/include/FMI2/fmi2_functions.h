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

#ifndef FMI2_FUNCTIONS_H_
#define FMI2_FUNCTIONS_H_

#include <string.h>
#include <fmilib_config.h>

#include "fmi2_types.h"
/**	\file fmi2_functions.h
	Mapping for the standard FMI 2.0 functions into fmi2_ namespace.

	\addtogroup fmi2_utils
	@{
*/

/** FMI 2.0 status codes */
typedef enum {
	fmi2_status_ok,
	fmi2_status_warning,
	fmi2_status_discard,
	fmi2_status_error,
	fmi2_status_fatal,
	fmi2_status_pending
} fmi2_status_t;

/** Convert #fmi2_status_t variable to string  */
FMILIB_EXPORT const char* fmi2_status_to_string(fmi2_status_t status);

/** FMI 1.0 logger function type */
typedef void  (*fmi2_callback_logger_ft)        (fmi2_component_environment_t env, fmi2_string_t instanceName, fmi2_status_t status, fmi2_string_t category, fmi2_string_t message, ...);
/** FMI 1.0 allocate memory function type */
typedef void* (*fmi2_callback_allocate_memory_ft)(size_t nobj, size_t size);
/** FMI 1.0 free memory  function type */
typedef void  (*fmi2_callback_free_memory_ft)    (void* obj);
/** FMI 1.0 step finished callback function type */
typedef void  (*fmi2_step_finished_ft)          (fmi2_component_environment_t env, fmi2_status_t status);

/** The FMI 2.0 callbacks */
typedef struct {
	fmi2_callback_logger_ft         logger;
	fmi2_callback_allocate_memory_ft allocateMemory;
	fmi2_callback_free_memory_ft     freeMemory;
	fmi2_step_finished_ft           stepFinished;
	fmi2_component_environment_t    componentEnvironment;
} fmi2_callback_functions_t;

/** Event info structure as used in FMI 1.0 ME */
typedef struct {
	fmi2_boolean_t iterationConverged;
	fmi2_boolean_t stateValueReferencesChanged;
	fmi2_boolean_t stateValuesChanged;
	fmi2_boolean_t terminateSimulation;
	fmi2_boolean_t upcomingTimeEvent;
	fmi2_real_t    nextEventTime;
} fmi2_event_info_t;

/** FMI 1.0 asyncronous co-simulation  status */
typedef enum {
	fmi2_do_step_status,
	fmi2_pending_status,
	fmi2_last_successful_time,
	fmi2_terminated
} fmi2_status_kind_t;


/* TODO: put the FMI 2.0 functions here */
/* FMI 1.0 common functions */
typedef const char*	  	    (*fmi2_get_version_ft)					(void);
typedef fmi2_status_t		(*fmi2_set_debug_logging_ft)			(fmi2_component_t c, fmi2_boolean_t loggingOn, size_t nCategories, const fmi2_string_t categories[]);
typedef fmi2_status_t		(*fmi2_set_real_ft)						(fmi2_component_t c, const fmi2_value_reference_t vr[], size_t nvr, const fmi2_real_t   value[]);
typedef fmi2_status_t		(*fmi2_set_integer_ft)					(fmi2_component_t c, const fmi2_value_reference_t vr[], size_t nvr, const fmi2_integer_t value[]);
typedef fmi2_status_t		(*fmi2_set_boolean_ft)					(fmi2_component_t c, const fmi2_value_reference_t vr[], size_t nvr, const fmi2_boolean_t value[]);
typedef fmi2_status_t		(*fmi2_set_string_ft)					(fmi2_component_t c, const fmi2_value_reference_t vr[], size_t nvr, const fmi2_string_t  value[]);
typedef fmi2_status_t		(*fmi2_get_real_ft)						(fmi2_component_t c, const fmi2_value_reference_t vr[], size_t nvr, fmi2_real_t   value[]);
typedef fmi2_status_t		(*fmi2_get_integer_ft)					(fmi2_component_t c, const fmi2_value_reference_t vr[], size_t nvr, fmi2_integer_t value[]);
typedef fmi2_status_t		(*fmi2_get_boolean_ft)					(fmi2_component_t c, const fmi2_value_reference_t vr[], size_t nvr, fmi2_boolean_t value[]);
typedef fmi2_status_t		(*fmi2_get_string_ft)					(fmi2_component_t c, const fmi2_value_reference_t vr[], size_t nvr, fmi2_string_t  value[]);

/* FMI ME 1.0 functions */
typedef const char*		    (*fmi2_get_model_typesPlatform_ft)		(void);
typedef fmi2_component_t	(*fmi2_instantiate_model_ft)			(fmi2_string_t instanceName, fmi2_string_t GUID, fmi2_callback_functions_t functions, fmi2_boolean_t loggingOn);
typedef void			    (*fmi2_free_model_instance_ft)			(fmi2_component_t c);
typedef fmi2_status_t		(*fmi2_set_time_ft)					(fmi2_component_t c, fmi2_real_t time);
typedef fmi2_status_t		(*fmi2_set_continuous_states_ft)		(fmi2_component_t c, const fmi2_real_t x[], size_t nx);
typedef fmi2_status_t		(*fmi2_completed_integrator_step_ft)	(fmi2_component_t c, fmi2_boolean_t* callEventUpdate);
typedef fmi2_status_t		(*fmi2_initialize_ft)					(fmi2_component_t c, fmi2_boolean_t toleranceControlled, fmi2_real_t relativeTolerance, fmi2_event_info_t* eventInfo);
typedef fmi2_status_t		(*fmi2_get_derivatives_ft)				(fmi2_component_t c, fmi2_real_t derivatives[]    , size_t nx);
typedef fmi2_status_t		(*fmi2_get_event_indicators_ft)			(fmi2_component_t c, fmi2_real_t eventIndicators[], size_t ni);
typedef fmi2_status_t		(*fmi2_event_update_ft)				(fmi2_component_t c, fmi2_boolean_t intermediateResults, fmi2_event_info_t* eventInfo);
typedef fmi2_status_t		(*fmi2_get_continuous_states_ft)		(fmi2_component_t c, fmi2_real_t states[], size_t nx);
typedef fmi2_status_t		(*fmi2_get_nominal_continuousStates_ft)	(fmi2_component_t c, fmi2_real_t x_nominal[], size_t nx);
typedef fmi2_status_t		(*fmi2_get_state_valueReferences_ft)	(fmi2_component_t c, fmi2_value_reference_t vrx[], size_t nx);
typedef fmi2_status_t		(*fmi2_terminate_ft)					(fmi2_component_t c);  


/* FMI CS 1.0 functions */
typedef const char*		(*fmi2_get_types_platform_ft)			(void );
typedef fmi2_component_t	(*fmi2_instantiate_slave_ft)			(fmi2_string_t  instanceName, fmi2_string_t  fmuGUID, fmi2_string_t  fmuLocation, 
															 fmi2_string_t  mimeType, fmi2_real_t timeout, fmi2_boolean_t visible, fmi2_boolean_t interactive, 
															 fmi2_callback_functions_t functions, fmi2_boolean_t loggingOn);
typedef fmi2_status_t		(*fmi2_initialize_slave_ft)			(fmi2_component_t c, fmi2_real_t tStart, fmi2_boolean_t StopTimeDefined, fmi2_real_t tStop);
typedef fmi2_status_t		(*fmi2_terminate_slave_ft)				(fmi2_component_t c);
typedef fmi2_status_t		(*fmi2_reset_slave_ft)					(fmi2_component_t c);
typedef void			    (*fmi2_free_slave_instance_ft)			(fmi2_component_t c);
typedef fmi2_status_t		(*fmi2_set_real_inputDerivatives_ft)	(fmi2_component_t c, const  fmi2_value_reference_t vr[], size_t nvr, const fmi2_integer_t order[], const  fmi2_real_t value[]);                                                  
typedef fmi2_status_t		(*fmi2_get_real_outputDerivatives_ft)	(fmi2_component_t c, const fmi2_value_reference_t vr[], size_t  nvr, const fmi2_integer_t order[], fmi2_real_t value[]);                                              
typedef fmi2_status_t		(*fmi2_cancel_step_ft)					(fmi2_component_t c);
typedef fmi2_status_t		(*fmi2_do_step_ft)						(fmi2_component_t c, fmi2_real_t currentCommunicationPoint, fmi2_real_t communicationStepSize, fmi2_boolean_t newStep);

typedef fmi2_status_t		(*fmi2_get_status_ft)					(fmi2_component_t c, const fmi2_status_kind_t s, fmi2_status_t*  value);
typedef fmi2_status_t		(*fmi2_get_real_status_ft)				(fmi2_component_t c, const fmi2_status_kind_t s, fmi2_real_t*    value);
typedef fmi2_status_t		(*fmi2_get_integer_status_ft)			(fmi2_component_t c, const fmi2_status_kind_t s, fmi2_integer_t* value);
typedef fmi2_status_t		(*fmi2_get_boolean_status_ft)			(fmi2_component_t c, const fmi2_status_kind_t s, fmi2_boolean_t* value);
typedef fmi2_status_t		(*fmi2_get_string_status_ft)			(fmi2_component_t c, const fmi2_status_kind_t s, fmi2_string_t*  value); 

/** @}
*/

#endif /* End of header file FMI2_FUNCTIONS_H_ */
