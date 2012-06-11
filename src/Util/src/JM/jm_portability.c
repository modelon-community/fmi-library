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

#include <stdio.h>

#include "JM/jm_types.h"
#include "JM/jm_portability.h"

#ifdef WIN32
#include <direct.h>
#define get_current_working_directory _getcwd
#define set_current_working_directory _chdir	
#else
#include <unistd.h>
#define get_current_working_directory getcwd
#define set_current_working_directory chdir
#endif

#define JM_PORTABILITY_DLL_ERROR_MESSAGE_SIZE 1000

DLL_HANDLE jm_portability_load_dll_handle(const char* dll_file_path)
{
#ifdef WIN32
	/* printf("Will try to load %s\n", dll_file_path); */
	return LoadLibrary(dll_file_path);
#else	
	return dlopen(dll_file_path, RTLD_LAZY);
#endif
}

jm_status_enu_t jm_portability_free_dll_handle(DLL_HANDLE dll_handle)
{	
#ifdef WIN32		
	if (FreeLibrary(dll_handle)==0) {
		return jm_status_error;
	} else {
		return jm_status_success;
	}
#else		
	if (dlclose(dll_handle)==0) {
		return jm_status_success;			
	} else {
		return jm_status_error;
	}
#endif
}

jm_status_enu_t jm_portability_load_dll_function(DLL_HANDLE dll_handle, char* dll_function_name, jm_dll_function_ptr* dll_function_ptrptr)
{
#ifdef WIN32
	*dll_function_ptrptr = (jm_dll_function_ptr)GetProcAddress(dll_handle, dll_function_name);
#else
	*dll_function_ptrptr = dlsym(dll_handle, dll_function_name);
#endif

	if (*dll_function_ptrptr == NULL) {
		return jm_status_error;
	} else {
		return jm_status_success;
	}
}

char* jm_portability_get_last_dll_error(void)
{
	static char err_str[JM_PORTABILITY_DLL_ERROR_MESSAGE_SIZE]; 

#ifdef WIN32
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	sprintf(err_str, "%s", lpMsgBuf);
#else
	sprintf(err_str, "%s", dlerror());
#endif	
	return err_str;
}


jm_status_enu_t jm_portability_get_current_working_directory(char* buffer, size_t len)
{
	if (get_current_working_directory(buffer, len) == NULL) {
		return jm_status_error;
	} else {
		return jm_status_success;
	}
}

jm_status_enu_t jm_portability_set_current_working_directory(char* cwd)
{
	if (set_current_working_directory(cwd) == 0) {
		return jm_status_success;
	} else {
		return jm_status_error;
	}
}