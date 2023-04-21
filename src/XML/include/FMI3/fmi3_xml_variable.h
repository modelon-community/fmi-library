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



/** \file fmi3_xml_variable.h
*  \brief Public interface to the FMI XML C-library. Handling of model variables.
*/

#ifndef FMI3_XML_VARIABLE_H_
#define FMI3_XML_VARIABLE_H_

#include "fmi3_xml_model_description.h"
#include "fmi3_xml_type.h"
#include "fmi3_xml_dimension.h"

#ifdef __cplusplus
extern "C" {
#endif
        /**
    \addtogroup fmi3_xml
    @{
    \addtogroup fmi3_xml_variables Functions for handling variable definitions.
    @}
    \addtogroup fmi3_xml_variables Functions for handling variable definitions.
    @{
    */
const char* fmi3_xml_get_variable_name(fmi3_xml_variable_t*);
const char* fmi3_xml_get_variable_description(fmi3_xml_variable_t*);

size_t fmi3_xml_get_variable_original_order(fmi3_xml_variable_t* v);

fmi3_value_reference_t fmi3_xml_get_variable_vr(fmi3_xml_variable_t*);

/*
    For scalar variable gives the type definition is present
*/
fmi3_xml_variable_typedef_t* fmi3_xml_get_variable_declared_type(fmi3_xml_variable_t*);
fmi3_base_type_enu_t fmi3_xml_get_variable_base_type(fmi3_xml_variable_t*);

fmi3_float64_t* fmi3_xml_get_float64_variable_start_array(fmi3_xml_float64_variable_t* v);
fmi3_float32_t* fmi3_xml_get_float32_variable_start_array(fmi3_xml_float32_variable_t* v);

jm_vector(fmi3_xml_dimension_t)* fmi3_xml_get_variable_dimension_vector(fmi3_xml_variable_t* v);

int fmi3_xml_variable_is_array(fmi3_xml_variable_t* v);

int fmi3_xml_get_variable_has_start(fmi3_xml_variable_t*);

fmi3_variability_enu_t fmi3_xml_get_variability(fmi3_xml_variable_t*);
fmi3_causality_enu_t fmi3_xml_get_causality(fmi3_xml_variable_t*);
fmi3_initial_enu_t fmi3_xml_get_initial(fmi3_xml_variable_t*);

fmi3_xml_variable_t* fmi3_xml_get_previous(fmi3_xml_variable_t* v);
fmi3_boolean_t fmi3_xml_get_canHandleMultipleSetPerTimeInstant(fmi3_xml_variable_t* v);
jm_status_enu_t fmi3_xml_get_variable_clocks(fmi3_xml_model_description_t* md, fmi3_xml_variable_t* v, jm_vector(jm_voidp)* list);

fmi3_xml_float64_variable_t* fmi3_xml_get_variable_as_float64(fmi3_xml_variable_t*);
fmi3_xml_float64_variable_t* fmi3_xml_get_float64_variable_derivative_of (fmi3_xml_float64_variable_t* v);
fmi3_boolean_t fmi3_xml_get_float64_variable_reinit (fmi3_xml_float64_variable_t* v);
fmi3_xml_unit_t* fmi3_xml_get_float64_variable_unit (fmi3_xml_float64_variable_t* v);
fmi3_xml_display_unit_t* fmi3_xml_get_float64_variable_display_unit (fmi3_xml_float64_variable_t* v);
fmi3_float64_t fmi3_xml_get_float64_variable_start (fmi3_xml_float64_variable_t* v);
fmi3_float64_t fmi3_xml_get_float64_variable_min (fmi3_xml_float64_variable_t* v);
fmi3_float64_t fmi3_xml_get_float64_variable_max (fmi3_xml_float64_variable_t* v);
fmi3_float64_t fmi3_xml_get_float64_variable_nominal (fmi3_xml_float64_variable_t* v);
fmi3_string_t fmi3_xml_get_float64_variable_quantity (fmi3_xml_float64_variable_t* v);
fmi3_boolean_t fmi3_xml_get_float64_variable_unbounded (fmi3_xml_float64_variable_t* v);
fmi3_boolean_t fmi3_xml_get_float64_variable_relative_quantity (fmi3_xml_float64_variable_t* v);

fmi3_xml_float32_variable_t* fmi3_xml_get_variable_as_float32(fmi3_xml_variable_t*);
fmi3_xml_float32_variable_t* fmi3_xml_get_float32_variable_derivative_of (fmi3_xml_float32_variable_t* v);
fmi3_boolean_t fmi3_xml_get_float32_variable_reinit (fmi3_xml_float32_variable_t* v);
fmi3_xml_unit_t* fmi3_xml_get_float32_variable_unit (fmi3_xml_float32_variable_t* v);
fmi3_xml_display_unit_t* fmi3_xml_get_float32_variable_display_unit (fmi3_xml_float32_variable_t* v);
fmi3_float32_t fmi3_xml_get_float32_variable_start (fmi3_xml_float32_variable_t* v);
fmi3_float32_t fmi3_xml_get_float32_variable_min (fmi3_xml_float32_variable_t* v);
fmi3_float32_t fmi3_xml_get_float32_variable_max (fmi3_xml_float32_variable_t* v);
fmi3_float32_t fmi3_xml_get_float32_variable_nominal (fmi3_xml_float32_variable_t* v);
fmi3_string_t fmi3_xml_get_float32_variable_quantity (fmi3_xml_float32_variable_t* v);
fmi3_boolean_t fmi3_xml_get_float32_variable_unbounded (fmi3_xml_float32_variable_t* v);
fmi3_boolean_t fmi3_xml_get_float32_variable_relative_quantity (fmi3_xml_float32_variable_t* v);

fmi3_xml_int64_variable_t* fmi3_xml_get_variable_as_int64(fmi3_xml_variable_t*);
fmi3_int64_t fmi3_xml_get_int64_variable_min (fmi3_xml_int64_variable_t* v);
fmi3_int64_t fmi3_xml_get_int64_variable_max (fmi3_xml_int64_variable_t* v);
fmi3_int64_t fmi3_xml_get_int64_variable_start (fmi3_xml_int64_variable_t* v);
fmi3_string_t fmi3_xml_get_int64_variable_quantity (fmi3_xml_int64_variable_t* v);

fmi3_xml_int32_variable_t* fmi3_xml_get_variable_as_int32(fmi3_xml_variable_t*);
fmi3_int32_t fmi3_xml_get_int32_variable_min (fmi3_xml_int32_variable_t* v);
fmi3_int32_t fmi3_xml_get_int32_variable_max (fmi3_xml_int32_variable_t* v);
fmi3_int32_t fmi3_xml_get_int32_variable_start (fmi3_xml_int32_variable_t* v);
fmi3_string_t fmi3_xml_get_int32_variable_quantity (fmi3_xml_int32_variable_t* v);

fmi3_xml_int16_variable_t* fmi3_xml_get_variable_as_int16(fmi3_xml_variable_t*);
fmi3_int16_t fmi3_xml_get_int16_variable_min (fmi3_xml_int16_variable_t* v);
fmi3_int16_t fmi3_xml_get_int16_variable_max (fmi3_xml_int16_variable_t* v);
fmi3_int16_t fmi3_xml_get_int16_variable_start (fmi3_xml_int16_variable_t* v);
fmi3_string_t fmi3_xml_get_int16_variable_quantity (fmi3_xml_int16_variable_t* v);

fmi3_xml_int8_variable_t* fmi3_xml_get_variable_as_int8(fmi3_xml_variable_t*);
fmi3_int8_t fmi3_xml_get_int8_variable_min (fmi3_xml_int8_variable_t* v);
fmi3_int8_t fmi3_xml_get_int8_variable_max (fmi3_xml_int8_variable_t* v);
fmi3_int8_t fmi3_xml_get_int8_variable_start (fmi3_xml_int8_variable_t* v);
fmi3_string_t fmi3_xml_get_int8_variable_quantity (fmi3_xml_int8_variable_t* v);

fmi3_xml_uint64_variable_t* fmi3_xml_get_variable_as_uint64(fmi3_xml_variable_t*);
fmi3_uint64_t fmi3_xml_get_uint64_variable_min (fmi3_xml_uint64_variable_t* v);
fmi3_uint64_t fmi3_xml_get_uint64_variable_max (fmi3_xml_uint64_variable_t* v);
fmi3_uint64_t fmi3_xml_get_uint64_variable_start (fmi3_xml_uint64_variable_t* v);
fmi3_string_t fmi3_xml_get_uint64_variable_quantity (fmi3_xml_uint64_variable_t* v);

fmi3_xml_uint32_variable_t* fmi3_xml_get_variable_as_uint32(fmi3_xml_variable_t*);
fmi3_uint32_t fmi3_xml_get_uint32_variable_min (fmi3_xml_uint32_variable_t* v);
fmi3_uint32_t fmi3_xml_get_uint32_variable_max (fmi3_xml_uint32_variable_t* v);
fmi3_uint32_t fmi3_xml_get_uint32_variable_start (fmi3_xml_uint32_variable_t* v);
fmi3_string_t fmi3_xml_get_uint32_variable_quantity (fmi3_xml_uint32_variable_t* v);

fmi3_xml_uint16_variable_t* fmi3_xml_get_variable_as_uint16(fmi3_xml_variable_t*);
fmi3_uint16_t fmi3_xml_get_uint16_variable_min (fmi3_xml_uint16_variable_t* v);
fmi3_uint16_t fmi3_xml_get_uint16_variable_max (fmi3_xml_uint16_variable_t* v);
fmi3_uint16_t fmi3_xml_get_uint16_variable_start (fmi3_xml_uint16_variable_t* v);
fmi3_string_t fmi3_xml_get_uint16_variable_quantity (fmi3_xml_uint16_variable_t* v);

fmi3_xml_uint8_variable_t* fmi3_xml_get_variable_as_uint8(fmi3_xml_variable_t*);
fmi3_uint8_t fmi3_xml_get_uint8_variable_min (fmi3_xml_uint8_variable_t* v);
fmi3_uint8_t fmi3_xml_get_uint8_variable_max (fmi3_xml_uint8_variable_t* v);
fmi3_uint8_t fmi3_xml_get_uint8_variable_start (fmi3_xml_uint8_variable_t* v);
fmi3_string_t fmi3_xml_get_uint8_variable_quantity (fmi3_xml_uint8_variable_t* v);

fmi3_xml_string_variable_t* fmi3_xml_get_variable_as_string(fmi3_xml_variable_t*);
const char* fmi3_xml_get_string_variable_start(fmi3_xml_string_variable_t* v);

fmi3_xml_bool_variable_t* fmi3_xml_get_variable_as_boolean(fmi3_xml_variable_t*);
fmi3_boolean_t fmi3_xml_get_boolean_variable_start(fmi3_xml_bool_variable_t* v);

fmi3_xml_binary_variable_t* fmi3_xml_get_variable_as_binary(fmi3_xml_variable_t*);
size_t fmi3_xml_get_binary_variable_start_length(fmi3_xml_binary_variable_t* v);
fmi3_binary_t fmi3_xml_get_binary_variable_start(fmi3_xml_binary_variable_t* v);
fmi3_string_t fmi3_xml_get_binary_variable_mime_type(fmi3_xml_binary_variable_t* v);
size_t fmi3_xml_get_binary_variable_max_size(fmi3_xml_binary_variable_t* v);

fmi3_xml_clock_variable_t* fmi3_xml_get_variable_as_clock(fmi3_xml_variable_t*);
fmi3_boolean_t fmi3_xml_get_clock_variable_can_be_deactivated(fmi3_xml_clock_variable_t* v);
fmi3_uint32_t fmi3_xml_get_clock_variable_priority(fmi3_xml_clock_variable_t* v);
fmi3_interval_variability_enu_t fmi3_xml_get_clock_variable_interval_variability(fmi3_xml_clock_variable_t* v);
fmi3_float32_t fmi3_xml_get_clock_variable_interval_decimal(fmi3_xml_clock_variable_t* v);
fmi3_float32_t fmi3_xml_get_clock_variable_shift_decimal(fmi3_xml_clock_variable_t* v);
fmi3_boolean_t fmi3_xml_get_clock_variable_supports_fraction(fmi3_xml_clock_variable_t* v);
fmi3_uint64_t fmi3_xml_get_clock_variable_resolution(fmi3_xml_clock_variable_t* v);
fmi3_uint64_t fmi3_xml_get_clock_variable_interval_counter(fmi3_xml_clock_variable_t* v);
fmi3_uint64_t fmi3_xml_get_clock_variable_shift_counter(fmi3_xml_clock_variable_t* v);

fmi3_xml_enum_variable_t* fmi3_xml_get_variable_as_enum(fmi3_xml_variable_t*);
fmi3_string_t fmi3_xml_get_enum_variable_quantity(fmi3_xml_enum_variable_t* v);
int fmi3_xml_get_enum_variable_start(fmi3_xml_enum_variable_t* v);
int fmi3_xml_get_enum_variable_min(fmi3_xml_enum_variable_t* v);
int fmi3_xml_get_enum_variable_max(fmi3_xml_enum_variable_t* v);


fmi3_variable_alias_kind_enu_t fmi3_xml_get_variable_alias_kind(fmi3_xml_variable_t*);
fmi3_xml_variable_t* fmi3_xml_get_variable_alias_base(fmi3_xml_model_description_t* md,fmi3_xml_variable_t*);

void fmi3_xml_variable_free_internals(jm_callbacks* callbacks, fmi3_xml_variable_t* var);

/**
    Return the list of all the variables aliased to the given one (including the base one.
    The list is ordered: base variable, aliases, negated aliases.
*/
jm_status_enu_t fmi3_xml_get_variable_aliases(fmi3_xml_model_description_t* md, fmi3_xml_variable_t*, jm_vector(jm_voidp)*);

/**
@}
*/
#ifdef __cplusplus
}
#endif
#endif
