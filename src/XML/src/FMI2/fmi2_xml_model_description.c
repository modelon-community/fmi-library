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


#include <JM/jm_named_ptr.h>
#include "fmi2_xml_model_description_impl.h"
#include "fmi2_xml_model_structure_impl.h"
#include "fmi2_xml_parser.h"

static const char* module = "FMI2XML";

fmi2_xml_model_description_t * fmi2_xml_allocate_model_description( jm_callbacks* callbacks) {
    jm_callbacks* cb;
    fmi2_xml_model_description_t* md;

    if(callbacks) {
        cb = callbacks;
    }
    else {
        cb = jm_get_default_callbacks();
    }
    md = (fmi2_xml_model_description_t*)cb->calloc(1, sizeof(fmi2_xml_model_description_t));
    if(!md) {
		jm_log_fatal(cb, module, "Could not allocate memory");
		return 0;
	}

    md->callbacks = cb;

    md->status = fmi2_xml_model_description_enu_empty;

    jm_vector_init(char)( & md->fmi2_xml_standard_version, 0,cb);
    jm_vector_init(char)(&md->modelName, 0,cb);
    jm_vector_init(char)(&md->modelIdentifierME, 0,cb);
    jm_vector_init(char)(&md->modelIdentifierCS, 0,cb);
    jm_vector_init(char)(&md->GUID, 0,cb);
    jm_vector_init(char)(&md->description, 0,cb);
    jm_vector_init(char)(&md->author, 0,cb);
    jm_vector_init(char)(&md->license, 0,cb);
    jm_vector_init(char)(&md->copyright, 0,cb);
    jm_vector_init(char)(&md->version, 0,cb);
    jm_vector_init(char)(&md->generationTool, 0,cb);
    jm_vector_init(char)(&md->generationDateAndTime, 0,cb);


    md->namingConvension = fmi2_naming_enu_flat;
    md->numberOfContinuousStates = 0;
    md->numberOfEventIndicators = 0;

    md->defaultExperimentStartTime = 0;

    md->defaultExperimentStopTime = 1.0;

    md->defaultExperimentTolerance = FMI2_DEFAULT_EXPERIMENT_TOLERANCE;

    md->defaultExperimentStepSize = FMI2_DEFAULT_EXPERIMENT_STEPSIZE;

    jm_vector_init(jm_string)(&md->sourceFilesME, 0, cb);
	jm_vector_init(jm_string)(&md->sourceFilesCS, 0, cb);

	jm_vector_init(jm_string)(&md->vendorList, 0, cb);
	jm_vector_init(jm_string)(&md->logCategories, 0, cb);
	jm_vector_init(jm_string)(&md->logCategoryDescriptions, 0, cb);

    jm_vector_init(jm_named_ptr)(&md->unitDefinitions, 0, cb);
    jm_vector_init(jm_named_ptr)(&md->displayUnitDefinitions, 0, cb);

    fmi2_xml_init_type_definitions(&md->typeDefinitions, cb);

    jm_vector_init(jm_named_ptr)(&md->variablesByName, 0, cb);

	md->variablesOrigOrder = 0;

	md->variablesByVR = 0;

    jm_vector_init(jm_string)(&md->descriptions, 0, cb);

    md->fmuKind = fmi2_fmu_kind_unknown;

	{
		int i = fmi2_capabilities_Num;
		while(i > 0)
			md->capabilities[--i] = 0;
		md->capabilities[fmi2_me_completedEventIterationIsProvided] = 1;
	}
    return md;
}



void fmi2_xml_clear_model_description( fmi2_xml_model_description_t* md) {

    md->status = fmi2_xml_model_description_enu_empty;
    jm_vector_free_data(char)(&md->fmi2_xml_standard_version);
    jm_vector_free_data(char)(&md->modelName);
    jm_vector_free_data(char)(&md->modelIdentifierME);
    jm_vector_free_data(char)(&md->modelIdentifierCS);
    jm_vector_free_data(char)(&md->GUID);
    jm_vector_free_data(char)(&md->description);
    jm_vector_free_data(char)(&md->author);
    jm_vector_free_data(char)(&md->license);
    jm_vector_free_data(char)(&md->copyright);
    jm_vector_free_data(char)(&md->version);
    jm_vector_free_data(char)(&md->generationTool);
    jm_vector_free_data(char)(&md->generationDateAndTime);

    md->namingConvension = fmi2_naming_enu_flat;
    md->numberOfContinuousStates = 0;
    md->numberOfEventIndicators = 0;

    md->defaultExperimentStartTime = 0;

    md->defaultExperimentStopTime = 0;

    md->defaultExperimentTolerance = 0;

    md->defaultExperimentStepSize = 0;

    jm_vector_foreach(jm_string)(&md->sourceFilesME, (void(*)(const char*))md->callbacks->free);
    jm_vector_free_data(jm_string)(&md->sourceFilesME);	

    jm_vector_foreach(jm_string)(&md->sourceFilesCS, (void(*)(const char*))md->callbacks->free);
    jm_vector_free_data(jm_string)(&md->sourceFilesCS);	

    jm_vector_foreach(jm_string)(&md->vendorList, (void(*)(const char*))md->callbacks->free);
    jm_vector_free_data(jm_string)(&md->vendorList);

    jm_vector_foreach(jm_string)(&md->logCategories, (void(*)(const char*))md->callbacks->free);
    jm_vector_free_data(jm_string)(&md->logCategories);	

    jm_vector_foreach(jm_string)(&md->logCategoryDescriptions, (void(*)(const char*))md->callbacks->free);
    jm_vector_free_data(jm_string)(&md->logCategoryDescriptions);	

    jm_named_vector_free_data(&md->unitDefinitions);
    jm_named_vector_free_data(&md->displayUnitDefinitions);

    fmi2_xml_free_type_definitions_data(&md->typeDefinitions);

    jm_named_vector_free_data(&md->variablesByName);
	if(md->variablesOrigOrder) {
		jm_vector_free(jm_voidp)(md->variablesOrigOrder);
		md->variablesOrigOrder = 0;
	}
    if(md->variablesByVR) {
		jm_vector_free(jm_voidp)(md->variablesByVR);
		md->variablesByVR = 0;
	}

    jm_vector_foreach(jm_string)(&md->descriptions, (void(*)(const char*))md->callbacks->free);
    jm_vector_free_data(jm_string)(&md->descriptions);

	fmi2_xml_free_model_structure(md->modelStructure);
	md->modelStructure = 0;
}

int fmi2_xml_is_model_description_empty(fmi2_xml_model_description_t* md) {
    return (md->status == fmi2_xml_model_description_enu_empty);
}

const char* fmi2_xml_get_last_error(fmi2_xml_model_description_t* md) {
	return jm_get_last_error(md->callbacks);
}

void fmi2_xml_clear_last_error(fmi2_xml_model_description_t* md) {
	jm_clear_last_error(md->callbacks);
    
    /* return (md->status != fmi2_xml_model_description_enu_error); */
}

void fmi2_xml_free_model_description(fmi2_xml_model_description_t* md) {
    jm_callbacks* cb;
	if(!md) return;
	cb = md->callbacks;
    fmi2_xml_clear_model_description(md);
    cb->free(md);
}

const char* fmi2_xml_get_model_name(fmi2_xml_model_description_t* md) {
    return jm_vector_char2string(&md->modelName);
}

const char* fmi2_xml_get_model_identifier_ME(fmi2_xml_model_description_t* md){
    return jm_vector_char2string(&md->modelIdentifierME);
}

const char* fmi2_xml_get_model_identifier_CS(fmi2_xml_model_description_t* md){
    return jm_vector_char2string(&md->modelIdentifierCS);
}

const char* fmi2_xml_get_GUID(fmi2_xml_model_description_t* md){
    return jm_vector_char2string(&md->GUID);
}

const char* fmi2_xml_get_description(fmi2_xml_model_description_t* md){
    return jm_vector_char2string(&md->description);
}

const char* fmi2_xml_get_author(fmi2_xml_model_description_t* md){
    return jm_vector_char2string(&md->author);
}

const char* fmi2_xml_get_license(fmi2_xml_model_description_t* md){
    return jm_vector_char2string(&md->license);
}

const char* fmi2_xml_get_copyright(fmi2_xml_model_description_t* md){
    return jm_vector_char2string(&md->copyright);
}

const char* fmi2_xml_get_model_standard_version(fmi2_xml_model_description_t* md){
    return jm_vector_char2string(&md->fmi2_xml_standard_version);
}


const char* fmi2_xml_get_model_version(fmi2_xml_model_description_t* md){
    return jm_vector_char2string(&md->version);
}

const char* fmi2_xml_get_generation_tool(fmi2_xml_model_description_t* md){
    return jm_vector_char2string(&md->generationTool);
}

const char* fmi2_xml_get_generation_date_and_time(fmi2_xml_model_description_t* md){
    return jm_vector_char2string(&md->generationDateAndTime);
}

fmi2_variable_naming_convension_enu_t fmi2_xml_get_naming_convention(fmi2_xml_model_description_t* md) {
    return md->namingConvension;
}


size_t fmi2_xml_get_number_of_continuous_states(fmi2_xml_model_description_t* md) {
    return md->numberOfContinuousStates;
}

size_t fmi2_xml_get_number_of_event_indicators(fmi2_xml_model_description_t* md) {
    return md->numberOfEventIndicators;
}

double fmi2_xml_get_default_experiment_start(fmi2_xml_model_description_t* md) {
    return md->defaultExperimentStartTime;
}

double fmi2_xml_get_default_experiment_stop(fmi2_xml_model_description_t* md){
    return md->defaultExperimentStopTime;
}

double fmi2_xml_get_default_experiment_tolerance(fmi2_xml_model_description_t* md){
    return md->defaultExperimentTolerance;
}

double fmi2_xml_get_default_experiment_step(fmi2_xml_model_description_t* md){
    return md->defaultExperimentStepSize;
}

fmi2_fmu_kind_enu_t fmi2_xml_get_fmu_kind(fmi2_xml_model_description_t* md) {
	return md->fmuKind;
}

unsigned int* fmi2_xml_get_capabilities(fmi2_xml_model_description_t* md) {
	return md->capabilities;
}

unsigned int fmi2_xml_get_capability(fmi2_xml_model_description_t* md, fmi2_capabilities_enu_t id) {
	assert((unsigned)id < (unsigned)fmi2_capabilities_Num);
	return md->capabilities[id];
}

fmi2_xml_unit_definitions_t* fmi2_xml_get_unit_definitions(fmi2_xml_model_description_t* md) {
    return (fmi2_xml_unit_definitions_t*)(&md->unitDefinitions);
}

unsigned int  fmi2_xml_get_unit_definitions_number(fmi2_xml_unit_definitions_t* ud) {
	if(!ud) {
		assert(ud && "Unit definitions cannot be NULL");
		return 0;
	}
    return (unsigned int)jm_vector_get_size(jm_named_ptr)(&ud->definitions);
}

fmi2_xml_type_definitions_t* fmi2_xml_get_type_definitions(fmi2_xml_model_description_t* md) {
	assert(md);
    return &md->typeDefinitions;
}

/** \brief Get the number of vendors that had annotations in the XML*/
size_t fmi2_xml_get_vendors_num(fmi2_xml_model_description_t* md) {
	assert(md);
	return jm_vector_get_size(jm_string)(&md->vendorList);
}

jm_vector(jm_string)* fmi2_xml_get_log_categories(fmi2_xml_model_description_t* md) {
	assert(md);
	return &md->logCategories;
}

jm_vector(jm_string)* fmi2_xml_get_log_category_descriptions(fmi2_xml_model_description_t* md) {
	assert(md);
	return &md->logCategoryDescriptions;
}

jm_vector(jm_string)* fmi2_xml_get_source_files_me(fmi2_xml_model_description_t* md) {
	assert(md);
	return &md->sourceFilesME;
}

jm_vector(jm_string)* fmi2_xml_get_source_files_cs(fmi2_xml_model_description_t* md) {
	assert(md);
	return &md->sourceFilesCS;
}

fmi2_xml_model_structure_t* fmi2_xml_get_model_structure(fmi2_xml_model_description_t* md) {
	assert(md);
	return md->modelStructure;
}


/** \brief Get the name of the vendor with that had annotations in the XML by index */
const char* fmi2_xml_get_vendor_name(fmi2_xml_model_description_t* md, size_t  index) {
	assert(fmi2_xml_get_vendors_num(md) > index);

	return jm_vector_get_item(jm_string)(&md->vendorList,index);
}

int fmi2_xml_is_valid_model_ID_char(char ch) {
	return ((( ch >= 'A') && (ch <= 'Z')) || 
			(( ch >= 'a') && (ch <= 'z')) || 
			(ch == '_'));
}

int fmi2_xml_is_valid_model_ID(const char* str) {
	size_t i, len = strlen(str);
	char ch = str[0];
	if( !fmi2_xml_is_valid_model_ID_char(ch)) return 0;
	for(i = 1; i < len; i++) {
		ch = str[i];
		if( !fmi2_xml_is_valid_model_ID_char(ch) && (ch < '0') && (ch > '9')) return 0;
	}
	return 1;
}

int fmi2_xml_handle_fmiModelDescription(fmi2_xml_parser_context_t *context, const char* data) {
    jm_name_ID_map_t namingConventionMap[] = {{"flat",fmi2_naming_enu_flat},{"structured", fmi2_naming_enu_structured},{0,0}};
    fmi2_xml_model_description_t* md = context->modelDescription;
    if(!data) {
		unsigned int numEventIndicators=0;
		int ret;
        if(context -> currentElmID != fmi2_xml_elmID_none) {
            fmi2_xml_parse_fatal(context, "fmi2_xml_model_description must be the root XML element");
            return -1;
        }
		jm_log_verbose(context->callbacks, module, "Parsing XML element fmiModelDescription");
		md->fmuKind = fmi2_fmu_kind_unknown;
        /* process the attributes */
		ret =        /* <xs:attribute name="fmiVersion" type="xs:normalizedString" use="required" fixed="2.0"/> */
                    fmi2_xml_set_attr_string(context, fmi2_xml_elmID_fmiModelDescription, fmi_attr_id_fmiVersion, 1, &(md->fmi2_xml_standard_version)) ||
                    /* <xs:attribute name="modelName" type="xs:normalizedString" use="required"> */
                    fmi2_xml_set_attr_string(context, fmi2_xml_elmID_fmiModelDescription, fmi_attr_id_modelName, 1, &(md->modelName)) ||
					/* <xs:attribute name="guid" type="xs:normalizedString" use="required"> */
                    fmi2_xml_set_attr_string(context, fmi2_xml_elmID_fmiModelDescription, fmi_attr_id_guid, 1, &(md->GUID)) ||
                    /* <xs:attribute name="description" type="xs:string"/> */
                    fmi2_xml_set_attr_string(context, fmi2_xml_elmID_fmiModelDescription, fmi_attr_id_description, 0, &(md->description)) ||
                    /* <xs:attribute name="author" type="xs:string"/> */
                    fmi2_xml_set_attr_string(context, fmi2_xml_elmID_fmiModelDescription, fmi_attr_id_author, 0, &(md->author)) ||
                    /* <xs:attribute name="version" type="xs:normalizedString"> */
                    fmi2_xml_set_attr_string(context, fmi2_xml_elmID_fmiModelDescription, fmi_attr_id_version, 0, &(md->version)) ||
			        /* <xs:attribute name="copyright" type="xs:string"> */
                    fmi2_xml_set_attr_string(context, fmi2_xml_elmID_fmiModelDescription, fmi_attr_id_copyright, 0, &(md->copyright)) ||
					/* <xs:attribute name="license" type="xs:string"> */
                    fmi2_xml_set_attr_string(context, fmi2_xml_elmID_fmiModelDescription, fmi_attr_id_license, 0, &(md->license)) ||
					/* <xs:attribute name="generationTool" type="xs:normalizedString"/> */
                    fmi2_xml_set_attr_string(context, fmi2_xml_elmID_fmiModelDescription, fmi_attr_id_generationTool, 0, &(md->generationTool)) ||
                    /* <xs:attribute name="generationDateAndTime" type="xs:dateTime"/> */
                    fmi2_xml_set_attr_string(context, fmi2_xml_elmID_fmiModelDescription, fmi_attr_id_generationDateAndTime, 0, &(md->generationDateAndTime)) ||
                    /* <xs:attribute name="variableNamingConvention" use="optional" default="flat"> */
                    fmi2_xml_set_attr_enum(context, fmi2_xml_elmID_fmiModelDescription, fmi_attr_id_variableNamingConvention, 0, (unsigned*)&(md->namingConvension), fmi2_naming_enu_flat, namingConventionMap) ||
                    /* <xs:attribute name="numberOfEventIndicators" type="xs:unsignedInt"/> */
                    fmi2_xml_set_attr_uint(context, fmi2_xml_elmID_fmiModelDescription, fmi_attr_id_numberOfEventIndicators, 0, &numEventIndicators,0);
					md->numberOfEventIndicators = numEventIndicators;
		return (ret );
    }
    else {
		/* check that fmuKind is defined and that model identifies are valid*/
		if(md->fmuKind == fmi2_fmu_kind_unknown) {
			fmi2_xml_parse_fatal(context, "Neither ModelExchange nor CoSimulation element were parsed correctly. FMU kind not known.");
			return -1;
		}
		if( (md->fmuKind != fmi2_fmu_kind_cs) && !fmi2_xml_is_valid_model_ID(fmi2_xml_get_model_identifier_ME(md))) {
			fmi2_xml_parse_error(context, "Model identifier '%s' is not valid (must be a valid C-identifier)", fmi2_xml_get_model_identifier_ME(md));
			return -1;
		}
		if( (md->fmuKind != fmi2_fmu_kind_me) && !fmi2_xml_is_valid_model_ID(fmi2_xml_get_model_identifier_CS(md))) {
			fmi2_xml_parse_error(context, "Model identifier '%s' is not valid (must be a valid C-identifier)", fmi2_xml_get_model_identifier_CS(md));
			return -1;
		}
		if( (md->fmuKind == fmi2_fmu_kind_me_and_cs) && (strcmp(fmi2_xml_get_model_identifier_CS(md), fmi2_xml_get_model_identifier_ME(md)) == 0)) {
			jm_log_info(context->callbacks,module, "Found model identifiers for ModelExchange and CoSimulation");
			return 1;
		}
		if(!md->modelStructure) {
			fmi2_xml_parse_fatal(context, "No model structure information available. Cannot continue.");
			return -1;
		}
        return 0;
    }
}

int fmi2_xml_handle_ModelExchange(fmi2_xml_parser_context_t *context, const char* data) {
    fmi2_xml_model_description_t* md = context->modelDescription;
    if(!data) {
		jm_log_verbose(context->callbacks, module, "Parsing XML element ModelExchange");

        /*  reset handles for the elements that are specific under ModelExchange */
        fmi2_xml_set_element_handle(context, "SourceFiles", FMI2_XML_ELM_ID(SourceFiles));
        fmi2_xml_set_element_handle(context, "File", FMI2_XML_ELM_ID(File));

		md->fmuKind = fmi2_fmu_kind_me;
        /* process the attributes */

        /* <xs:attribute name="providesDirectionalDerivative" type="xs:boolean" default="false"/> */
        if (fmi2_xml_is_attr_defined(context, fmi_attr_id_providesDirectionalDerivatives)) {
			fmi2_xml_parse_error(context, "Attribute 'providesDirectionalDerivatives' has been renamed to 'providesDirectionalDerivative'.");
            if (fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_ModelExchange, fmi_attr_id_providesDirectionalDerivatives,0,
                &md->capabilities[fmi2_me_providesDirectionalDerivatives],0)) return -1;
        }
        else {
            if (fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_ModelExchange, fmi_attr_id_providesDirectionalDerivative,0,
                &md->capabilities[fmi2_me_providesDirectionalDerivatives],0)) return -1;
        }

        return (	/* <xs:attribute name="modelIdentifier" type="xs:normalizedString" use="required"> */
                    fmi2_xml_set_attr_string(context, fmi2_xml_elmID_ModelExchange, fmi_attr_id_modelIdentifier, 1, &(md->modelIdentifierME)) ||
					/* 	<xs:attribute name="needsExecutionTool" type="xs:boolean" default="false"> */
                    fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_ModelExchange, fmi_attr_id_needsExecutionTool,0,
                                             &md->capabilities[fmi2_me_needsExecutionTool],0) ||
					/* <xs:attribute name="completedIntegratorStepNotNeeded" type="xs:boolean" default="false"/> */
                    fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_ModelExchange, fmi_attr_id_completedIntegratorStepNotNeeded,0,
                                             &md->capabilities[fmi2_me_completedIntegratorStepNotNeeded],0) ||
					/* <xs:attribute name="canBeInstantiatedOnlyOncePerProcess" type="xs:boolean" default="false"/> */
                    fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_ModelExchange, fmi_attr_id_canBeInstantiatedOnlyOncePerProcess,0,
                                             &md->capabilities[fmi2_me_canBeInstantiatedOnlyOncePerProcess],0) ||
					/* <xs:attribute name="canNotUseMemoryManagementFunctions" type="xs:boolean" default="false"/> */
                    fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_ModelExchange, fmi_attr_id_canNotUseMemoryManagementFunctions,0,
                                             &md->capabilities[fmi2_me_canNotUseMemoryManagementFunctions],0) ||
					/* <xs:attribute name="canGetAndSetFMUstate" type="xs:boolean" default="false"/> */
                    fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_ModelExchange, fmi_attr_id_canGetAndSetFMUstate,0,
                                             &md->capabilities[fmi2_me_canGetAndSetFMUstate],0) ||
					/* <xs:attribute name="canSerializeFMUstate" type="xs:boolean" default="false"/> */
                    fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_ModelExchange, fmi_attr_id_canSerializeFMUstate,0,
                                             &md->capabilities[fmi2_me_canSerializeFMUstate],0)
                   );
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
}

int fmi2_xml_handle_CoSimulation(fmi2_xml_parser_context_t *context, const char* data) {
    fmi2_xml_model_description_t* md = context->modelDescription;
    if(!data) {
		jm_log_verbose(context->callbacks, module, "Parsing XML element CoSimulation");

        /*  reset handles for the elements that are specific under CoSimulation */
        fmi2_xml_set_element_handle(context, "SourceFiles", FMI2_XML_ELM_ID(SourceFilesCS));
        fmi2_xml_set_element_handle(context, "File", FMI2_XML_ELM_ID(FileCS));

		if(md->fmuKind == fmi2_fmu_kind_me)
			md->fmuKind = fmi2_fmu_kind_me_and_cs;
		else
			md->fmuKind = fmi2_fmu_kind_cs;
        /* process the attributes */

        /* <xs:attribute name="providesDirectionalDerivative" type="xs:boolean" default="false"/> */
        if (fmi2_xml_is_attr_defined(context, fmi_attr_id_providesDirectionalDerivatives)) {
			fmi2_xml_parse_error(context, "Attribute 'providesDirectionalDerivatives' has been renamed to 'providesDirectionalDerivative'.");
            if (fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_CoSimulation, fmi_attr_id_providesDirectionalDerivatives,0,
                &md->capabilities[fmi2_cs_providesDirectionalDerivatives],0)) return -1;
        }
        else {
            if (fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_CoSimulation, fmi_attr_id_providesDirectionalDerivative,0,
                &md->capabilities[fmi2_cs_providesDirectionalDerivatives],0)) return -1;
        }

        return (	/* <xs:attribute name="modelIdentifier" type="xs:normalizedString" use="required"> */
                    fmi2_xml_set_attr_string(context, fmi2_xml_elmID_CoSimulation, fmi_attr_id_modelIdentifier, 1, &(md->modelIdentifierCS)) ||
					/* 	<xs:attribute name="needsExecutionTool" type="xs:boolean" default="false"> */
                    fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_CoSimulation, fmi_attr_id_needsExecutionTool,0,
                                             &md->capabilities[fmi2_cs_needsExecutionTool],0) ||
					/* <xs:attribute name="canHandleVariableCommunicationStepSize" type="xs:boolean" default="false"/> */
                    fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_CoSimulation, fmi_attr_id_canHandleVariableCommunicationStepSize,0,
                                             &md->capabilities[fmi2_cs_canHandleVariableCommunicationStepSize],0) ||
					/* <xs:attribute name="canInterpolateInputs" type="xs:boolean" default="false"/>*/
                    fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_CoSimulation, fmi_attr_id_canInterpolateInputs,0,
                                             &md->capabilities[fmi2_cs_canInterpolateInputs],0) ||
					/* <xs:attribute name="maxOutputDerivativeOrder" type="xs:unsignedInt" default="0"/> */
                    fmi2_xml_set_attr_uint(context,fmi2_xml_elmID_CoSimulation, fmi_attr_id_maxOutputDerivativeOrder,0,
                                             &md->capabilities[fmi2_cs_maxOutputDerivativeOrder],0) ||
					/* <xs:attribute name="canRunAsynchronuously" type="xs:boolean" default="false"/> */
                    fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_CoSimulation, fmi_attr_id_canRunAsynchronuously,0,
                                             &md->capabilities[fmi2_cs_canRunAsynchronuously],0) ||
					/* <xs:attribute name="canBeInstantiatedOnlyOncePerProcess" type="xs:boolean" default="false"/> */
                    fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_CoSimulation, fmi_attr_id_canBeInstantiatedOnlyOncePerProcess,0,
                                             &md->capabilities[fmi2_cs_canBeInstantiatedOnlyOncePerProcess],0) ||
					/* <xs:attribute name="canNotUseMemoryManagementFunctions" type="xs:boolean" default="false"/> */
                    fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_CoSimulation, fmi_attr_id_canNotUseMemoryManagementFunctions,0,
                                             &md->capabilities[fmi2_cs_canNotUseMemoryManagementFunctions],0) ||
					/* <xs:attribute name="canGetAndSetFMUstate" type="xs:boolean" default="false"/> */
                    fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_CoSimulation, fmi_attr_id_canGetAndSetFMUstate,0,
                                             &md->capabilities[fmi2_cs_canGetAndSetFMUstate],0) ||
					/* <xs:attribute name="canSerializeFMUstate" type="xs:boolean" default="false"/> */
                    fmi2_xml_set_attr_boolean(context,fmi2_xml_elmID_CoSimulation, fmi_attr_id_canSerializeFMUstate,0,
                                             &md->capabilities[fmi2_cs_canSerializeFMUstate],0) 
                   );
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
}

int fmi2_xml_handle_SourceFiles(fmi2_xml_parser_context_t *context, const char* data) {
    return 0;
}

static int push_back_jm_string(fmi2_xml_parser_context_t *context, jm_vector(jm_string) *stringvector, jm_vector(char)* buf) {
    size_t len;
    char* string = 0;
    jm_string *pstring;

    pstring = jm_vector_push_back(jm_string)(stringvector, string);
	len = jm_vector_get_size(char)(buf);
    if(pstring )
        *pstring = string = (char*)(context->callbacks->malloc(len + 1));
	if(!pstring || !string) {
	    fmi2_xml_parse_fatal(context, "Could not allocate memory");
		return -1;
	}
    if (len != 0) { /* No need to memcpy empty strings (gives assetion error) */ 
        memcpy(string, jm_vector_get_itemp(char)(buf,0), len);
    }
    string[len] = 0;
    return 0;
}


int fmi2_xml_handle_File(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi2_xml_model_description_t* md = context->modelDescription;
        jm_vector(char)* bufName = fmi2_xml_reserve_parse_buffer(context,1,100);

        if(!bufName) return -1;
        /* <xs:attribute name="name" type="xs:normalizedString" use="required"> */
        if( fmi2_xml_set_attr_string(context, fmi2_xml_elmID_File, fmi_attr_id_name, 1, bufName))
            return -1;
        return push_back_jm_string(context, &md->sourceFilesME, bufName);
    }
    else {
        return 0;
    }
}

int fmi2_xml_handle_SourceFilesCS(fmi2_xml_parser_context_t *context, const char* data) {
    return 0;
}

int fmi2_xml_handle_FileCS(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi2_xml_model_description_t* md = context->modelDescription;
        jm_vector(char)* bufName = fmi2_xml_reserve_parse_buffer(context,1,100);

        if(!bufName) return -1;
        /* <xs:attribute name="name" type="xs:normalizedString" use="required"> */
        if( fmi2_xml_set_attr_string(context, fmi2_xml_elmID_File, fmi_attr_id_name, 1, bufName))
            return -1;
        return push_back_jm_string(context, &md->sourceFilesCS, bufName);
    }
    else {
        return 0;
    }
}

int fmi2_xml_handle_LogCategories(fmi2_xml_parser_context_t *context, const char* data) {
/*    fmi2_xml_model_description_t* md = context->modelDescription; */
    if(!data) {
		jm_log_verbose(context->callbacks, module, "Parsing XML element LogCategories");
        /* process the attributes */
        return (0);
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
}

int fmi2_xml_handle_Category(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
        /* process the attributes */
        fmi2_xml_model_description_t* md = context->modelDescription;
        jm_vector(char)* bufName = fmi2_xml_reserve_parse_buffer(context,1,100);

        if (!bufName) return -1;
		/* <xs:attribute name="name" type="xs:normalizedString"> */
        if (fmi2_xml_set_attr_string(context, fmi2_xml_elmID_Category, fmi_attr_id_name, 1, bufName))
			return -1;
        if (push_back_jm_string(context, &md->logCategories, bufName) < 0) return -1;

        if (fmi2_xml_set_attr_string(context, fmi2_xml_elmID_Category, fmi_attr_id_description, 0, bufName) < 0) return -1;
        if (push_back_jm_string(context, &md->logCategoryDescriptions, bufName) < 0) return -1;
        return 0;
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
}

int fmi2_xml_handle_DefaultExperiment(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi2_xml_model_description_t* md = context->modelDescription;
        /* process the attributes */
        return (
        /* <xs:attribute name="startTime" type="xs:double"/> */
                fmi2_xml_set_attr_double(context, fmi2_xml_elmID_DefaultExperiment, fmi_attr_id_startTime, 0, &md->defaultExperimentStartTime, 0) ||
        /* <xs:attribute name="stopTime" type="xs:double"/>  */
                fmi2_xml_set_attr_double(context, fmi2_xml_elmID_DefaultExperiment, fmi_attr_id_stopTime, 0, &md->defaultExperimentStopTime, 1) ||
        /* <xs:attribute name="tolerance" type="xs:double">  */
                fmi2_xml_set_attr_double(context, fmi2_xml_elmID_DefaultExperiment, fmi_attr_id_tolerance, 0, &md->defaultExperimentTolerance, FMI2_DEFAULT_EXPERIMENT_TOLERANCE) ||
        /* <xs:attribute name="stepSize" type="xs:double">   */
                fmi2_xml_set_attr_double(context, fmi2_xml_elmID_DefaultExperiment, fmi_attr_id_stepSize, 0, &md->defaultExperimentStepSize, FMI2_DEFAULT_EXPERIMENT_STEPSIZE)
        );
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

jm_vector(jm_voidp)* fmi2_xml_get_variables_original_order(fmi2_xml_model_description_t* md) {
	return md->variablesOrigOrder;
}

jm_vector(jm_named_ptr)* fmi2_xml_get_variables_alphabetical_order(fmi2_xml_model_description_t* md){
	return &md->variablesByName;
}

jm_vector(jm_voidp)* fmi2_xml_get_variables_vr_order(fmi2_xml_model_description_t* md) {
	return md->variablesByVR;
}


fmi2_xml_variable_t* fmi2_xml_get_variable_by_name(fmi2_xml_model_description_t* md, const char* name) {
	jm_named_ptr key, *found;
    key.name = name;
    found = jm_vector_bsearch(jm_named_ptr)(&md->variablesByName, &key, jm_compare_named);
	if(!found) return 0;
	return found->ptr;
}


fmi2_xml_variable_t* fmi2_xml_get_variable_by_vr(fmi2_xml_model_description_t* md, fmi2_base_type_enu_t baseType, fmi2_value_reference_t vr) {
    fmi2_xml_variable_t key;
    fmi2_xml_variable_t *pkey = &key;
	fmi2_xml_variable_type_base_t keyType;
	fmi2_xml_variable_t *v = 0;
    void ** found;
	if(!md->variablesByVR) return 0;
	keyType.structKind = fmi2_xml_type_struct_enu_props;
	keyType.baseType = baseType;
	key.typeBase = &keyType;
	key.vr = vr;
    key.aliasKind = fmi2_variable_is_not_alias;

    found = jm_vector_bsearch(jm_voidp)(md->variablesByVR,(void**)&pkey, fmi2_xml_compare_vr);
    if(!found) return 0;
    v = (fmi2_xml_variable_t*)(*found);
    return v;
}


