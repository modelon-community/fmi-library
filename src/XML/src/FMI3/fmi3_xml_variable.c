/*
    Copyright (C) 2012-2023 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

     This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#include <JM/jm_vector.h>

#include "fmi3_xml_parser.h"
#include "fmi3_xml_type_impl.h"
#include "fmi3_xml_model_description_impl.h"
#include "fmi3_xml_dimension_impl.h"

#include "fmi3_xml_variable_impl.h"

static const char* module = "FMI3XML";


/* Define fmi3_value_reference_t vector functions */
#define JM_TEMPLATE_INSTANCE_TYPE fmi3_value_reference_t
#include "JM/jm_vector_template.h"
#undef JM_TEMPLATE_INSTANCE_TYPE


struct fmi3_xml_variable_default_values {
    fmi3_float32_t float32;
    fmi3_float64_t float64;
};

static struct fmi3_xml_variable_default_values VARIABLE_DEFAULT_VALUES = { 0, 0 };

static void* fmi3_xml_get_type_default_value(fmi3_bitness_enu_t bitness) {
    switch (bitness) {
        case fmi3_bitness_32:
            return &VARIABLE_DEFAULT_VALUES.float32;
        case fmi3_bitness_64:
            return &VARIABLE_DEFAULT_VALUES.float64;
        default:
            assert(0); /* implementation error */
            return NULL;
    }
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

fmi3_variable_alias_kind_enu_t fmi3_xml_get_variable_alias_kind(fmi3_xml_variable_t* v) {
    return (fmi3_variable_alias_kind_enu_t)v->aliasKind;
}

fmi3_xml_variable_t* fmi3_xml_get_variable_alias_base(fmi3_xml_model_description_t* md, fmi3_xml_variable_t* v) {
    fmi3_xml_variable_t key;
    fmi3_xml_variable_t *pkey = &key, *base;
    void ** found;
    if(!md->variablesByVR) return 0;
    if(v->aliasKind == fmi3_variable_is_not_alias) return v;
    key = *v;
    key.aliasKind = fmi3_variable_is_not_alias;

    found = jm_vector_bsearch(jm_voidp)(md->variablesByVR,(void**)&pkey, fmi3_xml_compare_vr);
    assert(found);
    base = *found;
    return base;
}

/*
    Return the list of all the variables aliased to the given one (including the base one.
    The list is ordered: base variable, aliases.
*/
jm_status_enu_t fmi3_xml_get_variable_aliases(fmi3_xml_model_description_t* md, fmi3_xml_variable_t* v,
        jm_vector(jm_voidp)* list)
{
    fmi3_xml_variable_t key, *cur;
    fmi3_value_reference_t vr = fmi3_xml_get_variable_vr(v);
    size_t baseIndex, i, num = jm_vector_get_size(jm_voidp)(md->variablesByVR);
    key = *v;
    key.aliasKind = 0;
    cur = &key;
    baseIndex = jm_vector_bsearch_index(jm_voidp)(md->variablesByVR,(void**)&cur, fmi3_xml_compare_vr);
    cur = (fmi3_xml_variable_t*)jm_vector_get_item(jm_voidp)(md->variablesByVR, baseIndex);
    assert(cur);
    i = baseIndex + 1;
    while(fmi3_xml_get_variable_vr(cur) == vr) {
        if(!jm_vector_push_back(jm_voidp)(list, cur)) {
            jm_log_fatal(md->callbacks,module,"Could not allocate memory");
            return jm_status_error;
        };
        if(i >= num) break;
        cur = (fmi3_xml_variable_t*)jm_vector_get_item(jm_voidp)(md->variablesByVR, i);
        assert(cur);
        i++;
    }
    if(baseIndex) {
        i = baseIndex - 1;
        cur = (fmi3_xml_variable_t*)jm_vector_get_item(jm_voidp)(md->variablesByVR, i);
        while(fmi3_xml_get_variable_vr(cur) == vr) {
            if(!jm_vector_push_back(jm_voidp)(list, cur)) {
                jm_log_fatal(md->callbacks,module,"Could not allocate memory");
                return jm_status_error;
            };
            i--;
            if(!i) break;
            cur = (fmi3_xml_variable_t*)jm_vector_get_item(jm_voidp)(md->variablesByVR, i - 1);
            assert(cur);
        }
    }
    return jm_status_success;
}

fmi3_xml_variable_typedef_t* fmi3_xml_get_variable_declared_type(fmi3_xml_variable_t* v) {
    return (fmi3_xml_variable_typedef_t*)(fmi3_xml_find_type_struct(v->type, fmi3_xml_type_struct_enu_typedef));
}

fmi3_base_type_enu_t fmi3_xml_get_variable_base_type(fmi3_xml_variable_t* v) {
    fmi3_xml_variable_type_base_t* type = v->type;
    return (type->baseType);
}

jm_vector(fmi3_xml_dimension_t)* fmi3_xml_get_variable_dimension_vector(fmi3_xml_variable_t* v) {
    return &v->dimensionsVector;
}

int fmi3_xml_variable_is_array(fmi3_xml_variable_t* v) {
    return jm_vector_get_size(fmi3_xml_dimension_t)(&v->dimensionsVector) > 0;
}

int fmi3_xml_get_variable_has_start(fmi3_xml_variable_t* v) {
    return (v->type->structKind == fmi3_xml_type_struct_enu_start); /* TODO: the typeBaseStruct chain is ordered, update UML-like docs with the different possible variations (and commit the docs) */
}

fmi3_variability_enu_t fmi3_xml_get_variability(fmi3_xml_variable_t* v) {
    return (fmi3_variability_enu_t)v->variability;
}

fmi3_causality_enu_t fmi3_xml_get_causality(fmi3_xml_variable_t* v) {
    return (fmi3_causality_enu_t)v->causality;
}

fmi3_initial_enu_t fmi3_xml_get_initial(fmi3_xml_variable_t* v) {
    return (fmi3_initial_enu_t)v->initial;
}

fmi3_xml_variable_t* fmi3_xml_get_previous(fmi3_xml_variable_t* v) {
    return v->previous.variable;
}

fmi3_boolean_t fmi3_xml_get_canHandleMultipleSetPerTimeInstant(fmi3_xml_variable_t* v) {
    return (fmi3_boolean_t)v->canHandleMultipleSetPerTimeInstant;
}

jm_status_enu_t fmi3_xml_get_variable_clocks(fmi3_xml_model_description_t* md, fmi3_xml_variable_t* v,
        jm_vector(jm_voidp)* list)
{
    fmi3_value_reference_t clockVr;
    fmi3_xml_variable_t* clockVar;
    size_t nClocks = jm_vector_get_size(fmi3_value_reference_t)(v->clocks);
    for (size_t i = 0; i < nClocks; i++) {
        clockVr = jm_vector_get_item(fmi3_value_reference_t)(v->clocks, i);
        clockVar = fmi3_xml_get_variable_by_vr(md, fmi3_base_type_clock, clockVr);
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
    return fmi3_xml_get_float_variable_nominal(v); /* TODO: this is not valid for arrays - .arrayXX will always be null - why do we even want to return 'nominal'? */
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

/* Generic start getter that's intentionally type generic - wrappers generated by macros return the specific type.
   @result - the return argument.
   returns:  0 if start value is defined, else 1.
*/
static int fmi3_xml_get_int_variable_start(fmi3_xml_int_variable_t* v, fmi3_int_union_t* result){
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    if(fmi3_xml_get_variable_has_start(vv)) {
        fmi3_xml_int_variable_start_t* start = (fmi3_xml_int_variable_start_t*)(vv->type);
        *result = start->start;
        return 0;
    }
    /* TODO: access of default value provided by FMIL - log warning? */
    return 1;
}

static fmi3_xml_float_type_props_t* fmi3_xml_find_type_struct_float_props(fmi3_xml_variable_t* v) {
    return (fmi3_xml_float_type_props_t*)(fmi3_xml_find_type_struct(v->type, fmi3_xml_type_struct_enu_props));
}

// -----------------------------------------------------------------------------
// Float64
// -----------------------------------------------------------------------------

fmi3_xml_float64_variable_t* fmi3_xml_get_variable_as_float64(fmi3_xml_variable_t* v) {
    if (fmi3_xml_get_variable_base_type(v) == fmi3_base_type_float64)
        return (void*)v;
    return 0;
}

fmi3_xml_float64_variable_t* fmi3_xml_get_float64_variable_derivative_of(fmi3_xml_float64_variable_t* v) {
    fmi3_xml_variable_t *vv = (fmi3_xml_variable_t *)v;
    return (fmi3_xml_float64_variable_t *)vv->derivativeOf;
}

fmi3_boolean_t fmi3_xml_get_float64_variable_reinit(fmi3_xml_float64_variable_t* v) {
    fmi3_xml_variable_t *vv = (fmi3_xml_variable_t *)v;
    return (fmi3_boolean_t)vv->reinit;
}

fmi3_xml_unit_t* fmi3_xml_get_float64_variable_unit(fmi3_xml_float64_variable_t* v) {
    fmi3_xml_float_type_props_t* props = fmi3_xml_find_type_struct_float_props((fmi3_xml_variable_t*)v);
    if(!props || !props->displayUnit) return 0;
    return props->displayUnit->baseUnit;
}

fmi3_xml_display_unit_t* fmi3_xml_get_float64_variable_display_unit(fmi3_xml_float64_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_float_type_props_t* props =
        (fmi3_xml_float_type_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));
    if(!props || !props->displayUnit || !props->displayUnit->displayUnit[0]) return 0;
    return props->displayUnit;
}

fmi3_string_t fmi3_xml_get_float64_variable_quantity(fmi3_xml_float64_variable_t* v) {
    fmi3_xml_float_type_props_t* props = fmi3_xml_find_type_struct_float_props((fmi3_xml_variable_t*)v);
    if(!props) return NULL;
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
    if (fmi3_xml_get_variable_base_type(v) == fmi3_base_type_float32)
        return (void*)v;
    return 0;
}

fmi3_xml_float32_variable_t* fmi3_xml_get_float32_variable_derivative_of(fmi3_xml_float32_variable_t* v) {
    fmi3_xml_variable_t *vv = (fmi3_xml_variable_t *)v;

    return (fmi3_xml_float32_variable_t *)vv->derivativeOf;
}

fmi3_boolean_t fmi3_xml_get_float32_variable_reinit(fmi3_xml_float32_variable_t* v) {
    fmi3_xml_variable_t *vv = (fmi3_xml_variable_t *)v;
    return (fmi3_boolean_t)vv->reinit;
}

fmi3_xml_unit_t* fmi3_xml_get_float32_variable_unit(fmi3_xml_float32_variable_t* v) {
    fmi3_xml_float_type_props_t* props = fmi3_xml_find_type_struct_float_props((fmi3_xml_variable_t*)v);
    if(!props || !props->displayUnit) return 0;
    return props->displayUnit->baseUnit;
}

fmi3_xml_display_unit_t* fmi3_xml_get_float32_variable_display_unit(fmi3_xml_float32_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_float_type_props_t* props =
        (fmi3_xml_float_type_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));
    if(!props || !props->displayUnit || !props->displayUnit->displayUnit[0]) return 0;
    return props->displayUnit;
}

fmi3_string_t fmi3_xml_get_float32_variable_quantity(fmi3_xml_float32_variable_t* v) {
    fmi3_xml_float_type_props_t* props = fmi3_xml_find_type_struct_float_props((fmi3_xml_variable_t*)v);
    if(!props) return NULL;
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
    fmi3_int_union_t start;
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v, &start) ? 0 : start.scalar64s;
}

fmi3_string_t fmi3_xml_get_int64_variable_quantity(fmi3_xml_int64_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_int_type_props_t* props =
        (fmi3_xml_int_type_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));
    if(!props) return NULL;
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
    fmi3_int_union_t start;
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v, &start) ? 0 : start.scalar32s;
}

fmi3_string_t fmi3_xml_get_int32_variable_quantity(fmi3_xml_int32_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_int_type_props_t* props =
        (fmi3_xml_int_type_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));
    if(!props) return NULL;
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
    fmi3_int_union_t start;
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v, &start) ? 0 : start.scalar16s;
}

fmi3_string_t fmi3_xml_get_int16_variable_quantity(fmi3_xml_int16_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_int_type_props_t* props =
        (fmi3_xml_int_type_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));
    if(!props) return NULL;
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
    fmi3_int_union_t start;
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v, &start) ? 0 : start.scalar8s;
}

fmi3_string_t fmi3_xml_get_int8_variable_quantity(fmi3_xml_int8_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_int_type_props_t* props =
        (fmi3_xml_int_type_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));
    if(!props) return NULL;
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
    fmi3_int_union_t start;
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v, &start) ? 0 : start.scalar64u;
}

fmi3_string_t fmi3_xml_get_uint64_variable_quantity(fmi3_xml_uint64_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_int_type_props_t* props =
        (fmi3_xml_int_type_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));
    if(!props) return NULL;
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
    fmi3_int_union_t start;
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v, &start) ? 0 : start.scalar32u;
}

fmi3_string_t fmi3_xml_get_uint32_variable_quantity(fmi3_xml_uint32_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_int_type_props_t* props =
        (fmi3_xml_int_type_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));
    if(!props) return NULL;
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
    fmi3_int_union_t start;
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v, &start) ? 0 : start.scalar16u;
}

fmi3_string_t fmi3_xml_get_uint16_variable_quantity(fmi3_xml_uint16_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_int_type_props_t* props =
        (fmi3_xml_int_type_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));
    if(!props) return NULL;
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
    fmi3_int_union_t start;
    return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v, &start) ? 0 : start.scalar8u;
}

fmi3_string_t fmi3_xml_get_uint8_variable_quantity(fmi3_xml_uint8_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_int_type_props_t* props =
        (fmi3_xml_int_type_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));
    if(!props) return NULL;
    return (fmi3_string_t)props->quantity;
}

// -----------------------------------------------------------------------------
// Enum
// -----------------------------------------------------------------------------

fmi3_xml_enum_variable_t* fmi3_xml_get_variable_as_enum(fmi3_xml_variable_t* v){
    if(fmi3_xml_get_variable_base_type(v) == fmi3_base_type_enum)  return (void*)v;
    return 0;
}

fmi3_string_t fmi3_xml_get_enum_variable_quantity(fmi3_xml_enum_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    fmi3_xml_enum_variable_props_t* props = (fmi3_xml_enum_variable_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));
    if(!props) return NULL;
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

int fmi3_xml_get_enum_variable_start(fmi3_xml_enum_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    if(fmi3_xml_get_variable_has_start(vv)) {
        fmi3_xml_int_variable_start_t* start = (fmi3_xml_int_variable_start_t*)(vv->type);
        return start->start.scalar32s;
    }
        return 0;
}

// -----------------------------------------------------------------------------
// String
// -----------------------------------------------------------------------------

fmi3_xml_string_variable_t* fmi3_xml_get_variable_as_string(fmi3_xml_variable_t* v){
    if(fmi3_xml_get_variable_base_type(v) == fmi3_base_type_str)  return (void*)v;
    return 0;
}

const char* fmi3_xml_get_string_variable_start(fmi3_xml_string_variable_t* v){
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    if(fmi3_xml_get_variable_has_start(vv)) {
        fmi3_xml_string_variable_start_t* start = (fmi3_xml_string_variable_start_t*)(vv->type);
        return start->start;
    }
    return 0;
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
        return start->start.scalar32s; /* schema: xs:boolean - 'Integer' type was used before IntXX, so just keeping that now */
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

fmi3_binary_t fmi3_xml_get_binary_variable_start(fmi3_xml_binary_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    if (fmi3_xml_get_variable_has_start(vv)) {
        fmi3_xml_binary_variable_start_t* start = (fmi3_xml_binary_variable_start_t*)(vv->type);
        return start->start; /* TODO: mismatch with return value? */
    }
    return 0;
}

// -----------------------------------------------------------------------------
// Clock
// -----------------------------------------------------------------------------

fmi3_xml_clock_variable_t* fmi3_xml_get_variable_as_clock(fmi3_xml_variable_t* v) {
    if (fmi3_xml_get_variable_base_type(v) == fmi3_base_type_clock) return (void*)v;
    return 0;
}

fmi3_clock_t fmi3_xml_get_clock_variable_start(fmi3_xml_clock_variable_t* v) {
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;
    if (fmi3_xml_get_variable_has_start(vv)) {
        fmi3_xml_clock_variable_start_t* start = (fmi3_xml_clock_variable_start_t*)(vv->type);
        return start->start;
    }
    return 0;
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
        break;
    case fmi3_base_type_int64:    /* fallthrough */
    case fmi3_base_type_int32:    /* fallthrough */
    case fmi3_base_type_int16:    /* fallthrough */
    case fmi3_base_type_int8:     /* fallthrough */
    case fmi3_base_type_uint64:   /* fallthrough */
    case fmi3_base_type_uint32:   /* fallthrough */
    case fmi3_base_type_uint16:   /* fallthrough */
    case fmi3_base_type_uint8:    /* fallthrough */
    case fmi3_base_type_bool:     /* fallthrough */
    case fmi3_base_type_str:      /* fallthrough */
    case fmi3_base_type_enum:     /* fallthrough */
        assert(0); /* TODO: NYI */
    default:
        assert(0); /* impl. error */
        break;
    }

    assert(0); /* impl. error */
    return NULL;
}

void fmi3_xml_variable_free_internals(jm_callbacks* callbacks, fmi3_xml_variable_t* var) {
    if (fmi3_xml_variable_is_array(var)) {
        if (fmi3_xml_get_variable_has_start(var)) {
            callbacks->free(fmi3_xml_get_variable_start_array(var));
        }
        callbacks->free(var->dimensionsArray);
    }
}

/**
 * Returns the default variability of the given Variable base type for the specific causality.
 * 
 * Note: Default values were clarified in FMI 3.0.1.
*/
static fmi3_variability_enu_t fmi3_xml_variable_get_default_variability(fmi3_xml_elm_enu_t elm_id,
        fmi3_causality_enu_t causality)
{
    bool isFloat = (elm_id == fmi3_xml_elmID_Float32Variable
                 || elm_id == fmi3_xml_elmID_Float64Variable);
    // TODO: Also check for structural parameter
    bool isParam = (causality == fmi3_causality_enu_parameter ||
                    causality == fmi3_causality_enu_calculated_parameter);

    return (isFloat && !isParam) ? fmi3_variability_enu_continuous : fmi3_variability_enu_discrete;
}

/**
 * Processes the attributes 'causality', 'variability' and 'initial'.
 * 
 * The attributes are error checked. On success, the parsed variable is updated with them.
 */
static int fmi3_xml_variable_process_attr_causality_variability_initial(fmi3_xml_parser_context_t* context,
        fmi3_xml_variable_t* variable, fmi3_xml_elm_enu_t elm_id)
{
    jm_name_ID_map_t causalityConventionMap[] = {
            {"local",               fmi3_causality_enu_local},
            {"input",               fmi3_causality_enu_input},
            {"output",              fmi3_causality_enu_output},
            {"parameter",           fmi3_causality_enu_parameter},
            {"calculatedParameter", fmi3_causality_enu_calculated_parameter},
            {"independent",         fmi3_causality_enu_independent},
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
    fmi3_causality_enu_t causality;
    fmi3_variability_enu_t variability, defaultVariability;
    fmi3_initial_enu_t initial, defaultInitial, validInitial;

    // Causality:
    if (fmi3_xml_set_attr_enum(context, elm_id, fmi_attr_id_causality, 0, &causality,
            fmi3_causality_enu_local, causalityConventionMap))
    {
        causality = fmi3_causality_enu_local;
    }
    variable->causality = causality;

    // Variability:
    defaultVariability = fmi3_xml_variable_get_default_variability(elm_id, causality);
    if (fmi3_xml_set_attr_enum(context, elm_id, fmi_attr_id_variability, 0, &variability,
            defaultVariability, variabilityConventionMap))
    {
        variability = fmi3_variability_enu_continuous;
    }
    if (!fmi3_is_valid_variability_causality(variability, causality)) {
        fmi3_variability_enu_t bad_variability = variability;
        variability = fmi3_get_default_valid_variability(causality);
        fmi3_xml_parse_error(context,
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
    if (fmi3_xml_set_attr_enum(context, elm_id, fmi_attr_id_initial, 0, &initial,
            defaultInitial,initialConventionMap))
    {
        initial = defaultInitial;
    }
    validInitial = fmi3_get_valid_initial(variability, causality, initial);
    if (validInitial != initial) {
        fmi3_xml_parse_error(context,
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

static int fmi3_xml_variable_process_attr_previous(fmi3_xml_parser_context_t* context,
        fmi3_xml_variable_t* variable, fmi3_xml_elm_enu_t elm_id)
{
    uint32_t previous;
    if (!fmi3_xml_is_attr_defined(context, fmi_attr_id_previous)) {
        return 0;
    }
    else if (fmi3_xml_set_attr_uint32(context, elm_id, fmi_attr_id_previous, 0 /* required */,
            &previous, 0 /* defaultVal */)) {
        return -1;
    }
    /* Store the VR since we cannot access the variable until after parsing all variables. */
    variable->previous.vr = previous;
    variable->hasPrevious = true;
    return 0;
}

static int fmi3_xml_variable_process_attr_multipleset(fmi3_xml_parser_context_t* context,
        fmi3_xml_variable_t* variable, fmi3_xml_elm_enu_t elm_id)
{
    unsigned int multipleSet;
    if (fmi3_xml_set_attr_boolean(context, elm_id, fmi_attr_id_canHandleMultipleSetPerTimeInstant,
            0 /* required */, &multipleSet, 1 /* defaultVal */)) {
        return -1;
    }
    variable->canHandleMultipleSetPerTimeInstant = (char)multipleSet;

    if (variable->causality != fmi3_causality_enu_input && !multipleSet) {
        fmi3_xml_parse_error(context, "Only variables with causality='input' can have canHandleMultipleSetPerTimeInstant=false");
        return -1;
    }
    return 0;
}

static int fmi3_xml_variable_process_attr_clocks(fmi3_xml_parser_context_t* context,
        fmi3_xml_variable_t* variable, fmi3_xml_elm_enu_t elm_id)
{
    // Attribute is optional. Avoid allocating vector unless necessary.
    if (fmi3_xml_peek_attr_str(context, fmi_attr_id_clocks) == NULL) {
        return 0;
    }

    variable->clocks = jm_vector_alloc(fmi3_value_reference_t)(0, 0, context->callbacks);
    if (!variable->clocks) {
        fmi3_xml_parse_fatal(context, "Could not allocate memory");
        return -1;
    }

    if (fmi3_xml_parse_attr_valueref_list(context, elm_id, fmi_attr_id_clocks, 0 /* required */, variable->clocks)) {
        return -1;
    }
    return 0;
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
    int atStartTag = !data;

     /* The real ID of the variable, such as 'Float64Variable' */
    fmi3_xml_elm_enu_t elm_id = atStartTag ? context->currentElemIdStartTag : context->currentElmID;

    if (!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_variable_t* variable;
        fmi3_xml_variable_t dummyV;
        const char* description = 0;
        jm_named_ptr named, *pnamed;
        jm_vector(char)* bufName  = fmi3_xml_reserve_parse_buffer(context, 1, 100);
        jm_vector(char)* bufDescr = fmi3_xml_reserve_parse_buffer(context, 2, 100);
        unsigned int vr;

        if(!bufName || !bufDescr) return -1;

        /* Get vr, name and description */
        if (fmi3_xml_set_attr_uint32(context, elm_id, fmi_attr_id_valueReference, 1, &vr, 0))         return -1;
        if (fmi3_xml_set_attr_string(context, fmi3_xml_elmID_Variable, fmi_attr_id_name, 1, bufName)) return -1;
        if (fmi3_xml_set_attr_string(context, elm_id, fmi_attr_id_description, 0, bufDescr))          return -1;

        /* Return early if undefined variable */
        if (context->skipOneVariableFlag) {
            jm_log_error(context->callbacks,module, "Ignoring variable with undefined vr '%s'", jm_vector_get_itemp(char)(bufName,0));
            return 0;
        }

        /* Get description if exists */
        if (jm_vector_get_size(char)(bufDescr)) {
            description = jm_string_set_put(&md->descriptions, jm_vector_get_itemp(char)(bufDescr,0));
        }

        /* Add Variable ptr to modelDescription obj */
        named.ptr = 0;
        named.name = 0;
        pnamed = jm_vector_push_back(jm_named_ptr)(&md->variablesByName, named);
        if (!pnamed) {
            fmi3_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }

        /* Set p.name and allocate Variable */
        *pnamed = jm_named_alloc_v(bufName, sizeof(fmi3_xml_variable_t), dummyV.name - (char*)&dummyV, context->callbacks);
        if (!pnamed->ptr) {
            fmi3_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }
        variable = pnamed->ptr;

        /* Initialize rest of Variable */
        variable->vr = vr;
        variable->description = description;
        /* Default values */
        variable->type = 0;
        variable->originalIndex = jm_vector_get_size(jm_named_ptr)(&md->variablesByName) - 1;
        variable->derivativeOf = 0;
        variable->previous.variable = NULL;  // Could correspond to a valid vr, why we need the 'hasPrevious' as well.
        variable->hasPrevious = false;
        variable->clocks = NULL;
        variable->aliasKind = fmi3_variable_is_not_alias;
        variable->reinit = 0;
        variable->canHandleMultipleSetPerTimeInstant = 1;
        jm_vector_init(fmi3_xml_dimension_t)(&variable->dimensionsVector, 0, context->callbacks);

        /* Save start value for processing after reading all Dimensions */
        variable->startAttr = fmi3_xml_peek_attr_str(context, fmi_attr_id_start);

        if (fmi3_xml_variable_process_attr_causality_variability_initial(context, variable, elm_id)) return -1;
        if (fmi3_xml_variable_process_attr_previous(context, variable, elm_id))    return -1;
        if (fmi3_xml_variable_process_attr_multipleset(context, variable, elm_id)) return -1;
        if (fmi3_xml_variable_process_attr_clocks(context, variable, elm_id)) return -1;
    }
    else { /* end of xml tag */
        if (context->skipOneVariableFlag) {
            context->skipOneVariableFlag = 0;
        } else {
            /* check that the type for the variable is set */
            fmi3_xml_model_description_t* md = context->modelDescription;
            fmi3_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
            if(!variable->type) {
                jm_log_error(context->callbacks, module, "No variable type element for variable %s. Assuming Float64.", variable->name);

                return fmi3_xml_handle_Float64Variable(context, NULL);
            }

            /* Allocate memory for the resolved dimensions. The main reason to do it here is because we have access to the callbacks. */
            if (fmi3_xml_variable_is_array(variable)) {
                size_t nDims = jm_vector_get_size(fmi3_xml_dimension_t)(&variable->dimensionsVector);

                variable->dimensionsArray = context->callbacks->malloc(nDims * sizeof(unsigned int));
                if (!variable->dimensionsArray) {
                    jm_log_error(context->callbacks, module, "Error: Unable to allocate memory for dimension as array");
                    return -1;
                }
            }
        }
    }
    return 0;
}

int fmi3_xml_get_has_start(fmi3_xml_parser_context_t *context, fmi3_xml_variable_t* variable) {
    int hasStart = fmi3_xml_is_attr_defined(context, fmi_attr_id_start);
    if (!hasStart) {
        if (variable->initial != (char)fmi3_initial_enu_calculated) {
            fmi3_xml_parse_error(context,
                    "Start attribute is required for this causality, variability and initial combination");
        }
    } else {
        /* If initial = calculated, it is not allowed to provide a start value. */
        if (variable->initial == (char)fmi3_initial_enu_calculated) {
            fmi3_xml_parse_error(context, "Start attribute is not allowed for variables with initial='calculated'");
            hasStart = 0;
        }
    }
    return hasStart;
}

static void fmi3_log_error_if_start_required(
    fmi3_xml_parser_context_t *context,
    fmi3_xml_variable_t *variable)
{
    if (variable->causality == fmi3_causality_enu_input) {
        jm_log_error(context->callbacks,
                       "Error: variable %s: start value required for input variables",
                       variable->name);
    } else if (variable->causality == fmi3_causality_enu_parameter) {
        jm_log_error(context->callbacks,
                       "Error: variable %s: start value required for parameter variables",
                       variable->name);
    } else if (variable->variability == fmi3_variability_enu_constant) {
        jm_log_error(context->callbacks,
                       "Error: variable %s: start value required for variables with constant variability",
                       variable->name);
    } else if (variable->initial == fmi3_initial_enu_exact) {
        jm_log_error(context->callbacks,
                       "Error: variable %s: start value required for variables with initial == \"exact\"",
                       variable->name);
    } else if (variable->initial == fmi3_initial_enu_approx) {
        jm_log_error(context->callbacks,
                       "Error: variable %s: start value required for variables with initial == \"approx\"",
                       variable->name);
    }
}

int fmi3_xml_handle_Dimension(fmi3_xml_parser_context_t* context, const char* data) {

    if (context->skipOneVariableFlag) {
        return 0; /* skip nested variable element */
    }

    if (!data) { /* start of tag */

        fmi3_xml_variable_t* currentVar = jm_vector_get_last(jm_named_ptr)(&context->modelDescription->variablesByName).ptr;
        fmi3_xml_dimension_t dim;
        fmi3_xml_attr_enu_t attrId;
        int hasStart;
        int hasVr;

        /* handle attributes*/
        hasStart = fmi3_xml_is_attr_defined(context, fmi_attr_id_start);
        hasVr = fmi3_xml_is_attr_defined(context, fmi_attr_id_valueReference);

        /* error check */
        if ( !(hasStart || hasVr) ) {
            jm_log_error(context->callbacks, "Error parsing Dimension: no attribute 'start' or 'valueReference' found", "");
            return -1;
        } else if (hasStart && hasVr) {
            jm_log_error(context->callbacks, "Error parsing Dimension: mutually exclusive attributes 'start' and 'valueReference' found", "");
            return -1;
        }

        /* set data */
        if (hasStart) {
            dim.has_vr = 0;
            attrId = fmi_attr_id_start;
            if (fmi3_xml_set_attr_uint64(context, fmi3_xml_elmID_Dimension, attrId, 0, &dim.start, 0)) {
                return -1;
            }
        } else if (hasVr) {
            dim.has_vr = 1;
            attrId = fmi_attr_id_valueReference;
            if (fmi3_xml_set_attr_uint32(context, fmi3_xml_elmID_Dimension, attrId, 0, &dim.vr, 0)) {
                return -1;
            }
        }

        /* update parent variable */
        if (!jm_vector_push_back(fmi3_xml_dimension_t)(&currentVar->dimensionsVector, dim)) {
            jm_log_error(context->callbacks, module, "Error: Unable to allocate memory for dimension data (vector alloc failed)");
            return -1;
        }

    } else { /* end of tag */
        /* do nothing */
    }

    return 0;
}

int fmi3_xml_handle_FloatXXVariable(fmi3_xml_parser_context_t* context, const char* data,
        fmi3_xml_float_type_props_t* defaultType,
        fmi3_xml_elm_enu_t elmID, /* ID of the Type (not the Variable) */
        const fmi3_xml_primitive_type_t* primType) {

    fmi3_xml_model_description_t* md;
    fmi3_xml_variable_t* variable;
    fmi3_xml_type_definitions_t* td;
    fmi3_xml_float_type_props_t* type;
    fmi3_bitness_enu_t bitness = primType->bitness;
    int res;

    if (context->skipOneVariableFlag) {
        return 0;
    }

    /* Extract common Variable info */
    res = fmi3_xml_handle_Variable(context, data);
    if (res) return res;

    md = context->modelDescription;
    variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
    td = &md->typeDefinitions;
    type = 0;

    if (!data) { /* start of tag */
        fmi3_xml_variable_type_base_t* declaredType = 0;

        assert(!variable->type);

        /* Get declared type: either a default or user defined */
        declaredType = fmi3_get_declared_type(context, elmID, &defaultType->super);
        if (!declaredType) return -1;

        /* Set type properties */
        {
            int hasUnit = fmi3_xml_is_attr_defined(context, fmi_attr_id_unit)
                       || fmi3_xml_is_attr_defined(context, fmi_attr_id_displayUnit);
            int hasMin  = fmi3_xml_is_attr_defined(context, fmi_attr_id_min);
            int hasMax  = fmi3_xml_is_attr_defined(context, fmi_attr_id_max);
            int hasNom  = fmi3_xml_is_attr_defined(context, fmi_attr_id_nominal);
            int hasQuan = fmi3_xml_is_attr_defined(context, fmi_attr_id_quantity);
            int hasRelQ = fmi3_xml_is_attr_defined(context, fmi_attr_id_relativeQuantity);
            int hasUnb  = fmi3_xml_is_attr_defined(context, fmi_attr_id_unbounded);

            if (hasUnit || hasMin || hasMax || hasNom || hasQuan || hasRelQ || hasUnb) {
                /* create a new type_props that overrides declared type's properties when necessary */

                fmi3_xml_float_type_props_t* dtProps = 0; /* declaredType properties */

                if (declaredType->structKind == fmi3_xml_type_struct_enu_typedef)
                    dtProps = (fmi3_xml_float_type_props_t*)(declaredType->nextLayer);
                else /* default type */
                    dtProps = (fmi3_xml_float_type_props_t*)declaredType;

                fmi3_xml_reserve_parse_buffer(context, 1, 0);
                fmi3_xml_reserve_parse_buffer(context, 2, 0);

                type = fmi3_xml_parse_float_type_properties(context, elmID, defaultType, primType);
                if (!type) {
                    variable->type = &((fmi3_xml_float_type_props_t*)declaredType)->super; /* fallback to default */
                    return -1;
                }

                type->super.nextLayer = declaredType;
                if (!hasUnit) type->displayUnit              = dtProps->displayUnit;
                if (!hasMin)  type->typeMin                  = dtProps->typeMin;
                if (!hasMax)  type->typeMax                  = dtProps->typeMax;
                if (!hasNom)  type->typeNominal              = dtProps->typeNominal;
                if (!hasQuan) type->quantity                 = dtProps->quantity;
                if (!hasRelQ) type->super.isRelativeQuantity = dtProps->super.isRelativeQuantity;
                if (!hasUnb)  type->super.isUnbounded        = dtProps->super.isUnbounded;
            } else {
                type = (fmi3_xml_float_type_props_t*)declaredType;
            }
        }
        variable->type = &type->super;

        /* Handle rest of attributes except start */
        {
            unsigned int derivativeOf;
            unsigned int reinit;

            if (fmi3_xml_set_attr_uint32(context, fmi3_xml_elmID_Float64, fmi_attr_id_derivative, 0, &derivativeOf, 0))
                return -1;
            /* Store the index as a pointer since we cannot access the variables list yet (we are constructing it). */
            variable->derivativeOf = (void*)((char*)NULL + derivativeOf);

            if (fmi3_xml_set_attr_boolean(context, fmi3_xml_elmID_Float64, fmi_attr_id_reinit, 0, &reinit, 0)) /* <xs:attribute name="reinit" type="xs:boolean" use="optional" default="false"> */
                return -1;
            variable->reinit = (char)reinit;

            if (variable->variability != fmi3_variability_enu_continuous && reinit) {
                /* If reinit is true, this variable must be continuous. */
                fmi3_xml_parse_error(context, "The reinit attribute may only be set on continuous-time states.");
                return -1;
            }
        }
        /* TODO: handle this cleanly: */
        /* just read the attribute, so we don't get any warning from it being unused in the buffer... it is saved as startAttr in the variable */
        {
            const char* tmp; /* unused */
            fmi3_xml_get_attr_str(context, fmi3_xml_elmID_Float64, fmi_attr_id_start, 0, &tmp);
        }
    } else { /* end of tag */
        /* set start value */

        /* We must wait until after parsing dimensions, because we can't otherwise know
         * if it's a 1x1 array or a scalar variable just by reading the start value. */
        int hasStart = variable->startAttr != NULL;
        if (hasStart) {
            jm_string startAttr = variable->startAttr;

            fmi3_xml_float_variable_start_t* start = (fmi3_xml_float_variable_start_t*)fmi3_xml_alloc_variable_type_start(td, variable->type, sizeof(fmi3_xml_float_variable_start_t));
            if (!start) {
                fmi3_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            if (fmi3_xml_variable_is_array(variable)) {
                size_t nArr; /* TODO: do something with this, e.g. dimension size verification */
                if (fmi3_xml_set_attr_array(context, elmID, fmi_attr_id_start, 0, (void**)&start->start, &nArr, startAttr, primType)) {
                    return -1;
                }
            } else { /* is scalar */
                /* restore the attribute buffer before it's used in set_attr_float */
                jm_vector_set_item(jm_string)(context->attrBuffer, fmi_attr_id_start, startAttr);

                if (fmi3_xml_set_attr_floatXX(context, elmID, fmi_attr_id_start, 0, &start->start, fmi3_xml_get_type_default_value(bitness), primType)) {
                    return -1;
                }
            }
            variable->type = &start->super;
        } else {
            fmi3_log_error_if_start_required(context, variable);
        }

        /* TODO: verify that dimension size == number of array start attribute values */
    }
    return 0;
}

int fmi3_xml_handle_IntXXVariable(fmi3_xml_parser_context_t* context, const char* data,
        fmi3_xml_int_type_props_t* defaultType,
        fmi3_xml_elm_enu_t elmID, /* ID of the Type (not the Variable) */
        const fmi3_xml_primitive_type_t* primType) {
    int res;

    if(context->skipOneVariableFlag) return 0;

    /* Extract common Variable info */
    res = fmi3_xml_handle_Variable(context, data);
    if (res) return res;

    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_type_definitions_t* td = &md->typeDefinitions;
        fmi3_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
        fmi3_xml_variable_type_base_t * declaredType = 0;
        fmi3_xml_int_type_props_t * type = 0;
        int hasStart;

        declaredType = fmi3_get_declared_type(context, elmID, &defaultType->super) ;

        if(!declaredType) return -1;
        {
            int hasMin = fmi3_xml_is_attr_defined(context,fmi_attr_id_min);
            int hasMax = fmi3_xml_is_attr_defined(context,fmi_attr_id_max);
            int hasQuan =  fmi3_xml_is_attr_defined(context,fmi_attr_id_quantity);
        if( hasMin || hasMax || hasQuan) {
            fmi3_xml_int_type_props_t* dtProps = 0; /* declaredType properties */

            if(declaredType->structKind != fmi3_xml_type_struct_enu_typedef)
                dtProps = (fmi3_xml_int_type_props_t*)declaredType;
            else
                dtProps = (fmi3_xml_int_type_props_t*)(declaredType->nextLayer);
            assert(dtProps->super.structKind == fmi3_xml_type_struct_enu_props);
            fmi3_xml_reserve_parse_buffer(context, 1, 0);
            fmi3_xml_reserve_parse_buffer(context, 2, 0);
            type = fmi3_xml_parse_intXX_type_properties(context, elmID, defaultType, primType);
            if (!type) {
                variable->type = &((fmi3_xml_int_type_props_t*)declaredType)->super; /* fallback to default */
                return -1;
            }
            type->super.nextLayer = declaredType;
            if(!hasMin)  type->typeMin  = dtProps->typeMin;
            if(!hasMax)  type->typeMax  = dtProps->typeMax;
            if(!hasQuan) type->quantity = dtProps->quantity;
        }
        else
            type = (fmi3_xml_int_type_props_t*)declaredType;
        }
        variable->type = &type->super;

        hasStart = fmi3_xml_get_has_start(context, variable);
        if (hasStart) {
            fmi3_xml_int_variable_start_t * start = (fmi3_xml_int_variable_start_t*)fmi3_xml_alloc_variable_type_start(td, &type->super, sizeof(fmi3_xml_int_variable_start_t));
            fmi3_int_union_t defVal;
            if (!start) {
                fmi3_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }

            /*
               This default value should never be used, since the standard does not define it.
               The FMU will hard-code one in the C API and it might not match our (this) default value
               - here I'm just giving a valid value for the type so it's defined on our side at least.
            */
            defVal.scalar64s = 0; /* set the whole bitfield to 0 - this will evaluate to '0' for all intXX types */

            if (fmi3_xml_set_attr_intXX(context, elmID, fmi_attr_id_start, 0, &start->start, &defVal, primType)) {
                jm_log_error(context->callbacks, module, "failed to parse start value for integer");
                return -1;
            }
            variable->type = &start->super;
        } else {
            fmi3_log_error_if_start_required(context, variable);
        }
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

/* Macro for creating handle_TYPEXXVariable function wrappers. */
#define gen_fmi3_xml_handle_TYPEXXVariable(TYPE_NO_SIGN, TYPE, TYPE_LC, BITNESS)                            \
    int fmi3_xml_handle_##TYPE##BITNESS##Variable(fmi3_xml_parser_context_t* context, const char* data) {   \
        return fmi3_xml_handle_##TYPE_NO_SIGN##XXVariable(context, data,                                    \
                &context->modelDescription->typeDefinitions.default##TYPE##BITNESS##Type,                   \
                fmi3_xml_elmID_##TYPE##BITNESS, &PRIMITIVE_TYPES.TYPE_LC##BITNESS);                         \
    }

gen_fmi3_xml_handle_TYPEXXVariable(Float, Float, float, 64)
gen_fmi3_xml_handle_TYPEXXVariable(Float, Float, float, 32)

gen_fmi3_xml_handle_TYPEXXVariable(Int,  Int,  int, 64)
gen_fmi3_xml_handle_TYPEXXVariable(Int,  Int,  int, 32)
gen_fmi3_xml_handle_TYPEXXVariable(Int,  Int,  int, 16)
gen_fmi3_xml_handle_TYPEXXVariable(Int,  Int,  int,  8)
gen_fmi3_xml_handle_TYPEXXVariable(Int, UInt, uint, 64)
gen_fmi3_xml_handle_TYPEXXVariable(Int, UInt, uint, 32)
gen_fmi3_xml_handle_TYPEXXVariable(Int, UInt, uint, 16)
gen_fmi3_xml_handle_TYPEXXVariable(Int, UInt, uint,  8)

int fmi3_xml_handle_BooleanVariable(fmi3_xml_parser_context_t *context, const char* data) {
    int res;

    if(context->skipOneVariableFlag) return 0;

    res = fmi3_xml_handle_Variable(context, data);
    if (res) return res;

    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_type_definitions_t* td = &md->typeDefinitions;
        fmi3_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
        int hasStart;

        assert(!variable->type);

        variable->type = fmi3_get_declared_type(context, fmi3_xml_elmID_Boolean, &td->defaultBooleanType) ;
        if(!variable->type) return -1;

        hasStart = fmi3_xml_get_has_start(context, variable);
        if(hasStart) {
            fmi3_xml_int_variable_start_t * start = (fmi3_xml_int_variable_start_t*)fmi3_xml_alloc_variable_type_start(td, variable->type, sizeof(fmi3_xml_int_variable_start_t));
            if(!start) {
                fmi3_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            /* TODO: something feels very wrong with the casting here - especially considering
               that it's later treating it as an enum, which also expects it to be unsigned (should be
               signed, right?), and it's accessed as a signed (in get_start)
               - just leaving it as it were for now... */
            if (fmi3_xml_set_attr_boolean(context, fmi3_xml_elmID_Boolean, fmi_attr_id_start, 0,
                    (unsigned int*)&start->start.scalar32s, 0)) {
                return -1;
            }
            variable->type = &start->super;
        } else {
            fmi3_log_error_if_start_required(context, variable);
        }
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi3_xml_handle_BinaryVariable(fmi3_xml_parser_context_t* context, const char* data) {
    if (context->skipOneVariableFlag) return 0;
    if (fmi3_xml_handle_Variable(context, data)) return -1;

    if (!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_type_definitions_t* td = &md->typeDefinitions;
        fmi3_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;

        assert(!variable->type);

        variable->type = fmi3_get_declared_type(context, fmi3_xml_elmID_Binary, &td->defaultBinaryType);
        if (!variable->type) return -1;
    }
    else {
        // TODO: Handle start
        return 0;
    }
    return 0;
}

int fmi3_xml_handle_ClockVariable(fmi3_xml_parser_context_t* context, const char* data) {
    if (context->skipOneVariableFlag) return 0;
    if (fmi3_xml_handle_Variable(context, data)) return -1;

    if (!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_type_definitions_t* td = &md->typeDefinitions;
        fmi3_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;

        assert(!variable->type);

        variable->type = fmi3_get_declared_type(context, fmi3_xml_elmID_Binary, &td->defaultBinaryType);
        if (!variable->type) return -1;
    }
    else {
        // TODO: Handle start
        return 0;
    }
    return 0;
}

int fmi3_xml_handle_StringVariable(fmi3_xml_parser_context_t *context, const char* data) {
    if (context->skipOneVariableFlag) return 0;
    if (fmi3_xml_handle_Variable(context, data)) return -1;

    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_type_definitions_t* td = &md->typeDefinitions;
        fmi3_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
        int hasStart;

        assert(!variable->type);

        variable->type = fmi3_get_declared_type(context, fmi3_xml_elmID_String,&td->defaultStringType) ;
        if(!variable->type) return -1;

        hasStart = fmi3_xml_get_has_start(context, variable);
        if(hasStart) {
            jm_vector(char)* bufStartStr = fmi3_xml_reserve_parse_buffer(context,1, 100);
            size_t strlen;
            fmi3_xml_string_variable_start_t* start;
            if(fmi3_xml_set_attr_string(context, fmi3_xml_elmID_String, fmi_attr_id_start, 0, bufStartStr)) return -1;
            strlen = jm_vector_get_size_char(bufStartStr);

            start = (fmi3_xml_string_variable_start_t*)fmi3_xml_alloc_variable_type_start(td, variable->type, sizeof(fmi3_xml_string_variable_start_t) + strlen);

            if(!start) {
                fmi3_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            if (strlen != 0) { /* No need to memcpy empty strings (gives assetion error) */
                memcpy(start->start, jm_vector_get_itemp_char(bufStartStr,0), strlen);
            }
            start->start[strlen] = 0;
            variable->type = &start->super;
        } else {
            fmi3_log_error_if_start_required(context, variable);
        }
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

fmi3_xml_enum_variable_props_t* fmi3_xml_parse_enum_properties(fmi3_xml_parser_context_t* context, fmi3_xml_enum_variable_props_t* declaredType) {

    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_enum_variable_props_t* props = 0;
    fmi3_xml_elm_enu_t elmID = fmi3_xml_elmID_Enumeration;
    const char* quantity = 0;

    jm_vector(char)* bufQuantity = fmi3_xml_reserve_parse_buffer(context,3,100);

    props = (fmi3_xml_enum_variable_props_t*)fmi3_xml_alloc_variable_type_props(&md->typeDefinitions,
           &md->typeDefinitions.defaultEnumType.base.super, sizeof(fmi3_xml_enum_variable_props_t));

    if (!bufQuantity || !props || fmi3_xml_set_attr_string(context, elmID, fmi_attr_id_quantity, 0, bufQuantity))
        return 0;
    if (jm_vector_get_size(char)(bufQuantity))
        quantity = jm_string_set_put(&md->typeDefinitions.quantities, jm_vector_get_itemp(char)(bufQuantity, 0));

    props->quantity = (quantity == 0) ? declaredType->quantity : quantity;

    if (    fmi3_xml_set_attr_int32(context, elmID, fmi_attr_id_min, 0, &props->typeMin, declaredType->typeMin) ||
            fmi3_xml_set_attr_int32(context, elmID, fmi_attr_id_max, 0, &props->typeMax, declaredType->typeMax)) {
        return 0;
    }

    return props;
}

int fmi3_xml_handle_EnumerationVariable(fmi3_xml_parser_context_t *context, const char* data) {
    int res;

    if(context->skipOneVariableFlag) return 0;

    res = fmi3_xml_handle_Variable(context, data);
    if (res) return res;

    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_type_definitions_t* td = &md->typeDefinitions;
        fmi3_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
        fmi3_xml_variable_type_base_t * declaredType = 0;
        fmi3_xml_enum_variable_props_t * type = 0;
        int hasStart;

        assert(!variable->type);

        declaredType = fmi3_get_declared_type(context, fmi3_xml_elmID_Enumeration,&td->defaultEnumType.base.super);

        if(!declaredType) return -1;

        if(
                fmi3_xml_is_attr_defined(context,fmi_attr_id_min) ||
                fmi3_xml_is_attr_defined(context,fmi_attr_id_max) ||
                fmi3_xml_is_attr_defined(context,fmi_attr_id_quantity)
                ) {
            fmi3_xml_enum_variable_props_t* dtProps = 0; /* declaredType properties */

            if(declaredType->structKind != fmi3_xml_type_struct_enu_typedef)
                dtProps = (fmi3_xml_enum_variable_props_t*)declaredType;
            else
                dtProps = (fmi3_xml_enum_variable_props_t*)declaredType->nextLayer;
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

        hasStart = fmi3_xml_get_has_start(context, variable);
        if (hasStart) {
            fmi3_xml_int_variable_start_t* start = (fmi3_xml_int_variable_start_t*)fmi3_xml_alloc_variable_type_start(td, &type->super, sizeof(fmi3_xml_int_variable_start_t));
            if (!start) {
                fmi3_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            if (fmi3_xml_set_attr_int32(context, fmi3_xml_elmID_Enumeration, fmi_attr_id_start, 0, &start->start.scalar32s, 0)) {
                start->start.scalar32s = type->typeMin;
            }
            variable->type = &start->super;
        } else {
            fmi3_log_error_if_start_required(context, variable);
        }
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

static int fmi3_xml_compare_variable_original_index (const void* first, const void* second) {
    size_t a = (*(fmi3_xml_variable_t**)first)->originalIndex;
    size_t b = (*(fmi3_xml_variable_t**)second)->originalIndex;
    if(a < b) return -1;
    if(a > b) return 1;
    return 0;
}

void fmi3_xml_eliminate_bad_alias(fmi3_xml_parser_context_t *context, size_t indexVR) {
    fmi3_xml_model_description_t* md = context->modelDescription;
    jm_vector(jm_voidp)* varByVR = md->variablesByVR;
    fmi3_xml_variable_t* v = (fmi3_xml_variable_t*)jm_vector_get_item(jm_voidp)(varByVR, indexVR);
    fmi3_value_reference_t vr = v->vr;
    fmi3_base_type_enu_t vt = fmi3_xml_get_variable_base_type(v);
    size_t i, n = jm_vector_get_size(jm_voidp)(varByVR);
    for(i = 0; i< n; i++) {
        jm_named_ptr key;
        size_t index;
        v = (fmi3_xml_variable_t*)jm_vector_get_item(jm_voidp)(varByVR, i);
        if((v->vr != vr)||(vt != fmi3_xml_get_variable_base_type(v))) continue;
        jm_vector_remove_item_jm_voidp(varByVR,i);
        n--; i--;
        key.name = v->name;
        index = jm_vector_bsearch_index(jm_named_ptr)(&md->variablesByName, &key, jm_compare_named);
        assert(index <= n);
        jm_vector_remove_item(jm_named_ptr)(&md->variablesByName,index);

        index = jm_vector_bsearch_index(jm_voidp)(md->variablesOrigOrder, (jm_voidp*)&v, fmi3_xml_compare_variable_original_index);
        assert(index <= n);

        jm_vector_remove_item(jm_voidp)(md->variablesOrigOrder,index);

        jm_log_error(context->callbacks, module,"Removing incorrect alias variable '%s'", v->name);
        md->callbacks->free(v);
    }
}

static int fmi3_xml_compare_vr_and_original_index (const void* first, const void* second) {
    int ret = fmi3_xml_compare_vr(first, second);
    if(ret != 0) return ret;

    {
        fmi3_xml_variable_t* a = *(fmi3_xml_variable_t**)first;
        fmi3_xml_variable_t* b = *(fmi3_xml_variable_t**)second;
        ret = a->causality - b->causality;
        if(ret != 0 ) return ret;
        ret = a->variability - b->variability;
        if(ret != 0) return ret;
        {
            size_t ai = a->originalIndex;
            size_t bi = b->originalIndex;
            if(ai > bi) return 1;
            if(ai < bi) return -1;
        }
    }

    return 0;
}

int fmi3_xml_handle_ModelVariables(fmi3_xml_parser_context_t *context, const char* data) {
    if(!data) {
        jm_log_verbose(context->callbacks, module,"Parsing XML element ModelVariables");
        /*  reset handles for the elements that are specific under ModelVariables */
        fmi3_xml_set_element_handle(context, "Float64",     FMI3_XML_ELM_ID(Float64Variable));
        fmi3_xml_set_element_handle(context, "Float32",     FMI3_XML_ELM_ID(Float32Variable));
        fmi3_xml_set_element_handle(context, "Int64",       FMI3_XML_ELM_ID(Int64Variable));
        fmi3_xml_set_element_handle(context, "Int32",       FMI3_XML_ELM_ID(Int32Variable));
        fmi3_xml_set_element_handle(context, "Int16",       FMI3_XML_ELM_ID(Int16Variable));
        fmi3_xml_set_element_handle(context, "Int8",        FMI3_XML_ELM_ID(Int8Variable));
        fmi3_xml_set_element_handle(context, "UInt64",      FMI3_XML_ELM_ID(UInt64Variable));
        fmi3_xml_set_element_handle(context, "UInt32",      FMI3_XML_ELM_ID(UInt32Variable));
        fmi3_xml_set_element_handle(context, "UInt16",      FMI3_XML_ELM_ID(UInt16Variable));
        fmi3_xml_set_element_handle(context, "UInt8",       FMI3_XML_ELM_ID(UInt8Variable));
        fmi3_xml_set_element_handle(context, "Enumeration", FMI3_XML_ELM_ID(EnumerationVariable));
        fmi3_xml_set_element_handle(context, "String",      FMI3_XML_ELM_ID(StringVariable));
        fmi3_xml_set_element_handle(context, "Boolean",     FMI3_XML_ELM_ID(BooleanVariable));
        fmi3_xml_set_element_handle(context, "Binary",      FMI3_XML_ELM_ID(BinaryVariable));
        fmi3_xml_set_element_handle(context, "Clock",       FMI3_XML_ELM_ID(ClockVariable));
        fmi3_xml_set_element_handle(context, "Tool",        FMI3_XML_ELM_ID(VariableTool));
    }
    else {
         /* postprocess variable list */

        fmi3_xml_model_description_t* md = context->modelDescription;
        jm_vector(jm_voidp)* varByVR;
        size_t i, numvar;

        numvar = jm_vector_get_size(jm_named_ptr)(&md->variablesByName);

        /* store the list of vars in original order */
        {
            size_t size = jm_vector_get_size(jm_named_ptr)(&md->variablesByName);
            md->variablesOrigOrder = jm_vector_alloc(jm_voidp)(size,size,md->callbacks);
            if(md->variablesOrigOrder) {
                size_t i;
                for(i= 0; i < size; ++i) {
                    jm_vector_set_item(jm_voidp)(md->variablesOrigOrder, i, jm_vector_get_item(jm_named_ptr)(&md->variablesByName,i).ptr);
                }
            }
        }

        /* sort the variables by names */
        jm_vector_qsort(jm_named_ptr)(&md->variablesByName,jm_compare_named);

        /* create VR index */
        md->status = fmi3_xml_model_description_enu_ok;
        {
            size_t size = jm_vector_get_size(jm_named_ptr)(&md->variablesByName);
            md->variablesByVR = jm_vector_alloc(jm_voidp)(size,size,md->callbacks);
            if(md->variablesByVR) {
                size_t i;
                for(i= 0; i < size; ++i) {
                    jm_vector_set_item(jm_voidp)(md->variablesByVR, i, jm_vector_get_item(jm_named_ptr)(&md->variablesByName,i).ptr);
                }
            }
        }

        /* look up actual pointers for the derivativeOf and previous fields in variablesOrigOrder */
        {
            size_t size = jm_vector_get_size(jm_voidp)(md->variablesOrigOrder);
            size_t k;
            for (k=0; k < size; k++) {
                fmi3_xml_variable_t *variable = jm_vector_get_item(jm_voidp)(md->variablesOrigOrder, k);

                if (variable->derivativeOf) {
                    /* Retrieve index that was stored as a pointer */
                    size_t index = (char*)variable->derivativeOf - (char *)NULL;
                    /* Convert from one- to zero-based indexing */
                    index--;
                    /* Ok to just check upper bound since index is unsigned. */
                    if (index >= size) {
                        fmi3_xml_parse_error(context, "The 'derivative' attribute must have a value "
                                                      "between 1 and the number of model variables.");
                        /* todo: free allocated memory? */
                        return -1;
                    }
                    variable->derivativeOf = (fmi3_xml_variable_t*)jm_vector_get_item(jm_voidp)(md->variablesOrigOrder, index);
                }
                if (variable->hasPrevious) {
                    // Resolve VR to variable.
                    
                    fmi3_value_reference_t vr = variable->previous.vr;
                    variable->previous.variable =
                            fmi3_xml_get_variable_by_vr(md, variable->type->baseType, vr);
                    if (!variable->previous.variable) {
                        fmi3_xml_parse_error(context, "The valueReference in previous=\"%" PRIu32 "\" "
                                                      "did not resolve to any variable.", vr);
                        /* todo: free allocated memory? */
                        return -1;
                    }
                }
            }
        }

        md->status = fmi3_xml_model_description_enu_empty;
        if(!md->variablesByVR || !md->variablesOrigOrder) {
            fmi3_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }
        varByVR = md->variablesByVR;
        jm_vector_qsort(jm_voidp)(varByVR, fmi3_xml_compare_vr_and_original_index);

        numvar = jm_vector_get_size(jm_voidp)(varByVR);

        if(numvar > 1){
            int foundBadAlias;

            jm_log_verbose(context->callbacks, module,"Building alias index");
            do {
                fmi3_xml_variable_t* a = (fmi3_xml_variable_t*)jm_vector_get_item(jm_voidp)(varByVR, 0);
                int startPresent = fmi3_xml_get_variable_has_start(a);
                int isConstant = (fmi3_xml_get_variability(a) == fmi3_variability_enu_constant);
                a->aliasKind = fmi3_variable_is_not_alias;

                foundBadAlias = 0;

                for(i = 1; i< numvar; i++) {
                    fmi3_xml_variable_t* b = (fmi3_xml_variable_t*)jm_vector_get_item(jm_voidp)(varByVR, i);
                    int b_startPresent = fmi3_xml_get_variable_has_start(b);
                    int b_isConstant = (fmi3_xml_get_variability(b) == fmi3_variability_enu_constant);
                    if((fmi3_xml_get_variable_base_type(a) == fmi3_xml_get_variable_base_type(b))
                            && (a->vr == b->vr)) {
                            /* an alias */
                            jm_log_verbose(context->callbacks,module,"Variables %s and %s reference the same vr %u. Marking '%s' as alias.",
                                                  a->name, b->name, b->vr, b->name);
                            b->aliasKind = fmi3_variable_is_alias;

                            if(!isConstant != !b_isConstant) {
                                jm_log_error(context->callbacks,module,
                                "Only constants can be aliases with constants (variables: %s and %s)",
                                    a->name, b->name);
                                fmi3_xml_eliminate_bad_alias(context,i);
                                numvar = jm_vector_get_size(jm_voidp)(varByVR);
                                foundBadAlias = 1;
                                break;
                            } else if (isConstant) {
                                if (!startPresent  || !b_startPresent) {
                                    jm_log_error(context->callbacks,module,
                                        "Constants in alias set must all have start attributes (variables: %s and %s)",
                                        a->name, b->name);
                                    fmi3_xml_eliminate_bad_alias(context,i);
                                    numvar = jm_vector_get_size(jm_voidp)(varByVR);
                                    foundBadAlias = 1;
                                    break;
                                }
                                /* TODO: Check that both start values are the same */
                            } else if(startPresent && b_startPresent) {
                                jm_log_error(context->callbacks,module,
                                    "Only one variable among non constant aliases is allowed to have start attribute (variables: %s and %s) %d, %d, const enum value: %d",
                                        a->name, b->name, fmi3_xml_get_variability(a), fmi3_xml_get_variability(b), fmi3_variability_enu_constant);
                                fmi3_xml_eliminate_bad_alias(context,i);
                                numvar = jm_vector_get_size(jm_voidp)(varByVR);
                                foundBadAlias = 1;
                                break;
                            }
                            if(b_startPresent) {
                                startPresent = 1;
                                a = b;
                            }
                    }
                    else {
                        b->aliasKind = fmi3_variable_is_not_alias;
                        startPresent = b_startPresent;
                        isConstant = b_isConstant;
                        a = b;
                    }
                }
            } while(foundBadAlias);
        }

        /* TODO: validate variables from Dimension VRs */
        {
            /*
                TODO: create a new vector that contains all VRs from dimensions
                from VR, find all variables and verify that they are integers with (causality == structuralParameter || variability == constant)
            */
        }

        numvar = jm_vector_get_size(jm_named_ptr)(&md->variablesByName);

        /* might give out a warning if(data[0] != 0) */
    }
    return 0;
}
