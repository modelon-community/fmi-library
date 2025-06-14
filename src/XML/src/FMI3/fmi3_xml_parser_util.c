/*
    Copyright (C) 2024 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#include "fmi3_xml_parser_util.h"
#include "JM/jm_portability.h"

static const char* module = "FMI3XML";

void fmi3_xml_parse_fatal(fmi3_xml_parser_context_t* context, const char* fmt, ...) {
    va_list args;
    va_start (args, fmt);
    jm_log_fatal_v(context->callbacks, module, fmt, args);
    va_end (args);
    XML_StopParser(context->parser,0);
}

void fmi3_xml_parse_error(fmi3_xml_parser_context_t* context, const char* fmt, ...) {
    va_list args;
    va_start (args, fmt);
    if (context->parser)
        jm_log_info(context->callbacks, module, "[Line:%lu] Detected during parsing:", XML_GetCurrentLineNumber(context->parser));
    jm_log_error_v(context->callbacks, module, fmt, args);
    va_end (args);
}

void fmi3_xml_parse_warning(fmi3_xml_parser_context_t* context, const char* fmt, ...) {
    va_list args;
    va_start (args, fmt);
    if (context->parser)
        jm_log_info(context->callbacks, module, "[Line:%lu] Detected during parsing:", XML_GetCurrentLineNumber(context->parser));
    jm_log_warning_v(context->callbacks, module, fmt, args);
    va_end (args);
}
