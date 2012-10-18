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



/** \file fmi_xml_context.h
*  \brief XML context is the entry point to the library. It is used to initialize, get FMI version and start parsing.
**
*/

#ifndef FMI_XML_CONTEXT_H_
#define FMI_XML_CONTEXT_H_

#include <stddef.h>
#include <JM/jm_callbacks.h>
#include <FMI/fmi_version.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \addtogroup fmi_xml FMI XML parsing library.
  @{
  */
/**  \brief Library context is primarily used for handling callbacks
*/
typedef struct fmi_xml_context_t fmi_xml_context_t;

/** \brief Allocate library context 
	@param callbacks - the callbacks to be used for memory allocation and logging. Can be NULL if default callbacks are to be used.
	@return A pointer to the newly allocated ::fmi_xml_context_t or NULL if memory allocation failed.
*/
fmi_xml_context_t* fmi_xml_allocate_context( jm_callbacks* callbacks);

/** \brief Free library context */
void fmi_xml_free_context(fmi_xml_context_t *context);

/** \brief Parse XML file to identify FMI standard version (only beginning of the file is parsed). */
fmi_version_enu_t fmi_xml_get_fmi_version( fmi_xml_context_t*, const char* fileName);

/** ModelDescription is the entry point for the package*/
typedef struct fmi1_xml_model_description_t fmi1_xml_model_description_t;
typedef struct fmi2_xml_model_description_t fmi2_xml_model_description_t;

/** \brief Parse FMI 1.0 XML file and create model description object.

	Errors are reported via the ::jm_callbacks object passed to fmi_xml_allocate_context().
	@return Model description object or NULL if parsing failed.
*/
fmi1_xml_model_description_t* fmi1_xml_parse( fmi_xml_context_t* c, const char* fileName);

/** \brief Parse FMI 2.0 XML file and create model description object.

	Errors are reported via the ::jm_callbacks object passed to fmi_xml_allocate_context().
	@return Model description object or NULL if parsing failed.
*/
fmi2_xml_model_description_t* fmi2_xml_parse( fmi_xml_context_t* c, const char* fileName);
/** @} 
*/
#ifdef __cplusplus
}
#endif
#endif
