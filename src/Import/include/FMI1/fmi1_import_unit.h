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

/** \file fmi1_import_unit.h
*  \brief Public interface to the FMI import C-library. Handling of variable units.
*/

#ifndef FMI1_IMPORT_UNIT_H_
#define FMI1_IMPORT_UNIT_H_

#include "fmi1_import.h"

#ifdef __cplusplus
extern "C" {
#endif

		/**
	\addtogroup fmi1_import
	@{
	\addtogroup fmi1_import_units Functions for handling unit definitions.
	@}
	\addtogroup fmi1_import_units Functions for handling unit definitions.
	@{
	*/

/** \brief Get a list of all the unit definitions in the model. */
fmi1_import_unit_definitions_t* fmi1_import_get_unit_definitions(fmi1_import_t* fmu);

/** \brief Get the number of unit definitions. */
unsigned int  fmi1_import_get_unit_definitions_number(fmi1_import_unit_definitions_t*);

/** \brief Get a unit definition */
fmi1_import_unit_t* fmi1_import_get_unit(fmi1_import_unit_definitions_t*, unsigned int  index);

/** \brief Get a unit name */
const char* fmi1_import_get_unit_name(fmi1_import_unit_t*);

/** \brief Get the number of display units associated with this unit */
unsigned int fmi1_import_get_unit_display_unit_number(fmi1_import_unit_t*);

/** \brief Get a display unit object by index.
	@param index The index of display unit to be returned. Must be less than the number returned by fmi1_import_get_unit_display_unit_number()
*/
fmi1_import_display_unit_t* fmi1_import_get_unit_display_unit(fmi1_import_unit_t*, size_t index);

/**
	\brief Get display unit associated with a real type definition.
	@return Display unit object of NULL if none was given.
*/
fmi1_import_display_unit_t* fmi1_import_get_type_display_unit(fmi1_import_real_typedef_t*);
/**
	\brief Get unit defition for a display unit.
*/
fmi1_import_unit_t* fmi1_import_get_base_unit(fmi1_import_display_unit_t*);
/**
	\brief Get display unit name
*/
const char* fmi1_import_get_display_unit_name(fmi1_import_display_unit_t*);

/**
	\brief Get the "gain" associated with the display unit.
*/
fmi1_real_t fmi1_import_get_display_unit_gain(fmi1_import_display_unit_t*);
/**
	\brief Get the "offset" associated with the display unit.
*/
fmi1_real_t fmi1_import_get_display_unit_offset(fmi1_import_display_unit_t*);

/**
	\brief Convert a value measured in "units" to a value measured with "display units"
	@param value The value to be converted.
	@param du The display unit object
	@param isRelativeQuantity specifies if "offset" should be incorporated into conversion
*/
fmi1_real_t fmi1_import_convert_to_display_unit(fmi1_real_t value , fmi1_import_display_unit_t* du, int isRelativeQuantity);


/**
	\brief Convert a value measured in "display units" to a value measured with "units"
	@param value The value to be converted.
	@param du The display unit object
	@param isRelativeQuantity specifies if "offset" should be incorporated into conversion
*/
fmi1_real_t fmi1_import_convert_from_display_unit(fmi1_real_t value, fmi1_import_display_unit_t* du, int isRelativeQuantity);
/** @} */
#ifdef __cplusplus
}
#endif
#endif
