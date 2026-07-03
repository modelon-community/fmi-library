/*
    Copyright (C) 2026 Modelon AB

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

void importlogger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message)
{
    printf("module = %s, log level = %s: %s\n", module, jm_log_level_to_string(log_level), message);
}

TEST_CASE("Test fmi version with patch updates") {
    /* Test that import accepts fmiVersions that include patch releases. */
    jm_callbacks callbacks;

    callbacks.malloc = malloc;
    callbacks.calloc = calloc;
    callbacks.realloc = realloc;
    callbacks.free = free;
    callbacks.logger = importlogger;
    callbacks.log_level = jm_log_level_debug;
    callbacks.context = 0;

    const char* tmpPath = fmi_import_mk_temp_dir(&callbacks, FMU_UNPACK_DIR, NULL);
    REQUIRE(tmpPath != nullptr);

    fmi_import_context_t* context = fmi_import_allocate_context(&callbacks);
    fmi_version_enu_t version = fmi_import_get_fmi_version(context, FMU3_PATCHED_PATH, tmpPath);
    REQUIRE(version == fmi_version_3_0_enu);

    fmi_import_free_context(context);
    REQUIRE(fmi_import_rmdir(&callbacks, tmpPath) == jm_status_success);
    callbacks.free((void*)tmpPath);
}
