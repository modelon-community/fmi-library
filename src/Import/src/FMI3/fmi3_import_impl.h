/*
    Copyright (C) 2023 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

/** \file fmi3_import_impl.h
*/

#ifndef FMI3_IMPORT_IMPL_H_
#define FMI3_IMPORT_IMPL_H_


#include "FMI3/fmi3_import.h"
#include "FMI3/fmi3_xml_model_description.h"

#include "../FMI/fmi_import_context_impl.h"
#include "../src/FMI3/fmi3_capi_impl.h"
#include "../src/FMI/fmi_util_options_impl.h"


#ifdef __cplusplus
extern "C" {
#endif

/*
 * 'capi' contains references to the FMU's loaded CAPI functions. Is NULL if never loaded or terminated.
 */
struct fmi3_import_t {
    char* dirPath;
    char* resourcePath;
    char* instanceName;
    jm_callbacks* callbacks;
    fmi3_xml_model_description_t* md;
    fmi3_capi_t* capi;
    jm_vector(char) logMessageBufferCoded;
    jm_vector(char) logMessageBufferExpanded;
    fmi_util_options_t* options;
};

#ifdef __cplusplus
}
#endif
#endif
