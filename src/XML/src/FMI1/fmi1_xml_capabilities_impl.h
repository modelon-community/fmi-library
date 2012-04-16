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

#ifndef FMI1_XML_CAPABILITIES_IMPL_H
#define FMI1_XML_CAPABILITIES_IMPL_H

#include <FMI1/fmi1_xml_capabilities.h>
#ifdef __cplusplus
extern "C" {
#endif

struct fmi1_xml_capabilities_t {
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

void fmi1_xml_init_capabilities(fmi1_xml_capabilities_t* c);

#ifdef __cplusplus
}
#endif
#endif /* FMI1_XML_CAPABILITIES_IMPL_H */
