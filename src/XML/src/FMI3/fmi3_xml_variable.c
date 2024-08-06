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

#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#include <stdbool.h>
#include <JM/jm_vector.h>

#include "fmi3_xml_parser.h"
#include "fmi3_xml_type_impl.h"
#include "fmi3_xml_model_description_impl.h"

#include "fmi3_xml_variable_impl.h"

/* For checking variable naming conventions */
#include <fmi3_xml_variable_name_parser.tab.h>
#define YYSTYPE YYFMI3STYPE
#include <fmi3_xml_variable_name_lex.h>

static const char* module = "FMI3XML";
struct fmi3_xml_variable_default_values {
    fmi3_float32_t float32;
    fmi3_float64_t float64;
    fmi3_uint8_t uint8;
    fmi3_uint16_t uint16;
    fmi3_uint32_t uint32;
    fmi3_uint64_t uint64;
    fmi3_int8_t int8;
    fmi3_int16_t int16;
    fmi3_int32_t int32;
    fmi3_int64_t int64;
    fmi3_boolean_t boolean;
    fmi3_string_t string;
    fmi3_string_t enumeration;
};

static struct fmi3_xml_variable_default_values VARIABLE_DEFAULT_VALUES =
       { 0.0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, false, "", 0 };


/* Define fmi3_value_reference_t vector functions */
#define JM_TEMPLATE_INSTANCE_TYPE fmi3_value_reference_t
#include "JM/jm_vector_template.h"
#undef JM_TEMPLATE_INSTANCE_TYPE

static void* fmi3_xml_get_type_default_value(fmi3_base_type_enu_t baseType) {
    switch (baseType) {
        case fmi3_base_type_float64:
            return &VARIABLE_DEFAULT_VALUES.float64;
        case fmi3_base_type_float32:
            return &VARIABLE_DEFAULT_VALUES.float32;
        case fmi3_base_type_int64:
            return &VARIABLE_DEFAULT_VALUES.int64;
        case fmi3_base_type_int32:
            return &VARIABLE_DEFAULT_VALUES.int32;
        case fmi3_base_type_int16:
            return &VARIABLE_DEFAULT_VALUES.int16;
        case fmi3_base_type_int8:
            return &VARIABLE_DEFAULT_VALUES.int8;
        case fmi3_base_type_uint64:
            return &VARIABLE_DEFAULT_VALUES.uint64;
        case fmi3_base_type_uint32:
            return &VARIABLE_DEFAULT_VALUES.uint32;
        case fmi3_base_type_uint16:
            return &VARIABLE_DEFAULT_VALUES.uint16;
        case fmi3_base_type_uint8:
            return &VARIABLE_DEFAULT_VALUES.uint8;
        case fmi3_base_type_bool:
            return &VARIABLE_DEFAULT_VALUES.boolean;
        case fmi3_base_type_str:
            return &VARIABLE_DEFAULT_VALUES.string;
        case fmi3_base_type_enum:
            return &VARIABLE_DEFAULT_VALUES.enumeration;
        default:
            assert(0);
            return NULL;
    }
}

static fmi3_int_union_t fmi3_xml_get_int_variable_start(fmi3_xml_int_variable_t* v){
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    if (fmi3_xml_get_variable_has_start(vv)) {
        fmi3_xml_int_variable_start_t* start = (fmi3_xml_int_variable_start_t*)(vv->type);
        return start->start;
    }
    fmi3_int_union_t def = { .array64s = 0 };
    return def;
}

const char* fmi3_xml_get_variable_name(fmi3_xml_variable_t* v) {
    return v->name;
}

const char* fmi3_xml_get_variable_description(fmi3_xml_variable_t* v) {
    return v->description;
}

size_t fmi3_xml_get_variable_original_order(fmi3_xml_variable_t* v) {
    return v->originalIndex;
}

fmi3_value_reference_t fmi3_xml_get_variable_vr(fmi3_xml_variable_t* v) {
    return v->vr;
}

fmi3_xml_variable_typedef_t* fmi3_xml_get_variable_declared_type(fmi3_xml_variable_t* v) {
    return (fmi3_xml_variable_typedef_t*)(fmi3_xml_find_type_struct(v->type, fmi3_xml_type_struct_enu_typedef));
}

fmi3_base_type_enu_t fmi3_xml_get_variable_base_type(fmi3_xml_variable_t* v) {
    fmi3_xml_variable_type_base_t* type = v->type;
    return type->baseType;
}

int fmi3_xml_variable_is_array(fmi3_xml_variable_t* v) {
    return v->dimensions != NULL && jm_vector_get_size(jm_voidp)(v->dimensions) > 0;
}

int fmi3_xml_get_variable_has_start(fmi3_xml_variable_t* v) {
    if (v->type) { // Can be null if parse error 
        return (v->type->structKind == fmi3_xml_type_struct_enu_start);
    } else {
        return 0;
    }
}

fmi3_variability_enu_t fmi3_xml_get_variable_variability(fmi3_xml_variable_t* v) {
    return (fmi3_variability_enu_t)v->variability;
}

fmi3_causality_enu_t fmi3_xml_get_variable_causality(fmi3_xml_variable_t* v) {
    return (fmi3_causality_enu_t)v->causality;
}

fmi3_initial_enu_t fmi3_xml_get_variable_initial(fmi3_xml_variable_t* v) {
    return (fmi3_initial_enu_t)v->initial;
}

fmi3_xml_variable_t* fmi3_xml_get_variable_previous(fmi3_xml_variable_t* v) {
    return v->hasPrevious ? v->previous.variable : NULL;
}

fmi3_boolean_t fmi3_xml_get_variable_can_handle_multiple_set_per_time_instant(fmi3_xml_variable_t* v) {
    return (fmi3_boolean_t)v->canHandleMultipleSetPerTimeInstant;
}

fmi3_boolean_t fmi3_xml_get_variable_intermediate_update(fmi3_xml_variable_t* v) {
    return (fmi3_boolean_t)v->intermediateUpdate;
}

int fmi3_xml_variable_is_clocked(fmi3_xml_variable_t* v) {
    /* A variable is considered clocked if it has the 'clocks' attribute */
    return v->clocks != NULL;
}

jm_status_enu_t fmi3_xml_get_variable_clocks(fmi3_xml_model_description_t* md, fmi3_xml_variable_t* v,
        jm_vector(jm_voidp)* list)
{
    fmi3_value_reference_t clockVr;
    fmi3_xml_variable_t* clockVar;

    if (!v->clocks) {
        // The variable has no clock attribute. Don't add anything to the list.
        return jm_status_success;
    }

    size_t nClocks = jm_vector_get_size(fmi3_value_reference_t)(v->clocks);
    for (size_t i = 0; i < nClocks; i++) {
        clockVr = jm_vector_get_item(fmi3_value_reference_t)(v->clocks, i);
        clockVar = fmi3_xml_get_variable_by_vr(md, clockVr);
        if (!jm_vector_push_back(jm_voidp)(list, clockVar)) {
            jm_log_fatal(md->callbacks, module, "Could not allocate memory");
            return jm_status_error;
        }
    }
    return jm_status_success;
}

static fmi3_xml_float_type_props_t* fmi3_xml_get_type_props_float(fmi3_xml_float_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_float_type_props_t* props = (fmi3_xml_float_type_props_t*)(fmi3_xml_find_type_props(vv->type));
    assert(props);
    return props;
}

/* float: min */
static fmi3_float_union_t fmi3_xml_get_float_variable_min(fmi3_xml_float_variable_t* v) {
    return fmi3_xml_get_type_props_float(v)->typeMin;
}

/* float: max */
static fmi3_float_union_t fmi3_xml_get_float_variable_max(fmi3_xml_float_variable_t* v) {
    return fmi3_xml_get_type_props_float(v)->typeMax;
}

/* float: nominal */
static fmi3_float_union_t fmi3_xml_get_float_variable_nominal(fmi3_xml_float_variable_t* v) {
    return fmi3_xml_get_type_props_float(v)->typeNominal;
}

/* float: start */
static fmi3_float_union_t fmi3_xml_get_float_variable_start(fmi3_xml_float_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    if (fmi3_xml_get_variable_has_start(vv)) {
        fmi3_xml_float_variable_start_t* start = (fmi3_xml_float_variable_start_t*)(vv->type);
        return start->start;
    }
    fmi3_float_union_t def = { .array64s = 0 };
    return def;
}

static fmi3_xml_int_type_props_t* fmi3_xml_get_type_props_int(fmi3_xml_int_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_int_type_props_t* props = (fmi3_xml_int_type_props_t*)(fmi3_xml_find_type_props(vv->type));
    assert(props);
    return props;
}

static fmi3_int_union_t fmi3_xml_get_int_variable_min(fmi3_xml_int_variable_t* v) {
    return fmi3_xml_get_type_props_int(v)->typeMin;
}

static fmi3_int_union_t fmi3_xml_get_int_variable_max(fmi3_xml_int_variable_t* v) {
    return fmi3_xml_get_type_props_int(v)->typeMax;
}

/* private helper for macros */
static fmi3_xml_float_type_props_t* fmi3_xml_find_type_struct_float_props(fmi3_xml_variable_t* v) {
    return (fmi3_xml_float_type_props_t*)(fmi3_xml_find_type_struct(v->type, fmi3_xml_type_struct_enu_props));
}

// -----------------------------------------------------------------------------
// Float64
// -----------------------------------------------------------------------------

fmi3_xml_float64_variable_t* fmi3_xml_get_variable_as_float64(fmi3_xml_variable_t* v) {
    if (!v) {return NULL;}
    if (fmi3_xml_get_variable_base_type(v) == fmi3_base_type_float64) {
        return (void*)v;
    }
    return NULL;
}

fmi3_xml_float64_variable_t* fmi3_xml_get_float64_variable_derivative_of(fmi3_xml_float64_variable_t* v) {
    fmi3_xml_variable_t *vv = (fmi3_xml_variable_t *)v;
    return (fmi3_xml_float64_variable_t *)vv->derivativeOf.variable;
}

fmi3_boolean_t fmi3_xml_get_float64_variable_reinit(fmi3_xml_float64_variable_t* v) {
    fmi3_xml_variable_t *vv = (fmi3_xml_variable_t *)v;
    return (fmi3_boolean_t)vv->reinit;
}

fmi3_xml_unit_t* fmi3_xml_get_float64_variable_unit(fmi3_xml_float64_variable_t* v) {
    fmi3_xml_float_type_props_t* props = fmi3_xml_find_type_struct_float_props((fmi3_xml_variable_t*)v);
    if (!props || !props->displayUnit) return 0;
    return props->displayUnit->baseUnit;
}

fmi3_xml_display_unit_t* fmi3_xml_get_float64_variable_display_unit(fmi3_xml_float64_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_float_type_props_t* props =
        (fmi3_xml_float_type_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));
    if (!props || !props->displayUnit || !props->displayUnit->displayUnit[0]) return 0;
    return props->displayUnit;
}

fmi3_string_t fmi3_xml_get_float64_variable_quantity(fmi3_xml_float64_variable_t* v) {
    fmi3_xml_float_type_props_t* props = fmi3_xml_find_type_struct_float_props((fmi3_xml_variable_t*)v);
    if (!props) return NULL;
    return (fmi3_string_t)props->quantity;
}

fmi3_boolean_t fmi3_xml_get_float64_variable_unbounded(fmi3_xml_float64_variable_t* v) {
    fmi3_xml_float_type_props_t* props = fmi3_xml_find_type_struct_float_props((fmi3_xml_variable_t*)v);
    assert(props);
    return props->super.isUnbounded;
}

fmi3_boolean_t fmi3_xml_get_float64_variable_relative_quantity(fmi3_xml_float64_variable_t* v) {
    fmi3_xml_float_type_props_t* props = fmi3_xml_find_type_struct_float_props((fmi3_xml_variable_t*)v);
    assert(props);
    return props->super.isRelativeQuantity;
}

fmi3_float64_t fmi3_xml_get_float64_variable_min(fmi3_xml_float64_variable_t* v) {
    return fmi3_xml_get_float_variable_min((fmi3_xml_float_variable_t*)v).scalar64s;
}

fmi3_float64_t fmi3_xml_get_float64_variable_max(fmi3_xml_float64_variable_t* v) {
    return fmi3_xml_get_float_variable_max((fmi3_xml_float_variable_t*)v).scalar64s;
}

fmi3_float64_t fmi3_xml_get_float64_variable_nominal(fmi3_xml_float64_variable_t* v) {
    return fmi3_xml_get_float_variable_nominal((fmi3_xml_float_variable_t*)v).scalar64s;
}

fmi3_float64_t fmi3_xml_get_float64_variable_start(fmi3_xml_float64_variable_t* v) {
    return fmi3_xml_get_float_variable_start((fmi3_xml_float_variable_t*)v).scalar64s;
}

fmi3_float64_t* fmi3_xml_get_float64_variable_start_array(fmi3_xml_float64_variable_t* v) {
    return fmi3_xml_get_float_variable_start((fmi3_xml_float_variable_t*)v).array64s;
}

// -----------------------------------------------------------------------------
// Float32
// -----------------------------------------------------------------------------

fmi3_xml_float32_variable_t* fmi3_xml_get_variable_as_float32(fmi3_xml_variable_t* v) {
    if (!v) {return NULL;}
    if (fmi3_xml_get_variable_base_type(v) == fmi3_base_type_float32) {
        return (void*)v;
    }
    return NULL;
}

fmi3_xml_float32_variable_t* fmi3_xml_get_float32_variable_derivative_of(fmi3_xml_float32_variable_t* v) {
    fmi3_xml_variable_t *vv = (fmi3_xml_variable_t *)v;
    return (fmi3_xml_float32_variable_t *)vv->derivativeOf.variable;
}

fmi3_boolean_t fmi3_xml_get_float32_variable_reinit(fmi3_xml_float32_variable_t* v) {
    fmi3_xml_variable_t *vv = (fmi3_xml_variable_t *)v;
    return (fmi3_boolean_t)vv->reinit;
}

fmi3_xml_unit_t* fmi3_xml_get_float32_variable_unit(fmi3_xml_float32_variable_t* v) {
    fmi3_xml_float_type_props_t* props = fmi3_xml_find_type_struct_float_props((fmi3_xml_variable_t*)v);
    if (!props || !props->displayUnit) return 0;
    return props->displayUnit->baseUnit;
}

fmi3_xml_display_unit_t* fmi3_xml_get_float32_variable_display_unit(fmi3_xml_float32_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_float_type_props_t* props =
        (fmi3_xml_float_type_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));
    if (!props || !props->displayUnit || !props->displayUnit->displayUnit[0]) return 0;
    return props->displayUnit;
}

fmi3_string_t fmi3_xml_get_float32_variable_quantity(fmi3_xml_float32_variable_t* v) {
    fmi3_xml_float_type_props_t* props = fmi3_xml_find_type_struct_float_props((fmi3_xml_variable_t*)v);
    if (!props) return NULL;
    return (fmi3_string_t)props->quantity;
}

fmi3_boolean_t fmi3_xml_get_float32_variable_unbounded(fmi3_xml_float32_variable_t* v) {
    fmi3_xml_float_type_props_t* props = fmi3_xml_find_type_struct_float_props((fmi3_xml_variable_t*)v);
    assert(props);
    return props->super.isUnbounded;
}

fmi3_boolean_t fmi3_xml_get_float32_variable_relative_quantity(fmi3_xml_float32_variable_t* v) {
    fmi3_xml_float_type_props_t* props = fmi3_xml_find_type_struct_float_props((fmi3_xml_variable_t*)v);
    assert(props);
    return props->super.isRelativeQuantity;
}

fmi3_float32_t fmi3_xml_get_float32_variable_min(fmi3_xml_float32_variable_t* v) {
    return fmi3_xml_get_float_variable_min((fmi3_xml_float_variable_t*)v).scalar32s;
}

fmi3_float32_t fmi3_xml_get_float32_variable_max(fmi3_xml_float32_variable_t* v) {
    return fmi3_xml_get_float_variable_max((fmi3_xml_float_variable_t*)v).scalar32s;
}

fmi3_float32_t fmi3_xml_get_float32_variable_nominal(fmi3_xml_float32_variable_t* v) {
    return fmi3_xml_get_float_variable_nominal((fmi3_xml_float_variable_t*)v).scalar32s;
}

fmi3_float32_t fmi3_xml_get_float32_variable_start(fmi3_xml_float32_variable_t* v) {
    return fmi3_xml_get_float_variable_start((fmi3_xml_float_variable_t*)v).scalar32s;
}

fmi3_float32_t* fmi3_xml_get_float32_variable_start_array(fmi3_xml_float32_variable_t* v) {
    return fmi3_xml_get_float_variable_start((fmi3_xml_float_variable_t*)v).array32s;
}

// -----------------------------------------------------------------------------
// Int64
// -----------------------------------------------------------------------------

fmi3_xml_int64_variable_t* fmi3_xml_get_variable_as_int64(fmi3_xml_variable_t* v) {
    if (fmi3_xml_get_variable_base_type(v) == fmi3_base_type_int64)
        return (void*)v;
    return 0;
}

fmi3_int64_t fmi3_xml_get_int64_variable_min(fmi3_xml_int64_variable_t* v) {
    return fmi3_xml_get_int_variable_min((fmi3_xml_int_variable_t*)v).scalar64s;
}

fmi3_int64_t fmi3_xml_get_int64_variable_max(fmi3_xml_int64_variable_t* v) {
    return fmi3_xml_get_int_variable_max((fmi3_xml_int_variable_t*)v).scalar64s;
}

fmi3_int64_t fmi3_xml_get_int64_variable_start(fmi3_xml_int64_variable_t* v) {
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v).scalar64s;
}

fmi3_int64_t* fmi3_xml_get_int64_variable_start_array(fmi3_xml_int64_variable_t* v) {
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v).array64s;
}

fmi3_string_t fmi3_xml_get_int64_variable_quantity(fmi3_xml_int64_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_int_type_props_t* props =
        (fmi3_xml_int_type_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));
    if (!props) return NULL;
    return (fmi3_string_t)props->quantity;
}

// -----------------------------------------------------------------------------
// Int32
// -----------------------------------------------------------------------------

fmi3_xml_int32_variable_t* fmi3_xml_get_variable_as_int32(fmi3_xml_variable_t* v) {
    if (fmi3_xml_get_variable_base_type(v) == fmi3_base_type_int32)
        return (void*)v;
    return 0;
}

fmi3_int32_t fmi3_xml_get_int32_variable_min(fmi3_xml_int32_variable_t* v) {
    return fmi3_xml_get_int_variable_min((fmi3_xml_int_variable_t*)v).scalar32s;
}

fmi3_int32_t fmi3_xml_get_int32_variable_max(fmi3_xml_int32_variable_t* v) {
    return fmi3_xml_get_int_variable_max((fmi3_xml_int_variable_t*)v).scalar32s;
}

fmi3_int32_t fmi3_xml_get_int32_variable_start(fmi3_xml_int32_variable_t* v) {
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v).scalar32s;
}

fmi3_int32_t* fmi3_xml_get_int32_variable_start_array(fmi3_xml_int32_variable_t* v) {
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v).array32s;
}

fmi3_string_t fmi3_xml_get_int32_variable_quantity(fmi3_xml_int32_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_int_type_props_t* props =
        (fmi3_xml_int_type_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));
    if (!props) return NULL;
    return (fmi3_string_t)props->quantity;
}

// -----------------------------------------------------------------------------
// Int16
// -----------------------------------------------------------------------------

fmi3_xml_int16_variable_t* fmi3_xml_get_variable_as_int16(fmi3_xml_variable_t* v) {
    if (fmi3_xml_get_variable_base_type(v) == fmi3_base_type_int16)
        return (void*)v;
    return 0;
}

fmi3_int16_t fmi3_xml_get_int16_variable_min(fmi3_xml_int16_variable_t* v) {
    return fmi3_xml_get_int_variable_min((fmi3_xml_int_variable_t*)v).scalar16s;
}

fmi3_int16_t fmi3_xml_get_int16_variable_max(fmi3_xml_int16_variable_t* v) {
    return fmi3_xml_get_int_variable_max((fmi3_xml_int_variable_t*)v).scalar16s;
}

fmi3_int16_t fmi3_xml_get_int16_variable_start(fmi3_xml_int16_variable_t* v) {
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v).scalar16s;
}

fmi3_int16_t* fmi3_xml_get_int16_variable_start_array(fmi3_xml_int16_variable_t* v) {
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v).array16s;
}

fmi3_string_t fmi3_xml_get_int16_variable_quantity(fmi3_xml_int16_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_int_type_props_t* props =
        (fmi3_xml_int_type_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));
    if (!props) return NULL;
    return (fmi3_string_t)props->quantity;
}

// -----------------------------------------------------------------------------
// Int8
// -----------------------------------------------------------------------------

fmi3_xml_int8_variable_t* fmi3_xml_get_variable_as_int8(fmi3_xml_variable_t* v) {
    if (fmi3_xml_get_variable_base_type(v) == fmi3_base_type_int8)
        return (void*)v;
    return 0;
}

fmi3_int8_t fmi3_xml_get_int8_variable_min(fmi3_xml_int8_variable_t* v) {
    return fmi3_xml_get_int_variable_min((fmi3_xml_int_variable_t*)v).scalar8s;
}

fmi3_int8_t fmi3_xml_get_int8_variable_max(fmi3_xml_int8_variable_t* v) {
    return fmi3_xml_get_int_variable_max((fmi3_xml_int_variable_t*)v).scalar8s;
}

fmi3_int8_t fmi3_xml_get_int8_variable_start(fmi3_xml_int8_variable_t* v) {
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v).scalar8s;
}

fmi3_int8_t* fmi3_xml_get_int8_variable_start_array(fmi3_xml_int8_variable_t* v) {
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v).array8s;
}

fmi3_string_t fmi3_xml_get_int8_variable_quantity(fmi3_xml_int8_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_int_type_props_t* props =
        (fmi3_xml_int_type_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));
    if (!props) return NULL;
    return (fmi3_string_t)props->quantity;
}

// -----------------------------------------------------------------------------
// UInt64
// -----------------------------------------------------------------------------

fmi3_xml_uint64_variable_t* fmi3_xml_get_variable_as_uint64(fmi3_xml_variable_t* v) {
    if (fmi3_xml_get_variable_base_type(v) == fmi3_base_type_uint64)
        return (void*)v;
    return 0;
}

fmi3_uint64_t fmi3_xml_get_uint64_variable_min(fmi3_xml_uint64_variable_t* v) {
    return fmi3_xml_get_int_variable_min((fmi3_xml_int_variable_t*)v).scalar64u;
}

fmi3_uint64_t fmi3_xml_get_uint64_variable_max(fmi3_xml_uint64_variable_t* v) {
    return fmi3_xml_get_int_variable_max((fmi3_xml_int_variable_t*)v).scalar64u;
}

fmi3_uint64_t fmi3_xml_get_uint64_variable_start(fmi3_xml_uint64_variable_t* v) {
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v).scalar64u;
}

fmi3_uint64_t* fmi3_xml_get_uint64_variable_start_array(fmi3_xml_uint64_variable_t* v) {
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v).array64u;
}

fmi3_string_t fmi3_xml_get_uint64_variable_quantity(fmi3_xml_uint64_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_int_type_props_t* props =
        (fmi3_xml_int_type_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));
    if (!props) return NULL;
    return (fmi3_string_t)props->quantity;
}

// -----------------------------------------------------------------------------
// UInt32
// -----------------------------------------------------------------------------

fmi3_xml_uint32_variable_t* fmi3_xml_get_variable_as_uint32(fmi3_xml_variable_t* v) {
    if (fmi3_xml_get_variable_base_type(v) == fmi3_base_type_uint32)
        return (void*)v;
    return 0;
}

fmi3_uint32_t fmi3_xml_get_uint32_variable_min(fmi3_xml_uint32_variable_t* v) {
    return fmi3_xml_get_int_variable_min((fmi3_xml_int_variable_t*)v).scalar32u;
}

fmi3_uint32_t fmi3_xml_get_uint32_variable_max(fmi3_xml_uint32_variable_t* v) {
    return fmi3_xml_get_int_variable_max((fmi3_xml_int_variable_t*)v).scalar32u;
}

fmi3_uint32_t fmi3_xml_get_uint32_variable_start(fmi3_xml_uint32_variable_t* v) {
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v).scalar32u;
}

fmi3_uint32_t* fmi3_xml_get_uint32_variable_start_array(fmi3_xml_uint32_variable_t* v) {
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v).array32u;
}

fmi3_string_t fmi3_xml_get_uint32_variable_quantity(fmi3_xml_uint32_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_int_type_props_t* props =
        (fmi3_xml_int_type_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));
    if (!props) return NULL;
    return (fmi3_string_t)props->quantity;
}

// -----------------------------------------------------------------------------
// UInt16
// -----------------------------------------------------------------------------

fmi3_xml_uint16_variable_t* fmi3_xml_get_variable_as_uint16(fmi3_xml_variable_t* v) {
    if (fmi3_xml_get_variable_base_type(v) == fmi3_base_type_uint16)
        return (void*)v;
    return 0;
}

fmi3_uint16_t fmi3_xml_get_uint16_variable_min(fmi3_xml_uint16_variable_t* v) {
    return fmi3_xml_get_int_variable_min((fmi3_xml_int_variable_t*)v).scalar16u;
}

fmi3_uint16_t fmi3_xml_get_uint16_variable_max(fmi3_xml_uint16_variable_t* v) {
    return fmi3_xml_get_int_variable_max((fmi3_xml_int_variable_t*)v).scalar16u;
}

fmi3_uint16_t fmi3_xml_get_uint16_variable_start(fmi3_xml_uint16_variable_t* v) {
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v).scalar16u;
}

fmi3_uint16_t* fmi3_xml_get_uint16_variable_start_array(fmi3_xml_uint16_variable_t* v) {
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v).array16u;
}

fmi3_string_t fmi3_xml_get_uint16_variable_quantity(fmi3_xml_uint16_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_int_type_props_t* props =
        (fmi3_xml_int_type_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));
    if (!props) return NULL;
    return (fmi3_string_t)props->quantity;
}

// -----------------------------------------------------------------------------
// UInt8
// -----------------------------------------------------------------------------

fmi3_xml_uint8_variable_t* fmi3_xml_get_variable_as_uint8(fmi3_xml_variable_t* v) {
    if (fmi3_xml_get_variable_base_type(v) == fmi3_base_type_uint8)
        return (void*)v;
    return 0;
}

fmi3_uint8_t fmi3_xml_get_uint8_variable_min(fmi3_xml_uint8_variable_t* v) {
    return fmi3_xml_get_int_variable_min((fmi3_xml_int_variable_t*)v).scalar8u;
}

fmi3_uint8_t fmi3_xml_get_uint8_variable_max(fmi3_xml_uint8_variable_t* v) {
    return fmi3_xml_get_int_variable_max((fmi3_xml_int_variable_t*)v).scalar8u;
}

fmi3_uint8_t fmi3_xml_get_uint8_variable_start(fmi3_xml_uint8_variable_t* v) {
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v).scalar8u;
}

fmi3_uint8_t* fmi3_xml_get_uint8_variable_start_array(fmi3_xml_uint8_variable_t* v) {
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v).array8u;
}

fmi3_string_t fmi3_xml_get_uint8_variable_quantity(fmi3_xml_uint8_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_int_type_props_t* props =
        (fmi3_xml_int_type_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));
    if (!props) return NULL;
    return (fmi3_string_t)props->quantity;
}

// -----------------------------------------------------------------------------
// Enum
// -----------------------------------------------------------------------------

fmi3_xml_enum_variable_t* fmi3_xml_get_variable_as_enum(fmi3_xml_variable_t* v){
    if (fmi3_xml_get_variable_base_type(v) == fmi3_base_type_enum)  return (void*)v;
    return 0;
}

fmi3_string_t fmi3_xml_get_enum_variable_quantity(fmi3_xml_enum_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_enum_variable_props_t* props = (fmi3_xml_enum_variable_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));
    if (!props) return NULL;
    return (fmi3_string_t)props->quantity;
}

int fmi3_xml_get_enum_variable_min(fmi3_xml_enum_variable_t* v){
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_variable_type_base_t* props = fmi3_xml_find_type_props(vv->type);
    return ((fmi3_xml_enum_variable_props_t*)props)->typeMin;
}

int fmi3_xml_get_enum_variable_max(fmi3_xml_enum_variable_t* v){
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_enum_variable_props_t* props =
        (fmi3_xml_enum_variable_props_t*)(fmi3_xml_find_type_props(vv->type));
    assert(props);
    return props->typeMax;
}

fmi3_int64_t fmi3_xml_get_enum_variable_start(fmi3_xml_enum_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    if (fmi3_xml_get_variable_has_start(vv)) {
        fmi3_xml_int_variable_start_t* start = (fmi3_xml_int_variable_start_t*)(vv->type);
        return start->start.scalar64s;
    }
    return 0;
}


fmi3_int64_t* fmi3_xml_get_enum_variable_start_array(fmi3_xml_enum_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    if (fmi3_xml_get_variable_has_start(vv)) {
        fmi3_xml_int_variable_start_t* start = (fmi3_xml_int_variable_start_t*)(vv->type);
        return start->start.array64s;
    }
    return 0;
}

// -----------------------------------------------------------------------------
// String
// -----------------------------------------------------------------------------

fmi3_xml_string_variable_t* fmi3_xml_get_variable_as_string(fmi3_xml_variable_t* v) {
    if (fmi3_xml_get_variable_base_type(v) == fmi3_base_type_str) return (void*)v;
    return NULL;
}

fmi3_string_t fmi3_xml_get_string_variable_start(fmi3_xml_string_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    if (fmi3_xml_get_variable_has_start(vv)) {
        fmi3_xml_string_variable_start_t* start = (fmi3_xml_string_variable_start_t*)(vv->type);
        return jm_vector_get_item(jm_voidp)(&start->stringStartValues, 0);
    }
    return NULL;
}

fmi3_string_t* fmi3_xml_get_string_variable_start_array(fmi3_xml_string_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    if (fmi3_xml_get_variable_has_start(vv)) {
        fmi3_xml_string_variable_start_t* start = (fmi3_xml_string_variable_start_t*)(vv->type);
        return (void*)jm_vector_get_itemp(jm_voidp)(&start->stringStartValues, 0);
    }
    return NULL;
}

// -----------------------------------------------------------------------------
// Boolean
// -----------------------------------------------------------------------------

fmi3_xml_bool_variable_t* fmi3_xml_get_variable_as_boolean(fmi3_xml_variable_t* v) {
    if (fmi3_xml_get_variable_base_type(v) == fmi3_base_type_bool)  return (void*)v;
    return 0;
}

fmi3_boolean_t fmi3_xml_get_boolean_variable_start(fmi3_xml_bool_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    if (fmi3_xml_get_variable_has_start(vv)) {
        fmi3_xml_int_variable_start_t* start = (fmi3_xml_int_variable_start_t*)(vv->type);
        return (fmi3_boolean_t)start->start.scalar8u;
    }
    return false;
}

fmi3_boolean_t* fmi3_xml_get_boolean_variable_start_array(fmi3_xml_bool_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    if (fmi3_xml_get_variable_has_start(vv)) {
        fmi3_xml_int_variable_start_t* start = (fmi3_xml_int_variable_start_t*)(vv->type);
        return (fmi3_boolean_t*)start->start.array8u;
    }
    return 0;
}

// -----------------------------------------------------------------------------
// Binary
// -----------------------------------------------------------------------------

fmi3_xml_binary_variable_t* fmi3_xml_get_variable_as_binary(fmi3_xml_variable_t* v) {
    if (fmi3_xml_get_variable_base_type(v) == fmi3_base_type_binary) return (void*)v;
    return 0;
}

size_t fmi3_xml_get_binary_variable_start_size(fmi3_xml_binary_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    if (fmi3_xml_get_variable_has_start(vv)) {
        fmi3_xml_binary_variable_start_t* start = (fmi3_xml_binary_variable_start_t*)(vv->type);
        return start->nStart;
    }
    return 0;
}

size_t fmi3_xml_get_binary_variable_start_array_size(fmi3_xml_binary_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    if (fmi3_xml_get_variable_has_start(vv)) {
        fmi3_xml_binary_variable_start_t* start = (fmi3_xml_binary_variable_start_t*)(vv->type);
        return start->nStart;
    }
    return 0;
}

size_t* fmi3_xml_get_binary_variable_start_array_sizes(fmi3_xml_binary_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    if (fmi3_xml_get_variable_has_start(vv)) {
        fmi3_xml_binary_variable_start_t* start = (fmi3_xml_binary_variable_start_t*)(vv->type);
        return (size_t*)jm_vector_get_itemp(size_t)(&start->binaryStartValuesSize, 0);
    }
    return NULL;
}

fmi3_binary_t* fmi3_xml_get_binary_variable_start_array(fmi3_xml_binary_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    if (fmi3_xml_get_variable_has_start(vv)) {
        fmi3_xml_binary_variable_start_t* start = (fmi3_xml_binary_variable_start_t*)(vv->type);
        return (fmi3_binary_t*)jm_vector_get_itemp(jm_voidp)(&start->binaryStartValues, 0);
    }
    return NULL;
}


fmi3_binary_t fmi3_xml_get_binary_variable_start(fmi3_xml_binary_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    if (fmi3_xml_get_variable_has_start(vv)) {
        fmi3_xml_binary_variable_start_t* start = (fmi3_xml_binary_variable_start_t*)(vv->type);
        return start->start;
    }
    return NULL;
}

fmi3_string_t fmi3_xml_get_binary_variable_mime_type(fmi3_xml_binary_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_binary_type_props_t* props = (fmi3_xml_binary_type_props_t*)fmi3_xml_find_type_props(vv->type);
    if (!props) return NULL;
    return (fmi3_string_t)props->mimeType;
}

fmi3_boolean_t fmi3_xml_get_binary_variable_has_max_size(fmi3_xml_binary_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_binary_type_props_t* props = (fmi3_xml_binary_type_props_t*)fmi3_xml_find_type_props(vv->type);
    if (!props) return 0;
    return props->hasMaxSize;
}

size_t fmi3_xml_get_binary_variable_max_size(fmi3_xml_binary_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_binary_type_props_t* props = (fmi3_xml_binary_type_props_t*)fmi3_xml_find_type_props(vv->type);
    if (!props) return 0;
    return props->maxSize;
}

// -----------------------------------------------------------------------------
// Clock
// -----------------------------------------------------------------------------

fmi3_xml_clock_variable_t* fmi3_xml_get_variable_as_clock(fmi3_xml_variable_t* v) {
    if (fmi3_xml_get_variable_base_type(v) == fmi3_base_type_clock) return (void*)v;
    return NULL;
}

fmi3_boolean_t fmi3_xml_get_clock_variable_can_be_deactivated(fmi3_xml_clock_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_clock_type_props_t* props = (fmi3_xml_clock_type_props_t*)fmi3_xml_find_type_props(vv->type);
    return props->canBeDeactivated;
}

fmi3_boolean_t fmi3_xml_get_clock_variable_has_priority(fmi3_xml_clock_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_clock_type_props_t* props = (fmi3_xml_clock_type_props_t*)fmi3_xml_find_type_props(vv->type);
    return props->hasPriority;
}

fmi3_uint32_t fmi3_xml_get_clock_variable_priority(fmi3_xml_clock_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_clock_type_props_t* props = (fmi3_xml_clock_type_props_t*)fmi3_xml_find_type_props(vv->type);
    return props->priority;
}

fmi3_interval_variability_enu_t fmi3_xml_get_clock_variable_interval_variability(fmi3_xml_clock_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_clock_type_props_t* props = (fmi3_xml_clock_type_props_t*)fmi3_xml_find_type_props(vv->type);
    return props->intervalVariability;
}

fmi3_boolean_t fmi3_xml_get_clock_variable_has_interval_decimal(fmi3_xml_clock_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_clock_type_props_t* props = (fmi3_xml_clock_type_props_t*)fmi3_xml_find_type_props(vv->type);
    return props->hasIntervalDecimal;
}

fmi3_float32_t fmi3_xml_get_clock_variable_interval_decimal(fmi3_xml_clock_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_clock_type_props_t* props = (fmi3_xml_clock_type_props_t*)fmi3_xml_find_type_props(vv->type);
    return props->intervalDecimal;
}

fmi3_float32_t fmi3_xml_get_clock_variable_shift_decimal(fmi3_xml_clock_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_clock_type_props_t* props = (fmi3_xml_clock_type_props_t*)fmi3_xml_find_type_props(vv->type);
    return props->shiftDecimal;
}

fmi3_boolean_t fmi3_xml_get_clock_variable_supports_fraction(fmi3_xml_clock_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_clock_type_props_t* props = (fmi3_xml_clock_type_props_t*)fmi3_xml_find_type_props(vv->type);
    return props->supportsFraction;
}

fmi3_boolean_t fmi3_xml_get_clock_variable_has_resolution(fmi3_xml_clock_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_clock_type_props_t* props = (fmi3_xml_clock_type_props_t*)fmi3_xml_find_type_props(vv->type);
    return props->hasResolution;
}

fmi3_uint64_t fmi3_xml_get_clock_variable_resolution(fmi3_xml_clock_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_clock_type_props_t* props = (fmi3_xml_clock_type_props_t*)fmi3_xml_find_type_props(vv->type);
    return props->resolution;
}

fmi3_boolean_t fmi3_xml_get_clock_variable_has_interval_counter(fmi3_xml_clock_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_clock_type_props_t* props = (fmi3_xml_clock_type_props_t*)fmi3_xml_find_type_props(vv->type);
    return props->hasIntervalCounter;
}

fmi3_uint64_t fmi3_xml_get_clock_variable_interval_counter(fmi3_xml_clock_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_clock_type_props_t* props = (fmi3_xml_clock_type_props_t*)fmi3_xml_find_type_props(vv->type);
    return props->intervalCounter;
}

fmi3_uint64_t fmi3_xml_get_clock_variable_shift_counter(fmi3_xml_clock_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_clock_type_props_t* props = (fmi3_xml_clock_type_props_t*)fmi3_xml_find_type_props(vv->type);
    return props->shiftCounter;
}

// -----------------------------------------------------------------------------
// Alias variable
// -----------------------------------------------------------------------------

fmi3_xml_alias_variable_list_t* fmi3_xml_get_variable_aliases(fmi3_xml_variable_t* v) {
    if (v->aliases) {
        return (void*)v->aliases;  // cast to opaque type
    }
    return NULL;
}

size_t fmi3_xml_get_alias_variables_number(fmi3_xml_alias_variable_list_t* aliases) {
    if (aliases) {
        return jm_vector_get_size(jm_voidp)(&aliases->vec);
    }
    return 0;
}

fmi3_xml_alias_variable_t* fmi3_xml_get_alias(fmi3_xml_alias_variable_list_t* aliases, size_t index) {
    if (!aliases) {
        return NULL;
    }
    return jm_vector_get_item(jm_voidp)(&aliases->vec, index);
}

const char* fmi3_xml_get_alias_variable_name(fmi3_xml_alias_variable_t* alias) {
    return alias->name;
}

const char* fmi3_xml_get_alias_variable_description(fmi3_xml_alias_variable_t* alias) {
    return alias->description;
}

fmi3_xml_display_unit_t* fmi3_xml_get_alias_variable_display_unit(fmi3_xml_alias_variable_t* alias) {
    return alias ? alias->displayUnit : NULL;
}

// -----------------------------------------------------------------------------
// Dimensions
// -----------------------------------------------------------------------------

fmi3_xml_dimension_list_t* fmi3_xml_get_variable_dimension_list(fmi3_xml_variable_t* v) {
    if (!v) {
        return NULL;
    }
    return (void*)v->dimensions;
}

size_t fmi3_xml_get_dimension_list_size(fmi3_xml_dimension_list_t* dl) {
    if (!dl) {
        return 0;
    }
    return jm_vector_get_size(jm_voidp)(&dl->vec);
}

fmi3_xml_dimension_t* fmi3_xml_get_dimension(fmi3_xml_dimension_list_t* dl, size_t idx) {
    if (!dl) {
        return NULL;
    }
    return jm_vector_get_item(jm_voidp)(&dl->vec, idx);
}

int fmi3_xml_get_dimension_has_vr(fmi3_xml_dimension_t* dim) {
    return dim->has_vr;
}

int fmi3_xml_get_dimension_has_start(fmi3_xml_dimension_t* dim) {
    return dim->has_start;
}

fmi3_uint64_t fmi3_xml_get_dimension_start(fmi3_xml_dimension_t* dim) {
    return dim->start;
}

fmi3_value_reference_t fmi3_xml_get_dimension_vr(fmi3_xml_dimension_t* dim) {
    return dim->vr;
}

// -----------------------------------------------------------------------------

/**
 * Returns the pointer to allocated start values, independent of variable type.
 */
static void* fmi3_xml_get_variable_start_array(fmi3_xml_variable_t* v) {
    assert(fmi3_xml_variable_is_array(v));
    switch (fmi3_xml_get_variable_base_type(v)) {
    case fmi3_base_type_float64: /* fallthrough */
    case fmi3_base_type_float32:
        return fmi3_xml_get_float_variable_start((fmi3_xml_float_variable_t*)v).ptr;
    case fmi3_base_type_uint64:
    case fmi3_base_type_int64:
    case fmi3_base_type_uint32:
    case fmi3_base_type_int32:
    case fmi3_base_type_uint16:
    case fmi3_base_type_int16:
    case fmi3_base_type_uint8:
    case fmi3_base_type_int8:
        return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v).ptr;
    case fmi3_base_type_bool:
        return fmi3_xml_get_boolean_variable_start_array((fmi3_xml_bool_variable_t*)v);
    case fmi3_base_type_enum:
        return fmi3_xml_get_enum_variable_start_array((fmi3_xml_enum_variable_t*)v);
    case fmi3_base_type_str:
        return fmi3_xml_get_string_variable_start_array((fmi3_xml_string_variable_t*)v);
    case fmi3_base_type_binary:
        return fmi3_xml_get_binary_variable_start_array((fmi3_xml_binary_variable_t*)v);
    default:
        assert(0); /* impl. error */
        break;
    }

    assert(0); /* impl. error */
    return NULL;
}

/**
 * Scans the first two characters in a hexstring and calculates the byte value.
 */
static int fmi3_xml_hexstring_to_byte(fmi3_xml_parser_context_t* context, const char* hexstr, uint8_t* byte) {
    // Should just use sscanf(pos, "%2hhx", byte), but MinGW-TDM GCC 5.1
    // doesn't understand the format and gives warnings. Ignoring the warning will result
    // in arrays of Binary variable causing heap corruption (maybe due to buffer overflow).

    *byte = 0;
    for (int i = 0; i < 2; i++) {
        uint8_t chVal;
        char ch = hexstr[i];
        if (ch >= '0' && ch <= '9') {
            chVal = ch - 48;
        }
        else if (ch >= 'a' && ch <= 'f') {
            chVal = ch - 97 + 10;
        }
        else if (ch >= 'A' && ch <= 'F') {
            chVal = ch - 65 + 10;
        }
        else {
            fmi3_xml_parse_error(context, "String is not hexadecimal: %s", hexstr);
            return -1;
        }
        if (i == 0) {
            *byte += chVal << 4;
        } else {
            *byte += chVal;
        }
    }
    return 0;
}
/**
 * Scans a hexstring to a bytearray.
 *
 * @param hexstr The input string to scan.
 * @param bytearr (Output arg.) A buffer which must be able to fit the scanned bytes.
*/
static int fmi3_xml_hexstring_to_bytearray(fmi3_xml_parser_context_t* context, const char* hexstr, uint8_t* bytearr) {
    size_t len = strlen(hexstr);
    if (len % 2 != 0) {
        // Note: 2 hexadecimal characters represent 1 byte, require even length inputs
        fmi3_xml_parse_error(context, "Hexadecimal string is not of even length: '%s'.", hexstr);
        return -1;
    }

    const char* pos = hexstr;
    size_t nByte = len / 2;  // rounds down odd-sized len
    for (size_t i = 0; i < nByte; i++) {
        if (fmi3_xml_hexstring_to_byte(context, pos, &bytearr[i])) {
            return -1;
        }
        pos += 2;
    }
    return 0;
}

static void free_string_start_values(jm_callbacks* callbacks, fmi3_xml_variable_t* var) {
    fmi3_xml_string_variable_start_t* start = (fmi3_xml_string_variable_start_t*)(var->type);
    jm_vector_foreach(jm_voidp)(&start->stringStartValues, callbacks->free);
    jm_vector_free_data(jm_voidp)(&start->stringStartValues);
}

static bool is_string_variable_with_start(fmi3_xml_variable_t* var) {
    if (var->type) { // Can be null if parse error
        return var->type->baseType == fmi3_base_type_str && (var->type->structKind == fmi3_xml_type_struct_enu_start);
    } else {
        return 0;
    }
}

static bool is_binary_variable_with_start(fmi3_xml_variable_t* var) {
    if (var->type) { // Can be null if parse error
        return var->type->baseType == fmi3_base_type_binary && (var->type->structKind == fmi3_xml_type_struct_enu_start);
    } else {
        return 0;
    }
}

static void fmi3_xml_free_alias(jm_callbacks* callbacks, fmi3_xml_alias_variable_t* alias) {
    alias->description = NULL;  // Memory not owned by alias
    alias->displayUnit = NULL;  // Memory not owned by alias
    callbacks->free(alias);  // Also frees alias->name
}

void fmi3_xml_free_variable(jm_callbacks* callbacks, fmi3_xml_variable_t* var) {
    /* We need special handling of Strings and Binaries because
    *  of how start values are handled differently for those types.
    */
    if (fmi3_xml_variable_is_array(var)) {
        if (is_string_variable_with_start(var)) {
            free_string_start_values(callbacks, var);
        } else if (is_binary_variable_with_start(var)) {
            fmi3_xml_binary_variable_start_t* start = (fmi3_xml_binary_variable_start_t*)(var->type);
            jm_vector_foreach(jm_voidp)(&start->binaryStartValues, callbacks->free);
            jm_vector_free_data(jm_voidp)(&start->binaryStartValues);
            jm_vector_free_data(size_t)(&start->binaryStartValuesSize);
        } else {
            callbacks->free(fmi3_xml_get_variable_start_array(var));
        }
    } else {
        /* Scalar String variables can have a start value that we need to free.*/
        if (is_string_variable_with_start(var)) {
            free_string_start_values(callbacks, var);
        }
    }

    if (var->aliases) {
        size_t nAlias = jm_vector_get_size(jm_voidp)(var->aliases);
        for (size_t i = 0; i < nAlias; i++) {
            fmi3_xml_free_alias(callbacks, (fmi3_xml_alias_variable_t*)jm_vector_get_item(jm_voidp)(var->aliases, i));
        }
        jm_vector_free(jm_voidp)(var->aliases);
        var->aliases = NULL;
    }

    if (var->dimensions) {
        jm_vector_foreach(jm_voidp)(var->dimensions, callbacks->free);
        jm_vector_free(jm_voidp)(var->dimensions);
        var->dimensions = NULL;
    }

    if (var->clocks) {
        jm_vector_free(fmi3_value_reference_t)(var->clocks);
        var->clocks = NULL;
    }
    callbacks->free(var);
}

/**
 * Processes the attributes 'causality', 'variability' and 'initial'.
 *
 * The attributes are error checked. On success, the parsed variable is updated with them.
 */
// TODO: These are deeply rooted in modelDescription territory; use modelDescription_elm_enu_t here?
static int fmi3_xml_variable_process_attr_causality_variability_initial(fmi3_xml_parser_context_t* context,
        fmi3_xml_variable_t* variable, fmi3_xml_elm_t elm_id)
{
    jm_name_ID_map_t causalityConventionMap[] = {
            {"parameter",           fmi3_causality_enu_parameter},
            {"calculatedParameter", fmi3_causality_enu_calculated_parameter},
            {"input",               fmi3_causality_enu_input},
            {"output",              fmi3_causality_enu_output},
            {"local",               fmi3_causality_enu_local},
            {"independent",         fmi3_causality_enu_independent},
            {"structuralParameter", fmi3_causality_enu_structural_parameter},
            {NULL,                  0} /* Equivalent to fmi3_causality_enu_parameter. Is this correct? */
    };
    jm_name_ID_map_t variabilityConventionMap[] = {
            {"continuous", fmi3_variability_enu_continuous},
            {"constant",   fmi3_variability_enu_constant},
            {"fixed",      fmi3_variability_enu_fixed},
            {"tunable",    fmi3_variability_enu_tunable},
            {"discrete",   fmi3_variability_enu_discrete},
            {NULL,         0}
    };
    jm_name_ID_map_t initialConventionMap[] = {
            {"approx",     fmi3_initial_enu_approx},
            {"calculated", fmi3_initial_enu_calculated},
            {"exact",      fmi3_initial_enu_exact},
            {NULL,         0}
    };

    // Using unsigned int to avoid warnings when calling fmi3_xml_parse_attr_as_enum
    unsigned int causality;
    unsigned int variability, defaultVariability;
    unsigned int initial, defaultInitial, validInitial;

    bool isFloat = ((elm_id.modelDescription == fmi3_xml_elmID_Float32) || 
                    (elm_id.modelDescription == fmi3_xml_elmID_Float64));

    // Causality:
    if (fmi3_xml_parse_attr_as_enum(context, elm_id, FMI3_ATTR(fmi_attr_id_causality), 0, &causality,
            fmi3_causality_enu_local, causalityConventionMap))
    {
        causality = fmi3_causality_enu_local;
    }
    variable->causality = causality;

    /* Specification: "The independent variable must be defined as a floating point type ..." */
    if ((!isFloat) && (causality == fmi3_causality_enu_independent)) {
        fmi3_xml_parse_warning(context, "Causality 'independent' is only allowed for float type variables.");
    }

    // Variability:
    defaultVariability = fmi3_get_default_valid_variability(causality, isFloat);
    if (fmi3_xml_parse_attr_as_enum(context, elm_id, FMI3_ATTR(fmi_attr_id_variability), 0, &variability,
            defaultVariability, variabilityConventionMap))
    {
        variability = defaultVariability;
    }
    if (!fmi3_is_valid_variability_causality(variability, causality)) {
        fmi3_variability_enu_t bad_variability = variability;
        variability = defaultVariability;
        fmi3_xml_parse_warning(context,
                "Invalid combination of variability %s and causality %s for"
                " variable '%s'. Setting variability to '%s'",
                fmi3_variability_to_string(bad_variability),
                fmi3_causality_to_string(causality),
                variable->name,
                fmi3_variability_to_string(variability));
    }
    variable->variability = variability;

    // Initial:
    defaultInitial = fmi3_get_default_initial(variability, causality);
    if (fmi3_xml_parse_attr_as_enum(context, elm_id, FMI3_ATTR(fmi_attr_id_initial), 0, &initial,
            defaultInitial,initialConventionMap))
    {
        initial = defaultInitial;
    }
    validInitial = fmi3_get_valid_initial(variability, causality, initial);
    if (validInitial != initial) {
        fmi3_xml_parse_warning(context,
                "Initial '%s' is not allowed for variability '%s' and "
                "causality '%s'. Setting initial to '%s' for variable '%s'",
                fmi3_initial_to_string(initial),
                fmi3_variability_to_string(variability),
                fmi3_causality_to_string(causality),
                fmi3_initial_to_string(validInitial),
                variable->name);
        initial = validInitial;
    }
    variable->initial = initial;

    return 0;
}

static int fmi3_xml_variable_process_attr_derivative(fmi3_xml_parser_context_t* context,
        fmi3_xml_variable_t* variable, fmi3_xml_elm_t elm_id)
{
    uint32_t derivative;
    if (!fmi3_xml_is_attr_defined(context, FMI3_ATTR(fmi_attr_id_derivative))) {
        return 0;
    }
    else if (fmi3_xml_parse_attr_as_uint32(context, elm_id, FMI3_ATTR(fmi_attr_id_derivative), 0 /* required */,
            &derivative, 0 /* defaultVal */))
    {
        return -1;
    }
    /* Store the VR since we cannot access the variable until after parsing all variables. */
    variable->derivativeOf.vr = derivative;
    variable->hasDerivativeOf = true;
    return 0;
}

static int fmi3_xml_variable_process_attr_previous(fmi3_xml_parser_context_t* context,
        fmi3_xml_variable_t* variable, fmi3_xml_elm_t elm_id)
{
    uint32_t previous;
    if (!fmi3_xml_is_attr_defined(context, FMI3_ATTR(fmi_attr_id_previous))) {
        return 0;
    }
    else if (fmi3_xml_parse_attr_as_uint32(context, elm_id, FMI3_ATTR(fmi_attr_id_previous), 0  /* required */, 
            &previous, 0 /* defaultVal */))
    {
        return -1;
    }

    /* Store the VR since we cannot access the variable until after parsing all variables. */
    variable->previous.vr = previous;
    variable->hasPrevious = true;

    if (!fmi3_xml_variable_is_clocked(variable)) {
        fmi3_xml_parse_warning(context, "Only variables with the attribute 'clocks' may have the attribute 'previous'.");
    }

    if (variable->variability != fmi3_variability_enu_discrete) {
        fmi3_xml_parse_warning(context, "Only variables with variability 'discrete' may have the attribute 'previous'.");
    }

    if (previous == fmi3_xml_get_variable_vr(variable)) {
        fmi3_xml_parse_warning(context, "A variable must not refer to itself in the attribute 'previous'.");
    }

    return 0;
}

static int fmi3_xml_variable_process_attr_multipleset(fmi3_xml_parser_context_t* context,
        fmi3_xml_variable_t* variable, fmi3_xml_elm_t elm_id)
{
    unsigned int multipleSet;
    if (fmi3_xml_parse_attr_as_boolean(context, elm_id, FMI3_ATTR(fmi_attr_id_canHandleMultipleSetPerTimeInstant),
            0 /* required */, &multipleSet, 1 /* defaultVal */)) {
        return -1;
    }
    variable->canHandleMultipleSetPerTimeInstant = (char)multipleSet;

    if (variable->causality != fmi3_causality_enu_input && !multipleSet) {
        fmi3_xml_parse_warning(context, "Only variables with causality 'input' can have canHandleMultipleSetPerTimeInstant=false");
    }
    return 0;
}

static int fmi3_xml_variable_process_attr_intermediateupdate(fmi3_xml_parser_context_t* context,
        fmi3_xml_variable_t* variable, fmi3_xml_elm_t elm_id)
{
    unsigned int intermediateUpdate;

    variable->intermediateUpdate = 0; // default, set early due to possible ignore
    // peek due to "must not have attribute" error check 
    if (!fmi3_xml_peek_attr_str(context, FMI3_ATTR(fmi_attr_id_intermediateUpdate))) {
        return 0;
    } 
    // else: attribute exists

    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_fmu_kind_enu_t fmuKind = fmi3_xml_get_fmu_kind(md);
    // Spec: "This attribute is ignored in Model Exchange and Scheduled Execution", ignored unless Co-Simulation
    // Multiple types can be defined, check for: not CS
    if (!(fmuKind & fmi3_fmu_kind_cs)) {
        jm_log_info(md->callbacks, "FMI3XML", "Attribute 'intermediateUpdate' ignored since FMU kind is not Co-Simulation.");
        return 0;
    }

    // Spec: "Variables of type Clock must not have the intermediateUpdate attribute"
    if (elm_id.modelDescription == fmi3_xml_elmID_Clock) {
        fmi3_xml_parse_warning(context, "Variables of type 'Clock' must not have the 'intermediateUpdate' attribute.");
    }

    if (fmi3_xml_parse_attr_as_boolean(context, elm_id, FMI3_ATTR(fmi_attr_id_intermediateUpdate),
            0 /* required */, &intermediateUpdate, 0 /* defaultVal */)) {
        return -1;
    }
    variable->intermediateUpdate = (char)intermediateUpdate;

    // Spec: "Variables with causality = parameter must not be marked with intermediateUpdate = true"
    if (intermediateUpdate && (fmi3_xml_get_variable_causality(variable) == fmi3_causality_enu_parameter)) {
        fmi3_xml_parse_warning(context, "Variables with causality 'parameter' must not be marked with intermediateUpdate='true'.");
    }

    return 0;
}

static int fmi3_xml_variable_process_attr_clocks(fmi3_xml_parser_context_t* context,
        fmi3_xml_variable_t* variable, fmi3_xml_elm_t elm_id)
{
    // Attribute is optional. Avoid allocating vector unless necessary.
    if (fmi3_xml_peek_attr_str(context, FMI3_ATTR(fmi_attr_id_clocks)) == NULL) {
        return 0;
    }

    variable->clocks = jm_vector_alloc(fmi3_value_reference_t)(0, 0, context->callbacks);
    if (!variable->clocks) {
        fmi3_xml_parse_fatal(context, "Could not allocate memory");
        return -1;
    }

    if (fmi3_xml_parse_attr_valueref_list(context, elm_id, FMI3_ATTR(fmi_attr_id_clocks), 0 /* required */, variable->clocks)) {
        return -1;
    }
    return 0;
}

static int fmi3_xml_variable_process_attr_reinit(fmi3_xml_parser_context_t* context,
        fmi3_xml_variable_t* variable, fmi3_xml_elm_t elm_id)
{
    unsigned int reinit;

    if (fmi3_xml_parse_attr_as_boolean(context, elm_id, FMI3_ATTR(fmi_attr_id_reinit), 0, &reinit, 0)) {
        return -1;
    }
    variable->reinit = (char)reinit;

    if (reinit && variable->variability != fmi3_variability_enu_continuous) {
        fmi3_xml_parse_warning(context, "Variable '%s', the reinit attribute may only be set on continuous-time states.", variable->name);
    }
    return 0;
}

static fmi3_xml_variable_t* fmi3_xml_alloc_variable_with_name(fmi3_xml_parser_context_t* context, const char* name) {
    // No need to +1 on size for null terminator since the 'name' field adds one byte
    fmi3_xml_variable_t* v = context->callbacks->calloc(1, sizeof(fmi3_xml_variable_t) + strlen(name));
    if (!v) {
        fmi3_xml_parse_fatal(context, "Could not allocate memory");
        return NULL;
    }
    strcpy(v->name, name);
    return v;
}

static fmi3_xml_alias_variable_t* fmi3_xml_alloc_alias_with_name(fmi3_xml_parser_context_t* context, const char* name) {
    // No need to +1 on size for null terminator since the 'name' field adds one byte
    fmi3_xml_alias_variable_t* alias = context->callbacks->calloc(1, sizeof(fmi3_xml_alias_variable_t) + strlen(name));
    if (!alias) {
        fmi3_xml_parse_fatal(context, "Could not allocate memory");
        return NULL;
    }
    strcpy(alias->name, name);
    return alias;
}

/**
 * Core functionality for parsing common attributes
 */
static int fmi3_xml_handle_Variable_unchecked(fmi3_xml_parser_context_t* context, const char* data) {
    int atStartTag = !data;

     /* The real ID of the variable, such as 'Float64Variable' */
    fmi3_xml_elm_t elm_id = atStartTag ? context->currentElemIdStartTag : context->currentElmID;
    fmi3_xml_model_description_t* md = context->modelDescription;

    if (!data) {
        fmi3_xml_variable_t* variable;
        const char* description = NULL;
        jm_vector(char)* bufName = fmi3_xml_reserve_parse_buffer(context, 1, 100);
        jm_vector(char)* bufDesc = fmi3_xml_reserve_parse_buffer(context, 2, 100);
        unsigned int vr;

        if (!bufName || !bufDesc) {return -1;} // failure is allocate buffers

        /* Get vr, name and description */

        /* Get required attributes: VR, name */
        int req = 0;
        // |= rather than instant returns assure all attributes are parsed.
        req |= fmi3_xml_parse_attr_as_uint32(context, elm_id, FMI3_ATTR(fmi_attr_id_valueReference), 1, &vr, 0);
        req |= fmi3_xml_parse_attr_as_string(context, elm_id, FMI3_ATTR(fmi_attr_id_name),           1, bufName);

        if (req) {
            // name or valueReference failed to parse
            // Variable parsing failed, since these are required
            return -1;
        }

        // optional, failure to parse should only result in missing description
        fmi3_xml_parse_attr_as_string(context, elm_id, FMI3_ATTR(fmi_attr_id_description), 0, bufDesc);

        if (jm_vector_get_size(char)(bufDesc)) {
            /* Add the description to the model-wide set and retrieve the pointer */
            description = jm_string_set_put(&md->descriptions, jm_vector_get_itemp(char)(bufDesc, 0));
        }

        // Create the variable and set name at same time:
        const char* name;
        if (jm_vector_get_size(char)(bufName) == 0) {  // FIXME: Empty string has been resized to 0
            name = "";
        } else {
            name = jm_vector_get_itemp(char)(bufName, 0);
        }
        variable = fmi3_xml_alloc_variable_with_name(context, name);
        if (!variable) {return -1;}

        /* Add Variable ptr to modelDescription obj */
        if (!jm_vector_push_back(jm_voidp)(&md->variablesOrigOrder, variable)) {
            fmi3_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }

        /* Initialize rest of Variable */
        variable->vr = vr;
        variable->description = description;
        /* Default values */
        variable->type = NULL;
        variable->originalIndex = jm_vector_get_size(jm_voidp)(&md->variablesOrigOrder) - 1;
        variable->derivativeOf.variable = NULL;  // Could correspond to a valid vr, why we need the 'hasDerivativeOf' as well.
        variable->hasDerivativeOf = false;
        variable->previous.variable = NULL;      // Could correspond to a valid vr, why we need the 'hasPrevious' as well.
        variable->hasPrevious = false;
        variable->clocks = NULL;
        variable->reinit = 0;
        variable->canHandleMultipleSetPerTimeInstant = 1;
        variable->dimensions = NULL;
        variable->aliases = NULL;

        int res = 0;
        // The following attributes are all optional, failure to parse shall not result in an invalid variable
        // Unexpected (malloc) failures still force the parser to stop via fmi_xml_parse_fatal(...)

        /* Save start value for processing after reading all Dimensions */
        res |= fmi3_xml_parse_attr_as_string(context, elm_id, FMI3_ATTR(fmi_attr_id_start), 0, &context->variableStartAttr);

        /* Process common attributes */
        res |= fmi3_xml_variable_process_attr_causality_variability_initial(context, variable, elm_id);
        res |= fmi3_xml_variable_process_attr_clocks(context, variable, elm_id);
        /* clocks required in error check for previous */
        res |= fmi3_xml_variable_process_attr_previous(context, variable, elm_id);
        res |= fmi3_xml_variable_process_attr_multipleset(context, variable, elm_id);
        res |= fmi3_xml_variable_process_attr_intermediateupdate(context, variable, elm_id);

        if (res) {
            // Errors&Warnings in optional attributes still yield a valid Variable
            return 0; // Variable successfully parsed
        }
    }
    else { /* end of xml tag */
        /* Check that the type for the variable is set */
        if (!md->latestVariableIsValid) {return -1;} // skip post-processing for invalid variables
        fmi3_xml_variable_t* variable = jm_vector_get_last(jm_voidp)(&md->variablesOrigOrder);
        if (!variable->type) {
            jm_log_error(context->callbacks, module, "No variable type element for variable %s. Assuming Float64.", variable->name);

            return fmi3_xml_handle_Float64(context, NULL);
        }
    }
    return 0; // Variable successfully parsed
}

/**
 * Common handler for Variables.
 *
 * Creates a new variable and adds it to the parsed context. Common attributes
 * are processed and error checked.
 *
 * Type-specific default attribute values are also handled here.
 */
int fmi3_xml_handle_Variable(fmi3_xml_parser_context_t* context, const char* data) {
    fmi3_xml_model_description_t* md = context->modelDescription;
    if (fmi3_xml_handle_Variable_unchecked(context, data)) { 
        // Variable failed to parse due to 
        // failure to parse name/valueReference or unexpected (malloc) failure
        // or latestVariableIsValid == 0, if called on closing tag
        fmi3_xml_set_model_description_invalid(md);
        md->latestVariableIsValid = 0; // To skip post-processing in fmi3_xml_handle_Variable_unchecked
        return -1;
    } else {
        md->latestVariableIsValid = 1;
    }

    return 0;
}

static void fmi3_log_warning_if_start_required(fmi3_xml_parser_context_t* context, fmi3_xml_variable_t* variable) {
    // causality checks
    if (variable->causality == fmi3_causality_enu_input) {
        fmi3_xml_parse_warning(context, "Variable '%s': start value required for input variables",
            fmi3_xml_get_variable_name(variable));
    } else if (variable->causality == fmi3_causality_enu_parameter) {
        fmi3_xml_parse_warning(context, "Variable '%s': start value required for parameter variables",
            fmi3_xml_get_variable_name(variable));
    } else if (variable->causality == fmi3_causality_enu_structural_parameter) {
        fmi3_xml_parse_warning(context, "Variable '%s': start value required for structuralParameter variables",
            fmi3_xml_get_variable_name(variable));
    // variability checks
    } else if (variable->variability == fmi3_variability_enu_constant) {
        fmi3_xml_parse_warning(context, "Variable '%s': start value required for variables with constant variability",
            fmi3_xml_get_variable_name(variable));
    // initial checks
    } else if (variable->initial == fmi3_initial_enu_exact) {
        fmi3_xml_parse_warning(context, "Variable '%s': start value required for variables with initial == \"exact\"",
            fmi3_xml_get_variable_name(variable));
    } else if (variable->initial == fmi3_initial_enu_approx) {
        fmi3_xml_parse_warning(context, "Variable '%s': start value required for variables with initial == \"approx\"",
            fmi3_xml_get_variable_name(variable));
    }
}

static void fmi3_log_warnings_start_exists(fmi3_xml_parser_context_t* context, fmi3_xml_variable_t* variable) {
    /* If initial = calculated or causality = independent, it is not allowed to provide a start attribute. */
    /* The variable is calculated by the FMU from other variables during initialization. For calculated variables a start attribute must not be provided. */
    // TODO: This one is quite a bit more delicate to properly implement, since the default inferred initial may cause issues here.
    // E.g., Float with causality = output + start values
    // variability = continuous is inferred, default initial for coninuous outputs is 'calculated', would need to choose non-default, but valid initial
    /* 
    if (variable->initial == fmi3_initial_enu_calculated) {
        fmi3_xml_parse_warning(context, "Variable '%s': Variables with initial == \"calculated\" are not allowed to have a start attribute.",
            fmi3_xml_get_variable_name(variable));
    }
    */
    if (variable->causality == fmi3_causality_enu_independent) {
        fmi3_xml_parse_warning(context, "Variable '%s': The independent variable is not allowed to have a start attribute.",
            fmi3_xml_get_variable_name(variable));
    }
}

int fmi3_xml_handle_Dimension(fmi3_xml_parser_context_t* context, const char* data) {
    if (!data) { /* start of tag */
        fmi3_xml_variable_t* currentVar = jm_vector_get_last(jm_voidp)(&context->modelDescription->variablesOrigOrder);

        // check if dimension is allowed for parent variable
        if (currentVar->causality == fmi3_causality_enu_structural_parameter) {
            fmi3_xml_parse_warning(context, "Variable %s: structuralParameters must not have Dimension elements.",
                fmi3_xml_get_variable_name(currentVar));
            // Parse them anyways
        }

        fmi3_xml_dimension_t* dim = context->callbacks->malloc(sizeof(fmi3_xml_dimension_t));
        if (!dim) {
            fmi3_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }
        // initialize defaults
        dim->has_start = 0;
        dim->has_vr = 0;
        dim->vr = 0;
        dim->start = 0;

        /* handle attributes*/
        int hasStart = fmi3_xml_is_attr_defined(context, FMI3_ATTR(fmi_attr_id_start));
        int hasVr = fmi3_xml_is_attr_defined(context, FMI3_ATTR(fmi_attr_id_valueReference));

        /* error check */
        if ( !(hasStart || hasVr) ) {
            fmi3_xml_parse_warning(context, "Error parsing Dimension: no attribute 'start' or 'valueReference' found");
        }
        if (hasStart && hasVr) {
            fmi3_xml_parse_warning(context, "Error parsing Dimension: mutually exclusive attributes 'start' and 'valueReference' found");
        }
        // Parse them anyways

        /* set data */
        int ret = 0;
        // Gating behing hasStart/Vr from peeking is necessary,
        // since fmi3_xml_parse_attr_as_* returns 0 if attribute does not exists (since it is not required)
        if (hasStart) {
            ret = fmi3_xml_parse_attr_as_uint64(context, FMI3_ELM(fmi3_xml_elmID_Dimension), FMI3_ATTR(fmi_attr_id_start), 0, &dim->start, 0);
            dim->has_start = ret ? 0 : 1;
        }
        
        if (hasVr) {
            ret = fmi3_xml_parse_attr_as_uint32(context, FMI3_ELM(fmi3_xml_elmID_Dimension), FMI3_ATTR(fmi_attr_id_valueReference), 0, &dim->vr, 0);
            dim->has_vr = ret ? 0 : 1;
        }

        /* update parent variable */
        if (!currentVar->dimensions) {
            currentVar->dimensions = jm_vector_alloc(jm_voidp)(0, 0, context->callbacks);
        }
        if (!jm_vector_push_back(jm_voidp)(currentVar->dimensions, dim)) {
            jm_log_error(context->callbacks, module, "Unable to allocate memory");
            return -1;
        }

    } else { /* end of tag */
        /* do nothing */
    }

    return 0;
}

int fmi3_xml_handle_FloatXX(fmi3_xml_parser_context_t* context, const char* data,
        fmi3_xml_float_type_props_t* defaultType,
        fmi3_xml_elm_t elmID, /* ID of the Type (not the Variable) */  // XXX: Why?
        const fmi3_xml_primitive_type_t* primType) {

    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_variable_t* variable;
    fmi3_xml_type_definition_list_t* td;
    fmi3_xml_float_type_props_t* type;

    /* Extract common Variable info & handle errors*/
    if (fmi3_xml_handle_Variable(context, data)) {
        // No Variable was created
        return -1; // continue parsing, skips nested elements
    }

    variable = jm_vector_get_last(jm_voidp)(&md->variablesOrigOrder);
    td = &md->typeDefinitions;

    if (!data) { /* start of tag */
        fmi3_xml_variable_type_base_t* declaredType = 0;

        assert(!variable->type);

        /* Get declared type: either a default or user defined */
        declaredType = fmi3_parse_declared_type_attr(context, elmID, &defaultType->super);
        if (!declaredType) return -1;

        /* Set type properties */
        int hasUnit = fmi3_xml_is_attr_defined(context, FMI3_ATTR(fmi_attr_id_unit))
                   || fmi3_xml_is_attr_defined(context, FMI3_ATTR(fmi_attr_id_displayUnit));
        int hasMin  = fmi3_xml_is_attr_defined(context, FMI3_ATTR(fmi_attr_id_min));
        int hasMax  = fmi3_xml_is_attr_defined(context, FMI3_ATTR(fmi_attr_id_max));
        int hasNom  = fmi3_xml_is_attr_defined(context, FMI3_ATTR(fmi_attr_id_nominal));
        int hasQuan = fmi3_xml_is_attr_defined(context, FMI3_ATTR(fmi_attr_id_quantity));
        int hasRelQ = fmi3_xml_is_attr_defined(context, FMI3_ATTR(fmi_attr_id_relativeQuantity));
        int hasUnb  = fmi3_xml_is_attr_defined(context, FMI3_ATTR(fmi_attr_id_unbounded));

        if (hasUnit || hasMin || hasMax || hasNom || hasQuan || hasRelQ || hasUnb) {
            /* create a new type_props that overrides declared type's properties when necessary */
            type = fmi3_xml_parse_float_type_properties(context, elmID, declaredType, primType);
            if (!type) {
                variable->type = declaredType; /* fallback */
                return -1;
            }
            variable->type = &type->super;
        } else {
            variable->type = declaredType;
        }

        int res = 0;
        // optional attributes
        res |= fmi3_xml_variable_process_attr_derivative(context, variable, elmID);
        res |= fmi3_xml_variable_process_attr_reinit(context, variable, elmID);

        if (res) {
            // failure to parse optional attributes does not result in failed Variable
            return 0;
        }

    } else { /* end of tag */
        /* set start value */

        /* We must wait until after parsing dimensions, because we can't otherwise know
         * if it's a 1x1 array or a scalar variable just by reading the start value. */
        int hasStart = jm_vector_get_size(char)(&context->variableStartAttr);
        if (hasStart) {
            jm_string startAttr = jm_vector_get_itemp(char)(&context->variableStartAttr, 0);

            fmi3_xml_float_variable_start_t* start = (fmi3_xml_float_variable_start_t*)fmi3_xml_alloc_variable_type_start(td, variable->type, sizeof(fmi3_xml_float_variable_start_t));
            if (!start) {
                fmi3_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            if (fmi3_xml_variable_is_array(variable)) {
                size_t nArr; /* TODO: do something with this, e.g. dimension size verification */
                if (fmi3_xml_parse_attr_as_array(context, elmID, FMI3_ATTR(fmi_attr_id_start), 0, (void**)&start->start, &nArr, startAttr, primType)) {
                    // start failed to parse as array
                    // TODO: Can one get these from some sort of defaults instead?
                    start->start.array64s = NULL;
                    start->start.array32s = NULL;
                    fmi3_log_warning_if_start_required(context, variable);
                }
            } else { /* is scalar */
                // TODO:
                // We should not restore the attribute buffer, but instead just read it directly
                // from the string.

                /* restore the attribute buffer before it's used in set_attr_float */
                jm_vector_set_item(jm_string)(context->attrMapById, fmi_attr_id_start, startAttr);

                if (fmi3_xml_parse_attr_as_floatXX(context, elmID, FMI3_ATTR(fmi_attr_id_start), 0, &start->start,
                        fmi3_xml_get_type_default_value(primType->baseType), primType)) {
                    return -1;
                }
            }
            variable->type = &start->super;
            fmi3_log_warnings_start_exists(context, variable);
        } else {
            fmi3_log_warning_if_start_required(context, variable);
        }

        /* TODO: verify that dimension size == number of array start attribute values */
    }
    return 0;
}

int fmi3_xml_handle_IntXX(fmi3_xml_parser_context_t* context, const char* data,
        fmi3_xml_int_type_props_t* defaultType,
        fmi3_xml_elm_t elmID, /* ID of the Type (not the Variable) */
        const fmi3_xml_primitive_type_t* primType)
{
    /* Extract common Variable info & handle errors*/
    if (fmi3_xml_handle_Variable(context, data)) {
        // No Variable was created
        return -1; // continue parsing, skips nested elements
    }

    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_type_definition_list_t* td = &md->typeDefinitions;
    fmi3_xml_variable_t* variable = jm_vector_get_last(jm_voidp)(&md->variablesOrigOrder);
    fmi3_xml_variable_type_base_t* declaredType = NULL;
    fmi3_xml_int_type_props_t* type = NULL;
    if (!data) {
        // Type-specific attributes:
        declaredType = fmi3_parse_declared_type_attr(context, elmID, &defaultType->super) ;
        if (!declaredType) return -1;

        int hasMin  = fmi3_xml_is_attr_defined(context, FMI3_ATTR(fmi_attr_id_min));
        int hasMax  = fmi3_xml_is_attr_defined(context, FMI3_ATTR(fmi_attr_id_max));
        int hasQuan = fmi3_xml_is_attr_defined(context, FMI3_ATTR(fmi_attr_id_quantity));
        if (hasMin || hasMax || hasQuan) {
            type = fmi3_xml_parse_intXX_type_properties(context, elmID, declaredType, primType);
            if (!type) {
                variable->type = declaredType; /* fallback */
                return -1;
            }
            type->super.nextLayer = declaredType;
            variable->type = &type->super;
        } else {
            variable->type = declaredType;
        }
    }
    else {
        /* We must wait until after parsing dimensions, because we can't otherwise know
         * if it's a 1x1 array or a scalar variable just by reading the start value. */
        int hasStart = jm_vector_get_size(char)(&context->variableStartAttr);
        if (hasStart) {
            jm_string startAttr = jm_vector_get_itemp(char)(&context->variableStartAttr, 0);
            fmi3_xml_int_variable_start_t* start =
                    (void*)fmi3_xml_alloc_variable_type_start(td, variable->type, sizeof(fmi3_xml_int_variable_start_t));
            if (!start) {
                fmi3_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            if (fmi3_xml_variable_is_array(variable)) {
                size_t nArr; /* TODO: do something with this, e.g. dimension size verification */
                if (fmi3_xml_parse_attr_as_array(context, elmID, FMI3_ATTR(fmi_attr_id_start), 0,
                        (void**)&start->start, &nArr, startAttr, primType)) {
                    // start failed to parse as array
                    // TODO: Can one get these from some sort of defaults instead?
                    start->start.array64s = NULL;
                    start->start.array32s = NULL;
                    start->start.array16s = NULL;
                    start->start.array8s  = NULL;
                    start->start.array64u = NULL;
                    start->start.array32u = NULL;
                    start->start.array16u = NULL;
                    start->start.array8u  = NULL;
                    fmi3_log_warning_if_start_required(context, variable);
                }
            } else { /* is scalar */
                /* restore the attribute buffer before it's used in set_attr_int */
                jm_vector_set_item(jm_string)(context->attrMapById, fmi_attr_id_start, startAttr);
                // This default value should never be used, since the standard does not define it.
                // The FMU will hard-code one in the C API and it might not match our (this) default value.
                // Here we just give a valid value for the type so it's defined on our side at least.
                fmi3_int_union_t defaultVal;
                defaultVal.scalar64s = 0; /* set the whole bitfield to 0 - this will evaluate to '0' for all intXX types */

                if (fmi3_xml_parse_attr_as_intXX(context, elmID, FMI3_ATTR(fmi_attr_id_start), 0, &start->start, &defaultVal, primType)) {
                    jm_log_error(context->callbacks, module, "failed to parse start value for integer");
                    return -1;
                }
            }
            variable->type = &start->super;
            fmi3_log_warnings_start_exists(context, variable);
        } else {
            fmi3_log_warning_if_start_required(context, variable);
        }
    }
    return 0;
}

/* Macro for creating handle_TYPEXX function wrappers. */
#define gen_fmi3_xml_handle_TYPEXX(TYPE_NO_SIGN, TYPE, TYPE_LC, BITNESS)                          \
    int fmi3_xml_handle_##TYPE##BITNESS(fmi3_xml_parser_context_t* context, const char* data) {   \
        return fmi3_xml_handle_##TYPE_NO_SIGN##XX(context, data,                                  \
                &context->modelDescription->typeDefinitions.default##TYPE##BITNESS##Type,         \
                FMI3_ELM(fmi3_xml_elmID_##TYPE##BITNESS), &PRIMITIVE_TYPES.TYPE_LC##BITNESS);               \
    }

gen_fmi3_xml_handle_TYPEXX(Float, Float, float, 64)
gen_fmi3_xml_handle_TYPEXX(Float, Float, float, 32)

gen_fmi3_xml_handle_TYPEXX(Int,  Int,  int, 64)
gen_fmi3_xml_handle_TYPEXX(Int,  Int,  int, 32)
gen_fmi3_xml_handle_TYPEXX(Int,  Int,  int, 16)
gen_fmi3_xml_handle_TYPEXX(Int,  Int,  int,  8)
gen_fmi3_xml_handle_TYPEXX(Int, UInt, uint, 64)
gen_fmi3_xml_handle_TYPEXX(Int, UInt, uint, 32)
gen_fmi3_xml_handle_TYPEXX(Int, UInt, uint, 16)
gen_fmi3_xml_handle_TYPEXX(Int, UInt, uint,  8)

int fmi3_xml_handle_Boolean(fmi3_xml_parser_context_t *context, const char* data) {
    /* Extract common Variable info & handle errors*/
    if (fmi3_xml_handle_Variable(context, data)) {
        // No Variable was created
        return -1; // continue parsing, skips nested elements
    }

    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_type_definition_list_t* td = &md->typeDefinitions;
    fmi3_xml_variable_t* variable = jm_vector_get_last(jm_voidp)(&md->variablesOrigOrder);
    if (!data) {

        assert(!variable->type);

        variable->type = fmi3_parse_declared_type_attr(context, FMI3_ELM(fmi3_xml_elmID_Boolean), &td->defaultBooleanType) ;
        if (!variable->type) {return -1;}

    } else {
        /* We must wait until after parsing dimensions, because we can't otherwise know
         * if it's a 1x1 array or a scalar variable just by reading the start value. */
        int hasStart = jm_vector_get_size(char)(&context->variableStartAttr);
        if (hasStart) {
            jm_string startAttr = jm_vector_get_itemp(char)(&context->variableStartAttr, 0);
            fmi3_xml_int_variable_start_t* start = (fmi3_xml_int_variable_start_t*)fmi3_xml_alloc_variable_type_start(
                    td, variable->type, sizeof(fmi3_xml_int_variable_start_t));
            if (!start) {
                fmi3_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            if (fmi3_xml_variable_is_array(variable)) {
                size_t nArr;
                if (fmi3_xml_parse_attr_as_array(context, FMI3_ELM(fmi3_xml_elmID_Boolean), FMI3_ATTR(fmi_attr_id_start),
                        0, (void**)&start->start, &nArr, startAttr, &PRIMITIVE_TYPES.boolean)) {
                    // start failed to parse as array
                    // TODO: Can one get these from some sort of defaults instead?
                    // TODO: Should this have the fmi3_xml_int_variable_start_t type?
                    start->start.array64s = NULL;
                    start->start.array32s = NULL;
                    start->start.array16s = NULL;
                    start->start.array8s  = NULL;
                    start->start.array64u = NULL;
                    start->start.array32u = NULL;
                    start->start.array16u = NULL;
                    start->start.array8u  = NULL;
                    fmi3_log_warning_if_start_required(context, variable);
                }
            } else {
                /* restore the attribute buffer before it's used in set_attr_boolean */
                jm_vector_set_item(jm_string)(context->attrMapById, fmi_attr_id_start, startAttr);

                if (fmi3_xml_parse_attr_as_boolean(context, FMI3_ELM(fmi3_xml_elmID_Boolean), FMI3_ATTR(fmi_attr_id_start), 0,
                        (unsigned int*)&start->start.scalar32s, 0)) {
                    return -1;
                }
            }
            variable->type = &start->super;
            fmi3_log_warnings_start_exists(context, variable);
        } else {
            fmi3_log_warning_if_start_required(context, variable);
        }
        return 0;
    }
    return 0;
}

int fmi3_xml_handle_Binary(fmi3_xml_parser_context_t* context, const char* data) {
    /* Extract common Variable info & handle errors*/
    if (fmi3_xml_handle_Variable(context, data)) {
        // No Variable was created
        return -1; // continue parsing, skips nested elements
    }
    fmi3_xml_elm_t elmID = FMI3_ELM(fmi3_xml_elmID_Binary);  // The ID corresponding to the actual parsed element name
    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_type_definition_list_t* td = &md->typeDefinitions;
    fmi3_xml_variable_t* variable = jm_vector_get_last(jm_voidp)(&md->variablesOrigOrder);

    if (!data) {
        fmi3_xml_set_element_handle(context, "Start", FMI3_ELM(fmi3_xml_elmID_BinaryVariableStart));

        fmi3_xml_binary_type_props_t* vProps;  // Variable props
        fmi3_xml_variable_type_base_t* declaredType = fmi3_parse_declared_type_attr(context, elmID,
                &td->defaultBinaryType.super);
        int hasMimeType = fmi3_xml_is_attr_defined(context, FMI3_ATTR(fmi_attr_id_mimeType));
        int hasMaxSize  = fmi3_xml_is_attr_defined(context, FMI3_ATTR(fmi_attr_id_maxSize));
        if (hasMimeType || hasMaxSize) {
            // Create binary properties:
            vProps = fmi3_xml_parse_binary_type_properties(context, elmID, declaredType);
            if (!vProps) {return -1;} // Can only really fail due to malloc failures
        }
        else {
            vProps = (fmi3_xml_binary_type_props_t*)declaredType;
        }
        assert(!variable->type);
        variable->type = &vProps->super;
    } else {
        size_t nStart = jm_vector_get_size(jm_voidp)(&context->currentStartVariableValues);
        if (nStart > 0) {
            fmi3_xml_binary_variable_start_t* startObj =
                    (fmi3_xml_binary_variable_start_t*)fmi3_xml_alloc_variable_type_start(
                            td, variable->type, sizeof(fmi3_xml_binary_variable_start_t));
            if (!startObj) {
                fmi3_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            // increment during looping, only for valid elements
            startObj->nStart = 0;

             // number of bytes per element in context->currentStartVariableValues
            jm_vector_init(jm_voidp)(&startObj->binaryStartValues, 0, context->callbacks);
            jm_vector_init(size_t)(&startObj->binaryStartValuesSize, 0, context->callbacks);
            uint8_t* bytes;
            for (int i = 0; i < nStart; i++) {
                char* item = (char*)jm_vector_get_item(jm_voidp)(&context->currentStartVariableValues, i);
                size_t sz = strlen(item)/((size_t)2);
                bytes = context->callbacks->malloc(sz);
                if (!bytes) {
                    fmi3_xml_parse_fatal(context, "Could not allocate memory");
                    return -1;
                }
                if (fmi3_xml_hexstring_to_bytearray(context, item, bytes)) {
                    // not valid hexstring or unexpected failure, skip current element
                    context->callbacks->free(bytes); // not part of free loop over jm_vector
                } else {
                    startObj->nStart++;
                    jm_vector_push_back(jm_voidp)(&startObj->binaryStartValues, bytes);
                    jm_vector_push_back(size_t)(&startObj->binaryStartValuesSize, sz);
                }
                // We can now free the string now since we have converted it above
                context->callbacks->free(item);
            }

            variable->type = &startObj->super;
            fmi3_log_warnings_start_exists(context, variable);
            // Resize the vector to 0 since we are now done with the current variable.
            jm_vector_resize(jm_voidp)(&context->currentStartVariableValues, 0);
        } else {
            fmi3_log_warning_if_start_required(context, variable);
        }
    }
    return 0;
}

int fmi3_xml_handle_Clock(fmi3_xml_parser_context_t* context, const char* data) {
    /* Extract common Variable info & handle errors*/
    if (fmi3_xml_handle_Variable(context, data)) {
        // No Variable was created
        return -1; // continue parsing, skips nested elements
    }

    if (!data) {
        fmi3_xml_elm_t elmID = FMI3_ELM(fmi3_xml_elmID_Clock);  // The ID corresponding to the actual parsed element name
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_type_definition_list_t* td = &md->typeDefinitions;
        fmi3_xml_variable_t* variable = jm_vector_get_last(jm_voidp)(&md->variablesOrigOrder);

        // NOTE:
        // For other Variables we typically check if any type-specific attributes have been
        // set, and then we create a new vProps, else we fallback to the TypeDef. Not doing that
        // for Clocks, because different from other Variables, they have an additional required
        // attribute 'intervalVariability'. That means we always have more data than what is in
        // the TypeDef. There are other solutions, but this felt like the least complex for now.

        assert(!variable->type);
        fmi3_xml_variable_type_base_t* declaredType = fmi3_parse_declared_type_attr(context, elmID,
                &td->defaultClockType.super);

        fmi3_xml_clock_type_props_t* props = fmi3_xml_parse_clock_type_properties(context, elmID, declaredType);
        if (!props) {
            fmi3_xml_set_model_description_invalid(md);
            md->latestVariableIsValid = 0;
            return 0;
        }

        variable->type = &props->super;

        if (variable->hasPrevious) {
            fmi3_xml_parse_warning(context, "Variables of type Clock must not have the 'previous' attribute.");
        }
    }
    return 0;
}

int fmi3_xml_handle_String(fmi3_xml_parser_context_t *context, const char* data) {
    /* Extract common Variable info & handle errors*/
    if (fmi3_xml_handle_Variable(context, data)) {
        // No Variable was created
        return -1; // continue parsing, skips nested elements
    }

    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_type_definition_list_t* td = &md->typeDefinitions;
    fmi3_xml_variable_t* variable = jm_vector_get_last(jm_voidp)(&md->variablesOrigOrder);

    if (!data) {
        fmi3_xml_set_element_handle(context, "Start", FMI3_ELM(fmi3_xml_elmID_StringVariableStart));
        assert(!variable->type);

        variable->type = fmi3_parse_declared_type_attr(context, FMI3_ELM(fmi3_xml_elmID_String), &td->defaultStringType) ;
        if (!variable->type) return -1;
    }
    else {
        /* Cannot use *has_start for strings */
        size_t nStart = jm_vector_get_size(jm_voidp)(&context->currentStartVariableValues);
        if (nStart > 0) {
            fmi3_xml_string_variable_start_t* startObj =
                    (fmi3_xml_string_variable_start_t*)fmi3_xml_alloc_variable_type_start(
                            td, variable->type, sizeof(fmi3_xml_string_variable_start_t));
            if (!startObj) {
                fmi3_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            jm_vector_init(jm_voidp)(&startObj->stringStartValues, 0, context->callbacks);
            // Passing ownership to stringStartValues
            jm_vector_copy(jm_voidp)(&startObj->stringStartValues, &context->currentStartVariableValues);
            variable->type = &startObj->super;
            // Resize the vector to 0 since we are now done with the previous values.
            jm_vector_resize(jm_voidp)(&context->currentStartVariableValues, 0);
            if (!fmi3_xml_variable_is_array(variable) && (nStart != 1)) {
                fmi3_xml_parse_warning(context, "Variable %s: Found %" PRIu64 " Start elements for non-array variable", variable->name, nStart);
            }
            fmi3_log_warnings_start_exists(context, variable); // only captures binary arrays
        } else {
            fmi3_log_warning_if_start_required(context, variable);
        }
    }
    return 0;
}

/**
 * Method responsible for populating context->currentStartVariableValues where each member
 * is a null-terminated string that contain its start value of the string variable.
 * For example in context->currentStartVariableValues[0] we store the first start value
 *             in context->currentStartVariableValues[1] we store the second start value
 * and so on.
*/
int fmi3_xml_handle_StringVariableStart(fmi3_xml_parser_context_t* context, const char* data) {
    if (!data) {
        /* For each <Start ...>, allocate memory, copy attribute to 'value' and push back to 'vec'. */
        jm_vector(jm_voidp)* vec = &context->currentStartVariableValues;
        const char* attr;
        if (fmi3_xml_get_attr_str(context, FMI3_ELM(fmi3_xml_elmID_StringVariableStart), FMI3_ATTR(fmi_attr_id_value), 0, &attr)) return -1;
        char* attrValue = context->callbacks->malloc(strlen(attr) + 1);
        strcpy(attrValue, attr);
        jm_vector_push_back(jm_voidp)(vec, attrValue);
    }
    return 0;
}

/**
 * Placeholder handler to initialize the framework.
 *
 * One of the Variable type-specific handlers should always be used.
*/
int fmi3_xml_handle_Start(fmi3_xml_parser_context_t* context, const char* data) {
    assert(0);
    return 0;
}


int fmi3_xml_handle_BinaryVariableStart(fmi3_xml_parser_context_t* context, const char* data) {
    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_type_definition_list_t* td = &md->typeDefinitions;
    fmi3_xml_variable_t* variable = jm_vector_get_last(jm_voidp)(&md->variablesOrigOrder);
    if (!data) {
        if (fmi3_xml_variable_is_array(variable)) {
            /* For each <Start ...>, allocate memory, copy attribute to 'value' and push back to 'vec'. */
            jm_vector(jm_voidp)* vec = &context->currentStartVariableValues;

            if (!fmi3_xml_peek_attr_str(context, FMI3_ATTR(fmi_attr_id_value))) {
                fmi3_xml_parse_error(context, "Start missing the required attribute 'value'");
                return -1; // Parsing start element failed
            }
            const char* attr;
            if (fmi3_xml_get_attr_str(context, FMI3_ELM(fmi3_xml_elmID_BinaryVariableStart), FMI3_ATTR(fmi_attr_id_value), 0, &attr)) {return -1;}
            int len = attr ? strlen(attr) : 0;
            if (len == 0) {
                fmi3_xml_parse_error(context, "Empty value attribute in Start element");
                return -1;
            }
            char* attrAsStr = context->callbacks->malloc(len + 1);
            strcpy(attrAsStr, attr);
            jm_vector_push_back(jm_voidp)(vec, attrAsStr);
        } else {
            jm_vector(char)* bufStartStr = fmi3_xml_reserve_parse_buffer(context, 1, 100);
            if (fmi3_xml_parse_attr_as_string(context, FMI3_ELM(fmi3_xml_elmID_BinaryVariableStart), FMI3_ATTR(fmi_attr_id_value), 0, bufStartStr)) {
                return -1;
            }

            // Add a start object to the top of the variable's type list:
            fmi3_xml_binary_variable_start_t* startObj;
            size_t len = jm_vector_get_size(char)(bufStartStr);
            if (len == 0) {
                fmi3_xml_parse_error(context, "Empty value attribute in Start element");
                return -1;
            }
            size_t arrSize = len / 2;
            size_t totSize = sizeof(fmi3_xml_binary_variable_start_t) + arrSize;
            startObj = (fmi3_xml_binary_variable_start_t*)fmi3_xml_alloc_variable_type_start(td, variable->type, totSize);
            if (!startObj) {
                fmi3_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            startObj->nStart = arrSize;
            if (fmi3_xml_hexstring_to_bytearray(context, jm_vector_get_itemp(char)(bufStartStr, 0), startObj->start)) {
                return -1;
            }
            variable->type = &startObj->super;
            fmi3_log_warnings_start_exists(context, variable); // binary non-array
        }
    }
    return 0;
}

fmi3_xml_enum_variable_props_t* fmi3_xml_parse_enum_properties(fmi3_xml_parser_context_t* context, fmi3_xml_enum_variable_props_t* declaredType) {

    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_enum_variable_props_t* props = 0;
    fmi3_xml_elm_t elmID = FMI3_ELM(fmi3_xml_elmID_Enumeration);
    const char* quantity = 0;

    jm_vector(char)* bufQuantity = fmi3_xml_reserve_parse_buffer(context,3,100);

    props = (fmi3_xml_enum_variable_props_t*)fmi3_xml_alloc_variable_or_typedef_props(&md->typeDefinitions,
           &md->typeDefinitions.defaultEnumType.base.super, sizeof(fmi3_xml_enum_variable_props_t));

    if (!bufQuantity || !props || fmi3_xml_parse_attr_as_string(context, elmID, FMI3_ATTR(fmi_attr_id_quantity), 0, bufQuantity))
        return 0;
    if (jm_vector_get_size(char)(bufQuantity))
        quantity = jm_string_set_put(&md->typeDefinitions.quantities, jm_vector_get_itemp(char)(bufQuantity, 0));

    props->quantity = (quantity == 0) ? declaredType->quantity : quantity;

    if (    fmi3_xml_parse_attr_as_int32(context, elmID, FMI3_ATTR(fmi_attr_id_min), 0, &props->typeMin, declaredType->typeMin) ||
            fmi3_xml_parse_attr_as_int32(context, elmID, FMI3_ATTR(fmi_attr_id_max), 0, &props->typeMax, declaredType->typeMax)) {
        return 0;
    }

    return props;
}

int fmi3_xml_handle_Enumeration(fmi3_xml_parser_context_t *context, const char* data) {
    /* Extract common Variable info & handle errors*/
    if (fmi3_xml_handle_Variable(context, data)) {
        // No Variable was created
        return -1; // continue parsing, skips nested elements
    }

    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_type_definition_list_t* td = &md->typeDefinitions;
    fmi3_xml_variable_t* variable = jm_vector_get_last(jm_voidp)(&md->variablesOrigOrder);
    fmi3_xml_variable_type_base_t * declaredType = 0;
    fmi3_xml_enum_variable_props_t * type = 0;

    if (!data) {

        assert(!variable->type);

        declaredType = fmi3_parse_declared_type_attr(context,
            FMI3_ELM(fmi3_xml_elmID_Enumeration), &td->defaultEnumType.base.super);

        if (!declaredType) return -1;

        if (
                fmi3_xml_is_attr_defined(context,FMI3_ATTR(fmi_attr_id_min)) ||
                fmi3_xml_is_attr_defined(context,FMI3_ATTR(fmi_attr_id_max)) ||
                fmi3_xml_is_attr_defined(context,FMI3_ATTR(fmi_attr_id_quantity))
                ) {
            fmi3_xml_enum_variable_props_t* dtProps = 0; /* declaredType properties */

            if (declaredType->structKind == fmi3_xml_type_struct_enu_typedef)
                dtProps = (fmi3_xml_enum_variable_props_t*)declaredType->nextLayer;
            else
                dtProps = (fmi3_xml_enum_variable_props_t*)declaredType;
            assert(dtProps->super.structKind == fmi3_xml_type_struct_enu_props);
            fmi3_xml_reserve_parse_buffer(context, 1, 0);
            fmi3_xml_reserve_parse_buffer(context, 2, 0);
            type = fmi3_xml_parse_enum_properties(context, dtProps);
            if (!type) {
                variable->type = &((fmi3_xml_enum_variable_props_t*)declaredType)->super; /* fallback to default */
                return -1;
            }
            type->super.nextLayer = declaredType;
        }
        else {
            type = (fmi3_xml_enum_variable_props_t*)declaredType;
        }
        variable->type = &type->super;
    }
    else {
        int hasStart = jm_vector_get_size(char)(&context->variableStartAttr);
        if (hasStart) {
            jm_string startAttr = jm_vector_get_itemp(char)(&context->variableStartAttr, 0);
            fmi3_xml_int_variable_start_t* start = (fmi3_xml_int_variable_start_t*)fmi3_xml_alloc_variable_type_start(
                    td, variable->type, sizeof(fmi3_xml_int_variable_start_t));
            if (!start) {
                fmi3_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            if (fmi3_xml_variable_is_array(variable)) {
                size_t nArr; /* TODO: do something with this, e.g. dimension size verification */
                if (fmi3_xml_parse_attr_as_array(context, FMI3_ELM(fmi3_xml_elmID_Enumeration), FMI3_ATTR(fmi_attr_id_start), 0,
                        (void**)&start->start, &nArr, startAttr, &PRIMITIVE_TYPES.enumeration)) {
                    // start failed to parse as array
                    // TODO: Can one get these from some sort of defaults instead?
                    // TODO: Should this have the fmi3_xml_int_variable_start_t type?
                    start->start.array64s = NULL;
                    start->start.array32s = NULL;
                    start->start.array16s = NULL;
                    start->start.array8s  = NULL;
                    start->start.array64u = NULL;
                    start->start.array32u = NULL;
                    start->start.array16u = NULL;
                    start->start.array8u  = NULL;
                    fmi3_log_warning_if_start_required(context, variable);
                }
            } else {
                /* restore the attribute buffer before it's used in set_attr_int */
                jm_vector_set_item(jm_string)(context->attrMapById, fmi_attr_id_start, startAttr);

                if (fmi3_xml_parse_attr_as_intXX(context, FMI3_ELM(fmi3_xml_elmID_Enumeration),
                        FMI3_ATTR(fmi_attr_id_start), 0, &start->start.scalar64s, 0, &PRIMITIVE_TYPES.enumeration)) {
                    start->start.scalar64s = type->typeMin;
                }
            }
            variable->type = &start->super;
            fmi3_log_warnings_start_exists(context, variable);
        } else {
            fmi3_log_warning_if_start_required(context, variable);
        }
    }
    return 0;
}

int fmi3_xml_handle_Alias(fmi3_xml_parser_context_t* context, const char* data) {
    if (!data) {
        fmi3_xml_elm_t elmID = FMI3_ELM(fmi3_xml_elmID_Alias);
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_variable_t* baseVar = jm_vector_get_last(jm_voidp)(&md->variablesOrigOrder);
        size_t bufIdx = 1;

        // FIXME: bufDesc will have size==0 when the attribute value is the empty string,
        // making it indistinguishable from not being defined at all.
        bool hasDesc = fmi3_xml_peek_attr_str(context, FMI3_ATTR(fmi_attr_id_description)) != NULL;
        
        // Read the attributes to memory owned by FMIL:
        jm_vector(char)* bufName = fmi3_xml_reserve_parse_buffer(context, bufIdx++, 100);
        jm_vector(char)* bufDesc = fmi3_xml_reserve_parse_buffer(context, bufIdx++, 100);
        if (!bufName || !bufDesc) return -1;
        if (fmi3_xml_parse_attr_as_string(context, elmID, FMI3_ATTR(fmi_attr_id_name),        1, bufName)) return -1;
        if (fmi3_xml_parse_attr_as_string(context, elmID, FMI3_ATTR(fmi_attr_id_description), 0, bufDesc)) return -1;

        // Create the alias and set name at same time:
        const char* name;
        if (jm_vector_get_size(char)(bufName) == 0) {  // FIXME: Empty string has been resized to 0
            name = "";
        } else {
            name = jm_vector_get_itemp(char)(bufName, 0);
        }
        fmi3_xml_alias_variable_t* alias = fmi3_xml_alloc_alias_with_name(context, name);
        if (!alias) return -1;
        
        // Add the alias to the base variable:
        if (!baseVar->aliases) {
            baseVar->aliases = jm_vector_alloc(jm_voidp)(0, 0, context->callbacks);
        }
        jm_vector_push_back(jm_voidp)(baseVar->aliases, alias);

        // Set the other fields:
        if (hasDesc) {
            /* Add the description to the model-wide set and retrieve the pointer */
            if (jm_vector_get_size(char)(bufDesc) == 0) {
                // FIXME: It doesn't work to get empty string from bufDesc since it's resized to 0
                alias->description = jm_string_set_put(&md->descriptions, "");
            } else {
                alias->description = jm_string_set_put(&md->descriptions, jm_vector_get_itemp(char)(bufDesc, 0));
            }
        }

        // Set displayUnit if FloatXX:
        alias->displayUnit = NULL;
        if (fmi3_base_type_enu_is_float(fmi3_xml_get_variable_base_type(baseVar))) {
            jm_vector(char)* bufDisplayUnit = fmi3_xml_reserve_parse_buffer(context, bufIdx++, 100);
            if (!bufDisplayUnit) return -1;
            if (fmi3_xml_parse_attr_as_string(context, elmID, FMI3_ATTR(fmi_attr_id_displayUnit), 0, bufDisplayUnit)) return -1;
            if (jm_vector_get_size(char)(bufDisplayUnit)) {
                jm_named_ptr searchKey, *searchRes;
                searchKey.name = jm_vector_get_itemp(char)(bufDisplayUnit, 0);
                searchRes = jm_vector_bsearch(jm_named_ptr)(
                            &md->displayUnitDefinitions, &searchKey, jm_compare_named);
                if (!searchRes) {
                    fmi3_xml_parse_warning(context, "Unknown displayUnit: %s",
                            jm_vector_get_itemp(char)(bufDisplayUnit, 0));
                } else {
                    alias->displayUnit = searchRes->ptr;
                }
                // TODO: Spec requires that displayUnit is only defined if unit is also defined. Check it!
            } else {
                // XXX: For variables we set the displayUnit to the unit when the displayUnit is not defined.
                // Not clear why we do that. Skipping here for now.
            }
        }
    }
    return 0;
}

static int fmi3_xml_compare_variable_original_index(const void* first, const void* second) {
    size_t a = (*(fmi3_xml_variable_t**)first)->originalIndex;
    size_t b = (*(fmi3_xml_variable_t**)second)->originalIndex;
    if (a < b) return -1;
    if (a > b) return 1;
    return 0;
}

static int fmi3_xml_compare_vr_and_original_index(const void* first, const void* second) {
    int ret = fmi3_xml_compare_vr(first, second);
    if (ret != 0) return ret;

    fmi3_xml_variable_t* a = *(fmi3_xml_variable_t**)first;
    fmi3_xml_variable_t* b = *(fmi3_xml_variable_t**)second;
    ret = a->causality - b->causality;
    if (ret != 0 ) return ret;
    ret = a->variability - b->variability;
    if (ret != 0) return ret;

    size_t ai = a->originalIndex;
    size_t bi = b->originalIndex;
    if (ai > bi) return 1;
    if (ai < bi) return -1;

    return 0;
}

int fmi3_xml_handle_ModelVariables(fmi3_xml_parser_context_t* context, const char* data) {
    if (!data) {
        jm_log_verbose(context->callbacks, module, "Parsing XML element ModelVariables");
    } else {
         // Post-process variable list
        fmi3_xml_model_description_t* md = context->modelDescription;

        // Create variablesByName
        size_t nVars = jm_vector_get_size(jm_voidp)(&md->variablesOrigOrder);
        jm_vector_resize(jm_named_ptr)(&md->variablesByName, nVars);  // Allocate a lot of memory to avoid resizing
        jm_vector_resize(jm_named_ptr)(&md->variablesByName, 0);      // Make sure we start pushback at index=0
        for (size_t i = 0; i < nVars; i++) {
            jm_named_ptr named;
            fmi3_xml_variable_t* v = jm_vector_get_item(jm_voidp)(&md->variablesOrigOrder, i);
            named.name = v->name;
            named.ptr = v;
            jm_vector_push_back(jm_named_ptr)(&md->variablesByName, named);
            if (v->aliases) {
                fmi3_xml_alias_variable_t* alias;
                for (size_t j = 0; j < jm_vector_get_size(jm_voidp)(v->aliases); j++) {
                    alias = jm_vector_get_item(jm_voidp)(v->aliases, j);
                    named.name = alias->name;
                    named.ptr = v;
                    jm_vector_push_back(jm_named_ptr)(&md->variablesByName, named);
                }
            }
        }
        jm_vector_qsort(jm_named_ptr)(&md->variablesByName, jm_compare_named);

        // Create variablesByVR
        jm_vector_resize(jm_voidp)(&md->variablesByVR, nVars);
        for (size_t i = 0; i < nVars; i++) {
            jm_vector_set_item(jm_voidp)(&md->variablesByVR, i, jm_vector_get_item(jm_voidp)(&md->variablesOrigOrder, i));
        }
        jm_vector_qsort(jm_voidp)(&md->variablesByVR, fmi3_xml_compare_vr_and_original_index);

        if (nVars > 0) {  // nVars=0 would cause integer overflow in the loop condition
            /* Error check duplicate variable names */
            for (size_t i = 0; i < nVars-1; ++i) {
                const char* name1 = jm_vector_get_item(jm_named_ptr)(&md->variablesByName, i).name;
                const char* name2 = jm_vector_get_item(jm_named_ptr)(&md->variablesByName, i+1).name;
                if (strcmp(name1, name2) == 0) {
                    fmi3_xml_parse_error(context, 
                            "Two variables with the same name '%s' found. This is not allowed.", name1);
                    fmi3_xml_set_model_description_invalid(md);
                }
            }

            // Error check duplicate VRs:
            for (size_t i = 0; i < nVars-1; ++i) {
                fmi3_xml_variable_t* v1 = jm_vector_get_item(jm_voidp)(&md->variablesByVR, i);
                fmi3_xml_variable_t* v2 = jm_vector_get_item(jm_voidp)(&md->variablesByVR, i+1);
                if (v1->vr == v2->vr) {
                    fmi3_xml_parse_error(context, "The following variables have the same valueReference: %s, %s",
                            v1->name, v2->name);
                    fmi3_xml_set_model_description_invalid(md);
                }
            }
        }

        yyscan_t scanner;
        YY_BUFFER_STATE buf;
        /* check variable name syntax */
        if (md->namingConvension == fmi3_naming_enu_structured) {
            yyfmi3lex_init(&scanner);
            for (size_t i = 0; i < nVars; ++i) {
                char *name = ((fmi3_xml_variable_t *) jm_vector_get_item(jm_voidp)(
                        &md->variablesOrigOrder, i))->name;
                buf = yyfmi3_scan_string(name, scanner);
                yyfmi3parse(scanner, md->callbacks, name);
                yyfmi3_delete_buffer(buf, scanner);
            }
            yyfmi3lex_destroy(scanner);
        }

        // look up actual pointers for the derivativeOf and previous fields
        for (size_t i = 0; i < nVars; i++) {
            fmi3_xml_variable_t *variable = jm_vector_get_item(jm_voidp)(&md->variablesOrigOrder, i);

            // These 2 checks can cause secondary errors, if the variable with the given valueReference was invalid

            if (variable->hasDerivativeOf) {
                // Resolve VR to variable.
                fmi3_value_reference_t vr = variable->derivativeOf.vr;
                variable->derivativeOf.variable = fmi3_xml_get_variable_by_vr(md, vr);
                if (!variable->derivativeOf.variable) {
                    fmi3_xml_parse_error(context, "The valueReference in derivative=\"%" PRIu32 "\" "
                                                  "did not resolve to any variable.", vr);
                    fmi3_xml_set_model_description_invalid(md);
                }
            }
            if (variable->hasPrevious) {
                // Resolve VR to variable.
                fmi3_value_reference_t vr = variable->previous.vr;
                variable->previous.variable = fmi3_xml_get_variable_by_vr(md, vr);
                if (!variable->previous.variable) {
                    fmi3_xml_parse_error(context, "The valueReference in previous=\"%" PRIu32 "\" "
                                                  "did not resolve to any variable.", vr);
                    fmi3_xml_set_model_description_invalid(md);
                }
            }
        }

        if (!md->isValid) { 
            fmi3_xml_parse_fatal(context, "Fatal failure in parsing ModelVariables. Variable(s) failed to parse or an essential error check failed.");
            return -1;
        }
        // TODO: Check variables that specify Dimension sizes (must be structuralParameter or constant)
    }
    return 0;
}
