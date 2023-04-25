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



/** \file fmi3_import_variable.c
*  \brief Methods to handle fmi3_import_variable_t.
*/

#include "FMI3/fmi3_import_variable.h"
#include "fmi3_import_impl.h"
#include "fmi3_import_variable_list_impl.h"
#include "FMI3/fmi3_import_dimension.h"
#include "FMI3/fmi3_import_dimension_list.h"

fmi3_import_variable_t* fmi3_import_get_variable_by_name(fmi3_import_t* fmu, const char* name) {
    return fmi3_xml_get_variable_by_name(fmu->md, name);
}

fmi3_import_variable_t* fmi3_import_get_variable_by_vr(fmi3_import_t* fmu, fmi3_base_type_enu_t baseType, fmi3_value_reference_t vr) {
    return fmi3_xml_get_variable_by_vr(fmu->md, baseType, vr);
}

const char* fmi3_import_get_variable_name(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_name(v);
}

const char* fmi3_import_get_variable_description(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_description(v);
}

fmi3_value_reference_t fmi3_import_get_variable_vr(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_vr(v);
}

/**
 * For scalar variable gives the type definition if present.
 */
fmi3_import_variable_typedef_t* fmi3_import_get_variable_declared_type(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_declared_type(v);
}

fmi3_base_type_enu_t fmi3_import_get_variable_base_type(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_base_type(v);
}

fmi3_import_dimension_list_t* fmi3_import_get_variable_dimension_list(fmi3_import_t* fmu, fmi3_import_variable_t* v) {
    return fmi3_import_alloc_dimension_list(fmu, v);
}

int fmi3_import_variable_is_array(fmi3_import_variable_t* v) {
    return fmi3_xml_variable_is_array(v);
}

int fmi3_import_get_variable_has_start(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_has_start(v);
}

fmi3_variability_enu_t fmi3_import_get_variability(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variability(v);
}

fmi3_causality_enu_t fmi3_import_get_causality(fmi3_import_variable_t* v) {
    return fmi3_xml_get_causality(v);
}

fmi3_initial_enu_t fmi3_import_get_initial(fmi3_import_variable_t* v) {
    return fmi3_xml_get_initial(v);
}

fmi3_import_variable_t* fmi3_import_get_previous(fmi3_import_variable_t* v) {
    return fmi3_xml_get_previous(v);
}

fmi3_boolean_t fmi3_import_get_canHandleMultipleSetPerTimeInstant(fmi3_import_variable_t* v) {
    return fmi3_xml_get_canHandleMultipleSetPerTimeInstant(v);
}

fmi3_import_variable_list_t* fmi3_import_get_variable_clocks(fmi3_import_t* fmu, fmi3_import_variable_t* v) {
    fmi3_import_variable_list_t* list = fmi3_import_alloc_variable_list(fmu, 0);
    if (fmi3_xml_get_variable_clocks(fmu->md, v, &list->variables) != jm_status_success) {
        fmi3_import_free_variable_list(list);
        return NULL;
    }
    return list;
}

// -----------------------------------------------------------------------------
// Float64
// -----------------------------------------------------------------------------

fmi3_import_float64_variable_t* fmi3_import_get_variable_as_float64(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_as_float64(v);
}

fmi3_float64_t fmi3_import_get_float64_variable_min(fmi3_import_float64_variable_t* v) {
    return fmi3_xml_get_float64_variable_min(v);
}

fmi3_float64_t fmi3_import_get_float64_variable_max(fmi3_import_float64_variable_t* v) {
    return fmi3_xml_get_float64_variable_max(v);
}

fmi3_float64_t fmi3_import_get_float64_variable_nominal(fmi3_import_float64_variable_t* v) {
    return fmi3_xml_get_float64_variable_nominal(v);
}

fmi3_float64_t fmi3_import_get_float64_variable_start(fmi3_import_float64_variable_t* v) {
    return fmi3_xml_get_float64_variable_start(v);
}

fmi3_float64_t* fmi3_import_get_float64_variable_start_array(fmi3_import_float64_variable_t* v) {
    return fmi3_xml_get_float64_variable_start_array(v);
}

fmi3_boolean_t fmi3_import_get_float64_variable_reinit(fmi3_import_float64_variable_t* v) {
    return fmi3_xml_get_float64_variable_reinit(v);
}

fmi3_import_float64_variable_t* fmi3_import_get_float64_variable_derivative_of(fmi3_import_float64_variable_t* v) {
    return fmi3_xml_get_float64_variable_derivative_of(v);
}

fmi3_import_unit_t* fmi3_import_get_float64_variable_unit(fmi3_import_float64_variable_t* v) {
    return fmi3_xml_get_float64_variable_unit(v);
}

fmi3_import_display_unit_t* fmi3_import_get_float64_variable_display_unit(fmi3_import_float64_variable_t* v) {
    return fmi3_xml_get_float64_variable_display_unit(v);
}

fmi3_string_t fmi3_import_get_float64_variable_quantity(fmi3_import_float64_variable_t* v) {
    return fmi3_xml_get_float64_variable_quantity(v);
}

fmi3_boolean_t fmi3_import_get_float64_variable_unbounded(fmi3_import_float64_variable_t* v) {
    return fmi3_xml_get_float64_variable_unbounded(v);
}

fmi3_boolean_t fmi3_import_get_float64_variable_relative_quantity(fmi3_import_float64_variable_t* v) {
    return fmi3_xml_get_float64_variable_relative_quantity(v);
}

// -----------------------------------------------------------------------------
// Float32
// -----------------------------------------------------------------------------

fmi3_import_float32_variable_t* fmi3_import_get_variable_as_float32(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_as_float32(v);
}

fmi3_float32_t fmi3_import_get_float32_variable_min(fmi3_import_float32_variable_t* v) {
    return fmi3_xml_get_float32_variable_min(v);
}

fmi3_float32_t fmi3_import_get_float32_variable_max(fmi3_import_float32_variable_t* v) {
    return fmi3_xml_get_float32_variable_max(v);
}

fmi3_float32_t fmi3_import_get_float32_variable_nominal(fmi3_import_float32_variable_t* v) {
    return fmi3_xml_get_float32_variable_nominal(v);
}

fmi3_float32_t fmi3_import_get_float32_variable_start(fmi3_import_float32_variable_t* v) {
    return fmi3_xml_get_float32_variable_start(v);
}

fmi3_float32_t* fmi3_import_get_float32_variable_start_array(fmi3_import_float32_variable_t* v) {
    return fmi3_xml_get_float32_variable_start_array(v);
}

fmi3_boolean_t fmi3_import_get_float32_variable_reinit(fmi3_import_float32_variable_t* v) {
    return fmi3_xml_get_float32_variable_reinit(v);
}

fmi3_import_float32_variable_t* fmi3_import_get_float32_variable_derivative_of(fmi3_import_float32_variable_t* v) {
    return fmi3_xml_get_float32_variable_derivative_of(v);
}

fmi3_import_unit_t* fmi3_import_get_float32_variable_unit(fmi3_import_float32_variable_t* v) {
    return fmi3_xml_get_float32_variable_unit(v);
}

fmi3_import_display_unit_t* fmi3_import_get_float32_variable_display_unit(fmi3_import_float32_variable_t* v) {
    return fmi3_xml_get_float32_variable_display_unit(v);
}

fmi3_string_t fmi3_import_get_float32_variable_quantity(fmi3_import_float32_variable_t* v) {
    return fmi3_xml_get_float32_variable_quantity(v);
}

fmi3_boolean_t fmi3_import_get_float32_variable_unbounded(fmi3_import_float32_variable_t* v) {
    return fmi3_xml_get_float32_variable_unbounded(v);
}

fmi3_boolean_t fmi3_import_get_float32_variable_relative_quantity(fmi3_import_float32_variable_t* v) {
    return fmi3_xml_get_float32_variable_relative_quantity(v);
}

// -----------------------------------------------------------------------------
// Int64
// -----------------------------------------------------------------------------

fmi3_import_int64_variable_t* fmi3_import_get_variable_as_int64(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_as_int64(v);
}

fmi3_int64_t fmi3_import_get_int64_variable_min(fmi3_import_int64_variable_t* v) {
    return fmi3_xml_get_int64_variable_min(v);
}

fmi3_int64_t fmi3_import_get_int64_variable_max(fmi3_import_int64_variable_t* v) {
    return fmi3_xml_get_int64_variable_max(v);
}

fmi3_int64_t fmi3_import_get_int64_variable_start(fmi3_import_int64_variable_t* v) {
    return fmi3_xml_get_int64_variable_start(v);
}

fmi3_int64_t* fmi3_import_get_int64_variable_start_array(fmi3_import_int64_variable_t* v) {
    return fmi3_xml_get_int64_variable_start_array(v);
}

fmi3_string_t fmi3_import_get_int64_variable_quantity(fmi3_import_int64_variable_t* v) {
    return fmi3_xml_get_int64_variable_quantity(v);
}

// ----------------------------------------------------------------------------
// Int32                                                                     
// ----------------------------------------------------------------------------

fmi3_import_int32_variable_t* fmi3_import_get_variable_as_int32(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_as_int32(v);
}

fmi3_int32_t fmi3_import_get_int32_variable_min(fmi3_import_int32_variable_t* v) {
    return fmi3_xml_get_int32_variable_min(v);
}

fmi3_int32_t fmi3_import_get_int32_variable_max(fmi3_import_int32_variable_t* v) {
    return fmi3_xml_get_int32_variable_max(v);
}

fmi3_int32_t fmi3_import_get_int32_variable_start(fmi3_import_int32_variable_t* v) {
    return fmi3_xml_get_int32_variable_start(v);
}

fmi3_int32_t* fmi3_import_get_int32_variable_start_array(fmi3_import_int32_variable_t* v) {
    return fmi3_xml_get_int32_variable_start_array(v);
}

fmi3_string_t fmi3_import_get_int32_variable_quantity(fmi3_import_int32_variable_t* v) {
    return fmi3_xml_get_int32_variable_quantity(v);
}

// -----------------------------------------------------------------------------
// Int16
// -----------------------------------------------------------------------------

fmi3_import_int16_variable_t* fmi3_import_get_variable_as_int16(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_as_int16(v);
}

fmi3_int16_t fmi3_import_get_int16_variable_min(fmi3_import_int16_variable_t* v) {
    return fmi3_xml_get_int16_variable_min(v);
}

fmi3_int16_t fmi3_import_get_int16_variable_max(fmi3_import_int16_variable_t* v) {
    return fmi3_xml_get_int16_variable_max(v);
}

fmi3_int16_t fmi3_import_get_int16_variable_start(fmi3_import_int16_variable_t* v) {
    return fmi3_xml_get_int16_variable_start(v);
}

fmi3_int16_t* fmi3_import_get_int16_variable_start_array(fmi3_import_int16_variable_t* v) {
    return fmi3_xml_get_int16_variable_start_array(v);
}

fmi3_string_t fmi3_import_get_int16_variable_quantity(fmi3_import_int16_variable_t* v) {
    return fmi3_xml_get_int16_variable_quantity(v);
}

// -----------------------------------------------------------------------------
// Int8
// -----------------------------------------------------------------------------

fmi3_import_int8_variable_t* fmi3_import_get_variable_as_int8(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_as_int8(v);
}

fmi3_int8_t fmi3_import_get_int8_variable_min(fmi3_import_int8_variable_t* v) {
    return fmi3_xml_get_int8_variable_min(v);
}

fmi3_int8_t fmi3_import_get_int8_variable_max(fmi3_import_int8_variable_t* v) {
    return fmi3_xml_get_int8_variable_max(v);
}

fmi3_int8_t fmi3_import_get_int8_variable_start(fmi3_import_int8_variable_t* v) {
    return fmi3_xml_get_int8_variable_start(v);
}

fmi3_int8_t* fmi3_import_get_int8_variable_start_array(fmi3_import_int8_variable_t* v) {
    return fmi3_xml_get_int8_variable_start_array(v);
}

fmi3_string_t fmi3_import_get_int8_variable_quantity(fmi3_import_int8_variable_t* v) {
    return fmi3_xml_get_int8_variable_quantity(v);
}

// -----------------------------------------------------------------------------
// UInt64
// -----------------------------------------------------------------------------

fmi3_import_uint64_variable_t* fmi3_import_get_variable_as_uint64(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_as_uint64(v);
}

fmi3_uint64_t fmi3_import_get_uint64_variable_min(fmi3_import_uint64_variable_t* v) {
    return fmi3_xml_get_uint64_variable_min(v);
}

fmi3_uint64_t fmi3_import_get_uint64_variable_max(fmi3_import_uint64_variable_t* v) {
    return fmi3_xml_get_uint64_variable_max(v);
}

fmi3_uint64_t fmi3_import_get_uint64_variable_start(fmi3_import_uint64_variable_t* v) {
    return fmi3_xml_get_uint64_variable_start(v);
}

fmi3_string_t fmi3_import_get_uint64_variable_quantity(fmi3_import_uint64_variable_t* v) {
    return fmi3_xml_get_uint64_variable_quantity(v);
}

// -----------------------------------------------------------------------------
// UInt32
// -----------------------------------------------------------------------------

fmi3_import_uint32_variable_t* fmi3_import_get_variable_as_uint32(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_as_uint32(v);
}

fmi3_uint32_t fmi3_import_get_uint32_variable_min(fmi3_import_uint32_variable_t* v) {
    return fmi3_xml_get_uint32_variable_min(v);
}

fmi3_uint32_t fmi3_import_get_uint32_variable_max(fmi3_import_uint32_variable_t* v) {
    return fmi3_xml_get_uint32_variable_max(v);
}

fmi3_uint32_t fmi3_import_get_uint32_variable_start(fmi3_import_uint32_variable_t* v) {
    return fmi3_xml_get_uint32_variable_start(v);
}

fmi3_string_t fmi3_import_get_uint32_variable_quantity(fmi3_import_uint32_variable_t* v) {
    return fmi3_xml_get_uint32_variable_quantity(v);
}

// -----------------------------------------------------------------------------
// UInt16
// -----------------------------------------------------------------------------

fmi3_import_uint16_variable_t* fmi3_import_get_variable_as_uint16(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_as_uint16(v);
}

fmi3_uint16_t fmi3_import_get_uint16_variable_min(fmi3_import_uint16_variable_t* v) {
    return fmi3_xml_get_uint16_variable_min(v);
}

fmi3_uint16_t fmi3_import_get_uint16_variable_max(fmi3_import_uint16_variable_t* v) {
    return fmi3_xml_get_uint16_variable_max(v);
}

fmi3_uint16_t fmi3_import_get_uint16_variable_start(fmi3_import_uint16_variable_t* v) {
    return fmi3_xml_get_uint16_variable_start(v);
}

fmi3_string_t fmi3_import_get_uint16_variable_quantity(fmi3_import_uint16_variable_t* v) {
    return fmi3_xml_get_uint16_variable_quantity(v);
}

// -----------------------------------------------------------------------------
// UInt8
// -----------------------------------------------------------------------------

fmi3_import_uint8_variable_t* fmi3_import_get_variable_as_uint8(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_as_uint8(v);
}

fmi3_uint8_t fmi3_import_get_uint8_variable_min(fmi3_import_uint8_variable_t* v) {
    return fmi3_xml_get_uint8_variable_min(v);
}

fmi3_uint8_t fmi3_import_get_uint8_variable_max(fmi3_import_uint8_variable_t* v) {
    return fmi3_xml_get_uint8_variable_max(v);
}

fmi3_uint8_t fmi3_import_get_uint8_variable_start(fmi3_import_uint8_variable_t* v) {
    return fmi3_xml_get_uint8_variable_start(v);
}

fmi3_string_t fmi3_import_get_uint8_variable_quantity(fmi3_import_uint8_variable_t* v) {
    return fmi3_xml_get_uint8_variable_quantity(v);
}

// -----------------------------------------------------------------------------
// String
// -----------------------------------------------------------------------------

fmi3_import_string_variable_t* fmi3_import_get_variable_as_string(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_as_string(v);
}

const char* fmi3_import_get_string_variable_start(fmi3_import_string_variable_t* v) {
    return fmi3_xml_get_string_variable_start(v);
}

// -----------------------------------------------------------------------------
// Boolean
// -----------------------------------------------------------------------------

fmi3_import_bool_variable_t* fmi3_import_get_variable_as_boolean(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_as_boolean(v);
}

fmi3_boolean_t fmi3_import_get_boolean_variable_start(fmi3_import_bool_variable_t* v){
    return fmi3_xml_get_boolean_variable_start(v);
}

// -----------------------------------------------------------------------------
// Binary
// -----------------------------------------------------------------------------

fmi3_import_binary_variable_t* fmi3_import_get_variable_as_binary(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_as_binary(v);
}

size_t fmi3_import_get_binary_variable_start_size(fmi3_import_binary_variable_t* v) {
    return fmi3_xml_get_binary_variable_start_size(v);
}

fmi3_binary_t fmi3_import_get_binary_variable_start(fmi3_import_binary_variable_t* v) {
    return fmi3_xml_get_binary_variable_start(v);
}

fmi3_string_t fmi3_import_get_binary_variable_mime_type(fmi3_import_binary_variable_t* v) {
    return fmi3_xml_get_binary_variable_mime_type(v);
}

size_t fmi3_import_get_binary_variable_max_size(fmi3_import_binary_variable_t* v) {
    return fmi3_xml_get_binary_variable_max_size(v);
}

// -----------------------------------------------------------------------------
// Clock
// -----------------------------------------------------------------------------

fmi3_import_clock_variable_t* fmi3_import_get_variable_as_clock(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_as_clock(v);
}

fmi3_boolean_t fmi3_import_get_clock_variable_can_be_deactivated(fmi3_import_clock_variable_t* v) {
    return fmi3_xml_get_clock_variable_can_be_deactivated(v);
}

fmi3_uint32_t fmi3_import_get_clock_variable_priority(fmi3_import_clock_variable_t* v) {
    return fmi3_xml_get_clock_variable_priority(v);
}

fmi3_interval_variability_enu_t fmi3_import_get_clock_variable_interval_variability(fmi3_import_clock_variable_t* v) {
    return fmi3_xml_get_clock_variable_interval_variability(v);
} 

fmi3_float32_t fmi3_import_get_clock_variable_interval_decimal(fmi3_import_clock_variable_t* v) {
    return fmi3_xml_get_clock_variable_interval_decimal(v);
}

fmi3_float32_t fmi3_import_get_clock_variable_shift_decimal(fmi3_import_clock_variable_t* v) {
    return fmi3_xml_get_clock_variable_shift_decimal(v);
}

fmi3_boolean_t fmi3_import_get_clock_variable_supports_fraction(fmi3_import_clock_variable_t* v) {
    return fmi3_xml_get_clock_variable_supports_fraction(v);
}

fmi3_uint64_t fmi3_import_get_clock_variable_resolution(fmi3_import_clock_variable_t* v) {
    return fmi3_xml_get_clock_variable_resolution(v);
}

fmi3_uint64_t fmi3_import_get_clock_variable_interval_counter(fmi3_import_clock_variable_t* v) {
    return fmi3_xml_get_clock_variable_interval_counter(v);
}

fmi3_uint64_t fmi3_import_get_clock_variable_shift_counter(fmi3_import_clock_variable_t* v) {
    return fmi3_xml_get_clock_variable_shift_counter(v);
}


// -----------------------------------------------------------------------------
// Enum
// -----------------------------------------------------------------------------

fmi3_import_enum_variable_t* fmi3_import_get_variable_as_enum(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_as_enum(v);
}

fmi3_string_t fmi3_import_get_enum_variable_quantity(fmi3_import_enum_variable_t* v) {
    return fmi3_xml_get_enum_variable_quantity(v);
}

int fmi3_import_get_enum_variable_start(fmi3_import_enum_variable_t* v) {
    return fmi3_xml_get_enum_variable_start(v);
}

int fmi3_import_get_enum_variable_min(fmi3_import_enum_variable_t* v) {
    return fmi3_xml_get_enum_variable_min(v);
}

int fmi3_import_get_enum_variable_max(fmi3_import_enum_variable_t* v) {
    return fmi3_xml_get_enum_variable_max(v);
}

// -----------------------------------------------------------------------------

fmi3_variable_alias_kind_enu_t fmi3_import_get_variable_alias_kind(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_alias_kind(v);
}

fmi3_import_variable_t* fmi3_import_get_variable_alias_base(fmi3_import_t* fmu,fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_alias_base(fmu->md, v);
}

/**
 *  Return the list of all the variables aliased to the given one (including the base one).
 *  The list is ordered: base variable, aliases, negated aliases.
 */
fmi3_import_variable_list_t* fmi3_import_get_variable_aliases(fmi3_import_t* fmu, fmi3_import_variable_t* v) {
    fmi3_import_variable_list_t* list = fmi3_import_alloc_variable_list(fmu, 0);
    if(fmi3_xml_get_variable_aliases(fmu->md, v, &list->variables) != jm_status_success) {
        fmi3_import_free_variable_list(list);
        return 0;
    }
    return list;
}

size_t fmi3_import_get_variable_original_order(fmi3_import_variable_t* v) {
    return fmi3_xml_get_variable_original_order((fmi3_xml_variable_t*)v);
}
