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
#include <FMI3/fmi3_types.h>
#include <FMI3/fmi3_functions.h>
#include <FMI3/fmi3_enums.h>
#include <FMI3/fmi3_capi.h>
#include <FMI/fmi_util.h>
#include "fmi3_import_impl.h"

static const char * module = "FMILIB";

/* Load and destroy functions */
jm_status_enu_t fmi3_import_create_dllfmu(fmi3_import_t* fmu, fmi3_fmu_kind_enu_t fmuKind, const fmi3_callback_functions_t* callBackFunctions) {

	char curDir[FILENAME_MAX + 2];
	char* dllDirPath = 0;
	char* dllFileName = 0;
	const char* modelIdentifier;
	fmi3_callback_functions_t defaultCallbacks;

	if (fmu == NULL) {
		assert(0);
		return jm_status_error;
	}

	if(fmu -> capi) {
		if(fmi3_capi_get_fmu_kind(fmu -> capi) == fmuKind) {
			jm_log_warning(fmu->callbacks, module, "FMU binary is already loaded"); 
			return jm_status_success;
		}
		else
			fmi3_import_destroy_dllfmu(fmu);		
	}

	if(fmuKind == fmi3_fmu_kind_me)
		modelIdentifier = fmi3_import_get_model_identifier_ME(fmu);
	else 	if(fmuKind == fmi3_fmu_kind_cs)
		modelIdentifier = fmi3_import_get_model_identifier_CS(fmu);
	else {
		assert(0);
		return jm_status_error;
	}

	if (modelIdentifier == NULL) {
		jm_log_error(fmu->callbacks, module, "No model identifier given");
		return jm_status_error;
	}

	if( jm_portability_get_current_working_directory(curDir, FILENAME_MAX+1) != jm_status_success) {
		jm_log_warning(fmu->callbacks, module, "Could not get current working directory (%s)", strerror(errno));
		curDir[0] = 0;
	};

	dllDirPath = fmi_construct_dll_dir_name(fmu->callbacks, fmu->dirPath);
	dllFileName = fmi_construct_dll_file_name(fmu->callbacks, dllDirPath, modelIdentifier);

	if (!dllDirPath ||!dllFileName) {
		fmu->callbacks->free(dllDirPath);
		return jm_status_error;
	}

	if(!callBackFunctions) {
		jm_callbacks* cb = fmu->callbacks;
		defaultCallbacks.allocateMemory = cb->calloc;
		defaultCallbacks.freeMemory = cb->free;
		defaultCallbacks.componentEnvironment = fmu;
		defaultCallbacks.logger = fmi3_log_forwarding;
		defaultCallbacks.stepFinished = 0;
		callBackFunctions = &defaultCallbacks;
	}

	if(jm_portability_set_current_working_directory(dllDirPath) != jm_status_success) {
		jm_log_fatal(fmu->callbacks, module, "Could not change to the DLL directory %s", dllDirPath);
		if(ENOENT == errno)
			jm_log_fatal(fmu->callbacks, module, "The FMU contains no binary for this platform.");
		else
			jm_log_fatal(fmu->callbacks, module, "System error: %s", strerror(errno));
	}
	else {
		/* Allocate memory for the C-API struct */
		fmu -> capi = fmi3_capi_create_dllfmu(fmu->callbacks, dllFileName, modelIdentifier, callBackFunctions, fmuKind);
	}


	/* Load the DLL handle */
	if (fmu -> capi) {
		jm_log_info(fmu->callbacks, module, 
			"Loading '" FMI_PLATFORM "' binary with '%s' platform types", fmi3_get_types_platform() );

		if(fmi3_capi_load_dll(fmu -> capi) == jm_status_error) {		
			fmi3_capi_destroy_dllfmu(fmu -> capi);
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
	if (fmi3_capi_load_fcn(fmu -> capi, fmi3_xml_get_capabilities(fmu->md)) == jm_status_error) {
		fmi3_capi_free_dll(fmu -> capi);			
		fmi3_capi_destroy_dllfmu(fmu -> capi);
		fmu -> capi = NULL;
		return jm_status_error;
	}
	jm_log_verbose(fmu->callbacks, module, "Successfully loaded all the interface functions"); 

	return jm_status_success;
}

void fmi3_import_set_debug_mode(fmi3_import_t* fmu, int mode) {
	if (fmu == NULL) {
		return;
	}
	fmi3_capi_set_debug_mode(fmu->capi, mode);
}

void fmi3_import_destroy_dllfmu(fmi3_import_t* fmu) {
	
	if (fmu == NULL) {
		return;
	}

	
	if(fmu -> capi) {
		jm_log_verbose(fmu->callbacks, module, "Releasing FMU CAPI interface"); 

		/* Free DLL handle */
		fmi3_capi_free_dll(fmu -> capi);

		/* Destroy the C-API struct */
		fmi3_capi_destroy_dllfmu(fmu -> capi);

		fmu -> capi = NULL;
	}
}

/* FMI 2.0 Common functions */
const char* fmi3_import_get_version(fmi3_import_t* fmu) {
	if(!fmu->capi) {
		jm_log_error(fmu->callbacks, module,"FMU CAPI is not loaded");
		return 0;
	}
	return fmi3_capi_get_version(fmu -> capi);
}

fmi3_status_t fmi3_import_set_debug_logging(fmi3_import_t* fmu, fmi3_boolean_t loggingOn, size_t nCategories, fmi3_string_t categories[]) {
	if(!fmu->capi) {
		jm_log_error(fmu->callbacks, module,"FMU CAPI is not loaded");
		return fmi3_status_fatal;
	}
	return fmi3_capi_set_debug_logging(fmu -> capi, loggingOn, nCategories, categories);
}


jm_status_enu_t fmi3_import_instantiate(fmi3_import_t* fmu,
  fmi3_string_t instanceName, fmi3_type_t fmuType,
  fmi3_string_t fmuResourceLocation, fmi3_boolean_t visible) {
    fmi3_string_t fmuInstantiationToken = fmi3_import_get_instantiation_token(fmu);
    fmi3_boolean_t loggingOn = (fmu->callbacks->log_level > jm_log_level_nothing);
    fmi3_component_t c;
    if(!fmuResourceLocation) 
        fmuResourceLocation = fmu->resourceLocation;
    c = fmi3_capi_instantiate(fmu -> capi, instanceName, fmuType, fmuInstantiationToken,
                              fmuResourceLocation, visible, loggingOn);
    if (c == NULL) {
        return jm_status_error;
    } else {
        return jm_status_success;
    }
}

void fmi3_import_free_instance(fmi3_import_t* fmu) {
    if (fmu != NULL) {
        fmi3_capi_free_instance(fmu -> capi);
    }
}

fmi3_status_t fmi3_import_setup_experiment(fmi3_import_t* fmu,
    fmi3_boolean_t tolerance_defined, fmi3_real_t tolerance,
    fmi3_real_t start_time, fmi3_boolean_t stop_time_defined,
    fmi3_real_t stop_time)
{
    assert(fmu);
    return fmi3_capi_setup_experiment(fmu->capi, tolerance_defined, tolerance,
                                      start_time, stop_time_defined, stop_time);
}

fmi3_status_t fmi3_import_enter_initialization_mode(fmi3_import_t* fmu)
{
    assert(fmu);
    return fmi3_capi_enter_initialization_mode(fmu->capi);
}

fmi3_status_t fmi3_import_exit_initialization_mode(fmi3_import_t* fmu)
{
    assert(fmu);
    return fmi3_capi_exit_initialization_mode(fmu->capi);
}

fmi3_status_t fmi3_import_terminate(fmi3_import_t* fmu) {
	return fmi3_capi_terminate(fmu -> capi);
}

fmi3_status_t fmi3_import_reset(fmi3_import_t* fmu) {
	return fmi3_capi_reset(fmu -> capi);
}


fmi3_status_t fmi3_import_set_real(fmi3_import_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const fmi3_real_t    value[]) {
	return fmi3_capi_set_real(fmu -> capi, vr, nvr, value);
}

fmi3_status_t fmi3_import_set_integer(fmi3_import_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const fmi3_integer_t value[]) {
	return fmi3_capi_set_integer(fmu -> capi, vr, nvr, value);
}

fmi3_status_t fmi3_import_set_boolean(fmi3_import_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const fmi3_boolean_t value[]) {
	return fmi3_capi_set_boolean(fmu -> capi, vr, nvr, value);
}

fmi3_status_t fmi3_import_set_string(fmi3_import_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const fmi3_string_t  value[]) {
	return fmi3_capi_set_string(fmu -> capi, vr, nvr, value);
}

fmi3_status_t fmi3_import_get_real(fmi3_import_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, fmi3_real_t    value[]) {
	return fmi3_capi_get_real(fmu -> capi, vr, nvr, value);
}

fmi3_status_t fmi3_import_get_integer(fmi3_import_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, fmi3_integer_t value[]) {
	return fmi3_capi_get_integer(fmu -> capi, vr, nvr, value);
}

fmi3_status_t fmi3_import_get_boolean(fmi3_import_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, fmi3_boolean_t value[]) {
	return fmi3_capi_get_boolean(fmu -> capi, vr, nvr, value);
}

fmi3_status_t fmi3_import_get_string(fmi3_import_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, fmi3_string_t  value[]) {
	return fmi3_capi_get_string(fmu -> capi, vr, nvr, value);
}

const char* fmi3_import_get_types_platform(fmi3_import_t* fmu) {
	return fmi3_capi_get_types_platform(fmu -> capi);
}

fmi3_status_t fmi3_import_get_fmu_state           (fmi3_import_t* fmu, fmi3_FMU_state_t* s) {
	return fmi3_capi_get_fmu_state(fmu -> capi,s);
}
fmi3_status_t fmi3_import_set_fmu_state           (fmi3_import_t* fmu, fmi3_FMU_state_t s){
	return fmi3_capi_set_fmu_state(fmu -> capi,s);
}
fmi3_status_t fmi3_import_free_fmu_state          (fmi3_import_t* fmu, fmi3_FMU_state_t* s){
	return fmi3_capi_free_fmu_state (fmu -> capi,s);
}
fmi3_status_t fmi3_import_serialized_fmu_state_size(fmi3_import_t* fmu, fmi3_FMU_state_t s, size_t* sz){
	return fmi3_capi_serialized_fmu_state_size(fmu -> capi,s,sz);
}
fmi3_status_t fmi3_import_serialize_fmu_state     (fmi3_import_t* fmu, fmi3_FMU_state_t s , fmi3_byte_t data[], size_t sz){
	return fmi3_capi_serialize_fmu_state(fmu -> capi,s,data,sz);
}
fmi3_status_t fmi3_import_de_serialize_fmu_state  (fmi3_import_t* fmu, const fmi3_byte_t data[], size_t sz, fmi3_FMU_state_t* s){
	return fmi3_capi_de_serialize_fmu_state (fmu -> capi,data,sz,s);
}

fmi3_status_t fmi3_import_get_directional_derivative(fmi3_import_t* fmu, const fmi3_value_reference_t v_ref[], size_t nv,
                                                                   const fmi3_value_reference_t z_ref[], size_t nz,
                                                                   const fmi3_real_t dv[], fmi3_real_t dz[]){
	return fmi3_capi_get_directional_derivative(fmu -> capi,v_ref, nv, z_ref, nz, dv, dz);
}

/* FMI 2.0 ME functions */

fmi3_status_t fmi3_import_enter_event_mode(fmi3_import_t* fmu) {
    return fmi3_capi_enter_event_mode(fmu->capi);
}

fmi3_status_t fmi3_import_new_discrete_states(fmi3_import_t* fmu, fmi3_event_info_t* eventInfo) {
    return fmi3_capi_new_discrete_states(fmu->capi, eventInfo);
}

fmi3_status_t fmi3_import_enter_continuous_time_mode(fmi3_import_t* fmu) {
    return fmi3_capi_enter_continuous_time_mode(fmu->capi);
}

fmi3_status_t fmi3_import_set_time(fmi3_import_t* fmu, fmi3_real_t time) {
	return fmi3_capi_set_time(fmu -> capi, time);
}

fmi3_status_t fmi3_import_set_continuous_states(fmi3_import_t* fmu, const fmi3_real_t x[], size_t nx) {
	return fmi3_capi_set_continuous_states(fmu -> capi, x, nx);
}

fmi3_status_t fmi3_import_completed_integrator_step(fmi3_import_t* fmu,
  fmi3_boolean_t noSetFMUStatePriorToCurrentPoint,
  fmi3_boolean_t* enterEventMode, fmi3_boolean_t* terminateSimulation) {
    return fmi3_capi_completed_integrator_step(fmu -> capi, noSetFMUStatePriorToCurrentPoint,
                                               enterEventMode, terminateSimulation);
}

fmi3_status_t fmi3_import_get_derivatives(fmi3_import_t* fmu, fmi3_real_t derivatives[], size_t nx) {
	return fmi3_capi_get_derivatives(fmu -> capi, derivatives, nx);
}

fmi3_status_t fmi3_import_get_event_indicators(fmi3_import_t* fmu, fmi3_real_t eventIndicators[], size_t ni) {
	return fmi3_capi_get_event_indicators(fmu -> capi, eventIndicators, ni);
}

fmi3_status_t fmi3_import_get_continuous_states(fmi3_import_t* fmu, fmi3_real_t states[], size_t nx) {
	return fmi3_capi_get_continuous_states(fmu -> capi, states, nx);
}

fmi3_status_t fmi3_import_get_nominals_of_continuous_states(fmi3_import_t* fmu, fmi3_real_t x_nominal[], size_t nx) {
	return fmi3_capi_get_nominals_of_continuous_states(fmu -> capi, x_nominal, nx);
}

/* FMI 2.0 CS functions */

fmi3_status_t fmi3_import_set_real_input_derivatives(fmi3_import_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const fmi3_integer_t order[], const  fmi3_real_t value[]) {
	return fmi3_capi_set_real_input_derivatives(fmu -> capi, vr, nvr, order, value);
}
                                                  
fmi3_status_t fmi3_import_get_real_output_derivatives(fmi3_import_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const fmi3_integer_t order[], fmi3_real_t value[]) {
	return fmi3_capi_get_real_output_derivatives(fmu -> capi, vr, nvr, order, value);
}
                                              
fmi3_status_t fmi3_import_cancel_step(fmi3_import_t* fmu) {
	return fmi3_capi_cancel_step(fmu -> capi);
}

fmi3_status_t fmi3_import_do_step(fmi3_import_t* fmu, fmi3_real_t currentCommunicationPoint, fmi3_real_t communicationStepSize, fmi3_boolean_t newStep) {
	return fmi3_capi_do_step(fmu -> capi, currentCommunicationPoint, communicationStepSize, newStep);
}

fmi3_status_t fmi3_import_get_status(fmi3_import_t* fmu, const fmi3_status_kind_t s, fmi3_status_t*  value) {
	return fmi3_capi_get_status(fmu -> capi, s, value);
}

fmi3_status_t fmi3_import_get_real_status(fmi3_import_t* fmu, const fmi3_status_kind_t s, fmi3_real_t*    value) {
	return fmi3_capi_get_real_status(fmu -> capi, s, value);
}

fmi3_status_t fmi3_import_get_integer_status(fmi3_import_t* fmu, const fmi3_status_kind_t s, fmi3_integer_t* value) {
	return fmi3_capi_get_integer_status(fmu -> capi, s, value);
}

fmi3_status_t fmi3_import_get_boolean_status(fmi3_import_t* fmu, const fmi3_status_kind_t s, fmi3_boolean_t* value) {
	return fmi3_capi_get_boolean_status(fmu -> capi, s, value);
}

fmi3_status_t fmi3_import_get_string_status(fmi3_import_t* fmu, const fmi3_status_kind_t s, fmi3_string_t*  value) {
	return fmi3_capi_get_string_status(fmu -> capi, s, value);
}

#ifdef __cplusplus
}
#endif
