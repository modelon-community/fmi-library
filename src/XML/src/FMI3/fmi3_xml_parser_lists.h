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

/*
This file contains all parsing related lists and structs
related to parsing XML elements and their attributes
*/

// TODO: Rename to something like *_scheme.h?

#ifndef FMI3_XML_XMLPARSER_LISTS_H
#define FMI3_XML_XMLPARSER_LISTS_H

#include <expat.h>

#include "fmi3_xml_terminals_and_icons_parser.h"
#include "fmi3_xml_model_description_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Attributes names common to modelDescription.xml & terminalsAndIcons.xml */
#define FMI3_XML_ATTRLIST_COMMON(EXPAND_XML_ATTRNAME) \
    EXPAND_XML_ATTRNAME(fmiVersion) \
    EXPAND_XML_ATTRNAME(name) \
    EXPAND_XML_ATTRNAME(description)

/** \brief Attribute names used in XML */
#define FMI3_XML_ATTRLIST(EXPAND_XML_ATTRNAME) \
    FMI3_XML_ATTRLIST_COMMON     (EXPAND_XML_ATTRNAME) \
    FMI3_XML_ATTRLIST_MODEL_DESCR(EXPAND_XML_ATTRNAME) \
    FMI3_XML_ATTRLIST_TERM_ICON  (EXPAND_XML_ATTRNAME)

#define FMI3_XML_ATTR_ID(attr) fmi_attr_id_##attr,
typedef enum fmi3_xml_attr_enu_t {
    FMI3_XML_ATTRLIST(FMI3_XML_ATTR_ID)
    fmi3_xml_attr_number
} fmi3_xml_attr_enu_t;

/** \brief Element names used in XML */
#define FMI3_XML_ELMLIST(EXPAND_XML_ELMNAME) \
    FMI3_XML_ELMLIST_MODEL_DESCR(EXPAND_XML_ELMNAME) \
    FMI3_XML_ELMLIST_TERM_ICON  (EXPAND_XML_ELMNAME)

/** \brief Element that can be placed under different parents get alternative names from the info struct */
#define FMI3_XML_ELMLIST_ALT(EXPAND_XML_ELMNAME) \
    FMI3_XML_ELMLIST_ALT_MODEL_DESCR(EXPAND_XML_ELMNAME) \
    FMI3_XML_ELMLIST_ALT_TERM_ICON  (EXPAND_XML_ELMNAME)

/** \brief Abstract elements that are only used in the scheme verification */
#define FMI3_XML_ELMLIST_ABSTRACT(EXPAND_XML_ELMNAME) \
    FMI3_XML_ELMLIST_ABSTRACT_MODEL_DESCR(EXPAND_XML_ELMNAME)

#ifdef __cplusplus
}
#endif

#endif /* FMI3_XML_XMLPARSER_LISTS_H */
