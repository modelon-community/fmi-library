/*
    Copyright (C) 2021 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

/** 
 * \file fmi2_import_options.h
 * \brief Public interface to the FMI import C-library. Handling of FMI Library options.
 */

#ifndef FMI2_IMPORT_OPTIONS_H_
#define FMI2_IMPORT_OPTIONS_H_

#include "FMI/fmi_import_context.h"
#include "JM/jm_portability.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
    \addtogroup fmi2_import
    @{
       \addtogroup fmi2_import_options Functions for handling FMI Library options. TODO this is duplicated below
    @}
    \addtogroup fmi2_import_options Functions for handling FMI Library options.
    @{
*/

/**
    \brief FMI Library options object.
*/
typedef struct fmi_import_options_t fmi_import_options_t;

/**
    \brief Returns the fmi_import_options_t:: object.
    \param fmu - an fmu object as returned by fmi2_import_parse_xml().
    \return fmi_import_options_t:: opaque object pointer
*/
FMILIB_EXPORT fmi_import_options_t* fmi2_import_get_options(fmi2_import_t* fmu);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* FMI2_IMPORT_OPTIONS_H_ */
