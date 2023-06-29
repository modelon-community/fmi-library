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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#include "catch.hpp"

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

/**
 * Tests that memory management for options is working when creating dllfmu and
 * freeing it multiple times.
 */
static void test_option_memory_management(fmi3_import_t* fmu) {
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
    opts = fmi3_import_get_options(fmu);
    fmi_import_set_option_loadlibrary_flag(opts, flag);

    /* Test after creating dllfmu */
    status = fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_me, nullptr, nullptr);
    REQUIRE(status == jm_status_success);

    opts2 = fmi3_import_get_options(fmu);
    REQUIRE(opts == opts2);
    fmi_import_set_option_loadlibrary_flag(opts, flag);

    /* Test with dllfmu destroyed */
    fmi3_import_destroy_dllfmu(fmu);

    opts2 = fmi3_import_get_options(fmu);
    REQUIRE(opts == opts2);
    fmi_import_set_option_loadlibrary_flag(opts, flag);

    /* Test after creating new dllfmu */
    status = fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_me, nullptr, nullptr);
    REQUIRE(status == jm_status_success);

    opts2 = fmi3_import_get_options(fmu);
    REQUIRE(opts == opts2);
    fmi_import_set_option_loadlibrary_flag(opts, flag);

    fmi3_import_destroy_dllfmu(fmu);
}

/**
 * Tests that the option has an effect.
 */
static void test_loadlibrary_flag(fmi3_import_t* fmu) {
    fmi_import_options_t* opts;
    jm_status_enu_t status;

    opts = fmi3_import_get_options(fmu);

    /* This is a bit hard to test, so mainly trying to see that setting the
     * option has some effect */
#ifdef WIN32

    /* Expect failure because we haven't signed the dll */
    fmi_import_set_option_loadlibrary_flag(opts, LOAD_LIBRARY_REQUIRE_SIGNED_TARGET);
    status = fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_me, nullptr, nullptr);
    REQUIRE(status == jm_status_error);
    fmi3_import_destroy_dllfmu(fmu);

    /* Expect success because ALTERED_SEARCH_PATH should not matter in this case. */
    fmi_import_set_option_loadlibrary_flag(opts, LOAD_WITH_ALTERED_SEARCH_PATH);
    status = fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_me, nullptr, nullptr);
    REQUIRE(status == jm_status_success);
    fmi3_import_destroy_dllfmu(fmu);
#else
    /* Expect failure because library should not get loaded */
    fmi_import_set_option_loadlibrary_flag(opts, RTLD_NOW | RTLD_NOLOAD);
    status = fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_me, nullptr, nullptr);
    REQUIRE(status == jm_status_error);
    fmi3_import_destroy_dllfmu(fmu);
#endif
}

TEST_CASE("Import option testing") {
    const char* tmpPath;
    jm_callbacks callbacks;
    fmi_import_context_t* context;
    fmi_version_enu_t version;

    fmi3_import_t* fmu;    

    callbacks.malloc = malloc;
    callbacks.calloc = calloc;
    callbacks.realloc = realloc;
    callbacks.free = free;
    callbacks.logger = jm_default_logger;
    callbacks.log_level = jm_log_level_debug;
    callbacks.context = nullptr;

#ifdef FMILIB_GENERATE_BUILD_STAMP
    printf("Library build stamp:\n%s\n", fmilib_get_build_stamp());
#endif

    tmpPath = fmi_import_mk_temp_dir(&callbacks, FMU_UNPACK_DIR, nullptr);
    REQUIRE(tmpPath != nullptr);

    context = fmi_import_allocate_context(&callbacks);
    version = fmi_import_get_fmi_version(context, FMU3_ME_PATH, tmpPath);
    REQUIRE(version == fmi_version_3_0_enu);

    fmu = fmi3_import_parse_xml(context, tmpPath, nullptr);
    REQUIRE(fmu != nullptr);

    /* Tests (they will exit early on failure): */
    test_option_memory_management(fmu);
    test_loadlibrary_flag(fmu);

    /* Clean up: */
    fmi3_import_free(fmu);
    fmi_import_free_context(context);
    REQUIRE(fmi_import_rmdir(&callbacks, tmpPath) == jm_status_success);
    callbacks.free((void*)tmpPath);
}
