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

#ifndef FMI3_XML_XMLPARSER_H
#define FMI3_XML_XMLPARSER_H

#include <expat.h>

#include <JM/jm_vector.h>
#include <JM/jm_stack.h>
#include <JM/jm_named_ptr.h>
#include <JM/jm_portability.h>

#include <FMI3/fmi3_enums.h>
#include "fmi3_xml_parser_context_impl.h"
#include "fmi3_xml_parser_util.h"

#ifdef __cplusplus
extern "C" {
#endif

jm_vector_declare_template(fmi3_value_reference_t)

#define fmi3_xml_diff_elmName(a, b) strcmp(a.elementName,b.elementName)

jm_define_comp_f(fmi3_xml_compare_elmName, fmi3_xml_element_handle_map_t, fmi3_xml_diff_elmName)

#define XML_BLOCK_SIZE 16000

/* Meta data about primitive types */
/* NOTE: elemID is should not be included because it's different during parsing of TypeDef / Variable */
typedef struct fmi3_xml_primitive_type_t {
    const char*             name;               /* human-readable name */
    size_t                  size;               /* size in bytes */
    fmi3_bitness_enu_t      bitness;            /* type's bitness */
    int                     isSigned;           /* if the type is signed */
    fmi3_base_type_enu_t    baseType;           /* enum value for primitive type (for a typedef object) */
} fmi3_xml_primitive_type_t;

typedef struct fmi3_xml_primitive_types_t {
    fmi3_xml_primitive_type_t float64;
    fmi3_xml_primitive_type_t float32;
    fmi3_xml_primitive_type_t int64;
    fmi3_xml_primitive_type_t int32;
    fmi3_xml_primitive_type_t int16;
    fmi3_xml_primitive_type_t int8;
    fmi3_xml_primitive_type_t uint64;
    fmi3_xml_primitive_type_t uint32;
    fmi3_xml_primitive_type_t uint16;
    fmi3_xml_primitive_type_t uint8;
    fmi3_xml_primitive_type_t boolean;
    fmi3_xml_primitive_type_t enumeration;
} fmi3_xml_primitive_types_t;

extern const fmi3_xml_primitive_types_t PRIMITIVE_TYPES;

jm_vector(char) * fmi3_xml_reserve_parse_buffer(fmi3_xml_parser_context_t* context, size_t index, size_t size);
int fmi3_xml_alloc_parse_buffer(fmi3_xml_parser_context_t* context, size_t items);
void fmi3_xml_free_variable_start_values(fmi3_xml_parser_context_t* context);
void fmi3_xml_free_parse_buffer(fmi3_xml_parser_context_t* context);

/**
 * Raises generic parse error for given attribute.
 */
void fmi3_xml_parse_attr_error(fmi3_xml_parser_context_t* context, fmi3_xml_elm_t elmID, const fmi3_xml_attr_t attrID, const char* attrStr);

int fmi3_xml_parse_attr_as_string (fmi3_xml_parser_context_t* context, fmi3_xml_elm_t elmID, const fmi3_xml_attr_t attrID, int required, jm_vector(char)* field);
int fmi3_xml_parse_attr_as_enum   (fmi3_xml_parser_context_t* context, fmi3_xml_elm_t elmID, const fmi3_xml_attr_t attrID, int required, unsigned int*    field, unsigned int   defaultVal, jm_name_ID_map_t* nameMap);
int fmi3_xml_parse_attr_as_bool   (fmi3_xml_parser_context_t* context, fmi3_xml_elm_t elmID, const fmi3_xml_attr_t attrID, int required, bool*            field, bool           defaultVal);
int fmi3_xml_parse_attr_as_boolean(fmi3_xml_parser_context_t* context, fmi3_xml_elm_t elmID, const fmi3_xml_attr_t attrID, int required, unsigned int*    field, unsigned int   defaultVal);
int fmi3_xml_parse_attr_as_intXX  (fmi3_xml_parser_context_t* context, fmi3_xml_elm_t elmID, const fmi3_xml_attr_t attrID, int required, void*            field, void*          defaultVal, const fmi3_xml_primitive_type_t* primType);
int fmi3_xml_parse_attr_as_int32  (fmi3_xml_parser_context_t* context, fmi3_xml_elm_t elmID, const fmi3_xml_attr_t attrID, int required, fmi3_int32_t*    field, fmi3_int32_t   defaultVal);
int fmi3_xml_parse_attr_as_uint64 (fmi3_xml_parser_context_t* context, fmi3_xml_elm_t elmID, const fmi3_xml_attr_t attrID, int required, fmi3_uint64_t*   field, fmi3_uint64_t  defaultVal);
int fmi3_xml_parse_attr_as_uint32 (fmi3_xml_parser_context_t* context, fmi3_xml_elm_t elmID, const fmi3_xml_attr_t attrID, int required, fmi3_uint32_t*   field, fmi3_uint32_t  defaultVal);
int fmi3_xml_parse_attr_as_floatXX(fmi3_xml_parser_context_t* context, fmi3_xml_elm_t elmID, const fmi3_xml_attr_t attrID, int required, void*            field, void*          defaultVal, const fmi3_xml_primitive_type_t* primType);
int fmi3_xml_parse_attr_as_float64(fmi3_xml_parser_context_t* context, fmi3_xml_elm_t elmID, const fmi3_xml_attr_t attrID, int required, fmi3_float64_t*  field, fmi3_float64_t defaultVal);
int fmi3_xml_parse_attr_as_float32(fmi3_xml_parser_context_t* context, fmi3_xml_elm_t elmID, const fmi3_xml_attr_t attrID, int required, fmi3_float32_t*  field, fmi3_float32_t defaultVal);
int fmi3_xml_parse_attr_as_sizet  (fmi3_xml_parser_context_t* context, fmi3_xml_elm_t elmID, const fmi3_xml_attr_t attrID, int required, size_t*          field, size_t*        defaultVal);
int fmi3_xml_parse_attr_as_array  (fmi3_xml_parser_context_t* context, fmi3_xml_elm_t elmID, const fmi3_xml_attr_t attrID, int required, void** arrPtr, size_t* arrSize, jm_string str, const fmi3_xml_primitive_type_t* primType);

int fmi3_xml_parse_attr_valueref_list(fmi3_xml_parser_context_t* context, fmi3_xml_elm_t elmID, const fmi3_xml_attr_t attrID, int required, jm_vector(fmi3_value_reference_t)* vrs);

int fmi3_xml_is_attr_defined( fmi3_xml_parser_context_t* context, const fmi3_xml_attr_t attrID);
jm_string fmi3_xml_peek_attr_str(fmi3_xml_parser_context_t* context, const fmi3_xml_attr_t attrID);
int fmi3_xml_get_attr_str(fmi3_xml_parser_context_t* context, fmi3_xml_elm_t elmID, const fmi3_xml_attr_t attrID, int required, const char** valp);

void fmi3_xml_set_element_handle(fmi3_xml_parser_context_t* context, const char* elm, fmi3_xml_elm_t id);
const char* fmi3_xml_elmid_to_name(fmi3_xml_parser_context_t* context, fmi3_xml_elm_t id);

#ifdef __cplusplus
}
#endif

#endif /* FMI3_XML_XMLPARSER_H */
