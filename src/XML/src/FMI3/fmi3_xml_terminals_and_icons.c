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

    fmi3_xml_terminals_and_icons_t* terminalsAndIcons = (fmi3_xml_terminals_and_icons_t*)cb->calloc(1, sizeof(fmi3_xml_terminals_and_icons_t));
    if (!terminalsAndIcons) {
        jm_log_fatal(cb, module, "Could not allocate memory");
        return 0;
    }
    terminalsAndIcons->callbacks = cb;
    terminalsAndIcons->status = -1; // TODO

    return terminalsAndIcons;
}

void fmi3_xml_free_terminals_and_icons(fmi3_xml_terminals_and_icons_t* termIcon) {
    // void(*cb_free)(const char*) = (void(*)(const char*))md->callbacks->free;
    // TODO: Use JM functionality?
    free(termIcon);
}

int fmi3_xml_handle_fmiTerminalsAndIcons(fmi3_xml_parser_context_t* context, const char* data) {
    if (!data) {
        jm_log_verbose(context->callbacks, module, "Parsing XML element fmiTerminalsAndIcons");
    } else { // post process <fmiTerminalsAndIcons>
        // TODO: Any error checks?
        ;
    }
    return 0;
}
