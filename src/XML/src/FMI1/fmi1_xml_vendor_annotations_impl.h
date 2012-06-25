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

#ifndef FMI1_XML_VENDORANNOTATIONS_H
#define FMI1_XML_VENDORANNOTATIONS_H

#include <JM/jm_named_ptr.h>

#include <FMI1/fmi1_xml_model_description.h>

#ifdef __cplusplus
extern "C" {
#endif

struct fmi1_xml_annotation_t {
    const char* name;
    char value[1];
};

struct fmi1_xml_vendor_t {
    jm_vector(jm_named_ptr) annotations;
    char name[1];
};

void fmi1_xml_vendor_free(fmi1_xml_vendor_t* v);

struct fmi1_xml_vendor_list_t {
    jm_vector(jm_voidp) vendors;
};

#ifdef __cplusplus
}
#endif

#endif /* FMI1_XML_VENDORANNOTATIONS_H */
