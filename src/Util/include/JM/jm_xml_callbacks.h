/*
    Copyright (C) 2012 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

     This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#ifndef JM_XML_CALLBACKS_H
#define JM_XML_CALLBACKS_H

#include <fmilib_config.h>

#ifdef __cplusplus
extern "C" {
#endif
/** \file jm_xml_callbacks.h Definition of ::jm_xml_callbacks_t and supporting functions
	*
	* \addtogroup jm_utils
	* @{
		\addtogroup jm_xml_callbacks
	* @}
*/
/** \addtogroup jm_xml_callbacks Definition of XML callbacks struct
* @{  */
typedef struct jm_xml_callbacks_t jm_xml_callbacks_t;

/** \name XML handling callbacks
* @{ 
*/
/** \brief Handle start of an XML element.
*
*  Arguments are: context as specified when setting up the callbacks, elm - name of the element, attr - attributes (names and values).
*  The function should return 0 on success or error code on exit (in which case parsing will be aborted).
*/
typedef int (*jm_xml_element_start_handle_ft)(	void *context, const char *elm, const char **attr);

/** \brief Handle data of an XML element.
*
*  Arguments are: context as specified when setting up the callbacks, s - data string, len - length of the data.
*  The function should return 0 on success or error code on exit (in which case parsing will be aborted).
*/
typedef int (*jm_xml_element_data_handle_ft)(void* context, const char *s, int len);

/** \brief Handle end of an XML element.
*
*  Arguments are: context as specified when setting up the callbacks, elm - name of the element.
*  The function should return 0 on success or error code on exit (in which case parsing will be aborted).
*/
typedef int (*jm_xml_element_end_handle_ft)(void *context, const char *elm);

/** \brief XML callbacks are used to process parts of XML that are not handled by the library */
struct jm_xml_callbacks_t {
	jm_xml_element_start_handle_ft startHandle;
	jm_xml_element_data_handle_ft  dataHandle;
	jm_xml_element_end_handle_ft   endHandle;
	void* context;
};
/* @}
*/

#ifdef __cplusplus
}
#endif
/* JM_CONTEXT_H */
#endif
