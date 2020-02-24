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

#include "JM/jm_vector.h"
#include "fmi3_xml_model_description_impl.h"
#include "fmi3_xml_dimension_impl.h"

#include "FMI3/fmi3_xml_variable.h"

static const char* module = "FMI3XML";

/* define fmi3_xml_dimension_t vector functions */
#define JM_TEMPLATE_INSTANCE_TYPE fmi3_xml_dimension_t
#include "JM/jm_vector_template.h"
#undef JM_TEMPLATE_INSTANCE_TYPE


/* wrapables for import side: */
int fmi3_xml_get_dimension_has_vr(fmi3_xml_dimension_t* dim) {
    return dim->has_vr;
}
int fmi3_xml_get_dimension_has_start(fmi3_xml_dimension_t* dim) {
    return !dim->has_vr;
}
int fmi3_xml_get_dimension_start(fmi3_xml_dimension_t* dim) {
    return dim->start;
}
fmi3_value_reference_t fmi3_xml_get_dimension_vr(fmi3_xml_dimension_t* dim) {
    return dim->vr;
}

