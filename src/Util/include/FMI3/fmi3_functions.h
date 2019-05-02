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

#ifndef _FMI3_FUNCTION_TYPES_H_
#define _FMI3_FUNCTION_TYPES_H_

#include <string.h>
#include <fmilib_config.h>

#include "fmi3_types.h"
/**	\file fmi3_functions.h
	Mapping for the standard FMI 2.0 functions into fmi3_ namespace.

	\addtogroup fmi3_utils
	@{
*/

#ifdef __cplusplus
extern "C" {
#endif
/* make sure all compiler use the same alignment policies for structures */
#if defined _MSC_VER || defined __GNUC__
#pragma pack(push,8)
#endif


/** FMI 2.0 status codes */
typedef enum {
	fmi3_status_ok,
	fmi3_status_warning,
	fmi3_status_discard,
	fmi3_status_error,
	fmi3_status_fatal,
	fmi3_status_pending
} fmi3_status_t;

typedef enum {
    fmi3_model_exchange,
    fmi3_cosimulation
} fmi3_type_t;


/* Type definitions */

/** Convert #fmi3_status_t variable to string  */
FMILIB_EXPORT const char* fmi3_status_to_string(fmi3_status_t status);

/** FMI 2.0 logger function type */
typedef void  (*fmi3_callback_logger_ft)        (fmi3_component_environment_t env, fmi3_string_t instanceName, fmi3_status_t status, fmi3_string_t category, fmi3_string_t message, ...);
/** FMI 2.0 allocate memory function type */
typedef void* (*fmi3_callback_allocate_memory_ft)(size_t nobj, size_t size);
/** FMI 2.0 free memory  function type */
typedef void  (*fmi3_callback_free_memory_ft)    (void* obj);
/** FMI 2.0 step finished callback function type */
typedef void  (*fmi3_step_finished_ft)          (fmi3_component_environment_t env, fmi3_status_t status);

/** The FMI 2.0 callbacks */
typedef struct {
	fmi3_callback_logger_ft         logger;
	fmi3_callback_allocate_memory_ft allocateMemory;
	fmi3_callback_free_memory_ft     freeMemory;
	fmi3_step_finished_ft           stepFinished;
	fmi3_component_environment_t    componentEnvironment;
} fmi3_callback_functions_t;

/** Event info structure as used in FMI 2.0 ME */
/* TODO: reuse from fmiFunctions.h */
typedef struct {
    fmi3_boolean_t newDiscreteStatesNeeded;
    fmi3_boolean_t terminateSimulation;
    fmi3_boolean_t nominalsOfContinuousStatesChanged;
    fmi3_boolean_t valuesOfContinuousStatesChanged;
    fmi3_boolean_t nextEventTimeDefined;
    fmi3_real_t    nextEventTime;
} fmi3_event_info_t;

/** Co-simulation status for FMI 2.0 CS */
/* TODO: reuse from fmiFunctions.h */
typedef enum {
    fmi3_do_step_status,
    fmi3_pending_status,
    fmi3_last_successful_time,
    fmi3_terminated
} fmi3_status_kind_t;



/* reset alignment policy to the one set before reading this file */
#if defined _MSC_VER || defined __GNUC__
#pragma pack(pop)
#endif

/* Define fmi function pointer types to simplify dynamic loading */

/***************************************************
Types for Common Functions
****************************************************/

/* Inquire version numbers of header files and setting logging status */
   typedef const char* (*fmi3_get_types_platform_ft)();
   typedef const char* (*fmi3_get_version_ft)();
   typedef fmi3_status_t   (*fmi3_set_debug_logging_ft)(fmi3_component_t, fmi3_boolean_t,size_t nCategories, const fmi3_string_t categories[]);

/* Creation and destruction of fmu instances and setting debug status */
   typedef fmi3_component_t (*fmi3_instantiate_ft) (fmi3_string_t, fmi3_type_t, fmi3_string_t, fmi3_string_t, const fmi3_callback_functions_t*, fmi3_boolean_t, fmi3_boolean_t);
   typedef void         (*fmi3_free_instance_ft)(fmi3_component_t);

/* Enter and exit initialization mode, terminate and reset */
   typedef fmi3_status_t (*fmi3_setup_experiment_ft)          (fmi3_component_t, fmi3_boolean_t, fmi3_real_t, fmi3_real_t, fmi3_boolean_t, fmi3_real_t);
   typedef fmi3_status_t (*fmi3_enter_initialization_mode_ft) (fmi3_component_t);
   typedef fmi3_status_t (*fmi3_exit_initialization_mode_ft)  (fmi3_component_t);
   typedef fmi3_status_t (*fmi3_terminate_ft)              (fmi3_component_t);
   typedef fmi3_status_t (*fmi3_reset_ft)     (fmi3_component_t);

/* Getting and setting variable values */
   typedef fmi3_status_t (*fmi3_get_real_ft)   (fmi3_component_t, const fmi3_value_reference_t[], size_t, fmi3_real_t   []);
   typedef fmi3_status_t (*fmi3_get_integer_ft)(fmi3_component_t, const fmi3_value_reference_t[], size_t, fmi3_integer_t[]);
   typedef fmi3_status_t (*fmi3_get_boolean_ft)(fmi3_component_t, const fmi3_value_reference_t[], size_t, fmi3_boolean_t[]);
   typedef fmi3_status_t (*fmi3_get_string_ft) (fmi3_component_t, const fmi3_value_reference_t[], size_t, fmi3_string_t []);

   typedef fmi3_status_t (*fmi3_set_real_ft)   (fmi3_component_t, const fmi3_value_reference_t[], size_t, const fmi3_real_t   []);
   typedef fmi3_status_t (*fmi3_set_integer_ft)(fmi3_component_t, const fmi3_value_reference_t[], size_t, const fmi3_integer_t[]);
   typedef fmi3_status_t (*fmi3_set_boolean_ft)(fmi3_component_t, const fmi3_value_reference_t[], size_t, const fmi3_boolean_t[]);
   typedef fmi3_status_t (*fmi3_set_string_ft) (fmi3_component_t, const fmi3_value_reference_t[], size_t, const fmi3_string_t []);

/* Getting and setting the internal _fmu_ state */
   typedef fmi3_status_t (*fmi3_get_fmu_state_ft)           (fmi3_component_t, fmi3_FMU_state_t*);
   typedef fmi3_status_t (*fmi3_set_fmu_state_ft)           (fmi3_component_t, fmi3_FMU_state_t);
   typedef fmi3_status_t (*fmi3_free_fmu_state_ft)          (fmi3_component_t, fmi3_FMU_state_t*);
   typedef fmi3_status_t (*fmi3_serialized_fmu_state_size_ft)(fmi3_component_t, fmi3_FMU_state_t, size_t*);
   typedef fmi3_status_t (*fmi3_serialize_fmu_state_ft)     (fmi3_component_t, fmi3_FMU_state_t, fmi3_byte_t[], size_t);
   typedef fmi3_status_t (*fmi3_de_serialize_fmu_state_ft)   (fmi3_component_t, const fmi3_byte_t[], size_t, fmi3_FMU_state_t*);

/* Getting directional derivatives */
   typedef fmi3_status_t (*fmi3_get_directional_derivative_ft)(fmi3_component_t, const fmi3_value_reference_t[], size_t,
                                                                   const fmi3_value_reference_t[], size_t,
                                                                   const fmi3_real_t[], fmi3_real_t[]);

/***************************************************
Types for Functions for FMI for Model Exchange
****************************************************/

/* Enter and exit the different modes */
   typedef fmi3_status_t (*fmi3_enter_event_mode_ft)          (fmi3_component_t);
   typedef fmi3_status_t (*fmi3_new_discrete_states_ft)       (fmi3_component_t, fmi3_event_info_t*);
   typedef fmi3_status_t (*fmi3_enter_continuous_time_mode_ft)(fmi3_component_t);
   typedef fmi3_status_t (*fmi3_completed_integrator_step_ft) (fmi3_component_t, fmi3_boolean_t, fmi3_boolean_t*, fmi3_boolean_t*);

/* Providing independent variables and re-initialization of caching */
   typedef fmi3_status_t (*fmi3_set_time_ft)                (fmi3_component_t, fmi3_real_t);
   typedef fmi3_status_t (*fmi3_set_continuous_states_ft)    (fmi3_component_t, const fmi3_real_t[], size_t);

/* Evaluation of the model equations */
   typedef fmi3_status_t (*fmi3_get_derivatives_ft)            (fmi3_component_t, fmi3_real_t[], size_t);
   typedef fmi3_status_t (*fmi3_get_event_indicators_ft)        (fmi3_component_t, fmi3_real_t[], size_t);
   typedef fmi3_status_t (*fmi3_get_continuous_states_ft)       (fmi3_component_t, fmi3_real_t[], size_t);
   typedef fmi3_status_t (*fmi3_get_nominals_of_continuous_states_ft)(fmi3_component_t, fmi3_real_t[], size_t);


/***************************************************
Types for_functions for FMI for Co-_simulation
****************************************************/

/* Simulating the slave */
   typedef fmi3_status_t (*fmi3_set_real_input_derivatives_ft) (fmi3_component_t, const fmi3_value_reference_t [], size_t, const fmi3_integer_t [], const fmi3_real_t []);
   typedef fmi3_status_t (*fmi3_get_real_output_derivatives_ft)(fmi3_component_t, const fmi3_value_reference_t [], size_t, const fmi3_integer_t [], fmi3_real_t []);

   typedef fmi3_status_t (*fmi3_do_step_ft)     (fmi3_component_t, fmi3_real_t, fmi3_real_t, fmi3_boolean_t);
   typedef fmi3_status_t (*fmi3_cancel_step_ft) (fmi3_component_t);

/* Inquire slave status */
   typedef fmi3_status_t (*fmi3_get_status_ft)       (fmi3_component_t, const fmi3_status_kind_t, fmi3_status_t* );
   typedef fmi3_status_t (*fmi3_get_real_status_ft)   (fmi3_component_t, const fmi3_status_kind_t, fmi3_real_t*   );
   typedef fmi3_status_t (*fmi3_get_integer_status_ft)(fmi3_component_t, const fmi3_status_kind_t, fmi3_integer_t*);
   typedef fmi3_status_t (*fmi3_get_boolean_status_ft)(fmi3_component_t, const fmi3_status_kind_t, fmi3_boolean_t*);
   typedef fmi3_status_t (*fmi3_get_string_status_ft) (fmi3_component_t, const fmi3_status_kind_t, fmi3_string_t* );

/**	@}
*/

#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif /* fmi3_function_types_h */
