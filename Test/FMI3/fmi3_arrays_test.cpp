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

typedef struct {
    fmi3_import_t* fmu; /* fmi3_log_forwarding is based on the fmu instance */
} fmi3_inst_env_t;


/* The logger function used for the logger callback struct. */
void importlogger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message)
{
    printf("module = %s, log level = %s: %s\n", module, jm_log_level_to_string(log_level), message);
}

/* The test file name, displayed during the testing */
static const char test_file_name[] = "[fmi3_arrays.cpp]";


void fmi_testutil_build_xml_path(char* buf, size_t bufSize, const char* basePath, const char* appendPath) {
    strncpy(buf, basePath,   bufSize);
    strncat(buf, appendPath, bufSize);
}


static fmi3_import_t *parse_xml(const char *model_desc_path)
{
    jm_callbacks *cb = jm_get_default_callbacks();
    fmi_import_context_t *ctx = fmi_import_allocate_context(cb);
    fmi3_import_t *xml;

    if (ctx == NULL) {
        return NULL;
    }

    xml = fmi3_import_parse_xml(ctx, model_desc_path, NULL);

    fmi_import_free_context(ctx);
    return xml;
}


TEST_CASE("Test basic float64 array parsing, work in progress"){
    jm_callbacks cb;
    fmi_import_context_t *context;

    cb.malloc    = malloc;
    cb.calloc    = calloc;
    cb.realloc   = realloc;
    cb.free      = free;
    cb.logger    = importlogger;
    cb.log_level = jm_log_level_all;
    cb.context   = NULL;
    context = fmi_import_allocate_context(&cb);
    fmi_import_set_configuration(context, FMI_IMPORT_NAME_CHECK);
    char xmlPath[1000];
    size_t sizeXmlPath = sizeof(xmlPath) / sizeof(char);

    fmi_testutil_build_xml_path(xmlPath, sizeXmlPath, ARRAY_TEST_MODEL_DESCRIPTION_DIR, "/valid");
    fmi3_import_t *xml = parse_xml(xmlPath);

    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "array4_64");
    SECTION("Test float64 fmi3_import_get_variable_has_start") {
        int has_start = fmi3_import_get_variable_has_start(v);
        REQUIRE(has_start == 1);
    }
    SECTION("Test float64 fmi3_import_variable_is_array") {
        int is_array = fmi3_import_variable_is_array(v);
        REQUIRE(is_array == 1);
    }
    fmi3_import_dimension_list_t* dimList = fmi3_import_get_variable_dimension_list(xml, v); /* allocates memory */
    size_t nDims = fmi3_import_get_dimension_list_size(dimList);
    /* check num start values */
    fmi3_float64_t* start = fmi3_import_get_float64_variable_start_array(fmi3_import_get_variable_as_float64(v));
    SECTION("Test float64 array") {
        double eps = 0.0000001;
        REQUIRE(fabs(start[0] - 1.0) < eps);
        REQUIRE(fabs(start[6] - 7.0) < eps);
        REQUIRE(fabs(start[7] - 8.0) < eps);
    }
    fmi3_import_free_dimension_list(dimList);
    fmi_import_free_context(context);
    fmi3_import_free(xml);
}

TEST_CASE("Test retrieve start values of an int64 array with small values"){
    jm_callbacks cb;
    fmi_import_context_t *context;

    cb.malloc    = malloc;
    cb.calloc    = calloc;
    cb.realloc   = realloc;
    cb.free      = free;
    cb.logger    = importlogger;
    cb.log_level = jm_log_level_all;
    cb.context   = NULL;
    context = fmi_import_allocate_context(&cb);
    fmi_import_set_configuration(context, FMI_IMPORT_NAME_CHECK);
    char xmlPath[1000];
    size_t sizeXmlPath = sizeof(xmlPath) / sizeof(char);

    fmi_testutil_build_xml_path(xmlPath, sizeXmlPath, ARRAY_TEST_MODEL_DESCRIPTION_DIR, "/valid");
    fmi3_import_t *xml = parse_xml(xmlPath);

    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "dummy_int64_array");
    SECTION("Test int64 fmi3_import_get_variable_has_start") {
        int has_start = fmi3_import_get_variable_has_start(v);
        REQUIRE(has_start == 1);
    }
    SECTION("Test int64 fmi3_import_variable_is_array") {
        int is_array = fmi3_import_variable_is_array(v);
        REQUIRE(is_array == 1);
    }
    fmi3_import_dimension_list_t* dimList = fmi3_import_get_variable_dimension_list(xml, v); /* allocates memory */
    size_t nDims = fmi3_import_get_dimension_list_size(dimList);
    /* check num start values */
    SECTION("Test int64 start array") {
        fmi3_int64_t* start = fmi3_import_get_int64_variable_start_array(fmi3_import_get_variable_as_int64(v));
        REQUIRE(start[0] == 54);
        REQUIRE(start[1] ==  23);
        REQUIRE(start[2] ==  11);
        REQUIRE(start[3] ==  5123);
    }
    fmi3_import_free_dimension_list(dimList);
    fmi_import_free_context(context);
    fmi3_import_free(xml);
}


TEST_CASE("Test retrieve start values of an int64 array with values as large/small as possible"){
    jm_callbacks cb;
    fmi_import_context_t *context;

    cb.malloc    = malloc;
    cb.calloc    = calloc;
    cb.realloc   = realloc;
    cb.free      = free;
    cb.logger    = importlogger;
    cb.log_level = jm_log_level_all;
    cb.context   = NULL;
    context = fmi_import_allocate_context(&cb);
    fmi_import_set_configuration(context, FMI_IMPORT_NAME_CHECK);
    char xmlPath[1000];
    size_t sizeXmlPath = sizeof(xmlPath) / sizeof(char);

    fmi_testutil_build_xml_path(xmlPath, sizeXmlPath, ARRAY_TEST_MODEL_DESCRIPTION_DIR, "/valid");
    fmi3_import_t *xml = parse_xml(xmlPath);

    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "dummy_int64_array2");
    SECTION("Test int64 fmi3_import_get_variable_has_start") {
        int has_start = fmi3_import_get_variable_has_start(v);
        REQUIRE(has_start == 1);
    }
    SECTION("Test int64 fmi3_import_variable_is_array") {
        int is_array = fmi3_import_variable_is_array(v);
        REQUIRE(is_array == 1);
    }
    fmi3_import_dimension_list_t* dimList = fmi3_import_get_variable_dimension_list(xml, v); /* allocates memory */
    size_t nDims = fmi3_import_get_dimension_list_size(dimList);
    /* check num start values */
    SECTION("Test int64 start array") {
        fmi3_int64_t* start = fmi3_import_get_int64_variable_start_array(fmi3_import_get_variable_as_int64(v));
        REQUIRE(start[0] ==  9223372036854775807);
        REQUIRE(start[1] ==  9223372036854775806);
        REQUIRE(start[2] ==  9223372036854775805);
        REQUIRE(start[3] == -9223372036854775808);
        REQUIRE(start[4] == 0);
        REQUIRE(start[5] == -9223372036854775807);
    }
    fmi3_import_free_dimension_list(dimList);
    fmi_import_free_context(context);
    fmi3_import_free(xml);
}