/*
    Copyright (C) 2012 Modelon AB

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

#include "FMI/fmi_minizip.h"

#include <JM/jm_types.h>
#include <JM/jm_callbacks.h>
#include <JM/jm_portability.h>

jm_status_enu_t fmi_zip_zip(const char* zip_file_path, int n_files_to_zip, const char** files_to_zip, jm_callbacks* callbacks)
{
    /* A call to minizip may change the current directory and therefore we must change it back */
    char cd[FILENAME_MAX];
    int status;

    /* Temporary save the current directory */
    if (jm_portability_get_current_working_directory(cd, sizeof(cd) / sizeof(char)) == jm_status_error) {
        jm_log(callbacks, "UNZIP", jm_log_level_error, "Could not get Current Directory");
        return jm_status_error;
    }

    /* Zip */
    status = fmi_minizip(zip_file_path, n_files_to_zip, files_to_zip);

    /* Reset the current directory */
    if (jm_portability_set_current_working_directory(cd) == jm_status_error) {
        jm_log(callbacks, "UNZIP", jm_log_level_warning, "Could not change back Current Directory");
        return jm_status_warning;
    }

    /* Return error status */
    if (status == 0) {
        return jm_status_success;
    } else {
        return jm_status_error;
    }
}
