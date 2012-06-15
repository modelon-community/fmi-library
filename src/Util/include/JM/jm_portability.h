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

#ifndef JM_PORTABILITY_H_
#define JM_PORTABILITY_H_
#include <fmilib_config.h>
#include "jm_callbacks.h"

/* Include platform dependent headers */
#if defined(_MSC_VER) || defined(WIN32)
#include <windows.h> /* Microsoft Windows API */
#define DLL_HANDLE HANDLE
#else
#define DLL_HANDLE void*
#include <dlfcn.h>  /* Standard POSIX/UNIX API */
#endif

#include "jm_types.h"
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
/** \brief Load a dll/so library into the process and return a handle. */
DLL_HANDLE		jm_portability_load_dll_handle		(const char* dll_file_path);

/** \brief Unload a Dll and release the handle*/
jm_status_enu_t jm_portability_free_dll_handle		(DLL_HANDLE dll_handle);

/** \brief A function pointer as returned when DLL symbol is loaded.*/
#ifdef WIN32
#define jm_dll_function_ptr FARPROC
#else
typedef void* jm_dll_function_ptr; 
#endif

/** \brief Find a function in the Dll and return a function pointer */
jm_status_enu_t jm_portability_load_dll_function	(DLL_HANDLE dll_handle, char* dll_function_name, jm_dll_function_ptr* dll_function_ptrptr);

/** \brief Return error associated with Dll handling */
char* jm_portability_get_last_dll_error	(void);

/** \brief Get current working directory name */
jm_status_enu_t jm_portability_get_current_working_directory(char* buffer, size_t len);

/** \brief Set current working directory*/
jm_status_enu_t jm_portability_set_current_working_directory(char* cwd);

/** \brief Get system-wide temporary directory */
const char* jm_get_system_temp_dir();

/** 
	\brief Create a unique file name 
	\param tmplt File name template ending with XXXXXX.
	\return A pointer to the modified template. The function returns NULL if 
	template is badly formed or no more unique names can be created from the given template.
*/
char* jm_mktemp(char* tmplt);

/**
	\brief Make a directory.
*/
jm_status_enu_t jm_mkdir(jm_callbacks* cb, const char* dir);

/**
\brief Remove directory and all it contents.
*/
jm_status_enu_t jm_rmdir(jm_callbacks* cb, const char* dir);

/*@}*/
#endif /* End of header file JM_PORTABILITY_H_ */
