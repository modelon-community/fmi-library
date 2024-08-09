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
#include <FMI3/fmi3_xml_parser_scheme.h>
#include "fmi3_xml_parser_context_impl.h"

jm_string fmi3_xml_get_xml_attr_name(fmi3_xml_parser_context_t* context, const fmi3_xml_attr_t enu) {
    const fmi3_xml_type_t xmlType = context->xmlType;
    switch (xmlType) {
        case fmi3_xml_type_modelDescription:
            return fmi3_xml_scheme_get_modelDescription_attrName(enu.modelDescription);
        case fmi3_xml_type_terminalAndIcons:
            return fmi_xml_scheme_get_termIcon_attrName(enu.termIcon);
    }
    return "unknown";
}

fmi3_xml_scheme_info_t fmi3_xml_get_scheme_info(fmi3_xml_parser_context_t* context, fmi3_xml_elm_t enu) {
    const fmi3_xml_type_t xmlType = context->xmlType;

    fmi3_xml_scheme_modelDescription_info_t info_modelDescription;
    fmi_xml_scheme_termIcon_info_t info_termIcon;

    fmi3_xml_scheme_info_t ret; 
    switch (xmlType) {
        case fmi3_xml_type_modelDescription:
            info_modelDescription = fmi3_xml_scheme_get_modelDescription_info(enu.modelDescription);

            ret.superID = FMI3_ELM(info_modelDescription.superID);
            ret.parentID = FMI3_ELM(info_modelDescription.parentID);
            ret.siblingIndex = info_modelDescription.siblingIndex;
            ret.multipleAllowed = info_modelDescription.multipleAllowed;
            break;
        case fmi3_xml_type_terminalAndIcons:
            info_termIcon = fmi_xml_scheme_get_termIcon_info(enu.termIcon);

            ret.superID = FMI_ELM_TERMICON(info_termIcon.superID);
            ret.parentID = FMI_ELM_TERMICON(info_termIcon.parentID);
            ret.siblingIndex = info_termIcon.siblingIndex;
            ret.multipleAllowed = info_termIcon.multipleAllowed;
            break;
        default: 
            // erroneous output
            ret.superID = FMI3_ELM_ANY(FMI_XML_ELMID_NONE);
            ret.parentID = FMI3_ELM_ANY(FMI_XML_ELMID_NONE);
            ret.siblingIndex = -1;
            ret.multipleAllowed = -1;
    }
    return ret;
}

fmi3_xml_element_handle_map_t fmi3_xml_get_element_handle(fmi3_xml_parser_context_t* context, fmi3_xml_elm_t enu) {
    const fmi3_xml_type_t xmlType = context->xmlType;

    fmi3_xml_modelDescription_element_handle_map_t map_modelDescription;
    fmi_xml_termIcon_element_handle_map_t map_termIcon;

    fmi3_xml_element_handle_map_t ret;
    switch (xmlType) {
        case fmi3_xml_type_modelDescription:
            map_modelDescription = fmi3_xml_scheme_get_modelDescription_handle(enu.modelDescription);

            ret.elementName = map_modelDescription.elementName;
            ret.elementHandle = (fmi3_xml_element_handle_ft) map_modelDescription.elementHandle;
            ret.elemID = FMI3_ELM(map_modelDescription.elemID);
            break;
        case fmi3_xml_type_terminalAndIcons:
            map_termIcon = fmi_xml_scheme_get_termIcon_handle(enu.termIcon);

            ret.elementName = map_termIcon.elementName;
            ret.elementHandle = (fmi3_xml_element_handle_ft) map_termIcon.elementHandle;
            ret.elemID = FMI_ELM_TERMICON(map_termIcon.elemID);
            break;
        default:
            // erroneous output
            ret.elementName = "unknown";
            ret.elementHandle = NULL;
            ret.elemID = FMI3_ELM_ANY(FMI_XML_ELMID_NONE);
    }
    return ret;
}

/* Get size of ATTRIBUTE enum based on xmlType */
size_t fmi3_xml_get_attr_enum_size(fmi3_xml_parser_context_t* context) {
    const fmi3_xml_type_t xmlType = context->xmlType;
    switch (xmlType) {
        case fmi3_xml_type_modelDescription:
            return fmi3_xml_get_modelDescription_attr_enum_size();
        case fmi3_xml_type_terminalAndIcons:
            return fmi_xml_get_termIcon_attr_enum_size();
        default:
            // erroneous
            return 0;
    }
}

/* Get size of ELEMENT enum based on xmlType */
size_t fmi3_xml_get_elm_enum_size_actual(fmi3_xml_parser_context_t* context) {
    const fmi3_xml_type_t xmlType = context->xmlType;
    switch (xmlType) {
        case fmi3_xml_type_modelDescription:
            return fmi3_xml_get_modelDescription_elm_enum_size_actual();
        case fmi3_xml_type_terminalAndIcons:
            return fmi_xml_get_termIcon_elm_enum_size_actual();
        default:
            // erroneous
            return 0;
    }
}

/**
 * Returns true if parent element's type or super type (recursively) matches
 * the expected type.
 */
int fmi3_xml_is_valid_parent(fmi3_xml_parser_context_t* context, fmi3_xml_elm_t child_id, fmi3_xml_elm_t parent_id) {
    fmi3_xml_elm_t p_id_expected = fmi3_xml_get_scheme_info(context, child_id).parentID;

    while (parent_id.any != p_id_expected.any && parent_id.any != FMI_XML_ELMID_NONE) {
        parent_id = fmi3_xml_get_scheme_info(context, parent_id).superID;
    }
    return parent_id.any == p_id_expected.any;
}

/**
 * Returns top level super type of the element.
 */
int fmi3_xml_get_super_type_rec(fmi3_xml_parser_context_t* context, fmi3_xml_elm_t id) {
    fmi3_xml_elm_t id_top = id;
    while ((fmi3_xml_get_scheme_info(context, id_top).superID).any != FMI_XML_ELMID_NONE) {
        id_top = fmi3_xml_get_scheme_info(context, id_top).superID;
    }
    return id_top.any;
}

/**
 * Returns true if the top-level super types are the same.
 */
int fmi3_xml_are_same_type(fmi3_xml_parser_context_t* context, fmi3_xml_elm_t id1, fmi3_xml_elm_t id2) {
    return fmi3_xml_get_super_type_rec(context, id1) == fmi3_xml_get_super_type_rec(context, id2);
}
