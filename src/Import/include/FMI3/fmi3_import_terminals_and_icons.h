/*
    Copyright (C) 2023 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

/** \file fmi3_import_terminals_and_icons.h
*  \brief Public interface to the FMI XML C-library. Handling of terminals and icons.
*/

#ifndef FMI3_IMPORT_TERMINALS_AND_ICONS_H_
#define FMI3_IMPORT_TERMINALS_AND_ICONS_H_

#include <stddef.h>
#include <fmilib_config.h>
#include <JM/jm_callbacks.h>

#include "fmi3_import.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
    \addtogroup fmi3_import 
    @{
       \defgroup fmi3_import_terminals_and_icons Handling of terminals and icons
    @}
 */

/**
    \addtogroup fmi3_import_terminals_and_icons
    \brief Handling of terminals and icons.
    @{ 
 */

/** \brief Opaque Terminal type */
typedef struct fmi3_xml_terminal_t fmi3_import_terminal_t;

/**
 * \brief Returns non-zero if terminalsAndIcons.xml has been found and successfully parsed.
 * @param fmu An #fmi3_import_t object as returned by #fmi3_import_parse_xml().
 */
FMILIB_EXPORT int fmi3_import_get_has_terminals_and_icons(fmi3_import_t* fmu);

/**
    \brief Get terminal by terminal name.
    @param fmu An #fmi3_import_t object as returned by #fmi3_import_parse_xml().
    @param name terminal name
    @return terminal pointer. NULL if terminal with given name does not exist.
 */
FMILIB_EXPORT fmi3_import_terminal_t* fmi3_import_get_terminal_by_name(fmi3_import_t* fmu, const char* name);

/** 
    \brief Get the variable name
    @param term An #fmi3_import_terminal_t pointer
    @return name
*/ 
FMILIB_EXPORT const char* fmi3_import_get_terminal_name(fmi3_import_terminal_t* term);

/** @} */

#ifdef __cplusplus
}
#endif

#endif // FMI3_IMPORT_TERMINALS_AND_ICONS_H_
