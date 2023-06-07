/*
    Copyright (C) 2012-2023 Modelon AB

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

#include <config_test.h>
#include <fmilib.h>

#define BUFFER 1000

extern int fmi1_test(fmi_import_context_t* context, const char* dirPath);
extern int fmi2_test(fmi_import_context_t* context, const char* dirPath);
extern int fmi3_test(fmi_import_context_t* context, const char* dirPath);

void importlogger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message)
{
        printf("module = %s, log level = %d: %s\n", module, log_level, message);
}

void do_exit(int code)
{
    printf("Press 'Enter' to exit\n");
    /* getchar(); */
    exit(code);
}

int main(int argc, char *argv[])
{
    const char* FMUPath;
    const char* tmpPath;
    jm_callbacks callbacks;
    fmi_import_context_t* context;
    fmi_version_enu_t version;
    int ret;

    if(argc < 2) {
        printf("Usage: %s <fmu_file>\n", argv[0]);
        do_exit(CTEST_RETURN_FAIL);
    }

    FMUPath = argv[1];

    callbacks.malloc = malloc;
    callbacks.calloc = calloc;
    callbacks.realloc = realloc;
    callbacks.free = free;
    callbacks.logger = importlogger;
    callbacks.log_level = jm_log_level_all;
    callbacks.context = 0;

#ifdef FMILIB_GENERATE_BUILD_STAMP
    printf("Library build stamp:\n%s\n", fmilib_get_build_stamp());
#endif

    tmpPath = fmi_import_mk_temp_dir(&callbacks, FMU_UNPACK_DIR, NULL);
    if (!tmpPath) {
        printf("Failed to create temporary directory in: " FMU_UNPACK_DIR "\n");
        do_exit(CTEST_RETURN_FAIL);
    }
    context = fmi_import_allocate_context(&callbacks);
    version = fmi_import_get_fmi_version(context, FMUPath, tmpPath);

    /* Check that version can be retrieved from unzipped FMU: */
    if (version != fmi_import_get_fmi_version(context, NULL, tmpPath)) {
        printf("Failure: different FMI version when retrieved from unzipped FMU.\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    if(version == fmi_version_1_enu) {
        ret = fmi1_test(context, tmpPath);
    }
    else if(version == fmi_version_2_0_enu) {
        ret = fmi2_test(context, tmpPath);
    }
    else if(version == fmi_version_3_0_enu) {
        ret = fmi3_test(context, tmpPath);
    }
    else {
        fmi_import_free_context(context);
        printf("Only versions 1.0, 2.0 and 3.0 are supported so far\n");
        do_exit(CTEST_RETURN_FAIL);
    }

    fmi_import_free_context(context);
    if (fmi_import_rmdir(&callbacks, tmpPath)) {
        printf("Problem when deleting FMU unpack directory.\n");
        do_exit(CTEST_RETURN_FAIL);
    }
    callbacks.free((void*)tmpPath);

    if(ret == CTEST_RETURN_SUCCESS)
        printf("Everything seems to be OK since you got this far=)!\n");

    do_exit(ret);

    return 0;
}
