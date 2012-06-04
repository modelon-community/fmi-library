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



/** \file fmi1_import_impl.h
*/

#ifndef FMI1_IMPORT_IMPL_H_
#define FMI1_IMPORT_IMPL_H_


#include <FMI1/fmi1_import.h>
#include <FMI1/fmi1_xml_model_description.h>

#include "../Common/fmi_import_context_impl.h"
#include "../src/FMI1/fmi1_capi_impl.h"


#ifdef __cplusplus
extern "C" {
#endif

struct fmi1_import_t {	
	char* dirPath;
	jm_callbacks* callbacks;
	fmi1_xml_model_description_t* md;
	fmi1_capi_t* capi;
	jm_vector(char) logMessageBuffer;
};

extern jm_vector(jm_voidp) fmi1_import_active_fmu_store;

extern jm_vector(jm_voidp)* fmi1_import_active_fmu;

#ifdef __cplusplus
}
#endif
#endif
