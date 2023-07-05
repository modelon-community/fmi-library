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

// TODO: Tidy up documentation

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
 * \brief Returns non-zero if terminalsAndIcons.xml has been found and successfully parsed
 * @param fmu An fmu object as returned by fmi3_import_parse_xml().
 */
FMILIB_EXPORT int fmi3_import_get_has_terminals_and_icons(fmi3_import_t* fmu);


#ifdef __cplusplus
}
#endif

#endif // FMI3_IMPORT_TERMINALS_AND_ICONS_H_
