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
#include "fmi3_xml_unit_struct_defs.h"
#include "fmi3_xml_parser_handler.h"

#ifdef __cplusplus
extern "C" {
#endif

fmi3_xml_display_unit_t* fmi3_xml_get_parsed_unit(fmi3_xml_parser_context_t* context, jm_vector(char)* name, int sorted);

#ifdef __cplusplus
}
#endif

#endif /* FMI3_XML_UNITIMPL_H */
