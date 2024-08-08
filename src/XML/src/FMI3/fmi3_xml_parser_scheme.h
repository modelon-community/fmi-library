/*
    Copyright (C) 2024 Modelon AB
    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    FMILIB_License.txt file for more details.
    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

/*
This file collects and unifies the XML schemes to be used for parsing.
*/

#ifndef FMI3_XML_XMLPARSER_SCHEME_H
#define FMI3_XML_XMLPARSER_SCHEME_H

#include "../FMI/fmi_xml_terminals_and_icons_scheme.h"
#include "fmi3_xml_model_description_scheme.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef union fmi3_xml_attr_t {
    int any; // when used in generic parser/handling context
    fmi3_xml_attr_modelDescription_enu_t modelDescription;
    fmi_xml_attr_termIcon_enu_t termIcon;
} fmi3_xml_attr_t;

// Shortcuts for defining fmi3_xml_attr_t for a given enum value
#define FMI3_ATTR_ANY(enu)     (fmi3_xml_attr_t){.any = enu}
#define FMI3_ATTR(enu)         (fmi3_xml_attr_t){.modelDescription = enu}
#define FMI_ATTR_TERMICON(enu) (fmi3_xml_attr_t){.termIcon = enu}

typedef union fmi3_xml_elm_t {
    int any; // when used in generic parser/handling context
    fmi3_xml_elm_modelDescription_enu_t modelDescription;
    fmi_xml_elm_termIcon_enu_t termIcon;
} fmi3_xml_elm_t;

// Shortcuts for defining fmi3_xml_elm_t for a given enum value
#define FMI3_ELM_ANY(enu)     (fmi3_xml_elm_t){.any = enu}
#define FMI3_ELM(enu)         (fmi3_xml_elm_t){.modelDescription = enu}
#define FMI_ELM_TERMICON(enu) (fmi3_xml_elm_t){.termIcon = enu}

#ifdef __cplusplus
}
#endif

#endif /* FMI3_XML_XMLPARSER_SCHEME_H */
