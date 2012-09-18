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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <errno.h>
#include <FMI2/fmi2_types.h>
#include <FMI2/fmi2_functions.h>
#include <FMI2/fmi2_enums.h>
#include <FMI2/fmi2_capi.h>
#include <FMI/fmi_util.h>
#include "fmi2_import_impl.h"

static const char * module = "FMILIB";

/* Load and destroy functions */
jm_status_enu_t fmi2_import_create_dllfmu(fmi2_import_t* fmu, fmi2_callback_functions_t callBackFunctions, int registerGlobally) {

	char curDir[FILENAME_MAX + 2];
	char* dllDirPath = 0;
	char* dllFileName = 0;
	const char* modelIdentifier;
	fmi2_fmu_kind_enu_t standard;

	if (fmu == NULL) {
		assert(0);
		return jm_status_error;
	}

	if(fmu -> capi) {
		jm_log_warning(fmu->callbacks, module, "FMU binary is already loaded"); 
		return jm_status_success;
	}

	standard = fmi2_import_get_fmu_kind(fmu);

	modelIdentifier = fmi2_import_get_model_identifier(fmu);
	if (modelIdentifier == NULL) {
		return jm_status_error;
	}

	if( jm_portability_get_current_working_directory(curDir, FILENAME_MAX+1) != jm_status_success) {
		jm_log_warning(fmu->callbacks, module, "Could not get current working directory (%s)", strerror(errno));
		curDir[0] = 0;
	};

	dllDirPath = fmi_construct_dll_dir_name(fmu->callbacks, fmu->dirPath);
	dllFileName = fmi_construct_dll_file_name(fmu->callbacks, modelIdentifier);

	if (!dllDirPath ||!dllFileName) {
		fmu->callbacks->free(dllDirPath);
		return jm_status_error;
	}

	if(jm_portability_set_current_working_directory(dllDirPath) != jm_status_success) {
		jm_log_fatal(fmu->callbacks, module, "Could not change to the DLL directory %s", dllDirPath);
		if(ENOENT == errno)
			jm_log_fatal(fmu->callbacks, module, "No binary for this platform?");
		else
			jm_log_fatal(fmu->callbacks, module, "System error: %s", strerror(errno));
	}
	else {
		/* Allocate memory for the C-API struct */
		fmu -> capi = fmi2_capi_create_dllfmu(fmu->callbacks, dllFileName, modelIdentifier, callBackFunctions, standard);
	}


	/* Load the DLL handle */
	if (fmu -> capi) {
		jm_log_info(fmu->callbacks, module, 
			"Loading '" FMI_PLATFORM "' binary with '%s' platform types", fmi2_get_types_platform() );

		if(fmi2_capi_load_dll(fmu -> capi) == jm_status_error) {		
			fmi2_capi_destroy_dllfmu(fmu -> capi);
			fmu -> capi = NULL;
		}
	}

	if(curDir[0] && (jm_portability_set_current_working_directory(curDir) != jm_status_success)) {
		jm_log_error(fmu->callbacks, module, "Could not restore current working directory (%s)", strerror(errno));
	}

	fmu->callbacks->free((jm_voidp)dllDirPath);
	fmu->callbacks->free((jm_voidp)dllFileName);

	if (fmu -> capi == NULL) {
		return jm_status_error;
	}


	/* Load the DLL functions */
	if (fmi2_capi_load_fcn(fmu -> capi) == jm_status_error) {
		fmi2_capi_free_dll(fmu -> capi);			
		fmi2_capi_destroy_dllfmu(fmu -> capi);
		fmu -> capi = NULL;
		return jm_status_error;
	}
	jm_log_verbose(fmu->callbacks, module, "Successfully loaded all the interface functions"); 

	if (registerGlobally) {
		fmu->registerGlobally = 1;
		if(!fmi2_import_active_fmu) {
			jm_vector_init(jm_voidp)(&fmi2_import_active_fmu_store,0, fmu->callbacks);
			fmi2_import_active_fmu = &fmi2_import_active_fmu_store;
			jm_log_debug(fmu->callbacks, module, "Created an empty active fmu list");
		}
		jm_vector_push_back(jm_voidp)(fmi2_import_active_fmu, fmu);
		jm_log_debug(fmu->callbacks, module, "Registrered active fmu(%p)", fmu);
	}

	return jm_status_success;
}

void fmi2_import_destroy_dllfmu(fmi2_import_t* fmu) {
	
	if (fmu == NULL) {
		return;
	}

	
	if(fmu -> capi) {
		jm_log_verbose(fmu->callbacks, module, "Releasing FMU CAPI interface"); 

		/* Free DLL handle */
		fmi2_capi_free_dll(fmu -> capi);

		/* Destroy the C-API struct */
		fmi2_capi_destroy_dllfmu(fmu -> capi);

		if(fmu->registerGlobally && fmi2_import_active_fmu) {
			size_t index;
			size_t nFmu;

			index = jm_vector_find_index(jm_voidp)(fmi2_import_active_fmu, (void**)&fmu, jm_compare_voidp);
			nFmu = jm_vector_get_size(jm_voidp)(fmi2_import_active_fmu);
			if(index < nFmu) {
				jm_vector_remove_item(jm_voidp)(fmi2_import_active_fmu,index);
				jm_log_debug(fmu->callbacks, module, "Unregistrered active fmu(%p)", fmu);
				if(nFmu == 1) {
					jm_vector_free_data(jm_voidp)(fmi2_import_active_fmu);
					fmi2_import_active_fmu = 0;
					jm_log_debug(fmu->callbacks, module, "Freed active fmu list");
				}
			}
			fmu->registerGlobally = 0;
		}

		fmu -> capi = NULL;
	}
	else {
		assert(fmu->registerGlobally == 0);
	}
}

/* FMI 1.0 Common functions */
const char* fmi2_import_get_version(fmi2_import_t* fmu) {
	if(!fmu->capi) {
		jm_log_error(fmu->callbacks, module,"FMU CAPI is not loaded");
		return 0;
	}
	return fmi2_capi_get_version(fmu -> capi);
}

fmi2_status_t fmi2_import_set_debug_logging(fmi2_import_t* fmu, fmi2_boolean_t loggingOn, size_t nCategories, fmi2_string_t categories[]) {
	if(!fmu->capi) {
		jm_log_error(fmu->callbacks, module,"FMU CAPI is not loaded");
		return fmi2_status_fatal;
	}
	return fmi2_capi_set_debug_logging(fmu -> capi, loggingOn, nCategories, categories);
}

fmi2_status_t fmi2_import_set_real(fmi2_import_t* fmu, const fmi2_value_reference_t vr[], size_t nvr, const fmi2_real_t    value[]) {
	return fmi2_capi_set_real(fmu -> capi, vr, nvr, value);
}

fmi2_status_t fmi2_import_set_integer(fmi2_import_t* fmu, const fmi2_value_reference_t vr[], size_t nvr, const fmi2_integer_t value[]) {
	return fmi2_capi_set_integer(fmu -> capi, vr, nvr, value);
}

fmi2_status_t fmi2_import_set_boolean(fmi2_import_t* fmu, const fmi2_value_reference_t vr[], size_t nvr, const fmi2_boolean_t value[]) {
	return fmi2_capi_set_boolean(fmu -> capi, vr, nvr, value);
}

fmi2_status_t fmi2_import_set_string(fmi2_import_t* fmu, const fmi2_value_reference_t vr[], size_t nvr, const fmi2_string_t  value[]) {
	return fmi2_capi_set_string(fmu -> capi, vr, nvr, value);
}

fmi2_status_t fmi2_import_get_real(fmi2_import_t* fmu, const fmi2_value_reference_t vr[], size_t nvr, fmi2_real_t    value[]) {
	return fmi2_capi_get_real(fmu -> capi, vr, nvr, value);
}

fmi2_status_t fmi2_import_get_integer(fmi2_import_t* fmu, const fmi2_value_reference_t vr[], size_t nvr, fmi2_integer_t value[]) {
	return fmi2_capi_get_integer(fmu -> capi, vr, nvr, value);
}

fmi2_status_t fmi2_import_get_boolean(fmi2_import_t* fmu, const fmi2_value_reference_t vr[], size_t nvr, fmi2_boolean_t value[]) {
	return fmi2_capi_get_boolean(fmu -> capi, vr, nvr, value);
}

fmi2_status_t fmi2_import_get_string(fmi2_import_t* fmu, const fmi2_value_reference_t vr[], size_t nvr, fmi2_string_t  value[]) {
	return fmi2_capi_get_string(fmu -> capi, vr, nvr, value);
}


/* FMI 1.0 ME functions */
const char* fmi2_import_get_model_types_platform(fmi2_import_t* fmu) {
	return fmi2_capi_get_model_types_platform(fmu -> capi);
}

jm_status_enu_t fmi2_import_instantiate_model(fmi2_import_t* fmu, fmi2_string_t instanceName) {
	fmi2_string_t GUID = fmi2_import_get_GUID(fmu);
	fmi2_boolean_t loggingOn = (fmu->callbacks->log_level > jm_log_level_nothing);
	fmi2_component_t c = fmi2_capi_instantiate_model(fmu -> capi, instanceName, GUID, loggingOn);
	if (c == NULL) {
		return jm_status_error;
	} else {
		return jm_status_success;
	}
}

void fmi2_import_free_model_instance(fmi2_import_t* fmu) {
	fmi2_capi_free_model_instance(fmu -> capi);
}

fmi2_status_t fmi2_import_set_time(fmi2_import_t* fmu, fmi2_real_t time) {
	return fmi2_capi_set_time(fmu -> capi, time);
}

fmi2_status_t fmi2_import_set_continuous_states(fmi2_import_t* fmu, const fmi2_real_t x[], size_t nx) {
	return fmi2_capi_set_continuous_states(fmu -> capi, x, nx);
}

fmi2_status_t fmi2_import_completed_integrator_step(fmi2_import_t* fmu, fmi2_boolean_t* callEventUpdate) {
	return fmi2_capi_completed_integrator_step(fmu -> capi, callEventUpdate);
}

fmi2_status_t fmi2_import_initialize(fmi2_import_t* fmu, fmi2_boolean_t toleranceControlled, fmi2_real_t relativeTolerance, fmi2_event_info_t* eventInfo) {
	return fmi2_capi_initialize(fmu -> capi, toleranceControlled, relativeTolerance, eventInfo);
}

fmi2_status_t fmi2_import_get_derivatives(fmi2_import_t* fmu, fmi2_real_t derivatives[], size_t nx) {
	return fmi2_capi_get_derivatives(fmu -> capi, derivatives, nx);
}

fmi2_status_t fmi2_import_get_event_indicators(fmi2_import_t* fmu, fmi2_real_t eventIndicators[], size_t ni) {
	return fmi2_capi_get_event_indicators(fmu -> capi, eventIndicators, ni);
}

fmi2_status_t fmi2_import_eventUpdate(fmi2_import_t* fmu, fmi2_boolean_t intermediateResults, fmi2_event_info_t* eventInfo) {
	return fmi2_capi_eventUpdate(fmu -> capi, intermediateResults, eventInfo);
}

fmi2_status_t fmi2_import_get_continuous_states(fmi2_import_t* fmu, fmi2_real_t states[], size_t nx) {
	return fmi2_capi_get_continuous_states(fmu -> capi, states, nx);
}

fmi2_status_t fmi2_import_get_nominal_continuous_states(fmi2_import_t* fmu, fmi2_real_t x_nominal[], size_t nx) {
	return fmi2_capi_get_nominal_continuous_states(fmu -> capi, x_nominal, nx);
}

fmi2_status_t fmi2_import_get_state_value_references(fmi2_import_t* fmu, fmi2_value_reference_t vrx[], size_t nx) {
	return fmi2_capi_get_state_value_references(fmu -> capi, vrx, nx);
}

fmi2_status_t fmi2_import_terminate(fmi2_import_t* fmu) {
	return fmi2_capi_terminate(fmu -> capi);
}


/* FMI 1.0 CS functions */
const char* fmi2_import_get_types_platform(fmi2_import_t* fmu) {
	return fmi2_capi_get_types_platform(fmu -> capi);
}

jm_status_enu_t fmi2_import_instantiate_slave(fmi2_import_t* fmu, fmi2_string_t instanceName, fmi2_string_t fmuLocation, fmi2_string_t mimeType,
																 fmi2_real_t timeout, fmi2_boolean_t visible, fmi2_boolean_t interactive) {
 	fmi2_string_t fmuGUID = fmi2_import_get_GUID(fmu);
	fmi2_boolean_t loggingOn = (fmu->callbacks->log_level > jm_log_level_nothing);
	fmi2_component_t c = fmi2_capi_instantiate_slave(fmu -> capi, instanceName, fmuGUID, fmuLocation, mimeType, timeout, visible, interactive, loggingOn);
	if (c == NULL) {
		return jm_status_error;
	} else {
		return jm_status_success;
	}
}

fmi2_status_t fmi2_import_initialize_slave(fmi2_import_t* fmu, fmi2_real_t tStart, fmi2_boolean_t StopTimeDefined, fmi2_real_t tStop) {
	return fmi2_capi_initialize_slave(fmu -> capi, tStart, StopTimeDefined, tStop);
}

fmi2_status_t fmi2_import_terminate_slave(fmi2_import_t* fmu) {
	return fmi2_capi_terminate_slave(fmu -> capi);
}

fmi2_status_t fmi2_import_reset_slave(fmi2_import_t* fmu) {
	return fmi2_capi_reset_slave(fmu -> capi);
}

void fmi2_import_free_slave_instance(fmi2_import_t* fmu) {
	fmi2_capi_free_slave_instance(fmu -> capi);
}

fmi2_status_t fmi2_import_set_real_input_derivatives(fmi2_import_t* fmu, const fmi2_value_reference_t vr[], size_t nvr, const fmi2_integer_t order[], const  fmi2_real_t value[]) {
	return fmi2_capi_set_real_input_derivatives(fmu -> capi, vr, nvr, order, value);
}
                                                  
fmi2_status_t fmi2_import_get_real_output_derivatives(fmi2_import_t* fmu, const fmi2_value_reference_t vr[], size_t nvr, const fmi2_integer_t order[], fmi2_real_t value[]) {
	return fmi2_capi_get_real_output_derivatives(fmu -> capi, vr, nvr, order, value);
}
                                              
fmi2_status_t fmi2_import_cancel_step(fmi2_import_t* fmu) {
	return fmi2_capi_cancel_step(fmu -> capi);
}

fmi2_status_t fmi2_import_do_step(fmi2_import_t* fmu, fmi2_real_t currentCommunicationPoint, fmi2_real_t communicationStepSize, fmi2_boolean_t newStep) {
	return fmi2_capi_do_step(fmu -> capi, currentCommunicationPoint, communicationStepSize, newStep);
}

fmi2_status_t fmi2_import_get_status(fmi2_import_t* fmu, const fmi2_status_kind_t s, fmi2_status_t*  value) {
	return fmi2_capi_get_status(fmu -> capi, s, value);
}

fmi2_status_t fmi2_import_get_real_status(fmi2_import_t* fmu, const fmi2_status_kind_t s, fmi2_real_t*    value) {
	return fmi2_capi_get_real_status(fmu -> capi, s, value);
}

fmi2_status_t fmi2_import_get_integer_status(fmi2_import_t* fmu, const fmi2_status_kind_t s, fmi2_integer_t* value) {
	return fmi2_capi_get_integer_status(fmu -> capi, s, value);
}

fmi2_status_t fmi2_import_get_boolean_status(fmi2_import_t* fmu, const fmi2_status_kind_t s, fmi2_boolean_t* value) {
	return fmi2_capi_get_boolean_status(fmu -> capi, s, value);
}

fmi2_status_t fmi2_import_get_string_status(fmi2_import_t* fmu, const fmi2_status_kind_t s, fmi2_string_t*  value) {
	return fmi2_capi_get_string_status(fmu -> capi, s, value);
}

#ifdef __cplusplus
}
#endif
