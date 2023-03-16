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

#include <JM/jm_callbacks.h>
#include "fmi3_import_impl.h"
#include "fmi3_import_priv.h"

int fmi3_import_check_has_FMU(fmi3_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, "FMILIB", "No FMU is loaded");
		return 0;
	}
	return 1;
}
