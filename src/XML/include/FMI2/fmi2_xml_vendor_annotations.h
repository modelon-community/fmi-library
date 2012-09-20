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

/** \file fmi2_xml_vendor_annotations.h
*  \brief Public interface to the FMI XML C-library. Handling of vendor annotations.
*/

#ifndef FMI2_XML_VENDORANNOTATIONS_H_
#define FMI2_XML_VENDORANNOTATIONS_H_

#include "fmi2_xml_model_description.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Basic support for vendor annotations. */

fmi2_xml_vendor_list_t* fmi2_xml_get_vendor_list(fmi2_xml_model_description_t* md);

unsigned int  fmi2_xml_get_number_of_vendors(fmi2_xml_vendor_list_t*);

fmi2_xml_vendor_t* fmi2_xml_get_vendor(fmi2_xml_vendor_list_t*, unsigned int  index);

/* fmi2_xml_vendor* fmiAddVendor(fmiModelDescription* md, char* name);

void* fmiRemoveVendor(fmi2_xml_vendor*); */

const char* fmi2_xml_get_vendor_name(fmi2_xml_vendor_t*);

#ifdef __cplusplus
}
#endif
#endif
