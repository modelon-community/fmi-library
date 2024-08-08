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

#ifndef FMI_XML_TERMINALS_AND_ICONS_SCEHEME_H
#define FMI_XML_TERMINALS_AND_ICONS_SCEHEME_H

#include "../FMI3/fmi3_xml_parser_scheme_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Attribute names found in terminalsAndIcons.xml */
#define FMI_XML_ATTRLIST_TERM_ICON(EXPAND_XML_ATTRNAME) \
    EXPAND_XML_ATTRNAME(fmiVersion) \
    EXPAND_XML_ATTRNAME(name) \
    EXPAND_XML_ATTRNAME(description)

/* Element names found in terminalsAndIcons.xml */
#define FMI_XML_ELMLIST_TERM_ICON(EXPAND_XML_ELMNAME) \
    EXPAND_XML_ELMNAME(fmiTerminalsAndIcons) \
    EXPAND_XML_ELMNAME(Terminals) \
    EXPAND_XML_ELMNAME(Terminal) \
    EXPAND_XML_ELMNAME(TerminalMemberVariable) \
    EXPAND_XML_ELMNAME(TerminalStreamMemberVariable) \
    EXPAND_XML_ELMNAME(TerminalGraphicalRepresentation)

/** \brief Element that can be placed under different parents get alternative names from the info struct */
#define FMI_XML_ELMLIST_ALT_TERM_ICON(EXPAND_XML_ELMNAME)  // TODO: Terminal should go here?

/*
    Define XML schema structure. Used to build the 'fmi_xml_scheme_termIcon_info_t' type (in fmi3_xml_parser.c).

    @sib_idx:
        the index in a sequence among siblings
    @multi_elem:
        if the parent can have multiple elements of this type
*/
/*      scheme_ID,                                              super_type,                   parent_ID,                                   sib_idx, multi_elem */
#define fmi_xml_scheme_termIcon_fmiTerminalsAndIcons            {fmi_xml_elmID_termIcon_none, fmi_xml_elmID_termIcon_none,                 0,       0}

#define fmi_xml_scheme_termIcon_Terminals                       {fmi_xml_elmID_termIcon_none, fmi_xml_elmID_termIcon_fmiTerminalsAndIcons, 1,       0}
#define fmi_xml_scheme_termIcon_Terminal                        {fmi_xml_elmID_termIcon_none, fmi_xml_elmID_termIcon_Terminals,            0,       1}
#define fmi_xml_scheme_termIcon_TerminalMemberVariable          {fmi_xml_elmID_termIcon_none, fmi_xml_elmID_termIcon_Terminal,             0,       1}
#define fmi_xml_scheme_termIcon_TerminalStreamMemberVariable    {fmi_xml_elmID_termIcon_none, fmi_xml_elmID_termIcon_Terminal,             1,       1}
// TODO: How to handle nested Terminals?
#define fmi_xml_scheme_termIcon_TerminalGraphicalRepresentation {fmi_xml_elmID_termIcon_none, fmi_xml_elmID_termIcon_Terminal,             3,       0}

// Attribute enum
#define FMI_TERMICON_XML_ATTR_ID(attr) fmi_termIcon_attr_id_##attr,
typedef enum fmi_xml_attr_termIcon_enu_t {
    FMI_XML_ATTRLIST_TERM_ICON(FMI_TERMICON_XML_ATTR_ID)
    fmi_xml_termIcon_attr_number
} fmi_xml_attr_termIcon_enu_t;


/* Build prototypes for all elm_handle_* functions */
#define EXPAND_ELM_HANDLE_FMI_TERMICON(elm) extern int fmi_xml_handle_##elm(fmi3_xml_parser_context_t* context, const char* data);
FMI_XML_ELMLIST_TERM_ICON    (EXPAND_ELM_HANDLE_FMI_TERMICON)
FMI_XML_ELMLIST_ALT_TERM_ICON(EXPAND_ELM_HANDLE_FMI_TERMICON)


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

typedef struct {
    fmi_xml_elm_termIcon_enu_t superID;  /* ID of super type or NULL if none */
    fmi_xml_elm_termIcon_enu_t parentID; /* expected parent ID for an element */
    int siblingIndex;       /* index among siblings */
    int multipleAllowed;    /* multiple elements of this kind kan come in a sequence as siblings*/
} fmi_xml_scheme_termIcon_info_t;

typedef struct fmi_xml_termIcon_element_handle_map_t fmi_xml_termIcon_element_handle_map_t;
typedef int (*fmi3_xml_termIcon_element_handle_ft)(fmi3_xml_parser_context_t* context, const char* data);
struct fmi_xml_termIcon_element_handle_map_t {
    const char* elementName;
    fmi3_xml_termIcon_element_handle_ft elementHandle;
    fmi_xml_elm_termIcon_enu_t elemID;
};

jm_string fmi_xml_scheme_get_termIcon_attrName(fmi_xml_attr_termIcon_enu_t enu);
fmi_xml_scheme_termIcon_info_t fmi_xml_scheme_get_termIcon_info(fmi_xml_elm_termIcon_enu_t enu);
fmi_xml_termIcon_element_handle_map_t fmi_xml_scheme_get_termIcon_handle(fmi_xml_elm_termIcon_enu_t enu);

#ifdef __cplusplus
}
#endif

#endif /* FMI_XML_TERMINALS_AND_ICONS_SCEHEME_H */
