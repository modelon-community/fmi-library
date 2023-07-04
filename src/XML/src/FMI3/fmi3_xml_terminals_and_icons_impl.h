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
// #include <JM/jm_named_ptr.h>
// #include <JM/jm_string_set.h>
// #include <FMI3/fmi3_xml_model_description.h>

#ifdef __cplusplus
extern "C" {
#endif

struct fmi3_xml_terminals_and_icons_t {
    jm_callbacks* callbacks;
    // TODO: Use an enum?
    int status;

    jm_vector(char) fmi3_xml_standard_version;
    // TODO: GraphicalRepresentation
    jm_vector(jm_voidp) fmi3_xml_terminals_and_icons_t;
    // TODO: Annotations
};


#ifdef __cplusplus
}
#endif

#endif // fmi3_xml_terminals_and_icons_impl_h_
