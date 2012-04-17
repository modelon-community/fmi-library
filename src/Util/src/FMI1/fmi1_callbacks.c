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
#include <limits.h>
#include <string.h>
#include <assert.h>

#include <FMI1/fmi1_callbacks.h>

#ifdef FMU_REPLACE_SYSTEM_MALLOC
#define fmi1_malloc malloc
#define fmi1_calloc calloc
#define fmi1_realloc realloc
#define fmi1_free free
#endif

void *fmi1_malloc(size_t size) {
    fmi1_memory_header_t* header;
    fmi1_callbacks_t* fmi_xml_cb;
    jm_callbacks* jmCB = jm_get_default_callbacks();
    fmi_xml_cb = jmCB->context;

    header = fmi_xml_cb->fmiFunctions.allocateMemory(1, size + sizeof(fmi1_memory_header_t) );
    if(!header) return 0;
    header->callbacks = fmi_xml_cb;
    header->size = size;
    header++;
    return header;
}

void *fmi1_calloc(size_t nelements, size_t elementSize){
    /* Use fmi1_malloc since header needs to be allocated and its in most cases != elementSize.
       Alternative might be to allocated nelements + (sizeof(fmi_xml_memory_header)+elementSize-1)/elementSize
       and then align on elementSize. This would complicate realloc and free.
       Formally there should always be a check that (nelements * elementSize) can be represented with size_t.
       For now just put an assert on that.
    */
    assert(!nelements || !elementSize || ((ULONG_MAX/elementSize) > (nelements +(sizeof(fmi1_memory_header_t)+elementSize-1)/elementSize)));
    return fmi1_malloc(nelements * elementSize);
}

/* fmifree gets the context pointer directly from the memory header. */
void fmi1_free ( void * ptr ){
    fmi1_memory_header_t* header = ptr;
    header--;
    header->callbacks->fmiFunctions.freeMemory(header);
}

void *fmi1_realloc(void *pointer, size_t size) {
    fmi1_memory_header_t* header;

    void* newmem;
    size_t copysize;

    /* handle special cases first*/
    if(!pointer) { /* equivalent to malloc */
        return fmi1_malloc(size);
    }

    header = pointer;
    header--;
    if(header->size == size) return pointer; /* no size change */

	if(size == 0) { /* equivalent to free */
        header->callbacks->jmFunctions.free(pointer);
        return 0;
    }

    newmem = header->callbacks->jmFunctions.malloc(size);
    if(!newmem) return 0; /* malloc failed */
    copysize = (size < header->size) ? size : header->size;
    memcpy(newmem, pointer, copysize);
    header->callbacks->jmFunctions.free(pointer);
    return newmem;
}

void fmi1_logger(jm_callbacks* cb, jm_string module, jm_log_level_enu_t log_level, jm_string message);

void fmi1_export_init_callbacks(fmi1_callbacks_t* callbacks, fmi1_callback_functions_t* fmiFunctions) {
    callbacks->jmFunctions.malloc = fmi1_malloc;
    callbacks->jmFunctions.calloc = fmi1_calloc;
    callbacks->jmFunctions.realloc = fmi1_realloc;
    callbacks->jmFunctions.free = fmi1_free;
    callbacks->jmFunctions.logger = fmi1_logger;
    callbacks->jmFunctions.context = callbacks;

    callbacks->fmiFunctions.logger = fmiFunctions->logger;
    callbacks->fmiFunctions.allocateMemory = fmiFunctions->allocateMemory;
    callbacks->fmiFunctions.freeMemory = fmiFunctions->freeMemory;

    jm_set_default_callbacks(&callbacks->jmFunctions);
}

void  fmi1_default_callback_logger(fmi1_component_t c, fmi1_string_t instanceName, fmi1_status_t status, fmi1_string_t category, fmi1_string_t message, ...) {
    va_list args;
    char buf[500], *curp;
    va_start (args, message);
    curp = buf;
    *curp = 0;
    if(instanceName) {
        sprintf(curp, "[%s]", instanceName);
        curp += strlen(instanceName)+2;
    }
    if(category) {
        sprintf(curp, "[%s]", category);
        curp += strlen(category)+2;
    }
    fprintf(stdout, "%s[status=%s]", buf, fmi1_status_to_string(status));
    vfprintf (stdout, message, args);
    fprintf(stdout, "\n");
    va_end (args);
}

void fmi1_logger(jm_callbacks* cb, jm_string module, jm_log_level_enu_t log_level, jm_string message) {
	fmi1_callbacks_t* c = (fmi1_callbacks_t*)cb->context;
	if(!c ||!c->fmiFunctions.logger) return;
	switch(log_level) {
	case jm_log_level_all:
	case jm_log_level_info:
		c->fmiFunctions.logger( c, module, fmi1_status_ok, "INFO", message);
		break;
	case jm_log_level_warning:
		c->fmiFunctions.logger( c, module, fmi1_status_warning, "WARNING", message);
		break;
	default:
		c->fmiFunctions.logger( c, module, fmi1_status_error, "ERROR", message);
	}
}

void fmi1_import_init_logger(jm_callbacks* cb, fmi1_callbacks_t* context) {
	cb->logger = fmi1_logger;
	cb->context = context;
}
