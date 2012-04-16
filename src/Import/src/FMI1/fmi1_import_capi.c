/*
    Copyright (C) 2012 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef __cplusplus
extern "C" {
#endif

#include "fmi1_import_impl.h"
#include <FMI1/fmi1_types.h>
#include <FMI1/fmi1_functions.h>
#include <FMI1/fmi1_enums.h>
#include <FMI1/fmi1_capi.h>


/* Load and destroy functions */
jm_status_enu_t fmi1_import_create_dllfmu(fmi1_import_t* fmu, fmi1_callback_functions_t callBackFunctions) {

	const char* dllPath; 
	const char* modelIdentifier;
	fmi1_fmu_kind_enu_t standard;

	if (fmu == NULL) {
		return jm_status_error;
	}

	standard = fmi1_import_get_fmu_kind(fmu);

	modelIdentifier = fmi1_import_get_model_identifier(fmu);
	if (modelIdentifier == NULL) {
		return jm_status_error;
	}

	dllPath = fmi_import_get_dll_path(fmu->dirPath, modelIdentifier, fmu->callbacks);
	if (dllPath == NULL) {
		return jm_status_error;
	}

	/* Allocate memory for the C-API struct */
	fmu -> capi = fmi1_capi_create_dllfmu(fmu->callbacks, dllPath, modelIdentifier, callBackFunctions, standard);	
	if (fmu -> capi == NULL) {
		fmu->callbacks->free((jm_voidp)dllPath);
		return jm_status_error;
	}

	/* Load the DLL handle */
	if (fmi1_capi_load_dll(fmu -> capi) == jm_status_error) {		
		fmi1_capi_destroy_dllfmu(fmu -> capi);
		fmu->callbacks->free((jm_voidp)dllPath);
		return jm_status_error;
	}


	/* Load the DLL functions */
	if (fmi1_capi_load_fcn(fmu -> capi) == jm_status_error) {
		fmi1_capi_free_dll(fmu -> capi);			
		fmi1_capi_destroy_dllfmu(fmu -> capi);
		fmu->callbacks->free((jm_voidp)dllPath);
		return jm_status_error;
	}

	return jm_status_success;
}

void fmi1_import_destroy_dllfmu(fmi1_import_t* fmu) {

	if (fmu == NULL) {
		return;
	}

	/* Free DLL handle */
	fmi1_capi_free_dll(fmu -> capi);

	/* Destroy the C-API struct */
	fmi1_capi_destroy_dllfmu(fmu -> capi);	
}

/* FMI 1.0 Common functions */
const char* fmi1_import_get_version(fmi1_import_t* fmu) {
	return fmi1_capi_get_version(fmu -> capi);
}

fmi1_status_t fmi1_import_set_debug_logging(fmi1_import_t* fmu, fmi1_boolean_t loggingOn) {
	return fmi1_capi_set_debug_logging(fmu -> capi, loggingOn);
}

fmi1_status_t fmi1_import_set_real(fmi1_import_t* fmu, const fmi1_value_reference_t vr[], size_t nvr, const fmi1_real_t    value[]) {
	return fmi1_capi_set_real(fmu -> capi, vr, nvr, value);
}

fmi1_status_t fmi1_import_set_integer(fmi1_import_t* fmu, const fmi1_value_reference_t vr[], size_t nvr, const fmi1_integer_t value[]) {
	return fmi1_capi_set_integer(fmu -> capi, vr, nvr, value);
}

fmi1_status_t fmi1_import_set_boolean(fmi1_import_t* fmu, const fmi1_value_reference_t vr[], size_t nvr, const fmi1_boolean_t value[]) {
	return fmi1_capi_set_boolean(fmu -> capi, vr, nvr, value);
}

fmi1_status_t fmi1_import_set_string(fmi1_import_t* fmu, const fmi1_value_reference_t vr[], size_t nvr, const fmi1_string_t  value[]) {
	return fmi1_capi_set_string(fmu -> capi, vr, nvr, value);
}

fmi1_status_t fmi1_import_get_real(fmi1_import_t* fmu, const fmi1_value_reference_t vr[], size_t nvr, fmi1_real_t    value[]) {
	return fmi1_capi_get_real(fmu -> capi, vr, nvr, value);
}

fmi1_status_t fmi1_import_get_integer(fmi1_import_t* fmu, const fmi1_value_reference_t vr[], size_t nvr, fmi1_integer_t value[]) {
	return fmi1_capi_get_integer(fmu -> capi, vr, nvr, value);
}

fmi1_status_t fmi1_import_get_boolean(fmi1_import_t* fmu, const fmi1_value_reference_t vr[], size_t nvr, fmi1_boolean_t value[]) {
	return fmi1_capi_get_boolean(fmu -> capi, vr, nvr, value);
}

fmi1_status_t fmi1_import_get_string(fmi1_import_t* fmu, const fmi1_value_reference_t vr[], size_t nvr, fmi1_string_t  value[]) {
	return fmi1_capi_get_string(fmu -> capi, vr, nvr, value);
}


/* FMI 1.0 ME functions */
const char* fmi1_import_get_model_types_platform(fmi1_import_t* fmu) {
	return fmi1_capi_get_model_types_platform(fmu -> capi);
}

jm_status_enu_t fmi1_import_instantiate_model(fmi1_import_t* fmu, fmi1_string_t instanceName, fmi1_string_t GUID, fmi1_boolean_t loggingOn) {
	fmi1_component_t c;
	
	c = fmi1_capi_instantiate_model(fmu -> capi, instanceName, GUID, loggingOn);
	if (c == NULL) {
		return jm_status_error;
	} else {
		return jm_status_success;
	}
}

void fmi1_import_free_model_instance(fmi1_import_t* fmu) {
	fmi1_capi_free_model_instance(fmu -> capi);
}

fmi1_status_t fmi1_import_set_time(fmi1_import_t* fmu, fmi1_real_t time) {
	return fmi1_capi_set_time(fmu -> capi, time);
}

fmi1_status_t fmi1_import_set_continuous_states(fmi1_import_t* fmu, const fmi1_real_t x[], size_t nx) {
	return fmi1_capi_set_continuous_states(fmu -> capi, x, nx);
}

fmi1_status_t fmi1_import_completed_integrator_step(fmi1_import_t* fmu, fmi1_boolean_t* callEventUpdate) {
	return fmi1_capi_completed_integrator_step(fmu -> capi, callEventUpdate);
}

fmi1_status_t fmi1_import_initialize(fmi1_import_t* fmu, fmi1_boolean_t toleranceControlled, fmi1_real_t relativeTolerance, fmi1_event_info_t* eventInfo) {
	return fmi1_capi_initialize(fmu -> capi, toleranceControlled, relativeTolerance, eventInfo);
}

fmi1_status_t fmi1_import_get_derivatives(fmi1_import_t* fmu, fmi1_real_t derivatives[], size_t nx) {
	return fmi1_capi_get_derivatives(fmu -> capi, derivatives, nx);
}

fmi1_status_t fmi1_import_get_event_indicators(fmi1_import_t* fmu, fmi1_real_t eventIndicators[], size_t ni) {
	return fmi1_capi_get_event_indicators(fmu -> capi, eventIndicators, ni);
}

fmi1_status_t fmi1_import_eventUpdate(fmi1_import_t* fmu, fmi1_boolean_t intermediateResults, fmi1_event_info_t* eventInfo) {
	return fmi1_capi_eventUpdate(fmu -> capi, intermediateResults, eventInfo);
}

fmi1_status_t fmi1_import_get_continuous_states(fmi1_import_t* fmu, fmi1_real_t states[], size_t nx) {
	return fmi1_capi_get_continuous_states(fmu -> capi, states, nx);
}

fmi1_status_t fmi1_import_get_nominal_continuous_states(fmi1_import_t* fmu, fmi1_real_t x_nominal[], size_t nx) {
	return fmi1_capi_get_nominal_continuous_states(fmu -> capi, x_nominal, nx);
}

fmi1_status_t fmi1_import_get_state_value_references(fmi1_import_t* fmu, fmi1_value_reference_t vrx[], size_t nx) {
	return fmi1_capi_get_state_value_references(fmu -> capi, vrx, nx);
}

fmi1_status_t fmi1_import_terminate(fmi1_import_t* fmu) {
	return fmi1_capi_terminate(fmu -> capi);
}


/* FMI 1.0 CS functions */
const char* fmi1_import_get_types_platform(fmi1_import_t* fmu) {
	return fmi1_capi_get_types_platform(fmu -> capi);
}

jm_status_enu_t fmi1_import_instantiate_slave(fmi1_import_t* fmu, fmi1_string_t instanceName, fmi1_string_t fmuGUID, fmi1_string_t fmuLocation, fmi1_string_t mimeType,
																 fmi1_real_t timeout, fmi1_boolean_t visible, fmi1_boolean_t interactive, fmi1_boolean_t loggingOn) {
	fmi1_component_t c;

	c = fmi1_capi_instantiate_slave(fmu -> capi, instanceName, fmuGUID, fmuLocation, mimeType, timeout, visible, interactive, loggingOn);
	if (c == NULL) {
		return jm_status_error;
	} else {
		return jm_status_success;
	}
}

fmi1_status_t fmi1_import_initialize_slave(fmi1_import_t* fmu, fmi1_real_t tStart, fmi1_boolean_t StopTimeDefined, fmi1_real_t tStop) {
	return fmi1_capi_initialize_slave(fmu -> capi, tStart, StopTimeDefined, tStop);
}

fmi1_status_t fmi1_import_terminate_slave(fmi1_import_t* fmu) {
	return fmi1_capi_terminate_slave(fmu -> capi);
}

fmi1_status_t fmi1_import_reset_slave(fmi1_import_t* fmu) {
	return fmi1_capi_reset_slave(fmu -> capi);
}

void fmi1_import_free_slave_instance(fmi1_import_t* fmu) {
	fmi1_capi_free_slave_instance(fmu -> capi);
}

fmi1_status_t fmi1_import_set_real_input_derivatives(fmi1_import_t* fmu, const fmi1_value_reference_t vr[], size_t nvr, const fmi1_integer_t order[], const  fmi1_real_t value[]) {
	return fmi1_capi_set_real_input_derivatives(fmu -> capi, vr, nvr, order, value);
}
                                                  
fmi1_status_t fmi1_import_get_real_output_derivatives(fmi1_import_t* fmu, const fmi1_value_reference_t vr[], size_t nvr, const fmi1_integer_t order[], fmi1_real_t value[]) {
	return fmi1_capi_get_real_output_derivatives(fmu -> capi, vr, nvr, order, value);
}
                                              
fmi1_status_t fmi1_import_cancel_step(fmi1_import_t* fmu) {
	return fmi1_capi_cancel_step(fmu -> capi);
}

fmi1_status_t fmi1_import_do_step(fmi1_import_t* fmu, fmi1_real_t currentCommunicationPoint, fmi1_real_t communicationStepSize, fmi1_boolean_t newStep) {
	return fmi1_capi_do_step(fmu -> capi, currentCommunicationPoint, communicationStepSize, newStep);
}

fmi1_status_t fmi1_import_get_status(fmi1_import_t* fmu, const fmi1_status_kind_t s, fmi1_status_t*  value) {
	return fmi1_capi_get_status(fmu -> capi, s, value);
}

fmi1_status_t fmi1_import_get_real_status(fmi1_import_t* fmu, const fmi1_status_kind_t s, fmi1_real_t*    value) {
	return fmi1_capi_get_real_status(fmu -> capi, s, value);
}

fmi1_status_t fmi1_import_get_integer_status(fmi1_import_t* fmu, const fmi1_status_kind_t s, fmi1_integer_t* value) {
	return fmi1_capi_get_integer_status(fmu -> capi, s, value);
}

fmi1_status_t fmi1_import_get_boolean_status(fmi1_import_t* fmu, const fmi1_status_kind_t s, fmi1_boolean_t* value) {
	return fmi1_capi_get_boolean_status(fmu -> capi, s, value);
}

fmi1_status_t fmi1_import_get_string_status(fmi1_import_t* fmu, const fmi1_status_kind_t s, fmi1_string_t*  value) {
	return fmi1_capi_get_string_status(fmu -> capi, s, value);
}

#ifdef __cplusplus
}
#endif