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

#ifndef FMI2_IMPORT_CAPABILITIES_H
#define FMI2_IMPORT_CAPABILITIES_H

#include <fmilib_config.h>

#ifdef __cplusplus
extern "C" {
#endif

	/**
		\file fmi2_import_capabilities.h
		Functions to retrieve capability flags.
	*/
	/**
	\addtogroup fmi2_import
	@{
	\addtogroup fmi2_import_capabilities Functions to retrieve capability flags.
	The functions accept a pointer to ::fmi2_import_capabilities_t returned by fmi2_import_get_capabilities().
	They return the flags as specified by the FMI 1.0 standard. Default values are returned for model-exachange FMUs.
	@}
	\addtogroup fmi2_import_capabilities
	@{
	*/
/**\name FMU capabilities flags */
/**@{ */
/** \brief A container for all the capability flags */
typedef struct fmi2_xml_capabilities_t fmi2_import_capabilities_t;
/** @} */

	/** \brief Retrieve  canHandleVariableCommunicationStepSize flag. */
FMILIB_EXPORT int fmi2_import_get_canHandleVariableCommunicationStepSize(fmi2_import_capabilities_t* );
	/** \brief Retrieve  canHandleEvents flag. */
FMILIB_EXPORT int fmi2_import_get_canHandleEvents(fmi2_import_capabilities_t* );
	/** \brief Retrieve  canRejectSteps flag. */
FMILIB_EXPORT int fmi2_import_get_canRejectSteps(fmi2_import_capabilities_t* );
	/** \brief Retrieve  canInterpolateInputs flag. */
FMILIB_EXPORT int fmi2_import_get_canInterpolateInputs(fmi2_import_capabilities_t* );
	/** \brief Retrieve  maxOutputDerivativeOrder. */
FMILIB_EXPORT unsigned int fmi2_import_get_maxOutputDerivativeOrder(fmi2_import_capabilities_t* );
	/** \brief Retrieve  canRunAsynchronuously flag. */
FMILIB_EXPORT int fmi2_import_get_canRunAsynchronuously(fmi2_import_capabilities_t* );
	/** \brief Retrieve  canSignalEvents flag. */
FMILIB_EXPORT int fmi2_import_get_canSignalEvents(fmi2_import_capabilities_t* );
	/** \brief Retrieve  canBeInstantiatedOnlyOncePerProcess flag. */
FMILIB_EXPORT int fmi2_import_get_canBeInstantiatedOnlyOncePerProcess(fmi2_import_capabilities_t* );
	/** \brief Retrieve  canNotUseMemoryManagementFunctions flag. */
FMILIB_EXPORT int fmi2_import_get_canNotUseMemoryManagementFunctions(fmi2_import_capabilities_t* );
/** 
@}
*/

#ifdef __cplusplus
}
#endif
#endif /* FMI2_IMPORT_CAPABILITIES_H */
