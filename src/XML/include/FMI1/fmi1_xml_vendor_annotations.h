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

/** \file fmi1_xml_vendor_annotations.h
*  \brief Public interface to the FMI XML C-library. Handling of vendor annotations.
*/

#ifndef FMI1_XML_VENDORANNOTATIONS_H_
#define FMI1_XML_VENDORANNOTATIONS_H_

#include "fmi1_xml_model_description.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Basic support for vendor annotations. */

fmi1_xml_vendor_list_t* fmi1_xml_get_vendor_list(fmi1_xml_model_description_t* md);

unsigned int  fmi1_xml_get_number_of_vendors(fmi1_xml_vendor_list_t*);

fmi1_xml_vendor_t* fmi1_xml_get_vendor(fmi1_xml_vendor_list_t*, unsigned int  index);

/* fmi1_xml_vendor* fmiAddVendor(fmiModelDescription* md, char* name);

void* fmiRemoveVendor(fmi1_xml_vendor*); */

const char* fmi1_xml_get_vendor_name(fmi1_xml_vendor_t*);

unsigned int  fmi1_xml_get_number_of_vendor_annotations(fmi1_xml_vendor_t*);

/*Note: Annotations can be used in other places but have common interface name-value */
fmi1_xml_annotation_t* fmi1_xml_get_vendor_annotation(fmi1_xml_vendor_t*, unsigned int  index);

const char* fmi1_xml_get_annotation_name(fmi1_xml_annotation_t*);

const char* fmi1_xml_get_annotation_value(fmi1_xml_annotation_t*);

/* fmi1_xml_annotation* fmi1_xml_add_vendor_annotation(fmi1_xml_vendor*, const char* name, const char* value);

fmi1_xml_annotation* fmi1_xml_remove_vendor_annotation(fmi1_xml_vendor*, const char* name, const char* value);
*/

#ifdef __cplusplus
}
#endif
#endif
