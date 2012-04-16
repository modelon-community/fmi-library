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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <jm_types.h>
#include <jm_portability.h>

#include <FMI1/fmi1_capi.h>
#include <FMI1/fmi1_capi_impl.h>

#define FUNCTION_NAME_LENGTH_MAX 2048			/* Maximum length of FMI function name. Used in the load DLL function. */
#define STRINGIFY(str) #str

/* Loading shared library functions */
static jm_status_enu_t fmi1_capi_get_fcn(fmi1_capi_t* fmu, const char* function_name, void** dll_function_ptrptr)
{
	char fname[FUNCTION_NAME_LENGTH_MAX];
	
	if (strlen(fmu->modelIdentifier) + strlen(function_name) + 2 > FUNCTION_NAME_LENGTH_MAX) {
		jm_log(fmu->callbacks, LOGGER_MODULE_NAME, jm_log_level_error,  "DLL function name is too long. Max name length is set to %s.", STRINGIFY(FUNCTION_NAME_LENGTH_MAX));
		return jm_status_error;
	}	

	sprintf(fname,"%s_%s",fmu->modelIdentifier, function_name);

	return jm_portability_load_dll_function(fmu->dllHandle, fname, dll_function_ptrptr);
}


/* Load FMI functions from DLL macro */
#define LOAD_DLL_FUNCTION(FMIFUNCTION) if (fmi1_capi_get_fcn(fmu, #FMIFUNCTION, (void**)&fmu->FMIFUNCTION) == jm_status_error) { \
	jm_log(fmu->callbacks, LOGGER_MODULE_NAME, jm_log_level_error, "Could not load the FMI function '"#FMIFUNCTION"'. %s", jm_portability_get_last_dll_error()); \
	return jm_status_error; \
}


/* Load FMI 1.0 Co-Simulation functions */
static jm_status_enu_t fmi1_capi_load_cs_fcn(fmi1_capi_t* fmu)
{
	/* Workaround for Dymola 2012 and SimulationX 3.x */
	if (fmi1_capi_get_fcn(fmu, "fmiGetTypesPlatform", (void**)&fmu->fmiGetTypesPlatform) == jm_status_error) {
		if (fmi1_capi_get_fcn(fmu, "fmiGetModelTypesPlatform", (void**)&fmu->fmiGetTypesPlatform) == jm_status_error) {
			jm_log(fmu->callbacks, LOGGER_MODULE_NAME, jm_log_level_error, "Could not load any of the FMI functions 'fmiGetModelTypesPlatform' or 'fmiGetTypesPlatform'. %s", jm_portability_get_last_dll_error());
			return jm_status_error;
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
	return jm_status_success; 
}

/* Load FMI 1.0 Model Exchange functions */
static jm_status_enu_t fmi1_capi_load_me_fcn(fmi1_capi_t* fmu)
{
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
	return jm_status_success; 
}

void fmi1_capi_destroy_dllfmu(fmi1_capi_t* fmu)
{
	if (fmu == NULL) {
		return;
	}

	fmu->callbacks->free((void*)fmu->dllPath);
	fmu->callbacks->free((void*)fmu->modelIdentifier);
	fmu->callbacks->free((void*)fmu);
}

fmi1_capi_t* fmi1_capi_create_dllfmu(jm_callbacks* cb, const char* dllPath, const char* modelIdentifier, fmi1_callback_functions_t callBackFunctions, fmi1_fmu_kind_enu_t standard)
{
	fmi1_capi_t* fmu = NULL;

	/* Minor check for the callbacks */
	if (cb == NULL) {
		return NULL;
	}

	/* Allocate memory for the FMU instance */
	fmu = (fmi1_capi_t*)cb->calloc(1, sizeof(fmi1_capi_t));
	if (fmu == NULL) { /* Could not allocate memory for the FMU struct */
		jm_log(cb, LOGGER_MODULE_NAME, jm_log_level_error, "Could not allocate memory for the FMU struct.");
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
		jm_log(cb, LOGGER_MODULE_NAME, jm_log_level_error, "Could not allocate memory for the DLL path string.");
		fmi1_capi_destroy_dllfmu(fmu);
		return NULL;
	}
	strcpy((char*)fmu->dllPath, dllPath);

	/* Copy the modelIdentifier */
	fmu->modelIdentifier = (char*)cb->calloc(sizeof(char), strlen(modelIdentifier) + 1);
	if (fmu->modelIdentifier == NULL) {
		jm_log(cb, LOGGER_MODULE_NAME, jm_log_level_error, "Could not allocate memory for the modelIdentifier string.");
		fmi1_capi_destroy_dllfmu(fmu);
		return NULL;
	}
	strcpy((char*)fmu->modelIdentifier, modelIdentifier);

	/* Everything was succesfull */
	return fmu;
}

jm_status_enu_t fmi1_capi_load_fcn(fmi1_capi_t* fmu)
{
	switch (fmu->standard) {
		case fmi1_fmu_kind_enu_me:				/* Load FMI 1.0 Model Exchange functions */
			if (fmi1_capi_load_me_fcn(fmu) == jm_status_error) {
				return jm_status_error;
			}
			break;
		case fmi1_fmu_kind_enu_cs_standalone:	/* Load FMI 1.0 Co-Simulation functions */
			if (fmi1_capi_load_cs_fcn(fmu) == jm_status_error) {
				return jm_status_error;
			}
			break;
	}
	return jm_status_success;
}

jm_status_enu_t fmi1_capi_load_dll(fmi1_capi_t* fmu)
{
	fmu->dllHandle = jm_portability_load_dll_handle(fmu->dllPath); /* Load the shared library */
	if (fmu->dllHandle == NULL) {
		jm_log(fmu->callbacks, LOGGER_MODULE_NAME, jm_log_level_error, "Could not load the DLL: %s", jm_portability_get_last_dll_error());
		return jm_status_error;
	} else {
		return jm_status_success;
	}
}

jm_status_enu_t fmi1_capi_free_dll(fmi1_capi_t* fmu)
{
	if (fmu->dllHandle) {		
		if (jm_portability_free_dll_handle(fmu->dllHandle) == jm_status_error) { /* Free the library handle */
			jm_log(fmu->callbacks, LOGGER_MODULE_NAME, jm_log_level_error, "Could not free the DLL: %s", jm_portability_get_last_dll_error());
			return jm_status_error;
		} else {
			return jm_status_success;
		}
	}
	return jm_status_success;
}

/* Common FMI 1.0 functions */

const char* fmi1_capi_get_version(fmi1_capi_t* fmu)
{
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
