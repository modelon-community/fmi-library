/*
    Copyright (C) 2021 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#include "FMI/fmi_util_options.h"
#include "JM/jm_portability.h"

#include "../src/FMI/fmi_util_options_impl.h"


static const char* MODULE = "FMIUT";

fmi_util_options_t* fmi_util_allocate_options(jm_callbacks* cb) {
    fmi_util_options_t* opts = cb->malloc(sizeof(fmi_util_options_t));
    if (!opts) {
        jm_log_fatal(cb, MODULE, "Could not allocate memory");
        return NULL;
    }

    opts->loadlibrary_flag = jm_portability_get_load_dll_handle_default_flag();

    return opts;
}

void fmi_util_free_options(jm_callbacks* cb, fmi_util_options_t* opts) {
    if (!opts) {
        return;
    }
    cb->free(opts);
}
