/*
    Copyright (C) 2012 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/** \file fmi1_import_vendor_annotations.h
*  \brief Public interface to the FMI XML C-library. Handling of vendor annotations.
*/

#ifndef FMI1_IMPORT_VENDORANNOTATIONS_H_
#define FMI1_IMPORT_VENDORANNOTATIONS_H_

#include "fmi1_import.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* \addtogroup  fmi1_import
* @{
*	\addtogroup  fmi1_import_annotations Basic support for vendor annotations.
*@}
 * \addtogroup  fmi1_import_annotations Basic support for vendor annotations.
 @{ 
 */

/** \brief Get the list of all the vendor annotations present in the XML file */
FMILIB_EXPORT fmi1_import_vendor_list_t* fmi1_import_get_vendor_list(fmi1_import_t* fmu);

/** \brief Get the number of different vendors */
FMILIB_EXPORT unsigned int  fmi1_import_get_number_of_vendors(fmi1_import_vendor_list_t*);

/** \brief Get the annotations associated with vendor specified by the index */
FMILIB_EXPORT fmi1_import_vendor_t* fmi1_import_get_vendor(fmi1_import_vendor_list_t*, unsigned int  index);

/** \brief Get the vendor name */
FMILIB_EXPORT const char* fmi1_import_get_vendor_name(fmi1_import_vendor_t*);

/** \brief Get the number of annotations provided for the vendor */
FMILIB_EXPORT unsigned int  fmi1_import_get_number_of_vendor_annotations(fmi1_import_vendor_t*);

/** \brief Get an annotation object for the vendor by index

	Note: Annotations may later be used in other places but have common interface name-value 
*/
FMILIB_EXPORT fmi1_import_annotation_t* fmi1_import_get_vendor_annotation(fmi1_import_vendor_t*, unsigned int  index);

/** \brief Get the name of the annotation */
FMILIB_EXPORT const char* fmi1_import_get_annotation_name(fmi1_import_annotation_t*);

/** \brief Get the value for the annotation */
FMILIB_EXPORT const char* fmi1_import_get_annotation_value(fmi1_import_annotation_t*);

/** @} */
#ifdef __cplusplus
}
#endif
#endif
