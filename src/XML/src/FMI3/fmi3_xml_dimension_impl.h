/*
    Copyright (C) 2023 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#ifndef FMI3_XML_DIMENSION_IMPL_H_
#define FMI3_XML_DIMENSION_IMPL_H_

#include "JM/jm_vector.h"
#include "FMI3/fmi3_xml_dimension.h"

#ifdef __cplusplus
extern "C" {
#endif

struct fmi3_xml_dimension_t {
    int has_vr;
    fmi3_uint64_t start;           /* value of the start attribute if 'has_vr' is false, else unassigned */
    fmi3_uint32_t vr;              /* value of the valueReference attribute if 'has_vr' is true, else unassigned */
};

/* declare fmi3_xml_dimension_t vector functions */
jm_vector_declare_template_no_typedef(fmi3_xml_dimension_t)

#ifdef __cplusplus
}
#endif

#endif /* FMI3_XML_DIMENSION_IMPL_H_ */

