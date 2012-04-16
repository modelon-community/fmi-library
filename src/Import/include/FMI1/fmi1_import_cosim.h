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

#ifndef FMI1_IMPORT_COSIM_H
#define FMI1_IMPORT_COSIM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "fmi1_import.h"
	/**
		\file fmi1_import_cosim.h
		Functions to retrieve co-simulation related information.
	*/
	/**
	\addtogroup fmi1_import
	@{
	\addtogroup fmi1_import_cosim Functions to retrieve co-simulation related information.
	@}
	*/
	/**
	\addtogroup fmi1_import_cosim
	@{
	*/

	/** \brief Get the entry point.*/
const char* fmi1_import_get_entry_point(fmi1_import_t* );
	/** \brief Get mime type. */
const char* fmi1_import_get_mime_type(fmi1_import_t* );
	/** \brief Get manual start flag. */
int fmi1_import_get_manual_start(fmi1_import_t* );

	/** \brief Get the number of additional models specified. */
size_t fmi1_import_get_number_of_additional_models(fmi1_import_t* fmu);

	/** \brief Get the name of an additional model.
		@param fmu - the FMU processed
		@param index - the index of an additional model (must be less than the number returned by fmi1_import_get_number_of_additional_models().
	*/
const char* fmi1_import_get_additional_model_name(fmi1_import_t* fmu, size_t index);

/**
@}
*/
#ifdef __cplusplus
}
#endif
#endif /* FMI1_IMPORT_COSIM_H */
