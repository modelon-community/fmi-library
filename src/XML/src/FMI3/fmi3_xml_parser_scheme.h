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
This file contains all parsing related lists and structs
related to parsing XML elements and their attributes
*/

#ifndef FMI3_XML_XMLPARSER_SCHEME_H
#define FMI3_XML_XMLPARSER_SCHEME_H

#include "../FMI/fmi_xml_terminals_and_icons_scheme.h"
#include "fmi3_xml_model_description_scheme.h"

#ifdef __cplusplus
extern "C" {
#endif

// TODO: Duplicate the common ones, for simplicity
/** \brief Attributes names common to modelDescription.xml & terminalsAndIcons.xml */
#define FMI3_XML_ATTRLIST_COMMON(EXPAND_XML_ATTRNAME) \
    EXPAND_XML_ATTRNAME(fmiVersion) \
    EXPAND_XML_ATTRNAME(name) \
    EXPAND_XML_ATTRNAME(description)

// TODO: Eliminate this list, once terminalsAndIcons has been properly separated
/** \brief Attribute names used in XML */
#define FMI3_XML_ATTRLIST(EXPAND_XML_ATTRNAME) \
    FMI3_XML_ATTRLIST_COMMON     (EXPAND_XML_ATTRNAME) \
    FMI3_XML_ATTRLIST_MODEL_DESCR(EXPAND_XML_ATTRNAME) \
    FMI_XML_ATTRLIST_TERM_ICON   (EXPAND_XML_ATTRNAME)

#define FMI3_XML_ATTR_ID(attr) fmi_attr_id_##attr,
typedef enum fmi3_xml_modelDescription_attr_enu_t {
    FMI3_XML_ATTRLIST(FMI3_XML_ATTR_ID)
    fmi3_modelDescription_xml_attr_number
} fmi3_xml_modelDescription_attr_enu_t;

#define FMI_TERMICON_XML_ATTR_ID(attr) fmi_termIcon_attr_id_##attr,
typedef enum fmi3_xml_termIcon_attr_enu_t {
    FMI3_XML_ATTRLIST_COMMON(FMI_TERMICON_XML_ATTR_ID) \
    FMI_XML_ATTRLIST_TERM_ICON(FMI_TERMICON_XML_ATTR_ID)
    fmi3_xml_termIcon_attr_number
} fmi3_xml_termIcon_attr_enu_t;

typedef int fmi3_xml_attr_enu_t;
// typedef union fmi3_xml_attr_enu_t {
//     fmi3_xml_modelDescription_attr_enu_t modelDescription;
//     fmi3_xml_termIcon_attr_enu_t termIcon;
// } fmi3_xml_attr_enu_t;

// #define FMI3_MODELDESCRIPTION_ATTR(attr) (const fmi3_xml_attr_enu_t){.modelDescription = attr}
// #define FMI_TERMICON_ATTR(attr) (const fmi3_xml_attr_enu_t){.termIcon = attr}

/* Build prototypes for all elm_handle_* functions */
typedef struct fmi3_xml_parser_context_t fmi3_xml_parser_context_t;
#define EXPAND_ELM_HANDLE_FMI3(elm) extern int fmi3_xml_handle_##elm(fmi3_xml_parser_context_t* context, const char* data);
FMI3_XML_ELMLIST_MODEL_DESCR         (EXPAND_ELM_HANDLE_FMI3)
FMI3_XML_ELMLIST_ALT_MODEL_DESCR     (EXPAND_ELM_HANDLE_FMI3)
FMI3_XML_ELMLIST_ABSTRACT_MODEL_DESCR(EXPAND_ELM_HANDLE_FMI3)

#define EXPAND_ELM_HANDLE_FMI_TERM_ICON(elm) extern int fmi_xml_handle_##elm(fmi3_xml_parser_context_t* context, const char* data);
FMI_XML_ELMLIST_TERM_ICON    (EXPAND_ELM_HANDLE_FMI_TERM_ICON)
FMI_XML_ELMLIST_ALT_TERM_ICON(EXPAND_ELM_HANDLE_FMI_TERM_ICON)

/**
 * Create an enum over all XML elements. This enum can be used to index
 * the following arrays:
 *      - fmi3_xml_modelDescription_scheme_info
 *      - fmi3_modelDescription_element_handle_map
 */
#define FMI3_XML_ELM_ID(elm) fmi3_xml_elmID_##elm
#define FMI3_XML_LIST_ELM_ID(elm) ,FMI3_XML_ELM_ID(elm)
// TODO: The none value is used in a couple of places for iteration purposes
// might make sense to use the same for both modelDescription & terminalAndIcons
typedef enum fmi3_xml_modelDescription_elm_enu_t {
    fmi3_xml_modelDescription_elmID_none = -1
    FMI3_XML_ELMLIST_MODEL_DESCR(FMI3_XML_LIST_ELM_ID)
    FMI_XML_ELMLIST_TERM_ICON   (FMI3_XML_LIST_ELM_ID)
    ,fmi3_xml_modelDescription_elm_actual_number
    FMI3_XML_ELMLIST_ALT_MODEL_DESCR     (FMI3_XML_LIST_ELM_ID)
    FMI_XML_ELMLIST_ALT_TERM_ICON        (FMI3_XML_LIST_ELM_ID)
    FMI3_XML_ELMLIST_ABSTRACT_MODEL_DESCR(FMI3_XML_LIST_ELM_ID)
    ,fmi3_xml_modelDescription_elm_number
} fmi3_xml_modelDescription_elm_enu_t;

#define FMI_TERMICON_XML_ELM_ID(elm) fmi_termIcon_xml_elmID_##elm
#define FMI_TERMICON_XML_LIST_ELM_ID(elm) ,FMI_TERMICON_XML_ELM_ID(elm)
typedef enum fmi3_xml_termIcon_elm_enu_t {
    fmi3_xml_termIcon_elmID_none = -1
    FMI_XML_ELMLIST_TERM_ICON(FMI_TERMICON_XML_LIST_ELM_ID)
    ,fmi3_xml_termIcon_elm_actual_number
    FMI_XML_ELMLIST_ALT_TERM_ICON(FMI_TERMICON_XML_LIST_ELM_ID)
    ,fmi3_xml_termIcon_elm_number
} fmi3_xml_termIcon_elm_enu_t;

typedef union fmi3_xml_elm_enu_t {
    fmi3_xml_modelDescription_elm_enu_t modelDescription;
    fmi3_xml_termIcon_elm_enu_t termIcon;
} fmi3_xml_elm_enu_t;

#ifdef __cplusplus
}
#endif

#endif /* FMI3_XML_XMLPARSER_SCHEME_H */
