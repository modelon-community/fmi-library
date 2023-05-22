/*
    Copyright (C) 2021 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "config_test.h"
#include "fmilib.h"

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#define BUFFER 1000

#define ASSERT_STATUS(EXPECTED_STATUS, ACTUAL_STATUS, FUNCTION_NAME) \
    if (EXPECTED_STATUS != ACTUAL_STATUS) { \
        printf("Function gave unexpected status: " FUNCTION_NAME "\n"); \
        do_exit(CTEST_RETURN_FAIL); \
    }

#define ASSERT_EQUALS(A, B, MESSAGE) \
    if (A != B) { \
        printf(MESSAGE "\n"); \
        do_exit(CTEST_RETURN_FAIL); \
    }

void do_exit(int code)
{
    printf("Press 'Enter' to exit\n");
    /* getchar(); */
    exit(code);
}

/**
 * Tests that memory management for options is working when creating dllfmu and
 * freeing it multiple times.
 */
void test_option_memory_management(fmi2_import_t* fmu, fmi2_callback_functions_t* cbf)
{
    fmi_import_options_t* opts;
    fmi_import_options_t* opts2;
    jm_status_enu_t status;
    jm_portability_loadlibrary_flag_t flag;

#ifdef WIN32
    flag = 0;
#else
    flag = RTLD_NOW | RTLD_LOCAL;
#endif

    /* Test get+set of option before dllfmu */
    opts = fmi2_import_get_options(fmu);
    fmi_import_set_option_loadlibrary_flag(opts, flag);

    /* Test after creating dllfmu */
    status = fmi2_import_create_dllfmu(fmu, fmi2_fmu_kind_me, cbf);
    ASSERT_STATUS(jm_status_success, status, "fmi2_import_create_dllfmu");

    opts2 = fmi2_import_get_options(fmu);
    ASSERT_EQUALS(opts, opts2, "A different options object was returned");
    fmi_import_set_option_loadlibrary_flag(opts, flag);

    /* Test with dllfmu destroyed */
    fmi2_import_destroy_dllfmu(fmu);

    opts2 = fmi2_import_get_options(fmu);
    ASSERT_EQUALS(opts, opts2, "A different options object was returned");
    fmi_import_set_option_loadlibrary_flag(opts, flag);

    /* Test after creating new dllfmu */
    status = fmi2_import_create_dllfmu(fmu, fmi2_fmu_kind_me, cbf);
    ASSERT_STATUS(jm_status_success, status, "fmi2_import_create_dllfmu");

    opts2 = fmi2_import_get_options(fmu);
    ASSERT_EQUALS(opts, opts2, "A different options object was returned");
    fmi_import_set_option_loadlibrary_flag(opts, flag);

    fmi2_import_destroy_dllfmu(fmu);
}

/**
 * Tests that the option has an effect.
 */
void test_loadlibrary_flag(fmi2_import_t* fmu, fmi2_callback_functions_t* cbf)
{
    fmi_import_options_t* opts;
    jm_status_enu_t status;

    opts = fmi2_import_get_options(fmu);

    /* This is a bit hard to test, so mainly trying to see that setting the
     * option has some effect */
#ifdef WIN32

    /* Expect failure because we haven't signed the dll */
    fmi_import_set_option_loadlibrary_flag(opts, LOAD_LIBRARY_REQUIRE_SIGNED_TARGET);
    status = fmi2_import_create_dllfmu(fmu, fmi2_fmu_kind_me, cbf);
    ASSERT_STATUS(jm_status_error, status, "fmi2_import_create_dllfmu");
    fmi2_import_destroy_dllfmu(fmu);

    /* Expect success because ALTERED_SEARCH_PATH should not matter in this case. */
    fmi_import_set_option_loadlibrary_flag(opts, LOAD_WITH_ALTERED_SEARCH_PATH);
    status = fmi2_import_create_dllfmu(fmu, fmi2_fmu_kind_me, cbf);
    ASSERT_STATUS(jm_status_success, status, "fmi2_import_create_dllfmu");
    fmi2_import_destroy_dllfmu(fmu);
#else
    /* Expect failure because library should not get loaded */
    fmi_import_set_option_loadlibrary_flag(opts, RTLD_NOW | RTLD_NOLOAD);
    status = fmi2_import_create_dllfmu(fmu, fmi2_fmu_kind_me, cbf);
    ASSERT_STATUS(jm_status_error, status, "fmi2_import_create_dllfmu");
    fmi2_import_destroy_dllfmu(fmu);
#endif
}

int main(int argc, char *argv[])
{
    fmi2_callback_functions_t callBackFunctions;
    const char* tmpPath;
    jm_callbacks callbacks;
    fmi_import_context_t* context;
    fmi_version_enu_t version;

    fmi2_import_t* fmu;    

    callbacks.malloc = malloc;
    callbacks.calloc = calloc;
    callbacks.realloc = realloc;
    callbacks.free = free;
    callbacks.logger = jm_default_logger;
    callbacks.log_level = jm_log_level_debug;
    callbacks.context = NULL;

#ifdef FMILIB_GENERATE_BUILD_STAMP
    printf("Library build stamp:\n%s\n", fmilib_get_build_stamp());
#endif

    tmpPath = fmi_import_mk_temp_dir(&callbacks, FMU_TEMPORARY_TEST_DIR, NULL);
    if (!tmpPath) {
        printf("Failed to create temporary directory in: " FMU_TEMPORARY_TEST_DIR "\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    context = fmi_import_allocate_context(&callbacks);
    version = fmi_import_get_fmi_version(context, FMU2_ME_PATH, tmpPath);
    if (version != fmi_version_2_0_enu) {
        printf("Only version 2.0 is supported by this code\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    fmu = fmi2_import_parse_xml(context, tmpPath, NULL);

    if (!fmu) {
        printf("Error parsing XML. Exiting.\n");
        do_exit(CTEST_RETURN_FAIL);
    }    

    callBackFunctions.logger = fmi2_log_forwarding;
    callBackFunctions.allocateMemory = calloc;
    callBackFunctions.freeMemory = free;
    callBackFunctions.componentEnvironment = fmu;

    /* Tests (they will exit early on failure): */
    test_option_memory_management(fmu, &callBackFunctions);
    test_loadlibrary_flag(fmu, &callBackFunctions);

    /* Clean up: */
    fmi2_import_free(fmu);
    fmi_import_free_context(context);
    tmpPath = fmi_import_mk_temp_dir(&callbacks, FMU_TEMPORARY_TEST_DIR, NULL);
    if (!tmpPath) {
        printf("Failed to create temporary directory in: " FMU_TEMPORARY_TEST_DIR "\n");
        do_exit(CTEST_RETURN_FAIL);
    }
    
    printf("Everything seems to be OK since you got this far=)!\n");
    return 0;
}