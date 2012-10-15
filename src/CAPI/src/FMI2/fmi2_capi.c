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
			jm_log_warning(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Reseting flag '%s'", fmi2_capability_to_string(flag)); 
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
/*   typedef const char* fmiGetTypesPlatformTYPE();
   typedef const char* fmiGetVersionTYPE();
   typedef fmiStatus   fmiSetDebugLoggingTYPE(fmiComponent, fmiBoolean, size_t, const fmiString []); */

	LOAD_DLL_FUNCTION(fmiGetTypesPlatform);
	LOAD_DLL_FUNCTION(fmiGetVersion);
	LOAD_DLL_FUNCTION(fmiSetDebugLogging);

	/* Getting and setting variable values */
/*   typedef fmiStatus fmiGetRealTYPE   (fmiComponent, const fmiValueReference[], size_t, fmiReal   []);
   typedef fmiStatus fmiGetIntegerTYPE(fmiComponent, const fmiValueReference[], size_t, fmiInteger[]);
   typedef fmiStatus fmiGetBooleanTYPE(fmiComponent, const fmiValueReference[], size_t, fmiBoolean[]);
   typedef fmiStatus fmiGetStringTYPE (fmiComponent, const fmiValueReference[], size_t, fmiString []); */

	LOAD_DLL_FUNCTION(fmiGetReal);
	LOAD_DLL_FUNCTION(fmiGetInteger);
	LOAD_DLL_FUNCTION(fmiGetBoolean);
	LOAD_DLL_FUNCTION(fmiGetString);

	/*   typedef fmiStatus fmiSetRealTYPE   (fmiComponent, const fmiValueReference[], size_t, const fmiReal   []);
   typedef fmiStatus fmiSetIntegerTYPE(fmiComponent, const fmiValueReference[], size_t, const fmiInteger[]);
   typedef fmiStatus fmiSetBooleanTYPE(fmiComponent, const fmiValueReference[], size_t, const fmiBoolean[]);
   typedef fmiStatus fmiSetStringTYPE (fmiComponent, const fmiValueReference[], size_t, const fmiString []); */

	LOAD_DLL_FUNCTION(fmiSetReal);
	LOAD_DLL_FUNCTION(fmiSetInteger);
	LOAD_DLL_FUNCTION(fmiSetBoolean);
	LOAD_DLL_FUNCTION(fmiSetString);

	return jm_status;
}

/* Load FMI 2.0 Co-Simulation functions */
static jm_status_enu_t fmi2_capi_load_cs_fcn(fmi2_capi_t* fmu, unsigned int capabilities[])
{
	jm_status_enu_t jm_status = jm_status_success;

	jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Loading functions for the co-simulation interface"); 

	jm_status = fmi2_capi_load_common_fcn(fmu, capabilities);

	/* Getting and setting the internal FMU state */
/*   typedef fmiStatus fmiGetFMUstateTYPE           (fmiComponent, fmiFMUstate*);
   typedef fmiStatus fmiSetFMUstateTYPE           (fmiComponent, fmiFMUstate);
   typedef fmiStatus fmiFreeFMUstateTYPE          (fmiComponent, fmiFMUstate*);
   typedef fmiStatus fmiSerializedFMUstateSizeTYPE(fmiComponent, fmiFMUstate, size_t*);
   typedef fmiStatus fmiSerializeFMUstateTYPE     (fmiComponent, fmiFMUstate, fmiByte[], size_t);
   typedef fmiStatus fmiDeSerializeFMUstateTYPE   (fmiComponent, const fmiByte[], size_t, fmiFMUstate*); */
   LOAD_DLL_FUNCTION_WITH_FLAG(fmiGetFMUstate,fmi2_cs_canGetAndSetFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmiSetFMUstate,fmi2_cs_canGetAndSetFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmiFreeFMUstate,fmi2_cs_canGetAndSetFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmiSerializedFMUstateSize,fmi2_cs_canSerializeFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmiSerializeFMUstate,fmi2_cs_canSerializeFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmiDeSerializeFMUstate,fmi2_cs_canSerializeFMUstate);

/* Getting partial derivatives */
/*   typedef fmiStatus fmiGetPartialDerivativesTYPE   (fmiComponent, setMatrixElement, void*, void*, void*, void*);
   typedef fmiStatus fmiGetDirectionalDerivativeTYPE(fmiComponent, const fmiValueReference[], size_t,
                                                                   const fmiValueReference[], size_t,
                                                                   const fmiReal[], fmiReal[]); */
    /*???  LOAD_DLL_FUNCTION_WITH_FLAG(fmiGetDirectionalDerivative,fmi2_cs_providesDirectionalDerivatives); */

/* Creation and destruction of slave instances */
/*   typedef fmiComponent fmiInstantiateSlaveTYPE (fmiString, fmiString, fmiString, const fmiCallbackFunctions*, fmiBoolean, fmiBoolean);
   typedef void         fmiFreeSlaveInstanceTYPE(fmiComponent); */
    LOAD_DLL_FUNCTION(fmiInstantiateSlave);
	LOAD_DLL_FUNCTION(fmiFreeSlaveInstance);

/* Simulating the slave */
/*   typedef fmiStatus fmiInitializeSlaveTYPE(fmiComponent, fmiReal, fmiReal, fmiBoolean, fmiReal);
   typedef fmiStatus fmiTerminateSlaveTYPE (fmiComponent);
   typedef fmiStatus fmiResetSlaveTYPE     (fmiComponent); */
	LOAD_DLL_FUNCTION(fmiInitializeSlave);
	LOAD_DLL_FUNCTION(fmiTerminateSlave);
	LOAD_DLL_FUNCTION(fmiResetSlave);

/*   typedef fmiStatus fmiSetRealInputDerivativesTYPE (fmiComponent, const fmiValueReference [], size_t, const fmiInteger [], const fmiReal []);
   typedef fmiStatus fmiGetRealOutputDerivativesTYPE(fmiComponent, const fmiValueReference [], size_t, const fmiInteger [], fmiReal []); */
	LOAD_DLL_FUNCTION(fmiSetRealInputDerivatives);
	LOAD_DLL_FUNCTION(fmiGetRealOutputDerivatives);

/*   typedef fmiStatus fmiDoStepTYPE     (fmiComponent, fmiReal, fmiReal, fmiBoolean);
   typedef fmiStatus fmiCancelStepTYPE (fmiComponent); */
	LOAD_DLL_FUNCTION(fmiCancelStep);
	LOAD_DLL_FUNCTION(fmiDoStep);

/* Inquire slave status */
/*   typedef fmiStatus fmiGetStatusTYPE       (fmiComponent, const fmiStatusKind, fmiStatus* );
   typedef fmiStatus fmiGetRealStatusTYPE   (fmiComponent, const fmiStatusKind, fmiReal*   );
   typedef fmiStatus fmiGetIntegerStatusTYPE(fmiComponent, const fmiStatusKind, fmiInteger*);
   typedef fmiStatus fmiGetBooleanStatusTYPE(fmiComponent, const fmiStatusKind, fmiBoolean*);
   typedef fmiStatus fmiGetStringStatusTYPE (fmiComponent, const fmiStatusKind, fmiString* ); */
	LOAD_DLL_FUNCTION(fmiGetStatus);
	LOAD_DLL_FUNCTION(fmiGetRealStatus);
	LOAD_DLL_FUNCTION(fmiGetIntegerStatus);
	LOAD_DLL_FUNCTION(fmiGetBooleanStatus);
	LOAD_DLL_FUNCTION(fmiGetStringStatus);

	return jm_status; 
}

/* Load FMI 1.0 Model Exchange functions */
static jm_status_enu_t fmi2_capi_load_me_fcn(fmi2_capi_t* fmu, unsigned int capabilities[])
{
	jm_status_enu_t jm_status = jm_status_success;

	jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Loading functions for the model exchange interface"); 

	jm_status = fmi2_capi_load_common_fcn(fmu, capabilities);
		/* Getting and setting the internal FMU state */
/*   typedef fmiStatus fmiGetFMUstateTYPE           (fmiComponent, fmiFMUstate*);
   typedef fmiStatus fmiSetFMUstateTYPE           (fmiComponent, fmiFMUstate);
   typedef fmiStatus fmiFreeFMUstateTYPE          (fmiComponent, fmiFMUstate*);
   typedef fmiStatus fmiSerializedFMUstateSizeTYPE(fmiComponent, fmiFMUstate, size_t*);
   typedef fmiStatus fmiSerializeFMUstateTYPE     (fmiComponent, fmiFMUstate, fmiByte[], size_t);
   typedef fmiStatus fmiDeSerializeFMUstateTYPE   (fmiComponent, const fmiByte[], size_t, fmiFMUstate*); */
   LOAD_DLL_FUNCTION_WITH_FLAG(fmiGetFMUstate,fmi2_me_canGetAndSetFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmiSetFMUstate,fmi2_me_canGetAndSetFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmiFreeFMUstate,fmi2_me_canGetAndSetFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmiSerializedFMUstateSize,fmi2_me_canSerializeFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmiSerializeFMUstate,fmi2_me_canSerializeFMUstate);
   LOAD_DLL_FUNCTION_WITH_FLAG(fmiDeSerializeFMUstate,fmi2_me_canSerializeFMUstate);

/* Getting partial derivatives */
/*   typedef fmiStatus fmiGetPartialDerivativesTYPE   (fmiComponent, setMatrixElement, void*, void*, void*, void*);
   typedef fmiStatus fmiGetDirectionalDerivativeTYPE(fmiComponent, const fmiValueReference[], size_t,
                                                                   const fmiValueReference[], size_t,
                                                                   const fmiReal[], fmiReal[]); */
    LOAD_DLL_FUNCTION_WITH_FLAG(fmiGetDirectionalDerivative,fmi2_me_providesDirectionalDerivatives); 

	/* Creation and destruction of model instances and setting debug status */
   /*typedef fmiComponent fmiInstantiateModelTYPE (fmiString, fmiString, fmiString, const fmiCallbackFunctions*, fmiBoolean, fmiBoolean);
    typedef void         fmiFreeModelInstanceTYPE(fmiComponent);*/
	LOAD_DLL_FUNCTION(fmiInstantiateModel);
	LOAD_DLL_FUNCTION(fmiFreeModelInstance);

/* Providing independent variables and re-initialization of caching */
   /*typedef fmiStatus fmiSetTimeTYPE                (fmiComponent, fmiReal);
   typedef fmiStatus fmiSetContinuousStatesTYPE    (fmiComponent, const fmiReal[], size_t);
   typedef fmiStatus fmiCompletedIntegratorStepTYPE(fmiComponent, fmiBoolean*); */
	LOAD_DLL_FUNCTION(fmiSetTime);
	LOAD_DLL_FUNCTION(fmiSetContinuousStates);
	LOAD_DLL_FUNCTION(fmiCompletedIntegratorStep);

/* Evaluation of the model equations */
/*   typedef fmiStatus fmiInitializeModelTYPE        (fmiComponent, fmiBoolean, fmiReal, fmiEventInfo*);
   typedef fmiStatus fmiEventUpdateTYPE            (fmiComponent, fmiBoolean, fmiEventInfo*);
   typedef fmiStatus fmiCompletedEventIterationTYPE(fmiComponent);
   typedef fmiStatus fmiTerminateTYPE              (fmiComponent); */
	LOAD_DLL_FUNCTION(fmiInitializeModel);
	LOAD_DLL_FUNCTION(fmiEventUpdate);
	LOAD_DLL_FUNCTION_WITH_FLAG(fmiCompletedEventIteration, fmi2_me_completedEventIterationIsProvided);
	LOAD_DLL_FUNCTION(fmiTerminate);

/*
   typedef fmiStatus fmiGetDerivativesTYPE            (fmiComponent, fmiReal[], size_t);
   typedef fmiStatus fmiGetEventIndicatorsTYPE        (fmiComponent, fmiReal[], size_t);
   typedef fmiStatus fmiGetContinuousStatesTYPE       (fmiComponent, fmiReal[], size_t);
   typedef fmiStatus fmiGetNominalContinuousStatesTYPE(fmiComponent, fmiReal[], size_t);
   typedef fmiStatus fmiGetStateValueReferencesTYPE   (fmiComponent, fmiValueReference[], size_t); */
	LOAD_DLL_FUNCTION(fmiGetDerivatives);
	LOAD_DLL_FUNCTION(fmiGetEventIndicators);
	LOAD_DLL_FUNCTION(fmiGetContinuousStates);
	LOAD_DLL_FUNCTION(fmiGetNominalContinuousStates);
	LOAD_DLL_FUNCTION(fmiGetStateValueReferences);

   
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

	jm_log_debug(cb, FMI_CAPI_MODULE_NAME, "Initializing data stuctures for FMICAPI.");

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

	jm_log_debug(cb, FMI_CAPI_MODULE_NAME, "Successfully initialized data stuctures for FMICAPI.");

	/* Everything was succesfull */
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

/* Common FMI 1.0 functions */

const char* fmi2_capi_get_version(fmi2_capi_t* fmu)
{
	assert(fmu);
	return fmu->fmiGetVersion();
}

const char* fmi2_capi_get_types_platform(fmi2_capi_t* fmu)
{
	assert(fmu);
	jm_log_verbose(fmu->callbacks, FMI_CAPI_MODULE_NAME, "Calling fmiGetModelTypesPlatform");
	return fmu->fmiGetTypesPlatform();
}

fmi2_status_t fmi2_capi_set_debug_logging(fmi2_capi_t* fmu, fmi2_boolean_t loggingOn, size_t nCategories, fmi2_string_t categories[])
{
	return fmu->fmiSetDebugLogging(fmu->c, loggingOn, nCategories, categories);
}

fmi2_status_t fmi2_capi_get_fmu_state           (fmi2_capi_t* fmu, fmi2_FMU_state_t* s) {
	return fmu->fmiGetFMUstate(fmu -> c,s);
}
fmi2_status_t fmi2_capi_set_fmu_state           (fmi2_capi_t* fmu, fmi2_FMU_state_t s){
	return fmu->fmiSetFMUstate(fmu -> c,s);
}
fmi2_status_t fmi2_capi_free_fmu_state          (fmi2_capi_t* fmu, fmi2_FMU_state_t* s){
	return fmu->fmiFreeFMUstate (fmu -> c,s);
}
fmi2_status_t fmi2_capi_serialized_fmu_state_size(fmi2_capi_t* fmu, fmi2_FMU_state_t s, size_t* sz){
	return fmu->fmiSerializedFMUstateSize(fmu -> c,s,sz);
}
fmi2_status_t fmi2_capi_serialize_fmu_state     (fmi2_capi_t* fmu, fmi2_FMU_state_t s , fmi2_byte_t data[], size_t sz){
	return fmu->fmiSerializeFMUstate(fmu -> c,s,data,sz);
}
fmi2_status_t fmi2_capi_de_serialize_fmu_state  (fmi2_capi_t* fmu, const fmi2_byte_t data[], size_t sz, fmi2_FMU_state_t* s){
	return fmu->fmiDeSerializeFMUstate (fmu -> c,data,sz,s);
}

fmi2_status_t fmi2_capi_get_directional_derivative(fmi2_capi_t* fmu, const fmi2_value_reference_t v_ref[], size_t nv,
                                                                   const fmi2_value_reference_t z_ref[], size_t nz,
                                                                   const fmi2_real_t dv[], fmi2_real_t dz[]){
	return fmu->fmiGetDirectionalDerivative(fmu -> c,v_ref, nv, z_ref, nz, dv, dz);
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

FMISETX(fmi2_capi_set_real,		fmiSetReal,		fmi2_real_t)
FMISETX(fmi2_capi_set_integer,	fmiSetInteger,	fmi2_integer_t)
FMISETX(fmi2_capi_set_boolean,	fmiSetBoolean,	fmi2_boolean_t)
FMISETX(fmi2_capi_set_string,	fmiSetString,	fmi2_string_t)

FMIGETX(fmi2_capi_get_real,	fmiGetReal,		fmi2_real_t)
FMIGETX(fmi2_capi_get_integer,	fmiGetInteger,	fmi2_integer_t)
FMIGETX(fmi2_capi_get_boolean,	fmiGetBoolean,	fmi2_boolean_t)
FMIGETX(fmi2_capi_get_string,	fmiGetString,	fmi2_string_t)
