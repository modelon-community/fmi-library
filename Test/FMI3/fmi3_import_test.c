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

#include <config_test.h>
#include <fmilib.h>
#include <JM/jm_portability.h>

/* KEEP THIS TEST AS PURE C TEST, DO NOT CONVERT TO C++ */

#define BUFFER 1000


/* Logger function used by the FMU internally */
static void fmi3logger(fmi3_instance_environment_t env, fmi3_status_t status, fmi3_string_t category, fmi3_string_t message)
{
    char msg[BUFFER];
    jm_snprintf(msg, BUFFER, message);
    printf("fmiStatus = %s;  %s: %s\n", fmi3_status_to_string(status), category, msg);
}

int fmi3_test(fmi_import_context_t* context, const char* dirPath)
{
    const char* modelIdentifier;
    const char* modelName;
    const char* instantiationToken;
    jm_status_enu_t status;

    fmi3_import_t* fmu;
    fmi3_fmu_kind_enu_t fmukind;

    fmu = fmi3_import_parse_xml(context, dirPath, 0);

    if(!fmu) {
        printf("Error parsing XML, exiting\n");
        return (CTEST_RETURN_FAIL);
    }
    modelName = fmi3_import_get_model_name(fmu);
    instantiationToken = fmi3_import_get_instantiation_token(fmu);

    printf("Model name: %s\n", modelName);
    if(fmi3_import_get_fmu_kind(fmu) == fmi3_fmu_kind_me) {
        modelIdentifier = fmi3_import_get_model_identifier_ME(fmu);
        printf("Model identifier for ME: %s\n", modelIdentifier);
        fmukind = fmi3_fmu_kind_me;
    }
    else if(fmi3_import_get_fmu_kind(fmu) == fmi3_fmu_kind_cs) {
        modelIdentifier = fmi3_import_get_model_identifier_CS(fmu);
        printf("Model identifier for CS: %s\n", modelIdentifier);
        fmukind = fmi3_fmu_kind_cs;
    }
    else if(fmi3_import_get_fmu_kind(fmu) == fmi3_fmu_kind_se) {
        modelIdentifier = fmi3_import_get_model_identifier_SE(fmu);
        printf("Model identifier for SE: %s\n", modelIdentifier);
        fmukind = fmi3_fmu_kind_se;
    }
    else {
        printf("Unxepected FMU kind, exiting\n");
        return (CTEST_RETURN_FAIL);
    }
    printf("Model instantiationToken: %s\n", instantiationToken);

    status = fmi3_import_create_dllfmu(fmu, fmukind, NULL, fmi3logger);
    if (status == jm_status_error) {
        printf("Could not create the DLL loading mechanism(C-API).\n");
        return(CTEST_RETURN_FAIL);
    }

    printf("Version returned from FMU:   %s\n", fmi3_import_get_version(fmu));

    fmi3_import_destroy_dllfmu(fmu);

    fmi3_import_free(fmu);

    return (CTEST_RETURN_SUCCESS);
}
