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

#ifndef FMI1_XML_TYPEIMPL_H
#define FMI1_XML_TYPEIMPL_H

#include <JM/jm_named_ptr.h>
#include <JM/jm_string_set.h>
#include <FMI1/fmi1_xml_model_description.h>

#include "fmi1_xml_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup Type definitions supporting structures

  For each basic type there is exactly one instance of
  fmi1_xml_variable_type_base_t with structKind=fmi1_xml_type_struct_enu_base.
  Those instances have nextLayer = NULL.

  Each type definition creates 1 or 2 instances:
  (1)  instance with structKind=fmi1_xml_type_struct_enu_typedef
    that gives the name & description of the type. baseType is a
    pointer to either  fmi1_xml_type_struct_enu_base or fmi1_xml_type_struct_enu_props
  (2)  optionally, an instance with the structKind=fmi1_xml_type_contrain_properties
    providing information on min/max/quantity/etc. baseType is a pointer
    to structKind=fmi1_xml_type_struct_enu_base

   Each variable definition may create none, 1 or 2 instances:
    (1) fmi1_xml_type_struct_enu_start providing the start value
    (2) structKind=fmi1_xml_type_struct_enu_props  providing information on min/max/quantity/etc.
    baseType is a pointer to either fmi1_xml_type_struct_enu_base or fmi1_xml_type_struct_enu_typedef.
 */

typedef enum {
    fmi1_xml_type_struct_enu_base,
    fmi1_xml_type_struct_enu_typedef,
    fmi1_xml_type_struct_enu_props,
    fmi1_xml_type_struct_enu_start
} fmi1_xml_type_struct_kind_enu_t;

typedef struct fmi1_xml_variable_type_base_t fmi1_xml_variable_type_base_t;

/*
   This is the super type for all type related structs. The typical workflow is to check the 'structKind' field,
   and then typecast accordingly.

   The complete variable information is an aggregate of several 'structKind' structs, placed in an ordered list
   'nextLayer'. An attempt to show the ordering is made in the 'diagrams.drawio' file.
*/
struct fmi1_xml_variable_type_base_t {
    fmi1_xml_variable_type_base_t* nextLayer;   /* the next layer in the type aggregate */
    fmi1_xml_type_struct_kind_enu_t structKind; /* the actual (sub) type */
    fmi1_base_type_enu_t baseType;              /* the FMI base type */
    char relativeQuantity;                      /* relativeQuantity flag (only used in fmi1_xml_real_type_props_t) */
    char isFixed;                               /* (only used for fmi1_xml_type_struct_enu_start) */

    fmi1_xml_variable_type_base_t* next;        /* should only be used for deallocation */
};

/* Variable type definition is general and is used for all types*/
struct fmi1_xml_variable_typedef_t {
    fmi1_xml_variable_type_base_t super;
    jm_string description;
    char typeName[1];
};

typedef struct fmi1_xml_real_type_props_t {
    fmi1_xml_variable_type_base_t super;
    jm_string quantity;

    fmi1_xml_display_unit_t* displayUnit;

    double typeMin;
    double typeMax;
    double typeNominal;
} fmi1_xml_real_type_props_t;

typedef struct fmi1_xml_integer_type_props_t {
    fmi1_xml_variable_type_base_t super;

    jm_string  quantity;

    int typeMin;
    int typeMax;
} fmi1_xml_integer_type_props_t;

typedef fmi1_xml_variable_type_base_t fmi1_xml_string_type_props_t;
typedef fmi1_xml_variable_type_base_t fmi1_xml_bool_type_props_t;

typedef struct fmi1_xml_enum_type_item_t {
    jm_string itemName;
    char itemDescription[1];
} fmi1_xml_enum_type_item_t;

typedef struct fmi1_xml_enum_type_props_t {
    fmi1_xml_variable_type_base_t super;

    jm_string quantity;
    int typeMin;
    int typeMax;
    jm_vector(jm_named_ptr) enumItems;
} fmi1_xml_enum_type_props_t;

typedef struct fmi1_xml_variable_start_real_t {
    fmi1_xml_variable_type_base_t super;
    double start;
} fmi1_xml_variable_start_real_t ;

/* fmi1_xml_variable_start_integer is used for boolean and enums as well*/
typedef struct fmi1_xml_variable_start_integer_t {
    fmi1_xml_variable_type_base_t super;
    int start;
} fmi1_xml_variable_start_integer_t ;

typedef struct fmi1_xml_variable_start_string_t {
    fmi1_xml_variable_type_base_t super;
    char start[1];
} fmi1_xml_variable_start_string_t;

/* WARNING: if trying to get the properties of a variable, don't use this function, use 'fmi1_xml_find_type_props' */
static fmi1_xml_variable_type_base_t* fmi1_xml_find_type_struct(fmi1_xml_variable_type_base_t* type, fmi1_xml_type_struct_kind_enu_t kind) {
    fmi1_xml_variable_type_base_t* typeBase = type;
    while(typeBase) {
        if(typeBase->structKind == kind) return typeBase;
        typeBase = typeBase->nextLayer;
    }
    return 0;
}

/* NOTE: looks for both enu_base and enu_props */
static fmi1_xml_variable_type_base_t* fmi1_xml_find_type_props(fmi1_xml_variable_type_base_t* type) {
    fmi1_xml_variable_type_base_t* typeBase = type;
    while(typeBase) {
        if((typeBase->structKind == fmi1_xml_type_struct_enu_base)
            || (typeBase->structKind == fmi1_xml_type_struct_enu_props)) return typeBase;
        typeBase = typeBase->nextLayer;
    }
    return 0;
}

struct fmi1_xml_type_definitions_t {
    jm_vector(jm_named_ptr) typeDefinitions;

    jm_string_set quantities;

    fmi1_xml_variable_type_base_t* typePropsList;

    fmi1_xml_real_type_props_t defaultRealType;
    fmi1_xml_enum_type_props_t defaultEnumType;
    fmi1_xml_integer_type_props_t defaultIntegerType;
    fmi1_xml_bool_type_props_t defaultBooleanType;
    fmi1_xml_string_type_props_t defaultStringType;
};

extern void fmi1_xml_init_type_definitions(fmi1_xml_type_definitions_t* td, jm_callbacks* cb) ;

extern void fmi1_xml_free_type_definitions_data(fmi1_xml_type_definitions_t* td);

extern void fmi1_xml_init_integer_typedef(fmi1_xml_integer_typedef_t* type);

extern void fmi1_xml_init_enum_typedef(fmi1_xml_enumeration_typedef_t* type, jm_callbacks* cb);

extern void fmi1_xml_free_enum_type(jm_named_ptr named);

fmi1_xml_variable_type_base_t* fmi1_xml_alloc_variable_type_props(fmi1_xml_type_definitions_t* td, fmi1_xml_variable_type_base_t* base, size_t typeSize);

fmi1_xml_variable_type_base_t* fmi1_xml_alloc_variable_type_start(fmi1_xml_type_definitions_t* td,fmi1_xml_variable_type_base_t* base, size_t typeSize);

fmi1_xml_real_type_props_t* fmi1_xml_parse_real_type_properties(fmi1_xml_parser_context_t* context, fmi1_xml_elm_enu_t elmID);

fmi1_xml_integer_type_props_t *fmi1_xml_parse_integer_type_properties(fmi1_xml_parser_context_t* context, fmi1_xml_elm_enu_t elmID);

extern int fmi1_check_last_elem_is_specific_type(fmi1_xml_parser_context_t *context);

fmi1_xml_variable_type_base_t* fmi1_get_declared_type(fmi1_xml_parser_context_t *context, fmi1_xml_elm_enu_t elmID, fmi1_xml_variable_type_base_t* defaultType);

#ifdef __cplusplus
}
#endif

#endif /* FMI1_XML_TYPEIMPL_H */
