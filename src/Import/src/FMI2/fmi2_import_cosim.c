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
#include <FMI2/fmi2_xml_model_description.h>
#include "fmi2_import_impl.h"

fmi2_fmu_kind_enu_t fmi2_import_get_fmu_kind(fmi2_import_t* fmu) {
    return fmi2_xml_get_fmu_kind(fmu->md);
}

size_t fmi2_import_get_number_of_additional_models(fmi2_import_t* fmu) {
    return fmi2_xml_get_number_of_additional_models(fmu->md);
}

const char* fmi2_import_get_additional_model_name(fmi2_import_t* fmu, size_t index) {
	return fmi2_xml_get_additional_model_name(fmu->md, index);
}


const char* fmi2_import_get_entry_point(fmi2_import_t* fmu) {
	return fmi2_xml_get_entry_point(fmu->md);
}

const char* fmi2_import_get_mime_type(fmi2_import_t* fmu){
    return fmi2_xml_get_mime_type(fmu->md);
}

int fmi2_import_get_manual_start(fmi2_import_t* fmu){
    return fmi2_xml_get_manual_start(fmu->md);
}
