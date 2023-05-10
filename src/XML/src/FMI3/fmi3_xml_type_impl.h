/*
    Copyright (C) 2012 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

     This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#ifndef FMI3_XML_TYPEIMPL_H
#define FMI3_XML_TYPEIMPL_H

#include <JM/jm_named_ptr.h>
#include <JM/jm_string_set.h>
#include <FMI3/fmi3_xml_model_description.h>

#include "fmi3_xml_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef union fmi3_float_union_t {
    void* ptr; /* only used for deallocation (any other pointer field could be used, but this makes the code cleaner) */
    fmi3_float64_t* array64s;
    fmi3_float32_t* array32s;
    fmi3_float64_t scalar64s;
    fmi3_float32_t scalar32s;
} fmi3_float_union_t;

typedef union fmi3_int_union_t {
    void* ptr; /* only used for deallocation (any other pointer field could be used, but this makes the code cleaner) */
    fmi3_int64_t*   array64s;
    fmi3_int32_t*   array32s;
    fmi3_int16_t*   array16s;
    fmi3_int8_t*    array8s;
    fmi3_uint64_t*  array64u;
    fmi3_uint32_t*  array32u;
    fmi3_uint16_t*  array16u;
    fmi3_uint8_t*   array8u;
    fmi3_int64_t    scalar64s;
    fmi3_int32_t    scalar32s;
    fmi3_int16_t    scalar16s;
    fmi3_int8_t     scalar8s;
    fmi3_uint64_t   scalar64u;
    fmi3_uint32_t   scalar32u;
    fmi3_uint16_t   scalar16u;
    fmi3_uint8_t    scalar8u;
} fmi3_int_union_t;

/**
 * A specifier telling what kind of struct this actually is.
 *
 * ========================================
 * === fmi3_xml_type_struct_enu_typedef ===
 * ========================================
 *
 * A TypeDefinition. The next node is always a _props specific to the primitive type for
 * this TypeDefinition. The default types (i.e. if a variable doesn't have any declaredType)
 * are not represented by any _typedef - only by _props.
 *
 * ======================================
 * === fmi3_xml_type_struct_enu_props ===
 * ======================================
 *
 * Attributes specific to the primitive type. Each TypeDefinition has one, and every default
 * type is also represented by a _props.
 *
 * Every Variable that defines a type-specific attribute also has a variable-specific
 * _props. Optional attributes that are not present on the Variable get their value copied
 * copied from it's TypeDefinition (or default type).
 *
 * So to conclude there are three cases that cause this struct:
 *      - Default type    (always)
 *      - TypeDefinition  (always)
 *      - Variable        (if has type attribute)
 *
 * The next node for a _props on a TypeDefinition (i.e. _typedef._props) is always
 * the default type. XXX: But it seems to never be used, since we don't have diff-sets.
 *
 * The next node for a _props on a Variable is its declaredType TypeDefinition (or the
 * default type).
 * This is also how a Variable is connected to it's declaredType. I.e. to find it, one
 * needs to iterate the nodes until a _typedef is found.
 *
 * XXX:
 * It seems the only reason to keep references to the "fallback" _props is to find the declared type.
 * And to eventually create diff-sets, but currently we just copy.
 *
 * ======================================
 * === fmi3_xml_type_struct_enu_start ===
 * ======================================
 *
 * Keeps the start attribute for a Variable. Is only used if the Variable defines a
 * start value.
 *
 * The next node is the _props.
 *
 * -----------------------------------------------------------------------------
 *
 * Examples of how the structure will be for some variables:
 *
 * <Float64/>
 *      type: -------------------------------------------------------------------------------------> (default>_props_t
 *
 * <Float64 start="..."/>
 *      type: start_t -----------------------------------------------------------------------------> (default)_props_t
 *
 * <Float64 start="..." declaredType="..."/>
 *      type: start_t -----------------------> (declaredType)_typedef_t -> (declaredType)_props_t -> (default)_props_t
 *
 * <Float64 start="..." declaredType="..." max="..."/>
 *      type: start_t -> (variable)_props_t -> (declaredType)_typedef_t -> (declaredType)_props_t -> (default)_props_t
 */
typedef enum {
    fmi3_xml_type_struct_enu_typedef, // Base object for a user-defined TypeDefinition
    fmi3_xml_type_struct_enu_props,   // Type-specific attributes for TypeDefiniton or Variable
    fmi3_xml_type_struct_enu_start    // Type-specific start value
} fmi3_xml_type_struct_kind_enu_t;

/**
 * The super type for all type related structs. The typical workflow is to check the 'structKind' field,
 * and then typecast accordingly.
 *
 * The complete variable information is an aggregate of several 'structKind' structs, placed in an ordered list
 * 'nextLayer'. An attempt to show the ordering is made in the 'diagrams.drawio' file.
 *
 * More info can also be found at the top of the file where this type is described.
 *
 * Note that all objects of this type are appended to a list owned by the type definitions object.
 */
typedef struct fmi3_xml_variable_type_base_t fmi3_xml_variable_type_base_t;
struct fmi3_xml_variable_type_base_t {
    fmi3_xml_variable_type_base_t* nextLayer;   /* The next layer in the type aggregate */ // TODO: Rename to next
    fmi3_xml_type_struct_kind_enu_t structKind; /* The actual (sub) type */
    fmi3_base_type_enu_t baseType;              /* The FMI base type */
    char isRelativeQuantity;                    /* RelativeQuantity flag (only used in fmi3_xml_real_type_props_t) */
    char isUnbounded;                           /* Unbounded flag        (only used in fmi3_xml_real_type_props_t) */

    // TODO: Remove (create a new jm_vector on typeDefinitions that works the same)
    fmi3_xml_variable_type_base_t* next;        /* For deallocation: the next node in the deallocation list */
};

// ----------------------------------------------------------------------------
// Typedef
// ----------------------------------------------------------------------------

/**
 *  Variable type definition is general and is used for all types.
 *  This is done to enable easy handling of SimpleType XML element
 *  (specific type element comes next).
 */
struct fmi3_xml_variable_typedef_t {
    fmi3_xml_variable_type_base_t super;
    jm_string description;
    char typeName[1];
};

// ----------------------------------------------------------------------------
// Props
// ----------------------------------------------------------------------------

typedef struct fmi3_xml_float_type_props_t {
    fmi3_xml_variable_type_base_t super;
    jm_string quantity;

    fmi3_xml_display_unit_t* displayUnit;

    fmi3_float_union_t typeMin;
    fmi3_float_union_t typeMax;
    fmi3_float_union_t typeNominal;
} fmi3_xml_float_type_props_t;

typedef struct fmi3_xml_int_type_props_t {
    fmi3_xml_variable_type_base_t super;

    jm_string  quantity;

    fmi3_int_union_t typeMin;
    fmi3_int_union_t typeMax;
} fmi3_xml_int_type_props_t;

typedef struct fmi3_xml_enum_variable_props_t {
    fmi3_xml_variable_type_base_t super;

    jm_string  quantity;

    fmi3_int32_t typeMin;
    fmi3_int32_t typeMax;
} fmi3_xml_enum_variable_props_t;

typedef struct fmi3_xml_enum_typedef_props_t {
    fmi3_xml_enum_variable_props_t base;
    jm_vector(jm_named_ptr) enumItems;
} fmi3_xml_enum_typedef_props_t;

typedef struct fmi3_xml_binary_type_props_t {
    fmi3_xml_variable_type_base_t super;

    jm_string mimeType;
    size_t maxSize;
} fmi3_xml_binary_type_props_t;

typedef struct fmi3_xml_clock_type_props_t {
    fmi3_xml_variable_type_base_t super;

    fmi3_boolean_t canBeDeactivated;
    fmi3_boolean_t supportsFraction;
    fmi3_interval_variability_enu_t intervalVariability;
    fmi3_uint32_t  priority;
    fmi3_uint64_t  resolution;
    fmi3_uint64_t  intervalCounter;
    fmi3_uint64_t  shiftCounter;
    fmi3_float32_t intervalDecimal;
    fmi3_float32_t shiftDecimal;
} fmi3_xml_clock_type_props_t;

typedef fmi3_xml_variable_type_base_t fmi3_xml_string_type_props_t;
typedef fmi3_xml_variable_type_base_t fmi3_xml_bool_type_props_t;

// ----------------------------------------------------------------------------
// Start
// ----------------------------------------------------------------------------

typedef struct fmi3_xml_float_variable_start_t {
    fmi3_xml_variable_type_base_t super;
    fmi3_float_union_t start;
} fmi3_xml_float_variable_start_t;

typedef struct fmi3_xml_int_variable_start_t {
    fmi3_xml_variable_type_base_t super;
    fmi3_int_union_t start;
} fmi3_xml_int_variable_start_t;

typedef struct fmi3_xml_variable_start_string_t {
    fmi3_xml_variable_type_base_t super;
    jm_vector(jm_voidp) stringStartValues;
    char start[1];
} fmi3_xml_string_variable_start_t;

typedef struct fmi3_xml_binary_variable_start_t {
    fmi3_xml_variable_type_base_t super;
    jm_vector(jm_voidp) binaryStartValues;
    jm_vector(size_t) binaryStartValuesSize;
    size_t nStart;
    fmi3_uint8_t start[1]; /* NOTE: Can be longer than 1. Memory can be allocated outside of struct boundary. */
} fmi3_xml_binary_variable_start_t;

// ----------------------------------------------------------------------------


typedef struct fmi3_xml_enum_type_item_t {
    jm_string itemName;
    int value;
    char itemDescription[1];
} fmi3_xml_enum_type_item_t;

static fmi3_xml_variable_type_base_t* fmi3_xml_find_type_struct(fmi3_xml_variable_type_base_t* type, fmi3_xml_type_struct_kind_enu_t kind) {
    fmi3_xml_variable_type_base_t* typeBase = type;
    while(typeBase) {
        if(typeBase->structKind == kind) return typeBase;
        typeBase = typeBase->nextLayer;
    }
    return 0;
}

static fmi3_xml_variable_type_base_t* fmi3_xml_find_type_props(fmi3_xml_variable_type_base_t* type) {
    fmi3_xml_variable_type_base_t* typeBase = type;
    while(typeBase) {
        if(typeBase->structKind == fmi3_xml_type_struct_enu_props) return typeBase;
        typeBase = typeBase->nextLayer;
    }
    return 0;
}

struct fmi3_xml_type_definitions_t {

    // All the parsed TypeDefinitions. Type of .ptr: fmi3_xml_variable_typedef_t
    jm_vector(jm_named_ptr) typeDefinitions;

    jm_string_set quantities; /* Storage for 'quantity' attribute for Variables and TypeDefinitions. */
    jm_string_set mimeTypes;  /* Storage for 'mimeType' attribute for Variables and TypeDefinitions. */

    /**
     * List that owns all allocated _props_t and _start_t objects, both for typedefs
     * and variables.
     *
     * The head points to the last allocated object, and needs to be updated when
     * new objects are added.
     *
     * XXX: We should rename it to reflect that it contains _start_t as well.
     */
    fmi3_xml_variable_type_base_t* typePropsList;

    fmi3_xml_float_type_props_t   defaultFloat64Type;
    fmi3_xml_float_type_props_t   defaultFloat32Type;
    fmi3_xml_enum_typedef_props_t defaultEnumType;
    fmi3_xml_int_type_props_t     defaultInt64Type;
    fmi3_xml_int_type_props_t     defaultInt32Type;
    fmi3_xml_int_type_props_t     defaultInt16Type;
    fmi3_xml_int_type_props_t     defaultInt8Type;
    fmi3_xml_int_type_props_t     defaultUInt64Type;
    fmi3_xml_int_type_props_t     defaultUInt32Type;
    fmi3_xml_int_type_props_t     defaultUInt16Type;
    fmi3_xml_int_type_props_t     defaultUInt8Type;
    fmi3_xml_bool_type_props_t    defaultBooleanType;
    fmi3_xml_string_type_props_t  defaultStringType;
    fmi3_xml_binary_type_props_t  defaultBinaryType;
    fmi3_xml_clock_type_props_t   defaultClockType;
};

extern void fmi3_xml_init_type_definitions(fmi3_xml_type_definitions_t* td, jm_callbacks* cb) ;

extern void fmi3_xml_free_type_definitions_data(fmi3_xml_type_definitions_t* td);

extern void fmi3_xml_init_enum_typedef(fmi3_xml_enumeration_typedef_t* type, jm_callbacks* cb);

extern void fmi3_xml_free_enum_type(jm_named_ptr named);

fmi3_xml_variable_type_base_t* fmi3_xml_alloc_variable_or_typedef_props(fmi3_xml_type_definitions_t* td, fmi3_xml_variable_type_base_t* base, size_t typeSize);

fmi3_xml_variable_type_base_t* fmi3_xml_alloc_variable_type_start(fmi3_xml_type_definitions_t* td,fmi3_xml_variable_type_base_t* base, size_t typeSize);

fmi3_xml_float_type_props_t* fmi3_xml_parse_float_type_properties(fmi3_xml_parser_context_t* context, fmi3_xml_elm_enu_t elmID, fmi3_xml_variable_type_base_t* fallbackType, const fmi3_xml_primitive_type_t* primType);
fmi3_xml_int_type_props_t* fmi3_xml_parse_intXX_type_properties(fmi3_xml_parser_context_t* context, fmi3_xml_elm_enu_t elmID, fmi3_xml_variable_type_base_t* fallbackType, const fmi3_xml_primitive_type_t* primType);
fmi3_xml_binary_type_props_t* fmi3_xml_parse_binary_type_properties(fmi3_xml_parser_context_t* context, fmi3_xml_elm_enu_t elmID, fmi3_xml_variable_type_base_t* fallbackType);
fmi3_xml_clock_type_props_t* fmi3_xml_parse_clock_type_properties(fmi3_xml_parser_context_t* context, fmi3_xml_elm_enu_t elmID, fmi3_xml_variable_type_base_t* fallbackType);

fmi3_xml_variable_type_base_t* fmi3_parse_declared_type_attr(fmi3_xml_parser_context_t *context, fmi3_xml_elm_enu_t elmID, fmi3_xml_variable_type_base_t* defaultType);

#ifdef __cplusplus
}
#endif

#endif /* FMI3_XML_TYPEIMPL_H */
