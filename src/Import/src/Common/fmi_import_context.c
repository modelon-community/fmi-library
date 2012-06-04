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

#include <stdio.h>
#include <stdarg.h>

#include <Common/jm_named_ptr.h>
#include <Common/fmi_import_context.h>
#include <fmi_zip_unzip.h>
#include <Common/fmi_import_util.h>

#include "fmi_import_context_impl.h"

#define MODULE "FMILIB"

fmi_import_context_t* fmi_import_allocate_context( jm_callbacks* callbacks) {
	jm_log_debug(callbacks, MODULE, "Inside fmi_import_allocate_context");
	return fmi_xml_allocate_context(callbacks);
}

void fmi_import_free_context( fmi_import_context_t* c) {
	fmi_xml_free_context(c);
}


fmi_version_enu_t fmi_import_get_fmi_version( fmi_import_context_t* c, const char* fileName, const char* dirName) {
	fmi_version_enu_t ret = fmi_version_unknown_enu;
	jm_status_enu_t status= fmi_zip_unzip(fileName, dirName, c->callbacks);
	char* mdpath;
	if(status == jm_status_error) return fmi_version_unknown_enu;
	mdpath = fmi_import_get_model_description_path(dirName, c->callbacks);
	ret = fmi_xml_get_fmi_version(c, mdpath);
	c->callbacks->free(mdpath);
	return ret;
}
