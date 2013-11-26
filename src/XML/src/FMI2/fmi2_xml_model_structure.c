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

/** \file fmi2_xml_model_structure.c
*  \brief Implementation for the model structure interface.
*/
#include <string.h>
#include <stdio.h>

#include "fmi2_xml_parser.h"
#include "fmi2_xml_model_structure_impl.h"
#include "fmi2_xml_model_description_impl.h"

static const char * module = "FMI2XML";

fmi2_xml_model_structure_t* fmi2_xml_allocate_model_structure(jm_callbacks* cb) {
	fmi2_xml_model_structure_t* ms = (fmi2_xml_model_structure_t*)(cb->calloc(1, sizeof(fmi2_xml_model_structure_t)));
	if(!ms) return 0;

	jm_vector_init(jm_voidp)(&ms->inputs,0,cb);
	jm_vector_init(size_t)(&ms->inputIsDerivative,0,cb); /** \brief If an element is non-zero than this input is a derivative of another input */

	jm_vector_init(jm_voidp)(&ms->derivatives,0,cb);
	jm_vector_init(jm_voidp)(&ms->states,0,cb);  /** \brief index of the associated state */
	
	jm_vector_init(jm_voidp)(&ms->outputs,0,cb);
	jm_vector_init(size_t)(&ms->outputIsDerivative,0,cb); /** \brief If an element is non-zero than this output is a derivative of another output */

	ms->isValidFlag = 1;

	ms->depsStatesOnInputs = fmi2_xml_allocate_dependencies(cb);
	ms->depsStatesOnStates = fmi2_xml_allocate_dependencies(cb);
	ms->depsOutputsOnInputs = fmi2_xml_allocate_dependencies(cb);
	ms->depsOutputsOnStates = fmi2_xml_allocate_dependencies(cb);

	if(!ms->depsStatesOnInputs || !ms->depsStatesOnStates || !ms->depsOutputsOnInputs || !ms->depsOutputsOnStates) {
		fmi2_xml_free_model_structure(ms);
		return 0;
	}

	return ms;
}

void fmi2_xml_free_model_structure(fmi2_xml_model_structure_t* ms) {
	jm_callbacks* cb;
	if(!ms) return;
	cb = ms->inputs.callbacks;
	jm_vector_free_data(jm_voidp)(&ms->inputs);
	jm_vector_free_data(size_t)(&ms->inputIsDerivative); /** \brief If an element is non-zero than this input is a derivative of another input */

	jm_vector_free_data(jm_voidp)(&ms->derivatives);
	jm_vector_free_data(jm_voidp)(&ms->states);  /** \brief index of the associated state */
	
	jm_vector_free_data(jm_voidp)(&ms->outputs);
	jm_vector_free_data(size_t)(&ms->outputIsDerivative); /** \brief If an element is non-zero than this output is a derivative of another output */
	
	fmi2_xml_free_dependencies(ms->depsStatesOnInputs); /** \brief Dependencies between vars. */
	fmi2_xml_free_dependencies(ms->depsStatesOnStates);
	fmi2_xml_free_dependencies(ms->depsOutputsOnInputs); /** \brief Dependencies between vars.  */
	fmi2_xml_free_dependencies(ms->depsOutputsOnStates);
	cb->free(ms);
}

/*
jm_vector(jm_voidp)* fmi2_xml_get_outputs(fmi2_xml_model_structure_t* ms) {
	return &ms->outputs;
}

jm_vector(jm_voidp)* fmi2_xml_get_inputs(fmi2_xml_model_structure_t* ms){
	return &ms->inputs;
}

jm_vector(jm_voidp)* fmi2_xml_get_states(fmi2_xml_model_structure_t* ms){
	return &ms->states;
}

jm_vector(jm_voidp)* fmi2_xml_get_derivatives(fmi2_xml_model_structure_t* ms){
	return &ms->derivatives;
}
*/

void fmi2_xml_get_dependencies(fmi2_xml_dependencies_t* dep, size_t** startIndex, size_t** dependency, char** factorKind){
	if(dep) {
		*startIndex = jm_vector_get_itemp(size_t)(&dep->startIndex, 0);
		*dependency = jm_vector_get_itemp(size_t)(&dep->dependencyIndex, 0);
		*factorKind = jm_vector_get_itemp(char)(&dep->dependencyFactorKind, 0);
	}
	else {
		*startIndex = 0;
	}
}

void fmi2_xml_get_dependencies_derivatives_on_inputs(fmi2_xml_model_structure_t* ms, 
						size_t** startIndex, size_t** dependency, char** factorKind) {
	fmi2_xml_get_dependencies(ms->depsStatesOnInputs, startIndex, dependency, factorKind);
}

void fmi2_xml_get_dependencies_derivatives_on_states(fmi2_xml_model_structure_t* ms, 
	size_t** startIndex, size_t** dependency, char** factorKind){
		fmi2_xml_get_dependencies(ms->depsStatesOnStates, startIndex, dependency, factorKind);
}

void fmi2_xml_get_dependencies_outputs_on_inputs(fmi2_xml_model_structure_t* ms, 
	size_t** startIndex, size_t** dependency, char** factorKind){
		fmi2_xml_get_dependencies(ms->depsOutputsOnInputs, startIndex, dependency, factorKind);
}

void fmi2_xml_get_dependencies_outputs_on_states(fmi2_xml_model_structure_t* ms, 
	size_t** startIndex, size_t** dependency, char** factorKind){
		fmi2_xml_get_dependencies(ms->depsOutputsOnStates, startIndex, dependency, factorKind);
}

fmi2_xml_dependencies_t* fmi2_xml_allocate_dependencies(jm_callbacks* cb) {
	fmi2_xml_dependencies_t* dep = (fmi2_xml_dependencies_t*)(cb->malloc(sizeof(fmi2_xml_dependencies_t)));
	if(!dep) return 0;
	jm_vector_init(size_t)(&dep->startIndex, 0, cb);
	jm_vector_push_back(size_t)(&dep->startIndex, 0);

	jm_vector_init(size_t)(&dep->dependencyIndex, 0, cb);
	jm_vector_init(char)(&dep->dependencyFactorKind, 0, cb);

	dep->isRowMajor = 1;

	return dep;
}

void fmi2_xml_zero_empty_dependencies(fmi2_xml_dependencies_t** pdep) {
	fmi2_xml_dependencies_t* dep =*pdep;
	size_t ndep = jm_vector_get_size(size_t)(&dep->dependencyIndex);
	size_t i;
	if(!dep) return;
	for(i = 0; i<ndep;i++) {
		if(jm_vector_get_item(size_t)(&dep->dependencyIndex, i)) break;
	}
	if(i == ndep) {
		fmi2_xml_free_dependencies(dep);
		*pdep = 0;
	}
}


void fmi2_xml_free_dependencies(fmi2_xml_dependencies_t* dep) {
    jm_callbacks* cb;
	if(!dep) return;
    cb = dep->startIndex.callbacks;
	jm_vector_free_data(size_t)(&dep->startIndex);

	jm_vector_free_data(size_t)(&dep->dependencyIndex);
	jm_vector_free_data(char)(&dep->dependencyFactorKind);
    cb->free(dep);
}


int fmi2_xml_check_model_structure(fmi2_xml_model_description_t* md) {
    size_t i, numvar, numStates, numInputs, numOutputs;
	fmi2_xml_model_structure_t* ms = md->modelStructure;
	jm_callbacks* cb = md->callbacks;

	if(!ms || !ms->isValidFlag) return 0;

    numvar = jm_vector_get_size(jm_named_ptr)(&md->variablesByName);
	numStates = jm_vector_get_size(jm_voidp)(&ms->states);
	numInputs = jm_vector_get_size(jm_voidp)(&ms->inputs);
	numOutputs = jm_vector_get_size(jm_voidp)(&ms->outputs);

	/* check that all inputs and outputs are listed and that states
		are not aliased to inputs */
    for(i = 0; i< numvar; i++) {
		jm_named_ptr named = jm_vector_get_item(jm_named_ptr)(&md->variablesByName, i);
        fmi2_xml_variable_t* v = named.ptr;
        if(v->causality == fmi2_causality_enu_input) {
			if(v->inputIndex == 0) {
				jm_log_error(cb, module, "Input variable %s is not listed among inputs in ModelStructure", fmi2_xml_get_variable_name(v));
				ms->isValidFlag = 0;
			}
		}
		else if(v->causality == fmi2_causality_enu_output) {
			if(v->outputIndex == 0) {
				jm_log_error(cb, module, "Output variable %s is not listed among outputs in ModelStructure", fmi2_xml_get_variable_name(v));
				ms->isValidFlag = 0;
			}
		}
		if(v->stateIndex != 0) {
			fmi2_xml_variable_t* vb = fmi2_xml_get_variable_alias_base(md, v);
			if((vb->causality == fmi2_causality_enu_input) || (vb->causality == fmi2_causality_enu_parameter)) {
				if(v == vb) {
					jm_log_error(cb, module, "Variable %s with causality %s is referred as a state in ModelStructure", 
						fmi2_xml_get_variable_name(v), fmi2_causality_to_string((fmi2_causality_enu_t)v->causality));
					ms->isValidFlag = 0;
				}
				else {
					jm_log_error(cb, module, "Variable %s aliased to %s with causality %s is referred as a state in ModelStructure", 
						fmi2_xml_get_variable_name(v), fmi2_xml_get_variable_name(vb), fmi2_causality_to_string((fmi2_causality_enu_t)v->causality));
					ms->isValidFlag = 0;
				}
			}
			if(vb->variability <= fmi2_variability_enu_discrete) {
				if(v == vb) {
					jm_log_error(cb, module, "Variable %s with variability %s is referred as a state in ModelStructure", 
						fmi2_xml_get_variable_name(v), fmi2_variability_to_string((fmi2_variability_enu_t)v->variability));
					ms->isValidFlag = 0;
				}
				else {
					jm_log_error(cb, module, "Variable %s aliased to %s with variability %s is referred as a state in ModelStructure", 
						fmi2_xml_get_variable_name(v), fmi2_xml_get_variable_name(vb), 
						fmi2_variability_to_string((fmi2_variability_enu_t)v->variability));
					ms->isValidFlag = 0;
				}
			}
		}
    }
	/*  check that "derivate" attribute is not out of range */
    for(i = 0; i< numInputs; i++) {
		size_t ind = jm_vector_get_item(size_t)(&ms->inputIsDerivative, i);
		if(ind > numInputs) {
			fmi2_xml_variable_t* v= (fmi2_xml_variable_t*)jm_vector_get_item(jm_voidp)(&ms->inputs, i);
			jm_log_error(cb, module, "Input %s indicates derivative index %u but number of inputs is %u", 
						fmi2_xml_get_variable_name(v), ind, numInputs);
			ms->isValidFlag = 0;
		}
	}
    for(i = 0; i< numOutputs; i++) {
		size_t ind = jm_vector_get_item(size_t)(&ms->outputIsDerivative, i);
		if(ind > numOutputs) {
			fmi2_xml_variable_t* v= (fmi2_xml_variable_t*)jm_vector_get_item(jm_voidp)(&ms->outputs, i);
			jm_log_error(cb, module, "Output %s indicates derivative index %u but number of outputs is %u", 
						fmi2_xml_get_variable_name(v), ind, numInputs);
			ms->isValidFlag = 0;
		}
	}

	/* check dependencies on inputs */
	fmi2_xml_zero_empty_dependencies(&ms->depsStatesOnInputs);
	if(ms->depsStatesOnInputs) {
		fmi2_xml_dependencies_t* dsi = ms->depsStatesOnInputs;
		size_t varIndex = 0;
		size_t numDeps = jm_vector_get_size(size_t)(&dsi->dependencyIndex);
		jm_vector_set_item(size_t)(&dsi->startIndex,numStates, numDeps);
		for(i = 0; i < numDeps; i++) {
			size_t ind = jm_vector_get_item(size_t)(&dsi->dependencyIndex, i);
			if(ind > numInputs) {
				fmi2_xml_variable_t* v;
				while(jm_vector_get_item(size_t)(&dsi->startIndex, varIndex+1) < i) varIndex++;
				v = (fmi2_xml_variable_t*)jm_vector_get_item(jm_voidp)(&ms->derivatives, varIndex);
				jm_log_error(cb, module, "Dependency list for state derivative %s contains input index %u but the number of inputs is %u",
					fmi2_xml_get_variable_name(v), ind, numInputs);
				ms->isValidFlag = 0;
			}
		}
	}
	fmi2_xml_zero_empty_dependencies(&ms->depsOutputsOnInputs);
	if(ms->depsOutputsOnInputs) {
		fmi2_xml_dependencies_t* doi = ms->depsOutputsOnInputs;
		size_t varIndex = 0;
		size_t numDeps = jm_vector_get_size(size_t)(&doi->dependencyIndex);
		jm_vector_set_item(size_t)(&doi->startIndex,numOutputs, numDeps);
		for(i = 0; i < numDeps; i++) {
			size_t ind = jm_vector_get_item(size_t)(&doi->dependencyIndex, i);
			fmi2_xml_variable_t* vOut;
			while(jm_vector_get_item(size_t)(&doi->startIndex, varIndex+1) < i) varIndex++;
			vOut = (fmi2_xml_variable_t*)jm_vector_get_item(jm_voidp)(&ms->outputs, varIndex);
			if(ind > numInputs) {
				jm_log_error(cb, module, "Dependency list for output %s contains input index %u but the number of inputs is %u",
					fmi2_xml_get_variable_name(vOut), ind, numInputs);
				ms->isValidFlag = 0;
			}
			else if(ind!=0) {
				fmi2_xml_variable_t* vIn = (fmi2_xml_variable_t*)jm_vector_get_item(jm_voidp)(&ms->inputs, ind - 1);
				fmi2_variability_enu_t variOut = fmi2_xml_get_variability(vOut);
				fmi2_variability_enu_t variIn = fmi2_xml_get_variability(vIn);
				if((variOut <= fmi2_variability_enu_tunable) && (variOut > variIn)) {
					jm_log_error(cb, module, "Output %s with variability %s depends on input %s with variability %s",
						fmi2_xml_get_variable_name(vOut), fmi2_variability_to_string(variOut), 
						fmi2_xml_get_variable_name(vIn), fmi2_variability_to_string(variIn));
					ms->isValidFlag = 0;
				}
			}
		}
	}
	/* check dependencies on states */
	fmi2_xml_zero_empty_dependencies(&ms->depsStatesOnStates);
	if(ms->depsStatesOnStates) {
		fmi2_xml_dependencies_t* dss = ms->depsStatesOnStates;
		size_t varIndex = 0;
		size_t numDeps = jm_vector_get_size(size_t)(&dss->dependencyIndex);
		jm_vector_set_item(size_t)(&dss->startIndex,numStates, numDeps);
		for(i = 0; i < numDeps; i++) {
			size_t ind = jm_vector_get_item(size_t)(&dss->dependencyIndex, i);
			if(ind > numStates) {
				fmi2_xml_variable_t* v;
				while(jm_vector_get_item(size_t)(&dss->startIndex, varIndex+1) < i) varIndex++;
				v = (fmi2_xml_variable_t*)jm_vector_get_item(jm_voidp)(&ms->derivatives, varIndex);
				jm_log_error(cb, module, "Dependency list for state derivative %s contains state index %u but the number of states is %u",
					fmi2_xml_get_variable_name(v), ind, numStates);
				ms->isValidFlag = 0;
			}
		}
	}
	fmi2_xml_zero_empty_dependencies(&ms->depsOutputsOnStates);
	if(ms->depsOutputsOnStates) {
		fmi2_xml_dependencies_t* dos = ms->depsOutputsOnStates;
		size_t varIndex = 0;
		size_t numDeps = jm_vector_get_size(size_t)(&dos->dependencyIndex);
		jm_vector_set_item(size_t)(&dos->startIndex,numOutputs, numDeps);
		for(i = 0; i < numDeps; i++) {
			size_t ind = jm_vector_get_item(size_t)(&dos->dependencyIndex, i);
			fmi2_xml_variable_t* vOut;
			while(jm_vector_get_item(size_t)(&dos->startIndex, varIndex+1) < i) varIndex++;
			vOut = (fmi2_xml_variable_t*)jm_vector_get_item(jm_voidp)(&ms->outputs, varIndex);
			if(ind > numStates ) {
				jm_log_error(cb, module, "Dependency list for output %s contains state index %u but the number of states is %u",
					fmi2_xml_get_variable_name(vOut), ind, numStates);
				ms->isValidFlag = 0;
			}
			else if(ind!=0) {
				fmi2_xml_variable_t* vState = (fmi2_xml_variable_t*)jm_vector_get_item(jm_voidp)(&ms->states, ind - 1 );
				fmi2_variability_enu_t variOut = fmi2_xml_get_variability(vOut);
				if(variOut <= fmi2_variability_enu_tunable) {
					jm_log_error(cb, module, "Output %s with variability %s depends on continuous state %s",
						fmi2_xml_get_variable_name(vOut), fmi2_variability_to_string(variOut), 
						fmi2_xml_get_variable_name(vState));
					ms->isValidFlag = 0;
				}
			}
		}
	}

	return ms->isValidFlag;
}

int fmi2_xml_handle_ModelStructure(fmi2_xml_parser_context_t *context, const char* data) {
    fmi2_xml_model_description_t* md = context->modelDescription;
    if(!data) {
		jm_log_verbose(context->callbacks, module,"Parsing XML element ModelStructure");
		/** allocate model structure */
		md->modelStructure = fmi2_xml_allocate_model_structure(md->callbacks);
		if(!md->modelStructure) {
				fmi2_xml_parse_fatal(context, module, "Could not allocate memory");
				return -1;
		}
    }
    else {
		/** make sure model structure information is consistent */
		if(!fmi2_xml_check_model_structure(md)) {
			fmi2_xml_parse_fatal(context, "Model structure is not valid due to detected errors. Cannot continue.");
			return -1;
		}
		md->numberOfContinuousStates = jm_vector_get_size(jm_voidp)(&md->modelStructure->states);
    }
    return 0;
}


int fmi2_xml_handle_Inputs(fmi2_xml_parser_context_t *context, const char* data) {
		/** nothing to do */
    return 0;
}
int fmi2_xml_handle_Derivatives(fmi2_xml_parser_context_t *context, const char* data) {
		/** nothing to do */
    return 0;
}
int fmi2_xml_handle_Outputs(fmi2_xml_parser_context_t *context, const char* data) {
		/** nothing to do */
    return 0;
}

int fmi2_xml_handle_Input(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi2_xml_model_description_t* md = context->modelDescription;
		fmi2_xml_model_structure_t* ms = md->modelStructure;
        jm_vector(char)* bufName = fmi2_xml_reserve_parse_buffer(context,1,100);
		int derivative;
	    jm_named_ptr key, *found;
    
        fmi2_xml_variable_t* variable;
		
		if(
			/*  <xs:attribute name="name" type="xs:normalizedString" use="required"/> */
			fmi2_xml_set_attr_string(context, fmi2_xml_elmID_Input, fmi_attr_id_name, 1, bufName))
		{
			ms->isValidFlag = 0;
			context->skipElementCnt = 1;
			return 0;
		}
		key.name = jm_vector_get_itemp(char)(bufName,0);
		found = jm_vector_bsearch(jm_named_ptr)(&(md->variablesByName),&key, jm_compare_named);
		if(!found) {
			fmi2_xml_parse_error(context, "Input element references variable %s that is not defined", key.name);
			context->skipElementCnt = 1;
			return 0;
		}
		variable = (fmi2_xml_variable_t*)found->ptr;
        if(variable->causality != fmi2_causality_enu_input) {
			fmi2_xml_parse_error(context, 
				"Input element cannot reference '%s' since causality is not input. Skipping.", variable->name);
			context->skipElementCnt = 1;
            return 0;
        }

		if(!fmi2_xml_is_attr_defined(context, fmi_attr_id_derivative) ||
			/*	<xs:attribute name="derivative" type="xs:int"> */
			fmi2_xml_set_attr_int(context, fmi2_xml_elmID_Input, fmi_attr_id_derivative, 0, &derivative, -1)) {
			derivative = 0;
		}
		else {
			if(derivative <= 0) {
				fmi2_xml_parse_error(context, "Input %s has attribute derivative=%d but indexing starts from 1.", 
					variable->name, derivative);
				ms->isValidFlag = 0;
			}
		}

		if(!jm_vector_push_back(jm_voidp)(&ms->inputs, variable) ||
			!jm_vector_push_back(size_t)(&ms->inputIsDerivative, (size_t)derivative)) {
				fmi2_xml_parse_fatal(context, "Could not allocate memory");
				return -1;
		}

		variable->inputIndex = jm_vector_get_size(jm_voidp)(&ms->inputs);
    }
    else {
    }
    return 0;
}

extern fmi2_xml_element_handle_map_t fmi2_element_handle_map[fmi2_xml_elm_number];
extern const char *fmi2_xmlAttrNames[fmi2_xml_attr_number];

int fmi2_xml_parse_dependency(fmi2_xml_parser_context_t *context, 
								fmi2_xml_elm_enu_t elmID, 
								fmi2_xml_attr_enu_t depAttrID,
								fmi2_xml_attr_enu_t depKindAttrID,
								fmi2_xml_dependencies_t* deps) {
    jm_string elmName;
    fmi2_xml_model_description_t* md = context->modelDescription;
	fmi2_xml_model_structure_t* ms = md->modelStructure;

    const char* listInd;
    const char* listKind;
	size_t numDepInd = 0;
	size_t numDepKind = 0;
	size_t totNumDep = jm_vector_get_size(size_t)(&deps->dependencyIndex);

	elmName = fmi2_element_handle_map[elmID].elementName;

	/* <xs:attribute name="stateDependencies"> (or 		<xs:attribute name="inputDependencies"> )
			<xs:simpleType>
				<xs:list itemType="xs:int"/>
			</xs:simpleType>
		</xs:attribute>
		*/
	if(fmi2_xml_get_attr_str(context, elmID, depAttrID, 0, &listInd)) {
		ms->isValidFlag = 0;
		return 0;
	}
	if(listInd) {
		 const char* cur = listInd;
		 int ind;
		 while(*cur) {
			 char ch = *cur;
			 while((ch ==' ') || (ch == '\t') || (ch =='\n') || (ch == '\r')) {
				 cur++; ch = *cur;
				 if(!ch) break;
			 }
			 if(!ch) break;
			 if(sscanf(cur, "%d", &ind) != 1) {
		         fmi2_xml_parse_error(context, "XML element '%s': could not parse item %d in the list for attribute '%s'", 
					 elmName, numDepInd, fmi2_xmlAttrNames[depAttrID]);
				ms->isValidFlag = 0;
				return 0;
			 }
			 if(ind < 1) {
		         fmi2_xml_parse_error(context, "XML element '%s': item %d=%d is less than one in the list for attribute '%s'", 
					 elmName, numDepInd, ind, fmi2_xmlAttrNames[depAttrID]);
				ms->isValidFlag = 0;
				return 0;
			 }
			 if(!jm_vector_push_back(size_t)(&deps->dependencyIndex, (size_t)ind)) {
				fmi2_xml_parse_fatal(context, "Could not allocate memory");
				return -1;
			}
			 while((*cur >= '0') && (*cur <= '9')) cur++;
			 numDepInd++;
		 }
    }

	/*
		<xs:attribute name="stateFactorKinds"> (or <xs:attribute name="inputFactorKinds">)
			<xs:simpleType>
				<xs:list itemType="fmiDependencyFactorKind"/>
			</xs:simpleType>
		</xs:attribute>
		*/
	if(fmi2_xml_get_attr_str(context, elmID, depKindAttrID, 0, &listKind)) {
		ms->isValidFlag = 0;
		return 0;
	}
	if(listKind) {
		 const char* cur = listKind;
		 char kind;
		 while(*cur) {
			 char ch = *cur;
			 while(ch && ((ch ==' ') || (ch == '\t') || (ch =='\n') || (ch == '\r'))) {
				 cur++; ch = *cur;
			 }
			 if(!ch) break;
			 if(strncmp("fixed", cur, 5) == 0) {
				 kind = fmi2_dependency_factor_kind_fixed;
				 cur+=5;
			 }
			 else if(strncmp("nonlinear", cur, 9) == 0) {
				 kind = fmi2_dependency_factor_kind_nonlinear;
				 cur+=9;
			 }
			 else if(strncmp("discrete", cur, 8) == 0) {
 				 kind = fmi2_dependency_factor_kind_discrete;
				 cur+=8;
			 }
			 else {
		         fmi2_xml_parse_error(context, "XML element '%s': could not parse item %d in the list for attribute '%s'", 
					 elmName, numDepKind, fmi2_xmlAttrNames[depKindAttrID]);
				ms->isValidFlag = 0;
				return 0;
			 }
			 if(!jm_vector_push_back(char)(&deps->dependencyFactorKind, kind)) {
				fmi2_xml_parse_fatal(context, "Could not allocate memory");
				return -1;
			}
			 numDepKind++;
		 }
    }
	if(listInd && listKind) {
		/* both lists are present - the number of items must match */
		if(numDepInd != numDepKind) {
  	            fmi2_xml_parse_error(context, "XML element '%s': different number of items (%u and %u) in the lists for '%s' and '%s'", 
					 elmName, numDepInd, numDepKind, 
					 fmi2_xmlAttrNames[depAttrID], fmi2_xmlAttrNames[depKindAttrID]);
				ms->isValidFlag = 0;
				return 0;
		}
	}
	else if(listInd) {
		/* only Dependencies are present, set all "kinds" to nonlinear */
		char kind = fmi2_dependency_factor_kind_nonlinear;
		if(jm_vector_reserve(char)(&deps->dependencyFactorKind,totNumDep + numDepInd) < totNumDep + numDepInd) {
				fmi2_xml_parse_fatal(context, "Could not allocate memory");
				return -1;
		}
		for(;numDepKind < numDepInd; numDepKind++)
			jm_vector_push_back(char)(&deps->dependencyFactorKind, kind);
	}
	else if(listKind) {
		/*  Only factor kinds are present -> assume that items just come in ascending order.
		*/
		if(jm_vector_reserve(size_t)(&deps->dependencyIndex,totNumDep + numDepKind) < totNumDep + numDepKind) {
				fmi2_xml_parse_fatal(context, "Could not allocate memory");
				return -1;
		}
		for(;numDepInd < numDepKind; numDepInd++)
			jm_vector_push_back(size_t)(&deps->dependencyIndex, numDepInd+1);
	}
	else {
		/* Dependencies are not provided. Put zero index/nonlinear to indicate that full row must be considered. */
		numDepInd = numDepKind = 1;
		if(!jm_vector_push_back(char)(&deps->dependencyFactorKind, fmi2_dependency_factor_kind_nonlinear) ||
		   !jm_vector_push_back(size_t)(&deps->dependencyIndex, 0)
			) {
				fmi2_xml_parse_fatal(context, "Could not allocate memory");
				return -1;
		}
	}
	if(!jm_vector_push_back(size_t)(&deps->startIndex, totNumDep + numDepInd)) {
		fmi2_xml_parse_fatal(context, "Could not allocate memory");
		return -1;
	}
    return 0;
}

int fmi2_xml_handle_Derivative(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi2_xml_model_description_t* md = context->modelDescription;
		fmi2_xml_model_structure_t* ms = md->modelStructure;
        jm_vector(char)* bufName = fmi2_xml_reserve_parse_buffer(context,1,100);
        jm_vector(char)* bufState = fmi2_xml_reserve_parse_buffer(context,2,100);
	    jm_named_ptr key, *found;
    
        fmi2_xml_variable_t* deriv, *state;
		
		if(
			/*  <xs:attribute name="name" type="xs:normalizedString" use="required"/> */
			fmi2_xml_set_attr_string(context, fmi2_xml_elmID_Derivative, fmi_attr_id_name, 1, bufName) ||
			/*  <xs:attribute name="state" type="xs:normalizedString" use="required"/> */
			fmi2_xml_set_attr_string(context, fmi2_xml_elmID_Derivative, fmi_attr_id_state, 1, bufState))
		{
			ms->isValidFlag = 0;
			context->skipElementCnt = 1;
			return 0;
		}
		key.name = jm_vector_get_itemp(char)(bufName,0);
		found = jm_vector_bsearch(jm_named_ptr)(&(md->variablesByName),&key, jm_compare_named);
		if(!found) {
			fmi2_xml_parse_error(context, "Derivative element references variable %s that is not defined", key.name);
			context->skipElementCnt = 1;
			return 0;
		}
		deriv = (fmi2_xml_variable_t*)found->ptr;
		key.name = jm_vector_get_itemp(char)(bufState,0);
		found = jm_vector_bsearch(jm_named_ptr)(&(md->variablesByName),&key, jm_compare_named);
		if(!found) {
			fmi2_xml_parse_error(context, "Derivative element references variable %s that is not defined", key.name);
			context->skipElementCnt = 1;
			return 0;
		}
		state = (fmi2_xml_variable_t*)found->ptr;
		if(!jm_vector_push_back(jm_voidp)(&ms->derivatives, deriv) ||
			!jm_vector_push_back(jm_voidp)(&ms->states, state)) {
				fmi2_xml_parse_fatal(context, "Could not allocate memory");
				return -1;
		}
		deriv->derivativeIndex = jm_vector_get_size(jm_voidp)(&ms->derivatives);
		state->stateIndex = jm_vector_get_size(jm_voidp)(&ms->states);

		return 
			fmi2_xml_parse_dependency(context, fmi2_xml_elmID_Derivative, 
											   fmi_attr_id_stateDependencies, fmi_attr_id_stateFactorKinds,
											    ms->depsStatesOnStates) ||
		    fmi2_xml_parse_dependency(context, fmi2_xml_elmID_Derivative, 
												  fmi_attr_id_inputDependencies, fmi_attr_id_inputFactorKinds,
												  ms->depsStatesOnInputs);
    }
    else {
    }
    return 0;
}


int fmi2_xml_handle_Output(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi2_xml_model_description_t* md = context->modelDescription;
		fmi2_xml_model_structure_t* ms = md->modelStructure;
        jm_vector(char)* bufName = fmi2_xml_reserve_parse_buffer(context,1,100);
		int derivative;
	    jm_named_ptr key, *found;
    
        fmi2_xml_variable_t* variable;
		
		if(
			/*  <xs:attribute name="name" type="xs:normalizedString" use="required"/> */
			fmi2_xml_set_attr_string(context, fmi2_xml_elmID_Output, fmi_attr_id_name, 1, bufName))
		{
			ms->isValidFlag = 0;
			context->skipElementCnt = 1;
			return 0;
		}
		key.name = jm_vector_get_itemp(char)(bufName,0);
		found = jm_vector_bsearch(jm_named_ptr)(&(md->variablesByName),&key, jm_compare_named);
		if(!found) {
			fmi2_xml_parse_error(context, "Output element references variable %s that is not defined", key.name);
			context->skipElementCnt = 1;
			return 0;
		}
		variable = (fmi2_xml_variable_t*)found->ptr;
        if(variable->causality != fmi2_causality_enu_output) {
			fmi2_xml_parse_error(context, 
				"Output element cannot reference '%s' since causality is not output. Skipping.", variable->name);
			context->skipElementCnt = 1;
            return 0;
        }

		if(!fmi2_xml_is_attr_defined(context, fmi_attr_id_derivative) ||
			/*	<xs:attribute name="derivative" type="xs:int"> */
			fmi2_xml_set_attr_int(context, fmi2_xml_elmID_Input, fmi_attr_id_derivative, 0, &derivative, -1)) {
			derivative = 0;
		}
		else {
			if(derivative < 1) {
				fmi2_xml_parse_error(context, "Output %s has attribute derivative=%d but indexing starts from one.", 
					variable->name, derivative);
				ms->isValidFlag = 0;
			}
		}

		if(!jm_vector_push_back(jm_voidp)(&ms->outputs, variable) ||
			!jm_vector_push_back(size_t)(&ms->outputIsDerivative, (size_t)derivative)) {
				fmi2_xml_parse_fatal(context, "Could not allocate memory");
				return -1;
		}

		variable->outputIndex = jm_vector_get_size(jm_voidp)(&ms->outputs);

		return 
			fmi2_xml_parse_dependency(context, fmi2_xml_elmID_Output, 
											   fmi_attr_id_stateDependencies, fmi_attr_id_stateFactorKinds,
											    ms->depsOutputsOnStates) ||
		    fmi2_xml_parse_dependency(context, fmi2_xml_elmID_Output, 
												  fmi_attr_id_inputDependencies, fmi_attr_id_inputFactorKinds,
												  ms->depsOutputsOnInputs);
    }
    else {
    }
    return 0;
}
