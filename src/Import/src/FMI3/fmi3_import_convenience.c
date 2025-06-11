/*
    Copyright (C) 2023 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include <FMI3/fmi3_xml_model_description.h>
#include <FMI3/fmi3_function_types.h>

#include "fmi3_import_impl.h"

/**
    \brief Collect model information by counting the number of variables with specific properties and fillinf in fmi3_import_model_counts_t struct.
    @param fmu - An fmu object as returned by fmi3_import_parse_xml().
    @param counts - a pointer to a preallocated struct.
*/
void fmi3_import_collect_model_counts(fmi3_import_t* fmu, fmi3_import_model_counts_t* counts) {
    jm_vector(jm_voidp)* vars = fmi3_xml_get_variables_original_order(fmu->md);
    size_t nv, i;
    memset(counts, 0, sizeof(fmi3_import_model_counts_t)); // set all values to zero
    if (!vars) return;
    nv = jm_vector_get_size(jm_voidp)(vars);
    for (i = 0; i< nv; i++) {
        fmi3_xml_variable_t* var = (fmi3_xml_variable_t*)jm_vector_get_item(jm_voidp)(vars, i);
        switch (fmi3_xml_get_variable_variability(var)) {
        case fmi3_variability_enu_constant:
            counts->num_constants++;
            break;
        case fmi3_variability_enu_fixed:
            counts->num_fixed++;
            break;
        case fmi3_variability_enu_tunable:
            counts->num_tunable++;
            break;
        case fmi3_variability_enu_discrete:
            counts->num_discrete++;
            break;
        case fmi3_variability_enu_continuous:
            counts->num_continuous++;
            break;
        default:
            assert(0);
        }
        switch (fmi3_xml_get_variable_causality(var)) {
        case fmi3_causality_enu_parameter:
            counts->num_parameters++;
            break;
        case fmi3_causality_enu_calculated_parameter:
            counts->num_calculated_parameters++;
            break;
        case fmi3_causality_enu_input:
            counts->num_inputs++;
            break;
        case fmi3_causality_enu_output:
            counts->num_outputs++;
            break;
        case fmi3_causality_enu_local:
            counts->num_local++;
            break;
        case fmi3_causality_enu_independent:
            counts->num_independent++;
            break;
        case fmi3_causality_enu_structural_parameter:
            counts->num_structural_parameters++;
            break;
        default: assert(0);
        }
        switch(fmi3_xml_get_variable_base_type(var)) {
        case fmi3_base_type_float64:
            counts->num_float64_vars++;
            break;
        case fmi3_base_type_float32:
            counts->num_float32_vars++;
            break;
        case fmi3_base_type_int64:
            counts->num_int64_vars++;
            break;
        case fmi3_base_type_int32:
            counts->num_int32_vars++;
            break;
        case fmi3_base_type_int16:
            counts->num_int16_vars++;
            break;
        case fmi3_base_type_int8:
            counts->num_int8_vars++;
            break;
        case fmi3_base_type_uint64:
            counts->num_uint64_vars++;
            break;
        case fmi3_base_type_uint32:
            counts->num_uint32_vars++;
            break;
        case fmi3_base_type_uint16:
            counts->num_uint16_vars++;
            break;
        case fmi3_base_type_uint8:
            counts->num_uint8_vars++;
            break;
        case fmi3_base_type_bool:
            counts->num_bool_vars++;
            break;
        case fmi3_base_type_str:
            counts->num_string_vars++;
            break;
        case fmi3_base_type_enum:
            counts->num_enum_vars++;
            break;
        case fmi3_base_type_binary:
            counts->num_binary_vars++;
            break;
        case fmi3_base_type_clock:
            counts->num_clock_vars++;
            break;
        default:
            assert(0);
        }
    }
    return;
}

/**
 * Transforms msgIn into msgOut by expanding variable references of the form #<VR># into variable names
 * and by replacing '##' with a single '#'.
 */ 
static void fmi3_import_expand_variable_references_impl(fmi3_import_t* fmu, const char* msgIn){
    jm_vector(char)* msgOut = &fmu->logMessageBufferExpanded;
    fmi3_xml_model_description_t* md = fmu->md;
    jm_callbacks* callbacks = fmu->callbacks;
    char curCh, nextCh;
    size_t i;  // Index of curCh in msgIn
    size_t msgLen = strlen(msgIn)+1; /* original message length including terminating 0 */

    if (jm_vector_reserve(char)(msgOut, msgLen + 100) < msgLen + 100) {
        jm_log(fmu->callbacks,"LOGGER", jm_log_level_warning, "Could not allocate memory for the log message");
        jm_vector_resize(char)(msgOut, 6);
        memcpy(jm_vector_get_itemp(char)(msgOut, 0), "ERROR", 6); /* at least 16 chars are always there */
        return;
    }

    const char* firstHash = strchr(msgIn, '#');
    if (firstHash) {
        // The string contains at least one '#'. Copy everything before it to msgOut:
        size_t nLeading = firstHash - msgIn;
        jm_vector_resize(char)(msgOut, nLeading);
        if (nLeading != 0) {
            memcpy(jm_vector_get_itemp(char)(msgOut, 0), msgIn, nLeading);
        }
        i = nLeading;
    }
    else {
        // The string contains no references. Just copy msgIn to msgOut.
        jm_vector_resize(char)(msgOut, msgLen);
        memcpy(jm_vector_get_itemp(char)(msgOut, 0), msgIn, msgLen);
        return;
    }
    curCh = msgIn[i];
    while (curCh) {
        nextCh = msgIn[i+1];
        if (curCh != '#') {
            jm_vector_push_back(char)(msgOut, curCh);  // Copy in to out
        }
        else if (nextCh == '#') {
            jm_vector_push_back(char)(msgOut, '#');
            i++;  // Skip the second #
        }
        else {
            unsigned int bufVR;
            fmi3_value_reference_t vr;
            size_t pastePos = jm_vector_get_size(char)(msgOut);

            size_t nDigits = 0;
            fmi3_xml_variable_t* var;
            const char* name;
            size_t nameLen;

            curCh = msgIn[++i];
            while (isdigit(curCh)) {
                jm_vector_push_back(char)(msgOut, curCh);
                curCh = msgIn[++i];
                nDigits++;
            }
            jm_vector_push_back(char)(msgOut, 0);
            if (nDigits == 0) {
                jm_log(callbacks,"LOGGER", jm_log_level_warning, "Expected value reference in log message here: '%s'",
                        jm_vector_get_itemp(char)(msgOut,0));
                jm_vector_resize(char)(msgOut, msgLen);
                memcpy(jm_vector_get_itemp(char)(msgOut,0),msgIn,msgLen);
                return;
            }
            else if (curCh != '#') {
                jm_log(callbacks,"LOGGER", jm_log_level_warning, "Expected terminating '#' in log message here: '%s'",
                        jm_vector_get_itemp(char)(msgOut,0));
                jm_vector_resize(char)(msgOut, msgLen);
                memcpy(jm_vector_get_itemp(char)(msgOut,0),msgIn,msgLen);
                return;
            }

            if (sscanf(jm_vector_get_itemp(char)(msgOut, pastePos), "%u",&bufVR) != 1) {
                jm_log(callbacks,"LOGGER", jm_log_level_warning, "Could not decode value reference in log message here: '%s'",
                        jm_vector_get_itemp(char)(msgOut,0));
                jm_vector_resize(char)(msgOut, msgLen);
                memcpy(jm_vector_get_itemp(char)(msgOut,0),msgIn,msgLen);
                return;
            }
            vr = bufVR;
            var = fmi3_xml_get_variable_by_vr(md, vr);
            if (!var) {
                jm_log(callbacks,"LOGGER", jm_log_level_warning, "Could not find variable referenced in log message here: '%s'",
                        jm_vector_get_itemp(char)(msgOut,0));
                jm_vector_resize(char)(msgOut, msgLen);
                memcpy(jm_vector_get_itemp(char)(msgOut,0),msgIn,msgLen);
                return;
            }
            name = fmi3_xml_get_variable_name(var);
            nameLen = strlen(name);
            if (jm_vector_resize(char)(msgOut, pastePos + nameLen) != pastePos + nameLen) {
                jm_log(callbacks,"LOGGER", jm_log_level_warning, "Could not allocate memory for the log message");
                jm_vector_resize(char)(msgOut, msgLen);
                memcpy(jm_vector_get_itemp(char)(msgOut,0),msgIn,msgLen);
                return;
            }
            memcpy(jm_vector_get_itemp(char)(msgOut, pastePos), name, nameLen);
        }
        curCh = msgIn[++i];
    }
    jm_vector_push_back(char)(msgOut, 0);
}

void fmi3_import_expand_variable_references(fmi3_import_t* fmu, const char* msgIn, char* msgOut, size_t maxMsgSize) {
    fmi3_import_expand_variable_references_impl(fmu, msgIn);
    strncpy(msgOut, jm_vector_get_itemp(char)(&fmu->logMessageBufferExpanded,0), maxMsgSize-1);
    msgOut[maxMsgSize - 1] = '\0';
}

void* fmi3_callback_allocate_memory_default(fmi3_instance_environment_t instEnv, size_t nobj, size_t size) {
    return calloc(nobj, size);
}

void fmi3_callback_free_memory_default(fmi3_instance_environment_t instEnv, void* obj) {
    free(obj);
}

void fmi3_log_forwarding(fmi3_instance_environment_t inst, fmi3_status_t status, fmi3_string_t category, fmi3_string_t message) {
#define BUFSIZE JM_MAX_ERROR_MESSAGE_SIZE
    char buffer[BUFSIZE], *buf, *curp, *msg;
    const char* statusStr;
    fmi3_import_t* fmu = (fmi3_import_t*)inst;
    jm_callbacks* cb;
    jm_log_level_enu_t logLevel;
    const char* instanceName;

    if (fmu) {
        cb = fmu->callbacks;
        buf = jm_vector_get_itemp(char)(&fmu->logMessageBufferCoded, 0);
    } else {
        // XXX: Why do even accept that fmu==NULL? What's the point of forwarding
        // the logging to the FMU in that case? Also, it's not documented in the
        // function's interface.
        cb = jm_get_default_callbacks();
        buf = buffer;
    }

    // Convert status to logLevel:
    switch (status) {
        case fmi3_status_discard:
        case fmi3_status_ok:
            logLevel = jm_log_level_info;
            break;
        case fmi3_status_warning:
            logLevel = jm_log_level_warning;
            break;
        case fmi3_status_error:
            logLevel = jm_log_level_error;
            break;
        case fmi3_status_fatal:
        default:
            logLevel = jm_log_level_fatal;
    }
    if (logLevel > cb->log_level) return;

    curp = buf;
    *curp = 0;

    // Add category:
    if (category) {
        curp += jm_snprintf(curp, 100, "[%s]", category);
    }
    
    // Add status:
    statusStr = fmi3_status_to_string(status);
    curp += jm_snprintf(curp, 200, "[FMU status:%s] ", statusStr);

    // Add message:
    if (fmu) {
        int bufsize = jm_vector_get_size(char)(&fmu->logMessageBufferCoded);
        int len;
        len = jm_snprintf(curp, bufsize -(curp-buf), message);
        if (len > (bufsize -(curp-buf+1))) {
            int offset = (curp-buf);
            len = jm_vector_resize(char)(&fmu->logMessageBufferCoded, len + offset + 1) - offset;
            buf = jm_vector_get_itemp(char)(&fmu->logMessageBufferCoded, 0);
            curp = buf + offset;
        }
        fmi3_import_expand_variable_references(fmu, buf, cb->errMessageBuffer, JM_MAX_ERROR_MESSAGE_SIZE);
        msg = jm_vector_get_itemp(char)(&fmu->logMessageBufferExpanded,0);
    }
    else {
        jm_snprintf(curp, BUFSIZE -(curp-buf), message);
        strncpy(cb->errMessageBuffer, buf, JM_MAX_ERROR_MESSAGE_SIZE);
        cb->errMessageBuffer[JM_MAX_ERROR_MESSAGE_SIZE - 1] = '\0';
        msg = cb->errMessageBuffer;
    }
    
    // Set instanceName
    if (fmu) {
        if (fmu->instanceName) {
            instanceName = fmu->instanceName;
        } else {
            instanceName = "<fmu-instance-name>";  // Called before instantiation. Should only happen in testing.
        }
    } else {
        // XXX: Doesn't make sense to not have an FMU... So using arbitrary value:
        instanceName = "<module>";
    }
    
    // Forward the call to the logger used during FMU parsing:
    if (cb->logger) {
        cb->logger(cb, instanceName, logLevel, msg);
    }
}

void fmi3_default_callback_logger(fmi3_instance_environment_t c, fmi3_string_t instanceName, fmi3_status_t status, fmi3_string_t category, fmi3_string_t message) {
    char buf[BUFSIZE], *curp;
    curp = buf;
    *curp = 0;
    if (instanceName) {
        curp += jm_snprintf(curp, 200, "[%s]", instanceName);
    }
    if (category) {
        curp += jm_snprintf(curp, 200, "[%s]", category);
    }
    fprintf(stdout, "%s[status=%s]", buf, fmi3_status_to_string(status));
    fprintf(stdout, "\n");
}

void fmi3_logger(jm_callbacks* cb, jm_string module, jm_log_level_enu_t log_level, jm_string message) {
    fmi3_logger_context_t* ctx = (fmi3_logger_context_t*)cb->context;
    fmi3_status_t status;

    if (!ctx || !ctx->logMessage) return;

    if (log_level > jm_log_level_all) {
        assert(0);
        status = fmi3_status_error;
    }
    else if (log_level >= jm_log_level_info)
        status = fmi3_status_ok;
    else if (log_level >= jm_log_level_warning)
        status = fmi3_status_warning;
    else if (log_level >= jm_log_level_error)
        status = fmi3_status_error;
    else if (log_level >= jm_log_level_fatal)
        status = fmi3_status_fatal;
    else {
        status = fmi3_status_ok;
    }

    ctx->logMessage(ctx->instanceEnvironment,
        status,
        (fmi3_string_t)jm_log_level_to_string(log_level),
        message);
}

void fmi3_import_init_logger(jm_callbacks* cb, fmi3_logger_context_t* loggerCallbacks) {
    cb->logger = fmi3_logger;
    cb->context = loggerCallbacks;
}

const char* fmi3_import_get_variable_description_by_name(fmi3_import_t* fmu, const char* name) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(fmu, name);
    if (v == NULL) {
        return NULL;
    }
    if (strcmp(name, fmi3_xml_get_variable_name(v)) == 0) {
        return fmi3_xml_get_variable_description(v);
    }
    fmi3_xml_alias_variable_t* alias_var = fmi3_xml_get_alias_variable_by_name(v, name);
    return fmi3_xml_get_alias_variable_description(alias_var);
}

fmi3_import_display_unit_t* fmi3_import_get_variable_display_unit_by_name(fmi3_import_t* fmu, const char* name) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(fmu, name);
    if (v == NULL) {
        return NULL;
    }
    if (strcmp(name, fmi3_xml_get_variable_name(v)) == 0) { /* no alias */
        fmi3_xml_float32_variable_t* v32 = fmi3_xml_get_variable_as_float32(v);
        if (v32) {
            return fmi3_xml_get_float32_variable_display_unit(v32);
        }
        fmi3_xml_float64_variable_t* v64 = fmi3_xml_get_variable_as_float64(v);
        if (v64) {
            return fmi3_xml_get_float64_variable_display_unit(v64);
        }
        return NULL; /* non-float variable */
    } else { /* alias */
        fmi3_xml_alias_variable_t* alias_var = fmi3_xml_get_alias_variable_by_name(v, name);
        if (!alias_var) {
            return NULL; /* safety*/
        }
        return fmi3_xml_get_alias_variable_display_unit(alias_var);
    }
}

fmi3_boolean_t fmi3_import_get_variable_is_clocked_by(fmi3_import_variable_t* var, fmi3_import_clock_variable_t* clock) {
    return fmi3_xml_get_variable_is_clocked_by(var, clock);
}
