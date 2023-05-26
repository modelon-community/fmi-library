#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#include "JM/jm_vector.h"

void fmi_testutil_build_xml_path(char* buf, size_t bufSize, const char* basePath, const char* appendPath) {
    strncpy(buf, basePath,   bufSize);
    strncat(buf, appendPath, bufSize);
}

/**
 * This function is called before a test fails via macro, so you can put a
 * breakpoint on it to make debugging easier
 */
void fmi_testutil_enter_breakpoint()
{
    /* You can put a breakpoint on this line to break on test failure. */
}

fmi3_import_t* fmi3_testutil_parse_xml(const char* xmldir) {
    jm_callbacks* cb = jm_get_default_callbacks();
    fmi_import_context_t* ctx = fmi_import_allocate_context(cb);
    if (!ctx) {
        printf("testutil: Context was NULL\n");
        return NULL;
    }

    fmi3_import_t* xml;
    xml = fmi3_import_parse_xml(ctx, xmldir, NULL);
    fmi_import_free_context(ctx);
    if (!xml) {
        printf("testutil: Failed to parse XML\n");
        return NULL;
    }

    return xml;
}

static void fmi3_testutil_log_and_save(jm_callbacks* cb, jm_string module, jm_log_level_enu_t log_level,
        jm_string message)
{
    // Write log to stderr:
    jm_default_logger(cb, module, log_level, message);
    
    // And also save it:
    const char* logLevelStr = jm_log_level_to_string(log_level);
    size_t msgLen = strlen(logLevelStr) + strlen(module) + strlen(message) + 20;  // Some extra for '[', spaces etc

    char* msg = cb->malloc(msgLen);  
    if (!msg) {
        printf("Test failure: Could not allocate memory");
        return;
    }

    jm_snprintf(msg, msgLen, "[%s][%s] %s\n", logLevelStr, module, message);
    fmi3_testutil_import_t* testfmu = cb->context;
    jm_vector_push_back(jm_voidp)(&testfmu->log, msg);
    if (log_level == jm_log_level_error || log_level == jm_log_level_fatal) {
        jm_vector_push_back(jm_voidp)(&testfmu->errLog, msg);
    } else if (log_level == jm_log_level_warning) {
        jm_vector_push_back(jm_voidp)(&testfmu->warnLog, msg);
    }
}

fmi3_testutil_import_t* fmi3_testutil_parse_xml_with_log(const char* xmldir) {
    fmi3_testutil_import_t* testfmu = malloc(sizeof(fmi3_testutil_import_t));
    if (!testfmu) {
        printf("Test failure: Could not allocate memory");
        return NULL;
    }
    testfmu->cb.calloc    = calloc;
    testfmu->cb.malloc    = malloc;
    testfmu->cb.realloc   = realloc;
    testfmu->cb.free      = free;
    testfmu->cb.logger    = fmi3_testutil_log_and_save;
    testfmu->cb.log_level = jm_log_level_info;
    testfmu->cb.context   = testfmu;
    jm_vector_init(jm_voidp)(&testfmu->log,     0, 0);
    jm_vector_init(jm_voidp)(&testfmu->warnLog, 0, 0);
    jm_vector_init(jm_voidp)(&testfmu->errLog,  0, 0);
    testfmu->fmu = NULL;

    fmi_import_context_t* ctx = fmi_import_allocate_context(&testfmu->cb);
    if (!ctx) {
        printf("testutil: Context was NULL\n");
        return testfmu;
    }

    fmi3_import_t* xml;
    xml = fmi3_import_parse_xml(ctx, xmldir, NULL);
    fmi_import_free_context(ctx);
    if (!xml) {
        printf("testutil: Failed to parse XML\n");
        return testfmu;
    }
    testfmu->fmu = xml;

    return testfmu;
}

void fmi3_testutil_import_free(fmi3_testutil_import_t* testfmu) {
    jm_callbacks* cb = jm_get_default_callbacks();

    // Free logs:
    jm_vector_foreach(jm_voidp)(&testfmu->log, cb->free);  // NOTE: 'log' owns all the log message memory
    jm_vector_free_data(jm_voidp)(&testfmu->log);
    jm_vector_free_data(jm_voidp)(&testfmu->warnLog);
    jm_vector_free_data(jm_voidp)(&testfmu->errLog);
    
    // Free the fmi3_import_t:
    if (testfmu->fmu) {
        fmi3_import_free(testfmu->fmu);
    }
    
    free(testfmu);  // Allocated without cb, so also freeing without.
}

size_t fmi3_testutil_get_num_errors(fmi3_testutil_import_t* testfmu) {
    return jm_vector_get_size(jm_voidp)(&testfmu->errLog);
}

size_t fmi3_testutil_get_num_problems(fmi3_testutil_import_t* testfmu) {
    return jm_vector_get_size(jm_voidp)(&testfmu->warnLog)
         + jm_vector_get_size(jm_voidp)(&testfmu->errLog);
}

bool fmi3_testutil_log_contains(fmi3_testutil_import_t* testfmu, const char* msgSubstr) {
    size_t n = jm_vector_get_size(jm_voidp)(&testfmu->log);
    for (size_t i = 0; i < n; i++) {
        const char* logMsg = (const char*)jm_vector_get_item(jm_voidp)(&testfmu->log, i);
        if (strstr(logMsg, msgSubstr)) {
            return true;
        }
    }
    return false;
}

void fail(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("Test failure: ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);

    exit(CTEST_RETURN_FAIL);
}

char* concat(char *s1, char *s2) {
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    /* +1 for the zero-terminator */
    char *result = (char *) malloc((len1 + len2 + 1) * sizeof(char));
    if (result == NULL) {
        exit(CTEST_RETURN_FAIL);
    }
    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1); /* +1 to copy the null-terminator */
    return result;
}