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

#ifndef fmi3_xml_terminals_and_icons_impl_h_
#define fmi3_xml_terminals_and_icons_impl_h_

#include <stdarg.h>

#include <JM/jm_callbacks.h>
#include <JM/jm_vector.h>
#include <JM/jm_named_ptr.h>
#include <JM/jm_string_set.h>

#ifdef __cplusplus
extern "C" {
#endif

struct fmi3_xml_terminals_and_icons_t {
    jm_callbacks* callbacks;

    /* used for cross-referencing, should be used as read-only */
    fmi3_xml_model_description_t* md; 

    jm_vector(char) fmi3_xml_standard_version;

    jm_vector(jm_voidp) terminalsOrigOrder;
    jm_vector(jm_named_ptr) terminalsByName;

    jm_string_set names; // storage for all terminal names
};

struct fmi3_xml_terminal_t {
    const char* name;
    // TODO: Currently only supports name; add remaining attributes.
};


#ifdef __cplusplus
}
#endif

#endif // fmi3_xml_terminals_and_icons_impl_h_
