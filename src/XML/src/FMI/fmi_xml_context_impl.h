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

#ifndef FMI_XML_CONTEXT_IMPL_H
#define FMI_XML_CONTEXT_IMPL_H

#include <expat.h>
#include <fmilib_config.h>
#include <JM/jm_vector.h>
#include <JM/jm_stack.h>
#include <JM/jm_named_ptr.h>

#include <FMI1/fmi1_xml_model_description.h>
#include <FMI/fmi_xml_context.h>

#ifdef __cplusplus
extern "C" {
#endif

struct fmi_xml_context_t {
	jm_callbacks* callbacks;
	
    XML_Parser parser;

	fmi_version_enu_t fmi_version;
};

#ifdef __cplusplus
}
#endif

#endif /* FMI_XML_CONTEXT_H */