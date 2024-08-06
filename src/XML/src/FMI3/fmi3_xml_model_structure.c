/*
    Copyright (C) 2023 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
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

static const char* module = "FMI3XML";

fmi3_xml_model_structure_t* fmi3_xml_allocate_model_structure(jm_callbacks* cb) {
    fmi3_xml_model_structure_t* ms = (fmi3_xml_model_structure_t*)(cb->calloc(1, sizeof(fmi3_xml_model_structure_t)));
    if (!ms) return NULL;

    jm_vector_init(jm_voidp)(&ms->outputs,0,cb);
    jm_vector_init(jm_voidp)(&ms->continuousStateDerivatives,0,cb);
    jm_vector_init(jm_voidp)(&ms->clockedStates,0,cb);
    jm_vector_init(jm_voidp)(&ms->initialUnknowns,0,cb);
    jm_vector_init(jm_voidp)(&ms->eventIndicators,0,cb);

    ms->isValid = 1;

    ms->outputDeps = fmi3_xml_allocate_dependencies(cb);
    ms->continuousStateDerivativeDeps = fmi3_xml_allocate_dependencies(cb);
    ms->clockedStateDeps = fmi3_xml_allocate_dependencies(cb);
    ms->initialUnknownDeps = fmi3_xml_allocate_dependencies(cb);
    ms->eventIndicatorDeps = fmi3_xml_allocate_dependencies(cb);

    if (!ms->outputDeps || !ms->continuousStateDerivativeDeps || !ms->initialUnknownDeps ||
       !ms->clockedStateDeps || !ms->eventIndicatorDeps) {
        fmi3_xml_free_model_structure(ms);
        return NULL;
    }

    return ms;
}

void fmi3_xml_free_model_structure(fmi3_xml_model_structure_t* ms) {
    jm_callbacks* cb;
    if (!ms) return;
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

// For trace-ability, in case we want to attach an error message to first call
static void fmi3_xml_set_model_structure_invalid(fmi3_xml_model_structure_t* ms) {
    if (ms) {ms->isValid = 0;}
}

jm_vector(jm_voidp)* fmi3_xml_get_outputs(fmi3_xml_model_structure_t* ms) {
    return &ms->outputs;
}

jm_vector(jm_voidp)* fmi3_xml_get_continuous_state_derivatives(fmi3_xml_model_structure_t* ms) {
    return &ms->continuousStateDerivatives;
}

jm_vector(jm_voidp)* fmi3_xml_get_clocked_states(fmi3_xml_model_structure_t* ms) {
    return &ms->clockedStates;
}

jm_vector(jm_voidp)* fmi3_xml_get_initial_unknowns(fmi3_xml_model_structure_t* ms) {
    return &ms->initialUnknowns;
}

jm_vector(jm_voidp)* fmi3_xml_get_event_indicators(fmi3_xml_model_structure_t* ms) {
    return &ms->eventIndicators;
}

static int fmi3_xml_get_dependencies(jm_vector(jm_voidp)* msVector, 
        fmi3_xml_dependencies_t* dep, fmi3_xml_variable_t* variable,
        size_t* numDependencies, int* dependsOnAll, size_t** dependencies, char** dependenciesKind)
{
    if (!msVector || !dep || !variable) return -1;

    int varFound = 0;
    size_t numOutputs = jm_vector_get_size(jm_voidp)(msVector);
    fmi3_value_reference_t inputVR = fmi3_xml_get_variable_vr(variable);

    // Do a linear search to find index of <variable> in <msVector>
    size_t varIndex;
    fmi3_xml_variable_t* var;
    for (varIndex = 0; varIndex < numOutputs; varIndex++) {
        var = (fmi3_xml_variable_t*)jm_vector_get_item(jm_voidp)(msVector, varIndex);
        if (fmi3_xml_get_variable_vr(var) == inputVR) {
            varFound = 1;
            break;
        }
    }
    if (varFound == 0) return 1; // variable not found
    // variable found & varIndex = index of variable in msVector list

    size_t startIndex = jm_vector_get_item(size_t)(&dep->startIndex, varIndex);
    *numDependencies = jm_vector_get_item(size_t)(&dep->startIndex, varIndex + 1) - startIndex;

    if (*numDependencies == 0) { // no dependencies or "depends on all"
        if (jm_vector_get_item(char)(&dep->dependencyOnAll, varIndex)) {
            *dependsOnAll = 1;
        } else {
            *dependsOnAll = 0;
        }
        *dependencies = NULL;
        *dependenciesKind = NULL;
    } else {
        *dependsOnAll = 0;
        *dependencies = jm_vector_get_itemp(size_t)(&dep->dependenciesVRs, startIndex);
        *dependenciesKind = jm_vector_get_itemp(char)(&dep->dependenciesKind, startIndex);
    }
    return 0; // OK
}

int fmi3_xml_get_output_dependencies(fmi3_xml_model_structure_t* ms, fmi3_xml_variable_t* variable,
        size_t* numDependencies, int* dependsOnAll, size_t** dependencies, char** dependenciesKind)
{
    return fmi3_xml_get_dependencies(&(ms->outputs), ms->outputDeps, variable,
            numDependencies, dependsOnAll, dependencies, dependenciesKind);
}

int fmi3_xml_get_continuous_state_derivative_dependencies(fmi3_xml_model_structure_t* ms, fmi3_xml_variable_t* variable,
        size_t* numDependencies, int* dependsOnAll, size_t** dependencies, char** dependenciesKind)
{
    return fmi3_xml_get_dependencies(&(ms->continuousStateDerivatives), ms->continuousStateDerivativeDeps, variable,
            numDependencies, dependsOnAll, dependencies, dependenciesKind);
}

int fmi3_xml_get_clocked_state_dependencies(fmi3_xml_model_structure_t* ms, fmi3_xml_variable_t* variable,
        size_t* numDependencies, int* dependsOnAll, size_t** dependencies, char** dependenciesKind)
{
    return fmi3_xml_get_dependencies(&(ms->clockedStates), ms->clockedStateDeps, variable,
            numDependencies, dependsOnAll, dependencies, dependenciesKind);
}

int fmi3_xml_get_initial_unknown_dependencies(fmi3_xml_model_structure_t* ms, fmi3_xml_variable_t* variable,
        size_t* numDependencies, int* dependsOnAll, size_t** dependencies, char** dependenciesKind)
{
    return fmi3_xml_get_dependencies(&(ms->initialUnknowns), ms->initialUnknownDeps, variable,
            numDependencies, dependsOnAll, dependencies, dependenciesKind);
}

int fmi3_xml_get_event_indicator_dependencies(fmi3_xml_model_structure_t* ms, fmi3_xml_variable_t* variable,
        size_t* numDependencies, int* dependsOnAll, size_t** dependencies, char** dependenciesKind)
{
    return fmi3_xml_get_dependencies(&(ms->eventIndicators), ms->eventIndicatorDeps, variable,
            numDependencies, dependsOnAll, dependencies, dependenciesKind);
}

fmi3_xml_dependencies_t* fmi3_xml_allocate_dependencies(jm_callbacks* cb) {
    fmi3_xml_dependencies_t* dep = (fmi3_xml_dependencies_t*)(cb->malloc(sizeof(fmi3_xml_dependencies_t)));
    if (!dep) return NULL;
    jm_vector_init(size_t)(&dep->startIndex, 0, cb);
    jm_vector_push_back(size_t)(&dep->startIndex, 0);

    jm_vector_init(size_t)(&dep->dependenciesVRs, 0, cb);
    jm_vector_init(char)(&dep->dependenciesKind, 0, cb);
    jm_vector_init(char)(&dep->dependencyOnAll, 0, cb);

    return dep;
}

void fmi3_xml_free_dependencies(fmi3_xml_dependencies_t* dep) {
    jm_callbacks* cb;
    if (!dep) return;
    cb = dep->startIndex.callbacks;
    jm_vector_free_data(size_t)(&dep->startIndex);

    jm_vector_free_data(size_t)(&dep->dependenciesVRs);
    jm_vector_free_data(char)(&dep->dependenciesKind);
    jm_vector_free_data(char)(&dep->dependencyOnAll);
    cb->free(dep);
}

int fmi3_xml_handle_ModelStructure(fmi3_xml_parser_context_t* context, const char* data) {
    fmi3_xml_model_description_t* md = context->modelDescription;
    if (!data) {
        jm_log_verbose(context->callbacks, module, "Parsing XML element ModelStructure");
        /** allocate model structure */
        md->modelStructure = fmi3_xml_allocate_model_structure(md->callbacks);
        if (!md->modelStructure) {
            fmi3_xml_parse_fatal(context, module, "Could not allocate memory");
            return -1;
        }
    } else {
        fmi3_xml_model_structure_t* ms = md->modelStructure;
        /** make sure model structure information is consistent */

        // TODO: Check that all ModelVariables with causality="output" are Outputs
        if (!ms->isValid) {
            fmi3_xml_parse_fatal(context, "Model structure is not valid due to detected errors. Cannot continue.");
            return -1;
        }
    }
    return 0;
}

/**
 * Parse the dependencies of an element in ModelStructure
 * Any errors when parsings will result in default dependencies = depends on everyhing
 */
static int fmi3_xml_parse_dependencies(fmi3_xml_parser_context_t* context,
        fmi3_xml_elm_t elmID, fmi3_xml_dependencies_t* deps,
        size_t* numDepInd, size_t* numDepKind)
{
    /**
     * Failure to parse results in default dependencies.
     * However, a number of 'dependencies' or 'dependenciesKind' may have already been parsed,
     * and added to the corresponding vectors.
     * These need to be removed again, hence numDepInd and numDepKind are outputs
     * Note: This approach incurs no extra cost for valid parsing.
    */
    const char* listInd;
    const char* listKind;
    *numDepInd = 0;
    *numDepKind = 0;
    size_t totNumDep = jm_vector_get_size(size_t)(&deps->dependenciesVRs);

    /*  <xs:attribute name="dependencies">
            <xs:simpleType>
                <xs:list itemType="xs:unsignedInt"/>
            </xs:simpleType>
        </xs:attribute> */
    if (fmi3_xml_get_attr_str(context, elmID, FMI3_ATTR(fmi_attr_id_dependencies), 0, &listInd)) {
        return -1;
    }
    if (listInd) {
        const char* cur = listInd;
        long long ind; // at least twice the size of unsigned int32
        while (*cur) {
            char ch = *cur;
            while ((ch ==' ') || (ch == '\t') || (ch =='\n') || (ch == '\r')) {
                cur++; ch = *cur;
                if (!ch) break;
            }
            if (!ch) break;
            if (sscanf(cur, "%lld", &ind) != 1) {
                fmi3_xml_parse_error(context, "XML element '%s': could not parse item %d, character '%c' in the list for attribute 'dependencies'",
                    fmi3_xml_elmid_to_name(context, elmID), *numDepInd, ch);
                return -1;
            }
            if (ind < 0) {
                fmi3_xml_parse_error(context, "XML element '%s': Attribute 'dependencies' contains invalid value: %lld.", 
                    fmi3_xml_elmid_to_name(context, elmID), ind);
                return -1;
            }
            if (!jm_vector_push_back(size_t)(&deps->dependenciesVRs, (size_t)ind)) {
                fmi3_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            while ((*cur >= '0') && (*cur <= '9')) cur++;
            (*numDepInd)++;
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
    if (fmi3_xml_get_attr_str(context, elmID, FMI3_ATTR(fmi_attr_id_dependenciesKind), 0, &listKind)) {
        return -1;
    }
    if (listKind) {
        const char* cur = listKind;
        char kind;
        while (*cur) {
            char ch = *cur;
            while (ch && ((ch ==' ') || (ch == '\t') || (ch =='\n') || (ch == '\r'))) {
                cur++; ch = *cur;
            }
            if (!ch) break;
            if (strncmp("dependent", cur, 9) == 0) {
                kind = fmi3_dependencies_kind_dependent;
                cur+=9;
            }
            else if (strncmp("constant", cur, 8) == 0) {
                kind = fmi3_dependencies_kind_constant;
                cur+=8;
            }
            else if (strncmp("fixed", cur, 5) == 0) {
                kind = fmi3_dependencies_kind_fixed;
                cur+=5;
            }
            else if (strncmp("tunable", cur, 7) == 0) {
                kind = fmi3_dependencies_kind_tunable;
                cur+=7;
            }
            else if (strncmp("discrete", cur, 8) == 0) {
                kind = fmi3_dependencies_kind_discrete;
                cur+=8;
            }
            else {
                fmi3_xml_parse_error(context, "XML element '%s': could not parse item %d in the list for attribute 'dependenciesKind'",
                    fmi3_xml_elmid_to_name(context, elmID), *numDepKind);
                return -1;
            }
            // Check possible invalid combinations of elmID & dependenciesKind entries
            if ((elmID.modelDescription == fmi3_xml_elmID_InitialUnknown) && 
                ((kind == fmi3_dependencies_kind_fixed) || (kind == fmi3_dependencies_kind_tunable) || (kind == fmi3_dependencies_kind_discrete))) 
            {
                fmi3_xml_parse_warning(context, "XML element 'InitialUnknown': '%s' is not allowed in list for attribute 'dependenciesKind'.",
                        fmi3_dependencies_kind_to_string(kind));
            }
            if (!jm_vector_push_back(char)(&deps->dependenciesKind, kind)) {
                fmi3_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            (*numDepKind)++;
        }
    }
    if (listInd && listKind) {
        /* both lists are present - the number of items must match */
        if (*numDepInd != *numDepKind) {
            fmi3_xml_parse_error(context, "XML element '%s': different number of items (%u and %u) in the lists for 'dependencies' and 'dependenciesKind'",
                    fmi3_xml_elmid_to_name(context, elmID), *numDepInd, *numDepKind);
            return -1;
        }
        jm_vector_push_back(size_t)(&deps->startIndex, totNumDep + *numDepInd);
        jm_vector_push_back(char)(&deps->dependencyOnAll, 0);
    }
    else if (listInd) {
        /* only Dependencies are present, set all kinds to dependent */
        /* this includes the case of empty dependencies */
        char kind = fmi3_dependencies_kind_dependent;
        if (jm_vector_reserve(char)(&deps->dependenciesKind, totNumDep + *numDepInd) < totNumDep + *numDepInd) {
            fmi3_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }
        for (size_t k = 0; k < *numDepInd; k++) {
            jm_vector_push_back(char)(&deps->dependenciesKind, kind);
        }
        jm_vector_push_back(size_t)(&deps->startIndex, totNumDep + *numDepInd);
        jm_vector_push_back(char)(&deps->dependencyOnAll, 0); // not dependent on all
    }
    else if (listKind) {
        fmi3_xml_parse_error(context, "XML element '%s': if `dependenciesKind` attribute is present then the `dependencies` attribute must also be present.",
            fmi3_xml_elmid_to_name(context, elmID));
        return -1;
    } else {
        /* Dependencies attribute is missing, set default dependencies */
        jm_vector_push_back(size_t)(&deps->startIndex, totNumDep);
        jm_vector_push_back(char)(&deps->dependencyOnAll, 1);
    }
    return 0;
}

/* Wrapper for #fmi3_xml_parse_dependencies, handling failed states */
static void fmi3_xml_parse_dependencies_error_wrapper(fmi3_xml_parser_context_t* context,
        fmi3_xml_elm_t elmID, fmi3_xml_dependencies_t* deps) {
    // number of added dependecies(Kind) entries, these are needed to clean dependencies in case of failure to parse
    size_t numDepInd = 0;
    size_t numDepKind = 0;
    size_t totNumDep = jm_vector_get_size(size_t)(&deps->dependenciesVRs); // original size, before parsing dependencies

    if (fmi3_xml_parse_dependencies(context, elmID, deps, &numDepInd, &numDepKind)) {
        // Dependecy parsing failed, cleanup of already parsed entries
        for (size_t k = 0; k < numDepInd; k++) {
            jm_vector_remove_item(size_t)(&deps->dependenciesVRs, totNumDep);
        }
        for (size_t k = 0; k < numDepKind; k++) {
            jm_vector_remove_item(char)(&deps->dependenciesKind, totNumDep);
        }

        // Fallback to default dependency (still requires some data entries)
        // API #fmi3_xml_get_dependencies requires consistent dependency data to retreive
        jm_vector_push_back(size_t)(&deps->startIndex, totNumDep);
        jm_vector_push_back(char)(&deps->dependencyOnAll, 1);
    }
}

/**
 * Parses an fmi3Unknown element(abstract parent to elements in ModelStructure)
 * After successful call, the variable will be put on the destVarList stack,
 * and the dependencies are stored in return-arg 'deps'.
 */
int fmi3_xml_parse_unknown(fmi3_xml_parser_context_t* context,
                           fmi3_xml_elm_t elmID,
                           jm_vector(jm_voidp) *destVarList,
                           fmi3_xml_dependencies_t* deps)
{
    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_model_structure_t* ms = md->modelStructure;

    fmi3_value_reference_t vr;
    fmi3_xml_variable_t* variable;

    if (fmi3_xml_parse_attr_as_uint32(context, elmID, FMI3_ATTR(fmi_attr_id_valueReference), 1, &vr, 0)){
        fmi3_xml_set_model_structure_invalid(ms);
        return -1;
    }

    variable = fmi3_xml_get_variable_by_vr(md, vr);
    if (!variable) {
        fmi3_xml_parse_error(context, "Failed to find variable for valueReference=%" PRId32 ".", vr);
        fmi3_xml_set_model_structure_invalid(ms);
        return -1;
    }
    if (!jm_vector_push_back(jm_voidp)(destVarList, variable)) {
        fmi3_xml_parse_fatal(context, "Could not allocate memory");
        fmi3_xml_set_model_structure_invalid(ms);
        return -1;
    }

    fmi3_xml_parse_dependencies_error_wrapper(context, elmID, deps); // handle possible parsing errors
    return 0;
}

int fmi3_xml_handle_Output(fmi3_xml_parser_context_t* context, const char* data) {
    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_model_structure_t* ms = md->modelStructure;
    if (!data) {
        if (fmi3_xml_parse_unknown(context, FMI3_ELM(fmi3_xml_elmID_Output), &ms->outputs, ms->outputDeps)) {
            return -1;
        }
    } else {
        // post-processing

        // Check for correct causality
        fmi3_xml_variable_t* var = (fmi3_xml_variable_t*)jm_vector_get_last(jm_voidp)(&ms->outputs);
        if (var->causality != fmi3_causality_enu_output) {
            fmi3_xml_parse_warning(context,
                    "The variable '%s' is an Output, but does not have causality='output'.",
                    fmi3_xml_get_variable_name(var));
        }
    }
    return 0;
}

int fmi3_xml_handle_ContinuousStateDerivative(fmi3_xml_parser_context_t* context, const char* data) {
    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_model_structure_t* ms = md->modelStructure;
    if (!data) {
        /* perform the parsing */
        if (fmi3_xml_parse_unknown(context, FMI3_ELM(fmi3_xml_elmID_ContinuousStateDerivative),
                                   &ms->continuousStateDerivatives, ms->continuousStateDerivativeDeps)) {
            return -1;
        }
    } else {
        // post-processing
        fmi3_xml_variable_t* derXX; /* float64 or float32 variable */
        int validDeriv; /* valid derivative found */
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
            fmi3_xml_parse_warning(context,
                    "The variable '%s' is a ContinuousStateDerivative, but does not specify the state variable it is a derivative of.",
                    fmi3_xml_get_variable_name(derXX));
        }
    }
    return 0;
}

int fmi3_xml_handle_ClockedState(fmi3_xml_parser_context_t* context, const char* data) {
    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_model_structure_t* ms = md->modelStructure;
    if (!data) {
        if (fmi3_xml_parse_unknown(context, FMI3_ELM(fmi3_xml_elmID_ClockedState),
                                   &ms->clockedStates, ms->clockedStateDeps)) {
            return -1;
        }
    } else {
        // post-processing
        fmi3_xml_variable_t* clockVar = (fmi3_xml_variable_t*)jm_vector_get_last(jm_voidp)(&ms->clockedStates);

        // previous attribute is required, spec: "If present, this variable is a <ClockedState>" */
        if (!clockVar->hasPrevious) {
            fmi3_xml_parse_warning(context,
                    "The variable '%s' is a ClockedState, but does not define the attribute 'previous'.",
                    fmi3_xml_get_variable_name(clockVar));
        }

        // ClockedStates must also have variability='discrete', and the attribute 'clocks'.
        // The same applies for variables with the attribute 'previous'.
        // ClockedStates require the attribute 'previous', so this is already checked.

        /* must not be of base type fmi3Clock */
        if (fmi3_xml_get_variable_base_type(clockVar) == fmi3_base_type_clock) {
            fmi3_xml_parse_warning(context,
                    "The variable '%s' is a ClockedState, but has the base type 'fmi3Clock'.",
                    fmi3_xml_get_variable_name(clockVar));
        }
    }
    return 0;
}

int fmi3_xml_handle_InitialUnknown(fmi3_xml_parser_context_t* context, const char* data) {
    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_model_structure_t* ms = md->modelStructure;
    if (!data) {
        if (fmi3_xml_parse_unknown(context, FMI3_ELM(fmi3_xml_elmID_InitialUnknown), &ms->initialUnknowns, ms->initialUnknownDeps)) {
            return -1;
        }
    } else {
        // post-processing
        ;
    }
    return 0;
}

int fmi3_xml_handle_EventIndicator(fmi3_xml_parser_context_t* context, const char* data) {
    fmi3_xml_model_description_t* md = context->modelDescription;
    fmi3_xml_model_structure_t* ms = md->modelStructure;

    // Ignored if Co-simulation & scheduled execution
    fmi3_fmu_kind_enu_t fmuKind = fmi3_xml_get_fmu_kind(md);
    // Multiple types can be defined, check for: not ME and (CS or SE)
    if (!(fmuKind & fmi3_fmu_kind_me) && ((fmuKind & fmi3_fmu_kind_cs) || (fmuKind & fmi3_fmu_kind_se))) {
        if (!data) {
            // Only show log message once, upon start of <EventIndicator>
            jm_log_info(md->callbacks, "FMI3XML", "EventIndicator ignored since FMU kind is Co-Simulation or Scheduled Excecution.");
        }
        return 0;
    }

    if (!data) {
        if (fmi3_xml_parse_unknown(context, FMI3_ELM(fmi3_xml_elmID_EventIndicator),
                                   &ms->eventIndicators, ms->eventIndicatorDeps)) {
            return -1;
        }
    } else {
        // post-processing
        fmi3_xml_variable_t* eventInd = (fmi3_xml_variable_t*)jm_vector_get_last(jm_voidp)(&ms->eventIndicators);
        /* EventIndicator must be continuous */
        if (fmi3_xml_get_variable_variability(eventInd) != fmi3_variability_enu_continuous) {
            fmi3_xml_parse_warning(context,
                    "The variable '%s' is an EventIndicator, but does not have variability='continuous'.",
                    fmi3_xml_get_variable_name(eventInd));
        }

        fmi3_base_type_enu_t baseType = fmi3_xml_get_variable_base_type(eventInd);
        if ((baseType != fmi3_base_type_float64) && (baseType != fmi3_base_type_float32)) {
            fmi3_xml_parse_warning(context,
                    "The variable '%s' is an EventIndicator, but does not have the base type 'Float32' or 'Float64'.",
                    fmi3_xml_get_variable_name(eventInd));
        }
    }
    return 0;
}
