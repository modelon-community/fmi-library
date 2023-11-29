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

#ifndef FMI3_XML_TERMINALS_AND_ICONS_PARSER_H
#define FMI3_XML_TERMINALS_AND_ICONS_PARSER_H

#include <expat.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Attributes common to modelDescription.xml & terminalsAndIcons.xml
 * are listed in FMI3_XML_ATTRLIST_COMMON in fmi3_xml_parser.h
 */
#define FMI3_XML_ATTRLIST_TERM_ICON(EXPAND_XML_ATTRNAME)  // XXX: Currently empty, but will be filled

#define FMI3_XML_ELMLIST_TERM_ICON(EXPAND_XML_ELMNAME) \
    EXPAND_XML_ELMNAME(fmiTerminalsAndIcons) \
    EXPAND_XML_ELMNAME(Terminals) \
    EXPAND_XML_ELMNAME(Terminal) \
    EXPAND_XML_ELMNAME(TerminalMemberVariable) \
    EXPAND_XML_ELMNAME(TerminalStreamMemberVariable) \
    EXPAND_XML_ELMNAME(TerminalGraphicalRepresentation)

/** \brief Element that can be placed under different parents get alternative names from the info struct */
#define FMI3_XML_ELMLIST_ALT_TERM_ICON(EXPAND_XML_ELMNAME)  // TODO: Add Terminal here?

#ifdef __cplusplus
}
#endif

#endif /* FMI3_XML_TERMINALS_AND_ICONS_PARSER_H */
