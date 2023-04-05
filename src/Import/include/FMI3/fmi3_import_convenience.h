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



/** \file fmi3_import_convenience.h
*  \brief Public interface to the FMI import C-library. Convenience functions.
*
*  The functions in this file are provided for convenience. The functionality
*  is already available via other lower level functions.
*/

#ifndef FMI3_IMPORT_CONVENIENCE_H_
#define FMI3_IMPORT_CONVENIENCE_H_

#include <FMI/fmi_import_context.h>
#include <FMI3/fmi3_function_types.h>

#ifdef __cplusplus
extern "C" {
#endif
        /**
    \addtogroup fmi3_import
    @{
    \addtogroup fmi3_import_convenience Convenience functions.
    @}
    \addtogroup fmi3_import_convenience Convenience functions.
    \brief The functions in this module are provided for convenience. The functionality
    *  is already available via other lower level functions.

    @{
    */
/**
\brief Collection of counters providing model information.
    */
typedef struct {
    /** \brief Number of constants */
    unsigned int num_constants;
    /** \brief  Number of fixed */
    unsigned int num_fixed;
    /** \brief  Number of tunable */
    unsigned int num_tunable;
    /** \brief  Number of discrete variables */
    unsigned int num_discrete;
    /** \brief  Number of continuous variables */
    unsigned int num_continuous;

    /** \brief  Number of parameters*/
    unsigned int num_parameters;
    /** \brief  Number of calculated parameters*/
    unsigned int num_calculated_parameters;
    /** \brief  Number of inputs */
    unsigned int num_inputs;
    /** \brief  Number of outputs */
    unsigned int num_outputs;
    /** \brief  Number of local variables */
    unsigned int num_local;
    /** \brief  Number of independent variables */
    unsigned int num_independent;

    /** \brief  Number of float64 variables*/
    unsigned int num_float64_vars;
    /** \brief  Number of float32 variables*/
    unsigned int num_float32_vars;

    /** \brief  Number of int64 variables*/
    unsigned int num_int64_vars;
    /** \brief  Number of int32 variables*/
    unsigned int num_int32_vars;
    /** \brief  Number of int16 variables*/
    unsigned int num_int16_vars;
    /** \brief  Number of int8 variables*/
    unsigned int num_int8_vars;
    /** \brief  Number of int64 variables*/
    unsigned int num_uint64_vars;
    /** \brief  Number of uint32 variables*/
    unsigned int num_uint32_vars;
    /** \brief  Number of uint16 variables*/
    unsigned int num_uint16_vars;
    /** \brief  Number of uint8 variables*/
    unsigned int num_uint8_vars;

    /** \brief  Number of enumeration variables*/
    unsigned int num_enum_vars;
    /** \brief  Number of boolean variables*/
    unsigned int num_bool_vars;
    /** \brief  Number of string variables*/
    unsigned int num_string_vars;
} fmi3_import_model_counts_t;

/**
\brief Struct for initializing #fmi3_logger.
 */
typedef struct {
    const fmi3_instance_environment_t instanceEnvironment;
    const fmi3_log_message_callback_ft logMessage;
} fmi3_logger_context_t;

/**
    \brief Collect model information by counting the number of variables with specific properties and fillinf in fmi3_import_model_counts_t struct.
    \param fmu - An fmu object as returned by fmi3_import_parse_xml().
    \param counts - a pointer to a preallocated struct.
*/
FMILIB_EXPORT
void fmi3_import_collect_model_counts(fmi3_import_t* fmu, fmi3_import_model_counts_t* counts);

/**
  \brief Transform msgIn into msgOut by expanding variable references of the form #\<Type\>\<VR\># into variable names
  and replacing '##' with a single #.
   \param fmu - An fmu object as returned by fmi3_import_parse_xml().
   \param msgIn - Log message as produced by an FMU.
   \param msgOut - Output message buffer.
   \param maxMsgSize - maximum message size
   */
FMILIB_EXPORT
void fmi3_import_expand_variable_references(fmi3_import_t* fmu, const char* msgIn, char* msgOut, size_t maxMsgSize);

/**
    \brief Default function for fmi3CallbackAllocateMemory.

    Calls calloc from stdlib.h. Argument 'instEnv' is not used.
*/
FMILIB_EXPORT
void* fmi3_callback_allocate_memory_default(fmi3_instance_environment_t instEnv, size_t nobj, size_t size);

/**
    \brief Default function for fmi3CallbackFreeMemory.

    Calls free from stdlib.h. Argument 'instEnv' is not used.
*/
FMILIB_EXPORT
void fmi3_callback_free_memory_default(fmi3_instance_environment_t instEnv, void* obj);

/**
    \brief An implementation of FMI 3.0 logger that forwards the messages to logger function inside ::jm_callbacks structure.

    The function is using a global array of active FMUs to find out which FMU is sending the log messege. It then
    forwards the message to the logger connected to the particular ::fmi3_import_t struct. The function is called by the FMU.
    The FMU must be loaded with non-zero registerGlobally parameter of fmi3_import_create_dllfmu() in order to work.
    If no matching ::fmi3_import_t struct is found on the global list then jm_get_default_callbacks() is used to get the default logger.
    Note that this function is not thread safe due to the use of the global list.
*/
FMILIB_EXPORT
void  fmi3_log_forwarding(fmi3_instance_environment_t inst, fmi3_status_t status, fmi3_string_t category, fmi3_string_t message);

/** \brief  Default FMI 3.0 logger may be used when instantiating FMUs */
FMILIB_EXPORT
void  fmi3_default_callback_logger(fmi3_instance_environment_t inst, fmi3_string_t instanceName, fmi3_status_t status, fmi3_string_t category, fmi3_string_t message);

/** \brief  Given ::fmi3_callback_functions_t logger (fmi3_logger), the ::jm_callbacks logger may be setup to redirect the messages to the fmi3_logger.

    The functions sets up the redirection. Note that the context field in ::jm_callbacks is set to point to the provided ::fmi3_callback_functions_t.
    \param cb FMI Library callbacks
    \param fmiCallbacks FMI 3.0 standard callbacks
*/
FMILIB_EXPORT
void fmi3_import_init_logger(jm_callbacks* cb, fmi3_logger_context_t* loggerCallbacks);

/**    @} */

/** @} */

#ifdef __cplusplus
}
#endif
#endif /* FMI3_IMPORT_CONVENIENCE_H_ */
