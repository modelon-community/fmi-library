/*
    Copyright (C) 2012 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "fmilib_config.h"

const char * fmilib_build_stamp = 
	"Built on: " __DATE__ " " __TIME__ "\n"
	"Last changed on: @FMILibrary_WC_LAST_CHANGED_DATE@\n"
	"Revision: @FMILibrary_WC_REVISION@";

	
const char* fmilib_get_build_stamp(void) {
	FMILIB_TRACE("Inside fmilib_get_build_stamp\n");
	return fmilib_build_stamp;
}