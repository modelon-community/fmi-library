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

#ifndef FMI3_XML_XMLPARSER_CONTEXT_IMPL_H
#define FMI3_XML_XMLPARSER_CONTEXT_IMPL_H

#include <expat.h>
#include <JM/jm_vector.h>
#include <JM/jm_stack.h>

#include <FMI/fmi_xml_terminals_and_icons.h>
#include <FMI3/fmi3_xml_model_description.h>
#include "fmi3_xml_parser_scheme.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Flag for current XML file being handled. */
// TODO: Might be more suited in parser.h?
typedef enum fmi3_xml_type_t {
    fmi3_xml_type_modelDescription,
    fmi3_xml_type_terminalAndIcons
} fmi3_xml_type_t;

typedef struct fmi3_xml_modelDescription_element_handle_map_t fmi3_xml_modelDescription_element_handle_map_t;
typedef struct fmi3_xml_termIcon_element_handle_map_t fmi3_xml_termIcon_element_handle_map_t;
typedef struct fmi3_xml_element_handle_map_t fmi3_xml_element_handle_map_t;

typedef int (*fmi3_xml_modelDescription_element_handle_ft)(fmi3_xml_parser_context_t* context, const char* data);
typedef int (*fmi3_xml_termIcon_element_handle_ft)(fmi3_xml_parser_context_t* context, const char* data);
typedef int (*fmi3_xml_element_handle_ft)(fmi3_xml_parser_context_t* context, const char* data);

struct fmi3_xml_modelDescription_element_handle_map_t {
    const char* elementName;
    fmi3_xml_modelDescription_element_handle_ft elementHandle;
    fmi3_xml_modelDescription_elm_enu_t elemID;
};

struct fmi3_xml_termIcon_element_handle_map_t {
    const char* elementName;
    fmi3_xml_termIcon_element_handle_ft elementHandle;
    fmi3_xml_termIcon_elm_enu_t elemID;
};

struct fmi3_xml_element_handle_map_t {
    const char* elementName;
    fmi3_xml_element_handle_ft elementHandle;
    fmi3_xml_elm_enu_t elemID;
};


// jm_vector_declare_template(fmi3_xml_modelDescription_element_handle_map_t)
// jm_vector_declare_template(fmi3_xml_termIcon_element_handle_map_t)
jm_vector_declare_template(fmi3_xml_element_handle_map_t)

/**
 * Struct for saving and accessing data between element handlers.
 */
struct fmi3_xml_parser_context_t {

    /* Flag for XML being handled, NEEDS TO BE FIRST */
    const fmi3_xml_type_t xmlType;

    /**
     * This is where the parsed XML is saved.
     */
    fmi3_xml_model_description_t* modelDescription;

    /* Terminals and Icons XML data */
    fmi_xml_terminals_and_icons_t* termIcon;

    jm_callbacks* callbacks;

    XML_Parser parser;

    /* Temporary storage of start values for String and Binary variables. */
    jm_vector(jm_voidp) currentStartVariableValues;

    /**
     * Actual type: jm_vector of jm_vector(char).
     *
     * Purpose seems to be to reduce the cost of converting char* to jm_vector(char)
     * by using an object pool. Typically for attributes.
     *
     * This should be used to keep values between elements.
     */
    jm_vector(jm_voidp) parseBuffer;

    /**
     * Used for writing to attrMapById. Uses lookup by attribute name instead
     * of attribute ID. The .ptr field points to attrMapById[id(attr_name)].
     * Currently used ONLY for writing.
     */
    jm_vector(jm_named_ptr)* attrMapByName;

    /**
     * Vector with a slot for every attribute for every element to allow constant lookup:
     *     attrMapById[<attr_id>] = <attr_value>
     *
     * Is populated with all parsed attributes for the current element before that element
     * handler is invoked.
     *
     * Typically attributes values are cleared when they are read, such that at the end of
     * parsing an element all attributes should be cleared.
     *
     * NOTE:
     * The pointers point to expat's internal memory. It's not allowed to save references
     * to this memory between element handle calls.
     */
    jm_vector(jm_string)* attrMapById;

    /**
     * A vector that only contains mappings from element names to element
     * handlers. Remember that there can be several mappings for the same
     * element name, but with "alternative names".
     * The mapping for an XML name therefore depends on the context. For
     * example, when the ModelVariable element is found, the mapping for
     * 'Int32' must change the handler for the "alternative name":
     * 'fmi3_xml_handle_Int32Variable'.
     */
    jm_vector(fmi3_xml_element_handle_map_t)* elmMap;

    fmi3_xml_unit_t* lastBaseUnit;

    /**
     * Incremented when an invalid element(or nested elements of invalid root
     * element) is found. Decremented when invalid element end tags are parsed.
     */
    int skipElementCnt;

    /**
     * There is no guarantee that all text will be handled in one call to the
     * function implementing the XML_CharacterDataHandler, and this variable
     * saves if a warning has already been generated.
     */
    int has_produced_data_warning;

    /**
     * Used to get parent element.
     * Top of stack:
     *   XML_StartElementHandler:
     *     on enter: grandparent or empty
     *     on exit: parent or empty (push)
     *   XML_EndElementHandler:
     *     on enter: parent or empty
     *     on exit: grandparent or empty (pop)
     */
    jm_stack(int) elmStack;

    /**
     * Contains the latest element text. For an MD without tool specific
     * annotations, this will always be empty. This variable is currently
     * only used as a bool-switch though...
     * TODO: Refactor to bool
     */
    jm_vector(char) elmData;

    /**
     * Contains a Variable's start attribute. Purpose is to allow it to be
     * processed in the Variable's end-handler.
     */
    jm_vector(char) variableStartAttr;

    /**
     * Element ID of the last processed sibling, or fmi3_xml_modelDescription_elmID_none if
     * no siblings have been processed.
     */
    fmi3_xml_elm_enu_t lastSiblingElemId;

    /**
     * Used for error checking and scheme verification.
     * Values:
     *   XML_StartElementHandler:
     *     on enter: parent
     *     on exit: self
     *   XML_EndElementHandler:
     *     on enter: self
     *     on exit: parent
     */
    fmi3_xml_elm_enu_t currentElmID;

    fmi3_xml_elm_enu_t currentElemIdStartTag;

    /* Variables for handling tool-specific XML elements */
    int anyElmCount;
    int useAnyHandleFlg;
    int useAnyHandleFlgTermIcon;
    char* anyToolName;
    void* anyParent;
    fmi3_xml_callbacks_t* anyHandle;
    fmi_termicon_xml_callbacks_t* anyHandleTermIcon;

    /* Data for restoring locale after parsing */
    jm_locale_t* jm_locale;
};

#ifdef __cplusplus
}
#endif

#endif /* FMI3_XML_XMLPARSER_CONTEXT_IMPL_H */
