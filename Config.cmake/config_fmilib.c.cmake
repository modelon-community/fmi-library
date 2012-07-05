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

#include "fmilib_config.h"

const char * fmilib_build_stamp = 
	"Built on: " __DATE__ " " __TIME__ "\n"
	"Last changed on: @FMILibrary_WC_LAST_CHANGED_DATE@\n"
	"Revision: @FMILibrary_WC_REVISION@";

	
const char* fmilib_get_build_stamp(void) {
	return fmilib_build_stamp;
}