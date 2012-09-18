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

fmi2_xml_capabilities_t* fmi2_xml_get_capabilities(fmi2_xml_model_description_t* md){
    return &md->capabilities;
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


int fmi2_xml_handle_Implementation(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
		jm_log_info(context->callbacks, module, "Processing implementation element (co-simulation FMU detected)");
    }
    else {
        /* might give out a warning if(data[0] != 0) */
    }
    return 0;
}

int fmi2_xml_handle_CoSimulation_StandAlone(fmi2_xml_parser_context_t *context, const char* data) {
    fmi2_xml_model_description_t* md = context->modelDescription;
    if(!data) {
        md->fmuKind = fmi2_fmu_kind_enu_cs_standalone;
    }
    else {
        /* might give out a warning if(data[0] != 0) */
    }
    return 0;
}



int fmi2_xml_handle_CoSimulation_Tool(fmi2_xml_parser_context_t *context, const char* data) {
    fmi2_xml_model_description_t* md = context->modelDescription;
    if(!data) {
        md->fmuKind = fmi2_fmu_kind_enu_cs_tool;
    }
    else {
        /* might give out a warning if(data[0] != 0) */
    }
    return 0;
}

int fmi2_xml_handle_Model(fmi2_xml_parser_context_t *context, const char* data) {
    fmi2_xml_model_description_t* md = context->modelDescription;
    if(!data) {
        return (
        /* <xs:attribute name="entryPoint"  type="xs:anyURI" use="required"/> */
        fmi2_xml_set_attr_string(context, fmi2_xml_elmID_Model, fmi_attr_id_entryPoint, 1, &(md->entryPoint)) ||
        /* <xs:attribute name="manualStart" type="xs:boolean" default="false"/> */
        fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_Model,fmi_attr_id_manualStart,0,&md->manual_start,0) ||
        /* <xs:attribute name="type"        type="xs:string" use="required"/> */
        fmi2_xml_set_attr_string(context, fmi2_xml_elmID_Model, fmi_attr_id_type,1,&md->mimeType)
                    );
    }
    else {
        /* might give out a warning if(data[0] != 0) */
    }
    return 0;
}

int fmi2_xml_handle_File(fmi2_xml_parser_context_t *context, const char* data) {
    fmi2_xml_model_description_t* md = context->modelDescription;
    if(!data) {
            jm_vector(char)* bufFileName = fmi2_xml_get_parse_buffer(context,2);
            char* fileName = 0;
            jm_string* pname;
            size_t len;
            if(fmi2_xml_set_attr_string(context, fmi2_xml_elmID_Model, fmi_attr_id_file, 1, bufFileName))
                return -1;
            len = jm_vector_get_size_char(bufFileName);
            pname = jm_vector_push_back(jm_string)(&md->additionalModels,fileName);
            if(pname) *pname = fileName =  md->callbacks->malloc(len + 1);
            if(!pname || !fileName) {
                fmi2_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            memcpy(fileName, jm_vector_get_itemp(char)(bufFileName,0), len);
            fileName[len] = 0;
    }
    else {
        /* might give out a warning if(data[0] != 0) */
    }
    return 0;
}

