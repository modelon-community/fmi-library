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

/* This is the header file for util (non-public) functions that are used in
 * several fmi3_import** compilation units.
 */

#ifndef FMI3_IMPORT_UTIL_
#define FMI3_IMPORT_UTIL_

#include "fmi3_import_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

int fmi3_import_check_has_FMU(fmi3_import_t* fmu);

#ifdef __cplusplus
}
#endif

#endif /* FMI3_IMPORT_UTIL_ */
