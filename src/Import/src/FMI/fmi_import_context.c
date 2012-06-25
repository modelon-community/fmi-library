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
#include <stdarg.h>

#include <JM/jm_named_ptr.h>
#include <FMI/fmi_import_context.h>
#include <FMI/fmi_zip_unzip.h>
#include <FMI/fmi_import_util.h>

#include "fmi_import_context_impl.h"

#define MODULE "FMILIB"

fmi_import_context_t* fmi_import_allocate_context( jm_callbacks* callbacks) {
	jm_log_verbose(callbacks, MODULE, "Allocating FMIL context");
	return fmi_xml_allocate_context(callbacks);
}

void fmi_import_free_context( fmi_import_context_t* c) {
	fmi_xml_free_context(c);
}


fmi_version_enu_t fmi_import_get_fmi_version( fmi_import_context_t* c, const char* fileName, const char* dirName) {
	fmi_version_enu_t ret = fmi_version_unknown_enu;
	jm_status_enu_t status;
	char* mdpath;
	jm_log_verbose(c->callbacks, MODULE, "Detecting FMI standard version");
	if(!fileName || !*fileName) {
		jm_log_fatal(c->callbacks, MODULE, "No FMU filename specified");
		return fmi_version_unknown_enu;
	}
	if(!dirName || !*dirName) {
		jm_log_fatal(c->callbacks, MODULE, "No temporary directory name specified");
		return fmi_version_unknown_enu;
	}
	status = fmi_zip_unzip(fileName, dirName, c->callbacks);
	if(status == jm_status_error) return fmi_version_unknown_enu;
	mdpath = fmi_import_get_model_description_path(dirName, c->callbacks);
	ret = fmi_xml_get_fmi_version(c, mdpath);
	jm_log_info(c->callbacks, MODULE, "XML specifies FMI standard version %s", fmi_version_to_string(ret));
	c->callbacks->free(mdpath);
	return ret;
}
