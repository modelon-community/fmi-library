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

#ifdef __cplusplus
extern "C" {
#endif

/* Attributes common to modelDescription.xml & terminalsAndIcons.xml
 * are listed in FMI3_XML_ATTRLIST_COMMON in fmi3_xml_parser.h
 */
#define FMI_XML_ATTRLIST_TERM_ICON(EXPAND_XML_ATTRNAME) \
    EXPAND_XML_ATTRNAME(fmiVersion) \
    EXPAND_XML_ATTRNAME(name) \
    EXPAND_XML_ATTRNAME(description)

#define FMI_XML_ELMLIST_TERM_ICON(EXPAND_XML_ELMNAME) \
    EXPAND_XML_ELMNAME(fmiTerminalsAndIcons) \
    EXPAND_XML_ELMNAME(Terminals) \
    EXPAND_XML_ELMNAME(Terminal) \
    EXPAND_XML_ELMNAME(TerminalMemberVariable) \
    EXPAND_XML_ELMNAME(TerminalStreamMemberVariable) \
    EXPAND_XML_ELMNAME(TerminalGraphicalRepresentation)

/** \brief Element that can be placed under different parents get alternative names from the info struct */
#define FMI_XML_ELMLIST_ALT_TERM_ICON(EXPAND_XML_ELMNAME)  // TODO: Terminal should go here?

// XXX: fmi3_xml_modelDescription_elmID_none is defined in fmi3_xml_parser.h, not good style
/*
    Define XML schema structure. Used to build the 'fmi3_xml_termIcon_scheme_info_t' type (in fmi3_xml_parser.c).

    @sib_idx:
        the index in a sequence among siblings
    @multi_elem:
        if the parent can have multiple elements of this type
*/
/*      scheme_ID,                                      super_type,                 parent_ID,                          sib_idx, multi_elem */
// #define fmi3_xml_scheme_fmiTerminalsAndIcons            {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_modelDescription_elmID_none,                 0,       0}

// #define fmi3_xml_scheme_Terminals                       {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_fmiTerminalsAndIcons, 1,       0}
// #define fmi3_xml_scheme_Terminal                        {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_Terminals,            0,       1}
// #define fmi3_xml_scheme_TerminalMemberVariable          {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_Terminal,             0,       1}
// #define fmi3_xml_scheme_TerminalStreamMemberVariable    {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_Terminal,             1,       1}
// TODO: How to handle nested Terminals?
// #define fmi3_xml_scheme_TerminalGraphicalRepresentation {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_Terminal,             3,       0}
// TODO: List formatting + use terminals and Icons type
// TODO: rename to include termIcon in name?

// TODO: Remove duplication
// TOOD: Better NONE value
// TODO: Right hand side: fmi_termIcon rename
#define fmi_termIcon_xml_scheme_fmiTerminalsAndIcons            {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_modelDescription_elmID_none,                 0,       0}

#define fmi_termIcon_xml_scheme_Terminals                       {fmi3_xml_modelDescription_elmID_none,       fmi_termIcon_xml_elmID_fmiTerminalsAndIcons, 1,       0}
#define fmi_termIcon_xml_scheme_Terminal                        {fmi3_xml_modelDescription_elmID_none,       fmi_termIcon_xml_elmID_Terminals,            0,       1}
#define fmi_termIcon_xml_scheme_TerminalMemberVariable          {fmi3_xml_modelDescription_elmID_none,       fmi_termIcon_xml_elmID_Terminal,             0,       1}
#define fmi_termIcon_xml_scheme_TerminalStreamMemberVariable    {fmi3_xml_modelDescription_elmID_none,       fmi_termIcon_xml_elmID_Terminal,             1,       1}
// TODO: How to handle nested Terminals?
#define fmi_termIcon_xml_scheme_TerminalGraphicalRepresentation {fmi3_xml_modelDescription_elmID_none,       fmi_termIcon_xml_elmID_Terminal,             3,       0}

#ifdef __cplusplus
}
#endif

#endif /* FMI_XML_TERMINALS_AND_ICONS_SCEHEME_H */
