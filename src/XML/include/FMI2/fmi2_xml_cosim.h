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

#ifndef FMI_XML_COSIM_H
#define FMI_XML_COSIM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "fmi2_xml_model_description.h"

const char* fmi2_xml_get_entry_point(fmi2_xml_model_description_t* );
const char* fmi2_xml_get_mime_type(fmi2_xml_model_description_t* );
int fmi2_xml_get_manual_start(fmi2_xml_model_description_t* );

size_t fmi2_xml_get_number_of_additional_models(fmi2_xml_model_description_t* md);

const char* fmi2_xml_get_additional_model_name(fmi2_xml_model_description_t* md, size_t index);

#ifdef __cplusplus
}
#endif
#endif /* FMI_XML_COSIM_H */
