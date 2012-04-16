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



/** \file fmi1_import_variable_list.h
*  \brief Public interface to the FMI XML C-library. Handling of variable lists.
*/

#ifndef FMI1_IMPORT_VARIABLELIST_H_
#define FMI1_IMPORT_VARIABLELIST_H_

 #include "fmi1_import.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 \addtogroup  fmi1_import
 @{
	\addtogroup  fmi1_import_varlist Handling of variable lists
 @}
*/

/** \addtogroup  fmi1_import_varlist Handling of variable lists
 @{ 
*/

/** Get the list of all the variables in the model */
fmi1_import_variable_list_t* fmi1_import_get_variable_list(fmi1_import_t* fmu);

/** Note that variable lists are allocated dynamically and must be freed when not needed any longer */
void fmi1_import_free_variable_list(fmi1_import_variable_list_t*);

/** Make a copy */
fmi1_import_variable_list_t* fmi1_import_clone_variable_list(fmi1_import_variable_list_t*);

/** Get number of variables in a list */
size_t  fmi1_import_get_variable_list_size(fmi1_import_variable_list_t*);

/** Get a pointer to the list of the value references for all the variables */
const fmi1_value_reference_t* fmi1_import_get_value_referece_list(fmi1_import_variable_list_t*);

/** Get a single variable from the list*/
fmi1_import_variable_t* fmi1_import_get_variable(fmi1_import_variable_list_t*, unsigned int  index);

/** Operations on variable lists. Every operation creates a new list. */
/* Select sub-lists. Both fromIndex and toIndex are "inclusive" */
fmi1_import_variable_list_t* fmi1_import_get_sublist(fmi1_import_variable_list_t*, unsigned int  fromIndex, unsigned int  toIndex);

/** Callback function typedef for the fmiFilterVariables. The function should return 0 to prevent a 
 variable from coming to the output list. */
typedef int (*fmi1_import_variable_filter_function_ft)(fmi1_import_variable_t*, void *);

/** fmi1_import_filter_variables calls  the provided 'filter' function on every variable in the list.
  The context parameter is forwarded to the filter function.
  @return a sub-list with the variables for which filter returned non-zero value. */
fmi1_import_variable_list_t* fmi1_import_filter_variables(fmi1_import_variable_list_t*, fmi1_import_variable_filter_function_ft filter, void* context);

/** \name  Join different lists 
@{
*/
fmi1_import_variable_list_t* fmi1_import_join_var_list(fmi1_import_variable_list_t*, fmi1_import_variable_list_t*);
fmi1_import_variable_list_t* fmi1_import_create_var_list(fmi1_import_t* fmu,fmi1_import_variable_t*);
fmi1_import_variable_list_t* fmi1_import_append_to_var_list(fmi1_import_variable_list_t*, fmi1_import_variable_t*);
fmi1_import_variable_list_t* fmi1_import_prepend_to_var_list(fmi1_import_variable_list_t*, fmi1_import_variable_list_t*);
/**
  @}
 */

/**
  @}
 */

#ifdef __cplusplus
}
#endif
#endif
