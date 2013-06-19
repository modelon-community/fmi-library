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

#ifndef FMI2_TYPES_H_
#define FMI2_TYPES_H_
/** \file fmi2_types.h
	Transformation of the standard FMI type names into fmi2_ prefixed.
*/
/**
	\addtogroup jm_utils
	@{
		\addtogroup fmi2_utils
	@}
*/

/**	\addtogroup fmi2_utils Functions and types supporting FMI 2.0 processing.
	@{
*/
/** \name Renaming of typedefs 
@{*/
#define fmiComponent fmi2_component_t
#define fmiComponentEnvironment fmi2_component_environment_t
#define fmiFMUstate fmi2_FMU_state_t
#define fmiValueReference fmi2_value_reference_t
#define fmiReal fmi2_real_t
#define fmiInteger fmi2_integer_t
#define fmiBoolean fmi2_boolean_t
#define fmiString fmi2_string_t
#define fmiByte fmi2_byte_t

/** @}*/
/* Standard FMI 2.0 types */
#ifdef fmiTypesPlatform_h
#undef fmiTypesPlatform_h
#endif
#include <FMI2/fmiTypesPlatform.h>
#undef fmiTypesPlatform_h

/** FMI platform name constant string.*/
static const char * fmi2_get_types_platform(void) {
	return fmiTypesPlatform;
}

#undef fmiTypesPlatform

/** FMI boolean constants.*/
typedef enum {
	fmi2_true=fmiTrue,
	fmi2_false=fmiFalse
} fmi2_boolean_enu_t;

#undef fmiTrue
#undef fmiFalse

/**	
	@}
*/

#undef fmiComponent
#undef fmiValueReference
#undef fmiReal
#undef fmiInteger
#undef fmiBoolean
#undef fmiString
#undef fmiUndefinedValueReference

#endif /* End of header file FMI2_TYPES_H_ */
