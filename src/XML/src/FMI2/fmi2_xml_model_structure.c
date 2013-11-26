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
/*
		if(!fmi2_xml_check_model_structure(md)) {
			fmi2_xml_parse_fatal(context, "Model structure is not valid due to detected errors. Cannot continue.");
			return -1;
		}
		md->numberOfContinuousStates = jm_vector_get_size(jm_voidp)(&md->modelStructure->states);
*/
    }
    return 0;
}


int fmi2_xml_handle_Outputs(fmi2_xml_parser_context_t *context, const char* data) {
		/** nothing to do */
    return 0;
}
int fmi2_xml_handle_Derivatives(fmi2_xml_parser_context_t *context, const char* data) {
		/** nothing to do */
    return 0;
}

int fmi2_xml_handle_Derivative(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
    }
    else {
    }
    return 0;
}

int fmi2_xml_handle_Output(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
    }
    else {
    }
    return 0;
}
