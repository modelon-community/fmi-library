/*
    Copyright (C) 2024 Modelon AB

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
#include <string.h>

#include <JM/jm_types.h>
#include <JM/jm_callbacks.h>
#include <JM/jm_portability.h>
#include <FMI/fmi_zip_unzip.h>
#include "config_test.h"

static int failures = 0;

#define TEST_ASSERT(cond, msg) do { \
    if (!(cond)) { \
        printf("FAIL: %s\n", msg); \
        failures++; \
    } else { \
        printf("PASS: %s\n", msg); \
    } \
} while(0)

void test_logger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message) {
    printf("  [%s] %s\n", module, message);
}

int main(int argc, char *argv[]) {
    jm_callbacks cb;
    char* tmpDir;
    char* subDir;
    char* zipPath;
    char* extractPath;
    jm_status_enu_t status;

    cb.malloc = malloc;
    cb.calloc = calloc;
    cb.realloc = realloc;
    cb.free = free;
    cb.logger = test_logger;
    cb.log_level = jm_log_level_debug;
    cb.context = 0;

    printf("=== Unicode path test ===\n\n");

    /* Test jm_mk_temp_dir with a prefix containing non-ASCII characters.
     * On Windows these are UTF-8 encoded and converted to wide chars internally.
     * On Linux/macOS UTF-8 paths work natively. */
    tmpDir = jm_mk_temp_dir(&cb, NULL, "\xc3\xa9t\xc3\xa9"); /* été in UTF-8 */
    TEST_ASSERT(tmpDir != NULL, "jm_mk_temp_dir with Unicode prefix");
    if (tmpDir) {
        printf("  Temp dir: %s\n", tmpDir);
    } else {
        printf("  Temp dir creation failed, skipping remaining tests\n");
        return 1;
    }

    /* Test mkdir with a Unicode path */
    {
        size_t len = strlen(tmpDir);
        subDir = (char*)cb.malloc(len + 20);
        sprintf(subDir, "%s%st\xc3\xa9st_dir", tmpDir, FMI_FILE_SEP); /* test_dir with accent */
        status = jm_mkdir(&cb, subDir);
        TEST_ASSERT(status == jm_status_success, "jm_mkdir with Unicode path");
    }

    /* Test chdir/getcwd with Unicode path */
    {
        char cwd[FILENAME_MAX + 2];
        status = jm_portability_set_current_working_directory(subDir);
        TEST_ASSERT(status == jm_status_success, "jm_portability_set_current_working_directory with Unicode path");
        if (status == jm_status_success) {
            status = jm_portability_get_current_working_directory(cwd, FILENAME_MAX + 1);
            TEST_ASSERT(status == jm_status_success, "jm_portability_get_current_working_directory after Unicode chdir");
            /* Restore original directory */
            jm_portability_set_current_working_directory(tmpDir);
        }
    }

    /* Test zip extraction to a Unicode path */
    {
        zipPath = (char*)cb.malloc(strlen(FMIL_TEST_DIR) + 40);
        sprintf(zipPath, "%s/try_to_uncompress_this_file.zip", FMIL_TEST_DIR);

        extractPath = (char*)cb.malloc(strlen(tmpDir) + 30);
        sprintf(extractPath, "%s%sextract_\xc3\xa9t\xc3\xa9", tmpDir, FMI_FILE_SEP);
        status = jm_mkdir(&cb, extractPath);
        if (status == jm_status_success) {
            status = fmi_zip_unzip(zipPath, extractPath, &cb);
            TEST_ASSERT(status == jm_status_success, "fmi_zip_unzip to Unicode path");
        }
    }

    /* Cleanup */
    if (subDir) {
        jm_rmdir(&cb, subDir);
        cb.free(subDir);
    }
    if (extractPath) {
        jm_rmdir(&cb, extractPath);
        cb.free(extractPath);
    }
    if (zipPath) cb.free(zipPath);
    if (tmpDir) {
        jm_rmdir(&cb, tmpDir);
        cb.free(tmpDir);
    }

    printf("\n=== %s ===\n", failures ? "SOME TESTS FAILED" : "ALL TESTS PASSED");
    return failures ? 1 : 0;
}
