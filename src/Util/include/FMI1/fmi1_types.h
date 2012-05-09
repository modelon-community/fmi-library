/*
    Copyright (C) 2012 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FMI1_TYPES_H_
#define FMI1_TYPES_H_
/** \file fmi1_types.h
	Transformation of the standard FMI type names into fmi1_ prefixed.
*/
/**
	\addtogroup jm_utils
	@{
		\addtogroup fmi1_utils
	@}
*/

/**	\addtogroup fmi1_utils Functions and types supporting FMI 1.0 processing.
	@{
*/
/** \name Renaming of typedefs 
@{*/
#define fmiComponent fmi1_component_t
#define fmiValueReference fmi1_value_reference_t
#define fmiReal fmi1_real_t
#define fmiInteger fmi1_integer_t
#define fmiBoolean fmi1_boolean_t
#define fmiString fmi1_string_t
/** @}*/
/* Standard FMI 1.0 ME and CS types */
#include "1.0-CS/fmiPlatformTypes.h"
/** FMI platform name constant string.*/
static const char * fmi1_get_platform(void) {
	return fmiPlatform;
}

#undef fmiPlatform

/** FMI boolean constants.*/
typedef enum {
	fmi1_true=fmiTrue,
	fmi1_false=fmiFalse
} fmi1_boolean_enu_t;

#undef fmiTrue
#undef fmiFalse

/** Undefined value for fmiValueReference (largest unsigned int value) */
typedef enum fmi1_value_reference_enu_t {
	fmi1_undefined_value_reference = (int)fmiUndefinedValueReference
} fmi1_value_reference_enu_t;

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

#endif /* End of header file FMI1_TYPES_H_ */
