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
#include "fmi2_import_impl.h"
#include "fmi2_import_variable_list_impl.h"

#include <FMI2/fmi2_types.h>
#include <FMI2/fmi2_functions.h>
#include <FMI2/fmi2_enums.h>
#include <FMI2/fmi2_capi.h>

static const char* module = "FMILIB";

/*#include "fmi2_import_vendor_annotations_impl.h"
#include "fmi2_import_parser.h"
*/
fmi2_import_t* fmi2_import_allocate(jm_callbacks* cb) {
	fmi2_import_t* fmu = (fmi2_import_t*)cb->calloc(1, sizeof(fmi2_import_t));

	if(!fmu) {
		jm_log_fatal(cb, module, "Could not allocate memory");
		return 0;
	}
	fmu->dirPath = 0;
	fmu->callbacks = cb;
	fmu->capi = 0;
	fmu->md = fmi2_xml_allocate_model_description(cb);
	jm_vector_init(char)(&fmu->logMessageBuffer,0,cb);

	if(!fmu->md) {
		cb->free(fmu);
		return 0;
	}

	return fmu;
}

const char* fmi2_import_get_last_error(fmi2_import_t* fmu) {
	return jm_get_last_error(fmu->callbacks);
}

fmi2_import_t* fmi2_import_parse_xml( fmi_import_context_t* context, const char* dirPath, jm_xml_callbacks_t* xml_callbacks) {
	char* xmlPath =  fmi_import_get_model_description_path(dirPath, context->callbacks);

	fmi2_import_t* fmu = fmi2_import_allocate(context->callbacks);

	if(!fmu) {
		context->callbacks->free(xmlPath);
		return 0;
	}
	
	jm_log_verbose( context->callbacks, "FMILIB", "Parsing model description XML");

	if(fmi2_xml_parse_model_description( fmu->md, xmlPath, xml_callbacks)) {
		fmi2_import_free(fmu);
		context->callbacks->free(xmlPath);
		return 0;
	}
	context->callbacks->free(xmlPath);
	
	fmu->dirPath =  context->callbacks->calloc(strlen(dirPath) + 1, sizeof(char));
	if (fmu->dirPath == NULL) {
		jm_log_fatal( context->callbacks, "FMILIB", "Could not allocated memory");
		fmi2_import_free(fmu);
		context->callbacks->free(xmlPath);
		return 0;
	}
	strcpy(fmu->dirPath, dirPath);

	jm_log_verbose( context->callbacks, "FMILIB", "Parsing finished successfully");

	return fmu;
}

void fmi2_import_free(fmi2_import_t* fmu) {
    jm_callbacks* cb = fmu->callbacks;

	if(!fmu) return;
	jm_log_verbose( fmu->callbacks, "FMILIB", "Releasing allocated library resources");	

	fmi2_import_destroy_dllfmu(fmu);
	fmi2_xml_free_model_description(fmu->md);
	jm_vector_free_data(char)(&fmu->logMessageBuffer);

	cb->free(fmu->dirPath);
    cb->free(fmu);
}

const char* fmi2_import_get_model_name(fmi2_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi2_xml_get_model_name(fmu->md);
}

unsigned int fmi2_import_get_capability(fmi2_import_t* fmu , fmi2_capabilities_enu_t id) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi2_xml_get_capability(fmu->md, id);
}


const char* fmi2_import_get_model_identifier_ME(fmi2_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi2_xml_get_model_identifier_ME(fmu->md);
}

const char* fmi2_import_get_model_identifier_CS(fmi2_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi2_xml_get_model_identifier_CS(fmu->md);
}

const char* fmi2_import_get_GUID(fmi2_import_t* fmu){
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
    return fmi2_xml_get_GUID(fmu->md);
}

const char* fmi2_import_get_description(fmi2_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi2_xml_get_description(fmu->md);
}

const char* fmi2_import_get_author(fmi2_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi2_xml_get_author(fmu->md);
}

const char* fmi2_import_get_license(fmi2_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi2_xml_get_license(fmu->md);
}
const char* fmi2_import_get_copyright(fmi2_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi2_xml_get_copyright(fmu->md);
}

const char* fmi2_import_get_model_standard_version(fmi2_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi2_xml_get_model_standard_version(fmu->md);
}

const char* fmi2_import_get_model_version(fmi2_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi2_xml_get_model_version(fmu->md);
}

const char* fmi2_import_get_generation_tool(fmi2_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi2_xml_get_generation_tool(fmu->md);
}

const char* fmi2_import_get_generation_date_and_time(fmi2_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi2_xml_get_generation_date_and_time(fmu->md);
}

fmi2_variable_naming_convension_enu_t fmi2_import_get_naming_convention(fmi2_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return fmi2_naming_enu_unknown;
	}
	return fmi2_xml_get_naming_convention(fmu->md);
}

unsigned int fmi2_import_get_number_of_continuous_states(fmi2_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi2_xml_get_number_of_continuous_states(fmu->md);
}

unsigned int fmi2_import_get_number_of_event_indicators(fmi2_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi2_xml_get_number_of_event_indicators(fmu->md);
}

double fmi2_import_get_default_experiment_start(fmi2_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi2_xml_get_default_experiment_start(fmu->md);
}

void fmi2_import_set_default_experiment_start(fmi2_import_t* fmu, double t) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return;
	}
	fmi2_xml_set_default_experiment_start(fmu->md, t);
}

double fmi2_import_get_default_experiment_stop(fmi2_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi2_xml_get_default_experiment_stop(fmu->md);
}

void fmi2_import_set_default_experiment_stop(fmi2_import_t* fmu, double t) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return;
	}
	fmi2_xml_set_default_experiment_stop(fmu->md, t);
}

double fmi2_import_get_default_experiment_tolerance(fmi2_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi2_xml_get_default_experiment_tolerance(fmu->md);
}

void fmi2_import_set_default_experiment_tolerance(fmi2_import_t* fmu, double tol) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return;
	}
	fmi2_xml_set_default_experiment_tolerance(fmu->md, tol);
}

fmi2_import_unit_definitions_t* fmi2_import_get_unit_definitions(fmi2_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi2_xml_get_unit_definitions(fmu->md);
}

unsigned int  fmi2_import_get_unit_definitions_number(fmi2_import_unit_definitions_t* ud) {
	return fmi2_xml_get_unit_definitions_number(ud);
}

fmi2_import_type_definitions_t* fmi2_import_get_type_definitions(fmi2_import_t* fmu) {
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi2_xml_get_type_definitions(fmu->md);
}

/* Get the list of all the variables in the model */
fmi2_import_variable_list_t* fmi2_import_get_variable_list(fmi2_import_t* fmu) {
	jm_vector(jm_voidp)* vars;
    fmi2_import_variable_list_t* vl;
    size_t nv, i;
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	vars = fmi2_xml_get_variables_original_order(fmu->md);
    nv = jm_vector_get_size(jm_voidp)(vars);
    vl = fmi2_import_alloc_variable_list(fmu, nv);
    if(!vl) return 0;
    for(i = 0; i< nv; i++) {
        jm_vector_set_item(jm_voidp)(&vl->variables, i, jm_vector_get_item(jm_voidp)(vars, i));
    }
    return vl;
}

fmi2_fmu_kind_enu_t fmi2_import_get_fmu_kind(fmi2_import_t* fmu) {
    return fmi2_xml_get_fmu_kind(fmu->md);
}

size_t fmi2_import_get_vendors_num(fmi2_import_t* fmu){
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi2_xml_get_vendors_num(fmu->md);
}

const char* fmi2_import_get_vendor_name(fmi2_import_t* fmu, unsigned int  index){
	if(!fmu->md) {
		jm_log_error(fmu->callbacks, module,"No FMU is loaded");
		return 0;
	}
	return fmi2_xml_get_vendor_name(fmu->md, index);
}
