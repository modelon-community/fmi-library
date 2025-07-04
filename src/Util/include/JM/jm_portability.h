/*
    Copyright (C) 2012-2023 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

     This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#ifndef JM_PORTABILITY_H_
#define JM_PORTABILITY_H_
#include <fmilib_config.h>
#include "jm_callbacks.h"

/* Include platform dependent headers */
#if defined(_MSC_VER) || defined(WIN32) || defined(__MINGW32__)
#include <windows.h> /* Microsoft Windows API */
#define DLL_HANDLE HANDLE
#else
#define DLL_HANDLE void*
#include <dlfcn.h>  /* Standard POSIX/UNIX API */
#endif

#include "jm_types.h"


#ifdef __cplusplus
extern "C" {
#endif

/** \file jm_portability.h
    Handling platform specific defines and functions.
*/
/**
    \addtogroup jm_utils
    @{
        \addtogroup jm_portability
    @}
*/
/** \addtogroup jm_portability Handling platform specific defines and functions
@{*/

/**
 * \brief Type for flag used when loading shared library. Same as would be passed to dlopen or LoadLibraryEx.
 */
#ifdef WIN32
typedef DWORD jm_portability_loadlibrary_flag_t;
#else
typedef int jm_portability_loadlibrary_flag_t;
#endif

/** \brief
 * Returns the default flag (defined by FMI Library) for the platform.
 */
jm_portability_loadlibrary_flag_t jm_portability_get_load_dll_handle_default_flag();

/** \brief
 * Load a shared library into the process and return a handle.
 *
 * The same flag as would be used when calling the underlying system dependent
 * library loading function can be used.
 */
DLL_HANDLE jm_portability_load_dll_handle_with_flag(const char* dll_file_path, jm_portability_loadlibrary_flag_t flag);

/** \brief Load a dll/so library into the process and return a handle. */
DLL_HANDLE        jm_portability_load_dll_handle        (const char* dll_file_path);

/** \brief Unload a Dll and release the handle*/
jm_status_enu_t jm_portability_free_dll_handle        (DLL_HANDLE dll_handle);

/** \brief A function pointer as returned when DLL symbol is loaded.*/
#ifdef WIN32
#define jm_dll_function_ptr FARPROC
#else
typedef void* jm_dll_function_ptr;
#endif

/** \brief Find a function in the Dll and return a function pointer */
jm_status_enu_t jm_portability_load_dll_function    (DLL_HANDLE dll_handle, char* dll_function_name, jm_dll_function_ptr* dll_function_ptrptr);

/** \brief Return error associated with Dll handling */
char* jm_portability_get_last_dll_error    (void);

/** \brief Get current working directory name */
jm_status_enu_t jm_portability_get_current_working_directory(char* buffer, size_t len);

/** \brief Set current working directory*/
jm_status_enu_t jm_portability_set_current_working_directory(const char* cwd);

/** \brief Get system-wide temporary directory */
const char* jm_get_system_temp_dir();

/**
    \brief Create a uniquely named temporary directory.
    @param cb - callbacks for memory allocation and logging. Default callbacks
            are used if this parameter is NULL.
    @param tmplt Directory name template ending with XXXXXX. The template is
            modified by the call.
    @return A pointer to the modified template. The function returns NULL if
            the template does not end with XXXXXX, or if the directory could
            not be created.
*/
char *jm_mkdtemp(jm_callbacks *cb, char *tmplt);

/**
    \brief Get absolute path to an existing directory
    @param cb - callbacks for memory allocation and logging. Default callbacks are used if this parameter is NULL.
    @param dir - path to a directory (relative or absolute).
    @param outPath - buffer for storing the directory
    @param len - of the buffer (if size is larger than FILENAME_MAX + 1 then the path will always fit in)
    @return Pointer to outPath on success, 0 - on error in which case a message is send to the logger.
*/
char* jm_get_dir_abspath(jm_callbacks* cb, const char* dir, char* outPath, size_t len);

/**
    \brief Create a unique temporary directory
    @param cb - callbacks for memory allocation and logging. Default callbacks are used if this parameter is NULL.
    @param systemTempDir - directory where the temp dir should be located both absolute and relative path are accepted.
                System-wide directory is used if this parameter is NULL.
    @param tempPrefix - File name template prefix used when creating temporaty directories. "jm" is used if this is NULL.
    @return A pointer to the temporary directory name (absolute path, no terminating '/'). Caller is responsible for freeing the memory.
        The function returns NULL if there were errors in which case a message is send to the logger.
*/
char* jm_mk_temp_dir(jm_callbacks* cb, const char* systemTempDir, const char* tempPrefix);

/**
    \brief Create a file:// URL from absolute path
    @param cb - callbacks for memory allocation and logging. Default callbacks are used if this parameter is NULL.
    @param absPath - absolute path to be converted into the URL
    @return A pointer to the URL. Caller is responsible for freeing the memory.
        The function returns NULL if there were errors in which case a message is send to the logger.
*/
char* jm_create_URL_from_abs_path(jm_callbacks* cb, const char* absPath);

/**
    \brief Make a directory.
*/
jm_status_enu_t jm_mkdir(jm_callbacks* cb, const char* dir);

/**
\brief Remove directory and all it contents.
*/
jm_status_enu_t jm_rmdir(jm_callbacks* cb, const char* dir);

/**
    \brief C89 compatible implementation of C99 vsnprintf.

    WARNING:
    Return value and what is written to buffer is compiler/platform dependent on
    failure.
    Some implementations return -1 on failure to write all chars, others a
    number greater than allowed write size. Writing of terminating null
    character on failure may also vary.
    For example, compare vsnprintf documentation for MSVC 2013 and 2015.
*/
FMILIB_EXPORT
int jm_vsnprintf(char * str, size_t size, const char * fmt, va_list al) jm_vprintf_format(3);

/**
    \brief C89 compatible implementation of C99 snprintf.

    WARNING:
    Return value and what is written to buffer is compiler/platform dependent on
    failure.
    See jm_vsnprintf for more info.
*/
FMILIB_EXPORT
int jm_snprintf(char * str, size_t size, const char * fmt, ...) jm_printf_format(3);

#ifdef HAVE_VA_COPY
#define JM_VA_COPY va_copy
#elif defined(HAVE___VA_COPY)
#define JM_VA_COPY __va_copy
#elif defined(WIN32)
#define JM_VA_COPY(dest,src) dest=src
#endif

/**
   \brief Sets the LC_NUMERIC locale. For MSVC and Linux, the locale is set for
   the current thread only (mt-safe). A follow up call to
   'jm_resetlocale_numeric' is needed to free the returned 'jm_locale_t'
   object.

   @param cb ::jm_callbacks
   @param value: Value to set for LC_NUMERIC.
   @return:
     Pointer to object for reseting thread settings (locale, and
     _configthreadlocale on Windows). NULL on failure.
 */
jm_locale_t* jm_setlocale_numeric(jm_callbacks* cb, const char* value);

/**
   \brief  Restores thread settings and locale.

   This function is only allowed to be called when the current locale is set by
   'jm_setlocale_numeric', and the 'jmloc' argument must be what is
   returned from that call. On Linux, the locale must in no way be modified since that
   call.

    @param cb ::jm_callbacks
    @param jmloc:
     Return value from previous call to 'jm_setlocale_numeric'. Current
     locale must be set with that function. This call will free 'jmloc', so it's
     not allowed to be used after.
   @return:
     0 on success.
    */
int jm_resetlocale_numeric(jm_callbacks* cb, jm_locale_t* jmloc);

#ifdef __cplusplus
}
#endif


/**@}*/
#endif /* End of header file JM_PORTABILITY_H_ */
