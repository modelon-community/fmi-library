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



/** \file fmi3_import_dimension_list.h
*  \brief Public interface to the FMI XML C-library. Handling of dimension lists.
*/

#ifndef FMI3_IMPORT_DIMENSION_LIST_H_
#define FMI3_IMPORT_DIMENSION_LIST_H_

#include "fmi3_import_dimension.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
    \addtogroup fmi3_import @{
       \defgroup fmi3_import_dimlist Handling of dimension lists
    @}
 */

/**
    \addtogroup fmi3_import_dimlist
    \brief dimension lists are provided to handle sets of dimensions.
    Note that dimension lists are allocated dynamically and must be freed when not needed any longer.
    @{ 
 */

/** \brief Allocate an empty list (internal only) */
fmi3_import_dimension_list_t* fmi3_import_alloc_dimension_list(fmi3_import_t* fmu, fmi3_import_variable_t* v);

/**  \brief Free a dimension list. Note that dimension lists are allocated dynamically and must be freed when not needed any longer 
    \param dl A dimension list.
*/
FMILIB_EXPORT void fmi3_import_free_dimension_list(fmi3_import_dimension_list_t* dl);

/** \brief  Get number of dimensions in a list */
FMILIB_EXPORT size_t fmi3_import_get_dimension_list_size(fmi3_import_dimension_list_t* dl);

/** \brief Get a single dimension from a list */
FMILIB_EXPORT fmi3_import_dimension_t* fmi3_import_get_dimension_list_item(fmi3_import_dimension_list_t* dl, size_t index);

#ifdef __cplusplus
}
#endif
#endif /* FMI3_IMPORT_DIMENSION_LIST_H_ */
