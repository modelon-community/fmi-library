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

#ifndef JM_CALLBACKS_H
#define JM_CALLBACKS_H
#include <stddef.h>
#include <stdarg.h>

#include "jm_types.h"

#ifdef __cplusplus
extern "C" {
#endif
/** \file jm_callbacks.h Definition of ::jm_callbacks and supporting functions
	*
	* \addtogroup jm_utils
	* @{
		\addtogroup jm_callbacks
	* @}
*/
/** \addtogroup jm_callbacks Definition of callbacks struct and supporting functions
* @{ 
* Forward declaration of the callbacks struct*/
typedef struct jm_callbacks jm_callbacks;

/** \name Memory management callbacks
* jm_malloc_f, jm_realloc_f, jm_calloc_f, jm_free_f function 
* types correspond to the standard C memory management functions
* @{ 
*/
typedef jm_voidp (*jm_malloc_f)(size_t size);

typedef jm_voidp (*jm_realloc_f)(void *ptr, size_t size);

typedef jm_voidp (*jm_calloc_f)(size_t numitems, size_t itemsize);

typedef void (*jm_free_f)(jm_voidp p);
/* @}
*/

/*
* \brief Logger callback type.
* The logger callback is used to report errors. Note that this function is only used in
* FMI standard intependent code (e.g., fmi_import_context()). Since logging functions
* are different between different standards separate logging functions are necessary for each fmi implementation.
* See \ref fmi1_callbacks.h
*/
typedef void (*jm_logger_f)(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message);

#define JM_MAX_ERROR_MESSAGE_SIZE 2000

/** The callbacks struct is sent to all the modules in the library */
struct jm_callbacks {
        jm_malloc_f malloc; /** Allocate non-initialized memory */
        jm_calloc_f calloc; /** Allocate zero initialized memory */
        jm_realloc_f realloc;  /** Re-allocate memory */
        jm_free_f free;        /** Free-allocated memory */
        jm_logger_f logger;		/** Logging callback */
		jm_log_level_enu_t log_level; /** Logging level */
		jm_voidp context;	/** Arbitrary context pointer passed to the logger function  */
		char errMessageBuffer[JM_MAX_ERROR_MESSAGE_SIZE]; /** The buffer used along with jm_get_last_error() */
};

/**
* An alternative way to get error information is to use jm_get_last_error(). This is only meaningful
* if logger function is not present. Otherwize the string is always empty.
*/
static jm_string jm_get_last_error(jm_callbacks* cb) {return cb->errMessageBuffer; }

/**
 Clear the last generated error message.
*/
static void jm_clear_last_error(jm_callbacks* cb) { cb->errMessageBuffer[0] = 0; }

/**
Set the structure to be returned by jm_get_default_callbacks().
@param c - a pointer to initialized struct to be used as default later on. If this is NULL
	library default implementation will be used.
*/
void jm_set_default_callbacks(jm_callbacks* c);

/**
Get default callbacks. The function never returns NULL.
*/
jm_callbacks* jm_get_default_callbacks();

/**
Send a message to the logger function.
	@param cb - callbacks to be used for reporting;
	@param module - a name of reporting module;
	@param log_level - message kind;
	@param fmt - "printf" type of format followed by the arguments.
*/
void jm_log(jm_callbacks* cb, const char* module, jm_log_level_enu_t log_level, const char* fmt, ...);

/** \copydoc jm_log()
	@param ap - variable size argument list.
*/
void jm_log_v(jm_callbacks* cb, const char* module, jm_log_level_enu_t log_level, const char* fmt, va_list ap);

/* @}
*/

#ifdef __cplusplus
}
#endif
/* JM_CONTEXT_H */
#endif
