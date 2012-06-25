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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <fmilib_config.h>

#include <JM/jm_types.h>
#include <JM/jm_portability.h>

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

#ifdef WIN32
#define MAX_TEMP_DIR_NAME_LENGTH 262
TCHAR jm_temp_dir_buffer[MAX_TEMP_DIR_NAME_LENGTH];
#endif

const char* jm_get_system_temp_dir() {
#ifdef WIN32
	if(!GetTempPath(MAX_TEMP_DIR_NAME_LENGTH, jm_temp_dir_buffer)) return 0;
	return jm_temp_dir_buffer;
#else
	return "/tmp/";
#endif
}

#ifdef WIN32
#include <io.h>
#define mktemp _mktemp
#else
char* mktemp(char*);
#endif
 
char* jm_mktemp(char* tmplt) {
	return mktemp(tmplt);
}

#ifdef WIN32
#include <direct.h>
#define MKDIR(dir) _mkdir(dir)
#else
#include <errno.h>
#include <sys/stat.h>
#define MKDIR(dir) mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
#endif

jm_status_enu_t jm_mkdir(jm_callbacks* cb, const char* dir) {
	if(MKDIR(dir)) {
		jm_log_fatal(cb,"JMPRT","Could not create directory %s", dir);
		return jm_status_error;
	}
	else
		return jm_status_success;
}


jm_status_enu_t jm_rmdir(jm_callbacks* cb, const char* dir) {
#ifdef WIN32
	const char* fmt_cmd = "rmdir /s /q %s";
#else
    const char* fmt_cmd = "rm -rf %s";
#endif
    char * buf = (char*)cb->calloc(sizeof(char), strlen(dir)+strlen(fmt_cmd)+1);
	if(!buf) {
	    jm_log_error(cb,"JMPRT","Could not allocate memory");
		return jm_status_error;
	}
    sprintf(buf, fmt_cmd, dir);
#ifdef WIN32
	{
		char* ch = buf+strlen(fmt_cmd) - 2;
		while(*ch) {
			if(*ch == '/') *ch = '\\';
			ch++;
		}
	}
#endif
    jm_log_verbose(cb,"JMPRT","Removing %s", dir);
    if(system(buf)) {
	    jm_log_error(cb,"JMPRT","Error removing %s (%s)", dir, strerror(errno));
		return jm_status_error;
	}
    cb->free(buf);
	return jm_status_success;
}