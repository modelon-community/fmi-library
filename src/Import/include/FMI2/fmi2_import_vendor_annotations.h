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

/** \file fmi2_import_vendor_annotations.h
*  \brief Public interface to the FMI XML C-library. Handling of vendor annotations.
*/

#ifndef FMI2_IMPORT_VENDORANNOTATIONS_H_
#define FMI2_IMPORT_VENDORANNOTATIONS_H_

#include <fmilib_config.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
* \addtogroup  fmi2_import
* @{
*	\addtogroup  fmi2_import_annotations Basic support for vendor annotations.
*@}
 * \addtogroup  fmi2_import_annotations Basic support for vendor annotations.
 @{ 
 */

/** 
\name Vendor annotation supporting structures
*/
/**@{ */
	/** \brief  Opaque list of vendor annotations. */
typedef struct fmi2_xml_vendor_list_t fmi2_import_vendor_list_t;
	/** \brief Opaque vendor object. */
typedef struct fmi2_xml_vendor_t fmi2_import_vendor_t;
	/** \brief Opaque annotation object. */
typedef struct fmi2_xml_annotation_t fmi2_import_annotation_t;
/**@} */

/** \brief Get the number of different vendors */
FMILIB_EXPORT unsigned int  fmi2_import_get_number_of_vendors(fmi2_import_vendor_list_t*);

/** \brief Get the annotations associated with vendor specified by the index */
FMILIB_EXPORT fmi2_import_vendor_t* fmi2_import_get_vendor(fmi2_import_vendor_list_t*, unsigned int  index);

/** \brief Get the vendor name */
FMILIB_EXPORT const char* fmi2_import_get_vendor_name(fmi2_import_vendor_t*);

/** \brief Get the number of annotations provided for the vendor */
FMILIB_EXPORT unsigned int  fmi2_import_get_number_of_vendor_annotations(fmi2_import_vendor_t*);

/** \brief Get an annotation object for the vendor by index

	Note: Annotations may later be used in other places but have common interface name-value 
*/
FMILIB_EXPORT fmi2_import_annotation_t* fmi2_import_get_vendor_annotation(fmi2_import_vendor_t*, unsigned int  index);

/** \brief Get the name of the annotation */
FMILIB_EXPORT const char* fmi2_import_get_annotation_name(fmi2_import_annotation_t*);

/** \brief Get the value for the annotation */
FMILIB_EXPORT const char* fmi2_import_get_annotation_value(fmi2_import_annotation_t*);

/** \brief Get the list of all the vendor annotations present in the XML file */
FMILIB_EXPORT fmi2_import_vendor_list_t* fmi2_import_get_vendor_list(fmi2_import_t* fmu);

/** @} */
#ifdef __cplusplus
}
#endif
#endif
