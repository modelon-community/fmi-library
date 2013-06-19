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
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include <JM/jm_types.h>
#include <JM/jm_portability.h>

#include <FMI1/fmi1_capi_impl.h>

#define FUNCTION_NAME_LENGTH_MAX 2048			/* Maximum length of FMI function name. Used in the load DLL function. */
#define STRINGIFY(str) #str

/* Loading shared library functions */
static jm_status_enu_t fmi1_capi_get_fcn(fmi1_capi_t* fmu, const char* function_name, jm_dll_function_ptr* dll_function_ptrptr)
{
	char fname[FUNCTION_NAME_LENGTH_MAX];
    int len;
	
	if (strlen(fmu->modelIdentifier) + strlen(function_name) + 2 > FUNCTION_NAME_LENGTH_MAX) {
		jm_log_fatal(fmu->callbacks, FMI_CAPI_MODULE_NAME, "DLL function name is too long. Max name length is set to %s.", STRINGIFY(FUNCTION_NAME_LENGTH_MAX));
		return jm_status_error;
	}	

	len = jm_snprintf(fname,FUNCTION_NAME_LENGTH_MAX,"%s_%s",fmu->modelIdentifier, function_name);

	return jm_portability_load_dll_function(fmu->dllHandle, fname, dll_function_ptrptr);
}


/* Load FMI functions from DLL macro */
#define LOAD_DLL_FUNCTION(FMIFUNCTION) if (fmi1_capi_get_fcn(fmu, #FMIFUNCTION, (jm_dll_function_ptr*)&fmu->FMIFUNCTION) == jm_status_error) { \
	jm_log_error(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Could not load the FMI function '"#FMIFUNCTION"'. %s", jm_portability_get_last_dll_error()); \
	jm_status = jm_status_error; \
}


/* Load FMI 1.0 Co-Simulation functions */
static jm_status_enu_t fmi1_capi_load_cs_fcn(fmi1_capi_t* fmu)
{
	jm_status_enu_t jm_status = jm_status_success;

	jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Loading functions for the co-simulation interface"); 

	/* Workaround for Dymola 2012 and SimulationX 3.x */
	if (fmi1_capi_get_fcn(fmu, "fmiGetTypesPlatform",(jm_dll_function_ptr*)&fmu->fmiGetTypesPlatform) == jm_status_error) {
		jm_log_warning(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Could not load the FMI function 'fmiGetTypesPlatform'. %s. Trying to load fmiGetModelTypesPlatform instead.", jm_portability_get_last_dll_error());
		jm_status = jm_status_warning;
		if (fmi1_capi_get_fcn(fmu, "fmiGetModelTypesPlatform", (jm_dll_function_ptr*)&fmu->fmiGetTypesPlatform) == jm_status_error) {
			jm_log_error(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Could not load the FMI function 'fmiGetModelTypesPlatform'. %s", jm_portability_get_last_dll_error());
			jm_status = jm_status_error;
		}
	}

	LOAD_DLL_FUNCTION(fmiInstantiateSlave);
	LOAD_DLL_FUNCTION(fmiInitializeSlave);
	LOAD_DLL_FUNCTION(fmiTerminateSlave);
	LOAD_DLL_FUNCTION(fmiResetSlave);
	LOAD_DLL_FUNCTION(fmiFreeSlaveInstance);
	LOAD_DLL_FUNCTION(fmiSetRealInputDerivatives);
	LOAD_DLL_FUNCTION(fmiGetRealOutputDerivatives);
	LOAD_DLL_FUNCTION(fmiCancelStep);
	LOAD_DLL_FUNCTION(fmiDoStep);
	LOAD_DLL_FUNCTION(fmiGetStatus);
	LOAD_DLL_FUNCTION(fmiGetRealStatus);
	LOAD_DLL_FUNCTION(fmiGetIntegerStatus);
	LOAD_DLL_FUNCTION(fmiGetBooleanStatus);
	LOAD_DLL_FUNCTION(fmiGetStringStatus);

	LOAD_DLL_FUNCTION(fmiGetVersion);
	LOAD_DLL_FUNCTION(fmiSetDebugLogging);
	LOAD_DLL_FUNCTION(fmiSetReal);
	LOAD_DLL_FUNCTION(fmiSetInteger);
	LOAD_DLL_FUNCTION(fmiSetBoolean);
	LOAD_DLL_FUNCTION(fmiSetString);
	LOAD_DLL_FUNCTION(fmiGetReal);
	LOAD_DLL_FUNCTION(fmiGetInteger);
	LOAD_DLL_FUNCTION(fmiGetBoolean);
	LOAD_DLL_FUNCTION(fmiGetString);
	return jm_status; 
}

/* Load FMI 1.0 Model Exchange functions */
static jm_status_enu_t fmi1_capi_load_me_fcn(fmi1_capi_t* fmu)
{
	jm_status_enu_t jm_status = jm_status_success;

	jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Loading functions for the model exchange interface"); 

	LOAD_DLL_FUNCTION(fmiGetModelTypesPlatform);
	LOAD_DLL_FUNCTION(fmiInstantiateModel);
	LOAD_DLL_FUNCTION(fmiFreeModelInstance);
	LOAD_DLL_FUNCTION(fmiSetTime);
	LOAD_DLL_FUNCTION(fmiSetContinuousStates);
	LOAD_DLL_FUNCTION(fmiCompletedIntegratorStep);
	LOAD_DLL_FUNCTION(fmiInitialize);
	LOAD_DLL_FUNCTION(fmiGetDerivatives);
	LOAD_DLL_FUNCTION(fmiGetEventIndicators);
	LOAD_DLL_FUNCTION(fmiEventUpdate);
	LOAD_DLL_FUNCTION(fmiGetContinuousStates);
	LOAD_DLL_FUNCTION(fmiGetNominalContinuousStates);
	LOAD_DLL_FUNCTION(fmiGetStateValueReferences);
	LOAD_DLL_FUNCTION(fmiTerminate);

	LOAD_DLL_FUNCTION(fmiGetVersion);
	LOAD_DLL_FUNCTION(fmiSetDebugLogging);
	LOAD_DLL_FUNCTION(fmiSetReal);
	LOAD_DLL_FUNCTION(fmiSetInteger);
	LOAD_DLL_FUNCTION(fmiSetBoolean);
	LOAD_DLL_FUNCTION(fmiSetString);
	LOAD_DLL_FUNCTION(fmiGetReal);
	LOAD_DLL_FUNCTION(fmiGetInteger);
	LOAD_DLL_FUNCTION(fmiGetBoolean);
	LOAD_DLL_FUNCTION(fmiGetString);

	return jm_status; 
}

void fmi1_capi_destroy_dllfmu(fmi1_capi_t* fmu)
{
	if (fmu == NULL) {
		return;
	}
	fmi1_capi_free_dll(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Releasing allocated memory");
	fmu->callbacks->free((void*)fmu->dllPath);
	fmu->callbacks->free((void*)fmu->modelIdentifier);
	fmu->callbacks->free((void*)fmu);
}

fmi1_capi_t* fmi1_capi_create_dllfmu(jm_callbacks* cb, const char* dllPath, const char* modelIdentifier, fmi1_callback_functions_t callBackFunctions, fmi1_fmu_kind_enu_t standard)
{
	fmi1_capi_t* fmu = NULL;

	jm_log_debug(cb, FMI_CAPI_MODULE_NAME, "Initializing data stuctures for FMICAPI.");

	/* Minor check for the callbacks */
	if (cb == NULL) {
		assert(0);
		return NULL;
	}

	/* Allocate memory for the FMU instance */
	fmu = (fmi1_capi_t*)cb->calloc(1, sizeof(fmi1_capi_t));
	if (fmu == NULL) { /* Could not allocate memory for the FMU struct */
		jm_log_fatal(cb, FMI_CAPI_MODULE_NAME, "Could not allocate memory for the FMU struct.");
		return NULL;
	}

	/* Set the import package callback functions */
	fmu->callbacks = cb;

	/* Set the FMI callback functions */
	fmu->callBackFunctions = callBackFunctions;

	/* Set FMI standard to load */
	fmu->standard = standard;

	/* Set all memory alloated pointers to NULL */
	fmu->dllPath = NULL;
	fmu->modelIdentifier = NULL;

	/* Copy DLL path */
	fmu->dllPath = (char*)cb->calloc(sizeof(char), strlen(dllPath) + 1);
	if (fmu->dllPath == NULL) {
		jm_log_fatal(cb, FMI_CAPI_MODULE_NAME, "Could not allocate memory for the DLL path string.");
		fmi1_capi_destroy_dllfmu(fmu);
		return NULL;
	}
	strcpy((char*)fmu->dllPath, dllPath);

	/* Copy the modelIdentifier */
	fmu->modelIdentifier = (char*)cb->calloc(sizeof(char), strlen(modelIdentifier) + 1);
	if (fmu->modelIdentifier == NULL) {
		jm_log_fatal(cb, FMI_CAPI_MODULE_NAME, "Could not allocate memory for the modelIdentifier string.");
		fmi1_capi_destroy_dllfmu(fmu);
		return NULL;
	}
	strcpy((char*)fmu->modelIdentifier, modelIdentifier);

	jm_log_debug(cb, FMI_CAPI_MODULE_NAME, "Successfully initialized data stuctures for FMICAPI.");

	/* Everything was succesfull */
	return fmu;
}

jm_status_enu_t fmi1_capi_load_fcn(fmi1_capi_t* fmu)
{
	assert(fmu);
	/* Load ME functions */
	if (fmu->standard == fmi1_fmu_kind_enu_me) {
		return fmi1_capi_load_me_fcn(fmu);
	/* Load CS functions */
	} else if (fmu->standard == fmi1_fmu_kind_enu_cs_standalone || fmu->standard == fmi1_fmu_kind_enu_cs_tool) {
		return fmi1_capi_load_cs_fcn(fmu);
	} else {
		return jm_status_error;
	}
}

jm_status_enu_t fmi1_capi_load_dll(fmi1_capi_t* fmu)
{
	assert(fmu && fmu->dllPath);
	fmu->dllHandle = jm_portability_load_dll_handle(fmu->dllPath); /* Load the shared library */
	if (fmu->dllHandle == NULL) {
		jm_log_fatal(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Could not load the DLL: %s", jm_portability_get_last_dll_error());
		return jm_status_error;
	} else {
		jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Loaded FMU binary from %s", fmu->dllPath);
		return jm_status_success;
	}
}

void fmi1_capi_set_debug_mode(fmi1_capi_t* fmu, int mode) {
	if(fmu)
		fmu->debugMode = mode;
}

int fmi1_capi_get_debug_mode(fmi1_capi_t* fmu) {
	if(fmu) return fmu->debugMode;
	return 0;
}

jm_status_enu_t fmi1_capi_free_dll(fmi1_capi_t* fmu)
{
	if (fmu == NULL) {		
		return jm_status_error; /* Return without writing any log message */
	}

	if (fmu->dllHandle) {
		jm_status_enu_t status =
			(fmu->debugMode != 0) ?
                /* When running valgrind this may be convenient to track mem leaks */ 
                jm_status_success:
                jm_portability_free_dll_handle(fmu->dllHandle);
		fmu->dllHandle = 0;
		if (status == jm_status_error) { /* Free the library handle */
			jm_log(fmu->callbacks, FMI_CAPI_MODULE_NAME, jm_log_level_error, "Could not free the DLL: %s", jm_portability_get_last_dll_error());
			return jm_status_error;
		} else {
			jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Successfully unloaded FMU binary");
			return jm_status_success;
		}
	}
	return jm_status_success;
}

/* Common FMI 1.0 functions */

const char* fmi1_capi_get_version(fmi1_capi_t* fmu)
{
	assert(fmu);
	return fmu->fmiGetVersion();
}

fmi1_status_t fmi1_capi_set_debug_logging(fmi1_capi_t* fmu, fmi1_boolean_t loggingOn)
{
	return fmu->fmiSetDebugLogging(fmu->c, loggingOn);
}

/* fmiSet* functions */
#define FMISETX(FNAME1, FNAME2, FTYPE) \
fmi1_status_t FNAME1(fmi1_capi_t* fmu, const fmi1_value_reference_t vr[], size_t nvr, const FTYPE value[])	\
{ \
	return fmu->FNAME2(fmu->c, vr, nvr, value); \
}

/* fmiGet* functions */
#define FMIGETX(FNAME1, FNAME2, FTYPE) \
fmi1_status_t FNAME1(fmi1_capi_t* fmu, const fmi1_value_reference_t vr[], size_t nvr, FTYPE value[]) \
{ \
	return fmu->FNAME2(fmu->c, vr, nvr, value); \
}

FMISETX(fmi1_capi_set_real,		fmiSetReal,		fmi1_real_t)
FMISETX(fmi1_capi_set_integer,	fmiSetInteger,	fmi1_integer_t)
FMISETX(fmi1_capi_set_boolean,	fmiSetBoolean,	fmi1_boolean_t)
FMISETX(fmi1_capi_set_string,	fmiSetString,	fmi1_string_t)

FMIGETX(fmi1_capi_get_real,	fmiGetReal,		fmi1_real_t)
FMIGETX(fmi1_capi_get_integer,	fmiGetInteger,	fmi1_integer_t)
FMIGETX(fmi1_capi_get_boolean,	fmiGetBoolean,	fmi1_boolean_t)
FMIGETX(fmi1_capi_get_string,	fmiGetString,	fmi1_string_t)

#ifdef __cplusplus
}
#endif
