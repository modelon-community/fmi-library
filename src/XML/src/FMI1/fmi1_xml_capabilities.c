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

#include "fmi1_xml_parser.h"
#include "fmi1_xml_capabilities_impl.h"
#include "fmi1_xml_model_description_impl.h"

int fmi1_xml_get_canHandleVariableCommunicationStepSize(fmi1_xml_capabilities_t* c ) {
    return c->canHandleVariableCommunicationStepSize;
}
int fmi1_xml_get_canHandleEvents(fmi1_xml_capabilities_t* c ) {
    return c->canHandleEvents;
}
int fmi1_xml_get_canRejectSteps(fmi1_xml_capabilities_t* c ) {
    return c->canRejectSteps;
}
int fmi1_xml_get_canInterpolateInputs(fmi1_xml_capabilities_t* c ) {
    return c->canInterpolateInputs;
}
unsigned int fmi1_xml_get_maxOutputDerivativeOrder(fmi1_xml_capabilities_t* c ) {
    return c->maxOutputDerivativeOrder;
}
int fmi1_xml_get_canRunAsynchronuously(fmi1_xml_capabilities_t* c ) {
    return c->canRunAsynchronuously;
}
int fmi1_xml_get_canSignalEvents(fmi1_xml_capabilities_t* c ) {
    return c->canSignalEvents;
}
int fmi1_xml_get_canBeInstantiatedOnlyOncePerProcess(fmi1_xml_capabilities_t* c ) {
    return c->canBeInstantiatedOnlyOncePerProcess;
}
int fmi1_xml_get_canNotUseMemoryManagementFunctions(fmi1_xml_capabilities_t* c ) {
    return c->canNotUseMemoryManagementFunctions;
}

void fmi1_xml_init_capabilities(fmi1_xml_capabilities_t* c) {
    memset(c,0,sizeof(fmi1_xml_capabilities_t));
}


int fmi1_xml_handle_Capabilities(fmi1_xml_parser_context_t *context, const char* data) {
    fmi1_xml_model_description_t* md = context->modelDescription;
    if(!data) {
        if(   (context -> currentElmHandle != fmi1_xml_handle_CoSimulation_StandAlone)
           && (context -> currentElmHandle != fmi1_xml_handle_CoSimulation_Tool))
        {
            fmi1_xml_parse_fatal(context, "Capabilities XML element must be a part of CoSimulation specification");
            return -1;
        }
        return (
                    /* <xs:attribute name="canHandleVariableCommunicationStepSize" type="xs:boolean" default="false"/> */
                    fmi1_xml_set_attr_boolean(context,fmi1_xml_elmID_Capabilities, fmi_attr_id_canHandleVariableCommunicationStepSize,0,
                                             &md->capabilities.canHandleVariableCommunicationStepSize,0) ||
                    /* <xs:attribute name="canHandleEvents" type="xs:boolean" default="false"/> */
                    fmi1_xml_set_attr_boolean(context,fmi1_xml_elmID_Capabilities, fmi_attr_id_canHandleEvents,0,
                                             &md->capabilities.canHandleEvents,0) ||
                    /* <xs:attribute name="canRejectSteps" type="xs:boolean" default="false"/> */
                    fmi1_xml_set_attr_boolean(context,fmi1_xml_elmID_Capabilities, fmi_attr_id_canRejectSteps,0,
                                             &md->capabilities.canRejectSteps,0) ||
                    /* <xs:attribute name="canInterpolateInputs" type="xs:boolean" default="false"/> */
                    fmi1_xml_set_attr_boolean(context,fmi1_xml_elmID_Capabilities, fmi_attr_id_canInterpolateInputs,0,
                                             &md->capabilities.canInterpolateInputs,0) ||
                    /* <xs:attribute name="maxOutputDerivativeOrder" type="xs:unsignedInt" default="0"/> */
                    fmi1_xml_set_attr_uint(context,fmi1_xml_elmID_Capabilities, fmi_attr_id_maxOutputDerivativeOrder,0,
                                             &md->capabilities.maxOutputDerivativeOrder,0) ||
                    /* <xs:attribute name="canRunAsynchronuously" type="xs:boolean" default="false"/> */
                    fmi1_xml_set_attr_boolean(context,fmi1_xml_elmID_Capabilities, fmi_attr_id_canRunAsynchronuously,0,
                                             &md->capabilities.canRunAsynchronuously,0) ||
                    /* <xs:attribute name="canSignalEvents" type="xs:boolean" default="false"/> */
                    fmi1_xml_set_attr_boolean(context,fmi1_xml_elmID_Capabilities, fmi_attr_id_canSignalEvents,0,
                                             &md->capabilities.canSignalEvents,0) ||
                    /* <xs:attribute name="canBeInstantiatedOnlyOncePerProcess" type="xs:boolean" default="false"/> */
                    fmi1_xml_set_attr_boolean(context,fmi1_xml_elmID_Capabilities, fmi_attr_id_canBeInstantiatedOnlyOncePerProcess,0,
                                             &md->capabilities.canBeInstantiatedOnlyOncePerProcess,0) ||
                    /* <xs:attribute name="canNotUseMemoryManagementFunctions" type="xs:boolean" default="false"/> */
                    fmi1_xml_set_attr_boolean(context,fmi1_xml_elmID_Capabilities, fmi_attr_id_canNotUseMemoryManagementFunctions,0,
                                             &md->capabilities.canNotUseMemoryManagementFunctions,0)
                    );
    }
    else {
        /* might give out a warning if(data[0] != 0) */
    }
    return 0;
}
