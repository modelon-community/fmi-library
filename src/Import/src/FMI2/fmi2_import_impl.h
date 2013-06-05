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



/** \file fmi2_import_impl.h
*/

#ifndef FMI2_IMPORT_IMPL_H_
#define FMI2_IMPORT_IMPL_H_


#include <FMI2/fmi2_import.h>
#include <FMI2/fmi2_xml_model_description.h>

#include "../FMI/fmi_import_context_impl.h"
#include "../src/FMI2/fmi2_capi_impl.h"


#ifdef __cplusplus
extern "C" {
#endif

struct fmi2_import_t {	
	char* dirPath;
	char* resourceLocation;
	jm_callbacks* callbacks;
	fmi2_xml_model_description_t* md;
	fmi2_capi_t* capi;
	jm_vector(char) logMessageBufferCoded;
	jm_vector(char) logMessageBufferExpanded;
};

#ifdef __cplusplus
}
#endif
#endif
