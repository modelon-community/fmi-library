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

/** \file fmi_xml_terminals_and_icons.h
*  \brief Public interface to the FMI XML C-library.
*/

#ifndef FMI_XML_TERMINALS_AND_ICONS_H_
#define FMI_XML_TERMINALS_AND_ICONS_H_

#include <JM/jm_callbacks.h>
#include <FMI3/fmi3_xml_callbacks.h>
#include <FMI2/fmi2_xml_model_description.h>
#include <FMI3/fmi3_xml_model_description.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
    \addtogroup fmi_xml
    @{
    \addtogroup fmi_xml_terminals_and_icons Functions for handling terminals and icons
    @{
*/

/* Structure encapsulating terminals and icons information */
typedef struct fmi_xml_terminals_and_icons_t fmi_xml_terminals_and_icons_t;
/* Structure encapsulating terminal information */
typedef struct fmi_xml_terminal_t fmi_xml_terminal_t;

fmi_xml_terminals_and_icons_t* fmi_xml_allocate_terminals_and_icons(jm_callbacks* callbacks);
void fmi_xml_free_terminals_and_icons(fmi_xml_terminals_and_icons_t* termIcon);

/**
   \brief Parse XML file
   Repeated calls invalidate the data structures created with the previous call to fmiParseXML,
   i.e., fmiClearModelDescrition is automatically called before reading in the new file.

    @param termIcon A terminalsAndIcons object as returned by #fmi_xml_allocate_terminals_and_icons.
    @param fileName A name (full path) of the terminalsAndIcons.xml file.
    @param xml_callbacks Callbacks to use for processing annotations (may be NULL).
    @return 0 if parsing was successful. Non-zero value indicates an error.
*/
int fmi3_xml_parse_terminals_and_icons(fmi_xml_terminals_and_icons_t* termIcon,
                                       const char* fileName,
                                       fmi_termicon_xml_callbacks_t* xml_callbacks);
int fmi2_xml_terminals_and_icons_set_model_description(fmi_xml_terminals_and_icons_t* termIcon,
                                                       fmi2_xml_model_description_t* md);
int fmi3_xml_terminals_and_icons_set_model_description(fmi_xml_terminals_and_icons_t* termIcon,
                                                       fmi3_xml_model_description_t* md);

int fmi_xml_get_has_terminals_and_icons(fmi_xml_terminals_and_icons_t* termIcon);
fmi_xml_terminal_t* fmi_xml_get_terminal_by_name(fmi_xml_terminals_and_icons_t* termIcon, const char* name);
const char* fmi_xml_get_terminal_name(fmi_xml_terminal_t* term);

/** @} */

#ifdef __cplusplus
}
#endif

#endif // FMI_XML_TERMINALS_AND_ICONS_H_
