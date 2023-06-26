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

#include <stdlib.h>
#include <stdio.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#include "catch.hpp"

void parse_exclude_msg(const char* xml_dir, const char* log_msg) {
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xml_dir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);
    
    REQUIRE(!fmi3_testutil_log_contains(tfmu, log_msg));
    fmi3_testutil_import_free(tfmu);
}

void parse_include_msg(const char* xml_dir, const char* log_msg) {
    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xml_dir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(fmu != nullptr);
    
    REQUIRE(fmi3_testutil_log_contains(tfmu, log_msg));
    fmi3_testutil_import_free(tfmu);
}

static fmi3_import_t* parse_xml(jm_callbacks* cb, const char* xmldir) {
    fmi_import_context_t* context;
    fmi3_import_t* xml;

    context = fmi_import_allocate_context(cb);

    xml = fmi3_import_parse_xml(context, xmldir, NULL);
    fmi_import_free_context(context);

    return xml;
}

TEST_CASE("Test naming conventions") {
    /* Test scalar variable names
     *
     * Every test below has a corresponding modelDescription in
     * Test/FMI3/naming_conventions_xmls/
     * What is passed to these macros are names of directories containing
     * modelDescriptions.
     */

    const char* log_msg = "Invalid structured ScalarVariable name";

    /* Test examples mentioned */
    parse_include_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/examples/foo", log_msg);
    parse_include_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/examples/derderx", log_msg);
    parse_exclude_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/examples/derx2", log_msg);

    /* FMI 2.0 standard examples from the documentaiton */
    parse_exclude_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/standard/vehicle.engine.speed", log_msg);
    parse_exclude_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/standard/resistor12.u", log_msg);
    parse_exclude_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/standard/v_min", log_msg);
    parse_exclude_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/standard/robot.axis.motor234", log_msg);
    parse_exclude_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/standard/derpipe34.T142", log_msg);

    /* Implementation test examples */
    parse_include_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/implementation/empty", log_msg);
    parse_include_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/implementation/-0", log_msg);
    parse_exclude_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/implementation/_0", log_msg);
    parse_exclude_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/implementation/a0", log_msg);
    parse_include_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/implementation/0a", log_msg);
    parse_include_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/implementation/0", log_msg);

    /* q-name tests */
    parse_include_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/q-name/empty", log_msg);
    parse_exclude_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/q-name/space", log_msg);
    parse_include_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/q-name/backslash", log_msg);
    parse_exclude_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/q-name/q-char", log_msg);
    parse_exclude_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/q-name/escape", log_msg);
    parse_include_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/q-name/chinese", log_msg);

    /* der() tests */
    parse_include_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/der/dera32", log_msg);
    parse_include_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/der/dera12", log_msg);
    parse_exclude_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/der/dera32-no-space", log_msg);
    parse_exclude_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/der/dera", log_msg);
    parse_include_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/der/dera-no-closing-parenthesis", log_msg);
    parse_exclude_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/der/somederthing", log_msg);
    parse_exclude_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/der/der0", log_msg);
    parse_include_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/der/der2", log_msg);
    parse_include_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/der/adera", log_msg);

    /* array and hierarchy tests */
    parse_exclude_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/array/n0", log_msg);
    parse_include_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/array/a1comma", log_msg);
    parse_exclude_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/array/a12345678", log_msg);
    parse_include_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/array/a12345678space", log_msg);
    parse_exclude_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/array/a1.a3", log_msg);
    parse_exclude_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/array/a.a123", log_msg);
    parse_include_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/array/aspace1", log_msg);
    parse_include_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/array/a1space", log_msg);
    parse_include_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/array/a1space1", log_msg);
    parse_include_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/array/aspacebracket1", log_msg);
    parse_include_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/array/a-1", log_msg);
    parse_exclude_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/array/a1", log_msg);
    parse_exclude_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/array/a.a", log_msg);
    parse_exclude_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/array/a", log_msg);

    /* list of variables */
    parse_include_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/list/aemptyc", log_msg);
    parse_exclude_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/list/cba", log_msg);

    /* flat hierarchy test */
    parse_include_msg(FMI3_TEST_XML_DIR "/naming_conventions_xmls/flat/q-char-nonescaped", log_msg);
}
