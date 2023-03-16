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



/** \file fmi3_xml_dimension.h
*  \brief Public interface to the FMI XML C-library. Handling of variable dimensions.
*/

#ifndef FMI3_XML_DIMENSION_H_
#define FMI3_XML_DIMENSION_H_

#include <JM/jm_vector.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct fmi3_xml_dimension_t fmi3_xml_dimension_t;
typedef struct jm_vector(fmi3_xml_dimension_t) jm_vector(fmi3_xml_dimension_t); /* TODO: I believe this is causing warnings on gcc when we declare all the functions with the template (redefinition) */

int fmi3_xml_get_dimension_has_vr(fmi3_xml_dimension_t* dim);
int fmi3_xml_get_dimension_has_start(fmi3_xml_dimension_t* dim);
fmi3_uint64_t fmi3_xml_get_dimension_start(fmi3_xml_dimension_t* dim);
fmi3_value_reference_t fmi3_xml_get_dimension_vr(fmi3_xml_dimension_t* dim);

#ifdef __cplusplus
}
#endif

#endif /* FMI3_XML_DIMENSION_H_ */
