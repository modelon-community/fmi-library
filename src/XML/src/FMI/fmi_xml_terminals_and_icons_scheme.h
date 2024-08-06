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
    Define XML schema structure. Used to build the 'fmi3_xml_termIcon_scheme_info_t' type (in fmi3_xml_parser.c).

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

#ifdef __cplusplus
}
#endif

#endif /* FMI_XML_TERMINALS_AND_ICONS_SCEHEME_H */
