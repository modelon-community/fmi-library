#ifndef fmiFunctions_h
#define fmiFunctions_h

/* This header file must be utilized when compiling a FMU.
   It defines all functions of the
         FMI 2.0 Model Exchange and Co-Simulation Interface.

   In order to have unique function names even if several FMUs
   are compiled together (e.g. for embedded systems), every "real" function name
   is constructed by prepending the function name by "FMIAPI_FUNCTION_PREFIX".
   Therefore, the typical usage is:

      #define FMIAPI_FUNCTION_PREFIX MyModel_
      #include "fmiFunctions.h"

   As a result, a function that is defined as "fmiGetDerivatives" in this header file,
   is actually getting the name "MyModel_fmiGetDerivatives".

   This only holds if the FMU is shipped in C source code, or is compiled in a
   static link library. For FMUs compiled in a DLL/sharedObject, the "actual" function
   names are used and "FMIAPI_FUNCTION_PREFIX" must not be defined.

   Revisions:
   - July 13, 2012: Portability improvements:
				-	DllExport changed into FMIAPI
				-	FUNCTION_PREFIX into FMIAPI_FUNCTION_PREFIX
				-	Allow for undefined FMIAPI_FUNCTION_PREFIX
   - Nov. 14, 2011: Adapted to FMI 2.0:
                       o Split into two files (fmiFunctions.h, fmiTypes.h) in order
                         that code that dynamically loads an FMU can directly
                         utilize the header files).
                       o Added C++ encapsulation of C-part, in order that the header
                         file can be directly utilized in C++ code.
                       o fmiCallbackFunctions is passed as pointer to fmiInstantiateXXX
                       o stepFinished within fmiCallbackFunctions has as first
                         argument "fmiComponentEnvironment" and not "fmiComponent".
                       o New functions to get and set the complete FMU state
                         and to compute partial derivatives.
   - Nov.  4, 2010: Adapted to specification text:
                       o fmiGetModelTypesPlatform renamed to fmiGetTypesPlatform
                       o fmiInstantiateSlave: Argument GUID     replaced by fmuGUID
                                              Argument mimetype replaced by mimeType
                       o tabs replaced by spaces
   - Oct. 16, 2010: Functions for FMI for Co-simulation added
   - Jan. 20, 2010: stateValueReferencesChanged added to struct fmiEventInfo (ticket #27)
                    (by M. Otter, DLR)
                    Added WIN32 pragma to define the struct layout (ticket #34)
                    (by J. Mauss, QTronic)
   - Jan.  4, 2010: Removed argument intermediateResults from fmiInitialize
                    Renamed macro fmiGetModelFunctionsVersion to fmiGetVersion
                    Renamed macro fmiModelFunctionsVersion to fmiVersion
                    Replaced fmiModel by fmiComponent in decl of fmiInstantiateModel
                    (by J. Mauss, QTronic)
   - Dec. 17, 2009: Changed extension "me" to "fmi" (by Martin Otter, DLR).
   - Dez. 14, 2009: Added eventInfo to meInitialize and added
                    meGetNominalContinuousStates (by Martin Otter, DLR)
   - Sept. 9, 2009: Added DllExport (according to Peter Nilsson's suggestion)
                    (by A. Junghanns, QTronic)
   - Sept. 9, 2009: Changes according to FMI-meeting on July 21:
                    meInquireModelTypesVersion     -> meGetModelTypesPlatform
                    meInquireModelFunctionsVersion -> meGetModelFunctionsVersion
                    meSetStates                    -> meSetContinuousStates
                    meGetStates                    -> meGetContinuousStates
                    removal of meInitializeModelClass
                    removal of meGetTime
                    change of arguments of meInstantiateModel
                    change of arguments of meCompletedIntegratorStep
                    (by Martin Otter, DLR):
   - July 19, 2009: Added "me" as prefix to file names (by Martin Otter, DLR).
   - March 2, 2009: Changed function definitions according to the last design
                    meeting with additional improvements (by Martin Otter, DLR).
   - Dec. 3 , 2008: First version by Martin Otter (DLR) and Hans Olsson (Dynasim).

   Copyright © 2008-2011, MODELISAR consortium. All rights reserved.
   This file is licensed by the copyright holders under the BSD License
   (http://www.opensource.org/licenses/bsd-license.html):

   ----------------------------------------------------------------------------
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   - Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
   - Neither the name of the copyright holders nor the names of its
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
   TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
   OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
   ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   ----------------------------------------------------------------------------

   with the extension:

   You may distribute or publicly perform any modification only under the
   terms of this license.
   (Note, this means that if you distribute a modified file,
    the modified file must also be provided under this license).
*/

#ifdef __cplusplus
extern "C" {
#endif

#include "fmiTypesPlatform.h"
#include "fmiFunctionTypes.h"
#include <stdlib.h>


/* 
	Export FMI API functions on Windows and under GCC.
	If custom linking is desired then the FMIAPI must be 
	defined before including this file.	For instance, 
	it may be set to __declspec(dllimport).
*/
#if !defined(FMIAPI) && !defined(FMIAPI_FUNCTION_PREFIX)
 #if defined _WIN32 || defined __CYGWIN__
	/* Note: both gcc & MSVC on Windows support this syntax. */
      #define FMIAPI __declspec(dllexport) 
 #else
  #if __GNUC__ >= 4
    #define FMIAPI __attribute__ ((visibility ("default")))
  #else
    #define FMIAPI
  #endif
 #endif
#endif

/* Macros to construct the real function name
   (prepend function name by FMIAPI_FUNCTION_PREFIX) */
#if defined(FMIAPI_FUNCTION_PREFIX)
	#define fmiPaste(a,b)     a ## b
	#define fmiPasteB(a,b)    fmiPaste(a,b)
	#define fmiFullName(name) fmiPasteB(FMIAPI_FUNCTION_PREFIX, name)
#else
	#define fmiFullName(name) name
#endif

/***************************************************
Common Functions
****************************************************/
#define fmiGetTypesPlatform         fmiFullName(fmiGetTypesPlatform)
#define fmiGetVersion               fmiFullName(fmiGetVersion)
#define fmiSetDebugLogging          fmiFullName(fmiSetDebugLogging)
#define fmiGetReal                  fmiFullName(fmiGetReal)
#define fmiGetInteger               fmiFullName(fmiGetInteger)
#define fmiGetBoolean               fmiFullName(fmiGetBoolean)
#define fmiGetString                fmiFullName(fmiGetString)
#define fmiSetReal                  fmiFullName(fmiSetReal)
#define fmiSetInteger               fmiFullName(fmiSetInteger)
#define fmiSetBoolean               fmiFullName(fmiSetBoolean)
#define fmiSetString                fmiFullName(fmiSetString)
#define fmiGetFMUstate              fmiFullName(fmiGetFMUstate)
#define fmiSetFMUstate              fmiFullName(fmiSetFMUstate)
#define fmiFreeFMUstate             fmiFullName(fmiFreeFMUstate)
#define fmiSerializedFMUstateSize   fmiFullName(fmiSerializedFMUstateSize)
#define fmiSerializeFMUstate        fmiFullName(fmiSerializeFMUstate)
#define fmiDeSerializeFMUstate      fmiFullName(fmiDeSerializeFMUstate)
#define fmiGetPartialDerivatives    fmiFullName(fmiGetPartialDerivatives)
#define fmiGetDirectionalDerivative fmiFullName(fmiGetDirectionalDerivative)


/***************************************************
Functions for FMI for Model Exchange
****************************************************/
#define fmiInstantiateModel           fmiFullName(fmiInstantiateModel)
#define fmiFreeModelInstance          fmiFullName(fmiFreeModelInstance)
#define fmiSetTime                    fmiFullName(fmiSetTime)
#define fmiSetContinuousStates        fmiFullName(fmiSetContinuousStates)
#define fmiInitializeModel            fmiFullName(fmiInitializeModel)
#define fmiEventUpdate                fmiFullName(fmiEventUpdate)
#define fmiCompletedIntegratorStep    fmiFullName(fmiCompletedIntegratorStep)
#define fmiTerminate                  fmiFullName(fmiTerminate)
#define fmiGetDerivatives             fmiFullName(fmiGetDerivatives)
#define fmiGetEventIndicators         fmiFullName(fmiGetEventIndicators)
#define fmiGetContinuousStates        fmiFullName(fmiGetContinuousStates)
#define fmiGetNominalContinuousStates fmiFullName(fmiGetNominalContinuousStates)
#define fmiGetStateValueReferences    fmiFullName(fmiGetStateValueReferences)


/***************************************************
Functions for FMI for Co-Simulation
****************************************************/
#define fmiInstantiateSlave         fmiFullName(fmiInstantiateSlave)
#define fmiFreeSlaveInstance        fmiFullName(fmiFreeSlaveInstance)
#define fmiInitializeSlave          fmiFullName(fmiInitializeSlave)
#define fmiTerminateSlave           fmiFullName(fmiTerminateSlave)
#define fmiResetSlave               fmiFullName(fmiResetSlave)
#define fmiSetRealInputDerivatives  fmiFullName(fmiSetRealInputDerivatives)
#define fmiGetRealOutputDerivatives fmiFullName(fmiGetRealOutputDerivatives)
#define fmiDoStep                   fmiFullName(fmiDoStep)
#define fmiCancelStep               fmiFullName(fmiCancelStep)
#define fmiGetStatus                fmiFullName(fmiGetStatus)
#define fmiGetRealStatus            fmiFullName(fmiGetRealStatus)
#define fmiGetIntegerStatus         fmiFullName(fmiGetIntegerStatus)
#define fmiGetBooleanStatus         fmiFullName(fmiGetBooleanStatus)
#define fmiGetStringStatus          fmiFullName(fmiGetStringStatus)

/* Version number */
#define fmiVersion "2.0"


/***************************************************
Common Functions
****************************************************/

/* Inquire version numbers of header files */
   FMIAPI fmiGetTypesPlatformTYPE fmiGetTypesPlatform;
   FMIAPI fmiGetVersionTYPE       fmiGetVersion;
   FMIAPI fmiSetDebugLoggingTYPE  fmiSetDebugLogging;

/* Getting and setting variables values */
   FMIAPI fmiGetRealTYPE    fmiGetReal;
   FMIAPI fmiGetIntegerTYPE fmiGetInteger;
   FMIAPI fmiGetBooleanTYPE fmiGetBoolean;
   FMIAPI fmiGetStringTYPE  fmiGetString;

   FMIAPI fmiSetRealTYPE    fmiSetReal;
   FMIAPI fmiSetIntegerTYPE fmiSetInteger;
   FMIAPI fmiSetBooleanTYPE fmiSetBoolean;
   FMIAPI fmiSetStringTYPE  fmiSetString;

/* Getting and setting the internal FMU state */
   FMIAPI fmiGetFMUstateTYPE            fmiGetFMUstate;
   FMIAPI fmiSetFMUstateTYPE            fmiSetFMUstate;
   FMIAPI fmiFreeFMUstateTYPE           fmiFreeFMUstate;
   FMIAPI fmiSerializedFMUstateSizeTYPE fmiSerializedFMUstateSize;
   FMIAPI fmiSerializeFMUstateTYPE      fmiSerializeFMUstate;
   FMIAPI fmiDeSerializeFMUstateTYPE    fmiDeSerializeFMUstate;

/* Getting partial derivatives */
   FMIAPI fmiGetDirectionalDerivativeTYPE fmiGetDirectionalDerivative;

/***************************************************
Functions for FMI for Model Exchange
****************************************************/

/* Creation and destruction of model instances and setting debug status */
   FMIAPI fmiInstantiateModelTYPE  fmiInstantiateModel;
   FMIAPI fmiFreeModelInstanceTYPE fmiFreeModelInstance;

/* Providing independent variables and re-initialization of caching */
   FMIAPI fmiSetTimeTYPE                 fmiSetTime;
   FMIAPI fmiSetContinuousStatesTYPE     fmiSetContinuousStates;
   FMIAPI fmiCompletedIntegratorStepTYPE fmiCompletedIntegratorStep;

/* Evaluation of the model equations */
   FMIAPI fmiInitializeModelTYPE         fmiInitializeModel;
   FMIAPI fmiEventUpdateTYPE             fmiEventUpdate;
   FMIAPI fmiCompletedIntegratorStepTYPE fmiCompletedIntegratorStep;
   FMIAPI fmiTerminateTYPE               fmiTerminate;

   FMIAPI fmiGetDerivativesTYPE             fmiGetDerivatives;
   FMIAPI fmiGetEventIndicatorsTYPE         fmiGetEventIndicators;
   FMIAPI fmiGetContinuousStatesTYPE        fmiGetContinuousStates;
   FMIAPI fmiGetNominalContinuousStatesTYPE fmiGetNominalContinuousStates;
   FMIAPI fmiGetStateValueReferencesTYPE    fmiGetStateValueReferences;


/***************************************************
Functions for FMI for Co-Simulation
****************************************************/

/* Creation and destruction of slave instances */
   FMIAPI fmiInstantiateSlaveTYPE  fmiInstantiateSlave;
   FMIAPI fmiFreeSlaveInstanceTYPE fmiFreeSlaveInstance;

/* Simulating the slave */
   FMIAPI fmiInitializeSlaveTYPE fmiInitializeSlave;
   FMIAPI fmiTerminateSlaveTYPE  fmiTerminateSlave;
   FMIAPI fmiResetSlaveTYPE      fmiResetSlave;

   FMIAPI fmiSetRealInputDerivativesTYPE  fmiSetRealInputDerivatives;
   FMIAPI fmiGetRealOutputDerivativesTYPE fmiGetRealOutputDerivatives;

   FMIAPI fmiDoStepTYPE     fmiDoStep;
   FMIAPI fmiCancelStepTYPE fmiCancelStep;

/* Inquire slave status */
   FMIAPI fmiGetStatusTYPE        fmiGetStatus;
   FMIAPI fmiGetRealStatusTYPE    fmiGetRealStatus;
   FMIAPI fmiGetIntegerStatusTYPE fmiGetIntegerStatus;
   FMIAPI fmiGetBooleanStatusTYPE fmiGetBooleanStatus;
   FMIAPI fmiGetStringStatusTYPE  fmiGetStringStatus;

#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif /* fmiFunctions_h */
