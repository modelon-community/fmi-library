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

#ifndef FMI3_XML_UNITIMPL_H
#define FMI3_XML_UNITIMPL_H

#include <JM/jm_vector.h>
#include <JM/jm_named_ptr.h>
#include "FMI3/fmi3_xml_model_description.h"
#include "fmi3_xml_parser.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Structure encapsulating base unit information */

struct fmi3_xml_display_unit_t {
    double factor;
    double offset;
    unsigned int inverse;
    fmi3_xml_unit_t* baseUnit;
    char displayUnit[1];
};

struct fmi3_xml_unit_t {
        jm_vector(jm_voidp) displayUnits;
        int SI_base_unit_exp[fmi3_SI_base_units_Num];
        double factor;
        double offset;
        fmi3_xml_display_unit_t defaultDisplay;
        char baseUnit[1];
};

struct fmi3_xml_unit_definitions_t {
    jm_vector(jm_named_ptr) definitions;
};

fmi3_xml_display_unit_t* fmi3_xml_get_parsed_unit(fmi3_xml_parser_context_t *context, jm_vector(char)* name, int sorted);

#ifdef __cplusplus
}
#endif

#endif /* FMI3_XML_UNITIMPL_H */
