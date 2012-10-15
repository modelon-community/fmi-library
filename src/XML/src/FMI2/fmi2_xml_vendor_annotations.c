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

#include <string.h>

#include "fmi2_xml_parser.h"
#include "fmi2_xml_model_description_impl.h"

static const char* module = "FMI2XML";


int fmi2_xml_handle_VendorAnnotations(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
 		jm_log_verbose(context->callbacks, module, "Parsing XML element VendorAnnotations");
	}
    else {
        /* might give out a warning if(data[0] != 0) */
    }
    return 0;
}

int fmi2_xml_handle_Annotations(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
 		jm_log_verbose(context->callbacks, module, "Parsing XML element Annotations");
	}
    else {
        /* might give out a warning if(data[0] != 0) */
    }
    return 0;
}

int fmi2_xml_handle_VariableTool(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
			size_t len;
            fmi2_xml_model_description_t* md = context->modelDescription;
            jm_vector(char)* bufName = fmi2_xml_reserve_parse_buffer(context,1,100);
            jm_string *pvendor;
			char* vendor = 0;
			
            if(!bufName) return -1;
            /* <xs:attribute name="name" type="xs:normalizedString" use="required"> */
            if( fmi2_xml_set_attr_string(context, fmi2_xml_elmID_Tool, fmi_attr_id_name, 1, bufName))
				return -1;
            pvendor = jm_vector_push_back(jm_string)(&md->vendorList, vendor);
			len = jm_vector_get_size(char)(bufName);
            if(pvendor )
                *pvendor = vendor = (char*)(context->callbacks->malloc(len + 1));
	        if(!pvendor || !vendor) {
	            fmi2_xml_parse_fatal(context, "Could not allocate memory");
		        return -1;
			}
            memcpy(vendor, jm_vector_get_itemp(char)(bufName,0), len);
            vendor[len] = 0;

			context->anyToolName = vendor;
			context->anyParent = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
			context->useAnyHandleFlg = 1;
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
		context->useAnyHandleFlg = 0;
        return 0;
    }
    return 0;
}

int fmi2_xml_handle_Tool(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
			size_t len;
            fmi2_xml_model_description_t* md = context->modelDescription;
            jm_vector(char)* bufName = fmi2_xml_reserve_parse_buffer(context,1,100);
            jm_string *pvendor;
			char* vendor = 0;
			
            if(!bufName) return -1;
            /* <xs:attribute name="name" type="xs:normalizedString" use="required"> */
            if( fmi2_xml_set_attr_string(context, fmi2_xml_elmID_Tool, fmi_attr_id_name, 1, bufName))
				return -1;
            pvendor = jm_vector_push_back(jm_string)(&md->vendorList, vendor);
			len = jm_vector_get_size(char)(bufName);
            if(pvendor )
                *pvendor = vendor = (char*)(context->callbacks->malloc(len + 1));
	        if(!pvendor || !vendor) {
	            fmi2_xml_parse_fatal(context, "Could not allocate memory");
		        return -1;
			}
            memcpy(vendor, jm_vector_get_itemp(char)(bufName,0), len);
            vendor[len] = 0;

			context->anyToolName = vendor;
			context->anyParent = 0;
			context->useAnyHandleFlg = 1;
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
		context->useAnyHandleFlg = 0;
        return 0;
    }
    return 0;
}

