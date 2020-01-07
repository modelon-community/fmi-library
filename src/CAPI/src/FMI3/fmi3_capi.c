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
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include <JM/jm_types.h>
#include <JM/jm_portability.h>

#include <FMI3/fmi3_capi_impl.h>

#define FUNCTION_NAME_LENGTH_MAX 2048			/* Maximum length of FMI function name. Used in the load DLL function. */
#define STRINGIFY(str) #str

/* Loading shared library functions */
static jm_status_enu_t fmi3_capi_get_fcn(fmi3_capi_t* fmu, const char* function_name, jm_dll_function_ptr* dll_function_ptrptr, jm_status_enu_t* status )
{
		jm_status_enu_t jm_status = jm_portability_load_dll_function(fmu->dllHandle, (char*)function_name, dll_function_ptrptr);
		if (jm_status == jm_status_error) {
			jm_log_error(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Could not load the FMI function '%s'. %s", function_name, jm_portability_get_last_dll_error());
			*status = jm_status_error;
		}
		return jm_status;
}

static void fmi3_capi_get_fcn_with_flag(fmi3_capi_t* fmu, const char* function_name,
													jm_dll_function_ptr* dll_function_ptrptr,
													unsigned int capabilities[],
													fmi3_capabilities_enu_t flag) {
	jm_status_enu_t status = jm_status_success;
	if(capabilities[flag]) {
		fmi3_capi_get_fcn(fmu, function_name, dll_function_ptrptr, &status);
		if(status != jm_status_success) {
			jm_log_warning(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Resetting flag '%s'", fmi3_capability_to_string(flag));
			capabilities[flag] = 0;
		}
	}
}

/* Load FMI functions from DLL macro */
#define LOAD_DLL_FUNCTION(FMIFUNCTION) fmi3_capi_get_fcn(fmu, #FMIFUNCTION, (jm_dll_function_ptr*)&fmu->FMIFUNCTION, &jm_status)

/* Load FMI functions from DLL macro for functions controlled by capability flags */
#define LOAD_DLL_FUNCTION_WITH_FLAG(FMIFUNCTION, FLAG) \
	fmi3_capi_get_fcn_with_flag(fmu, #FMIFUNCTION, (jm_dll_function_ptr*)&fmu->FMIFUNCTION, capabilities, FLAG)

static jm_status_enu_t fmi3_capi_load_common_fcn(fmi3_capi_t* fmu, unsigned int capabilities[])
{
	jm_status_enu_t jm_status = jm_status_success;
	/***************************************************
Types for Common Functions
****************************************************/

/* Inquire version numbers of header files and setting logging status */
/*   typedef const char* fmi3GetTypesPlatformTYPE(void);
   typedef const char* fmi3GetVersionTYPE(void);
   typedef fmi3Status  fmi3SetDebugLoggingTYPE(fmi3Component, fmi3Boolean, size_t, const fmi3String[]); */

	LOAD_DLL_FUNCTION(fmi3GetTypesPlatform);
	LOAD_DLL_FUNCTION(fmi3GetVersion);
	LOAD_DLL_FUNCTION(fmi3SetDebugLogging);

/* Enter and exit initialization mode, terminate and reset */
/* typedef fmi3Status fmi3TerminateTYPE              (fmi3Component);
   typedef fmi3Status fmi3ResetTYPE                  (fmi3Component); */
	LOAD_DLL_FUNCTION(fmi3Terminate);
	LOAD_DLL_FUNCTION(fmi3Reset);

    /* Creation and destruction of instances and setting debug status */
    /*typedef fmi3Component fmi3InstantiateTYPE (fmi3String, fmi3Type, fmi3String, fmi3String, const fmi3CallbackFunctions*, fmi3Boolean, fmi3Boolean);
    typedef void          fmi3FreeInstanceTYPE(fmi3Component);*/
    LOAD_DLL_FUNCTION(fmi3Instantiate);
    LOAD_DLL_FUNCTION(fmi3FreeInstance);

   /* typedef fmi3Status fmi3SetupExperimentTYPE        (fmi3Component, fmi3Boolean, fmi3Real, fmi3Real, fmi3Boolean, fmi3Real);
   typedef fmi3Status fmi3EnterInitializationModeTYPE(fmi3Component);
   typedef fmi3Status fmi3ExitInitializationModeTYPE (fmi3Component); */
    LOAD_DLL_FUNCTION(fmi3SetupExperiment);
    LOAD_DLL_FUNCTION(fmi3EnterInitializationMode);
    LOAD_DLL_FUNCTION(fmi3ExitInitializationMode);

	/* Getting and setting variable values */
/*   typedef fmi3Status fmi3GetRealTYPE   (fmi3Component, const fmi3ValueReference[], size_t, fmi3Real   []);
   typedef fmi3Status fmi3GetIntegerTYPE(fmi3Component, const fmi3ValueReference[], size_t, fmi3Integer[]);
   typedef fmi3Status fmi3GetBooleanTYPE(fmi3Component, const fmi3ValueReference[], size_t, fmi3Boolean[]);
   typedef fmi3Status fmi3GetStringTYPE (fmi3Component, const fmi3ValueReference[], size_t, fmi3String []); */

	LOAD_DLL_FUNCTION(fmi3GetReal);
	LOAD_DLL_FUNCTION(fmi3GetInteger);
	LOAD_DLL_FUNCTION(fmi3GetBoolean);
	LOAD_DLL_FUNCTION(fmi3GetString);

	/*   typedef fmi3Status fmi3SetRealTYPE   (fmi3Component, const fmi3ValueReference[], size_t, const fmi3Real   []);
   typedef fmi3Status fmi3SetIntegerTYPE(fmi3Component, const fmi3ValueReference[], size_t, const fmi3Integer[]);
   typedef fmi3Status fmi3SetBooleanTYPE(fmi3Component, const fmi3ValueReference[], size_t, const fmi3Boolean[]);
   typedef fmi3Status fmi3SetStringTYPE (fmi3Component, const fmi3ValueReference[], size_t, const fmi3String []); */

	LOAD_DLL_FUNCTION(fmi3SetReal);
	LOAD_DLL_FUNCTION(fmi3SetInteger);
	LOAD_DLL_FUNCTION(fmi3SetBoolean);
	LOAD_DLL_FUNCTION(fmi3SetString);

	return jm_status;
}

/* Load FMI 3.0 Co-Simulation functions */
static jm_status_enu_t fmi3_capi_load_cs_fcn(fmi3_capi_t* fmu, unsigned int capabilities[])
{
	jm_status_enu_t jm_status = jm_status_success;

	jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Loading functions for the co-simulation interface");

	jm_status = fmi3_capi_load_common_fcn(fmu, capabilities);

	/* Getting and setting the internal FMU state */
/*   typedef fmi3Status fmi3GetFMUstateTYPE           (fmi3Component, fmi3FMUstate*);
   typedef fmi3Status fmi3SetFMUstateTYPE           (fmi3Component, fmi3FMUstate);
   typedef fmi3Status fmi3FreeFMUstateTYPE          (fmi3Component, fmi3FMUstate*);
   typedef fmi3Status fmi3SerializedFMUstateSizeTYPE(fmi3Component, fmi3FMUstate, size_t*);
   typedef fmi3Status fmi3SerializeFMUstateTYPE     (fmi3Component, fmi3FMUstate, fmi3Byte[], size_t);
   typedef fmi3Status fmi3DeSerializeFMUstateTYPE   (fmi3Component, const fmi3Byte[], size_t, fmi3FMUstate*); */
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi3GetFMUstate,fmi3_cs_canGetAndSetFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi3SetFMUstate,fmi3_cs_canGetAndSetFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi3FreeFMUstate,fmi3_cs_canGetAndSetFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi3SerializedFMUstateSize,fmi3_cs_canSerializeFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi3SerializeFMUstate,fmi3_cs_canSerializeFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi3DeSerializeFMUstate,fmi3_cs_canSerializeFMUstate);

/* Getting directional derivatives */
/*   typedef fmi3Status fmi3GetDirectionalDerivativeTYPE(fmi3Component, const fmi3ValueReference[], size_t,
                                                                   const fmi3ValueReference[], size_t,
                                                                   const fmi3Real[], fmi3Real[]);*/
    LOAD_DLL_FUNCTION_WITH_FLAG(fmi3GetDirectionalDerivative,fmi3_cs_providesDirectionalDerivatives);

/* Simulating the slave */
/*   typedef fmi3Status fmi3SetRealInputDerivativesTYPE (fmi3Component, const fmi3ValueReference [], size_t, const fmi3Integer [], const fmi3Real []);
   typedef fmi3Status fmi3GetRealOutputDerivativesTYPE(fmi3Component, const fmi3ValueReference [], size_t, const fmi3Integer [], fmi3Real []); */
	LOAD_DLL_FUNCTION(fmi3SetRealInputDerivatives);
	LOAD_DLL_FUNCTION(fmi3GetRealOutputDerivatives);

/*   typedef fmi3Status fmi3DoStepTYPE     (fmi3Component, fmi3Real, fmi3Real, fmi3Boolean);
   typedef fmi3Status fmi3CancelStepTYPE (fmi3Component); */
	LOAD_DLL_FUNCTION(fmi3CancelStep);
	LOAD_DLL_FUNCTION(fmi3DoStep);

/* Inquire slave status */
/*   typedef fmi3Status fmi3GetStatusTYPE       (fmi3Component, const fmi3StatusKind, fmi3Status* );
   typedef fmi3Status fmi3GetRealStatusTYPE   (fmi3Component, const fmi3StatusKind, fmi3Real*   );
   typedef fmi3Status fmi3GetIntegerStatusTYPE(fmi3Component, const fmi3StatusKind, fmi3Integer*);
   typedef fmi3Status fmi3GetBooleanStatusTYPE(fmi3Component, const fmi3StatusKind, fmi3Boolean*);
   typedef fmi3Status fmi3GetStringStatusTYPE (fmi3Component, const fmi3StatusKind, fmi3String* ); */
	LOAD_DLL_FUNCTION(fmi3GetStatus);
	LOAD_DLL_FUNCTION(fmi3GetRealStatus);
	LOAD_DLL_FUNCTION(fmi3GetIntegerStatus);
	LOAD_DLL_FUNCTION(fmi3GetBooleanStatus);
	LOAD_DLL_FUNCTION(fmi3GetStringStatus);

	return jm_status;
}

/* Load FMI 3.0 Model Exchange functions */
static jm_status_enu_t fmi3_capi_load_me_fcn(fmi3_capi_t* fmu, unsigned int capabilities[])
{
	jm_status_enu_t jm_status = jm_status_success;

	jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Loading functions for the model exchange interface");

	jm_status = fmi3_capi_load_common_fcn(fmu, capabilities);
		/* Getting and setting the internal FMU state */
/*   typedef fmi3Status fmi3GetFMUstateTYPE           (fmi3Component, fmi3FMUstate*);
   typedef fmi3Status fmi3SetFMUstateTYPE           (fmi3Component, fmi3FMUstate);
   typedef fmi3Status fmi3FreeFMUstateTYPE          (fmi3Component, fmi3FMUstate*);
   typedef fmi3Status fmi3SerializedFMUstateSizeTYPE(fmi3Component, fmi3FMUstate, size_t*);
   typedef fmi3Status fmi3SerializeFMUstateTYPE     (fmi3Component, fmi3FMUstate, fmi3Byte[], size_t);
   typedef fmi3Status fmi3DeSerializeFMUstateTYPE   (fmi3Component, const fmi3Byte[], size_t, fmi3FMUstate*); */
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi3GetFMUstate,fmi3_me_canGetAndSetFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi3SetFMUstate,fmi3_me_canGetAndSetFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi3FreeFMUstate,fmi3_me_canGetAndSetFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi3SerializedFMUstateSize,fmi3_me_canSerializeFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi3SerializeFMUstate,fmi3_me_canSerializeFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi3DeSerializeFMUstate,fmi3_me_canSerializeFMUstate);

/* Getting directional derivatives */
/*   typedef fmi3Status fmi3GetDirectionalDerivativeTYPE(fmi3Component, const fmi3ValueReference[], size_t,
                                                                   const fmi3ValueReference[], size_t,
                                                                   const fmi3Real[], fmi3Real[]); */
    LOAD_DLL_FUNCTION_WITH_FLAG(fmi3GetDirectionalDerivative,fmi3_me_providesDirectionalDerivatives);

/* Enter and exit the different modes */
/*   typedef fmi3Status fmi3EnterEventModeTYPE         (fmi3Component);
   typedef fmi3Status fmi3NewDiscreteStatesTYPE      (fmi3Component, fmi3EventInfo*);
   typedef fmi3Status fmi3EnterContinuousTimeModeTYPE(fmi3Component);
   typedef fmi3Status fmi3CompletedIntegratorStepTYPE(fmi3Component, fmi3Boolean, fmi3Boolean*, fmi3Boolean*);*/
	LOAD_DLL_FUNCTION(fmi3EnterEventMode);
	LOAD_DLL_FUNCTION(fmi3NewDiscreteStates);
	LOAD_DLL_FUNCTION(fmi3EnterContinuousTimeMode);
	LOAD_DLL_FUNCTION(fmi3CompletedIntegratorStep);

/* Providing independent variables and re-initialization of caching */
   /*typedef fmi3Status fmi3SetTimeTYPE            (fmi3Component, fmi3Real);
   typedef fmi3Status fmi3SetContinuousStatesTYPE(fmi3Component, const fmi3Real[], size_t);*/
	LOAD_DLL_FUNCTION(fmi3SetTime);
	LOAD_DLL_FUNCTION(fmi3SetContinuousStates);

/* Evaluation of the model equations */

/*
   typedef fmi3Status fmi3GetDerivativesTYPE               (fmi3Component, fmi3Real[], size_t);
   typedef fmi3Status fmi3GetEventIndicatorsTYPE           (fmi3Component, fmi3Real[], size_t);
   typedef fmi3Status fmi3GetContinuousStatesTYPE          (fmi3Component, fmi3Real[], size_t);
   typedef fmi3Status fmi3GetNominalsOfContinuousStatesTYPE(fmi3Component, fmi3Real[], size_t);*/
	LOAD_DLL_FUNCTION(fmi3GetDerivatives);
	LOAD_DLL_FUNCTION(fmi3GetEventIndicators);
	LOAD_DLL_FUNCTION(fmi3GetContinuousStates);
	LOAD_DLL_FUNCTION(fmi3GetNominalsOfContinuousStates);

	return jm_status;
}

void fmi3_capi_destroy_dllfmu(fmi3_capi_t* fmu)
{
	if (fmu == NULL) {
		return;
	}
	fmi3_capi_free_dll(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Releasing allocated memory");
	fmu->callbacks->free((void*)fmu->dllPath);
	fmu->callbacks->free((void*)fmu->modelIdentifier);
	fmu->callbacks->free((void*)fmu);
}

fmi3_capi_t* fmi3_capi_create_dllfmu(jm_callbacks* cb, const char* dllPath, const char* modelIdentifier, const fmi3_callback_functions_t* callBackFunctions, fmi3_fmu_kind_enu_t standard)
{
	fmi3_capi_t* fmu = NULL;

	jm_log_debug(cb, FMI_CAPI_MODULE_NAME, "Initializing data structures for FMICAPI.");

	/* Minor check for the callbacks */
	if (cb == NULL) {
		assert(0);
		return NULL;
	}

	/* Allocate memory for the FMU instance */
	fmu = (fmi3_capi_t*)cb->calloc(1, sizeof(fmi3_capi_t));
	if (fmu == NULL) { /* Could not allocate memory for the FMU struct */
		jm_log_fatal(cb, FMI_CAPI_MODULE_NAME, "Could not allocate memory for the FMU struct.");
		return NULL;
	}

	/* Set the import package callback functions */
	fmu->callbacks = cb;

	/* Set the FMI callback functions */
	fmu->callBackFunctions = *callBackFunctions;

	/* Set FMI standard to load */
	fmu->standard = standard;

	/* Set all memory alloated pointers to NULL */
	fmu->dllPath = NULL;
	fmu->modelIdentifier = NULL;


	/* Copy DLL path */
	fmu->dllPath = (char*)cb->calloc(sizeof(char), strlen(dllPath) + 1);
	if (fmu->dllPath == NULL) {
		jm_log_fatal(cb, FMI_CAPI_MODULE_NAME, "Could not allocate memory for the DLL path string.");
		fmi3_capi_destroy_dllfmu(fmu);
		return NULL;
	}
	strcpy((char*)fmu->dllPath, dllPath);

	/* Copy the modelIdentifier */
	fmu->modelIdentifier = (char*)cb->calloc(sizeof(char), strlen(modelIdentifier) + 1);
	if (fmu->modelIdentifier == NULL) {
		jm_log_fatal(cb, FMI_CAPI_MODULE_NAME, "Could not allocate memory for the modelIdentifier string.");
		fmi3_capi_destroy_dllfmu(fmu);
		return NULL;
	}
	strcpy((char*)fmu->modelIdentifier, modelIdentifier);

	jm_log_debug(cb, FMI_CAPI_MODULE_NAME, "Successfully initialized data structures for FMICAPI.");

	/* Everything was successful */
	return fmu;
}

jm_status_enu_t fmi3_capi_load_fcn(fmi3_capi_t* fmu, unsigned int capabilities[])
{
	assert(fmu);
	/* Load ME functions */
	if (fmu->standard == fmi3_fmu_kind_me) {
		return fmi3_capi_load_me_fcn(fmu, capabilities);
	/* Load CS functions */
	} else if (fmu->standard == fmi3_fmu_kind_cs) {
		return fmi3_capi_load_cs_fcn(fmu, capabilities);
	} else {
		jm_log_error(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Unexpected FMU kind in FMICAPI.");
		return jm_status_error;
	}
}

jm_status_enu_t fmi3_capi_load_dll(fmi3_capi_t* fmu)
{
	assert(fmu && fmu->dllPath);
	fmu->dllHandle = jm_portability_load_dll_handle(fmu->dllPath); /* Load the shared library */
	if (fmu->dllHandle == NULL) {
		jm_log_fatal(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Could not load the FMU binary: %s", jm_portability_get_last_dll_error());
		return jm_status_error;
	} else {
		jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Loaded FMU binary from %s", fmu->dllPath);
		return jm_status_success;
	}
}

void fmi3_capi_set_debug_mode(fmi3_capi_t* fmu, int mode) {
	if(fmu)
		fmu->debugMode = mode;
}

int fmi3_capi_get_debug_mode(fmi3_capi_t* fmu) {
	if(fmu) return fmu->debugMode;
	return 0;
}

fmi3_fmu_kind_enu_t fmi3_capi_get_fmu_kind(fmi3_capi_t* fmu) {
	if(fmu) return fmu->standard;
	return fmi3_fmu_kind_unknown;
}

jm_status_enu_t fmi3_capi_free_dll(fmi3_capi_t* fmu)
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
			jm_log(fmu->callbacks, FMI_CAPI_MODULE_NAME, jm_log_level_error, "Could not free the FMU binary: %s", jm_portability_get_last_dll_error());
			return jm_status_error;
		} else {
			jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Successfully unloaded FMU binary");
			return jm_status_success;
		}
	}
	return jm_status_success;
}

/* Common FMI 3.0 functions */

const char* fmi3_capi_get_version(fmi3_capi_t* fmu)
{
	assert(fmu);
	return fmu->fmi3GetVersion();
}

const char* fmi3_capi_get_types_platform(fmi3_capi_t* fmu)
{
	assert(fmu);
	jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3GetModelTypesPlatform");
	return fmu->fmi3GetTypesPlatform();
}

fmi3_status_t fmi3_capi_set_debug_logging(fmi3_capi_t* fmu, fmi3_boolean_t loggingOn, size_t nCategories, fmi3_string_t categories[])
{
	return fmu->fmi3SetDebugLogging(fmu->c, loggingOn, nCategories, categories);
}

fmi3_component_t fmi3_capi_instantiate(fmi3_capi_t* fmu,
  fmi3_string_t instanceName, fmi3_type_t fmuType, fmi3_string_t fmuGUID,
  fmi3_string_t fmuResourceLocation, fmi3_boolean_t visible,
  fmi3_boolean_t loggingOn)
{
    return fmu->c = fmu->fmi3Instantiate(instanceName, fmuType, fmuGUID,
        fmuResourceLocation, &fmu->callBackFunctions, visible, loggingOn);
}

void fmi3_capi_free_instance(fmi3_capi_t* fmu)
{
    if(fmu != NULL && fmu->c != NULL) {
        fmu->fmi3FreeInstance(fmu->c);
        fmu->c = 0;
    }
}


fmi3_status_t fmi3_capi_setup_experiment(fmi3_capi_t* fmu,
    fmi3_boolean_t tolerance_defined, fmi3_real_t tolerance,
    fmi3_real_t start_time, fmi3_boolean_t stop_time_defined,
    fmi3_real_t stop_time)
{
    assert(fmu); assert(fmu->c);
    jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3SetupExperiment");
    return fmu->fmi3SetupExperiment(fmu->c, tolerance_defined, tolerance,
                                   start_time, stop_time_defined, stop_time);
}

fmi3_status_t fmi3_capi_enter_initialization_mode(fmi3_capi_t* fmu)
{
    assert(fmu); assert(fmu->c);
    jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3EnterInitializationMode");
    return fmu->fmi3EnterInitializationMode(fmu->c);
}

fmi3_status_t fmi3_capi_exit_initialization_mode(fmi3_capi_t* fmu)
{
    assert(fmu); assert(fmu->c);
    jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3ExitInitializationMode");
    return fmu->fmi3ExitInitializationMode(fmu->c);
}


fmi3_status_t fmi3_capi_terminate(fmi3_capi_t* fmu)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi3Terminate");
	return fmu->fmi3Terminate(fmu->c);
}

fmi3_status_t fmi3_capi_reset(fmi3_capi_t* fmu)
{
	return fmu->fmi3Reset(fmu->c);
}

fmi3_status_t fmi3_capi_get_fmu_state           (fmi3_capi_t* fmu, fmi3_FMU_state_t* s) {
	return fmu->fmi3GetFMUstate(fmu -> c,s);
}
fmi3_status_t fmi3_capi_set_fmu_state           (fmi3_capi_t* fmu, fmi3_FMU_state_t s){
	return fmu->fmi3SetFMUstate(fmu -> c,s);
}
fmi3_status_t fmi3_capi_free_fmu_state          (fmi3_capi_t* fmu, fmi3_FMU_state_t* s){
	return fmu->fmi3FreeFMUstate (fmu -> c,s);
}
fmi3_status_t fmi3_capi_serialized_fmu_state_size(fmi3_capi_t* fmu, fmi3_FMU_state_t s, size_t* sz){
	return fmu->fmi3SerializedFMUstateSize(fmu -> c,s,sz);
}
fmi3_status_t fmi3_capi_serialize_fmu_state     (fmi3_capi_t* fmu, fmi3_FMU_state_t s , fmi3_byte_t data[], size_t sz){
	return fmu->fmi3SerializeFMUstate(fmu -> c,s,data,sz);
}
fmi3_status_t fmi3_capi_de_serialize_fmu_state  (fmi3_capi_t* fmu, const fmi3_byte_t data[], size_t sz, fmi3_FMU_state_t* s){
	return fmu->fmi3DeSerializeFMUstate (fmu -> c,data,sz,s);
}

fmi3_status_t fmi3_capi_get_directional_derivative(fmi3_capi_t* fmu, const fmi3_value_reference_t v_ref[], size_t nv,
                                                                   const fmi3_value_reference_t z_ref[], size_t nz,
                                                                   const fmi3_real_t dv[], fmi3_real_t dz[]){
	return fmu->fmi3GetDirectionalDerivative(fmu -> c, z_ref, nz, v_ref, nv, dv, dz);
}


/* fmiSet* functions */
#define FMISETX(FNAME1, FNAME2, FTYPE) \
fmi3_status_t FNAME1(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, const FTYPE value[])	\
{ \
	return fmu->FNAME2(fmu->c, vr, nvr, value); \
}

/* fmiGet* functions */
#define FMIGETX(FNAME1, FNAME2, FTYPE) \
fmi3_status_t FNAME1(fmi3_capi_t* fmu, const fmi3_value_reference_t vr[], size_t nvr, FTYPE value[]) \
{ \
	return fmu->FNAME2(fmu->c, vr, nvr, value); \
}

FMISETX(fmi3_capi_set_real,		fmi3SetReal,		fmi3_real_t)
FMISETX(fmi3_capi_set_integer,	fmi3SetInteger,	fmi3_integer_t)
FMISETX(fmi3_capi_set_boolean,	fmi3SetBoolean,	fmi3_boolean_t)
FMISETX(fmi3_capi_set_string,	fmi3SetString,	fmi3_string_t)

FMIGETX(fmi3_capi_get_real,	fmi3GetReal,		fmi3_real_t)
FMIGETX(fmi3_capi_get_integer,	fmi3GetInteger,	fmi3_integer_t)
FMIGETX(fmi3_capi_get_boolean,	fmi3GetBoolean,	fmi3_boolean_t)
FMIGETX(fmi3_capi_get_string,	fmi3GetString,	fmi3_string_t)
