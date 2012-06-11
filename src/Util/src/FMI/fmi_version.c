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

#include <assert.h>
#include <Common/fmi_version.h>

static const char* fmi_versions[] = {
	"1.0",
	"2.0"
};

const char * fmi_version_to_string(fmi_version_enu_t v){
	if( (v <= fmi_version_unknown_enu) || (v > fmi_version_unsupported_enu - 1)) {
		assert(0);
		return "UNEXPECTED";
	}
	return fmi_versions[v - 1];
}

