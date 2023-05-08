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
#include <inttypes.h>

#include "fmi3_xml_parser.h"
#include "fmi3_xml_model_structure_impl.h"
#include "fmi3_xml_model_description_impl.h"

static const char * module = "FMI3XML";

fmi3_xml_model_structure_t* fmi3_xml_allocate_model_structure(jm_callbacks* cb) {
    fmi3_xml_model_structure_t* ms = (fmi3_xml_model_structure_t*)(cb->calloc(1, sizeof(fmi3_xml_model_structure_t)));
    if(!ms) return 0;

    jm_vector_init(jm_voidp)(&ms->outputs,0,cb);
    jm_vector_init(jm_voidp)(&ms->continuousStateDerivatives,0,cb);
    jm_vector_init(jm_voidp)(&ms->clockedStates,0,cb);
    jm_vector_init(jm_voidp)(&ms->initialUnknowns,0,cb);
    jm_vector_init(jm_voidp)(&ms->eventIndicators,0,cb);

    ms->isValidFlag = 1;

    ms->outputDeps = fmi3_xml_allocate_dependencies(cb);
    ms->continuousStateDerivativeDeps = fmi3_xml_allocate_dependencies(cb);
    ms->clockedStateDeps = fmi3_xml_allocate_dependencies(cb);
    ms->initialUnknownDeps = fmi3_xml_allocate_dependencies(cb);
    ms->eventIndicatorDeps = fmi3_xml_allocate_dependencies(cb);

    if(!ms->outputDeps || !ms->continuousStateDerivativeDeps || !ms->initialUnknownDeps ||
       !ms->clockedStateDeps || !ms->eventIndicatorDeps) {
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
    jm_vector_free_data(jm_voidp)(&ms->continuousStateDerivatives);
    jm_vector_free_data(jm_voidp)(&ms->clockedStates);
    jm_vector_free_data(jm_voidp)(&ms->initialUnknowns);
    jm_vector_free_data(jm_voidp)(&ms->eventIndicators);

    fmi3_xml_free_dependencies(ms->outputDeps);
    fmi3_xml_free_dependencies(ms->continuousStateDerivativeDeps);
    fmi3_xml_free_dependencies(ms->clockedStateDeps);
    fmi3_xml_free_dependencies(ms->initialUnknownDeps);
    fmi3_xml_free_dependencies(ms->eventIndicatorDeps);
    cb->free(ms);
}

jm_vector(jm_voidp)* fmi3_xml_get_outputs(fmi3_xml_model_structure_t* ms) {
    return &ms->outputs;
}

jm_vector(jm_voidp)* fmi3_xml_get_continuous_state_derivatives(fmi3_xml_model_structure_t* ms){
    return &ms->continuousStateDerivatives;
}

jm_vector(jm_voidp)* fmi3_xml_get_clocked_states(fmi3_xml_model_structure_t* ms){
    return &ms->clockedStates;
}

jm_vector(jm_voidp)* fmi3_xml_get_initial_unknowns(fmi3_xml_model_structure_t* ms){
    return &ms->initialUnknowns;
}

jm_vector(jm_voidp)* fmi3_xml_get_event_indicators(fmi3_xml_model_structure_t* ms){
    return &ms->eventIndicators;
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

void fmi3_xml_get_continuous_state_derivatives_dependencies(fmi3_xml_model_structure_t* ms,
                                           size_t** startIndex, size_t** dependency, char** factorKind) {
    fmi3_xml_get_dependencies(ms->continuousStateDerivativeDeps, startIndex, dependency, factorKind);
}

void fmi3_xml_get_clocked_states_dependencies(fmi3_xml_model_structure_t* ms,
                                           size_t** startIndex, size_t** dependency, char** factorKind) {
    fmi3_xml_get_dependencies(ms->clockedStateDeps, startIndex, dependency, factorKind);
}

void fmi3_xml_get_initial_unknowns_dependencies(fmi3_xml_model_structure_t* ms,
                                           size_t** startIndex, size_t** dependency, char** factorKind) {
    fmi3_xml_get_dependencies(ms->initialUnknownDeps, startIndex, dependency, factorKind);
}

void fmi3_xml_get_event_indicators_dependencies(fmi3_xml_model_structure_t* ms,
                                           size_t** startIndex, size_t** dependency, char** factorKind) {
    fmi3_xml_get_dependencies(ms->eventIndicatorDeps, startIndex, dependency, factorKind);
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

        // TODO: Check that all ModelVariables with causality="output" are Outputs

        if(!fmi3_xml_check_model_structure(md)) {
            fmi3_xml_parse_fatal(context, "Model structure is not valid due to detected errors. Cannot continue.");
            return -1;
        }

        md->numberOfContinuousStates = jm_vector_get_size(jm_voidp)(&md->modelStructure->continuousStateDerivatives);
    }
    return 0;
}

/**
 * Parse the dependencies of an element in ModelStructure
 */
int fmi3_xml_parse_dependencies(fmi3_xml_parser_context_t *context,
                                fmi3_xml_elm_enu_t elmID,
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
    if(fmi3_xml_get_attr_str(context, elmID, fmi_attr_id_dependencies, 0, &listInd)) {
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
                // TODO: Rework this error; list actual type
                fmi3_xml_parse_error(context, "XML element 'Unknown': could not parse item %d in the list for attribute 'dependencies'",
                    numDepInd);
               ms->isValidFlag = 0;
               return 0;
            }
            if(ind < 1) {
                // TODO: Rework this error; list actual type
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
    if(fmi3_xml_get_attr_str(context, elmID, fmi_attr_id_dependenciesKind, 0, &listKind)) {
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
                // TODO: Rework this error; list actual type
                 fmi3_xml_parse_error(context, "XML element 'Unknown': could not parse item %d in the list for attribute 'dependenciesKind'",
                     numDepKind);
                 ms->isValidFlag = 0;
                 return 0;
             }
             if (elmID == fmi3_xml_elmID_InitialUnknown) {
                if (kind == fmi3_dependency_factor_kind_fixed) {
                    fmi3_xml_parse_error(context, "XML element 'InitialUnknown': 'fixed' is not allowed in list for attribute 'dependenciesKind'; setting to 'dependent'");
                    kind = fmi3_dependency_factor_kind_dependent;
                }
                else if (!(kind == fmi3_dependency_factor_kind_dependent || kind == fmi3_dependency_factor_kind_constant)) {
                    fmi3_xml_parse_error(context, "XML element 'InitialUnknown': only 'dependent' and 'constant' allowed in list for attribute 'dependenciesKind'");
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
            // TODO: Reference actual XML element type
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
        // TODO: Reference actual XML element type
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
 * Parses an fmi3Unknown element(abstract parent to elements in ModelStructure)
 * After successful call, the variable will be put on the destVarList stack,
 * and the dependencies are stored in return-arg 'deps'.
 */
int fmi3_xml_parse_unknown(fmi3_xml_parser_context_t *context,
                           fmi3_xml_elm_enu_t elmID,
                           jm_vector(jm_voidp) *destVarList,
                           fmi3_xml_dependencies_t* deps)
{
    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_model_structure_t* ms = md->modelStructure;

    fmi3_value_reference_t vr;
    fmi3_xml_variable_t* variable;

    if (fmi3_xml_set_attr_uint32(context, elmID, fmi_attr_id_valueReference, 1, &vr, 0)) return -1;

    variable = fmi3_xml_get_variable_by_vr(md, vr);
    if (!variable) {
        fmi3_xml_parse_error(context, "Failed to find variable for valueReference=%" PRId32 ".");
        ms->isValidFlag = 0;
        return -1;
    }
    if (!jm_vector_push_back(jm_voidp)(destVarList, variable)) {
        fmi3_xml_parse_fatal(context, "Could not allocate memory");
        ms->isValidFlag = 0;
        return -1;
    }

    return fmi3_xml_parse_dependencies(context, elmID, deps);
}

int fmi3_xml_handle_Output(fmi3_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_model_structure_t* ms = md->modelStructure;

        int status =  fmi3_xml_parse_unknown(context, fmi3_xml_elmID_Output, &ms->outputs, ms->outputDeps);
        if (status)
            return status;

        // Check for correct causality
        fmi3_xml_variable_t* var = (fmi3_xml_variable_t*)jm_vector_get_last(jm_voidp)(&ms->outputs);
        if (var->causality != fmi3_causality_enu_output){
            ms->isValidFlag = 0;
            fmi3_xml_parse_error(context,
                    "The Output '%s' does not have causality='output'.",
                    fmi3_xml_get_variable_name(var));
            return -1;
        }
    }
    return 0;
}

int fmi3_xml_handle_ContinuousStateDerivative(fmi3_xml_parser_context_t *context, const char* data) {
    if(!data) {

        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_model_structure_t* ms = md->modelStructure;
        fmi3_xml_variable_t* derXX; /* float64 or float32 variable */
        int validDeriv; /* valid derivative found */

        /* perform the parsing */
        if (fmi3_xml_parse_unknown(context, fmi3_xml_elmID_ContinuousStateDerivative,
                                   &ms->continuousStateDerivatives, ms->continuousStateDerivativeDeps)) {
            return -1;
        }

        /* validate return values */

        /* continuosStateDerivatives can be any of floatXX */
        derXX = (fmi3_xml_variable_t*)jm_vector_get_last(jm_voidp)(&ms->continuousStateDerivatives);
        if (derXX->type->baseType == fmi3_base_type_float64) {
            fmi3_xml_float64_variable_t* der = (fmi3_xml_float64_variable_t*)derXX;
            validDeriv = fmi3_xml_get_float64_variable_derivative_of(der) != NULL;
        }
        else { /* float32 */
            fmi3_xml_float32_variable_t* der = (fmi3_xml_float32_variable_t*)derXX;
            validDeriv = fmi3_xml_get_float32_variable_derivative_of(der) != NULL;
        }
        if (!validDeriv) {
            ms->isValidFlag = 0;
            fmi3_xml_parse_error(context,
                    "The continuous state derivative '%s' does not specify the state variable that it is a derivative of.",
                    fmi3_xml_get_variable_name(derXX));
            return -1;
        }
    }
    return 0;
}

int fmi3_xml_handle_ClockedState(fmi3_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_model_structure_t* ms = md->modelStructure;

        /* perform the parsing */
        if (fmi3_xml_parse_unknown(context, fmi3_xml_elmID_ClockedState,
                                   &ms->clockedStates, ms->clockedStateDeps)) {
            return -1;
        }

        /* validate return values */
        fmi3_xml_variable_t* clock_var = (fmi3_xml_variable_t*)jm_vector_get_last(jm_voidp)(&ms->clockedStates);

        /* clock attribute required */
        if (!fmi3_xml_get_variable_has_clocks(clock_var)){
            ms->isValidFlag = 0;
            fmi3_xml_parse_error(context,
                    "The ClockedState '%s' does not have the attribute='clocks'.",
                    fmi3_xml_get_variable_name(clock_var));
            return -1;

        }

        /* previous attribute is required */
        if (!fmi3_xml_get_previous(clock_var)){
            ms->isValidFlag = 0;
            fmi3_xml_parse_error(context,
                    "The ClockedState '%s' does not have the attribute 'previous'.",
                    fmi3_xml_get_variable_name(clock_var));
            return -1;
        }

        /* must be discrete */
        if (fmi3_xml_get_variability(clock_var) != fmi3_variability_enu_discrete){
            ms->isValidFlag = 0;
            fmi3_xml_parse_error(context,
                    "The ClockedState '%s' does not have variability='discrete'.",
                    fmi3_xml_get_variable_name(clock_var));
            return -1;
        }

        /* must not be of base type fmi3Clock */
        if (fmi3_xml_get_variable_base_type(clock_var) == fmi3_base_type_clock){
            ms->isValidFlag = 0;
            fmi3_xml_parse_error(context,
                    "The ClockedState '%s' must not have the base type fmi3Clock.",
                    fmi3_xml_get_variable_name(clock_var));
            return -1;
        }
    }
    return 0;
}

int fmi3_xml_handle_InitialUnknown(fmi3_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_model_structure_t* ms = md->modelStructure;

        return fmi3_xml_parse_unknown(context, fmi3_xml_elmID_InitialUnknown, &ms->initialUnknowns, ms->initialUnknownDeps);
    }
    return 0;
}

int fmi3_xml_handle_EventIndicator(fmi3_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi3_xml_model_description_t* md = context->modelDescription;
        fmi3_xml_model_structure_t* ms = md->modelStructure;

        // Ignored if Co-simulation & scheduled execution
        fmi3_fmu_kind_enu_t fmu_kind = fmi3_xml_get_fmu_kind(md);
        if ((fmu_kind == fmi3_fmu_kind_cs) || (fmu_kind == fmi3_fmu_kind_se)){
            jm_log_info(md->callbacks, "FMI3XML", "EventIndicator ignored since FMU kind is Co-Simulation or Scheduled Excecution.");
            return 0;
        }

        /* perform the parsing */
        if (fmi3_xml_parse_unknown(context, fmi3_xml_elmID_EventIndicator,
                                   &ms->eventIndicators, ms->eventIndicatorDeps)) {
            return -1;
        }

        /* validate return values */

        fmi3_xml_variable_t* event_ind = (fmi3_xml_variable_t*)jm_vector_get_last(jm_voidp)(&ms->eventIndicators);
        /* EventIndicator must be continuous */
        if (fmi3_xml_get_variability(event_ind) != fmi3_variability_enu_continuous){
            ms->isValidFlag = 0;
            fmi3_xml_parse_error(context,
                    "The EventIndicator '%s' does not have variability='continuous'.",
                    fmi3_xml_get_variable_name(event_ind));
            return -1;
        }

        fmi3_base_type_enu_t base_type = fmi3_xml_get_variable_base_type(event_ind);
        if ((base_type != fmi3_base_type_float64) && (base_type != fmi3_base_type_float32)){
            ms->isValidFlag = 0;
            fmi3_xml_parse_error(context,
                    "The EventIndicator '%s' does not have the base type 'Float32' or 'Float64'.",
                    fmi3_xml_get_variable_name(event_ind));
            return -1;
        }
    }
    return 0;
}
