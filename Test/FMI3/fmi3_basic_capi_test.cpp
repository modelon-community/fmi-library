/*
    Copyright (C) 2023 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "config_test.h"
#include "fmilib.h"
#include "FMI3/fmi3_import_priv.h"
#include "FMI3/fmi3_capi.h"
#include "FMI3/fmi3PlatformTypes.h"

typedef struct {
    fmi3_import_t* fmu; /* fmi3_log_forwarding is based on the fmu instance */
} fmi3_inst_env_t;


/* The following dummy types/functions are used only for testing in order to be able
    invoke the necessary functions, for example FMU instantiation.
*/
void fmi3_dummy_log_callback(fmi3_instance_environment_t instEnv, fmi3_status_t status,
        fmi3_string_t category, fmi3_string_t message) {
    fmi3_log_forwarding(((fmi3_inst_env_t*)instEnv)->fmu, status, category, message);
}
void dummy_log_message_callback(
    fmi3_instance_environment_t* env,
    fmi3_status_t status,
    fmi3_string_t category,
    fmi3_string_t message) {
    /* Empty by design */
}
void dummy_clock_update_callback(fmi3_instance_environment_t* env) { /* Empty by design */}
void dummy_lock_preemption_callback() { /* Empty by design */}
void dummy_unlock_preemption_callback() { /* Empty by design */}

typedef struct {
    /* Empty by design */
} dummy_fmi3_instance_environment_t;

/* The logger function used for the logger callback struct. */
void importlogger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message)
{
    printf("module = %s, log level = %s: %s\n", module, jm_log_level_to_string(log_level), message);
}

/* The test file name, displayed during the testing */
static const char test_file_name[] = "[fmi3_basic_capi_test.cpp]";

/* Function used only to deallocate resources that were allocated during the testing.  */
void clean_up(fmi3_import_t* fmu, fmi_import_context_t* context, fmi3_inst_env_t inst_env) {
    fmi3_import_free_instance(fmu);
    fmi3_import_free(fmu);
    fmi_import_free_context(context);
    inst_env.fmu = NULL;
}

TEST_CASE( "Test CAPI methods using a Model Exchange FMU", test_file_name)
{
    jm_callbacks callbacks;
    fmi_import_context_t* context;
    fmi3_inst_env_t inst_env;
    callbacks.malloc = malloc;
    callbacks.calloc = calloc;
    callbacks.realloc = realloc;
    callbacks.free = free;
    callbacks.logger = importlogger;
    callbacks.log_level = jm_log_level_debug;
    callbacks.context = 0;
    context = fmi_import_allocate_context(&callbacks);
    fmi_version_enu_t version = fmi_import_get_fmi_version(context, FMU3_ME_PATH, FMU_TEMPORARY_TEST_DIR);
    fmi3_import_t* fmu = fmi3_import_parse_xml(context, FMU_TEMPORARY_TEST_DIR, 0);
    inst_env.fmu = fmu;

    SECTION( "Verifying returned result from fmi_import_get_fmi_version" ) {
        REQUIRE(version == fmi_version_3_0_enu);
    }

    SECTION( "Verifying FMU kind" ){
        fmi3_fmu_kind_enu_t fmu_kind = fmi3_import_get_fmu_kind(fmu);
        REQUIRE(fmi3_fmu_kind_me == fmu_kind);
    }

    SECTION( "Verifying FMU version returns '3.0'") {
        jm_status_enu_t status = fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_me, &inst_env, fmi3_dummy_log_callback);
        REQUIRE(status == 0);
        REQUIRE(strcmp(fmi3_import_get_version(fmu), "3.0") == 0);
    }

    SECTION( "Verify debug mode can be set" ) {
        int expected_mode = 1;
        int actual_mode = -1; // value will be overriden
        // it is required to setup the C-API struct before invoking set/get debug mode
        jm_status_enu_t status = fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_me, &inst_env, fmi3_dummy_log_callback);
        REQUIRE(status == 0);

        fmi3_import_set_debug_mode(fmu, expected_mode);
        actual_mode = fmi3_capi_get_debug_mode(fmu->capi);
        REQUIRE(actual_mode == expected_mode);
        fmi3_import_set_debug_mode(fmu, 0); // set back to zero to prevent segfaults
    }

    SECTION( "Test instantiate and set values on float64 array" ) {
        size_t n_value_references = 1;
        const fmi3_value_reference_t value_references[n_value_references] = {1234567};
        size_t n_values = 4;
        const fmi3_float64_t values[n_values] = {1.2, 2.5, 3.2, 4.5};
        jm_status_enu_t status = fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_me, &inst_env, fmi3_dummy_log_callback);
        REQUIRE( status == 0 );
        fmi3_string_t instanceName = "Test ME model instance";
        fmi3_string_t fmuInstantiationToken;
        fmi3_string_t resourcePath = "";
        fmi3_boolean_t visible = fmi3_false;
        fmi3_boolean_t loggingOn = fmi3_false;
        dummy_fmi3_instance_environment_t instance_env = {};
        jm_status_enu_t instantiate_status = fmi3_import_instantiate_model_exchange(
            fmu,
            instanceName,
            resourcePath,
            visible,
            loggingOn,
            ((fmi3_instance_environment_t)&instance_env),
            (fmi3_log_message_callback_ft)dummy_log_message_callback
        );
        REQUIRE( instantiate_status == jm_status_success );
        fmi3_status_t status_from_float_set = fmi3_import_set_float64(
            fmu,
            value_references,
            n_value_references,
            values,
            n_values
        );
        REQUIRE( status_from_float_set == fmi3_status_ok );

        fmi3_float64_t values_from_get[n_values] = {0.0, 0.0, 0.0, 0.0};
        fmi3_status_t status_from_float_get = fmi3_import_get_float64(
            fmu,
            value_references,
            n_value_references,
            values_from_get,
            n_values
        );
        REQUIRE( status_from_float_get == fmi3_status_ok );
        // We use indivdual calls to REQUIRE to make a potential error point to the exact line
        REQUIRE( values[0] == values_from_get[0] );
        REQUIRE( values[1] == values_from_get[1] );
        REQUIRE( values[2] == values_from_get[2] );
        REQUIRE( values[3] == values_from_get[3] );
    }

    SECTION( "Test instantiate and set values on int64 array" ) {
        size_t n_value_references = 1;
        /* This value reference does not matter because the Dummy FMU
            implementation for set/get int64 uses a fixed dummy array.
        */
        const fmi3_value_reference_t value_references[n_value_references] = {1};
        size_t n_int_values = 2;
        const fmi3_int64_t int64_values[n_int_values] = {-9223372036854775800, 9223372036854775800};
        jm_status_enu_t status = fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_me, &inst_env, fmi3_dummy_log_callback);
        REQUIRE( status == 0 );
        fmi3_string_t instanceName = "Test ME model instance";
        fmi3_string_t fmuInstantiationToken;
        fmi3_string_t resourcePath = "";
        fmi3_boolean_t visible = fmi3_false;
        fmi3_boolean_t loggingOn = fmi3_false;
        dummy_fmi3_instance_environment_t instance_env = {};
        jm_status_enu_t instantiate_status = fmi3_import_instantiate_model_exchange(
            fmu,
            instanceName,
            resourcePath,
            visible,
            loggingOn,
            ((fmi3_instance_environment_t)&instance_env),
            (fmi3_log_message_callback_ft)dummy_log_message_callback
        );
        REQUIRE( instantiate_status == jm_status_success );
        fmi3_status_t status_from_int_set = fmi3_import_set_int64(
            fmu,
            value_references,
            n_value_references,
            int64_values,
            n_int_values
        );
        REQUIRE( status_from_int_set == fmi3_status_ok );

        fmi3_int64_t values_from_int_get[2] = {0, 0};
        fmi3_status_t status_from_int_get = fmi3_import_get_int64(
            fmu,
            value_references,
            n_value_references,
            values_from_int_get,
            n_int_values
        );
        REQUIRE( status_from_int_get == fmi3_status_ok );
        REQUIRE( int64_values[0] == values_from_int_get[0] );
        REQUIRE( int64_values[1] == values_from_int_get[1] );
    }
    clean_up(fmu, context, inst_env);
}

TEST_CASE( "Test CAPI methods using a Co-Simulation FMU", test_file_name)
{
    jm_callbacks callbacks;
    fmi_import_context_t* context;
    fmi3_inst_env_t inst_env;
    callbacks.malloc = malloc;
    callbacks.calloc = calloc;
    callbacks.realloc = realloc;
    callbacks.free = free;
    callbacks.logger = importlogger;
    callbacks.log_level = jm_log_level_debug;
    callbacks.context = 0;
    context = fmi_import_allocate_context(&callbacks);
    fmi_version_enu_t version = fmi_import_get_fmi_version(context, FMU3_CS_PATH, FMU_TEMPORARY_TEST_DIR);
    fmi3_import_t* fmu = fmi3_import_parse_xml(context, FMU_TEMPORARY_TEST_DIR, 0);
    inst_env.fmu = fmu;

    SECTION( "Verifying returned result from fmi_import_get_fmi_version" ) {
        REQUIRE(version == fmi_version_3_0_enu);
    }

    SECTION( "Verifying FMU kind" ){
        fmi3_fmu_kind_enu_t fmu_kind = fmi3_import_get_fmu_kind(fmu);
        REQUIRE(fmi3_fmu_kind_cs == fmu_kind);
    }

    SECTION( "Verifying FMU version returns '3.0'") {
        jm_status_enu_t status = fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_cs, &inst_env, fmi3_dummy_log_callback);
        REQUIRE(status == 0);
        REQUIRE(strcmp(fmi3_import_get_version(fmu), "3.0") == 0);
    }

    SECTION( "Verify debug mode can be set" ) {
        int expected_mode = 1;
        int actual_mode = -1; // value will be overriden
        // it is required to setup the C-API struct before invoking set/get debug mode
        jm_status_enu_t status = fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_cs, &inst_env, fmi3_dummy_log_callback);
        REQUIRE(status == 0);

        fmi3_import_set_debug_mode(fmu, expected_mode);
        actual_mode = fmi3_capi_get_debug_mode(fmu->capi);
        REQUIRE(actual_mode == expected_mode);
        fmi3_import_set_debug_mode(fmu, 0); // set back to zero to prevent segfaults
    }

    SECTION( "Test instantiate and set values on float64 array" ) {
        size_t n_value_references = 1;
        const fmi3_value_reference_t value_references[n_value_references] = {1234567};
        size_t n_values = 4;
        const fmi3_float64_t values[n_values] = {1.2, 2.5, 3.2, 4.5};
        jm_status_enu_t status = fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_cs, &inst_env, fmi3_dummy_log_callback);
        REQUIRE( status == 0 );
        fmi3_string_t instanceName = "Test CS model instance";
        fmi3_string_t fmuInstantiationToken;
        fmi3_string_t resourcePath = "";
        fmi3_boolean_t visible = fmi3_false;
        fmi3_boolean_t loggingOn = fmi3_false;
        fmi3_boolean_t eventModeUsed = fmi3_false;
        fmi3_boolean_t earlyReturnAllowed = fmi3_false;
        const fmi3_value_reference_t requiredIntermediateVariables[] = {NULL};
        size_t nRequiredIntermediateVariables = 0;
        jm_status_enu_t instantiate_status = fmi3_import_instantiate_co_simulation(
            fmu,
            instanceName,
            resourcePath,
            visible,
            loggingOn,
            eventModeUsed,
            earlyReturnAllowed,
            requiredIntermediateVariables,
            nRequiredIntermediateVariables,
            NULL,
            NULL,
            NULL
        );
        REQUIRE( instantiate_status == jm_status_success );
        fmi3_status_t status_from_float_set = fmi3_import_set_float64(
            fmu,
            value_references,
            n_value_references,
            values,
            n_values
        );
        REQUIRE( status_from_float_set == fmi3_status_ok );

        fmi3_float64_t values_from_get[n_values] = {0.0, 0.0, 0.0, 0.0};
        fmi3_status_t status_from_float_get = fmi3_import_get_float64(
            fmu,
            value_references,
            n_value_references,
            values_from_get,
            n_values
        );
        REQUIRE( status_from_float_get == fmi3_status_ok );
        // We use indivdual calls to REQUIRE to make a potential error point to the exact line
        REQUIRE( values[0] == values_from_get[0] );
        REQUIRE( values[1] == values_from_get[1] );
        REQUIRE( values[2] == values_from_get[2] );
        REQUIRE( values[3] == values_from_get[3] );
    }

    SECTION( "Test instantiate and set values on int64 array" ) {
        size_t n_value_references = 1;
        /* This value reference does not matter because the Dummy FMU
            implementation for set/get int64 uses a fixed dummy array.
        */
        const fmi3_value_reference_t value_references[n_value_references] = {1};
        size_t n_int_values = 2;
        const fmi3_int64_t int64_values[n_int_values] = {-9223372036854775801, 9223372036854775801};
        jm_status_enu_t status = fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_cs, &inst_env, fmi3_dummy_log_callback);
        REQUIRE( status == 0 );
        fmi3_string_t instanceName = "Test CS model instance";
        fmi3_string_t fmuInstantiationToken;
        fmi3_string_t resourcePath = "";
        fmi3_boolean_t visible = fmi3_false;
        fmi3_boolean_t loggingOn = fmi3_false;
        fmi3_boolean_t eventModeUsed = fmi3_false;
        fmi3_boolean_t earlyReturnAllowed = fmi3_false;
        const fmi3_value_reference_t requiredIntermediateVariables[] = {NULL};
        size_t nRequiredIntermediateVariables = 0;
        jm_status_enu_t instantiate_status = fmi3_import_instantiate_co_simulation(
            fmu,
            instanceName,
            resourcePath,
            visible,
            loggingOn,
            eventModeUsed,
            earlyReturnAllowed,
            requiredIntermediateVariables,
            nRequiredIntermediateVariables,
            NULL,
            NULL,
            NULL
        );
        REQUIRE( instantiate_status == jm_status_success );
        fmi3_status_t status_from_int_set = fmi3_import_set_int64(
            fmu,
            value_references,
            n_value_references,
            int64_values,
            n_int_values
        );
        REQUIRE( status_from_int_set == fmi3_status_ok );

        fmi3_int64_t values_from_int_get[2] = {0, 0};

        fmi3_status_t status_from_int_get = fmi3_import_get_int64(
            fmu,
            value_references,
            n_value_references,
            values_from_int_get,
            n_int_values
        );
        REQUIRE( status_from_int_get == fmi3_status_ok );
        REQUIRE( int64_values[0] == values_from_int_get[0] );
        REQUIRE( int64_values[1] == values_from_int_get[1] );
    }
    clean_up(fmu, context, inst_env);

}

TEST_CASE( "Test CAPI methods using a Scheduled-Execution FMU", test_file_name)
{
    jm_callbacks callbacks;
    fmi_import_context_t* context;
    fmi3_inst_env_t inst_env;
    callbacks.malloc = malloc;
    callbacks.calloc = calloc;
    callbacks.realloc = realloc;
    callbacks.free = free;
    callbacks.logger = importlogger;
    callbacks.log_level = jm_log_level_debug;
    callbacks.context = 0;
    context = fmi_import_allocate_context(&callbacks);
    fmi_version_enu_t version = fmi_import_get_fmi_version(context, FMU3_SE_PATH, FMU_TEMPORARY_TEST_DIR);
    fmi3_import_t* fmu = fmi3_import_parse_xml(context, FMU_TEMPORARY_TEST_DIR, 0);
    inst_env.fmu = fmu;

    SECTION( "Verifying returned result from fmi_import_get_fmi_version" ) {
        REQUIRE(version == fmi_version_3_0_enu);
    }

    SECTION( "Verifying FMU kind" ){
        fmi3_fmu_kind_enu_t fmu_kind = fmi3_import_get_fmu_kind(fmu);
        REQUIRE(fmi3_fmu_kind_se == fmu_kind);
    }

    SECTION( "Verifying FMU version returns '3.0'") {
        fmi3_inst_env_t inst_env;
        inst_env.fmu = fmu;
        jm_status_enu_t status = fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_se, &inst_env, fmi3_dummy_log_callback);
        REQUIRE(status == 0);
        REQUIRE(strcmp(fmi3_import_get_version(fmu), "3.0") == 0);
    }

    SECTION( "Verify debug mode can be set" ) {
        int expected_mode = 1;
        int actual_mode = -1; // value will be overriden
        // it is required to setup the C-API struct before invoking set/get debug mode
        jm_status_enu_t status = fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_se, &inst_env, fmi3_dummy_log_callback);
        REQUIRE(status == 0);

        fmi3_import_set_debug_mode(fmu, expected_mode);
        actual_mode = fmi3_capi_get_debug_mode(fmu->capi);
        REQUIRE(actual_mode == expected_mode);
        fmi3_import_set_debug_mode(fmu, 0); // set back to zero to prevent segfaults
    }

    SECTION( "Test instantiate and set values on float64 array" ) {
        size_t n_value_references = 1;
        const fmi3_value_reference_t value_references[n_value_references] = {1234567};
        size_t n_values = 4;
        const fmi3_float64_t values[n_values] = {1.2, 2.5, 3.2, 4.5};
        jm_status_enu_t status = fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_se, &inst_env, fmi3_dummy_log_callback);
        REQUIRE( status == 0 );
        fmi3_string_t instanceName = "Test SE model instance";
        fmi3_string_t fmuInstantiationToken;
        fmi3_string_t resourcePath = "";
        fmi3_boolean_t visible = fmi3_false;
        fmi3_boolean_t loggingOn = fmi3_false;
        dummy_fmi3_instance_environment_t instance_env = {};
        jm_status_enu_t instantiate_status = fmi3_import_instantiate_scheduled_execution(
                fmu,
                instanceName,
                resourcePath,
                visible,
                loggingOn,
                ((fmi3_instance_environment_t)&instance_env),
                (fmi3_log_message_callback_ft)dummy_log_message_callback,
                (fmi3_clock_update_callback_ft)dummy_clock_update_callback,
                (fmi3_lock_preemption_callback_ft)dummy_lock_preemption_callback,
                (fmi3_unlock_preemption_callback_ft)dummy_unlock_preemption_callback
        );
        REQUIRE( instantiate_status == jm_status_success );
        fmi3_status_t status_from_float_set = fmi3_import_set_float64(
            fmu,
            value_references,
            n_value_references,
            values,
            n_values
        );
        REQUIRE( status_from_float_set == fmi3_status_ok );

        fmi3_float64_t values_from_get[n_values] = {0.0, 0.0, 0.0, 0.0};
        fmi3_status_t status_from_float_get = fmi3_import_get_float64(
            fmu,
            value_references,
            n_value_references,
            values_from_get,
            n_values
        );
        REQUIRE( status_from_float_get == fmi3_status_ok );
        // We use indivdual calls to REQUIRE to make a potential error point to the exact line
        REQUIRE( values[0] == values_from_get[0] );
        REQUIRE( values[1] == values_from_get[1] );
        REQUIRE( values[2] == values_from_get[2] );
        REQUIRE( values[3] == values_from_get[3] );
    }

    SECTION( "Test instantiate and set values on int64 array" ) {
        size_t n_value_references = 1;
        /* This value reference does not matter because the Dummy FMU
            implementation for set/get int64 uses a fixed dummy array.
        */
        const fmi3_value_reference_t value_references[n_value_references] = {1};
        size_t n_int_values = 2;
        const fmi3_int64_t int64_values[n_int_values] = {-9223372036854775802, 9223372036854775802};
        jm_status_enu_t status = fmi3_import_create_dllfmu(fmu, fmi3_fmu_kind_se, &inst_env, fmi3_dummy_log_callback);
        REQUIRE( status == 0 );
        fmi3_string_t instanceName = "Test SE model instance";
        fmi3_string_t fmuInstantiationToken;
        fmi3_string_t resourcePath = "";
        fmi3_boolean_t visible = fmi3_false;
        fmi3_boolean_t loggingOn = fmi3_false;
        dummy_fmi3_instance_environment_t instance_env = {};
        jm_status_enu_t instantiate_status = fmi3_import_instantiate_scheduled_execution(
                fmu,
                instanceName,
                resourcePath,
                visible,
                loggingOn,
                ((fmi3_instance_environment_t)&instance_env),
                (fmi3_log_message_callback_ft)dummy_log_message_callback,
                (fmi3_clock_update_callback_ft)dummy_clock_update_callback,
                (fmi3_lock_preemption_callback_ft)dummy_lock_preemption_callback,
                (fmi3_unlock_preemption_callback_ft)dummy_unlock_preemption_callback
        );
        REQUIRE( instantiate_status == jm_status_success );
        fmi3_status_t status_from_int_set = fmi3_import_set_int64(
            fmu,
            value_references,
            n_value_references,
            int64_values,
            n_int_values
        );
        REQUIRE( status_from_int_set == fmi3_status_ok );

        fmi3_int64_t values_from_int_get[2] = {0, 0};
        fmi3_status_t status_from_int_get = fmi3_import_get_int64(
            fmu,
            value_references,
            n_value_references,
            values_from_int_get,
            n_int_values
        );
        REQUIRE( status_from_int_get == fmi3_status_ok );
        REQUIRE( int64_values[0] == values_from_int_get[0] );
        REQUIRE( int64_values[1] == values_from_int_get[1] );
    }
    clean_up(fmu, context, inst_env);
}