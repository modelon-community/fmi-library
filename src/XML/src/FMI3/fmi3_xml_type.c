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

#include <limits.h>
#include <float.h>
#include <string.h>
#include "JM/jm_named_ptr.h"

#include "fmi3_xml_model_description_impl.h"
#include "fmi3_xml_type_impl.h"
#include "fmi3_xml_unit_impl.h"
#include "fmi3_xml_parser.h"

static const char* module = "FMI3XML";

#define UINTXX_MIN (0)

unsigned int fmi3_xml_get_type_definition_list_size(fmi3_xml_type_definition_list_t* td) {
    return (unsigned int)jm_vector_get_size(jm_named_ptr)(&td->typeDefinitions);
}

fmi3_xml_variable_typedef_t* fmi3_xml_get_typedef(fmi3_xml_type_definition_list_t* td, unsigned int  index) {
    if(index >= fmi3_xml_get_type_definition_list_size(td)) return 0;
    return (fmi3_xml_variable_typedef_t*)jm_vector_get_item(jm_named_ptr)(&td->typeDefinitions, index).ptr;
}

const char* fmi3_xml_get_type_name(fmi3_xml_variable_typedef_t* t) {
    return t->typeName;
}

const char* fmi3_xml_get_type_description(fmi3_xml_variable_typedef_t* t) {
    const char * ret = t->description;
    return (ret ? ret : "");
}

fmi3_base_type_enu_t fmi3_xml_get_base_type(fmi3_xml_variable_typedef_t* t) {
    return t->super.baseType;
}

fmi3_xml_float_typedef_t* fmi3_xml_get_type_as_float(fmi3_xml_variable_typedef_t* t) {
    fmi3_base_type_enu_t bt = fmi3_xml_get_base_type(t);
    if (bt == fmi3_base_type_float64 || bt == fmi3_base_type_float32)
        return (fmi3_xml_float_typedef_t*)t;
    return NULL;
}

fmi3_xml_int_typedef_t* fmi3_xml_get_type_as_int(fmi3_xml_variable_typedef_t* t) {
    if (fmi3_xml_get_base_type(t) ==
            fmi3_base_type_int64  ||
            fmi3_base_type_int32  ||
            fmi3_base_type_int16  ||
            fmi3_base_type_int8   ||
            fmi3_base_type_uint64 ||
            fmi3_base_type_uint32 ||
            fmi3_base_type_uint16 ||
            fmi3_base_type_uint8) {
        return (fmi3_xml_int_typedef_t*)t;
    }
    return NULL;
}

fmi3_xml_enumeration_typedef_t* fmi3_xml_get_type_as_enum(fmi3_xml_variable_typedef_t* t) {
    if (fmi3_xml_get_base_type(t) == fmi3_base_type_enum) {
        return (fmi3_xml_enumeration_typedef_t*)t;
    }
    return NULL;
}

fmi3_xml_binary_typedef_t* fmi3_xml_get_type_as_binary(fmi3_xml_variable_typedef_t* t) {
    if (fmi3_xml_get_base_type(t) == fmi3_base_type_binary) {
        return (fmi3_xml_binary_typedef_t*)t;
    }
    return NULL;
}

fmi3_xml_clock_typedef_t* fmi3_xml_get_type_as_clock(fmi3_xml_variable_typedef_t* t) {
    if (fmi3_xml_get_base_type(t) == fmi3_base_type_clock) {
        return (fmi3_xml_clock_typedef_t*)t;
    }
    return NULL;
}

const char* fmi3_xml_get_type_quantity(fmi3_xml_variable_typedef_t* t) {
    fmi3_xml_variable_type_base_t* props = t->super.nextLayer;
    if(props->structKind != fmi3_xml_type_struct_enu_props) return NULL;

    switch(props->baseType) {
    case fmi3_base_type_float64:
    case fmi3_base_type_float32:
        return ((fmi3_xml_float_type_props_t*)props)->quantity;
    case fmi3_base_type_int64:
    case fmi3_base_type_int32:
    case fmi3_base_type_int16:
    case fmi3_base_type_int8:
    case fmi3_base_type_uint64:
    case fmi3_base_type_uint32:
    case fmi3_base_type_uint16:
    case fmi3_base_type_uint8:
        return ((fmi3_xml_int_type_props_t*)props)->quantity;
    case fmi3_base_type_bool:
    case fmi3_base_type_binary:
    case fmi3_base_type_clock:
    case fmi3_base_type_str:
        return NULL;
    case fmi3_base_type_enum:
        return ((fmi3_xml_enum_variable_props_t*)props)->quantity;
    default:
        assert(0); /* Implementation error */
        return NULL;
    }
}

// -----------------------------------------------------------------------------
// Float64
// -----------------------------------------------------------------------------

static fmi3_xml_float_type_props_t* fmi3_xml_get_float_type_props(fmi3_xml_float_typedef_t* t) {
    fmi3_xml_variable_typedef_t* vt = (void*)t;
    return (void*)(vt->super.nextLayer);
}

fmi3_float64_t fmi3_xml_get_float64_type_min(fmi3_xml_float_typedef_t* t) {
    return fmi3_xml_get_float_type_props(t)->typeMin.scalar64s;
}

fmi3_float64_t fmi3_xml_get_float64_type_max(fmi3_xml_float_typedef_t* t) {
    return fmi3_xml_get_float_type_props(t)->typeMax.scalar64s;
}

fmi3_float64_t fmi3_xml_get_float64_type_nominal(fmi3_xml_float_typedef_t* t) {
    return fmi3_xml_get_float_type_props(t)->typeNominal.scalar64s;
}

fmi3_xml_unit_t* fmi3_xml_get_float64_type_unit(fmi3_xml_float_typedef_t* t) {
    fmi3_xml_display_unit_t* du = fmi3_xml_get_float_type_props(t)->displayUnit;
    if (du) {
        return du->baseUnit;
    }
    return NULL;
}

int fmi3_xml_get_float64_type_is_relative_quantity(fmi3_xml_float_typedef_t* t) {
    return fmi3_xml_get_float_type_props(t)->super.isRelativeQuantity;
}

int fmi3_xml_get_float64_type_is_unbounded(fmi3_xml_float_typedef_t* t) {
    return fmi3_xml_get_float_type_props(t)->super.isUnbounded;
}

fmi3_xml_display_unit_t* fmi3_xml_get_float64_type_display_unit(fmi3_xml_float_typedef_t* t) {
    fmi3_xml_display_unit_t* du = fmi3_xml_get_float_type_props(t)->displayUnit;
    if (!du || (&du->baseUnit->defaultDisplay == du)) {
        return NULL;
    }
    return du;
}

// -----------------------------------------------------------------------------
// Float32
// -----------------------------------------------------------------------------

fmi3_float32_t fmi3_xml_get_float32_type_min(fmi3_xml_float_typedef_t* t) {
    return fmi3_xml_get_float_type_props(t)->typeMin.scalar32s;
}

fmi3_float32_t fmi3_xml_get_float32_type_max(fmi3_xml_float_typedef_t* t) {
    return fmi3_xml_get_float_type_props(t)->typeMax.scalar32s;
}

fmi3_float32_t fmi3_xml_get_float32_type_nominal(fmi3_xml_float_typedef_t* t) {
    return fmi3_xml_get_float_type_props(t)->typeNominal.scalar32s;
}

fmi3_xml_unit_t* fmi3_xml_get_float32_type_unit(fmi3_xml_float_typedef_t* t) {
    fmi3_xml_display_unit_t* du = fmi3_xml_get_float_type_props(t)->displayUnit;
    if (du) {
        return du->baseUnit;
    }
    return NULL;
}

int fmi3_xml_get_float32_type_is_relative_quantity(fmi3_xml_float_typedef_t* t) {
    return fmi3_xml_get_float_type_props(t)->super.isRelativeQuantity;
}

int fmi3_xml_get_float32_type_is_unbounded(fmi3_xml_float_typedef_t* t) {
    return fmi3_xml_get_float_type_props(t)->super.isUnbounded;
}

fmi3_xml_display_unit_t* fmi3_xml_get_float32_type_display_unit(fmi3_xml_float_typedef_t* t) {
    fmi3_xml_display_unit_t* du = fmi3_xml_get_float_type_props(t)->displayUnit;
    if (!du || (&du->baseUnit->defaultDisplay == du)) {
        return NULL;
    }
    return du;
}

// -----------------------------------------------------------------------------
// Int64
// -----------------------------------------------------------------------------

static fmi3_xml_int_type_props_t* fmi3_xml_get_int_type_props(fmi3_xml_int_typedef_t* t) {
    fmi3_xml_variable_typedef_t* vt = (void*)t;
    return (void*)(vt->super.nextLayer);
}

fmi3_int64_t fmi3_xml_get_int64_type_min(fmi3_xml_int_typedef_t* t) {
    return fmi3_xml_get_int_type_props(t)->typeMin.scalar64s;
}

fmi3_int64_t fmi3_xml_get_int64_type_max(fmi3_xml_int_typedef_t* t) {
    return fmi3_xml_get_int_type_props(t)->typeMax.scalar64s;
}

// -----------------------------------------------------------------------------
// Int32
// -----------------------------------------------------------------------------

fmi3_int32_t fmi3_xml_get_int32_type_min(fmi3_xml_int_typedef_t* t) {
    return fmi3_xml_get_int_type_props(t)->typeMin.scalar32s;
}

fmi3_int32_t fmi3_xml_get_int32_type_max(fmi3_xml_int_typedef_t* t) {
    return fmi3_xml_get_int_type_props(t)->typeMax.scalar32s;
}

// -----------------------------------------------------------------------------
// Int16
// -----------------------------------------------------------------------------

fmi3_int16_t fmi3_xml_get_int16_type_min(fmi3_xml_int_typedef_t* t) {
    return fmi3_xml_get_int_type_props(t)->typeMin.scalar16s;
}

fmi3_int16_t fmi3_xml_get_int16_type_max(fmi3_xml_int_typedef_t* t) {
    return fmi3_xml_get_int_type_props(t)->typeMax.scalar16s;
}

// -----------------------------------------------------------------------------
// Int8
// -----------------------------------------------------------------------------

fmi3_int8_t fmi3_xml_get_int8_type_min(fmi3_xml_int_typedef_t* t) {
    return fmi3_xml_get_int_type_props(t)->typeMin.scalar8s;
}

fmi3_int8_t fmi3_xml_get_int8_type_max(fmi3_xml_int_typedef_t* t) {
    return fmi3_xml_get_int_type_props(t)->typeMax.scalar8s;
}

// -----------------------------------------------------------------------------
// UInt64
// -----------------------------------------------------------------------------

fmi3_uint64_t fmi3_xml_get_uint64_type_min(fmi3_xml_int_typedef_t* t) {
    return fmi3_xml_get_int_type_props(t)->typeMin.scalar64u;
}

fmi3_uint64_t fmi3_xml_get_uint64_type_max(fmi3_xml_int_typedef_t* t) {
    return fmi3_xml_get_int_type_props(t)->typeMax.scalar64u;
}

// -----------------------------------------------------------------------------
// UInt32
// -----------------------------------------------------------------------------

fmi3_uint32_t fmi3_xml_get_uint32_type_min(fmi3_xml_int_typedef_t* t) {
    return fmi3_xml_get_int_type_props(t)->typeMin.scalar32u;
}

fmi3_uint32_t fmi3_xml_get_uint32_type_max(fmi3_xml_int_typedef_t* t) {
    return fmi3_xml_get_int_type_props(t)->typeMax.scalar32u;
}

// -----------------------------------------------------------------------------
// UInt16
// -----------------------------------------------------------------------------

fmi3_uint16_t fmi3_xml_get_uint16_type_min(fmi3_xml_int_typedef_t* t) {
    return fmi3_xml_get_int_type_props(t)->typeMin.scalar16u;
}

fmi3_uint16_t fmi3_xml_get_uint16_type_max(fmi3_xml_int_typedef_t* t) {
    return fmi3_xml_get_int_type_props(t)->typeMax.scalar16u;
}

// -----------------------------------------------------------------------------
// UInt8
// -----------------------------------------------------------------------------

fmi3_uint8_t fmi3_xml_get_uint8_type_min(fmi3_xml_int_typedef_t* t) {
    return fmi3_xml_get_int_type_props(t)->typeMin.scalar8u;
}

fmi3_uint8_t fmi3_xml_get_uint8_type_max(fmi3_xml_int_typedef_t* t) {
    return fmi3_xml_get_int_type_props(t)->typeMax.scalar8u;
}

// -----------------------------------------------------------------------------
// Enum
// -----------------------------------------------------------------------------

static int fmi3_xml_compare_enum_val(const void* first, const void* second) {
    const jm_named_ptr* a = first;
    const jm_named_ptr* b = second;
    fmi3_xml_enum_type_item_t* ai = a->ptr;
    fmi3_xml_enum_type_item_t* bi = b->ptr;
    return (ai->value - bi->value);
}

static fmi3_xml_enum_typedef_props_t* fmi3_xml_get_enum_typedef_props(fmi3_xml_enumeration_typedef_t* t) {
    fmi3_xml_variable_typedef_t* vt = (void*)t;
    return (void*)(vt->super.nextLayer);
}

int fmi3_xml_get_enum_type_min(fmi3_xml_enumeration_typedef_t* t){
    fmi3_xml_enum_typedef_props_t* props = fmi3_xml_get_enum_typedef_props(t);
    fmi3_xml_enum_type_item_t* item;
    if(jm_vector_get_size(jm_named_ptr)(&props->enumItems) == 0) return 0;
    item = jm_vector_get_item(jm_named_ptr)(&props->enumItems, 0).ptr;
    return item->value;
}

int fmi3_xml_get_enum_type_max(fmi3_xml_enumeration_typedef_t* t){
    fmi3_xml_enum_typedef_props_t* props = fmi3_xml_get_enum_typedef_props(t);
    jm_named_ptr* itemp = jm_vector_get_lastp(jm_named_ptr)(&props->enumItems);
    fmi3_xml_enum_type_item_t* item;
    if(!itemp) return 0;
    item = itemp->ptr;
    return item->value;
}

unsigned int fmi3_xml_get_enum_type_size(fmi3_xml_enumeration_typedef_t* t) {
    fmi3_xml_enum_typedef_props_t* props = fmi3_xml_get_enum_typedef_props(t);
    return (unsigned int)jm_vector_get_size(jm_named_ptr)(&props->enumItems);
}

const char* fmi3_xml_get_enum_type_item_name(fmi3_xml_enumeration_typedef_t* t, unsigned int  item) {
    fmi3_xml_enum_typedef_props_t* props = fmi3_xml_get_enum_typedef_props(t);
    if((item == 0) || (item > fmi3_xml_get_enum_type_size(t) )) return  0;
    return jm_vector_get_item(jm_named_ptr)(&props->enumItems,item-1).name;
}

int fmi3_xml_get_enum_type_item_value(fmi3_xml_enumeration_typedef_t* t, unsigned int  item) {
    fmi3_xml_enum_typedef_props_t* props = fmi3_xml_get_enum_typedef_props(t);
    fmi3_xml_enum_type_item_t* eitem;
    if((item == 0) || (item > fmi3_xml_get_enum_type_size(t) )) return  0;
    eitem = jm_vector_get_item(jm_named_ptr)(&props->enumItems,item-1).ptr;
    return eitem->value;
}

const char* fmi3_xml_get_enum_type_value_name(fmi3_xml_enumeration_typedef_t* t, int value) {
    fmi3_xml_enum_typedef_props_t* props = fmi3_xml_get_enum_typedef_props(t);
    fmi3_xml_enum_type_item_t keyitem;
    jm_named_ptr* itemp, key;
    keyitem.value = value;
    key.ptr = &keyitem;
    itemp = jm_vector_bsearch(jm_named_ptr)(&props->enumItems, &key, fmi3_xml_compare_enum_val);
    if(!itemp) return  0;
    return itemp->name;
}

const char* fmi3_xml_get_enum_type_item_description(fmi3_xml_enumeration_typedef_t* t, unsigned int  item){
    fmi3_xml_enum_typedef_props_t* props = fmi3_xml_get_enum_typedef_props(t);
    fmi3_xml_enum_type_item_t* e;
    if(item > fmi3_xml_get_enum_type_size(t) ) return  0;
    e = jm_vector_get_item(jm_named_ptr)(&props->enumItems,item-1).ptr;
    return e->itemDescription;
}

// -----------------------------------------------------------------------------
// Binary
// -----------------------------------------------------------------------------

static fmi3_xml_binary_type_props_t* fmi3_xml_get_binary_type_props(fmi3_xml_binary_typedef_t* t) {
    fmi3_xml_variable_typedef_t* vt = (void*)t;
    return (void*)(vt->super.nextLayer);
}

fmi3_string_t fmi3_xml_get_binary_type_mime_type(fmi3_xml_binary_typedef_t* t) {
    return fmi3_xml_get_binary_type_props(t)->mimeType;
}

fmi3_boolean_t fmi3_xml_get_binary_type_has_max_size(fmi3_xml_binary_typedef_t* t) {
    return fmi3_xml_get_binary_type_props(t)->hasMaxSize;
}

size_t fmi3_xml_get_binary_type_max_size(fmi3_xml_binary_typedef_t* t) {
    return fmi3_xml_get_binary_type_props(t)->maxSize;
}

// -----------------------------------------------------------------------------
// Clock
// -----------------------------------------------------------------------------

static fmi3_xml_clock_type_props_t* fmi3_xml_get_clock_type_props(fmi3_xml_clock_typedef_t* t) {
    fmi3_xml_variable_typedef_t* vt = (void*)t;
    return (void*)(vt->super.nextLayer);
}

fmi3_boolean_t fmi3_xml_get_clock_type_can_be_deactivated(fmi3_xml_clock_typedef_t* t) {
    return fmi3_xml_get_clock_type_props(t)->canBeDeactivated;
}

fmi3_boolean_t fmi3_xml_get_clock_type_has_priority(fmi3_xml_clock_typedef_t* t) {
    return fmi3_xml_get_clock_type_props(t)->hasPriority;
}

fmi3_uint32_t fmi3_xml_get_clock_type_priority(fmi3_xml_clock_typedef_t* t) {
    return fmi3_xml_get_clock_type_props(t)->priority;
}

fmi3_interval_variability_enu_t fmi3_xml_get_clock_type_interval_variability(fmi3_xml_clock_typedef_t* t) {
    return fmi3_xml_get_clock_type_props(t)->intervalVariability;
}

fmi3_boolean_t fmi3_xml_get_clock_type_has_interval_decimal(fmi3_xml_clock_typedef_t* t) {
    return fmi3_xml_get_clock_type_props(t)->hasIntervalDecimal;
}

fmi3_float32_t fmi3_xml_get_clock_type_interval_decimal(fmi3_xml_clock_typedef_t* t) {
    return fmi3_xml_get_clock_type_props(t)->intervalDecimal;
}

fmi3_float32_t fmi3_xml_get_clock_type_shift_decimal(fmi3_xml_clock_typedef_t* t) {
    return fmi3_xml_get_clock_type_props(t)->shiftDecimal;
}

fmi3_boolean_t fmi3_xml_get_clock_type_supports_fraction(fmi3_xml_clock_typedef_t* t) {
    return fmi3_xml_get_clock_type_props(t)->supportsFraction;
}

fmi3_boolean_t fmi3_xml_get_clock_type_has_resolution(fmi3_xml_clock_typedef_t* t) {
    return fmi3_xml_get_clock_type_props(t)->hasResolution;
}

fmi3_uint64_t fmi3_xml_get_clock_type_resolution(fmi3_xml_clock_typedef_t* t) {
    return fmi3_xml_get_clock_type_props(t)->resolution;
}

fmi3_boolean_t fmi3_xml_get_clock_type_has_interval_counter(fmi3_xml_clock_typedef_t* t) {
    return fmi3_xml_get_clock_type_props(t)->hasIntervalCounter;
}

fmi3_uint64_t fmi3_xml_get_clock_type_interval_counter(fmi3_xml_clock_typedef_t* t) {
    return fmi3_xml_get_clock_type_props(t)->intervalCounter;
}

fmi3_uint64_t fmi3_xml_get_clock_type_shift_counter(fmi3_xml_clock_typedef_t* t) {
    return fmi3_xml_get_clock_type_props(t)->shiftCounter;
}

// -----------------------------------------------------------------------------

void fmi3_xml_init_variable_type_base(fmi3_xml_variable_type_base_t* type, fmi3_xml_type_struct_kind_enu_t kind, fmi3_base_type_enu_t baseType) {
    type->nextLayer = 0;
    type->next = 0;
    type->structKind = kind;
    type->baseType = baseType;
    type->isRelativeQuantity = 0;
    type->isUnbounded = 0;
}

void fmi3_xml_init_float64_type_properties(fmi3_xml_float_type_props_t* type) {
    fmi3_xml_init_variable_type_base(&type->super, fmi3_xml_type_struct_enu_props, fmi3_base_type_float64);
    type->quantity = 0;
    type->typeMin.scalar64s = -DBL_MAX;
    type->typeMax.scalar64s = DBL_MAX;
    type->typeNominal.scalar64s = 1.0;
    type->displayUnit = 0;
}

void fmi3_xml_init_float32_type_properties(fmi3_xml_float_type_props_t* type) {
    fmi3_xml_init_variable_type_base(&type->super, fmi3_xml_type_struct_enu_props, fmi3_base_type_float32);
    type->quantity = 0;
    type->typeMin.scalar32s = -FLT_MAX;
    type->typeMax.scalar32s = FLT_MAX;
    type->typeNominal.scalar32s = 1.0;
    type->displayUnit = 0;
}

void fmi3_xml_init_int64_type_properties(fmi3_xml_int_type_props_t* type) {
    fmi3_xml_init_variable_type_base(&type->super, fmi3_xml_type_struct_enu_props, fmi3_base_type_int64);
    type->quantity = NULL;
    type->typeMin.scalar64s = INT64_MIN;
    type->typeMax.scalar64s = INT64_MAX;
}

void fmi3_xml_init_int32_type_properties(fmi3_xml_int_type_props_t* type) {
    fmi3_xml_init_variable_type_base(&type->super, fmi3_xml_type_struct_enu_props, fmi3_base_type_int32);
    type->quantity = NULL;
    type->typeMin.scalar32s = INT32_MIN;
    type->typeMax.scalar32s = INT32_MAX;
}

void fmi3_xml_init_int16_type_properties(fmi3_xml_int_type_props_t* type) {
    fmi3_xml_init_variable_type_base(&type->super, fmi3_xml_type_struct_enu_props, fmi3_base_type_int16);
    type->quantity = NULL;
    type->typeMin.scalar16s = INT16_MIN;
    type->typeMax.scalar16s = INT16_MAX;
}

void fmi3_xml_init_int8_type_properties(fmi3_xml_int_type_props_t* type) {
    fmi3_xml_init_variable_type_base(&type->super, fmi3_xml_type_struct_enu_props, fmi3_base_type_int8);
    type->quantity = NULL;
    type->typeMin.scalar8s = INT8_MIN;
    type->typeMax.scalar8s = INT8_MAX;
}

void fmi3_xml_init_uint64_type_properties(fmi3_xml_int_type_props_t* type) {
    fmi3_xml_init_variable_type_base(&type->super, fmi3_xml_type_struct_enu_props, fmi3_base_type_uint64);
    type->quantity = NULL;
    type->typeMin.scalar64u = UINTXX_MIN;
    type->typeMax.scalar64u = UINT64_MAX;
}

void fmi3_xml_init_uint32_type_properties(fmi3_xml_int_type_props_t* type) {
    fmi3_xml_init_variable_type_base(&type->super, fmi3_xml_type_struct_enu_props, fmi3_base_type_uint32);
    type->quantity = NULL;
    type->typeMin.scalar32u = UINTXX_MIN;
    type->typeMax.scalar32u = UINT32_MAX;
}

void fmi3_xml_init_uint16_type_properties(fmi3_xml_int_type_props_t* type) {
    fmi3_xml_init_variable_type_base(&type->super, fmi3_xml_type_struct_enu_props, fmi3_base_type_uint16);
    type->quantity = NULL;
    type->typeMin.scalar16u = UINTXX_MIN;
    type->typeMax.scalar16u = UINT16_MAX;
}

void fmi3_xml_init_uint8_type_properties(fmi3_xml_int_type_props_t* type) {
    fmi3_xml_init_variable_type_base(&type->super, fmi3_xml_type_struct_enu_props, fmi3_base_type_uint8);
    type->quantity = NULL;
    type->typeMin.scalar8u = UINTXX_MIN;
    type->typeMax.scalar8u = UINT8_MAX;
}

void fmi3_xml_init_binary_type_properties(fmi3_xml_binary_type_props_t* type) {
    fmi3_xml_init_variable_type_base(&type->super, fmi3_xml_type_struct_enu_props, fmi3_base_type_binary);
    type->mimeType = "application/octet-stream";
    type->hasMaxSize = fmi3_false;
    type->maxSize = 0;
}

void fmi3_xml_init_clock_type_properties(fmi3_xml_clock_type_props_t* type) {
    fmi3_xml_init_variable_type_base(&type->super, fmi3_xml_type_struct_enu_props, fmi3_base_type_clock);

    type->hasPriority        = false;
    type->hasIntervalDecimal = false;
    type->hasResolution      = false;
    type->hasIntervalCounter = false;

    type->canBeDeactivated      = false;
    type->supportsFraction      = false;
    type->resolution            = 0;
    type->priority              = 0;
    type->intervalVariability   = fmi3_interval_variability_unknown;
    type->intervalCounter       = 0;
    type->shiftCounter          = 0;
    type->intervalDecimal       = 0.0;
    type->shiftDecimal          = 0.0;
}

void fmi3_xml_init_enumeration_variable_properties(fmi3_xml_enum_variable_props_t* type, jm_callbacks* cb) {
    fmi3_xml_init_variable_type_base(&type->super, fmi3_xml_type_struct_enu_props, fmi3_base_type_enum);
    type->quantity = 0;
    type->typeMin = 0;
    type->typeMax = 0;
}

void fmi3_xml_init_enumeration_type_properties(fmi3_xml_enum_typedef_props_t* type, jm_callbacks* cb) {
    fmi3_xml_init_enumeration_variable_properties(&type->base, cb);
    jm_vector_init(jm_named_ptr)(&type->enumItems,0,cb);
}

void fmi3_xml_free_enumeration_type_props(fmi3_xml_enum_typedef_props_t* type) {
    jm_named_vector_free_data(&type->enumItems);
}

void fmi3_xml_init_type_definitions(fmi3_xml_type_definition_list_t* td, jm_callbacks* cb) {
    jm_vector_init(jm_named_ptr)(&td->typeDefinitions,0,cb);

    jm_vector_init(jm_string)(&td->quantities, 0, cb);
    jm_vector_init(jm_string)(&td->mimeTypes, 0, cb);

    fmi3_xml_init_float64_type_properties(&td->defaultFloat64Type);
    fmi3_xml_init_float32_type_properties(&td->defaultFloat32Type);
    fmi3_xml_init_enumeration_type_properties(&td->defaultEnumType,cb);
    fmi3_xml_init_int64_type_properties(&td->defaultInt64Type);
    fmi3_xml_init_int32_type_properties(&td->defaultInt32Type);
    fmi3_xml_init_int16_type_properties(&td->defaultInt16Type);
    fmi3_xml_init_int8_type_properties(&td->defaultInt8Type);
    fmi3_xml_init_uint64_type_properties(&td->defaultUInt64Type);
    fmi3_xml_init_uint32_type_properties(&td->defaultUInt32Type);
    fmi3_xml_init_uint16_type_properties(&td->defaultUInt16Type);
    fmi3_xml_init_uint8_type_properties(&td->defaultUInt8Type);
    fmi3_xml_init_binary_type_properties(&td->defaultBinaryType);
    fmi3_xml_init_clock_type_properties(&td->defaultClockType);

    fmi3_xml_init_variable_type_base(&td->defaultBooleanType, fmi3_xml_type_struct_enu_props, fmi3_base_type_bool);
    fmi3_xml_init_variable_type_base(&td->defaultStringType,  fmi3_xml_type_struct_enu_props, fmi3_base_type_str);

    td->typePropsList = NULL;
}

void fmi3_xml_free_type_definitions_data(fmi3_xml_type_definition_list_t* td) {
    jm_callbacks* cb = td->typeDefinitions.callbacks;

    jm_vector_foreach(jm_string)(&td->quantities,(void(*)(const char*))cb->free);
    jm_vector_free_data(jm_string)(&td->quantities);

    jm_vector_foreach(jm_string)(&td->mimeTypes,(void(*)(const char*))cb->free);
    jm_vector_free_data(jm_string)(&td->mimeTypes);

    {
        fmi3_xml_variable_type_base_t* next;
        fmi3_xml_variable_type_base_t* cur = td->typePropsList;
        while(cur) {
            next = cur->next;
            if(    (cur->baseType == fmi3_base_type_enum)
                && (cur->structKind == fmi3_xml_type_struct_enu_props)
                && (cur->nextLayer == NULL)
                ) {
                fmi3_xml_enum_typedef_props_t* props = (fmi3_xml_enum_typedef_props_t*)cur;
                fmi3_xml_free_enumeration_type_props(props);
            }
            cb->free(cur);
            cur = next;
        }
        td->typePropsList = NULL;
    }

    jm_named_vector_free_data(&td->typeDefinitions);
}

int fmi3_xml_handle_TypeDefinitions(fmi3_xml_parser_context_t *context, const char* data) {
    if(!data) {
        jm_log_verbose(context->callbacks, module, "Parsing XML element TypeDefinitions");
    }
    else {
        fmi3_xml_type_definition_list_t* defs =  &context->modelDescription->typeDefinitions;

        jm_vector_qsort(jm_named_ptr)(&defs->typeDefinitions, jm_compare_named);
        return 0;
    }
    return 0;
}

int fmi3_xml_handle_SimpleType(fmi3_xml_parser_context_t *context, const char* data) {
    if (!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_type_definition_list_t* td = &md->typeDefinitions;
        jm_named_ptr named, *pnamed;
        fmi3_xml_variable_typedef_t dummy;
        fmi3_xml_variable_typedef_t* typeDef;
        size_t nameoffset = dummy.typeName - (char*)&dummy; /* calculate offset for name ptr in jm_named_ptr */
        fmi3_base_type_enu_t placeHolderType = fmi3_base_type_float64; /* arbitrarily chosen baseType that will be overriden later */

        /* allocate buffers */
        jm_vector(char)* bufName = fmi3_xml_reserve_parse_buffer(context,1,100);
        jm_vector(char)* bufDescr = fmi3_xml_reserve_parse_buffer(context,2,100);
        if (!bufName || !bufDescr)
            return -1;

        /* read attributes to buffers */
        if (fmi3_xml_parse_attr_as_string(context, fmi3_xml_elmID_SimpleType, fmi_attr_id_name, 1, bufName) ||
            fmi3_xml_parse_attr_as_string(context, fmi3_xml_elmID_SimpleType, fmi_attr_id_description, 0, bufDescr)) {
            return -1;
        }

        /* set default pointer fields to NULL so we later can verify if allocation works */
        named.ptr = NULL;
        named.name = NULL;

        /* push to typeDefinitions */
        pnamed = jm_vector_push_back(jm_named_ptr)(&td->typeDefinitions, named);
        if (!pnamed) {
            fmi3_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }

        /* allocate typedef and write name to both the typedef struct and the jm_named_ptr (they both point to same memory) */
        *pnamed = jm_named_alloc_v(bufName, sizeof(fmi3_xml_variable_typedef_t), nameoffset, context->callbacks);
        if (!pnamed->ptr) {
            fmi3_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }

        /* initialize the type_base struct as a typedef, with a placeHolder type that will be overriden by the real primitive type */
        typeDef = pnamed->ptr;
        fmi3_xml_init_variable_type_base(&typeDef->super, fmi3_xml_type_struct_enu_typedef, placeHolderType);
        if (jm_vector_get_size(char)(bufDescr)) {
            const char* description = jm_string_set_put(&md->descriptions, jm_vector_get_itemp(char)(bufDescr, 0));
            typeDef->description = description;
        } else {
            typeDef->description = "";
        }

    } else { /* end of tag */
        fmi3_xml_variable_typedef_t* typeDef;
        typeDef = jm_vector_get_last(jm_named_ptr)(&(context->modelDescription->typeDefinitions.typeDefinitions)).ptr;
        if (typeDef->super.nextLayer == NULL) {
            fmi3_xml_parse_fatal(context, "No specific type given for type definition %s", typeDef->typeName);
            return -1;
        }
    }
    return 0;
}

/**
 * Adds a new _props object (containing either variable or type attributes) on top of the list-node
 * given in parameter 'base'.
 *
 * Generic for all struct kinds (float, int, ...), i.e. the caller needs to typecast to the
 * correct type and make sure that enough memory is given via parameter 'typeSize'.
 */
fmi3_xml_variable_type_base_t* fmi3_xml_alloc_variable_or_typedef_props(fmi3_xml_type_definition_list_t* td,
        fmi3_xml_variable_type_base_t* nextLayer, size_t typeSize)
{
    jm_callbacks* cb = td->typeDefinitions.callbacks;
    fmi3_xml_variable_type_base_t* type = cb->malloc(typeSize);
    if (!type) {
        fmi3_xml_parse_fatal(cb->context, "Could not allocate memory");
        return NULL;
    }
    fmi3_xml_init_variable_type_base(type, fmi3_xml_type_struct_enu_props, nextLayer->baseType);
    type->nextLayer = nextLayer;

    // Push to typePropsList:
    type->next = td->typePropsList;
    td->typePropsList = type;

    return type;
}

/**
 * Adds a new start object on top of the list-node given in parameter 'base'.
 *
 * Generic for all struct kinds (float, int, ...), i.e. the caller needs to typecast to the
 * correct type and make sure that enough memory is given via parameter 'typeSize'.
 *
 * @param td
 * @param nextLayer The current top of the type_base_t list. The new top will be the new start_t object.
 * @param typeSize The total size of memory for the start_t object. Typically the size of the start_t struct
 *                 plus memory for the actual start value, if the value is variable-size.
 */
fmi3_xml_variable_type_base_t* fmi3_xml_alloc_variable_type_start(fmi3_xml_type_definition_list_t* td,
        fmi3_xml_variable_type_base_t* nextLayer, size_t typeSize)
{
    jm_callbacks* cb = td->typeDefinitions.callbacks;

    fmi3_xml_variable_type_base_t* start = cb->malloc(typeSize);
    if (!start) {
        fmi3_xml_parse_fatal(cb->context, "Could not allocate memory");
        return NULL;
    }
    fmi3_xml_init_variable_type_base(start, fmi3_xml_type_struct_enu_start, nextLayer->baseType);
    start->nextLayer = nextLayer;

    // Push to typePropsList
    start->next = td->typePropsList;
    td->typePropsList = start;

    return start;
}

/**
 * Parses all the FloatXX-specific attributes.
 *
 * @param fallbackType:
 *      For Variables: The typedef_t of the declaredType if defined, else the props_t of the default type.
 *      For TypeDefinitions: The props_t of the default type.
 */
fmi3_xml_float_type_props_t* fmi3_xml_parse_float_type_properties(fmi3_xml_parser_context_t* context,
        fmi3_xml_elm_enu_t elmID, fmi3_xml_variable_type_base_t* fallbackType, const fmi3_xml_primitive_type_t* primType)
{
    jm_named_ptr named, *pnamed;
    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_float_type_props_t* fallbackProps;
    fmi3_xml_float_type_props_t* props;
    const char* quantity = NULL;
    unsigned int relQuanBuf, unboundedBuf;

    if (fallbackType->structKind == fmi3_xml_type_struct_enu_typedef) {
        fallbackProps = (void*)fallbackType->nextLayer;
    } else {
        fallbackProps = (void*)fallbackType;
    }

    jm_vector(char)* bufQuantity = fmi3_xml_reserve_parse_buffer(context, 1, 100);
    jm_vector(char)* bufUnit     = fmi3_xml_reserve_parse_buffer(context, 2, 100);
    jm_vector(char)* bufDispUnit = fmi3_xml_reserve_parse_buffer(context, 3, 100);

    props = (fmi3_xml_float_type_props_t*)fmi3_xml_alloc_variable_or_typedef_props(
            &md->typeDefinitions, fallbackType, sizeof(fmi3_xml_float_type_props_t));

    if (!bufQuantity || !bufUnit || !bufDispUnit || !props ||
            fmi3_xml_parse_attr_as_string(context, elmID, fmi_attr_id_quantity, 0, bufQuantity) ||
            fmi3_xml_parse_attr_as_string(context, elmID, fmi_attr_id_unit, 0, bufUnit) ||
            fmi3_xml_parse_attr_as_string(context, elmID, fmi_attr_id_displayUnit, 0, bufDispUnit))
    {
        fmi3_xml_parse_fatal(context, "Error parsing float type properties");
        return NULL;
    }
    if (jm_vector_get_size(char)(bufQuantity)) {
        quantity = jm_string_set_put(&md->typeDefinitions.quantities, jm_vector_get_itemp(char)(bufQuantity, 0));
    }

    props->quantity = quantity;
    props->displayUnit = 0;
    if (jm_vector_get_size(char)(bufDispUnit)) {
        named.name = jm_vector_get_itemp(char)(bufDispUnit, 0);
        pnamed = jm_vector_bsearch(jm_named_ptr)(&(md->displayUnitDefinitions), &named, jm_compare_named);
        if (!pnamed) {
            fmi3_xml_parse_fatal(context, "Unknown displayUnit: %s", jm_vector_get_itemp(char)(bufDispUnit, 0));
            return NULL;
        }
        props->displayUnit = pnamed->ptr;
    } else {
        if (jm_vector_get_size(char)(bufUnit)) {
            props->displayUnit = fmi3_xml_get_parsed_unit(context, bufUnit, 1);
        }
    }

    if (fmi3_xml_parse_attr_as_boolean(context, elmID, fmi_attr_id_relativeQuantity, 0, &relQuanBuf, fallbackProps->super.isRelativeQuantity) ||
        fmi3_xml_parse_attr_as_boolean(context, elmID, fmi_attr_id_unbounded, 0, &unboundedBuf, fallbackProps->super.isUnbounded) ||
        fmi3_xml_parse_attr_as_floatXX(context, elmID, fmi_attr_id_min, 0, &props->typeMin, &fallbackProps->typeMin, primType) ||
        fmi3_xml_parse_attr_as_floatXX(context, elmID, fmi_attr_id_max, 0, &props->typeMax, &fallbackProps->typeMax, primType) ||
        fmi3_xml_parse_attr_as_floatXX(context, elmID, fmi_attr_id_nominal, 0, &props->typeNominal, &fallbackProps->typeNominal, primType))
    {
        // NOTE: Should not need fatal error here since none of these attributes are used to
        // reference other parts of the XML.
        return NULL;
    }

    props->super.isRelativeQuantity = (relQuanBuf) ? 1 : 0;
    props->super.isUnbounded = (unboundedBuf) ? 1 : 0;
    return props;
}

int fmi3_xml_handle_FloatType(fmi3_xml_parser_context_t* context, const char* data, fmi3_xml_elm_enu_t elmID,
        fmi3_xml_float_type_props_t* defaultType, const fmi3_xml_primitive_type_t* primType)
{
    if (fmi3_xml_handle_SimpleType(context, data)) return -1;

    if (!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_variable_typedef_t* typeDef;
        fmi3_xml_float_type_props_t * props;

        props = fmi3_xml_parse_float_type_properties(context, elmID, &defaultType->super, primType);
        if (!props) return -1;

        // Get the typedef already created for SimpleType and set props
        typeDef = jm_vector_get_last(jm_named_ptr)(&md->typeDefinitions.typeDefinitions).ptr;
        typeDef->super.baseType = primType->baseType;
        typeDef->super.nextLayer = &props->super;
    }
    return 0;
}

int fmi3_xml_handle_Float64Type(fmi3_xml_parser_context_t* context, const char* data) {
    return fmi3_xml_handle_FloatType(context, data, fmi3_xml_elmID_Float64Type, &context->modelDescription->typeDefinitions.defaultFloat64Type, &PRIMITIVE_TYPES.float64);
}

int fmi3_xml_handle_Float32Type(fmi3_xml_parser_context_t* context, const char* data) {
    return fmi3_xml_handle_FloatType(context, data, fmi3_xml_elmID_Float32Type, &context->modelDescription->typeDefinitions.defaultFloat32Type, &PRIMITIVE_TYPES.float32);
}

/**
 * Parses all the IntXX-specific attributes.
 *
 * @param fallbackType:
 *      For Variables: The typedef_t of the declaredType if defined, else the props_t of the default type.
 *      For TypeDefinitions: The props_t of the default type.
 */
fmi3_xml_int_type_props_t* fmi3_xml_parse_intXX_type_properties(fmi3_xml_parser_context_t* context,
        fmi3_xml_elm_enu_t elmID, fmi3_xml_variable_type_base_t* fallbackType, const fmi3_xml_primitive_type_t* primType)
{
    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_type_definition_list_t* td = &md->typeDefinitions;
    fmi3_xml_int_type_props_t* fallbackProps = NULL;
    fmi3_xml_int_type_props_t* props = NULL;
    const char* quantity = NULL;

    // XXX: Why don't we give fatal errors in this funciton, but we do it for floatXX?

    if (fallbackType->structKind == fmi3_xml_type_struct_enu_typedef) {
        fallbackProps = (void*)fallbackType->nextLayer;
    } else {
        fallbackProps = (void*)fallbackType;
    }

    jm_vector(char)* bufQuantity = fmi3_xml_reserve_parse_buffer(context, 1, 100);
    if (!bufQuantity) return NULL;

    props = (fmi3_xml_int_type_props_t*)fmi3_xml_alloc_variable_or_typedef_props(td, fallbackType, sizeof(fmi3_xml_int_type_props_t));
    if (!props) return NULL;

    if (fmi3_xml_parse_attr_as_string(context, elmID, fmi_attr_id_quantity, 0, bufQuantity))
        return NULL;

    if (jm_vector_get_size(char)(bufQuantity))
        quantity = jm_string_set_put(&td->quantities, jm_vector_get_itemp(char)(bufQuantity, 0));

    props->quantity = quantity;

    if (    fmi3_xml_parse_attr_as_intXX(context, elmID, fmi_attr_id_min, 0, &props->typeMin, &fallbackProps->typeMin, primType) ||
            fmi3_xml_parse_attr_as_intXX(context, elmID, fmi_attr_id_max, 0, &props->typeMax, &fallbackProps->typeMax, primType))
        return NULL;

    return props;
}

int fmi3_xml_handle_IntXXType(fmi3_xml_parser_context_t *context, const char* data, fmi3_xml_elm_enu_t elemID,
        fmi3_xml_int_type_props_t* defaultType, const fmi3_xml_primitive_type_t* primType)
{
    if (fmi3_xml_handle_SimpleType(context, data)) return -1;

    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_variable_typedef_t* typeDef;
        fmi3_xml_int_type_props_t * props;

        props = fmi3_xml_parse_intXX_type_properties(context, elemID, &defaultType->super, primType);
        if (!props) return -1;

        // Get the typedef already created for SimpleType and set props
        typeDef = jm_vector_get_last(jm_named_ptr)(&md->typeDefinitions.typeDefinitions).ptr;
        typeDef->super.baseType = primType->baseType;
        typeDef->super.nextLayer = &props->super;
    }
    return 0;
}

int fmi3_xml_handle_Int64Type(fmi3_xml_parser_context_t* context, const char* data) {
    return fmi3_xml_handle_IntXXType(context, data, fmi3_xml_elmID_Int64Type, &context->modelDescription->typeDefinitions.defaultInt64Type, &PRIMITIVE_TYPES.int64);
}

int fmi3_xml_handle_Int32Type(fmi3_xml_parser_context_t* context, const char* data) {
    return fmi3_xml_handle_IntXXType(context, data, fmi3_xml_elmID_Int32Type, &context->modelDescription->typeDefinitions.defaultInt32Type, &PRIMITIVE_TYPES.int32);
}

int fmi3_xml_handle_Int16Type(fmi3_xml_parser_context_t* context, const char* data) {
    return fmi3_xml_handle_IntXXType(context, data, fmi3_xml_elmID_Int16Type, &context->modelDescription->typeDefinitions.defaultInt16Type, &PRIMITIVE_TYPES.int16);
}

int fmi3_xml_handle_Int8Type(fmi3_xml_parser_context_t* context, const char* data) {
    return fmi3_xml_handle_IntXXType(context, data, fmi3_xml_elmID_Int8Type, &context->modelDescription->typeDefinitions.defaultInt8Type, &PRIMITIVE_TYPES.int8);
}

int fmi3_xml_handle_UInt64Type(fmi3_xml_parser_context_t* context, const char* data) {
    return fmi3_xml_handle_IntXXType(context, data, fmi3_xml_elmID_UInt64Type, &context->modelDescription->typeDefinitions.defaultUInt64Type, &PRIMITIVE_TYPES.uint64);
}

int fmi3_xml_handle_UInt32Type(fmi3_xml_parser_context_t* context, const char* data) {
    return fmi3_xml_handle_IntXXType(context, data, fmi3_xml_elmID_UInt32Type, &context->modelDescription->typeDefinitions.defaultUInt32Type, &PRIMITIVE_TYPES.uint32);
}

int fmi3_xml_handle_UInt16Type(fmi3_xml_parser_context_t* context, const char* data) {
    return fmi3_xml_handle_IntXXType(context, data, fmi3_xml_elmID_UInt16Type, &context->modelDescription->typeDefinitions.defaultUInt16Type, &PRIMITIVE_TYPES.uint16);
}

int fmi3_xml_handle_UInt8Type(fmi3_xml_parser_context_t* context, const char* data) {
    return fmi3_xml_handle_IntXXType(context, data, fmi3_xml_elmID_UInt8Type, &context->modelDescription->typeDefinitions.defaultUInt8Type, &PRIMITIVE_TYPES.uint8);
}

int fmi3_xml_handle_BooleanType(fmi3_xml_parser_context_t* context, const char* data) {
    if (fmi3_xml_handle_SimpleType(context, data)) return -1;

    if (!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_variable_typedef_t* typeDef;

        // Get the typedef already created for SimpleType and set props
        typeDef = jm_vector_get_last(jm_named_ptr)(&context->modelDescription->typeDefinitions.typeDefinitions).ptr;
        typeDef->super.baseType = fmi3_base_type_bool;
        typeDef->super.nextLayer = &md->typeDefinitions.defaultBooleanType;
    }
    return 0;
}

/**
 * Parses all the Binary-specific attributes.
 *
 * @param fallbackType:
 *      For Variables: The typedef_t of the declaredType if defined, else the props_t of the default type.
 *      For TypeDefinitions: The props_t of the default type.
 */
fmi3_xml_binary_type_props_t* fmi3_xml_parse_binary_type_properties(fmi3_xml_parser_context_t* context,
        fmi3_xml_elm_enu_t elmID, fmi3_xml_variable_type_base_t* fallbackType)
{
    fmi3_xml_type_definition_list_t* td = &context->modelDescription->typeDefinitions;
    fmi3_xml_binary_type_props_t* fallbackProps;
    fmi3_xml_binary_type_props_t* props;

    if (fallbackType->structKind == fmi3_xml_type_struct_enu_typedef) {
        fallbackProps = (void*)fallbackType->nextLayer;
    } else {
        fallbackProps = (void*)fallbackType;
    }

    // Create props:
    props = (fmi3_xml_binary_type_props_t*)fmi3_xml_alloc_variable_or_typedef_props(td, fallbackType,
            sizeof(fmi3_xml_binary_type_props_t));
    if (!props) {return NULL;}

    // maxSize:
    // without default values, needs peeking to correctly set hasAttr flag
    if (fmi3_xml_peek_attr_str(context, fmi_attr_id_maxSize)) { // attribute exists
        props->hasMaxSize = fmi3_true;
        if (fmi3_xml_parse_attr_as_sizet(context, elmID, fmi_attr_id_maxSize, 0 /*required*/,
                &props->maxSize, &fallbackProps->maxSize)) {
            // BinaryType: fallbackProps = default; Binary: fallbackProps = declaredType props(or default)
            props->hasMaxSize = fallbackProps->hasMaxSize;
            props->maxSize = fallbackProps->maxSize;
        }
    } else { 
        // attribute does not exist, use default or declaredType as fallback
        props->hasMaxSize = fallbackProps->hasMaxSize;
        props->maxSize = fallbackProps->maxSize;
    }

    // mimeType:
    if (fmi3_xml_is_attr_defined(context, fmi_attr_id_mimeType)) {
        jm_vector(char)* mimeType = fmi3_xml_reserve_parse_buffer(context, 1, 100);
        if (!mimeType) {return NULL;} // buffer allocation failure
        if (fmi3_xml_parse_attr_as_string(context, elmID, fmi_attr_id_mimeType, 0, mimeType)) {
            props->mimeType = fallbackProps->mimeType; // parsing failure
        } else { // successful parse
            if (jm_vector_get_size(char)(mimeType) == 0) {
                // XXX: Doesn't seem like we properly store the empty string? Will trigger assertion
                // failures later, and 'quantity' is handled the same way.
                props->mimeType = NULL;
            } else {
                props->mimeType = jm_string_set_put(&td->mimeTypes, jm_vector_get_itemp(char)(mimeType, 0));
            }
        }
    } else {
        props->mimeType = fallbackProps->mimeType;
    }
    return props;
}

int fmi3_xml_handle_BinaryType(fmi3_xml_parser_context_t* context, const char* data) {
    if (fmi3_xml_handle_SimpleType(context, data)) return -1;

    if (!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_variable_typedef_t* typeDef;
        fmi3_xml_binary_type_props_t* props;
        fmi3_xml_binary_type_props_t* defaultType = &context->modelDescription->typeDefinitions.defaultBinaryType;

        props = fmi3_xml_parse_binary_type_properties(context, fmi3_xml_elmID_BinaryType, &defaultType->super);
        if (!props) return -1;

        // Get the typedef already created for SimpleType and set props
        typeDef = jm_vector_get_last(jm_named_ptr)(&md->typeDefinitions.typeDefinitions).ptr;
        typeDef->super.baseType = fmi3_base_type_binary;
        typeDef->super.nextLayer = &props->super;
    }
    return 0;
}

/**
 * Parses all the Clock-specific attributes.
 *
 * @param fallbackType:
 *      For Variables: The typedef_t of the declaredType if defined, else the props_t of the default type.
 *      For TypeDefinitions: The props_t of the default type.
 */
fmi3_xml_clock_type_props_t* fmi3_xml_parse_clock_type_properties(fmi3_xml_parser_context_t* context,
        fmi3_xml_elm_enu_t elmID, fmi3_xml_variable_type_base_t* fallbackType)
{
    fmi3_xml_type_definition_list_t* td = &context->modelDescription->typeDefinitions;
    fmi3_xml_clock_type_props_t* fallbackProps;
    fmi3_xml_clock_type_props_t* props;

    if (fallbackType->structKind == fmi3_xml_type_struct_enu_typedef) {
        fallbackProps = (void*)fallbackType->nextLayer;
    } else {
        fallbackProps = (void*)fallbackType;
    }

    // Create properties, Note: This sets defaults from fmi3_xml_init_clock_type_properties
    props = (void*)fmi3_xml_alloc_variable_or_typedef_props(td, fallbackType, sizeof(fmi3_xml_clock_type_props_t));
    if (!props) {return NULL;}

    jm_name_ID_map_t intervalVariabilityMap[] = {
            {"constant",   fmi3_interval_variability_constant},
            {"fixed",      fmi3_interval_variability_fixed},
            {"tunable",    fmi3_interval_variability_tunable},
            {"changing",   fmi3_interval_variability_changing},
            {"countdown",  fmi3_interval_variability_countdown},
            {"triggered",  fmi3_interval_variability_triggered},
            {NULL,         0}
    };
    // NOTE: The parsing of intervalVariability could maybe be relaxed to not be 'required'
    // for Variables if they have a non-default TypeDefinition, since then they could inherit
    // that attribute. However, the schema files don't allow it.
    int ret = fmi3_xml_parse_attr_as_enum(context, elmID, fmi_attr_id_intervalVariability, 1 /*required*/,
            &props->intervalVariability, fallbackProps->intervalVariability, intervalVariabilityMap);
    if (ret) {
        // Note: Even though this attribute is required, this does not break the API (by design)
        // accept it anyways
        props->intervalVariability = fmi3_interval_variability_unknown;
    }
    
    // The following attributes are optional, failure to parse does not stop parsing of current element
    // with default values
    // XXX: fmi3_xml_parse_as_* only considers default if parsing was successful
    if (fmi3_xml_parse_attr_as_bool(context, elmID, fmi_attr_id_canBeDeactivated, 0 /*required*/,
            &props->canBeDeactivated, fallbackProps->canBeDeactivated)) {
        props->canBeDeactivated = fallbackProps->canBeDeactivated;
    }
    if (fmi3_xml_parse_attr_as_float32(context, elmID, fmi_attr_id_shiftDecimal, 0 /*required*/,
            &props->shiftDecimal, fallbackProps->shiftDecimal)) {
        props->shiftDecimal = fallbackProps->shiftDecimal;
    }
    if (fmi3_xml_parse_attr_as_bool(context, elmID, fmi_attr_id_supportsFraction, 0 /*required*/,
            &props->supportsFraction, fallbackProps->supportsFraction)) {
        props->supportsFraction = fallbackProps->supportsFraction;
    }
    if (fmi3_xml_parse_attr_as_uint64(context, elmID, fmi_attr_id_shiftCounter, 0 /*required*/,
            &props->shiftCounter, fallbackProps->shiftCounter)) {
        props->shiftCounter = fallbackProps->shiftCounter;
    }

    // without default values, needs peeking to correctly set hasAttr flag
    // priority
    if (fmi3_xml_peek_attr_str(context, fmi_attr_id_priority)) { // attribute exists
        props->hasPriority = fmi3_true;
        if (fmi3_xml_parse_attr_as_uint32(context, elmID, fmi_attr_id_priority, 0 /*required*/,
                &props->priority, fallbackProps->priority)) {
            // ClockType: fallbackProps = default; Clock: fallbackProps = declaredType props(or default)
            props->hasPriority = fallbackProps->hasPriority;
            props->priority = fallbackProps->priority;
        }
    } else { 
        // attribute does not exist, use default or declaredType as fallback
        props->hasPriority = fallbackProps->hasPriority;
        props->priority = fallbackProps->priority;
    }

    // resolution
    if (fmi3_xml_peek_attr_str(context, fmi_attr_id_resolution)) { // attribute exists
        props->hasResolution = fmi3_true;
        if (fmi3_xml_parse_attr_as_uint64(context, elmID, fmi_attr_id_resolution, 0 /*required*/,
                &props->resolution, fallbackProps->resolution)) {
            // ClockType: fallbackProps = default; Clock: fallbackProps = declaredType props(or default)
            props->hasResolution = fallbackProps->hasResolution;
            props->resolution = fallbackProps->resolution;
        }
    } else { 
        // attribute does not exist, use default or declaredType as fallback
        props->hasResolution = fallbackProps->hasResolution;
        props->resolution = fallbackProps->resolution;
    }

    // intervalCounter
    if (fmi3_xml_peek_attr_str(context, fmi_attr_id_intervalCounter)) { // attribute exists
        props->hasIntervalCounter = fmi3_true;
        if (fmi3_xml_parse_attr_as_uint64(context, elmID, fmi_attr_id_intervalCounter, 0 /*required*/,
                &props->intervalCounter, fallbackProps->intervalCounter)) {
            // ClockType: fallbackProps = default; Clock: fallbackProps = declaredType props(or default)
            props->hasIntervalCounter = fallbackProps->hasIntervalCounter;
            props->intervalCounter = fallbackProps->intervalCounter;
        }
    } else { 
        // attribute does not exist, use default or declaredType as fallback
        props->hasIntervalCounter = fallbackProps->hasIntervalCounter;
        props->intervalCounter = fallbackProps->intervalCounter;
    }

    // hasIntervalDecimal
    if (fmi3_xml_peek_attr_str(context, fmi_attr_id_intervalDecimal)) { // attribute exists
        props->hasIntervalDecimal = fmi3_true;
        if (fmi3_xml_parse_attr_as_float32(context, elmID, fmi_attr_id_intervalDecimal, 0 /*required*/,
                &props->intervalDecimal, fallbackProps->intervalDecimal)) {
            // ClockType: fallbackProps = default; Clock: fallbackProps = declaredType props(or default)
            props->hasIntervalDecimal = fallbackProps->hasIntervalDecimal;
            props->intervalDecimal = fallbackProps->intervalDecimal;
        }
    } else { 
        // attribute does not exist, use default or declaredType as fallback
        props->hasIntervalDecimal = fallbackProps->hasIntervalDecimal;
        props->intervalDecimal = fallbackProps->intervalDecimal;
    }

    return props; 
}

int fmi3_xml_handle_ClockType(fmi3_xml_parser_context_t* context, const char* data) {
    if (fmi3_xml_handle_SimpleType(context, data)) return -1;

    if (!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_variable_typedef_t* typeDef;
        fmi3_xml_clock_type_props_t* props;
        fmi3_xml_clock_type_props_t* defaultType = &context->modelDescription->typeDefinitions.defaultClockType;

        props = fmi3_xml_parse_clock_type_properties(context, fmi3_xml_elmID_ClockType, &defaultType->super);
        if (!props) return -1;

        // Get the typedef already created for SimpleType and set props
        typeDef = jm_vector_get_last(jm_named_ptr)(&md->typeDefinitions.typeDefinitions).ptr;
        typeDef->super.baseType = fmi3_base_type_clock;
        typeDef->super.nextLayer = &props->super;
    }
    return 0;
}

int fmi3_xml_handle_StringType(fmi3_xml_parser_context_t* context, const char* data) {
    if (fmi3_xml_handle_SimpleType(context, data)) return -1;

    if (!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_variable_typedef_t* typeDef;

        // Get the typedef already created for SimpleType and set props
        typeDef = jm_vector_get_last(jm_named_ptr)(&context->modelDescription->typeDefinitions.typeDefinitions).ptr;
        typeDef->super.baseType = fmi3_base_type_str;
        typeDef->super.nextLayer = &md->typeDefinitions.defaultStringType;
    }
    return 0;
}

int fmi3_xml_handle_EnumerationType(fmi3_xml_parser_context_t* context, const char* data) {
    if (fmi3_xml_handle_SimpleType(context, data)) return -1;

    if (!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_enum_typedef_props_t * props;
        fmi3_xml_variable_typedef_t* typeDef;
        const char * quantity = 0;
        jm_vector(char)* bufQuantity = fmi3_xml_reserve_parse_buffer(context,3,100);

        props = (fmi3_xml_enum_typedef_props_t*)fmi3_xml_alloc_variable_or_typedef_props(
                                                    &md->typeDefinitions,
                                                    &md->typeDefinitions.defaultEnumType.base.super,
                                                    sizeof(fmi3_xml_enum_typedef_props_t));

        if (props) {
            /* the init call will override the 'next' field which has been set when the properties were allocated,
               so we need to restore it after initializing */
            fmi3_xml_variable_type_base_t* nextTmp = props->base.super.next;
            fmi3_xml_init_enumeration_type_properties(props, context->callbacks);
            props->base.super.next = nextTmp;
        }
        if (!bufQuantity || !props ||
                fmi3_xml_parse_attr_as_string(context, fmi3_xml_elmID_Int32Type, fmi_attr_id_quantity, 0, bufQuantity)
                )
            return -1;
        if (jm_vector_get_size(char)(bufQuantity))
            quantity = jm_string_set_put(&md->typeDefinitions.quantities, jm_vector_get_itemp(char)(bufQuantity, 0));

        props->base.quantity = quantity;

        // Get the typedef already created for SimpleType and set props
        typeDef = jm_vector_get_last(jm_named_ptr)(&context->modelDescription->typeDefinitions.typeDefinitions).ptr;
        typeDef->super.baseType = fmi3_base_type_enum;
        typeDef->super.nextLayer = &props->base.super;
    } else {
        fmi3_xml_enum_type_item_t *min, *max;
        fmi3_xml_variable_typedef_t* typeDef;
        fmi3_xml_enum_typedef_props_t* props;
        jm_vector(jm_named_ptr)* items;

        /* sort enum items, check that there are no duplicates */
        typeDef = jm_vector_get_last(jm_named_ptr)(&context->modelDescription->typeDefinitions.typeDefinitions).ptr;
        props = (fmi3_xml_enum_typedef_props_t*)typeDef->super.nextLayer;
        items = &props->enumItems;
        size_t i, n = jm_vector_get_size(jm_named_ptr)(items);
        jm_vector_qsort(jm_named_ptr)(items, fmi3_xml_compare_enum_val);
        for (i = 1; i < n; i++) {
            fmi3_xml_enum_type_item_t* a = jm_vector_get_itemp(jm_named_ptr)(items, i-1)->ptr;
            fmi3_xml_enum_type_item_t* b = jm_vector_get_itemp(jm_named_ptr)(items, i)->ptr;
            if (a->value == b->value) {
                jm_log_error(context->callbacks, module, "Enum items '%s' and '%s' within enumeration '%s' have the same value %d",
                    a->itemName, b->itemName, typeDef->typeName, a->value);
            }
        }

        /* Set type min/max to smallest/largest enum value. */
        min = jm_vector_get_itemp(jm_named_ptr)(items, 0)->ptr;
        max = jm_vector_get_lastp(jm_named_ptr)(items)->ptr;
        props->base.typeMin = min->value;
        props->base.typeMax = max->value;
    }
    return 0;
}

int fmi3_xml_handle_Item(fmi3_xml_parser_context_t* context, const char* data) {
    if (!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        jm_vector(char)* bufName = fmi3_xml_reserve_parse_buffer(context,1,100);
        jm_vector(char)* bufDescr = fmi3_xml_reserve_parse_buffer(context,2,100);
        /* this enum item belongs to the last created enum = head of typePropsList */
        fmi3_xml_enum_typedef_props_t * enumProps = (fmi3_xml_enum_typedef_props_t*)md->typeDefinitions.typePropsList;
        fmi3_xml_enum_type_item_t * item;
        jm_named_ptr named, *pnamed;
        size_t descrlen;
        fmi3_int32_t value;

        assert((enumProps->base.super.structKind == fmi3_xml_type_struct_enu_props)
            && (enumProps->base.super.baseType == fmi3_base_type_enum));

        if (!bufName || !bufDescr ||
                fmi3_xml_parse_attr_as_string(context, fmi3_xml_elmID_Item, fmi_attr_id_name, 1, bufName) ||
                fmi3_xml_parse_attr_as_string(context, fmi3_xml_elmID_Item, fmi_attr_id_description, 0, bufDescr) ||
                fmi3_xml_parse_attr_as_int32( context, fmi3_xml_elmID_Item, fmi_attr_id_value, 1, &value, 0)) {
            return -1;
        }
        descrlen = jm_vector_get_size(char)(bufDescr);
        named.ptr = 0;
        named.name = 0;
        pnamed = jm_vector_push_back(jm_named_ptr)(&enumProps->enumItems, named);

        if (pnamed) *pnamed = named = jm_named_alloc_v(bufName,sizeof(fmi3_xml_enum_type_item_t)+descrlen+1,sizeof(fmi3_xml_enum_type_item_t)+descrlen,context->callbacks);
        item = named.ptr;
        if (!pnamed || !item) {
            fmi3_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }
        item->itemName = named.name;
        item->value = value;
        if (descrlen)
            memcpy(item->itemDescription,jm_vector_get_itemp(char)(bufDescr,0), descrlen);
        item->itemDescription[descrlen] = 0;
    }
    return 0;
}

/**
 * Parses the declaredType attribute for a variable. Returns a _typedef if a declaredType was specified
 * and could be found, otherwise the _props for the default type.
 */
fmi3_xml_variable_type_base_t* fmi3_parse_declared_type_attr(fmi3_xml_parser_context_t* context,
        fmi3_xml_elm_enu_t elmID, fmi3_xml_variable_type_base_t* defaultType)
{
    jm_named_ptr key, *found;
    jm_vector(char)* bufDeclaredType = fmi3_xml_reserve_parse_buffer(context, 1, 100);

    fmi3_xml_parse_attr_as_string(context, elmID, fmi_attr_id_declaredType, 0, bufDeclaredType);
    if ( !jm_vector_get_size(char)(bufDeclaredType) )
        return defaultType;

    key.name = jm_vector_get_itemp(char)(bufDeclaredType, 0);
    found = jm_vector_bsearch(jm_named_ptr)(&(context->modelDescription->typeDefinitions.typeDefinitions), &key, jm_compare_named);
    if (!found) {
        jm_log_error(context->callbacks, module, "Declared type %s not found in type definitions. Ignoring.", key.name);
        return defaultType;
    } else {
        fmi3_xml_variable_type_base_t* retType = found->ptr;
        if (retType->baseType != defaultType->baseType) {
            jm_log_error(context->callbacks, module, "Declared type %s does not match variable type. Ignoring.", key.name);
            return defaultType;
        }
        return retType;
    }
}

#undef UINTXX_MIN
