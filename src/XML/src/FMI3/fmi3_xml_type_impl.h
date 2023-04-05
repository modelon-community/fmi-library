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

/** \defgroup Type definitions supporting structures

  The type structures are designed to save memory and
  to enable handling of diff-sets in the future.
  For each basic type (FloatXX, IntXX, each Enumeration, String & Boolean)
  there is a default instance of fmi3_xml_variable_type_base_t with
  structKind=fmi3_xml_type_struct_enu_props. Those instances have
  nextLayer = NULL.

  Each type definition creates 1 or 2 instances:
  (1)  instance with structKind=fmi3_xml_type_struct_enu_typedef
    that gives the name & description of the type. baseType is a
    pointer to either  fmi3_xml_type_struct_enu_base or fmi3_xml_type_struct_enu_props
  (2)  optionally, an instance with the structKind=fmi3_xml_type_contrain_properties
    providing information on min/max/quantity/etc. baseType is a pointer
    to the default properties struct.

   Each variable definition may create none, 1 or 2 instances:
    (1) fmi3_xml_type_struct_enu_start providing the start value
    (2) structKind=fmi3_xml_type_struct_enu_props  providing information on min/max/quantity/etc.
    baseType is a pointer to either fmi3_xml_type_struct_enu_base or fmi3_xml_type_struct_enu_typedef.

   For Enums there are two different property structs since type definition
   gives the list of items and variables give min and max.
 */

typedef enum {
    fmi3_xml_type_struct_enu_typedef,
    fmi3_xml_type_struct_enu_props,
    fmi3_xml_type_struct_enu_start
} fmi3_xml_type_struct_kind_enu_t;

typedef struct fmi3_xml_variable_type_base_t fmi3_xml_variable_type_base_t;

/*
   This is the super type for all type related structs. The typical workflow is to check the 'structKind' field,
   and then typecast accordingly.

   The complete variable information is an aggregate of several 'structKind' structs, placed in an ordered list
   'nextLayer'. An attempt to show the ordering is made in the 'diagrams.drawio' file.
*/
struct fmi3_xml_variable_type_base_t {
    fmi3_xml_variable_type_base_t* nextLayer;   /* the next layer in the type aggregate */
    fmi3_xml_type_struct_kind_enu_t structKind; /* the actual (sub) type */
    fmi3_base_type_enu_t baseType;              /* the FMI base type */
    char isRelativeQuantity;                    /* relativeQuantity flag (only used in fmi3_xml_real_type_props_t) */
    char isUnbounded;                           /* unbounded flag        (only used in fmi3_xml_real_type_props_t) */

    fmi3_xml_variable_type_base_t* next;        /* should only be used for deallocation */
};

/*
    Variable type definition is general and is used for all types.
    This is done to enable easy handling of SimpleType XML element
    (specific type element comes next).
*/
struct fmi3_xml_variable_typedef_t {
    fmi3_xml_variable_type_base_t super;
    jm_string description;
    char typeName[1];
};

typedef union fmi3_float_union_t {
    void* ptr; /* only used for deallocation (any other pointer field could be used, but this makes the code cleaner) */
    fmi3_float64_t* array64s;
    fmi3_float32_t* array32s;
    fmi3_float64_t scalar64s;
    fmi3_float32_t scalar32s;
} fmi3_float_union_t;

typedef struct fmi3_xml_float_type_props_t {
    fmi3_xml_variable_type_base_t super;
    jm_string quantity;

    fmi3_xml_display_unit_t* displayUnit;

    fmi3_float_union_t typeMin;
    fmi3_float_union_t typeMax;
    fmi3_float_union_t typeNominal;
} fmi3_xml_float_type_props_t;

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

/* NEW */
typedef struct fmi3_xml_int_type_props_t {
    fmi3_xml_variable_type_base_t super;

    jm_string  quantity;

    fmi3_int_union_t typeMin;
    fmi3_int_union_t typeMax;
} fmi3_xml_int_type_props_t;

typedef fmi3_xml_variable_type_base_t fmi3_xml_string_type_props_t;
typedef fmi3_xml_variable_type_base_t fmi3_xml_bool_type_props_t;

typedef struct fmi3_xml_enum_type_item_t {
    jm_string itemName;
    int value;
    char itemDesciption[1];
} fmi3_xml_enum_type_item_t;

static int fmi1_xml_compare_enum_val (const void* first, const void* second) {
    const jm_named_ptr* a = first;
    const jm_named_ptr* b = second;
    fmi3_xml_enum_type_item_t* ai = a->ptr;
    fmi3_xml_enum_type_item_t* bi = b->ptr;
    return (ai->value - bi->value);
}

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

typedef struct fmi3_xml_variable_start_float_t {
    fmi3_xml_variable_type_base_t super;
    fmi3_float_union_t start;
} fmi3_xml_variable_start_float_t;

typedef struct fmi3_xml_variable_start_int_t {
    fmi3_xml_variable_type_base_t super;
    fmi3_int_union_t start;
} fmi3_xml_variable_start_int_t;

typedef struct fmi3_xml_variable_start_string_t {
    fmi3_xml_variable_type_base_t super;
    char start[1];
} fmi3_xml_variable_start_string_t;

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
    jm_vector(jm_named_ptr) typeDefinitions;

    jm_string_set quantities;

    /* intended purpose seems to be as memory deallocation ptr, but also used in fmi3_xml_handle_Item (TODO: see if can be changed to single purpose) */
    fmi3_xml_variable_type_base_t* typePropsList;

    fmi3_xml_float_type_props_t defaultFloat64Type;
    fmi3_xml_float_type_props_t defaultFloat32Type;
    fmi3_xml_enum_typedef_props_t defaultEnumType;
    fmi3_xml_int_type_props_t defaultInt64Type;
    fmi3_xml_int_type_props_t defaultInt32Type;
    fmi3_xml_int_type_props_t defaultInt16Type;
    fmi3_xml_int_type_props_t defaultInt8Type;
    fmi3_xml_int_type_props_t defaultUInt64Type;
    fmi3_xml_int_type_props_t defaultUInt32Type;
    fmi3_xml_int_type_props_t defaultUInt16Type;
    fmi3_xml_int_type_props_t defaultUInt8Type;
    fmi3_xml_bool_type_props_t defaultBooleanType;
    fmi3_xml_string_type_props_t defaultStringType;
};

extern void fmi3_xml_init_type_definitions(fmi3_xml_type_definitions_t* td, jm_callbacks* cb) ;

extern void fmi3_xml_free_type_definitions_data(fmi3_xml_type_definitions_t* td);

extern void fmi3_xml_init_enum_typedef(fmi3_xml_enumeration_typedef_t* type, jm_callbacks* cb);

extern void fmi3_xml_free_enum_type(jm_named_ptr named);

fmi3_xml_variable_type_base_t* fmi3_xml_alloc_variable_type_props(fmi3_xml_type_definitions_t* td, fmi3_xml_variable_type_base_t* base, size_t typeSize);

fmi3_xml_variable_type_base_t* fmi3_xml_alloc_variable_type_start(fmi3_xml_type_definitions_t* td,fmi3_xml_variable_type_base_t* base, size_t typeSize);

fmi3_xml_float_type_props_t* fmi3_xml_parse_float_type_properties(fmi3_xml_parser_context_t* context, fmi3_xml_elm_enu_t elmID, fmi3_xml_float_type_props_t* defaultType, const fmi3_xml_primitive_type_t* primType);

fmi3_xml_int_type_props_t* fmi3_xml_parse_intXX_type_properties(fmi3_xml_parser_context_t* context, fmi3_xml_elm_enu_t elmID, fmi3_xml_int_type_props_t* defaultType, const fmi3_xml_primitive_type_t* primType);

/* TODO: what are these extern? */
extern int fmi3_check_last_elem_is_specific_type(fmi3_xml_parser_context_t *context);

extern jm_named_ptr fmi3_xml_variable_type_alloc(fmi3_xml_parser_context_t* context, jm_vector(char)* name, jm_vector(char)* description, size_t size);

extern void* fmi3_xml_variable_type_create(fmi3_xml_parser_context_t* context, size_t size, jm_vector(jm_named_ptr)* typeList );

fmi3_xml_variable_type_base_t* fmi3_get_declared_type(fmi3_xml_parser_context_t *context, fmi3_xml_elm_enu_t elmID, fmi3_xml_variable_type_base_t* defaultType);

#ifdef __cplusplus
}
#endif

#endif /* FMI3_XML_TYPEIMPL_H */
