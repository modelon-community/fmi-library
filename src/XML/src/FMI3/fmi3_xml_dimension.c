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
#include "FMI3/fmi3_xml_dimension.h"

#include "FMI3/fmi3_xml_variable.h"

static const char* module = "FMI3XML";

/* define fmi3_xml_dimension_t vector functions */
#define JM_TEMPLATE_INSTANCE_TYPE fmi3_xml_dimension_t
#include "JM/jm_vector_template.h"
#undef JM_TEMPLATE_INSTANCE_TYPE

/**
 * Get size as defined in XML (start attribute is returned for resolved VRs)
 */
unsigned int fmi3_xml_dimension_get_size(fmi3_xml_model_description_t* md, fmi3_xml_dimension_t dim) {
    if (!dim.has_vr) {
        return dim.start;
    } else {
        fmi3_integer_t vr = dim.vr;
        fmi3_xml_integer_variable_t* varRef = fmi3_xml_get_variable_as_integer(fmi3_xml_get_variable_by_vr(md, fmi3_base_type_int, vr));
        
        fmi3_integer_t start = fmi3_xml_get_integer_variable_start(varRef);
        if (start == 0) {
            jm_log_error(md->callbacks, module, "Start value for variable used as dimension length was 0. VR: '%d'", vr);
            assert(0); /* TODO: how are errors (fatals) handled? */
        }

        return start;
    }
}

