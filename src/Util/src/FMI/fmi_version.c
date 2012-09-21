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

#include <assert.h>
#include <FMI/fmi_version.h>

static const char* fmi_versions[] = {
	"1.0",
	"2.0"
};

const char * fmi_version_to_string(fmi_version_enu_t v){
	if( (v <= fmi_version_unknown_enu) || (v > fmi_version_unsupported_enu - 1)) {
		return "unknown";
	}
	return fmi_versions[v - 1];
}

