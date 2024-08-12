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
XML scheme interaction via unified types and APIs.
*/

#ifndef FMI3_XML_XMLPARSER_SCHEME_H
#define FMI3_XML_XMLPARSER_SCHEME_H

#include <FMI/fmi_xml_terminals_and_icons_scheme.h>
#include <FMI3/fmi3_xml_model_description_scheme.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Flag for current XML file being handled. */
typedef enum fmi3_xml_type_t {
    fmi3_xml_type_modelDescription,
    fmi3_xml_type_terminalAndIcons
} fmi3_xml_type_t;

typedef union fmi3_xml_attr_t {
    int any; // when used in generic parser/handling context
    fmi3_xml_attr_modelDescription_enu_t modelDescription;
    fmi_xml_attr_termIcon_enu_t termIcon;
} fmi3_xml_attr_t;

// Shortcuts for defining fmi3_xml_attr_t for a given enum value
#define FMI3_ATTR_ANY(enu)     (fmi3_xml_attr_t){.any = enu}
#define FMI3_ATTR(enu)         (fmi3_xml_attr_t){.modelDescription = enu}
#define FMI_ATTR_TERMICON(enu) (fmi3_xml_attr_t){.termIcon = enu}

typedef union fmi3_xml_elm_t {
    int any; // when used in generic parser/handling context
    fmi3_xml_elm_modelDescription_enu_t modelDescription;
    fmi_xml_elm_termIcon_enu_t termIcon;
} fmi3_xml_elm_t;

// Shortcuts for defining fmi3_xml_elm_t for a given enum value
#define FMI3_ELM_ANY(enu)     (fmi3_xml_elm_t){.any = enu}
#define FMI3_ELM(enu)         (fmi3_xml_elm_t){.modelDescription = enu}
#define FMI_ELM_TERMICON(enu) (fmi3_xml_elm_t){.termIcon = enu}

/** Keeps information about the allowed parent element ID, index among siblings in a sequence and if
    multiple elements of this type are allowed in a sequence.
*/
typedef struct {
    fmi3_xml_elm_t superID;  /* ID of super type or NULL if none */
    fmi3_xml_elm_t parentID; /* expected parent ID for an element */
    int siblingIndex;       /* index among siblings */
    int multipleAllowed;    /* multiple elements of this kind kan come in a sequence as siblings*/
} fmi3_xml_scheme_info_t;

typedef struct fmi3_xml_element_handle_map_t fmi3_xml_element_handle_map_t;
typedef int (*fmi3_xml_element_handle_ft)(fmi3_xml_parser_context_t* context, const char* data);
struct fmi3_xml_element_handle_map_t {
    const char* elementName;
    fmi3_xml_element_handle_ft elementHandle;
    fmi3_xml_elm_t elemID;
};

jm_string fmi3_xml_get_xml_attr_name(fmi3_xml_parser_context_t* context, const fmi3_xml_attr_t enu);
fmi3_xml_scheme_info_t fmi3_xml_get_scheme_info(fmi3_xml_parser_context_t* context, fmi3_xml_elm_t enu);
fmi3_xml_element_handle_map_t fmi3_xml_get_element_handle(fmi3_xml_parser_context_t* context, fmi3_xml_elm_t enu);

size_t fmi3_xml_get_attr_enum_size(fmi3_xml_parser_context_t* context);
size_t fmi3_xml_get_elm_enum_size_actual(fmi3_xml_parser_context_t* context);

int fmi3_xml_is_valid_parent(fmi3_xml_parser_context_t* context, fmi3_xml_elm_t child_id, fmi3_xml_elm_t parent_id);
int fmi3_xml_get_super_type_rec(fmi3_xml_parser_context_t* context, fmi3_xml_elm_t id);
int fmi3_xml_are_same_type(fmi3_xml_parser_context_t* context, fmi3_xml_elm_t id1, fmi3_xml_elm_t id2);

#ifdef __cplusplus
}
#endif

#endif /* FMI3_XML_XMLPARSER_SCHEME_H */
