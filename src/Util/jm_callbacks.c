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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "jm_callbacks.h"

void jm_default_logger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message) {
	switch(log_level) {
	case jm_log_level_all:
	case jm_log_level_info:
		fprintf(stdout, "[INFO][%s]%s\n",module, message);
	case jm_log_level_warning:
		fprintf(stdout, "[WARNING][%s]%s\n",module, message);
	default:
		fprintf(stdout, "[ERROR][%s]%s\n",module, message);
	}
}

void jm_log(jm_callbacks* cb, const char* module, jm_log_level_enu_t log_level, const char* fmt, ...) {
	va_list args;
    va_start (args, fmt);
    jm_log_v(cb, module, log_level, fmt, args);
    va_end (args);
}

void jm_log_v(jm_callbacks* cb, const char* module, jm_log_level_enu_t log_level, const char* fmt, va_list ap) {
	if(log_level < cb->log_level) return;
    vsprintf(cb->errMessageBuffer, fmt, ap);
	if(cb->logger) {
		cb->logger(cb,module, log_level, cb->errMessageBuffer);
	}
}

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
