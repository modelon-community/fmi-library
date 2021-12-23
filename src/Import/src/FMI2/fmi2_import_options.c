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

#include "FMI2/fmi2_import.h"
#include <JM/jm_portability.h>

#include "fmi2_import_impl.h"


static const char* module = "FMILIB";

fmi_import_options_t* fmi2_import_get_options(fmi2_import_t* fmu) {
    if (fmu->options) {
        return fmu->options;
    } else {
        /* Options ownership has been moved to CAPI */
        return fmu->capi->options;
    }
}

/* TODO: Move this to correct file (fmi and not fmi2) */
void fmi_import_set_option_loadlibrary_flag(fmi_import_options_t* options, jm_portability_loadlibrary_flag_t flag) {
    options->loadlibrary_flag = flag;
}

