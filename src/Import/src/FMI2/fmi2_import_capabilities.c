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

#include "fmi2_import_impl.h"

int fmi2_import_get_canHandleVariableCommunicationStepSize(fmi2_import_capabilities_t* c ) {
    return fmi2_xml_get_canHandleVariableCommunicationStepSize(c);
}
int fmi2_import_get_canHandleEvents(fmi2_import_capabilities_t* c ) {
    return fmi2_xml_get_canHandleEvents(c);
}
int fmi2_import_get_canRejectSteps(fmi2_import_capabilities_t* c ) {
    return fmi2_xml_get_canRejectSteps( c );
}
int fmi2_import_get_canInterpolateInputs(fmi2_import_capabilities_t* c ) {
    return fmi2_xml_get_canInterpolateInputs(c);
}
unsigned int fmi2_import_get_maxOutputDerivativeOrder(fmi2_import_capabilities_t* c ) {
    return fmi2_xml_get_maxOutputDerivativeOrder(c);
}
int fmi2_import_get_canRunAsynchronuously(fmi2_import_capabilities_t* c ) {
    return fmi2_xml_get_canRunAsynchronuously(c);
}
int fmi2_import_get_canSignalEvents(fmi2_import_capabilities_t* c ) {
    return fmi2_xml_get_canSignalEvents(c);
}
int fmi2_import_get_canBeInstantiatedOnlyOncePerProcess(fmi2_import_capabilities_t* c ) {
    return fmi2_xml_get_canBeInstantiatedOnlyOncePerProcess(c);
}
int fmi2_import_get_canNotUseMemoryManagementFunctions(fmi2_import_capabilities_t* c ) {
    return fmi2_xml_get_canNotUseMemoryManagementFunctions(c);
}
