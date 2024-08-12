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

#include "JM/jm_named_ptr.h"
#include <FMI/fmi_xml_terminals_and_icons.h>
#include "fmi_xml_terminals_and_icons_impl.h"
#include "../FMI3/fmi3_xml_parser_context_impl.h"
#include "../FMI3/fmi3_xml_parser.h"

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

    termIcon->graphicalRepresentation = NULL;

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
        
        // free graphicalDescription
        if (termIcon->graphicalRepresentation) {
            callbacks->free(termIcon->graphicalRepresentation->coordinateSystem);
            callbacks->free(termIcon->graphicalRepresentation->icon);
        }
        callbacks->free(termIcon->graphicalRepresentation);

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
        if (context->currentElmID.termIcon != fmi_xml_elmID_termIcon_none) {
            fmi3_xml_parse_fatal(context, "fmiTerminalsAndIcons must be the root XML element");
            return -1;
        }
        jm_log_verbose(context->callbacks, module, "Parsing XML element fmiTerminalsAndIcons");

        ret = fmi3_xml_parse_attr_as_string(context, FMI_ELM_TERMICON(fmi_xml_elmID_termIcon_fmiTerminalsAndIcons), FMI_ATTR_TERMICON(fmi_attr_id_fmiVersion), 1 /* required */,
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

int fmi_xml_handle_GraphicalRepresentation(fmi3_xml_parser_context_t* context, const char* data) {
    fmi_xml_terminals_and_icons_t* termIcon = context->termIcon;
    if (!data) {
        termIcon->graphicalRepresentation = (fmi_xml_graphicalRepresentation_t*)context->callbacks->calloc(1, sizeof(fmi_xml_graphicalRepresentation_t));
        if (!termIcon->graphicalRepresentation) {
            fmi3_xml_parse_fatal(context, "Could not allocate memory");
            return 1;
        }
        // GraphicalRepresentation has a default coordinateSystem, allocate here directly
        termIcon->graphicalRepresentation->coordinateSystem = (fmi_xml_coordinateSystem_t*)context->callbacks->calloc(1, sizeof(fmi_xml_coordinateSystem_t));
        if (!termIcon->graphicalRepresentation->coordinateSystem) {
            fmi3_xml_parse_fatal(context, "Could not allocate memory");
            return 1;
        }
        termIcon->graphicalRepresentation->coordinateSystem->x1 = -100;
        termIcon->graphicalRepresentation->coordinateSystem->y1 = -100;
        termIcon->graphicalRepresentation->coordinateSystem->x2 = 100;
        termIcon->graphicalRepresentation->coordinateSystem->y2 = 100;
        termIcon->graphicalRepresentation->coordinateSystem->suggestedScalingFactorTo_mm = 0.1;
    } else {
        ;
    }
    return 0;
}

int fmi_xml_handle_CoordinateSystem(fmi3_xml_parser_context_t* context, const char* data) {
    fmi_xml_terminals_and_icons_t* termIcon = context->termIcon;
    fmi_xml_coordinateSystem_t* coordSys = termIcon->graphicalRepresentation->coordinateSystem;
    if (!data) {
        // Allocation in GraphicalRepresentation
        int ret = 0;

        ret |= fmi3_xml_parse_attr_as_float64(context, FMI_ELM_TERMICON(fmi_xml_elmID_termIcon_CoordinateSystem), 
                FMI_ATTR_TERMICON(fmi_termIcon_attr_id_x1), 1, &(coordSys->x1), -100);
        ret |= fmi3_xml_parse_attr_as_float64(context, FMI_ELM_TERMICON(fmi_xml_elmID_termIcon_CoordinateSystem), 
                FMI_ATTR_TERMICON(fmi_termIcon_attr_id_y1), 1, &(coordSys->y1), -100);
        ret |= fmi3_xml_parse_attr_as_float64(context, FMI_ELM_TERMICON(fmi_xml_elmID_termIcon_CoordinateSystem), 
                FMI_ATTR_TERMICON(fmi_termIcon_attr_id_x2), 1, &(coordSys->x2), 100);
        ret |= fmi3_xml_parse_attr_as_float64(context, FMI_ELM_TERMICON(fmi_xml_elmID_termIcon_CoordinateSystem), 
                FMI_ATTR_TERMICON(fmi_termIcon_attr_id_y2), 1, &(coordSys->y2), 100);
        ret |= fmi3_xml_parse_attr_as_float64(context, FMI_ELM_TERMICON(fmi_xml_elmID_termIcon_CoordinateSystem), 
                FMI_ATTR_TERMICON(fmi_termIcon_attr_id_suggestedScalingFactorTo_mm), 1, &(coordSys->suggestedScalingFactorTo_mm), 0.1);
        if (ret) {
            fmi3_xml_parse_error(context, "Failed to parse complete CoordinateSystem, using default system (-100, -100), (100, 100).");
            coordSys->x1 = -100;
            coordSys->y1 = -100;
            coordSys->x2 = 100;
            coordSys->y2 = 100;
            coordSys->suggestedScalingFactorTo_mm = 0.1;
            return 1;
        }
    } else {
        /* SPEC: [...] where the coordinates of the first point shall be less than the coordinates of the second point. */
        if (coordSys->x1 >= coordSys->x2) {
            fmi3_xml_parse_warning(context, "'CoordinateSystem' not well-defined, requires x1 = %f < x2 = %f.",
                coordSys->x1, coordSys->x2);
        }
        if (coordSys->y1 >= coordSys->y2) {
            fmi3_xml_parse_warning(context, "'CoordinateSystem' not well-defined, requires y1 = %f < y2 = %f.",
                coordSys->y1, coordSys->y2);
        }
    }
    return 0;
}

int fmi_xml_handle_Icon(fmi3_xml_parser_context_t* context, const char* data) {
    fmi_xml_terminals_and_icons_t* termIcon = context->termIcon;
    fmi_xml_graphicalRepresentation_t* graphRepr = termIcon->graphicalRepresentation;
    if (!data) {
        int ret;
        fmi_xml_icon_t* icon = (fmi_xml_icon_t*)context->callbacks->calloc(1, sizeof(fmi_xml_icon_t));
        if (!icon) {
            fmi3_xml_parse_fatal(context, "Could not allocate memory");
            return 1;
        }
        ret = 0;
        ret |= fmi3_xml_parse_attr_as_float64(context, FMI_ELM_TERMICON(fmi_xml_elmID_termIcon_Icon), 
                FMI_ATTR_TERMICON(fmi_termIcon_attr_id_x1), 1, &(icon->x1), 0);
        ret |= fmi3_xml_parse_attr_as_float64(context, FMI_ELM_TERMICON(fmi_xml_elmID_termIcon_Icon), 
                FMI_ATTR_TERMICON(fmi_termIcon_attr_id_y1), 1, &(icon->y1), 0);
        ret |= fmi3_xml_parse_attr_as_float64(context, FMI_ELM_TERMICON(fmi_xml_elmID_termIcon_Icon), 
                FMI_ATTR_TERMICON(fmi_termIcon_attr_id_x2), 1, &(icon->x2), 0);
        ret |= fmi3_xml_parse_attr_as_float64(context, FMI_ELM_TERMICON(fmi_xml_elmID_termIcon_Icon), 
                FMI_ATTR_TERMICON(fmi_termIcon_attr_id_y2), 1, &(icon->y2), 0);
        if (ret) {
            fmi3_xml_parse_error(context, "Failed to parse complete Icon.");
            context->callbacks->free(icon); // Discard element
            return 1;
        }
        graphRepr->icon = icon;
    } else {
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
        if (fmi3_xml_parse_attr_as_string(context, FMI_ELM_TERMICON(fmi_xml_elmID_termIcon_Terminal), FMI_ATTR_TERMICON(fmi_termIcon_attr_id_name), 1 /* required */, bufName)) {return -1;}

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


/* Top level*/
int fmi_xml_get_has_terminals_and_icons(fmi_xml_terminals_and_icons_t* termIcon) {
    return termIcon ? 1 : 0;
}

/* Graphical Representation */
int fmi_xml_get_has_graphical_representation(fmi_xml_terminals_and_icons_t* termIcon) {
    if (!termIcon) {
        return 0;
    }
    return termIcon->graphicalRepresentation ? 1 : 0;
}

int fmi_xml_get_graphical_representation_system_coordinates(fmi_xml_graphicalRepresentation_t* graphRepr, 
        double* x1, double* y1, double* x2, double* y2) {
    if (!graphRepr || (!graphRepr->coordinateSystem)) {
        return 1;
    }
    *x1 = graphRepr->coordinateSystem->x1;
    *y1 = graphRepr->coordinateSystem->y1;
    *x2 = graphRepr->coordinateSystem->x2;
    *y2 = graphRepr->coordinateSystem->y2;
    return 0;
}

int fmi_xml_get_graphical_representation_suggested_scaling(fmi_xml_graphicalRepresentation_t* graphRepr, 
        double* suggested_scaling) {
    if (!graphRepr || (!graphRepr->coordinateSystem)) {
        return 1;
    }
    *suggested_scaling = graphRepr->coordinateSystem->suggestedScalingFactorTo_mm;
    return 0;
}

int fmi_xml_get_graphical_representation_has_icon(fmi_xml_graphicalRepresentation_t* graphRepr) {
    if (!graphRepr) {
        return 0;
    }
    return graphRepr->icon ? 1 : 0;
}

int fmi_xml_get_graphical_representation_icon_coordinates(fmi_xml_graphicalRepresentation_t* graphRepr, 
        double* x1, double* y1, double* x2, double* y2) {
    if (!graphRepr || (!graphRepr->icon)) {
        return 1;
    }
    *x1 = graphRepr->icon->x1;
    *y1 = graphRepr->icon->y1;
    *x2 = graphRepr->icon->x2;
    *y2 = graphRepr->icon->y2;
    return 0;
}

/* Terminals*/
fmi_xml_terminal_t* fmi_xml_get_terminal_by_name(fmi_xml_terminals_and_icons_t* termIcon, const char* name) {
    if (!name) {
        return NULL;
    }
    jm_named_ptr key, *found;
    key.name = name;
    found = jm_vector_bsearch(jm_named_ptr)(&termIcon->terminalsByName, &key, jm_compare_named);
    return found ? found->ptr : NULL;
}

const char* fmi_xml_get_terminal_name(fmi_xml_terminal_t* term) {
    return term ? term->name : NULL;
}
