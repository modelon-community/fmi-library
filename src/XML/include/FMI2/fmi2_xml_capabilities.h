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

#ifndef FMI2_XML_CAPABILITIES_H
#define FMI2_XML_CAPABILITIES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "FMI2/fmi2_xml_model_description.h"
	/**
		\file fmi2_xml_capabilities.h
		Functions to retrieve capability flags.
	*/
	/**
	\addtogroup fmi2_xml
	@{
	\addtogroup fmi2_xml_capabilities Functions to retrieve capability flags.
	The functions accept a pointer to ::fmi2_xml_capabilities_t returned by fmi2_xml_get_capabilities().
	They return the flags as specified by the FMI 1.0 standard. Default values are returned for model-exachange FMUs.
	@}
	\addtogroup fmi2_xml_capabilities
	@{
	*/
/** \brief Retrieve  canHandleVariableCommunicationStepSize flag. */
int fmi2_xml_get_canHandleVariableCommunicationStepSize(fmi2_xml_capabilities_t* );
	/** \brief Retrieve  canHandleEvents flag. */
int fmi2_xml_get_canHandleEvents(fmi2_xml_capabilities_t* );
	/** \brief Retrieve  canRejectSteps flag. */
int fmi2_xml_get_canRejectSteps(fmi2_xml_capabilities_t* );
	/** \brief Retrieve  canInterpolateInputs flag. */
int fmi2_xml_get_canInterpolateInputs(fmi2_xml_capabilities_t* );
	/** \brief Retrieve  maxOutputDerivativeOrder. */
unsigned int fmi2_xml_get_maxOutputDerivativeOrder(fmi2_xml_capabilities_t* );
	/** \brief Retrieve  canRunAsynchronuously flag. */
int fmi2_xml_get_canRunAsynchronuously(fmi2_xml_capabilities_t* );
	/** \brief Retrieve  canSignalEvents flag. */
int fmi2_xml_get_canSignalEvents(fmi2_xml_capabilities_t* );
	/** \brief Retrieve  canBeInstantiatedOnlyOncePerProcess flag. */
int fmi2_xml_get_canBeInstantiatedOnlyOncePerProcess(fmi2_xml_capabilities_t* );
	/** \brief Retrieve  canNotUseMemoryManagementFunctions flag. */
int fmi2_xml_get_canNotUseMemoryManagementFunctions(fmi2_xml_capabilities_t* );

/** 
@}
*/

#ifdef __cplusplus
}
#endif
#endif /* FMI2_XML_CAPABILITIES_H */
