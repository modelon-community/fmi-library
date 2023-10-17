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

#ifndef FMI3_XML_XMLPARSER_UTIL_H
#define FMI3_XML_XMLPARSER_UTIL_H

#include "fmi3_xml_parser_handler.h"

#ifdef __cplusplus
extern "C" {
#endif

void fmi3_xml_parse_fatal  (fmi3_xml_parser_context_t* context, const char* fmt, ...);
void fmi3_xml_parse_error  (fmi3_xml_parser_context_t* context, const char* fmt, ...);
void fmi3_xml_parse_warning(fmi3_xml_parser_context_t* context, const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* FMI3_XML_XMLPARSER_UTIL_H */
