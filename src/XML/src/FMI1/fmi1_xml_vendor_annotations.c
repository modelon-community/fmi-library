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

#include "fmi1_xml_parser.h"
#include "fmi1_xml_model_description_impl.h"
#include "fmi1_xml_vendor_annotations_impl.h"

static const char* module = "FMI1XML";

void fmi1_xml_vendor_free(fmi1_xml_vendor_t* v) {
    jm_named_vector_free_data(&v->annotations);
    v->annotations.callbacks->free(v);
}

const char* fmi1_xml_get_vendor_name(fmi1_xml_vendor_t* v) {
    return v->name;
}

unsigned int  fmi1_xml_get_number_of_vendor_annotations(fmi1_xml_vendor_t* v) {
    return (unsigned int)jm_vector_get_size(jm_named_ptr)(&v->annotations);
}

fmi1_xml_annotation_t* fmi1_xml_get_vendor_annotation(fmi1_xml_vendor_t* v, unsigned int  index) {
    if(index >= fmi1_xml_get_number_of_vendor_annotations(v)) return 0;
    return jm_vector_get_item(jm_named_ptr)(&v->annotations, index).ptr;
}

const char* fmi1_xml_get_annotation_name(fmi1_xml_annotation_t* a) {
    return a->name;
}

const char* fmi1_xml_get_annotation_value(fmi1_xml_annotation_t* a) {
    return a->value;
}

int fmi1_xml_handle_VendorAnnotations(fmi1_xml_parser_context_t *context, const char* data) {
    if(!data) {
 		jm_log_verbose(context->callbacks, module, "Parsing XML element VendorAnnotations");
	}
    else {
        /* might give out a warning if(data[0] != 0) */
    }
    return 0;
}

int fmi1_xml_handle_Tool(fmi1_xml_parser_context_t *context, const char* data) {
    if(!data) {
            fmi1_xml_model_description_t* md = context->modelDescription;
            jm_vector(char)* bufName = fmi1_xml_reserve_parse_buffer(context,1,100);
            fmi1_xml_vendor_t* vendor = 0;
            fmi1_xml_vendor_t dummyV;
            jm_voidp *pvendor;

            if(!bufName) return -1;
            /* <xs:attribute name="name" type="xs:normalizedString" use="required"> */
            if( fmi1_xml_set_attr_string(context, fmi1_xml_elmID_Tool, fmi_attr_id_name, 1, bufName)) return -1;
            pvendor = jm_vector_push_back(jm_voidp)(&md->vendorList, vendor);
            if(pvendor )
                *pvendor = vendor = jm_named_alloc_v(bufName,sizeof(fmi1_xml_vendor_t), dummyV.name - (char*)&dummyV, context->callbacks).ptr;
            if(!pvendor || !vendor) {
                fmi1_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            jm_vector_init(jm_named_ptr)(&vendor->annotations,0, context->callbacks);
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}
int fmi1_xml_handle_Annotation(fmi1_xml_parser_context_t *context, const char* data) {
    if(!data) {
            fmi1_xml_model_description_t* md = context->modelDescription;
            size_t numVendors = jm_vector_get_size(jm_voidp)(&(md->vendorList));
            fmi1_xml_vendor_t* vendor =(fmi1_xml_vendor_t*)jm_vector_get_item(jm_voidp)(&(md->vendorList), numVendors-1);
            jm_vector(char)* bufName = fmi1_xml_reserve_parse_buffer(context,1,100);
            jm_vector(char)* bufValue = fmi1_xml_reserve_parse_buffer(context,2,100);
            jm_named_ptr named, *pnamed;
            fmi1_xml_annotation_t* annotation = 0;
            size_t vallen;

            if(!bufName || !bufValue ||
            /*     <xs:attribute name="name" type="xs:normalizedString" use="required"/> */
                fmi1_xml_set_attr_string(context, fmi1_xml_elmID_Annotation, fmi_attr_id_name, 1, bufName) ||
            /* <xs:attribute name="value" type="xs:string" use="required"/> */
                fmi1_xml_set_attr_string(context, fmi1_xml_elmID_Annotation, fmi_attr_id_value, 1, bufValue)
                    )
                return -1;
            vallen = jm_vector_get_size(char)(bufValue);
            named.ptr = 0;
			named.name = 0;
            pnamed = jm_vector_push_back(jm_named_ptr)(&vendor->annotations, named);

            if(pnamed) *pnamed = named = jm_named_alloc_v(bufName,sizeof(fmi1_xml_annotation_t)+vallen+1,sizeof(fmi1_xml_annotation_t)+vallen,context->callbacks);
            annotation = named.ptr;
            if( !pnamed || !annotation ) {
                fmi1_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            annotation->name = named.name;
            if(vallen)
                memcpy(annotation->value,jm_vector_get_itemp(char)(bufValue,0), vallen);
            annotation->value[vallen] = 0;
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

