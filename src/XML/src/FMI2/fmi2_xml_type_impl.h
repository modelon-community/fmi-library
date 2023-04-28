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

#ifndef FMI2_XML_TYPEIMPL_H
#define FMI2_XML_TYPEIMPL_H

#include <JM/jm_named_ptr.h>
#include <JM/jm_string_set.h>
#include <FMI2/fmi2_xml_model_description.h>

#include "fmi2_xml_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup Type definitions supporting structures

  The type structures are designed to save memory and
  to enable handling of diff-sets in the future.
  For each basic type (Real, Integer, each Enumeration, String & Boolean)
  there is a default instance of fmi2_xml_variable_type_base_t with
  structKind=fmi2_xml_type_struct_enu_props. Those instances have
  nextLayer = NULL.

  Each type definition creates 1 or 2 instances:
  (1)  instance with structKind=fmi2_xml_type_struct_enu_typedef
    that gives the name & description of the type. baseType is a
    pointer to either  fmi2_xml_type_struct_enu_base or fmi2_xml_type_struct_enu_props
  (2)  optionally, an instance with the structKind=fmi2_xml_type_contrain_properties
    providing information on min/max/quantity/etc. baseType is a pointer
    to the default properties struct.

   Each variable definition may create none, 1 or 2 instances:
    (1) fmi2_xml_type_struct_enu_start providing the start value
    (2) structKind=fmi2_xml_type_struct_enu_props  providing information on min/max/quantity/etc.
    baseType is a pointer to either fmi2_xml_type_struct_enu_base or fmi2_xml_type_struct_enu_typedef.

   For Enums there are two different property structs since type definition
   gives the list of items and variables give min and max.
 */

typedef enum {
    fmi2_xml_type_struct_enu_typedef,
    fmi2_xml_type_struct_enu_props,
    fmi2_xml_type_struct_enu_start
} fmi2_xml_type_struct_kind_enu_t;

typedef struct fmi2_xml_variable_type_base_t fmi2_xml_variable_type_base_t;

/*
   This is the super type for all type related structs. The typical workflow is to check the 'structKind' field,
   and then typecast accordingly.

   The complete variable information is an aggregate of several 'structKind' structs, placed in an ordered list
   'nextLayer'. An attempt to show the ordering is made in the 'diagrams.drawio' file.
*/
struct fmi2_xml_variable_type_base_t {
    fmi2_xml_variable_type_base_t* nextLayer;   /* the next layer in the type aggregate */
    fmi2_xml_type_struct_kind_enu_t structKind; /* the actual (sub) type */
    fmi2_base_type_enu_t baseType;              /* the FMI base type */
    char isRelativeQuantity;                    /* relativeQuantity flag (only used in fmi2_xml_real_type_props_t) */
    char isUnbounded;                           /* unbounded flag        (only used in fmi2_xml_real_type_props_t) */

    fmi2_xml_variable_type_base_t* next;        /* should only be used for deallocation */
};

/*
    Variable type definition is general and is used for all types.
    This is done to enable easy handling of SimpleType XML element
    (specific type element comes next).
*/
struct fmi2_xml_variable_typedef_t {
    fmi2_xml_variable_type_base_t super;
    jm_string description;
    char typeName[1];
};

typedef struct fmi2_xml_real_type_props_t {
    fmi2_xml_variable_type_base_t super;
    jm_string quantity;

    fmi2_xml_display_unit_t* displayUnit;

    double typeMin;
    double typeMax;
    double typeNominal;
} fmi2_xml_real_type_props_t;

typedef struct fmi2_xml_integer_type_props_t {
    fmi2_xml_variable_type_base_t super;

    jm_string  quantity;

    int typeMin;
    int typeMax;
} fmi2_xml_integer_type_props_t;

typedef fmi2_xml_variable_type_base_t fmi2_xml_string_type_props_t;
typedef fmi2_xml_variable_type_base_t fmi2_xml_bool_type_props_t;

typedef struct fmi2_xml_enum_type_item_t {
    jm_string itemName;
    int value;
    char itemDesciption[1];
} fmi2_xml_enum_type_item_t;

typedef struct fmi2_xml_enum_variable_props_t {
    fmi2_xml_variable_type_base_t super;

    jm_string  quantity;

    int typeMin;
    int typeMax;
} fmi2_xml_enum_variable_props_t;

typedef struct fmi2_xml_enum_typedef_props_t {
    fmi2_xml_enum_variable_props_t base;
    jm_vector(jm_named_ptr) enumItems;
} fmi2_xml_enum_typedef_props_t;

typedef struct fmi2_xml_variable_start_real_t {
    fmi2_xml_variable_type_base_t super;
    double start;
} fmi2_xml_variable_start_real_t ;

/* fmi2_xml_variable_start_integer is used for boolean and enums as well*/
typedef struct fmi2_xml_variable_start_integer_t {
    fmi2_xml_variable_type_base_t super;
    int start;
} fmi2_xml_variable_start_integer_t ;

typedef struct fmi2_xml_variable_start_string_t {
    fmi2_xml_variable_type_base_t super;
    char start[1];
} fmi2_xml_variable_start_string_t;

static fmi2_xml_variable_type_base_t* fmi2_xml_find_type_struct(fmi2_xml_variable_type_base_t* type, fmi2_xml_type_struct_kind_enu_t kind) {
    fmi2_xml_variable_type_base_t* typeBase = type;
    while(typeBase) {
        if(typeBase->structKind == kind) return typeBase;
        typeBase = typeBase->nextLayer;
    }
    return 0;
}

static fmi2_xml_variable_type_base_t* fmi2_xml_find_type_props(fmi2_xml_variable_type_base_t* type) {
    fmi2_xml_variable_type_base_t* typeBase = type;
    while(typeBase) {
        if(typeBase->structKind == fmi2_xml_type_struct_enu_props) return typeBase;
        typeBase = typeBase->nextLayer;
    }
    return 0;
}

struct fmi2_xml_type_definitions_t {
    jm_vector(jm_named_ptr) typeDefinitions;

    jm_string_set quantities;

    fmi2_xml_variable_type_base_t* typePropsList;

    fmi2_xml_real_type_props_t defaultRealType;
    fmi2_xml_enum_typedef_props_t defaultEnumType;
    fmi2_xml_integer_type_props_t defaultIntegerType;
    fmi2_xml_bool_type_props_t defaultBooleanType;
    fmi2_xml_string_type_props_t defaultStringType;
};

extern void fmi2_xml_init_type_definitions(fmi2_xml_type_definitions_t* td, jm_callbacks* cb) ;

extern void fmi2_xml_free_type_definitions_data(fmi2_xml_type_definitions_t* td);

extern void fmi2_xml_init_integer_typedef(fmi2_xml_integer_typedef_t* type);

extern void fmi2_xml_init_enum_typedef(fmi2_xml_enumeration_typedef_t* type, jm_callbacks* cb);

extern void fmi2_xml_free_enum_type(jm_named_ptr named);

fmi2_xml_variable_type_base_t* fmi2_xml_alloc_variable_type_props(fmi2_xml_type_definitions_t* td, fmi2_xml_variable_type_base_t* base, size_t typeSize);

fmi2_xml_variable_type_base_t* fmi2_xml_alloc_variable_type_start(fmi2_xml_type_definitions_t* td,fmi2_xml_variable_type_base_t* base, size_t typeSize);

fmi2_xml_real_type_props_t* fmi2_xml_parse_real_type_properties(fmi2_xml_parser_context_t* context, fmi2_xml_elm_enu_t elmID);

fmi2_xml_integer_type_props_t *fmi2_xml_parse_integer_type_properties(fmi2_xml_parser_context_t* context, fmi2_xml_elm_enu_t elmID);

extern int fmi2_check_last_elem_is_specific_type(fmi2_xml_parser_context_t *context);

fmi2_xml_variable_type_base_t* fmi2_get_declared_type(fmi2_xml_parser_context_t *context, fmi2_xml_elm_enu_t elmID, fmi2_xml_variable_type_base_t* defaultType);

#ifdef __cplusplus
}
#endif

#endif /* FMI2_XML_TYPEIMPL_H */
