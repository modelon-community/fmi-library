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

#include <stdio.h>


#include "JM/jm_named_ptr.h"
#include "fmi3_xml_parser.h"
// #include "fmi3_xml_terminals_and_icons.h"
#include "fmi3_xml_terminals_and_icons_impl.h"

static const char* module = "FMI3XML";

fmi3_xml_terminals_and_icons_t* fmi3_xml_allocate_terminals_and_icons(jm_callbacks* callbacks) {
    jm_callbacks* cb = callbacks ? callbacks : jm_get_default_callbacks();

    fmi3_xml_terminals_and_icons_t* termIcon = (fmi3_xml_terminals_and_icons_t*)cb->calloc(1, sizeof(fmi3_xml_terminals_and_icons_t));
    if (!termIcon) {
        jm_log_fatal(cb, module, "Could not allocate memory");
        return 0;
    }
    termIcon->callbacks = cb;

    jm_vector_init(char)(&termIcon->fmi3_xml_standard_version, 0, cb);

    termIcon->status = -1; // TODO

    return termIcon;
}

void fmi3_xml_free_terminals_and_icons(fmi3_xml_terminals_and_icons_t* termIcon) {
    if (termIcon) {
        jm_vector_free_data(char)(&termIcon->fmi3_xml_standard_version);
    }
    free(termIcon);
}

int fmi3_xml_handle_fmiTerminalsAndIcons(fmi3_xml_parser_context_t* context, const char* data) {
    fmi3_xml_terminals_and_icons_t* termIcon = context->termIcon;
    if (!data) {
        int ret;
        if (context->currentElmID != fmi3_xml_elmID_none) {
            fmi3_xml_parse_fatal(context, "fmiTerminalsAndIcons must be the root XML element");
            return -1;
        }
        jm_log_verbose(context->callbacks, module, "Parsing XML element fmiTerminalsAndIcons");

        ret = fmi3_xml_parse_attr_as_string(context, fmi3_xml_elmID_fmiModelDescription, fmi_attr_id_fmiVersion, 1, &(termIcon->fmi3_xml_standard_version));
        if (ret) {
            return ret;
        }
        // TODO: Error check that fmiVersion lines up with the one from ModelDescription

        return ret;
    } else { // post process <fmiTerminalsAndIcons>
        ;
    }
    return 0;
}

int fmi3_xml_handle_Terminals(fmi3_xml_parser_context_t* context, const char* data) {
    // fmi3_xml_terminals_and_icons_t* termIcon = context->termIcon;
    if (!data) {
        ;
    } else { // post process <Terminals>
        ;
    }
    return 0;
}

int fmi3_xml_handle_Terminal(fmi3_xml_parser_context_t* context, const char* data) {
    // fmi3_xml_terminals_and_icons_t* termIcon = context->termIcon;
    if (!data) {
        ;
    } else { // post process <Terminals>
        ;
    }
    return 0;
}
