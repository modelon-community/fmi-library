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

#include "fmi2_xml_parser.h"
#include "fmi2_xml_capabilities_impl.h"
#include "fmi2_xml_model_description_impl.h"

int fmi2_xml_get_canHandleVariableCommunicationStepSize(fmi2_xml_capabilities_t* c ) {
    return c->canHandleVariableCommunicationStepSize;
}
int fmi2_xml_get_canHandleEvents(fmi2_xml_capabilities_t* c ) {
    return c->canHandleEvents;
}
int fmi2_xml_get_canRejectSteps(fmi2_xml_capabilities_t* c ) {
    return c->canRejectSteps;
}
int fmi2_xml_get_canInterpolateInputs(fmi2_xml_capabilities_t* c ) {
    return c->canInterpolateInputs;
}
unsigned int fmi2_xml_get_maxOutputDerivativeOrder(fmi2_xml_capabilities_t* c ) {
    return c->maxOutputDerivativeOrder;
}
int fmi2_xml_get_canRunAsynchronuously(fmi2_xml_capabilities_t* c ) {
    return c->canRunAsynchronuously;
}
int fmi2_xml_get_canSignalEvents(fmi2_xml_capabilities_t* c ) {
    return c->canSignalEvents;
}
int fmi2_xml_get_canBeInstantiatedOnlyOncePerProcess(fmi2_xml_capabilities_t* c ) {
    return c->canBeInstantiatedOnlyOncePerProcess;
}
int fmi2_xml_get_canNotUseMemoryManagementFunctions(fmi2_xml_capabilities_t* c ) {
    return c->canNotUseMemoryManagementFunctions;
}

void fmi2_xml_init_capabilities(fmi2_xml_capabilities_t* c) {
    memset(c,0,sizeof(fmi2_xml_capabilities_t));
}


int fmi2_xml_handle_Capabilities(fmi2_xml_parser_context_t *context, const char* data) {
    fmi2_xml_model_description_t* md = context->modelDescription;
    if(!data) {
        return (
                    /* <xs:attribute name="canHandleVariableCommunicationStepSize" type="xs:boolean" default="false"/> */
                    fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_Capabilities, fmi_attr_id_canHandleVariableCommunicationStepSize,0,
                                             &md->capabilities.canHandleVariableCommunicationStepSize,0) ||
                    /* <xs:attribute name="canHandleEvents" type="xs:boolean" default="false"/> */
                    fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_Capabilities, fmi_attr_id_canHandleEvents,0,
                                             &md->capabilities.canHandleEvents,0) ||
                    /* <xs:attribute name="canRejectSteps" type="xs:boolean" default="false"/> */
                    fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_Capabilities, fmi_attr_id_canRejectSteps,0,
                                             &md->capabilities.canRejectSteps,0) ||
                    /* <xs:attribute name="canInterpolateInputs" type="xs:boolean" default="false"/> */
                    fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_Capabilities, fmi_attr_id_canInterpolateInputs,0,
                                             &md->capabilities.canInterpolateInputs,0) ||
                    /* <xs:attribute name="maxOutputDerivativeOrder" type="xs:unsignedInt" default="0"/> */
                    fmi2_xml_set_attr_uint(context,fmi2_xml_elmID_Capabilities, fmi_attr_id_maxOutputDerivativeOrder,0,
                                             &md->capabilities.maxOutputDerivativeOrder,0) ||
                    /* <xs:attribute name="canRunAsynchronuously" type="xs:boolean" default="false"/> */
                    fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_Capabilities, fmi_attr_id_canRunAsynchronuously,0,
                                             &md->capabilities.canRunAsynchronuously,0) ||
                    /* <xs:attribute name="canSignalEvents" type="xs:boolean" default="false"/> */
                    fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_Capabilities, fmi_attr_id_canSignalEvents,0,
                                             &md->capabilities.canSignalEvents,0) ||
                    /* <xs:attribute name="canBeInstantiatedOnlyOncePerProcess" type="xs:boolean" default="false"/> */
                    fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_Capabilities, fmi_attr_id_canBeInstantiatedOnlyOncePerProcess,0,
                                             &md->capabilities.canBeInstantiatedOnlyOncePerProcess,0) ||
                    /* <xs:attribute name="canNotUseMemoryManagementFunctions" type="xs:boolean" default="false"/> */
                    fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_Capabilities, fmi_attr_id_canNotUseMemoryManagementFunctions,0,
                                             &md->capabilities.canNotUseMemoryManagementFunctions,0)
                    );
    }
    else {
        /* might give out a warning if(data[0] != 0) */
    }
    return 0;
}
