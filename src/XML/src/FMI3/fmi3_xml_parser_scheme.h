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

#define FMI3_XML_ATTR_ID(attr) fmi_attr_id_##attr,
typedef enum fmi3_xml_attr_modelDescription_enu_t {
    FMI3_XML_ATTRLIST_MODEL_DESCR(FMI3_XML_ATTR_ID)
    fmi3_xml_modelDescription_attr_number
} fmi3_xml_attr_modelDescription_enu_t;

#define FMI_TERMICON_XML_ATTR_ID(attr) fmi_termIcon_attr_id_##attr,
typedef enum fmi_xml_attr_termIcon_enu_t {
    FMI_XML_ATTRLIST_TERM_ICON(FMI_TERMICON_XML_ATTR_ID)
    fmi_xml_termIcon_attr_number
} fmi_xml_attr_termIcon_enu_t;

typedef union fmi3_xml_attr_t {
    int any; // when used in generic parser/handling context
    fmi3_xml_attr_modelDescription_enu_t modelDescription;
    fmi_xml_attr_termIcon_enu_t termIcon;
} fmi3_xml_attr_t;

// Shortcuts for defining fmi3_xml_attr_t for a given enum value
#define FMI3_ATTR_ANY(enu)     (fmi3_xml_attr_t){.any = enu}
#define FMI3_ATTR(enu)         (fmi3_xml_attr_t){.modelDescription = enu}
#define FMI_ATTR_TERMICON(enu) (fmi3_xml_attr_t){.termIcon = enu}

/* Build prototypes for all elm_handle_* functions */
typedef struct fmi3_xml_parser_context_t fmi3_xml_parser_context_t;
#define EXPAND_ELM_HANDLE_FMI3(elm) extern int fmi3_xml_handle_##elm(fmi3_xml_parser_context_t* context, const char* data);
FMI3_XML_ELMLIST_MODEL_DESCR         (EXPAND_ELM_HANDLE_FMI3)
FMI3_XML_ELMLIST_ALT_MODEL_DESCR     (EXPAND_ELM_HANDLE_FMI3)
FMI3_XML_ELMLIST_ABSTRACT_MODEL_DESCR(EXPAND_ELM_HANDLE_FMI3)

#define EXPAND_ELM_HANDLE_FMI_TERMICON(elm) extern int fmi_xml_handle_##elm(fmi3_xml_parser_context_t* context, const char* data);
FMI_XML_ELMLIST_TERM_ICON    (EXPAND_ELM_HANDLE_FMI_TERMICON)
FMI_XML_ELMLIST_ALT_TERM_ICON(EXPAND_ELM_HANDLE_FMI_TERMICON)

/**
 * Create an enum over all modelDescription XML elements. This enum can be used to index
 * the following arrays:
 *      - fmi3_xml_scheme_modelDescription_info
 *      - fmi3_modelDescription_element_handle_map
 */
#define FMI3_XML_ELM_ID(elm) fmi3_xml_elmID_##elm
#define FMI3_XML_LIST_ELM_ID(elm) ,FMI3_XML_ELM_ID(elm)
#define FMI_XML_ELMID_NONE (-1)
typedef enum fmi3_xml_elm_modelDescription_enu_t {
    fmi3_xml_elmID_none = FMI_XML_ELMID_NONE
    FMI3_XML_ELMLIST_MODEL_DESCR(FMI3_XML_LIST_ELM_ID)
    ,fmi3_xml_elm_actual_number
    FMI3_XML_ELMLIST_ALT_MODEL_DESCR     (FMI3_XML_LIST_ELM_ID)
    FMI3_XML_ELMLIST_ABSTRACT_MODEL_DESCR(FMI3_XML_LIST_ELM_ID)
    ,fmi3_xml_elm_number
} fmi3_xml_elm_modelDescription_enu_t;

/**
 * Create an enum over all terminalsAndIcons XML elements. This enum can be used to index
 * the following arrays:
 *      - fmi_xml_scheme_termIcon_info
 *      - fmi_termIcon_element_handle_map
 */
#define FMI_TERMICON_XML_ELM_ID(elm) fmi_xml_elmID_termIcon_##elm
#define FMI_TERMICON_XML_LIST_ELM_ID(elm) ,FMI_TERMICON_XML_ELM_ID(elm)
typedef enum fmi_xml_elm_termIcon_enu_t {
    fmi_xml_elmID_termIcon_none = FMI_XML_ELMID_NONE
    FMI_XML_ELMLIST_TERM_ICON(FMI_TERMICON_XML_LIST_ELM_ID)
    ,fmi_xml_elm_termIcon_actual_number
    FMI_XML_ELMLIST_ALT_TERM_ICON(FMI_TERMICON_XML_LIST_ELM_ID)
    ,fmi_xml_elm_termIcon_number
} fmi_xml_elm_termIcon_enu_t;

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
