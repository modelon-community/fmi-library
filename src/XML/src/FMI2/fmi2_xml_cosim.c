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
#include <JM/jm_vector.h>
#include "fmi2_xml_model_description_impl.h"
#include "fmi2_xml_parser.h"

static const char * module = "FMI2XML";

fmi2_fmu_kind_enu_t fmi2_xml_get_fmu_kind(fmi2_xml_model_description_t* md) {
    return md->fmuKind;
}

unsigned int fmi2_xml_get_capability(fmi2_xml_model_description_t* md, fmi2_capabilities_enu_t id) {
	assert((id >= 0) && (id < fmi2_capabilities_Num));
	return md->capabilities[id];
}

size_t fmi2_xml_get_number_of_additional_models(fmi2_xml_model_description_t* md) {
    return jm_vector_get_size(jm_string)(&md->additionalModels);
}

const char* fmi2_xml_get_additional_model_name(fmi2_xml_model_description_t* md, size_t index) {
    if(fmi2_xml_get_number_of_additional_models(md) <= index) return 0;
    return jm_vector_get_item(jm_string)(&md->additionalModels,index);
}


const char* fmi2_xml_get_entry_point(fmi2_xml_model_description_t* md) {
    return jm_vector_char2string(&md->entryPoint);
}

const char* fmi2_xml_get_mime_type(fmi2_xml_model_description_t* md){
    return jm_vector_char2string(&md->mimeType);
}

int fmi2_xml_get_manual_start(fmi2_xml_model_description_t* md){
    return md->manual_start;
}

