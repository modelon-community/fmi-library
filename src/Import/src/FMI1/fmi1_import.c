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

#include <stdio.h>
#include <stdarg.h>

#include <JM/jm_named_ptr.h>
#include "fmi1_import_impl.h"
#include "fmi1_import_variable_list_impl.h"

#include <FMI1/fmi1_types.h>
#include <FMI1/fmi1_functions.h>
#include <FMI1/fmi1_enums.h>
#include <FMI1/fmi1_capi.h>

static const char* module = "FMILIB";

/*#include "fmi1_import_vendor_annotations_impl.h"
#include "fmi1_import_parser.h"
*/
fmi1_import_t* fmi1_import_allocate(jm_callbacks* cb) {
	fmi1_import_t* fmu = (fmi1_import_t*)cb->calloc(1, sizeof(fmi1_import_t));
    
	if(!fmu || (jm_vector_init(char)(&fmu->logMessageBufferCoded,JM_MAX_ERROR_MESSAGE_SIZE,cb) < JM_MAX_ERROR_MESSAGE_SIZE)) {
		jm_log_fatal(cb, module, "Could not allocate memory");
        if(fmu) cb->free(fmu);
		return 0;
	}
    

	fmu->dirPath = 0;
	fmu->location = 0;
	fmu->callbacks = cb;
	fmu->capi = 0;
	fmu->md = fmi1_xml_allocate_model_description(cb);
	fmu->registerGlobally = 0;
	jm_vector_init(char)(&fmu->logMessageBufferExpanded,0,cb);

	if(!fmu->md) {
		cb->free(fmu);
		return 0;
	}

	return fmu;
}

const char* fmi1_import_get_last_error(fmi1_import_t* fmu) {
	return jm_get_last_error(fmu->callbacks);
}

fmi1_import_t* fmi1_import_parse_xml( fmi_import_context_t* context, const char* dirPath) {
	char* xmlPath; 
	char absPath[FILENAME_MAX + 2];
	jm_callbacks* cb;
	fmi1_import_t* fmu;
    int configuration = 0;

	if(!context) return 0;

	cb = context->callbacks; 
	
	xmlPath =  fmi_import_get_model_description_path(dirPath, context->callbacks);

	fmu = fmi1_import_allocate(context->callbacks);

	if(!fmu) {
		context->callbacks->free(xmlPath);
		return 0;
	}
	
	jm_log_verbose( cb, "FMILIB", "Parsing model description XML");

    /* convert the import configuration to the xml configuration */
    if (context->configuration & FMI_IMPORT_NAME_CHECK) {
        configuration |= FMI1_XML_NAME_CHECK;
    }

	if(fmi1_xml_parse_model_description( fmu->md, xmlPath, configuration)) {
		fmi1_import_free(fmu);
		cb->free(xmlPath);
		return 0;
	}
	cb->free(xmlPath);
	
	fmu->dirPath =  (char*)cb->calloc(strlen(dirPath) + 1, sizeof(char));

	if(jm_get_dir_abspath(cb, dirPath, absPath, FILENAME_MAX + 2)) {
		fmu->location = fmi_import_create_URL_from_abs_path(cb, absPath);
	}
	
	if ((fmu->dirPath == NULL) || (fmu->location == 0)){
		jm_log_fatal( cb, "FMILIB", "Could not allocated memory");
		fmi1_import_free(fmu);
		cb->free(xmlPath);
		return 0;
	}
	strcpy(fmu->dirPath, dirPath);

	jm_log_verbose( cb, "FMILIB", "Parsing finished successfully");

	return fmu;
}

void fmi1_import_free(fmi1_import_t* fmu) {
    jm_callbacks* cb = fmu->callbacks;

	if(!fmu) return;
	jm_log_verbose( fmu->callbacks, "FMILIB", "Releasing allocated library resources");	

	fmi1_import_destroy_dllfmu(fmu);
	fmi1_xml_free_model_description(fmu->md);
	jm_vector_free_data(char)(&fmu->logMessageBufferCoded);
	jm_vector_free_data(char)(&fmu->logMessageBufferExpanded);

	cb->free(fmu->dirPath);
	cb->free(fmu->location);
    cb->free(fmu);
}

const char* fmi1_import_get_model_name(fmi1_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi1_xml_get_model_name(fmu->md);
}

const char* fmi1_import_get_model_identifier(fmi1_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi1_xml_get_model_identifier(fmu->md);
}

const char* fmi1_import_get_GUID(fmi1_import_t* fmu){
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
    return fmi1_xml_get_GUID(fmu->md);
}

const char* fmi1_import_get_description(fmi1_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi1_xml_get_description(fmu->md);
}

const char* fmi1_import_get_author(fmi1_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi1_xml_get_author(fmu->md);
}

const char* fmi1_import_get_model_standard_version(fmi1_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi1_xml_get_model_standard_version(fmu->md);
}

const char* fmi1_import_get_model_version(fmi1_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi1_xml_get_model_version(fmu->md);
}

const char* fmi1_import_get_generation_tool(fmi1_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi1_xml_get_generation_tool(fmu->md);
}

const char* fmi1_import_get_generation_date_and_time(fmi1_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi1_xml_get_generation_date_and_time(fmu->md);
}

fmi1_variable_naming_convension_enu_t fmi1_import_get_naming_convention(fmi1_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return fmi1_naming_enu_unknown;
	}
	return fmi1_xml_get_naming_convention(fmu->md);
}

unsigned int fmi1_import_get_number_of_continuous_states(fmi1_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi1_xml_get_number_of_continuous_states(fmu->md);
}

unsigned int fmi1_import_get_number_of_event_indicators(fmi1_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi1_xml_get_number_of_event_indicators(fmu->md);
}

double fmi1_import_get_default_experiment_start(fmi1_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi1_xml_get_default_experiment_start(fmu->md);
}

void fmi1_import_set_default_experiment_start(fmi1_import_t* fmu, double t) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return;
	}
	fmi1_xml_set_default_experiment_start(fmu->md, t);
}

double fmi1_import_get_default_experiment_stop(fmi1_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi1_xml_get_default_experiment_stop(fmu->md);
}

void fmi1_import_set_default_experiment_stop(fmi1_import_t* fmu, double t) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return;
	}
	fmi1_xml_set_default_experiment_stop(fmu->md, t);
}

double fmi1_import_get_default_experiment_tolerance(fmi1_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi1_xml_get_default_experiment_tolerance(fmu->md);
}

void fmi1_import_set_default_experiment_tolerance(fmi1_import_t* fmu, double tol) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return;
	}
	fmi1_xml_set_default_experiment_tolerance(fmu->md, tol);
}

fmi1_import_vendor_list_t* fmi1_import_get_vendor_list(fmi1_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi1_xml_get_vendor_list(fmu->md);
}

unsigned int  fmi1_import_get_number_of_vendors(fmi1_import_vendor_list_t* vl) {
	return fmi1_xml_get_number_of_vendors(vl);
}

fmi1_import_vendor_t* fmi1_import_get_vendor(fmi1_import_vendor_list_t* v, unsigned int  index) {
	return fmi1_xml_get_vendor(v, index);
}

fmi1_import_unit_definitions_t* fmi1_import_get_unit_definitions(fmi1_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi1_xml_get_unit_definitions(fmu->md);
}

unsigned int  fmi1_import_get_unit_definitions_number(fmi1_import_unit_definitions_t* ud) {
	return fmi1_xml_get_unit_definitions_number(ud);
}

fmi1_import_type_definitions_t* fmi1_import_get_type_definitions(fmi1_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi1_xml_get_type_definitions(fmu->md);
}

/* Get the list of all the variables in the model */
fmi1_import_variable_list_t* fmi1_import_get_variable_list(fmi1_import_t* fmu) {
	jm_vector(jm_voidp)* vars;
    fmi1_import_variable_list_t* vl;
    size_t nv, i;
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	vars = fmi1_xml_get_variables_original_order(fmu->md);
	if(vars)
		nv = jm_vector_get_size(jm_voidp)(vars);
	else
		nv = 0;
    vl = fmi1_import_alloc_variable_list(fmu, nv);
    if(!vl) return 0;
    for(i = 0; i< nv; i++) {
        jm_vector_set_item(jm_voidp)(&vl->variables, i, jm_vector_get_item(jm_voidp)(vars, i));
    }
    return vl;
}

/* Get the list of all the variables in the model in alphabetical order */
fmi1_import_variable_list_t* fmi1_import_get_variable_list_alphabetical_order(fmi1_import_t* fmu) {
	jm_vector(jm_named_ptr)* vars;
    fmi1_import_variable_list_t* vl;
    size_t nv, i;
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	vars = fmi1_xml_get_variables_alphabetical_order(fmu->md);
    nv = jm_vector_get_size(jm_named_ptr)(vars);
    vl = fmi1_import_alloc_variable_list(fmu, nv);
    if(!vl) return 0;
    for(i = 0; i< nv; i++) {
		jm_vector_set_item(jm_voidp)(&vl->variables, i, jm_vector_get_item(jm_named_ptr)(vars, i).ptr);
    }
    return vl;
}
