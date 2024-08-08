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

#include "JM/jm_portability.h"
#include "fmi_xml_terminals_and_icons_scheme.h"

#define ATTR_STR(attr) #attr,
const jm_string fmi_termIcon_xmlAttrNames[fmi_xml_termIcon_attr_number] = {
    FMI_XML_ATTRLIST_TERM_ICON(ATTR_STR)
};

/**
 * The expansion of below macro is also a macro. Example:
 * EXPAND_ELM_SCHEME_TERMICON(Terminals) -> fmi_xml_scheme_termIcon_Terminals -> {fmi_xml_elmID_termIcon_none, fmi_xml_elmID_termIcon_fmiTerminalsAndIcons, 1, 0}
 */
#define EXPAND_ELM_SCHEME_TERMICON(elm) fmi_xml_scheme_termIcon_##elm ,
/* Global array of all terminalsAndIcons scheme_info_t. Index it with fmi_xml_elm_termIcon_enu_t entries. */
fmi_xml_scheme_termIcon_info_t fmi_xml_scheme_termIcon_info[fmi_xml_elm_termIcon_number] = {
    FMI_XML_ELMLIST_TERM_ICON(EXPAND_ELM_SCHEME_TERMICON)
    {fmi_xml_elm_termIcon_actual_number, 0, 0},
    FMI_XML_ELMLIST_ALT_TERM_ICON(EXPAND_ELM_SCHEME_TERMICON)
};

/**
 * Global array of all defined fmi_xml_termIcon_element_handle_map_t structs.
 * Typical use:
 *      Parse element name, from element name find ID, use ID to index this
 *      array.
 */
#define EXPAND_ELM_NAME_FMI_TERM_ICON(elm) { #elm, fmi_xml_handle_##elm, fmi_xml_elmID_termIcon_##elm},
const fmi_xml_termIcon_element_handle_map_t fmi_termIcon_element_handle_map[fmi_xml_elm_termIcon_number] = {
    FMI_XML_ELMLIST_TERM_ICON(EXPAND_ELM_NAME_FMI_TERM_ICON)
    { NULL, NULL, fmi_xml_elm_termIcon_actual_number},
    FMI_XML_ELMLIST_ALT_TERM_ICON(EXPAND_ELM_NAME_FMI_TERM_ICON)
};

jm_string fmi_xml_scheme_get_termIcon_attrName(fmi_xml_attr_termIcon_enu_t enu) {
    return fmi_termIcon_xmlAttrNames[enu];
}

fmi_xml_scheme_termIcon_info_t fmi_xml_scheme_get_termIcon_info(fmi_xml_elm_termIcon_enu_t enu) {
    return fmi_xml_scheme_termIcon_info[enu];
}

fmi_xml_termIcon_element_handle_map_t fmi_xml_scheme_get_termIcon_handle(fmi_xml_elm_termIcon_enu_t enu) {
    return fmi_termIcon_element_handle_map[enu];
}

size_t fmi_xml_get_termIcon_attr_enum_size() {
    return fmi_xml_termIcon_attr_number;
}

size_t fmi_xml_get_termIcon_elm_enum_size_actual() {
    return fmi_xml_elm_termIcon_actual_number;
}
