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

#ifndef FMI2_XML_CAPABILITIES_IMPL_H
#define FMI2_XML_CAPABILITIES_IMPL_H

#include <FMI2/fmi2_xml_capabilities.h>
#ifdef __cplusplus
extern "C" {
#endif

struct fmi2_xml_capabilities_t {
    int canHandleVariableCommunicationStepSize;
    int canHandleEvents;
    int canRejectSteps;
    int canInterpolateInputs;
    unsigned int maxOutputDerivativeOrder;
    int canRunAsynchronuously;
    int canSignalEvents;
    int canBeInstantiatedOnlyOncePerProcess;
    int canNotUseMemoryManagementFunctions;
};

void fmi2_xml_init_capabilities(fmi2_xml_capabilities_t* c);

#ifdef __cplusplus
}
#endif
#endif /* FMI2_XML_CAPABILITIES_IMPL_H */
