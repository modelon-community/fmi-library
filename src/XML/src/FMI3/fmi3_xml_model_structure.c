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

/** \file fmi3_xml_model_structure.c
*  \brief Implementation for the model structure interface.
*/
#include <string.h>
#include <stdio.h>

#include "fmi3_xml_parser.h"
#include "fmi3_xml_model_structure_impl.h"
#include "fmi3_xml_model_description_impl.h"

static const char * module = "FMI3XML";

fmi3_xml_model_structure_t* fmi3_xml_allocate_model_structure(jm_callbacks* cb) {
    fmi3_xml_model_structure_t* ms = (fmi3_xml_model_structure_t*)(cb->calloc(1, sizeof(fmi3_xml_model_structure_t)));
    if(!ms) return 0;

    jm_vector_init(jm_voidp)(&ms->outputs,0,cb);
    jm_vector_init(jm_voidp)(&ms->derivatives,0,cb);
    jm_vector_init(jm_voidp)(&ms->discreteStates,0,cb);
    jm_vector_init(jm_voidp)(&ms->initialUnknowns,0,cb);

    ms->isValidFlag = 1;

    ms->outputDeps = fmi3_xml_allocate_dependencies(cb);
    ms->derivativeDeps = fmi3_xml_allocate_dependencies(cb);
    ms->discreteStateDeps = fmi3_xml_allocate_dependencies(cb);
    ms->initialUnknownDeps = fmi3_xml_allocate_dependencies(cb);

    if(!ms->outputDeps || !ms->derivativeDeps || !ms->discreteStateDeps || !ms->initialUnknownDeps) {
        fmi3_xml_free_model_structure(ms);
        return 0;
    }

    return ms;
}

void fmi3_xml_free_model_structure(fmi3_xml_model_structure_t* ms) {
    jm_callbacks* cb;
    if(!ms) return;
    cb = ms->outputs.callbacks;

    jm_vector_free_data(jm_voidp)(&ms->outputs);
    jm_vector_free_data(jm_voidp)(&ms->derivatives);
    jm_vector_free_data(jm_voidp)(&ms->discreteStates);
    jm_vector_free_data(jm_voidp)(&ms->initialUnknowns);

    fmi3_xml_free_dependencies(ms->outputDeps);
    fmi3_xml_free_dependencies(ms->derivativeDeps);
    fmi3_xml_free_dependencies(ms->discreteStateDeps);
    fmi3_xml_free_dependencies(ms->initialUnknownDeps);
    cb->free(ms);
}

jm_vector(jm_voidp)* fmi3_xml_get_outputs(fmi3_xml_model_structure_t* ms) {
    return &ms->outputs;
}

jm_vector(jm_voidp)* fmi3_xml_get_derivatives(fmi3_xml_model_structure_t* ms){
    return &ms->derivatives;
}

jm_vector(jm_voidp)* fmi3_xml_get_discrete_states(fmi3_xml_model_structure_t* ms){
    return &ms->discreteStates;
}

jm_vector(jm_voidp)* fmi3_xml_get_initial_unknowns(fmi3_xml_model_structure_t* ms){
    return &ms->initialUnknowns;
}


void fmi3_xml_get_dependencies(fmi3_xml_dependencies_t* dep, size_t** startIndex, size_t** dependency, char** factorKind){
    if(dep) {
        if (jm_vector_get_size(size_t)(&dep->dependencyIndex) == 0) {
            *startIndex = NULL;
            *dependency = NULL;
            *factorKind = NULL;
        } else {
            *startIndex = jm_vector_get_itemp(size_t)(&dep->startIndex, 0);
            *dependency = jm_vector_get_itemp(size_t)(&dep->dependencyIndex, 0);
            *factorKind = jm_vector_get_itemp(char)(&dep->dependencyFactorKind, 0);
        }
    }
    else {
        *startIndex = 0;
    }
}

void fmi3_xml_get_outputs_dependencies(fmi3_xml_model_structure_t* ms,
                                           size_t** startIndex, size_t** dependency, char** factorKind) {
    fmi3_xml_get_dependencies(ms->outputDeps, startIndex, dependency, factorKind);
}

void fmi3_xml_get_derivatives_dependencies(fmi3_xml_model_structure_t* ms,
                                           size_t** startIndex, size_t** dependency, char** factorKind) {
    fmi3_xml_get_dependencies(ms->derivativeDeps, startIndex, dependency, factorKind);
}

void fmi3_xml_get_discrete_states_dependencies(fmi3_xml_model_structure_t* ms,
                                           size_t** startIndex, size_t** dependency, char** factorKind) {
    fmi3_xml_get_dependencies(ms->discreteStateDeps, startIndex, dependency, factorKind);
}

void fmi3_xml_get_initial_unknowns_dependencies(fmi3_xml_model_structure_t* ms,
                                           size_t** startIndex, size_t** dependency, char** factorKind) {
    fmi3_xml_get_dependencies(ms->initialUnknownDeps, startIndex, dependency, factorKind);
}


fmi3_xml_dependencies_t* fmi3_xml_allocate_dependencies(jm_callbacks* cb) {
    fmi3_xml_dependencies_t* dep = (fmi3_xml_dependencies_t*)(cb->malloc(sizeof(fmi3_xml_dependencies_t)));
    if(!dep) return 0;
    jm_vector_init(size_t)(&dep->startIndex, 0, cb);
    jm_vector_push_back(size_t)(&dep->startIndex, 0);

    jm_vector_init(size_t)(&dep->dependencyIndex, 0, cb);
    jm_vector_init(char)(&dep->dependencyFactorKind, 0, cb);

    dep->isRowMajor = 1;

    return dep;
}

void fmi3_xml_zero_empty_dependencies(fmi3_xml_dependencies_t** pdep) {
    fmi3_xml_dependencies_t* dep =*pdep;
    size_t ndep = jm_vector_get_size(size_t)(&dep->dependencyIndex);
    size_t i;
    if(!dep) return;
    for(i = 0; i<ndep;i++) {
        if(jm_vector_get_item(size_t)(&dep->dependencyIndex, i)) break;
    }
    if(i == ndep) {
        fmi3_xml_free_dependencies(dep);
        *pdep = 0;
    }
}


void fmi3_xml_free_dependencies(fmi3_xml_dependencies_t* dep) {
    jm_callbacks* cb;
    if(!dep) return;
    cb = dep->startIndex.callbacks;
    jm_vector_free_data(size_t)(&dep->startIndex);

    jm_vector_free_data(size_t)(&dep->dependencyIndex);
    jm_vector_free_data(char)(&dep->dependencyFactorKind);
    cb->free(dep);
}


int fmi3_xml_check_model_structure(fmi3_xml_model_description_t* md) {
    fmi3_xml_model_structure_t* ms = md->modelStructure;

    if(!ms || !ms->isValidFlag) return 0;

    return ms->isValidFlag;
}

int fmi3_xml_handle_ModelStructure(fmi3_xml_parser_context_t *context, const char* data) {
    fmi3_xml_model_description_t* md = context->modelDescription;
    if(!data) {
        jm_log_verbose(context->callbacks, module,"Parsing XML element ModelStructure");
        /** allocate model structure */
        md->modelStructure = fmi3_xml_allocate_model_structure(md->callbacks);
        if(!md->modelStructure) {
                fmi3_xml_parse_fatal(context, module, "Could not allocate memory");
                return -1;
        }
    }
    else {
        /** make sure model structure information is consistent */

        if(!fmi3_xml_check_model_structure(md)) {
            fmi3_xml_parse_fatal(context, "Model structure is not valid due to detected errors. Cannot continue.");
            return -1;
        }
/*        md->numberOfContinuousStates = jm_vector_get_size(jm_voidp)(&md->modelStructure->states); */

    }
    return 0;
}


int fmi3_xml_handle_Outputs(fmi3_xml_parser_context_t *context, const char* data) {
    if (!data) {
        jm_log_verbose(context->callbacks, module, "Parsing XML element Outputs");
        /*  reset handles for the elements that are specific under Outputs */
/*        fmi3_xml_set_element_handle(context, "Unknown", FMI3_XML_ELM_ID(OutputUnknown));*/
        fmi3_xml_set_element_handle(context, "Unknown", FMI3_XML_ELM_ID(Unknown));
    }
    return 0;
}
int fmi3_xml_handle_Derivatives(fmi3_xml_parser_context_t *context, const char* data) {
    if (!data) {
        jm_log_verbose(context->callbacks, module, "Parsing XML element Derivatives");
        /*  reset handles for the elements that are specific under Derivatives */
        fmi3_xml_set_element_handle(context, "Unknown", FMI3_XML_ELM_ID(DerivativeUnknown));
    }
    else {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_model_structure_t* ms = md->modelStructure;
        /* count the number of continuous states as the number of <Unknown> elements under <Derivatives> */
        md->numberOfContinuousStates = jm_vector_get_size(jm_voidp)(&ms->derivatives);
    }
    return 0;
}
int fmi3_xml_handle_DiscreteStates(fmi3_xml_parser_context_t *context, const char* data) {
    if (!data) {
        jm_log_verbose(context->callbacks, module, "Parsing XML element DiscreteStates");
        /*  reset handles for the elements that are specific under DiscreteStates */
        fmi3_xml_set_element_handle(context, "Unknown", FMI3_XML_ELM_ID(DiscreteStateUnknown));
    }
    return 0;
}
int fmi3_xml_handle_InitialUnknowns(fmi3_xml_parser_context_t *context, const char* data) {
    if (!data) {
        jm_log_verbose(context->callbacks, module, "Parsing XML element InitialUnknowns");
        /*  reset handles for the elements that are specific under InitialUnknowns */
        fmi3_xml_set_element_handle(context, "Unknown", FMI3_XML_ELM_ID(InitialUnknown));
    }
    return 0;
}

/**
 * Parse the dependencies of an Unknown element
 */
int fmi3_xml_parse_dependencies(fmi3_xml_parser_context_t *context,
                                fmi3_xml_elm_enu_t parentElmID,
                                fmi3_xml_dependencies_t* deps)
{
    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_model_structure_t* ms = md->modelStructure;

    const char* listInd;
    const char* listKind;
    size_t numDepInd = 0;
    size_t numDepKind = 0;
    size_t totNumDep = jm_vector_get_size(size_t)(&deps->dependencyIndex);

    /*  <xs:attribute name="dependencies">
            <xs:simpleType>
                <xs:list itemType="xs:unsignedInt"/>
            </xs:simpleType>
        </xs:attribute> */
    if(fmi3_xml_get_attr_str(context, fmi3_xml_elmID_Unknown, fmi_attr_id_dependencies, 0, &listInd)) {
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
                fmi3_xml_parse_error(context, "XML element 'Unknown': could not parse item %d in the list for attribute 'dependencies'",
                    numDepInd);
               ms->isValidFlag = 0;
               return 0;
            }
            if(ind < 1) {
                fmi3_xml_parse_error(context, "XML element 'Unknown': item %d=%d is less than one in the list for attribute 'dependencies'",
                    numDepInd, ind);
               ms->isValidFlag = 0;
               return 0;
            }
            if(!jm_vector_push_back(size_t)(&deps->dependencyIndex, (size_t)ind)) {
               fmi3_xml_parse_fatal(context, "Could not allocate memory");
               return -1;
            }
            while((*cur >= '0') && (*cur <= '9')) cur++;
            numDepInd++;
        }
    }

    /*
        <xs:attribute name="dependenciesKind">
            <xs:simpleType>
                <xs:list>
                    <xs:simpleType>
                        <xs:restriction base="xs:normalizedString">
                            <xs:enumeration value="dependent"/>
                            <xs:enumeration value="constant"/>
                            <xs:enumeration value="fixed"/>
                            <xs:enumeration value="tunable"/>
                            <xs:enumeration value="discrete"/>
                        </xs:restriction>
                    </xs:simpleType>
                </xs:list>
            </xs:simpleType>
        </xs:attribute>
        */
    if(fmi3_xml_get_attr_str(context, fmi3_xml_elmID_Unknown, fmi_attr_id_dependenciesKind, 0, &listKind)) {
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
             if(strncmp("dependent", cur, 9) == 0) {
                 kind = fmi3_dependency_factor_kind_dependent;
                 cur+=9;
             }
             else if(strncmp("constant", cur, 8) == 0) {
                 kind = fmi3_dependency_factor_kind_constant;
                 cur+=8;
             }
             else if(strncmp("fixed", cur, 5) == 0) {
                 kind = fmi3_dependency_factor_kind_fixed;
                 cur+=5;
             }
             else if(strncmp("tunable", cur, 7) == 0) {
                 kind = fmi3_dependency_factor_kind_tunable;
                 cur+=7;
             }
             else if(strncmp("discrete", cur, 8) == 0) {
                  kind = fmi3_dependency_factor_kind_discrete;
                 cur+=8;
             }
             else {
                 fmi3_xml_parse_error(context, "XML element 'Unknown': could not parse item %d in the list for attribute 'dependenciesKind'",
                     numDepKind);
                 ms->isValidFlag = 0;
                 return 0;
             }
             if (parentElmID == fmi3_xml_elmID_InitialUnknowns) {
                if (kind == fmi3_dependency_factor_kind_fixed) {
                    fmi3_xml_parse_error(context, "XML element 'Unknown' within 'InitialUnknowns': 'fixed' is not allowed in list for attribute 'dependenciesKind'; setting to 'dependent'");
                    kind = fmi3_dependency_factor_kind_dependent;
                }
                else if (!(kind == fmi3_dependency_factor_kind_dependent || kind == fmi3_dependency_factor_kind_constant)) {
                    fmi3_xml_parse_error(context, "XML element 'Unknown' within 'InitialUnknowns': only 'dependent' and 'constant' allowed in list for attribute 'dependenciesKind'");
                    ms->isValidFlag = 0;
                    return 0;
                }
             }
             if(!jm_vector_push_back(char)(&deps->dependencyFactorKind, kind)) {
                fmi3_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
             numDepKind++;
         }
    }
    if(listInd && listKind) {
        /* both lists are present - the number of items must match */
        if(numDepInd != numDepKind) {
            fmi3_xml_parse_error(context, "XML element 'Unknown': different number of items (%u and %u) in the lists for 'dependencies' and 'dependenciesKind'",
                                 numDepInd, numDepKind);
            ms->isValidFlag = 0;
            return 0;
        }
    }
    else if(listInd) {
        /* only Dependencies are present, set all kinds to dependent */
        char kind = fmi3_dependency_factor_kind_dependent;
        if(jm_vector_reserve(char)(&deps->dependencyFactorKind,totNumDep + numDepInd) < totNumDep + numDepInd) {
            fmi3_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }
        for(;numDepKind < numDepInd; numDepKind++)
            jm_vector_push_back(char)(&deps->dependencyFactorKind, kind);
    }
    else if(listKind) {
        fmi3_xml_parse_error(context, "XML element 'Unknown': if `dependenciesKind` attribute is present then the `dependencies` attribute must be present also.");
        ms->isValidFlag = 0;
        return 0;
    }
    else {
        /* Dependencies are not provided. Put zero index/dependent to indicate that full row must be considered. */
        numDepInd = numDepKind = 1;
        if(!jm_vector_push_back(char)(&deps->dependencyFactorKind, fmi3_dependency_factor_kind_dependent) ||
         !jm_vector_push_back(size_t)(&deps->dependencyIndex, 0)
          ) {
            fmi3_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }
    }
    if(!jm_vector_push_back(size_t)(&deps->startIndex, totNumDep + numDepInd)) {
        fmi3_xml_parse_fatal(context, "Could not allocate memory");
        return -1;
    }

    return 0;
}


/**
 * Parses an Unknown element
 * After successful call, the variable will be put on the destVarList stack,
 * and the dependencies are stored in return-arg 'deps'.
 */
int fmi3_xml_parse_unknown(fmi3_xml_parser_context_t *context,
                           fmi3_xml_elm_enu_t parentElmID,
                           jm_vector(jm_voidp) *destVarList,
                           fmi3_xml_dependencies_t* deps)
{
    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_model_structure_t* ms = md->modelStructure;

    unsigned int index;
    fmi3_xml_variable_t* variable;

    /* <xs:attribute name="index" type="xs:unsignedInt" use="required"> */
    if (fmi3_xml_set_attr_uint32(context, fmi3_xml_elmID_Unknown, fmi_attr_id_index, 1, &index, 0)) return -1;
    index--; /* Convert from one- to zero-based indexing */

    /* Ok to just check upper bound since index is unsigned. */
    if (index >= jm_vector_get_size(jm_voidp)(md->variablesOrigOrder)) {
        fmi3_xml_parse_error(context, "The index attribute must have a value between 1 and the number of model variables.");
        ms->isValidFlag = 0;
        return -1;
    }
    variable = (fmi3_xml_variable_t*)jm_vector_get_item(jm_voidp)(md->variablesOrigOrder, index);

    if (!jm_vector_push_back(jm_voidp)(destVarList, variable)) {
        fmi3_xml_parse_fatal(context, "Could not allocate memory");
        ms->isValidFlag = 0;
        return -1;
    }

    return fmi3_xml_parse_dependencies(context, parentElmID, deps);
}


/*int fmi3_xml_handle_OutputUnknown(fmi3_xml_parser_context_t *context, const char* data) {*/
int fmi3_xml_handle_Unknown(fmi3_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_model_structure_t* ms = md->modelStructure;

        return fmi3_xml_parse_unknown(context, fmi3_xml_elmID_Outputs, &ms->outputs, ms->outputDeps);
    }
    else {
    }
    return 0;
}

int fmi3_xml_handle_DerivativeUnknown(fmi3_xml_parser_context_t *context, const char* data) {
    if (!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_model_structure_t* ms = md->modelStructure;
        fmi3_xml_variable_t* derXX; /* float64 or float32 variable */
        int validDeriv; /* valid derivative found */

        /* perform the parsing */
        int status = fmi3_xml_parse_unknown(context, fmi3_xml_elmID_Derivatives, &ms->derivatives, ms->derivativeDeps);
        if (status)
            return status;


        /* validate return values */

        /* derivatives can be any of floatXX */
        derXX = (fmi3_xml_variable_t*)jm_vector_get_last(jm_voidp)(&ms->derivatives);
        if (derXX->type->baseType == fmi3_base_type_float64) {
            fmi3_xml_float64_variable_t* der = (fmi3_xml_float64_variable_t*)jm_vector_get_last(jm_voidp)(&ms->derivatives);
            validDeriv = fmi3_xml_get_float64_variable_derivative_of(der) != NULL;
        }
        else { /* float32 */
            fmi3_xml_float32_variable_t* der = (fmi3_xml_float32_variable_t*)jm_vector_get_last(jm_voidp)(&ms->derivatives);
            validDeriv = fmi3_xml_get_float32_variable_derivative_of(der) != NULL;
        }
        if (!validDeriv) {
            ms->isValidFlag = 0;
            fmi3_xml_parse_error(context,
                    "The state derivative '%s' does not specify the state variable that it is a derivative of.",
                    fmi3_xml_get_variable_name(derXX));
            return -1;
        }
    }
    return 0;
}

int fmi3_xml_handle_DiscreteStateUnknown(fmi3_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_model_structure_t* ms = md->modelStructure;

        return fmi3_xml_parse_unknown(context, fmi3_xml_elmID_DiscreteStates, &ms->discreteStates, ms->discreteStateDeps);
    }
    else {
    }
    return 0;
}

int fmi3_xml_handle_InitialUnknown(fmi3_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_model_structure_t* ms = md->modelStructure;

        return fmi3_xml_parse_unknown(context, fmi3_xml_elmID_InitialUnknowns, &ms->initialUnknowns, ms->initialUnknownDeps);
    }
    else {
    }
    return 0;
}
