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
#include "fmi3_xml_model_description_scheme.h"

#define ATTR_STR(attr) #attr,
const jm_string fmi3_modelDescription_xmlAttrNames[fmi3_xml_modelDescription_attr_number] = {
    FMI3_XML_ATTRLIST_MODEL_DESCR(ATTR_STR)
};

/**
 * The expansion of below macro is also a macro. Example:
 * EXPAND_ELM_SCHEME(Float64) -> fmi3_xml_scheme_Float64 -> {fmi3_xml_elmID_SimpleType, fmi3_xml_elmID_TypeDefinitions, 0, 1}
 */
#define EXPAND_ELM_SCHEME(elm) fmi3_xml_scheme_##elm ,
/* Global array of all modelDescription scheme_info_t. Index it with fmi3_xml_elm_modelDescription_enu_t entries. */
const fmi3_xml_scheme_modelDescription_info_t fmi3_xml_scheme_modelDescription_info[fmi3_xml_elm_number] = {
    FMI3_XML_ELMLIST_MODEL_DESCR(EXPAND_ELM_SCHEME)
    {fmi3_xml_elm_actual_number, 0, 0},
    FMI3_XML_ELMLIST_ALT_MODEL_DESCR     (EXPAND_ELM_SCHEME)
    FMI3_XML_ELMLIST_ABSTRACT_MODEL_DESCR(EXPAND_ELM_SCHEME)
};

/**
 * Global array of all defined fmi3_xml_modelDescription_element_handle_map_t structs.
 * Typical use:
 *      Parse element name, from element name find ID, use ID to index this
 *      array.
 */
#define EXPAND_ELM_NAME_FMI3(elm) { #elm, fmi3_xml_handle_##elm, fmi3_xml_elmID_##elm},
const fmi3_xml_modelDescription_element_handle_map_t fmi3_modelDescription_element_handle_map[fmi3_xml_elm_number] = {
    FMI3_XML_ELMLIST_MODEL_DESCR(EXPAND_ELM_NAME_FMI3)
    { NULL, NULL, fmi3_xml_elm_actual_number},
    FMI3_XML_ELMLIST_ALT_MODEL_DESCR     (EXPAND_ELM_NAME_FMI3)
    FMI3_XML_ELMLIST_ABSTRACT_MODEL_DESCR(EXPAND_ELM_NAME_FMI3)
};

jm_string fmi3_xml_scheme_get_modelDescription_attrName(fmi3_xml_attr_modelDescription_enu_t enu) {
    return fmi3_modelDescription_xmlAttrNames[enu];
}

fmi3_xml_scheme_modelDescription_info_t fmi3_xml_scheme_get_modelDescription_info(fmi3_xml_elm_modelDescription_enu_t enu) {
    return fmi3_xml_scheme_modelDescription_info[enu];
}

fmi3_xml_modelDescription_element_handle_map_t fmi3_xml_scheme_get_modelDescription_handle(fmi3_xml_elm_modelDescription_enu_t enu) {
    return fmi3_modelDescription_element_handle_map[enu];
}
