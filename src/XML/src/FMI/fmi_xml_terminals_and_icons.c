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
#include <FMI2/fmi2_xml_model_description.h>
#include "../FMI3/fmi3_xml_parser.h"
#include "fmi_xml_terminals_and_icons_impl.h"

static const char* module = "FMIXML";

fmi_xml_terminals_and_icons_t* fmi_xml_allocate_terminals_and_icons(jm_callbacks* callbacks) {
    jm_callbacks* cb = callbacks ? callbacks : jm_get_default_callbacks();

    fmi_xml_terminals_and_icons_t* termIcon = (fmi_xml_terminals_and_icons_t*)callbacks->calloc(1, sizeof(fmi_xml_terminals_and_icons_t));
    if (!termIcon) {
        jm_log_fatal(cb, module, "Could not allocate memory");
        return 0;
    }
    termIcon->callbacks = cb;
    termIcon->md2 = NULL;
    termIcon->md3 = NULL;

    jm_vector_init(char)(&termIcon->fmi3_xml_standard_version, 0, cb);

    jm_vector_init(jm_voidp)(&termIcon->terminalsOrigOrder, 0, cb);
    jm_vector_init(jm_named_ptr)(&termIcon->terminalsByName, 0, cb);

    jm_vector_init(jm_string)(&termIcon->names, 0, cb);

    return termIcon;
}

static fmi_xml_terminal_t* fmi_xml_alloc_terminal(fmi3_xml_parser_context_t* context) {
    fmi_xml_terminal_t* term = (fmi_xml_terminal_t*)context->callbacks->calloc(1, sizeof(fmi_xml_terminal_t));
    if (!term) {
        fmi3_xml_parse_fatal(context, "Could not allocate memory");
        return NULL;
    }
    return term;
}

static void fmi_xml_free_terminal(jm_callbacks* callbacks, fmi_xml_terminal_t* term) {
    callbacks->free(term);
}

void fmi_xml_free_terminals_and_icons(fmi_xml_terminals_and_icons_t* termIcon) {
    if (!termIcon) {return;}
    jm_callbacks* callbacks = termIcon->callbacks;
    if (termIcon) {
        jm_vector_free_data(char)(&(termIcon->fmi3_xml_standard_version));

        // free terminals
        for (size_t i = 0; i < jm_vector_get_size(jm_voidp)(&(termIcon->terminalsOrigOrder)); i++) {
            fmi_xml_terminal_t* term = (fmi_xml_terminal_t*) jm_vector_get_item(jm_voidp)(&(termIcon->terminalsOrigOrder), i);
            fmi_xml_free_terminal(callbacks, term);
        }
        jm_vector_free_data(jm_voidp)(&(termIcon->terminalsOrigOrder));
        jm_vector_free_data(jm_named_ptr)(&termIcon->terminalsByName);

        // function pointer to callbacks->free function for usage in jm_vector_foreach calls
        void(*cb_free)(const char*) = (void(*)(const char*))callbacks->free;

        // free names set
        jm_vector_foreach(jm_string)(&termIcon->names, cb_free);
        jm_vector_free_data(jm_string)(&termIcon->names);
    }
    callbacks->free(termIcon);
}

int fmi2_xml_terminals_and_icons_set_model_description(fmi_xml_terminals_and_icons_t* termIcon,
                                                       fmi2_xml_model_description_t* md) {
    if (!termIcon || !md || termIcon->md3) {
        return -1;
    } else {
        termIcon->md2 = md;
        return 0;
    }
}

int fmi3_xml_terminals_and_icons_set_model_description(fmi_xml_terminals_and_icons_t* termIcon,
                                                       fmi3_xml_model_description_t* md) {
    if (!termIcon || !md || termIcon->md2) {
        return -1;
    } else {
        termIcon->md3 = md;
        return 0;
    }
}

int fmi_xml_handle_fmiTerminalsAndIcons(fmi3_xml_parser_context_t* context, const char* data) {
    fmi_xml_terminals_and_icons_t* termIcon = context->termIcon;
    if (!data) {
        int ret;
        if (context->currentElmID.termIcon != fmi3_xml_termIcon_elmID_none) {
            fmi3_xml_parse_fatal(context, "fmiTerminalsAndIcons must be the root XML element");
            return -1;
        }
        jm_log_verbose(context->callbacks, module, "Parsing XML element fmiTerminalsAndIcons");

        ret = fmi3_xml_parse_attr_as_string(context, FMI_TERMICON_ELM(fmi_termIcon_xml_elmID_fmiTerminalsAndIcons), FMI_TERMICON_ATTR(fmi_attr_id_fmiVersion), 1 /* required */,
                                            &(termIcon->fmi3_xml_standard_version));
        if (ret) {
            return ret;
        }
        // Error check: fmiVersion in terminalsAndIcons and modelDescription must match
        const char* modelDescrfmiVersion = NULL;
        if (termIcon->md2 && !(termIcon->md3)) {
            modelDescrfmiVersion = fmi2_xml_get_model_standard_version(termIcon->md2);
        }
        if (termIcon->md3 && !(termIcon->md2)) {
            modelDescrfmiVersion = fmi3_xml_get_model_standard_version(termIcon->md3);
        } 
        const char* termIconfmiVersion = jm_vector_char2string(&(termIcon->fmi3_xml_standard_version));
        if (strcmp(modelDescrfmiVersion, termIconfmiVersion)) {
            fmi3_xml_parse_fatal(context,
                                 "Mismatch of attribute 'fmiVersion' in modelDescription.xml: '%s' and terminalsAndIcons.xml: '%s'.", 
                                 modelDescrfmiVersion,
                                 termIconfmiVersion);
            return -1;
        }

        return ret;
    } else { // post process <fmiTerminalsAndIcons>
        ;
    }
    return 0;
}

int fmi_xml_handle_Terminals(fmi3_xml_parser_context_t* context, const char* data) {
    fmi_xml_terminals_and_icons_t* termIcon = context->termIcon;
    if (!data) {
        ;
    } else { // post process <Terminals>
        // Create terminalsByName
        size_t nVars = jm_vector_get_size(jm_voidp)(&termIcon->terminalsOrigOrder);

        jm_vector_resize(jm_named_ptr)(&termIcon->terminalsByName, nVars);  // Allocate a lot of memory to avoid resizing
        jm_vector_resize(jm_named_ptr)(&termIcon->terminalsByName, 0);      // Make sure we start pushback at index=0
        for (size_t i = 0; i < nVars; i++) {
            jm_named_ptr named;
            fmi_xml_terminal_t* term = jm_vector_get_item(jm_voidp)(&termIcon->terminalsOrigOrder, i);
            named.name = term->name;
            named.ptr = term;
            jm_vector_push_back(jm_named_ptr)(&termIcon->terminalsByName, named);
        }
        jm_vector_qsort(jm_named_ptr)(&termIcon->terminalsByName, jm_compare_named);

        // Error check, Terminal names are must be unique
        if (nVars > 0) {  // nVars=0 would cause integer overflow in the loop condition
            for (size_t i = 0; i < nVars-1; ++i) {
                const char* name1 = jm_vector_get_item(jm_named_ptr)(&termIcon->terminalsByName, i).name;
                const char* name2 = jm_vector_get_item(jm_named_ptr)(&termIcon->terminalsByName, i+1).name;
                if(strcmp(name1, name2) == 0) {
                    fmi3_xml_parse_fatal(context, 
                            "Two terminals with the same name '%s' found. This is not allowed by the specification.", name1);
                    return -1;
                }
            }
        }
    }
    return 0;
}

int fmi_xml_handle_Terminal(fmi3_xml_parser_context_t* context, const char* data) {
    fmi_xml_terminals_and_icons_t* termIcon = context->termIcon;
    if (!data) {
        fmi_xml_terminal_t* term = fmi_xml_alloc_terminal(context);
        if (!term) {return -1;}

        /* Add Terminal ptr to terminalAndIcons obj */
        if (!jm_vector_push_back(jm_voidp)(&termIcon->terminalsOrigOrder, term)) {
            fmi3_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }
        // Parse and set attributes
        term->name = "";

        size_t bufIdx = 1;
        // parse name
        jm_vector(char)* bufName = fmi3_xml_reserve_parse_buffer(context, bufIdx++, 100);
        if (!bufName) {return -1;}
        if (fmi3_xml_parse_attr_as_string(context, FMI_TERMICON_ELM(fmi_termIcon_xml_elmID_Terminal), FMI_TERMICON_ATTR(fmi_attr_id_name), 1 /* required */, bufName)) {return -1;}

        /* Add the name to the terminalsAndIcons-wide set and retrieve the pointer */
        if (jm_vector_get_size(char)(bufName)) {
            term->name = jm_string_set_put(&termIcon->names, jm_vector_get_itemp(char)(bufName, 0));
        }

        // TODO: Parse the remaining attributes

    } else { // post process <Terminal>
        ;
    }
    return 0;
}

int fmi_xml_handle_TerminalMemberVariable(fmi3_xml_parser_context_t* context, const char* data) {
    // TODO: Implement
    if (!data) {
        ;
    } else {
        ;
    }
    return 0;
}

int fmi_xml_handle_TerminalStreamMemberVariable(fmi3_xml_parser_context_t* context, const char* data) {
    // TODO: Implement
    if (!data) {
        ;
    } else {
        ;
    }
    return 0;
}

int fmi_xml_handle_TerminalGraphicalRepresentation(fmi3_xml_parser_context_t* context, const char* data) {
    // TODO: Implement
    if (!data) {
        ;
    } else {
        ;
    }
    return 0;
}

int fmi_xml_get_has_terminals_and_icons(fmi_xml_terminals_and_icons_t* termIcon) {
    return termIcon ? 1 : 0;
}

fmi_xml_terminal_t* fmi_xml_get_terminal_by_name(fmi_xml_terminals_and_icons_t* termIcon, const char* name){
    if (!name) return NULL;
    jm_named_ptr key, *found;
    key.name = name;
    found = jm_vector_bsearch(jm_named_ptr)(&termIcon->terminalsByName, &key, jm_compare_named);
    if (!found) return NULL;
    return found->ptr;
}

const char* fmi_xml_get_terminal_name(fmi_xml_terminal_t* term) {
    return term ? term->name : NULL;
}
