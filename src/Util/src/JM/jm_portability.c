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
#include <locale.h>
#include <string.h>

#include <fmilib_config.h>

#include <JM/jm_types.h>
#include <JM/jm_portability.h>

static const char * module = "JMPRT";

#ifdef WIN32
#include <shlwapi.h>
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
	return dlopen(dll_file_path, RTLD_NOW|RTLD_LOCAL);
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
	jm_snprintf(err_str, JM_PORTABILITY_DLL_ERROR_MESSAGE_SIZE, "%s", lpMsgBuf);
#else
	jm_snprintf(err_str, JM_PORTABILITY_DLL_ERROR_MESSAGE_SIZE, "%s", dlerror());
#endif	
	return err_str;
}


jm_status_enu_t jm_portability_get_current_working_directory(char* buffer, size_t len)
{
	int ilen = (int)len;
	if(ilen != len) ilen = FILENAME_MAX + 2;
	setlocale(LC_CTYPE, "en_US.UTF-8"); /* just in case, does not seem to have an effect */
	if (get_current_working_directory(buffer, ilen) == NULL) {
		return jm_status_error;
	} else {
		return jm_status_success;
	}
}

jm_status_enu_t jm_portability_set_current_working_directory(const char* cwd)
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
#else
#include <stdlib.h>
#endif
char *jm_mkdtemp(jm_callbacks *cb, char *tmplt)
{
#ifdef WIN32
    /* Windows does not have mkdtemp, use mktemp + mkdir */

    if(!_mktemp(tmplt)) {
        jm_log_fatal(cb, module, "Could not create a unique temporary directory name");
        return NULL;
    }
    if(jm_mkdir(cb, tmplt) != jm_status_success) {
        return NULL;
    }
    return tmplt;

#else
    return mkdtemp(tmplt);
#endif
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
	if(!cb) {
		cb = jm_get_default_callbacks();
	}
	if(MKDIR(dir)) {
		jm_log_fatal(cb,module,"Could not create directory %s", dir);
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
	if(!cb) {
		cb = jm_get_default_callbacks();
	}
	if(!buf) {
	    jm_log_error(cb,module,"Could not allocate memory");
		return jm_status_error;
	}
    sprintf(buf, fmt_cmd, dir);/*safe*/
#ifdef WIN32
	{
		char* ch = buf+strlen(fmt_cmd) - 2;
		while(*ch) {
			if(*ch == '/') *ch = '\\';
			ch++;
		}
	}
#endif
    jm_log_verbose(cb,module,"Removing %s", dir);
    if(system(buf)) {
	    jm_log_error(cb,module,"Error removing %s (%s)", dir, strerror(errno));
		return jm_status_error;
	}
    cb->free(buf);
	return jm_status_success;
}

char* jm_get_dir_abspath(jm_callbacks* cb, const char* dir, char* outPath, size_t len) {
	char curDir[FILENAME_MAX + 2];

	if(!cb) {
		cb = jm_get_default_callbacks();
	}
	if( jm_portability_get_current_working_directory(curDir, FILENAME_MAX+1) != jm_status_success) {
		jm_log_fatal(cb,module, "Could not get current working directory (%s)", strerror(errno));
		return 0;
	};

	if(jm_portability_set_current_working_directory(dir) != jm_status_success) {
		jm_log_fatal(cb,module, "Could not change to the directory %s", dir);
		jm_portability_set_current_working_directory(curDir);
		return 0;
	};
	if( jm_portability_get_current_working_directory(outPath, len) != jm_status_success) {
		jm_log_fatal(cb,module, "Could not get absolute path for the directory (%s)", strerror(errno));
		jm_portability_set_current_working_directory(curDir);
		return 0;
	};
	jm_portability_set_current_working_directory(curDir);
	return outPath;
}


char* jm_mk_temp_dir(jm_callbacks* cb, const char* systemTempDir, const char* tempPrefix)
{
	size_t len;

	char tmpDir[FILENAME_MAX + 2];
	char* tmpPath;

	if(!cb) {
		cb = jm_get_default_callbacks();
	}
	if(!systemTempDir) {
		systemTempDir = jm_get_system_temp_dir();
		if(!systemTempDir) systemTempDir = "./";
	}
	if(!tempPrefix) {
		tempPrefix = "jm";
	}
	len = strlen(systemTempDir);

	if(!jm_get_dir_abspath(cb, systemTempDir, tmpDir, FILENAME_MAX + 2)) {
		return 0;
	}

	len = strlen(tmpDir);
	if(tmpDir[len-1] != FMI_FILE_SEP[0]) {
		tmpDir[len] = FMI_FILE_SEP[0]; 
		tmpDir[len+1] = 0;
		len++;
	}
	len += strlen(tempPrefix) + 6;
	if(len + 16 > FILENAME_MAX) {
		jm_log_fatal(cb,module, "Canonical name for the temporary files directory is too long (system limit for path length is %d)", FILENAME_MAX);
		return 0;
	}
	tmpPath = (char*)cb->malloc(len + 7);
	if(!tmpPath) {
		jm_log_fatal(cb, module,"Could not allocate memory");
		return 0;
	}
	sprintf(tmpPath,"%s%sXXXXXX",tmpDir,tempPrefix);/*safe*/

    if (jm_mkdtemp(cb, tmpPath) == NULL) {
        jm_log_fatal(cb, module,"Could not create a unique temporary directory");
    }

	return tmpPath;
}

char* jm_create_URL_from_abs_path(jm_callbacks* cb, const char* path) {
	/* worst case: all symbols are 4-byte UTF-8 and need to be %-encoded */
#define MAX_URL_LENGTH  (FILENAME_MAX * 4 * 3 + 7)
	char buffer[MAX_URL_LENGTH];
	char* url;
	size_t urllen;
	if(!cb) {
		cb = jm_get_default_callbacks();
	}

#if defined(_WIN32) || defined(WIN32)
	{
		DWORD pathLen = MAX_URL_LENGTH;
		HRESULT code = UrlCreateFromPathA(
			path,
			buffer,
			&pathLen,
			0);
		if( (code != S_FALSE) && (code != S_OK)) {
			jm_log_fatal(cb, module,"Could not constuct file URL from path %s", path);
			return 0;
		}
		urllen = pathLen;
	}
#else
	{
		size_t i, len = strlen(path);
		char *curBuf = buffer + 7;
		unsigned char ch;
		strcpy(buffer, "file://");
		for( i = 0; i < len; i++) {
			ch = (unsigned char)path[i];
			if( (ch == '/') || ((ch >= 'A') && (ch <= 'Z')) 
				|| ((ch >= 'a') && (ch <= 'z'))
				|| ((ch >= '0') && (ch <= '9'))
				|| (ch == '-') || (ch == '_') || (ch == '.') ||(ch == '~')) {
					*curBuf = ch;
					curBuf++;
					continue;
			}
			sprintf(curBuf, "%%%2X", (int)ch);/*safe*/
			curBuf+=3;
		}
		*curBuf = 0;
		urllen = curBuf - buffer;
	}
#endif
	url = (char*)cb->malloc(urllen+1);
	if(!url) {
		jm_log_fatal(cb, module,"Could not allocate memory");
		return 0;
	}
	strcpy(url, buffer);
	return url;
}

#ifndef HAVE_VSNPRINTF
int jm_rpl_vsnprintf(char *, size_t, const char *, va_list);
#endif

int jm_vsnprintf(char * str, size_t size, const char * fmt, va_list al) {
    return jm_rpl_vsnprintf(str, size, fmt, al);
}

int jm_snprintf(char * str, size_t size, const char * fmt, ...) {
    va_list args;
    int ret;
    va_start (args, fmt);
    ret = jm_vsnprintf(str, size, fmt, args);
    va_end (args);
    return ret;
}
