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

#ifndef FMI_UTIL_OPTIONS_IMPL_H
#define FMI_UTIL_OPTIONS_IMPL_H

#include <JM/jm_portability.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct fmi_util_options_t {
    jm_portability_loadlibrary_flag_t loadlibrary_flag;
} fmi_util_options_t;

#ifdef __cplusplus
}
#endif

#endif /* FMI_UTIL_OPTIONS_IMPL_H */
