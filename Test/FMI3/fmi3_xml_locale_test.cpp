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
#include <locale.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#include "catch.hpp"

/**
 * Tests that parsing works as expected, and that the previous locale and
 * thread settings are reset.
 *
 * Test is by default disabled, because it requires the target machine to have
 * specific language packs.
 */
TEST_CASE("Test parsing with locale set") {

#ifdef FMILIB_TEST_LOCALE
    char* loc_old = nullptr;
    char* tmp = nullptr;

    /* Any locale that uses decimal comma instead of decimal point. */
#ifdef WIN32
    const char* locale_bad = "Swedish_Sweden.1252";
#else
    const char* locale_bad = "sv_SE.utf8";
#endif

    // TODO: Are specific thread setting still required?
    /* Set/get thread-specific settings (and later check that they are
     * restored).
     * Do nothing for Linux since I don't think it's possible to check equality
     * of locale_t.
     */
#ifdef _MSC_VER
    int thread_setting = _DISABLE_PER_THREAD_LOCALE;
    _configthreadlocale(thread_setting);
#endif

    /* NOT MT-SAFE: But it's the only way to test it for Linux. There are
     * currently no other tests that modify the locale globally, so should be
     * OK. Also, ctest tests are by default not run in parallel.
     */
    tmp = setlocale(LC_NUMERIC, locale_bad);
    REQUIRE(tmp != NULL); 
    /* If this errors, it's possible that your machine doesn't have
        * the locale installed.
        *
        * Windows: It seemed like I had at least Danish, French, Swedish
        *   installed by default.
        *
        * Linux (Ubuntu 18): I had to install a language pack to get this.
    */

    /*
     * Value of 'tmp' returned from 'setlocale' may be changed by further calls
     * to setlocale, and it's also possible that the returned value is not
     * "string equal" to the argument (i.e. alias values for the same locale).
     * To be able to later compare the restored value, we therefore must copy
     * 'tmp'.
     */
    loc_old = (char*)malloc(strlen(tmp) + 1);
    REQUIRE(loc_old != NULL);
    strcpy(loc_old, tmp);
    tmp = NULL;

    SECTION("Parsing test") {
        /* Perform parsing and verify that the bad global locale did not affect
         * the result. */

        const char* xmldir = FMI3_TEST_XML_DIR "/env/locale";
        
        fmi3_import_t* xml = fmi3_testutil_parse_xml(xmldir);
        REQUIRE(xml != nullptr);

        /* If the decimal delimiter is comma, sscanf will only parse
        * until the dot. */
        REQUIRE(fmi3_import_get_default_experiment_start(xml)     == 2.3 );
        REQUIRE(fmi3_import_get_default_experiment_stop(xml)      == 3.55);
        REQUIRE(fmi3_import_get_default_experiment_tolerance(xml) == 1e-6);
        REQUIRE(fmi3_import_get_default_experiment_step(xml)      == 2e-3);

        fmi3_import_free(xml);
    }

    /* Cleanup and verify that locale is properly restored.
     *
     * Getting locale should be MT-safe if all setting of locale is done in
     * per_thread context.
     */
    tmp = setlocale(LC_NUMERIC, loc_old);
    REQUIRE(tmp != NULL);
    REQUIRE_STREQ(tmp, locale_bad);
    free(loc_old);

#ifdef _MSC_VER
    if (_configthreadlocale(0) != thread_setting) {
        /* This was set at the beginning of the test, and should now have been restored. */
        fail("unexpected Windows thread setting");
    }
#endif

#endif // FMILIB_TEST_LOCALE
}
