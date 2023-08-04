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

#include "config_test.h"

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"
#include <JM/jm_portability.h>

#include "catch.hpp"

#define BUFFER 1000

/* globals */
fmi3_boolean_t G_dummy_lock_preemption_callback_called;
fmi3_boolean_t G_dummy_unlock_preemption_callback_called;
static int g_dummy_clock_callbacks;

typedef struct {
    fmi3_boolean_t clock_update_callback_called; /* Boolean used to test the call to fmi3ClockUpdateCallback */
    fmi3_boolean_t log_message_callback_called;  /* Boolean used to test the call to fmi3LogMessageCallback  */
} dummy_fmi3_instance_environment_t;

void importlogger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message)
{
        printf("module = %s, log level = %s: %s\n", module, jm_log_level_to_string(log_level), message);
}

void dummy_log_message_callback(
    fmi3_instance_environment_t env,
    fmi3_status_t status,
    fmi3_string_t category,
    fmi3_string_t message)
{
    ((dummy_fmi3_instance_environment_t*)env)->log_message_callback_called = fmi3_true;
}

void dummy_clock_update_callback() {
    g_dummy_clock_callbacks++;
}

/* function implementing fmi3_lock_preemption_callback_ft */
void dummy_lock_preemption_callback() {
    G_dummy_lock_preemption_callback_called = fmi3_true;
}

/* function implementing fmi3_unlock_preemption_callback_ft */
void dummy_unlock_preemption_callback() {
    G_dummy_unlock_preemption_callback_called = fmi3_true;
}

void test_capi_wrappers_se(fmi3_import_t* fmu) {
    jm_status_enu_t jmstatus;

    fmi3_string_t instanceName = "Test SE model instance";
    fmi3_string_t resourcePath = "";
    fmi3_boolean_t visible = fmi3_false;
    fmi3_boolean_t loggingOn = fmi3_false;
    dummy_fmi3_instance_environment_t instance_env;
    instance_env.clock_update_callback_called = fmi3_false;
    instance_env.log_message_callback_called = fmi3_false;

    g_dummy_clock_callbacks = 0;

    /* reset value that will be set by intermedate update callback */
    G_dummy_lock_preemption_callback_called     = fmi3_false;
    G_dummy_unlock_preemption_callback_called   = fmi3_false;

    REQUIRE_STREQ(fmi3_import_get_version(fmu), "3.0");
    REQUIRE_STREQ(fmi3_import_get_instantiation_token(fmu), "123");

    INFO("Instantiate");
    jmstatus = fmi3_import_instantiate_scheduled_execution(
        fmu,
        instanceName,
        resourcePath,
        visible,
        loggingOn,
        ((fmi3_instance_environment_t)&instance_env),
        &dummy_log_message_callback,
        &dummy_clock_update_callback,
        &dummy_lock_preemption_callback,
        &dummy_unlock_preemption_callback
    );
    REQUIRE(jmstatus == jm_status_success);

    INFO("Verify callbacks");
    REQUIRE(instance_env.log_message_callback_called == fmi3_true);

    REQUIRE(g_dummy_clock_callbacks > 0);
    REQUIRE(G_dummy_lock_preemption_callback_called == fmi3_true);
    REQUIRE(G_dummy_unlock_preemption_callback_called == fmi3_true);

    /* verify that the CAPI wrappers are loaded and have matching signatures */
    REQUIRE(fmi3_import_activate_model_partition(fmu, 0, 0) == fmi3_status_ok);

    /* clean up */
    REQUIRE(fmi3_import_terminate(fmu) == fmi3_status_ok);
    fmi3_import_free_instance(fmu);
}

/*****************************************
 *** instanceEnvironment and callbacks ***
 *****************************************/

/* Structure and callbacks for counting number of allocs/frees, so we can verify that they are equal in number */

typedef struct {
    fmi3_import_t* fmu; /* fmi3_log_forwarding is based on the fmu instance */
} fmi3_inst_env_count_calls_t;

void fmi3_test_log_forwarding_wrap(fmi3_instance_environment_t instEnv, fmi3_status_t status,
        fmi3_string_t category, fmi3_string_t message) {
    fmi3_log_forwarding(((fmi3_inst_env_count_calls_t*)instEnv)->fmu, status, category, message);
}

/************
 *** main ***
 ************/

TEST_CASE("main") {
    fmi3_inst_env_count_calls_t instEnv;
    const char* tmpPath;
    jm_callbacks callbacks;
    fmi_import_context_t* context;
    fmi_version_enu_t version;

    callbacks.malloc = malloc;
    callbacks.calloc = calloc;
    callbacks.realloc = realloc;
    callbacks.free = free;
    callbacks.logger = importlogger;
    callbacks.log_level = jm_log_level_debug;
    callbacks.context = 0;

#ifdef FMILIB_GENERATE_BUILD_STAMP
    printf("Library build stamp:\n%s\n", fmilib_get_build_stamp());
#endif

    tmpPath = fmi_import_mk_temp_dir(&callbacks, FMU_UNPACK_DIR, NULL);
    REQUIRE(tmpPath != nullptr);

    context = fmi_import_allocate_context(&callbacks);
    REQUIRE(context != nullptr);
    REQUIRE(fmi_import_get_fmi_version(context, FMU3_SE_PATH, tmpPath) == fmi_version_3_0_enu);

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(tmpPath);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);

    REQUIRE((fmi3_import_get_fmu_kind(fmu) & fmi3_fmu_kind_se) == fmi3_fmu_kind_se);

    /* Test custom callbacks - .._me_test tests default */
    instEnv.fmu = fmu;
    REQUIRE(fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_se, &instEnv, fmi3_test_log_forwarding_wrap) == jm_status_success);

    test_capi_wrappers_se(fmu);

    /* TODO: add simulation */

    fmi3_import_destroy_dllfmu(fmu);

    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 0);
    fmi3_testutil_import_free(tfmu);

    fmi_import_free_context(context);
    REQUIRE(fmi_import_rmdir(&callbacks, tmpPath) == jm_status_success);
    callbacks.free((void*)tmpPath);
}