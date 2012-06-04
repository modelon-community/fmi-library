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

#ifndef FMI1_ENUMS_H_
#define FMI1_ENUMS_H_

#include <fmilib_config.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \file fmi1_enums.h 
	\brief Definions the enum types used with FMI 1.0 libs
*/

/**	\addtogroup fmi1_utils
	@{
	\addtogroup fmi1_enums
	@}
*/
/** \addtogroup fmi1_enums Enum types used with FMI 1.0 libs
	@{
*/
/** Naming convention for the variables in XML file*/
typedef enum fmi1_variable_naming_convension_enu_t
{ 
        fmi1_naming_enu_flat,
        fmi1_naming_enu_structured
} fmi1_variable_naming_convension_enu_t;

/** \brief Convert a #fmi1_variable_naming_convension_enu_t constant into string */
FMILIB_EXPORT const char* fmi1_naming_convention_to_string(fmi1_variable_naming_convension_enu_t convention);

/** FMU 1.0 kinds */
typedef enum fmi1_fmu_kind_enu_t
{
        fmi1_fmu_kind_enu_me = 0,
        fmi1_fmu_kind_enu_cs_standalone,
        fmi1_fmu_kind_enu_cs_tool
} fmi1_fmu_kind_enu_t;

/** \brief Convert a #fmi1_fmu_kind_enu_t constant into string  */
FMILIB_EXPORT const char* fmi1_fmu_kind_to_string(fmi1_fmu_kind_enu_t kind);

/** Variability property for variables */
typedef enum fmi1_variability_enu_t {
        fmi1_variability_enu_constant,
        fmi1_variability_enu_parameter,
        fmi1_variability_enu_discrete,
        fmi1_variability_enu_continuous
} fmi1_variability_enu_t;

/** \brief Convert a #fmi1_variability_enu_t constant into string  */
FMILIB_EXPORT const char* fmi1_variability_to_string(fmi1_variability_enu_t v);

/** Causality property for variables */
typedef enum fmi1_causality_enu_t {
        fmi1_causality_enu_input,
        fmi1_causality_enu_output,
        fmi1_causality_enu_internal,
        fmi1_causality_enu_none
} fmi1_causality_enu_t;

/** \brief Convert a #fmi1_causality_enu_t constant into string  */
FMILIB_EXPORT const char* fmi1_causality_to_string(fmi1_causality_enu_t c);

/** Alias property for variables */
typedef enum fmi1_variable_alias_kind_enu_t {
    fmi1_variable_is_negated_alias = -1,
    fmi1_variable_is_not_alias = 0,
    fmi1_variable_is_alias = 1
} fmi1_variable_alias_kind_enu_t;

/** Base types used in type definitions */
typedef enum fmi1_base_type_enu_t
{
	fmi1_base_type_real,
	fmi1_base_type_int,
	fmi1_base_type_bool,
	fmi1_base_type_str,
	fmi1_base_type_enum
} fmi1_base_type_enu_t;

/* Convert base type constant to string */
FMILIB_EXPORT const char* fmi1_base_type_to_string(fmi1_base_type_enu_t bt);

/**	
 @}
*/
#ifdef __cplusplus
}
#endif

#endif /* End of header file FMI1_ENUMS_H_ */
