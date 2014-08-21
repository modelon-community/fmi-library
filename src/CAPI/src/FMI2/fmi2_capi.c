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

#include <FMI2/fmi2_capi_impl.h>

#define FUNCTION_NAME_LENGTH_MAX 2048			/* Maximum length of FMI function name. Used in the load DLL function. */
#define STRINGIFY(str) #str

/* Loading shared library functions */
static jm_status_enu_t fmi2_capi_get_fcn(fmi2_capi_t* fmu, const char* function_name, jm_dll_function_ptr* dll_function_ptrptr, jm_status_enu_t* status )
{
		jm_status_enu_t jm_status = jm_portability_load_dll_function(fmu->dllHandle, (char*)function_name, dll_function_ptrptr);
		if (jm_status == jm_status_error) {
			jm_log_error(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Could not load the FMI function '%s'. %s", function_name, jm_portability_get_last_dll_error()); 
			*status = jm_status_error;
		}
		return jm_status;
}

static void fmi2_capi_get_fcn_with_flag(fmi2_capi_t* fmu, const char* function_name, 
													jm_dll_function_ptr* dll_function_ptrptr, 
													unsigned int capabilities[],
													fmi2_capabilities_enu_t flag) {
	jm_status_enu_t status = jm_status_success;
	if(capabilities[flag]) {
		fmi2_capi_get_fcn(fmu, function_name, dll_function_ptrptr, &status);
		if(status != jm_status_success) {
			jm_log_warning(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Resetting flag '%s'", fmi2_capability_to_string(flag)); 
			capabilities[flag] = 0;
		}
	}
}

/* Load FMI functions from DLL macro */
#define LOAD_DLL_FUNCTION(FMIFUNCTION) fmi2_capi_get_fcn(fmu, #FMIFUNCTION, (jm_dll_function_ptr*)&fmu->FMIFUNCTION, &jm_status)

/* Load FMI functions from DLL macro for functions controlled by capability flags */
#define LOAD_DLL_FUNCTION_WITH_FLAG(FMIFUNCTION, FLAG) \
	fmi2_capi_get_fcn_with_flag(fmu, #FMIFUNCTION, (jm_dll_function_ptr*)&fmu->FMIFUNCTION, capabilities, FLAG)

static jm_status_enu_t fmi2_capi_load_common_fcn(fmi2_capi_t* fmu, unsigned int capabilities[])
{
	jm_status_enu_t jm_status = jm_status_success;
	/***************************************************
Types for Common Functions
****************************************************/

/* Inquire version numbers of header files and setting logging status */
/*   typedef const char* fmi2GetTypesPlatformTYPE(void);
   typedef const char* fmi2GetVersionTYPE(void);
   typedef fmi2Status  fmi2SetDebugLoggingTYPE(fmi2Component, fmi2Boolean, size_t, const fmi2String[]); */

	LOAD_DLL_FUNCTION(fmi2GetTypesPlatform);
	LOAD_DLL_FUNCTION(fmi2GetVersion);
	LOAD_DLL_FUNCTION(fmi2SetDebugLogging);

/* Enter and exit initialization mode, terminate and reset */
/* typedef fmi2Status fmi2TerminateTYPE              (fmi2Component);
   typedef fmi2Status fmi2ResetTYPE                  (fmi2Component); */
	LOAD_DLL_FUNCTION(fmi2Terminate);
	LOAD_DLL_FUNCTION(fmi2Reset);

    /* Creation and destruction of instances and setting debug status */
    /*typedef fmi2Component fmi2InstantiateTYPE (fmi2String, fmi2Type, fmi2String, fmi2String, const fmi2CallbackFunctions*, fmi2Boolean, fmi2Boolean);
    typedef void          fmi2FreeInstanceTYPE(fmi2Component);*/
    LOAD_DLL_FUNCTION(fmi2Instantiate);
    LOAD_DLL_FUNCTION(fmi2FreeInstance);

   /* typedef fmi2Status fmi2SetupExperimentTYPE        (fmi2Component, fmi2Boolean, fmi2Real, fmi2Real, fmi2Boolean, fmi2Real);
   typedef fmi2Status fmi2EnterInitializationModeTYPE(fmi2Component);
   typedef fmi2Status fmi2ExitInitializationModeTYPE (fmi2Component); */
    LOAD_DLL_FUNCTION(fmi2SetupExperiment);
    LOAD_DLL_FUNCTION(fmi2EnterInitializationMode);
    LOAD_DLL_FUNCTION(fmi2ExitInitializationMode);

	/* Getting and setting variable values */
/*   typedef fmi2Status fmi2GetRealTYPE   (fmi2Component, const fmi2ValueReference[], size_t, fmi2Real   []);
   typedef fmi2Status fmi2GetIntegerTYPE(fmi2Component, const fmi2ValueReference[], size_t, fmi2Integer[]);
   typedef fmi2Status fmi2GetBooleanTYPE(fmi2Component, const fmi2ValueReference[], size_t, fmi2Boolean[]);
   typedef fmi2Status fmi2GetStringTYPE (fmi2Component, const fmi2ValueReference[], size_t, fmi2String []); */

	LOAD_DLL_FUNCTION(fmi2GetReal);
	LOAD_DLL_FUNCTION(fmi2GetInteger);
	LOAD_DLL_FUNCTION(fmi2GetBoolean);
	LOAD_DLL_FUNCTION(fmi2GetString);

	/*   typedef fmi2Status fmi2SetRealTYPE   (fmi2Component, const fmi2ValueReference[], size_t, const fmi2Real   []);
   typedef fmi2Status fmi2SetIntegerTYPE(fmi2Component, const fmi2ValueReference[], size_t, const fmi2Integer[]);
   typedef fmi2Status fmi2SetBooleanTYPE(fmi2Component, const fmi2ValueReference[], size_t, const fmi2Boolean[]);
   typedef fmi2Status fmi2SetStringTYPE (fmi2Component, const fmi2ValueReference[], size_t, const fmi2String []); */

	LOAD_DLL_FUNCTION(fmi2SetReal);
	LOAD_DLL_FUNCTION(fmi2SetInteger);
	LOAD_DLL_FUNCTION(fmi2SetBoolean);
	LOAD_DLL_FUNCTION(fmi2SetString);

	return jm_status;
}

/* Load FMI 2.0 Co-Simulation functions */
static jm_status_enu_t fmi2_capi_load_cs_fcn(fmi2_capi_t* fmu, unsigned int capabilities[])
{
	jm_status_enu_t jm_status = jm_status_success;

	jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Loading functions for the co-simulation interface"); 

	jm_status = fmi2_capi_load_common_fcn(fmu, capabilities);

	/* Getting and setting the internal FMU state */
/*   typedef fmi2Status fmi2GetFMUstateTYPE           (fmi2Component, fmi2FMUstate*);
   typedef fmi2Status fmi2SetFMUstateTYPE           (fmi2Component, fmi2FMUstate);
   typedef fmi2Status fmi2FreeFMUstateTYPE          (fmi2Component, fmi2FMUstate*);
   typedef fmi2Status fmi2SerializedFMUstateSizeTYPE(fmi2Component, fmi2FMUstate, size_t*);
   typedef fmi2Status fmi2SerializeFMUstateTYPE     (fmi2Component, fmi2FMUstate, fmi2Byte[], size_t);
   typedef fmi2Status fmi2DeSerializeFMUstateTYPE   (fmi2Component, const fmi2Byte[], size_t, fmi2FMUstate*); */
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi2GetFMUstate,fmi2_cs_canGetAndSetFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi2SetFMUstate,fmi2_cs_canGetAndSetFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi2FreeFMUstate,fmi2_cs_canGetAndSetFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi2SerializedFMUstateSize,fmi2_cs_canSerializeFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi2SerializeFMUstate,fmi2_cs_canSerializeFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi2DeSerializeFMUstate,fmi2_cs_canSerializeFMUstate);

/* Getting directional derivatives */
/*   typedef fmi2Status fmi2GetDirectionalDerivativeTYPE(fmi2Component, const fmi2ValueReference[], size_t,
                                                                   const fmi2ValueReference[], size_t,
                                                                   const fmi2Real[], fmi2Real[]);*/
    LOAD_DLL_FUNCTION_WITH_FLAG(fmi2GetDirectionalDerivative,fmi2_cs_providesDirectionalDerivatives); 

/* Simulating the slave */
/*   typedef fmi2Status fmi2SetRealInputDerivativesTYPE (fmi2Component, const fmi2ValueReference [], size_t, const fmi2Integer [], const fmi2Real []);
   typedef fmi2Status fmi2GetRealOutputDerivativesTYPE(fmi2Component, const fmi2ValueReference [], size_t, const fmi2Integer [], fmi2Real []); */
	LOAD_DLL_FUNCTION(fmi2SetRealInputDerivatives);
	LOAD_DLL_FUNCTION(fmi2GetRealOutputDerivatives);

/*   typedef fmi2Status fmi2DoStepTYPE     (fmi2Component, fmi2Real, fmi2Real, fmi2Boolean);
   typedef fmi2Status fmi2CancelStepTYPE (fmi2Component); */
	LOAD_DLL_FUNCTION(fmi2CancelStep);
	LOAD_DLL_FUNCTION(fmi2DoStep);

/* Inquire slave status */
/*   typedef fmi2Status fmi2GetStatusTYPE       (fmi2Component, const fmi2StatusKind, fmi2Status* );
   typedef fmi2Status fmi2GetRealStatusTYPE   (fmi2Component, const fmi2StatusKind, fmi2Real*   );
   typedef fmi2Status fmi2GetIntegerStatusTYPE(fmi2Component, const fmi2StatusKind, fmi2Integer*);
   typedef fmi2Status fmi2GetBooleanStatusTYPE(fmi2Component, const fmi2StatusKind, fmi2Boolean*);
   typedef fmi2Status fmi2GetStringStatusTYPE (fmi2Component, const fmi2StatusKind, fmi2String* ); */
	LOAD_DLL_FUNCTION(fmi2GetStatus);
	LOAD_DLL_FUNCTION(fmi2GetRealStatus);
	LOAD_DLL_FUNCTION(fmi2GetIntegerStatus);
	LOAD_DLL_FUNCTION(fmi2GetBooleanStatus);
	LOAD_DLL_FUNCTION(fmi2GetStringStatus);

	return jm_status; 
}

/* Load FMI 2.0 Model Exchange functions */
static jm_status_enu_t fmi2_capi_load_me_fcn(fmi2_capi_t* fmu, unsigned int capabilities[])
{
	jm_status_enu_t jm_status = jm_status_success;

	jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Loading functions for the model exchange interface"); 

	jm_status = fmi2_capi_load_common_fcn(fmu, capabilities);
		/* Getting and setting the internal FMU state */
/*   typedef fmi2Status fmi2GetFMUstateTYPE           (fmi2Component, fmi2FMUstate*);
   typedef fmi2Status fmi2SetFMUstateTYPE           (fmi2Component, fmi2FMUstate);
   typedef fmi2Status fmi2FreeFMUstateTYPE          (fmi2Component, fmi2FMUstate*);
   typedef fmi2Status fmi2SerializedFMUstateSizeTYPE(fmi2Component, fmi2FMUstate, size_t*);
   typedef fmi2Status fmi2SerializeFMUstateTYPE     (fmi2Component, fmi2FMUstate, fmi2Byte[], size_t);
   typedef fmi2Status fmi2DeSerializeFMUstateTYPE   (fmi2Component, const fmi2Byte[], size_t, fmi2FMUstate*); */
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi2GetFMUstate,fmi2_me_canGetAndSetFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi2SetFMUstate,fmi2_me_canGetAndSetFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi2FreeFMUstate,fmi2_me_canGetAndSetFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi2SerializedFMUstateSize,fmi2_me_canSerializeFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi2SerializeFMUstate,fmi2_me_canSerializeFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmi2DeSerializeFMUstate,fmi2_me_canSerializeFMUstate);

/* Getting directional derivatives */
/*   typedef fmi2Status fmi2GetDirectionalDerivativeTYPE(fmi2Component, const fmi2ValueReference[], size_t,
                                                                   const fmi2ValueReference[], size_t,
                                                                   const fmi2Real[], fmi2Real[]); */
    LOAD_DLL_FUNCTION_WITH_FLAG(fmi2GetDirectionalDerivative,fmi2_me_providesDirectionalDerivatives); 

/* Enter and exit the different modes */
/*   typedef fmi2Status fmi2EnterEventModeTYPE         (fmi2Component);
   typedef fmi2Status fmi2NewDiscreteStatesTYPE      (fmi2Component, fmi2EventInfo*);
   typedef fmi2Status fmi2EnterContinuousTimeModeTYPE(fmi2Component);
   typedef fmi2Status fmi2CompletedIntegratorStepTYPE(fmi2Component, fmi2Boolean, fmi2Boolean*, fmi2Boolean*);*/
	LOAD_DLL_FUNCTION(fmi2EnterEventMode);
	LOAD_DLL_FUNCTION(fmi2NewDiscreteStates);
	LOAD_DLL_FUNCTION(fmi2EnterContinuousTimeMode);
	LOAD_DLL_FUNCTION(fmi2CompletedIntegratorStep);

/* Providing independent variables and re-initialization of caching */
   /*typedef fmi2Status fmi2SetTimeTYPE            (fmi2Component, fmi2Real);
   typedef fmi2Status fmi2SetContinuousStatesTYPE(fmi2Component, const fmi2Real[], size_t);*/
	LOAD_DLL_FUNCTION(fmi2SetTime);
	LOAD_DLL_FUNCTION(fmi2SetContinuousStates);

/* Evaluation of the model equations */

/*
   typedef fmi2Status fmi2GetDerivativesTYPE               (fmi2Component, fmi2Real[], size_t);
   typedef fmi2Status fmi2GetEventIndicatorsTYPE           (fmi2Component, fmi2Real[], size_t);
   typedef fmi2Status fmi2GetContinuousStatesTYPE          (fmi2Component, fmi2Real[], size_t);
   typedef fmi2Status fmi2GetNominalsOfContinuousStatesTYPE(fmi2Component, fmi2Real[], size_t);*/
	LOAD_DLL_FUNCTION(fmi2GetDerivatives);
	LOAD_DLL_FUNCTION(fmi2GetEventIndicators);
	LOAD_DLL_FUNCTION(fmi2GetContinuousStates);
	LOAD_DLL_FUNCTION(fmi2GetNominalsOfContinuousStates);
   
	return jm_status; 
}

void fmi2_capi_destroy_dllfmu(fmi2_capi_t* fmu)
{
	if (fmu == NULL) {
		return;
	}
	fmi2_capi_free_dll(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Releasing allocated memory");
	fmu->callbacks->free((void*)fmu->dllPath);
	fmu->callbacks->free((void*)fmu->modelIdentifier);
	fmu->callbacks->free((void*)fmu);
}

fmi2_capi_t* fmi2_capi_create_dllfmu(jm_callbacks* cb, const char* dllPath, const char* modelIdentifier, const fmi2_callback_functions_t* callBackFunctions, fmi2_fmu_kind_enu_t standard)
{
	fmi2_capi_t* fmu = NULL;

	jm_log_debug(cb, FMI_CAPI_MODULE_NAME, "Initializing data structures for FMICAPI.");

	/* Minor check for the callbacks */
	if (cb == NULL) {
		assert(0);
		return NULL;
	}

	/* Allocate memory for the FMU instance */
	fmu = (fmi2_capi_t*)cb->calloc(1, sizeof(fmi2_capi_t));
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
		fmi2_capi_destroy_dllfmu(fmu);
		return NULL;
	}
	strcpy((char*)fmu->dllPath, dllPath);

	/* Copy the modelIdentifier */
	fmu->modelIdentifier = (char*)cb->calloc(sizeof(char), strlen(modelIdentifier) + 1);
	if (fmu->modelIdentifier == NULL) {
		jm_log_fatal(cb, FMI_CAPI_MODULE_NAME, "Could not allocate memory for the modelIdentifier string.");
		fmi2_capi_destroy_dllfmu(fmu);
		return NULL;
	}
	strcpy((char*)fmu->modelIdentifier, modelIdentifier);

	jm_log_debug(cb, FMI_CAPI_MODULE_NAME, "Successfully initialized data structures for FMICAPI.");

	/* Everything was successful */
	return fmu;
}

jm_status_enu_t fmi2_capi_load_fcn(fmi2_capi_t* fmu, unsigned int capabilities[])
{
	assert(fmu);
	/* Load ME functions */
	if (fmu->standard == fmi2_fmu_kind_me) {
		return fmi2_capi_load_me_fcn(fmu, capabilities);
	/* Load CS functions */
	} else if (fmu->standard == fmi2_fmu_kind_cs) {
		return fmi2_capi_load_cs_fcn(fmu, capabilities);
	} else {
		jm_log_error(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Unexpected FMU kind in FMICAPI.");
		return jm_status_error;
	}
}

jm_status_enu_t fmi2_capi_load_dll(fmi2_capi_t* fmu)
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

void fmi2_capi_set_debug_mode(fmi2_capi_t* fmu, int mode) {
	if(fmu)
		fmu->debugMode = mode;
}

int fmi2_capi_get_debug_mode(fmi2_capi_t* fmu) {
	if(fmu) return fmu->debugMode;
	return 0;
}

fmi2_fmu_kind_enu_t fmi2_capi_get_fmu_kind(fmi2_capi_t* fmu) {
	if(fmu) return fmu->standard;
	return fmi2_fmu_kind_unknown;
}

jm_status_enu_t fmi2_capi_free_dll(fmi2_capi_t* fmu)
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

/* Common FMI 2.0 functions */

const char* fmi2_capi_get_version(fmi2_capi_t* fmu)
{
	assert(fmu);
	return fmu->fmi2GetVersion();
}

const char* fmi2_capi_get_types_platform(fmi2_capi_t* fmu)
{
	assert(fmu);
	jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi2GetModelTypesPlatform");
	return fmu->fmi2GetTypesPlatform();
}

fmi2_status_t fmi2_capi_set_debug_logging(fmi2_capi_t* fmu, fmi2_boolean_t loggingOn, size_t nCategories, fmi2_string_t categories[])
{
	return fmu->fmi2SetDebugLogging(fmu->c, loggingOn, nCategories, categories);
}

fmi2_component_t fmi2_capi_instantiate(fmi2_capi_t* fmu,
  fmi2_string_t instanceName, fmi2_type_t fmuType, fmi2_string_t fmuGUID,
  fmi2_string_t fmuResourceLocation, fmi2_boolean_t visible,
  fmi2_boolean_t loggingOn)
{
    return fmu->c = fmu->fmi2Instantiate(instanceName, fmuType, fmuGUID,
        fmuResourceLocation, &fmu->callBackFunctions, visible, loggingOn);
}

void fmi2_capi_free_instance(fmi2_capi_t* fmu)
{
    if(fmu->c) {
        fmu->fmi2FreeInstance(fmu->c);
        fmu->c = 0;
    }
}


fmi2_status_t fmi2_capi_setup_experiment(fmi2_capi_t* fmu,
    fmi2_boolean_t tolerance_defined, fmi2_real_t tolerance,
    fmi2_real_t start_time, fmi2_boolean_t stop_time_defined,
    fmi2_real_t stop_time)
{
    assert(fmu); assert(fmu->c);
    jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi2SetupExperiment");
    return fmu->fmi2SetupExperiment(fmu->c, tolerance_defined, tolerance,
                                   start_time, stop_time_defined, stop_time);
}

fmi2_status_t fmi2_capi_enter_initialization_mode(fmi2_capi_t* fmu)
{
    assert(fmu); assert(fmu->c);
    jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi2EnterInitializationMode");
    return fmu->fmi2EnterInitializationMode(fmu->c);
}

fmi2_status_t fmi2_capi_exit_initialization_mode(fmi2_capi_t* fmu)
{
    assert(fmu); assert(fmu->c);
    jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi2ExitInitializationMode");
    return fmu->fmi2ExitInitializationMode(fmu->c);
}


fmi2_status_t fmi2_capi_terminate(fmi2_capi_t* fmu)
{
	assert(fmu);
	jm_log_debug(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmi2Terminate");
	return fmu->fmi2Terminate(fmu->c);
}

fmi2_status_t fmi2_capi_reset(fmi2_capi_t* fmu)
{
	return fmu->fmi2Reset(fmu->c);
}

fmi2_status_t fmi2_capi_get_fmu_state           (fmi2_capi_t* fmu, fmi2_FMU_state_t* s) {
	return fmu->fmi2GetFMUstate(fmu -> c,s);
}
fmi2_status_t fmi2_capi_set_fmu_state           (fmi2_capi_t* fmu, fmi2_FMU_state_t s){
	return fmu->fmi2SetFMUstate(fmu -> c,s);
}
fmi2_status_t fmi2_capi_free_fmu_state          (fmi2_capi_t* fmu, fmi2_FMU_state_t* s){
	return fmu->fmi2FreeFMUstate (fmu -> c,s);
}
fmi2_status_t fmi2_capi_serialized_fmu_state_size(fmi2_capi_t* fmu, fmi2_FMU_state_t s, size_t* sz){
	return fmu->fmi2SerializedFMUstateSize(fmu -> c,s,sz);
}
fmi2_status_t fmi2_capi_serialize_fmu_state     (fmi2_capi_t* fmu, fmi2_FMU_state_t s , fmi2_byte_t data[], size_t sz){
	return fmu->fmi2SerializeFMUstate(fmu -> c,s,data,sz);
}
fmi2_status_t fmi2_capi_de_serialize_fmu_state  (fmi2_capi_t* fmu, const fmi2_byte_t data[], size_t sz, fmi2_FMU_state_t* s){
	return fmu->fmi2DeSerializeFMUstate (fmu -> c,data,sz,s);
}

fmi2_status_t fmi2_capi_get_directional_derivative(fmi2_capi_t* fmu, const fmi2_value_reference_t v_ref[], size_t nv,
                                                                   const fmi2_value_reference_t z_ref[], size_t nz,
                                                                   const fmi2_real_t dv[], fmi2_real_t dz[]){
	return fmu->fmi2GetDirectionalDerivative(fmu -> c, z_ref, nz, v_ref, nv, dv, dz);
}


/* fmiSet* functions */
#define FMISETX(FNAME1, FNAME2, FTYPE) \
fmi2_status_t FNAME1(fmi2_capi_t* fmu, const fmi2_value_reference_t vr[], size_t nvr, const FTYPE value[])	\
{ \
	return fmu->FNAME2(fmu->c, vr, nvr, value); \
}

/* fmiGet* functions */
#define FMIGETX(FNAME1, FNAME2, FTYPE) \
fmi2_status_t FNAME1(fmi2_capi_t* fmu, const fmi2_value_reference_t vr[], size_t nvr, FTYPE value[]) \
{ \
	return fmu->FNAME2(fmu->c, vr, nvr, value); \
}

FMISETX(fmi2_capi_set_real,		fmi2SetReal,		fmi2_real_t)
FMISETX(fmi2_capi_set_integer,	fmi2SetInteger,	fmi2_integer_t)
FMISETX(fmi2_capi_set_boolean,	fmi2SetBoolean,	fmi2_boolean_t)
FMISETX(fmi2_capi_set_string,	fmi2SetString,	fmi2_string_t)

FMIGETX(fmi2_capi_get_real,	fmi2GetReal,		fmi2_real_t)
FMIGETX(fmi2_capi_get_integer,	fmi2GetInteger,	fmi2_integer_t)
FMIGETX(fmi2_capi_get_boolean,	fmi2GetBoolean,	fmi2_boolean_t)
FMIGETX(fmi2_capi_get_string,	fmi2GetString,	fmi2_string_t)
