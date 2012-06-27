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
#include <assert.h>

#include <FMI1/fmi1_functions.h>

#include <FMI1/fmi1_enums.h>

const char* fmi1_naming_convention_to_string(fmi1_variable_naming_convension_enu_t convention) {
    if(convention == fmi1_naming_enu_flat) return "flat";
    if(convention == fmi1_naming_enu_structured) return "structured";
    return "Unknown";
}

const char* fmi1_fmu_kind_to_string(fmi1_fmu_kind_enu_t kind) {
    switch (kind) {
    case fmi1_fmu_kind_enu_me: return "ModelExchange";
    case fmi1_fmu_kind_enu_cs_standalone: return "CoSimulation_StandAlone";
    case fmi1_fmu_kind_enu_cs_tool: return "CoSimulation_Tool";
	default: break;
    }
    return "Unknown";
}

const char* fmi1_variability_to_string(fmi1_variability_enu_t v) {
    switch(v) {
    case fmi1_variability_enu_constant: return "constant";
    case fmi1_variability_enu_parameter: return "parameter";
    case fmi1_variability_enu_discrete: return "discrete";
    case fmi1_variability_enu_continuous: return "continuous";
	default: break;
    }
    return "Unknown";
}

const char* fmi1_causality_to_string(fmi1_causality_enu_t c) {
    switch(c) {
    case fmi1_causality_enu_input: return "input";
    case fmi1_causality_enu_output: return "output";
    case fmi1_causality_enu_internal: return "internal";
    case fmi1_causality_enu_none: return "none";
	default: break;
    };
    return "Unknown";
}

const char* fmi1_status_to_string(fmi1_status_t status) {
	switch(status) {
	case fmi1_status_ok:
		return "OK";
	case fmi1_status_warning:
		return "Warning";
	case fmi1_status_discard:
		return "Discard";
	case fmi1_status_error:
		return "Error";
	case fmi1_status_fatal:
		return "Fatal";
	case fmi1_status_pending:
		return "Pending";
	default: break;
	}
	return "Undefined";
}

const char* fmi1_base_type_to_string(fmi1_base_type_enu_t bt) {
    switch(bt) {
    case fmi1_base_type_real: return "Real";
    case fmi1_base_type_int: return "Integer";
    case fmi1_base_type_bool: return "Boolean";
    case fmi1_base_type_str: return "String";
    case fmi1_base_type_enum: return "Enumeration";
	default: break;
    }
    return "Error";
}
