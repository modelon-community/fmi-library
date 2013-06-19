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



/** \file fmi1_import_impl.h
*/

#ifndef FMI1_IMPORT_IMPL_H_
#define FMI1_IMPORT_IMPL_H_


#include <FMI1/fmi1_import.h>
#include <FMI1/fmi1_xml_model_description.h>

#include "../FMI/fmi_import_context_impl.h"
#include "../src/FMI1/fmi1_capi_impl.h"


#ifdef __cplusplus
extern "C" {
#endif

struct fmi1_import_t {	
	char* dirPath;
	char* location;
	jm_callbacks* callbacks;
	fmi1_xml_model_description_t* md;
	fmi1_capi_t* capi;
	int registerGlobally;
	jm_vector(char) logMessageBufferCoded;
	jm_vector(char) logMessageBufferExpanded;
};

extern jm_callbacks fmi1_import_active_fmu_store_callbacks;

extern jm_vector(jm_voidp) fmi1_import_active_fmu_store;

extern jm_vector(jm_voidp)* fmi1_import_active_fmu;

#ifdef __cplusplus
}
#endif
#endif
