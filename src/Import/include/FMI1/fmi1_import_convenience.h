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



/** \file fmi1_import_convenience.h
*  \brief Public interface to the FMI import C-library. Convenience functions.
*
*  The functions in this file are provided for convenience. The functionality
*  is already available via other lower level functions.
*/

#ifndef FMI1_IMPORT_CONVENIENCE_H_
#define FMI1_IMPORT_CONVENIENCE_H_

#include "fmi1_import.h"


#ifdef __cplusplus
extern "C" {
#endif
		/**
	\addtogroup fmi1_import
	@{
	\addtogroup fmi1_import_convenience Convenience functions.
	@}
	\addtogroup fmi1_import_convenience Convenience functions.
	\brief The functions in this module are provided for convenience. The functionality
	*  is already available via other lower level functions.
	@{
	*/
	
/** 
\brief Collection of counters providing model information.
	*/
typedef struct {
	/** \brief Number of constants */
	unsigned int num_constants;
	/** \brief  Number of parameters */
	unsigned int num_parameters;

	/** \brief  Number of discrete variables */
	unsigned int num_discrete;
	/** \brief  Number of continuous variables */
	unsigned int num_continuous;

	/** \brief  Number of inputs */
	unsigned int num_inputs;
	/** \brief  Number of outputs */
	unsigned int num_outputs;
	/** \brief  Number of internal variables */
	unsigned int num_internal;
	/** \brief  Number of variables with causality 'none'*/
	unsigned int num_causality_none;

	/** \brief  Number of real variables*/
	unsigned int num_real_vars; 
	/** \brief  Number of integer variables*/
	unsigned int num_integer_vars; 
	/** \brief  Number of enumeration variables*/
	unsigned int num_enum_vars; 
	/** \brief  Number of boolean variables*/
	unsigned int num_bool_vars; 
	/** \brief  Number of string variables*/
	unsigned int num_string_vars; 
} fmi1_import_model_counts_t;

/**
	\brief Collect model information by counting the number of variables with specific properties and fillinf in fmi1_import_model_counts_t struct.
	\param fmu - An fmu object as returned by fmi1_import_parse_xml().
	\param counts - a pointer to a preallocated struct.
*/
FMILIB_EXPORT void fmi1_import_collect_model_counts(fmi1_import_t* fmu, fmi1_import_model_counts_t* counts);

/** @} */

#ifdef __cplusplus
}
#endif
#endif /* FMI1_IMPORT_CONVENIENCE_H_ */
