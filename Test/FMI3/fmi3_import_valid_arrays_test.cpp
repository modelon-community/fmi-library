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
#include "fmi_testutil.h"
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


/*
void fmi_testutil_build_xml_path(char* buf, size_t bufSize, const char* basePath, const char* appendPath) {
    strncpy(buf, basePath,   bufSize);
    strncat(buf, appendPath, bufSize);
}
*/


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

/*
* This function verifies that input argument 'v' has a start value, that it is an array and
* and that we retrieve the correct number of dimensions..
*   Input parameters:
*       v: The array variable to test
*       xml: The xml object (as returned from fmi3_import_parse_xml).
*       nDimsExpected: The expected number of dimensions for the array variable 'v'.
*/
static fmi3_import_dimension_list_t* basic_array_checks(fmi3_import_variable_t* v, fmi3_import_t* xml,
        size_t nDimsExpected) {
    int has_start = fmi3_import_get_variable_has_start(v);
    REQUIRE(has_start == 1);
    int is_array = fmi3_import_variable_is_array(v);
    REQUIRE(is_array == 1);

    fmi3_import_dimension_list_t* dimList = fmi3_import_get_variable_dimension_list(xml, v); /* allocates memory */
    size_t nDims = fmi3_import_get_dimension_list_size(dimList);
    REQUIRE(nDims == nDimsExpected);
    return dimList; /* has to be freed by the caller. */
}


TEST_CASE("Binary array") {
    const char* xmldir = FMI3_TEST_XML_DIR "/arrays/valid/binary_array";
    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);

    SECTION("Test binary start array") {
        fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "binary_array");
        fmi3_import_dimension_list_t* dimList = basic_array_checks(v, xml, 1);
        fmi3_binary_t* start = fmi3_import_get_binary_variable_start_array(fmi3_import_get_variable_as_binary(v));
        REQUIRE(start[0][0]   == 0x00U);
        REQUIRE(start[0][1]   == 0x11U);
        REQUIRE(start[0][2]   == 0xBBU);
        REQUIRE(start[0][3]   == 0xffU);
        REQUIRE(start[0][4]   == 0x02U);
        REQUIRE(start[0][5]   == 0x9eU);
        REQUIRE(start[0][6]   == 0xE4U);
        REQUIRE(start[0][7]   == 0xCdU);
        start++;
        REQUIRE(start[0][0]   == 0x3cU);
        REQUIRE(start[0][1]   == 0x3fU);
        start++;
        REQUIRE(start[0][0]   == 0xE4U);
        REQUIRE(start[0][1]   == 0xCdU);
        fmi3_import_free_dimension_list(dimList);
    }
    fmi3_import_free(xml);
}

TEST_CASE("Test array parsing and verify retrieved start values are as expected"){
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

    fmi_testutil_build_xml_path(xmlPath, sizeXmlPath, FMI3_TEST_XML_DIR, "/arrays/valid");
    fmi3_import_t *xml = parse_xml(xmlPath);
    SECTION("Test boolean start array") {
        REQUIRE(xml != nullptr);
        fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "bool_array");
        fmi3_import_dimension_list_t* dimList = basic_array_checks(v, xml, 2);
        fmi3_boolean_t* start = fmi3_import_get_boolean_variable_start_array(fmi3_import_get_variable_as_boolean(v));
        REQUIRE(start[0] == false);
        REQUIRE(start[1] == true);
        REQUIRE(start[2] == true);
        REQUIRE(start[3] == false);
        fmi3_import_free_dimension_list(dimList);
    }
    SECTION("Test float64 array") {
        fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "array4_64");
        fmi3_import_dimension_list_t* dimList = basic_array_checks(v, xml, 3);
        fmi3_float64_t* start = fmi3_import_get_float64_variable_start_array(
                fmi3_import_get_variable_as_float64(v));
        double eps = 0.0000001;
        REQUIRE(fabs(start[0] - 1.0) < eps);
        REQUIRE(fabs(start[6] - 7.0) < eps);
        REQUIRE(fabs(start[7] - 8.0) < eps);
        fmi3_import_free_dimension_list(dimList);
    }

    SECTION("Test int64 start array") {
        fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "int64_array");
        fmi3_import_dimension_list_t* dimList = basic_array_checks(v, xml, 2);
        fmi3_int64_t* start = fmi3_import_get_int64_variable_start_array(fmi3_import_get_variable_as_int64(v));
        REQUIRE(start[0] ==  9223372036854775807);
        REQUIRE(start[1] ==  1234);
        REQUIRE(start[2] ==  9223372036854775805);
        REQUIRE(start[3] == -9223372036854775805);
        REQUIRE(start[4] == 0);
        REQUIRE(start[5] == -9223372036854775807);
        fmi3_import_free_dimension_list(dimList);
    }

    SECTION("Test int32 start array") {
        fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "int32_array");
        fmi3_import_dimension_list_t* dimList = basic_array_checks(v, xml, 2);

        fmi3_int32_t* start = fmi3_import_get_int32_variable_start_array(fmi3_import_get_variable_as_int32(v));
        REQUIRE(start[0] ==  2147483647);
        REQUIRE(start[1] ==  2147483646);
        REQUIRE(start[2] == -2147483648);
        REQUIRE(start[3] == -2147483647);
        fmi3_import_free_dimension_list(dimList);
    }

    SECTION("Test int16 start array") {
        fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "int16_array");
        fmi3_import_dimension_list_t* dimList = basic_array_checks(v, xml, 2);
        fmi3_int16_t* start = fmi3_import_get_int16_variable_start_array(fmi3_import_get_variable_as_int16(v));
        REQUIRE(start[0] ==  32767);
        REQUIRE(start[1] ==  32766);
        REQUIRE(start[2] == -32768);
        REQUIRE(start[3] == -32767);
        fmi3_import_free_dimension_list(dimList);
    }

    SECTION("Test int8 start array") {
        fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "int8_array");
        fmi3_import_dimension_list_t* dimList = basic_array_checks(v, xml, 2);
        fmi3_int8_t* start = fmi3_import_get_int8_variable_start_array(fmi3_import_get_variable_as_int8(v));
        REQUIRE(start[0] ==  127);
        REQUIRE(start[1] ==  126);
        REQUIRE(start[2] == -128);
        REQUIRE(start[3] == -127);
        fmi3_import_free_dimension_list(dimList);
    }


    SECTION("Test uint64 start array") {
        fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "uint64_array");
        fmi3_import_dimension_list_t* dimList = basic_array_checks(v, xml, 2);
        fmi3_uint64_t* start = fmi3_import_get_uint64_variable_start_array(fmi3_import_get_variable_as_uint64(v));
        REQUIRE(start[0] == 18446744073709551615ULL);
        REQUIRE(start[1] == 0);
        REQUIRE(start[2] == 1);
        REQUIRE(start[3] == 2);
        fmi3_import_free_dimension_list(dimList);
    }

    SECTION("Test uint32 start array") {
        fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "uint32_array");
        fmi3_import_dimension_list_t* dimList = basic_array_checks(v, xml, 2);

        fmi3_uint32_t* start = fmi3_import_get_uint32_variable_start_array(fmi3_import_get_variable_as_uint32(v));
        REQUIRE(start[0] == 0);
        REQUIRE(start[1] == 4294967295);
        REQUIRE(start[2] == 5);
        REQUIRE(start[3] == 1);
        REQUIRE(start[4] == 2);
        REQUIRE(start[5] == 1);
        fmi3_import_free_dimension_list(dimList);
    }

    SECTION("Test uint16 start array") {
        fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "uint16_array");
        fmi3_import_dimension_list_t* dimList = basic_array_checks(v, xml, 2);
        fmi3_uint16_t* start = fmi3_import_get_uint16_variable_start_array(fmi3_import_get_variable_as_uint16(v));
        REQUIRE(start[0] == 65535);
        REQUIRE(start[1] == 65535);
        REQUIRE(start[2] == 1);
        REQUIRE(start[3] == 2);
        REQUIRE(start[4] == 3);
        REQUIRE(start[5] == 4);
        fmi3_import_free_dimension_list(dimList);
    }

    SECTION("Test uint8 start array") {
        fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "uint8_array");
        fmi3_import_dimension_list_t* dimList = basic_array_checks(v, xml, 3);
        fmi3_uint8_t* start = fmi3_import_get_uint8_variable_start_array(fmi3_import_get_variable_as_uint8(v));
        REQUIRE(start[1] == 255);
        REQUIRE(start[2] == 3);
        REQUIRE(start[3] == 255);
        REQUIRE(start[4] == 5);
        REQUIRE(start[5] == 6);
        REQUIRE(start[6] == 255);
        REQUIRE(start[7] == 255);
        fmi3_import_free_dimension_list(dimList);
    }

    SECTION("Test enum start array") {
        fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "enum_array");
        fmi3_import_dimension_list_t* dimList = basic_array_checks(v, xml, 1);
        fmi3_int64_t* start = fmi3_import_get_enum_variable_start_array(fmi3_import_get_variable_as_enum(v));
        REQUIRE(start[0] == 3);
        REQUIRE(start[1] == 1);
        REQUIRE(start[2] == 2);
        REQUIRE(start[3] == 3);
        REQUIRE(start[4] == 1);
        REQUIRE(start[5] == 9223372036854775807);
        REQUIRE(start[6] == 2);
        fmi3_import_free_dimension_list(dimList);
    }

    SECTION("Test enum start array with only one (negative) value") {
        fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "enum_array2");
        fmi3_import_dimension_list_t* dimList = basic_array_checks(v, xml, 1);
        fmi3_int64_t* start = fmi3_import_get_enum_variable_start_array(fmi3_import_get_variable_as_enum(v));
        REQUIRE(start[0] == -57);
        fmi3_import_free_dimension_list(dimList);
    }

    SECTION("Test string start array") {
        fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "string_array");
        fmi3_import_dimension_list_t* dimList = basic_array_checks(v, xml, 1);
        fmi3_string_t* start = fmi3_import_get_string_variable_start_array(fmi3_import_get_variable_as_string(v));
        REQUIRE(start[0][0]   == 'T');
        REQUIRE(start[0][1]   == 'h');
        REQUIRE(start[0][2]   == 'e');
        REQUIRE(start[0][3]   == ' ');
        REQUIRE(start[0][4]   == 'f');
        REQUIRE(start[0][5]   == 'i');
        REQUIRE(start[0][6]   == 'r');
        REQUIRE(start[0][7]   == 's');
        REQUIRE(start[0][8]   == 't');
        REQUIRE(start[0][9]   == ' ');
        REQUIRE(start[0][10]  == 's');
        REQUIRE(start[0][11]  == 't');
        REQUIRE(start[0][12]  == 'r');
        REQUIRE(start[0][13]  == 'i');
        REQUIRE(start[0][14]  == 'n');
        REQUIRE(start[0][15]  == 'g');
        REQUIRE(start[0][16]  == ',');
        start++;
        REQUIRE(start[0][0]   == 'a');
        REQUIRE(start[0][1]   == 'n');
        REQUIRE(start[0][2]   == 'd');
        /* Skip straight to the last start value directly */
        start++;
        REQUIRE(start[0][0]   == 'a');
        REQUIRE(start[0][1]   == 'b');
        REQUIRE(start[0][2]   == 'c');
        REQUIRE(start[0][3]   == 'd');
        REQUIRE(start[0][4]   == '.');
        fmi3_import_free_dimension_list(dimList);
    }

    SECTION("Test string start no array") {
        fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "string_var");
        fmi3_string_t* start = fmi3_import_get_string_variable_start(fmi3_import_get_variable_as_string(v));
        REQUIRE(start[0][0]    == 'A');
        REQUIRE(start[0][1]    == ' ');
        REQUIRE(start[0][2]    == 's');
        REQUIRE(start[0][3]    == 'T');
        REQUIRE(start[0][4]    == 'r');
        REQUIRE(start[0][5]    == 'i');
        REQUIRE(start[0][6]    == 'n');
        REQUIRE(start[0][7]    == 'g');
        REQUIRE(start[0][8]    == ' ');
        REQUIRE(start[0][9]    == 'v');
        REQUIRE(start[0][10]   == 'a');
        REQUIRE(start[0][11]   == 'l');
        REQUIRE(start[0][12]   == 'u');
        REQUIRE(start[0][13]   == 'e');
    }
    fmi_import_free_context(context);
    fmi3_import_free(xml);
}

TEST_CASE("Test int64 start array with 100 variables") {
    const char* xmldir = FMI3_TEST_XML_DIR "/arrays/valid/big1";
    fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
    REQUIRE(xml != nullptr);

    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "int64_array_100");
    REQUIRE(v != nullptr);

    fmi3_import_dimension_list_t* dimList = basic_array_checks(v, xml, 2);
    fmi3_int64_t* start = fmi3_import_get_int64_variable_start_array(fmi3_import_get_variable_as_int64(v));
    int expected_value = 0;
    for (int i = 1; i <= 100; i++) {
        expected_value = (i%5 == 0) ? -i : i;
        REQUIRE(start[i-1] == expected_value);
    }
    fmi3_import_free_dimension_list(dimList);

    fmi3_import_free(xml);
}