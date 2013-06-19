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
#include <stdarg.h>
#include <assert.h>

#include "JM/jm_callbacks.h"
#include "JM/jm_portability.h"

static const char* jm_log_level_str[] = 
{
	"NOTHING",
	"FATAL",
	"ERROR",
	"WARNING",
	"INFO",
	"VERBOSE",
	"DEBUG",
	"ALL"
};

/** \brief Convert log level into a string */
const char* jm_log_level_to_string(jm_log_level_enu_t level) {
	if((level >= jm_log_level_nothing) && (level <= jm_log_level_all))
		return jm_log_level_str[level];
	else {
		assert(0);
		return "UNEXPECTED";
	}
}

void jm_default_logger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message) {
	fprintf(stderr, "[%s][%s] %s\n", jm_log_level_to_string(log_level), module, message);
}

void jm_log(jm_callbacks* cb, const char* module, jm_log_level_enu_t log_level, const char* fmt, ...) {
	va_list args;
	if(log_level > cb->log_level) return;
    va_start (args, fmt);
    jm_log_v(cb, module, log_level, fmt, args);
    va_end (args);
}

void jm_log_v(jm_callbacks* cb, const char* module, jm_log_level_enu_t log_level, const char* fmt, va_list ap) {
	if(log_level > cb->log_level) return;
    jm_vsnprintf(cb->errMessageBuffer, JM_MAX_ERROR_MESSAGE_SIZE, fmt, ap);
	if(cb->logger) {
		cb->logger(cb,module, log_level, cb->errMessageBuffer);
	}
}

#define CREATE_LOG_FUNCTIONS(log_level) \
void jm_log_ ## log_level(jm_callbacks* cb, const char* module, const char* fmt, ...) { \
	va_list args; \
    va_start (args, fmt); \
	jm_log_v(cb, module, jm_log_level_ ## log_level, fmt, args); \
    va_end (args); \
} \
	void jm_log_ ## log_level ## _v(jm_callbacks* cb, const char* module, const char* fmt, va_list ap) { \
    jm_log_v(cb, module, jm_log_level_ ## log_level, fmt, ap); \
}

CREATE_LOG_FUNCTIONS(fatal)
CREATE_LOG_FUNCTIONS(error)
CREATE_LOG_FUNCTIONS(warning)
CREATE_LOG_FUNCTIONS(info)
CREATE_LOG_FUNCTIONS(verbose)
#ifdef FMILIB_ENABLE_LOG_LEVEL_DEBUG
CREATE_LOG_FUNCTIONS(debug)
#endif


jm_callbacks jm_standard_callbacks;

jm_callbacks* jm_standard_callbacks_ptr = 0;

jm_callbacks* jm_default_callbacks = 0;

void jm_set_default_callbacks(jm_callbacks* c) {
	if(c)
		jm_default_callbacks = c;
	else
		jm_default_callbacks = jm_standard_callbacks_ptr;
}

jm_callbacks* jm_get_default_callbacks() { 
	if(!jm_default_callbacks) {
		if(!jm_standard_callbacks_ptr) {
			jm_standard_callbacks.calloc = calloc;
			jm_standard_callbacks.malloc = malloc;
			jm_standard_callbacks.realloc = realloc;
			jm_standard_callbacks.free = free;
			jm_standard_callbacks.logger = jm_default_logger;
			jm_standard_callbacks.log_level = jm_log_level_info;
			jm_standard_callbacks.context = 0;
			jm_standard_callbacks.errMessageBuffer[0] = 0;

			jm_standard_callbacks_ptr = &jm_standard_callbacks;
		}
		jm_default_callbacks = jm_standard_callbacks_ptr;
	}
	return jm_default_callbacks; 
}
