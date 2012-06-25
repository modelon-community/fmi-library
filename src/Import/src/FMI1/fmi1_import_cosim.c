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
#include <FMI1/fmi1_xml_model_description.h>
#include "fmi1_import_impl.h"

fmi1_fmu_kind_enu_t fmi1_import_get_fmu_kind(fmi1_import_t* fmu) {
    return fmi1_xml_get_fmu_kind(fmu->md);
}

fmi1_import_capabilities_t* fmi1_import_get_capabilities(fmi1_import_t* fmu){
    return fmi1_xml_get_capabilities(fmu->md);
}

size_t fmi1_import_get_number_of_additional_models(fmi1_import_t* fmu) {
    return fmi1_xml_get_number_of_additional_models(fmu->md);
}

const char* fmi1_import_get_additional_model_name(fmi1_import_t* fmu, size_t index) {
	return fmi1_xml_get_additional_model_name(fmu->md, index);
}


const char* fmi1_import_get_entry_point(fmi1_import_t* fmu) {
	return fmi1_xml_get_entry_point(fmu->md);
}

const char* fmi1_import_get_mime_type(fmi1_import_t* fmu){
    return fmi1_xml_get_mime_type(fmu->md);
}

int fmi1_import_get_manual_start(fmi1_import_t* fmu){
    return fmi1_xml_get_manual_start(fmu->md);
}
