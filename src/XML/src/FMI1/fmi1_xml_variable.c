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

#include <string.h>
#include <stdio.h>

#include <JM/jm_vector.h>

#include "fmi1_xml_parser.h"
#include "fmi1_xml_type_impl.h"
#include "fmi1_xml_model_description_impl.h"

#include "fmi1_xml_variable_impl.h"

static const char* module = "FMI1XML";

const char* fmi1_xml_get_variable_name(fmi1_xml_variable_t* v) {
    return v->name;
}

const char* fmi1_xml_get_variable_description(fmi1_xml_variable_t* v) {
    return v->description;
}

size_t fmi1_xml_get_variable_original_order(fmi1_xml_variable_t* v) {
    return v->originalIndex;
}

fmi1_value_reference_t fmi1_xml_get_variable_vr(fmi1_xml_variable_t* v) {
    return v->vr;
}

fmi1_variable_alias_kind_enu_t fmi1_xml_get_variable_alias_kind(fmi1_xml_variable_t* v) {
    return (fmi1_variable_alias_kind_enu_t)v->aliasKind;
}

fmi1_xml_variable_t* fmi1_xml_get_variable_alias_base(fmi1_xml_model_description_t* md, fmi1_xml_variable_t* v) {
    fmi1_xml_variable_t key;
    fmi1_xml_variable_t *pkey = &key, *base;
    void ** found;
    if(!md->variablesByVR) return 0;
    if(v->aliasKind == fmi1_variable_is_not_alias) return v;
    key = *v;
    key.aliasKind = fmi1_variable_is_not_alias;

    found = jm_vector_bsearch(jm_voidp)(md->variablesByVR,(void**)&pkey, fmi1_xml_compare_vr);
    assert(found);
    base = *found;
    return base;
}

/*
    Return the list of all the variables aliased to the given one (including the base one.
    The list is ordered: base variable, aliases, negated aliases.
*/
jm_status_enu_t fmi1_xml_get_variable_aliases(fmi1_xml_model_description_t* md,fmi1_xml_variable_t* v, jm_vector(jm_voidp)* list) {
    fmi1_xml_variable_t key, *cur;
    fmi1_value_reference_t vr = fmi1_xml_get_variable_vr(v);
    size_t baseIndex, i, num = jm_vector_get_size(jm_voidp)(md->variablesByVR);
    key = *v;
    key.aliasKind = 0;
    cur = &key;
    baseIndex = jm_vector_bsearch_index(jm_voidp)(md->variablesByVR,(void**)&cur, fmi1_xml_compare_vr);
    cur = (fmi1_xml_variable_t*)jm_vector_get_item(jm_voidp)(md->variablesByVR, baseIndex);
    assert(cur);
    i = baseIndex + 1;
    while(fmi1_xml_get_variable_vr(cur) == vr) {
        if(!jm_vector_push_back(jm_voidp)(list, cur)) {
            jm_log_fatal(md->callbacks,module,"Could not allocate memory");
            return jm_status_error;
        };
        if(i >= num) break;
        cur = (fmi1_xml_variable_t*)jm_vector_get_item(jm_voidp)(md->variablesByVR, i);
        assert(cur);
        i++;
    }
    if(baseIndex) {
        i = baseIndex - 1;
        cur = (fmi1_xml_variable_t*)jm_vector_get_item(jm_voidp)(md->variablesByVR, i);
        while(fmi1_xml_get_variable_vr(cur) == vr) {
            if(!jm_vector_push_back(jm_voidp)(list, cur)) {
                jm_log_fatal(md->callbacks,module,"Could not allocate memory");
                return jm_status_error;
            };
            i--;
            if(!i) break;
            cur = (fmi1_xml_variable_t*)jm_vector_get_item(jm_voidp)(md->variablesByVR, i - 1);
            assert(cur);
        }
    }
    return jm_status_success;
}


fmi1_xml_variable_typedef_t* fmi1_xml_get_variable_declared_type(fmi1_xml_variable_t* v) {
    return (fmi1_xml_variable_typedef_t*)(fmi1_xml_find_type_struct(v->typeBase, fmi1_xml_type_struct_enu_typedef));
}

fmi1_base_type_enu_t fmi1_xml_get_variable_base_type(fmi1_xml_variable_t* v) {
    fmi1_xml_variable_type_base_t* type = v->typeBase;
    type = fmi1_xml_find_type_struct(type, fmi1_xml_type_struct_enu_base);
    return (type->baseType);
}

int fmi1_xml_get_variable_has_start(fmi1_xml_variable_t* v) {
    return (v->typeBase->structKind == fmi1_xml_type_struct_enu_start);
}

int   fmi1_xml_get_variable_is_fixed(fmi1_xml_variable_t* v) {
    fmi1_xml_variable_type_base_t* type = v->typeBase;
    return ((type->structKind == fmi1_xml_type_struct_enu_start) && (type->isFixed));
}

fmi1_variability_enu_t fmi1_xml_get_variability(fmi1_xml_variable_t* v) {
    return (fmi1_variability_enu_t)v->variability;
}

fmi1_causality_enu_t fmi1_xml_get_causality(fmi1_xml_variable_t* v) {
    return (fmi1_causality_enu_t)v->causality;
}

double fmi1_xml_get_real_variable_start(fmi1_xml_real_variable_t* v) {
    fmi1_xml_variable_t* vv = (fmi1_xml_variable_t*)v;
    if(fmi1_xml_get_variable_has_start(vv)) {
        fmi1_xml_variable_start_real_t* start = (fmi1_xml_variable_start_real_t*)(vv->typeBase);
        return start->start;
    }
        return fmi1_xml_get_real_variable_nominal(v);
}

fmi1_xml_unit_t* fmi1_xml_get_real_variable_unit(fmi1_xml_real_variable_t* v) {
    fmi1_xml_variable_t* vv = (fmi1_xml_variable_t*)v;
    fmi1_xml_real_type_props_t* props = (fmi1_xml_real_type_props_t*)(fmi1_xml_find_type_struct(vv->typeBase, fmi1_xml_type_struct_enu_props));
    if(!props || !props->displayUnit) return 0;
    return props->displayUnit->baseUnit;
}

fmi1_xml_display_unit_t* fmi1_xml_get_real_variable_display_unit(fmi1_xml_real_variable_t* v) {
    fmi1_xml_variable_t* vv = (fmi1_xml_variable_t*)v;
    fmi1_xml_real_type_props_t* props = (fmi1_xml_real_type_props_t*)(fmi1_xml_find_type_struct(vv->typeBase, fmi1_xml_type_struct_enu_props));
    if(!props || !props->displayUnit || !props->displayUnit->displayUnit[0]) return 0;
    return props->displayUnit;
}

double fmi1_xml_get_real_variable_max(fmi1_xml_real_variable_t* v) {
    fmi1_xml_variable_t* vv = (fmi1_xml_variable_t*)v;
    fmi1_xml_real_type_props_t* props = (fmi1_xml_real_type_props_t*)(fmi1_xml_find_type_props(vv->typeBase));
    assert(props);
    return props->typeMax;
}

double fmi1_xml_get_real_variable_min(fmi1_xml_real_variable_t* v) {
    fmi1_xml_variable_t* vv = (fmi1_xml_variable_t*)v;
    fmi1_xml_real_type_props_t* props = (fmi1_xml_real_type_props_t*)(fmi1_xml_find_type_props(vv->typeBase));
    assert(props);
    return props->typeMin;
}

double fmi1_xml_get_real_variable_nominal(fmi1_xml_real_variable_t* v){
    fmi1_xml_variable_t* vv = (fmi1_xml_variable_t*)v;
    fmi1_xml_real_type_props_t* props = (fmi1_xml_real_type_props_t*)(fmi1_xml_find_type_props(vv->typeBase));
    assert(props);
    return props->typeNominal;
}

int fmi1_xml_get_integer_variable_start(fmi1_xml_integer_variable_t* v){
    fmi1_xml_variable_t* vv = (fmi1_xml_variable_t*)v;
    if(fmi1_xml_get_variable_has_start(vv)) {
        fmi1_xml_variable_start_integer_t* start = (fmi1_xml_variable_start_integer_t*)(vv->typeBase);
        return start->start;
    }
        return 0;
}

int fmi1_xml_get_integer_variable_min(fmi1_xml_integer_variable_t* v){
    fmi1_xml_variable_t* vv = (fmi1_xml_variable_t*)v;
    fmi1_xml_integer_type_props_t* props = (fmi1_xml_integer_type_props_t*)(fmi1_xml_find_type_props(vv->typeBase));
    assert(props);
    return props->typeMin;
}

int fmi1_xml_get_integer_variable_max(fmi1_xml_integer_variable_t* v){
    fmi1_xml_variable_t* vv = (fmi1_xml_variable_t*)v;
    fmi1_xml_integer_type_props_t* props = (fmi1_xml_integer_type_props_t*)(fmi1_xml_find_type_props(vv->typeBase));
    assert(props);
    return props->typeMax;
}

int fmi1_xml_get_enum_variable_min(fmi1_xml_enum_variable_t* v){
    fmi1_xml_variable_t* vv = (fmi1_xml_variable_t*)v;
    fmi1_xml_enum_type_props_t* props = (fmi1_xml_enum_type_props_t*)(fmi1_xml_find_type_props(vv->typeBase));
    assert(props);
    return props->typeMin;
}

int fmi1_xml_get_enum_variable_max(fmi1_xml_enum_variable_t* v){
    fmi1_xml_variable_t* vv = (fmi1_xml_variable_t*)v;
    fmi1_xml_enum_type_props_t* props = (fmi1_xml_enum_type_props_t*)(fmi1_xml_find_type_props(vv->typeBase));
    assert(props);
    return props->typeMax;
}

const char* fmi1_xml_get_string_variable_start(fmi1_xml_string_variable_t* v){
    fmi1_xml_variable_t* vv = (fmi1_xml_variable_t*)v;
    if(fmi1_xml_get_variable_has_start(vv)) {
        fmi1_xml_variable_start_string_t* start = (fmi1_xml_variable_start_string_t*)(vv->typeBase);
        return start->start;
    }
    return 0;
}

int fmi1_xml_get_enum_variable_start(fmi1_xml_enum_variable_t* v) {
    fmi1_xml_variable_t* vv = (fmi1_xml_variable_t*)v;
    if(fmi1_xml_get_variable_has_start(vv)) {
        fmi1_xml_variable_start_integer_t* start = (fmi1_xml_variable_start_integer_t*)(vv->typeBase);
        return start->start;
    }
        return 0;
}

fmi1_boolean_t fmi1_xml_get_boolean_variable_start(fmi1_xml_bool_variable_t* v) {
    fmi1_xml_variable_t* vv = (fmi1_xml_variable_t*)v;
    if(fmi1_xml_get_variable_has_start(vv)) {
        fmi1_xml_variable_start_integer_t* start = (fmi1_xml_variable_start_integer_t*)(vv->typeBase);
        return start->start;
    }
        return 0;
}

size_t fmi1_xml_get_direct_dependency_size(fmi1_xml_model_description_t* md,fmi1_xml_variable_t*v) {
    if(v->directDependency) {
        return jm_vector_get_size(jm_voidp)(v->directDependency);
    }
    else{
        return jm_vector_get_size(jm_voidp)(md->inputVariables);
    }
}

/* DirectDependency is returned for variables with causality Output. Null pointer for others. */
jm_status_enu_t fmi1_xml_get_direct_dependency(fmi1_xml_model_description_t* md, fmi1_xml_variable_t* v, jm_vector(jm_voidp)* list) {
    size_t size = 0;
    if(fmi1_xml_get_causality(v) != fmi1_causality_enu_output) return jm_status_error;
    jm_vector_resize(jm_voidp)(list, 0);
    if(v->directDependency) {
        size = jm_vector_get_size(jm_voidp)(v->directDependency);
        if(jm_vector_reserve(jm_voidp)(list, size) < size) return jm_status_error;
        jm_vector_copy(jm_voidp)(list,v->directDependency);
    }
    /*no direct dependency defined, deliver all inputs, see FMI for ME spec. p 38.*/
    else{
        if (md->inputVariables){
            jm_vector_copy(jm_voidp)(list,md->inputVariables);
        }
        else {
            jm_log_error(md->callbacks,module, "List of input variables not found.");
        }
    }
    return jm_status_success;
}

fmi1_xml_real_variable_t* fmi1_xml_get_variable_as_real(fmi1_xml_variable_t* v) {
    if(fmi1_xml_get_variable_base_type(v) == fmi1_base_type_real)  return (void*)v;
    return 0;
}

fmi1_xml_integer_variable_t* fmi1_xml_get_variable_as_integer(fmi1_xml_variable_t*v){
    if(fmi1_xml_get_variable_base_type(v) == fmi1_base_type_int)  return (void*)v;
    return 0;
}
fmi1_xml_enum_variable_t* fmi1_xml_get_variable_as_enum(fmi1_xml_variable_t* v){
    if(fmi1_xml_get_variable_base_type(v) == fmi1_base_type_enum)  return (void*)v;
    return 0;
}
fmi1_xml_string_variable_t* fmi1_xml_get_variable_as_string(fmi1_xml_variable_t* v){
    if(fmi1_xml_get_variable_base_type(v) == fmi1_base_type_str)  return (void*)v;
    return 0;
}
fmi1_xml_bool_variable_t* fmi1_xml_get_variable_as_boolean(fmi1_xml_variable_t* v){
    if(fmi1_xml_get_variable_base_type(v) == fmi1_base_type_bool)  return (void*)v;
    return 0;
}

void fmi1_xml_free_direct_dependencies(jm_named_ptr named) {
        fmi1_xml_variable_t* v = named.ptr;
        if(v->directDependency) {
                jm_vector_free(jm_voidp)(v->directDependency);
                v->directDependency = 0;
        }
}

int fmi1_xml_handle_ScalarVariable(fmi1_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi1_xml_model_description_t* md = context->modelDescription;
        fmi1_xml_variable_t* variable;
        fmi1_xml_variable_t dummyV;
        const char* description = 0;
        jm_named_ptr named, *pnamed;
        jm_vector(char)* bufName = fmi1_xml_reserve_parse_buffer(context,1,100);
        jm_vector(char)* bufDescr = fmi1_xml_reserve_parse_buffer(context,2,100);
        unsigned int vr;

        if(!bufName || !bufDescr) return -1;

        /*   <xs:attribute name="valueReference" type="xs:unsignedInt" use="required"> */
        if(fmi1_xml_set_attr_uint(context, fmi1_xml_elmID_ScalarVariable, fmi_attr_id_valueReference, 1, &vr, 0)) return -1;

        if(vr == fmi1_undefined_value_reference) {
            context->skipOneVariableFlag = 1;
        }

        if(
        /*  <xs:attribute name="name" type="xs:normalizedString" use="required"/> */
            fmi1_xml_set_attr_string(context, fmi1_xml_elmID_ScalarVariable, fmi_attr_id_name, 1, bufName) ||
        /* <xs:attribute name="description" type="xs:string"/> */
            fmi1_xml_set_attr_string(context, fmi1_xml_elmID_ScalarVariable, fmi_attr_id_description, 0, bufDescr)
        ) return -1;

        if(context->skipOneVariableFlag) {
            jm_log_error(context->callbacks,module, "Ignoring variable with undefined vr '%s'", jm_vector_get_itemp(char)(bufName,0));
            return 0;
        }
        if(jm_vector_get_size(char)(bufDescr)) {
            description = jm_string_set_put(&md->descriptions, jm_vector_get_itemp(char)(bufDescr,0));
        }

        named.ptr = 0;
        named.name = 0;
        pnamed = jm_vector_push_back(jm_named_ptr)(&md->variablesByName, named);

        if(pnamed) *pnamed = named = jm_named_alloc_v(bufName,sizeof(fmi1_xml_variable_t), dummyV.name - (char*)&dummyV, context->callbacks);
        variable = named.ptr;
        if( !pnamed || !variable ) {
            fmi1_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }
        variable->vr = vr;
        variable->description = description;
        variable->typeBase = 0;
        variable->directDependency = 0;
        variable->originalIndex = jm_vector_get_size(jm_named_ptr)(&md->variablesByName) - 1;

        {
            jm_name_ID_map_t variabilityConventionMap[] = {{"continuous",fmi1_variability_enu_continuous},
                                                            {"constant", fmi1_variability_enu_constant},
                                                            {"parameter", fmi1_variability_enu_parameter},
                                                            {"discrete", fmi1_variability_enu_discrete},{0,0}};
            unsigned int variability;
            /*  <xs:attribute name="variability" default="continuous"> */
            if(fmi1_xml_set_attr_enum(context, fmi1_xml_elmID_ScalarVariable, fmi_attr_id_variability,0,&variability,fmi1_variability_enu_continuous,variabilityConventionMap))
                return -1;
            variable->variability = variability;
        }
        {
            jm_name_ID_map_t causalityConventionMap[] = {{"internal",fmi1_causality_enu_internal},
                                                         {"input",fmi1_causality_enu_input},
                                                         {"output",fmi1_causality_enu_output},
                                                         {"none",fmi1_causality_enu_none},{0,0}};
            /* <xs:attribute name="causality" default="internal"> */
            unsigned int causality;
            if(fmi1_xml_set_attr_enum(context, fmi1_xml_elmID_ScalarVariable, fmi_attr_id_causality,0,&causality,fmi1_causality_enu_internal,causalityConventionMap))
                return -1;
            variable->causality = causality;
        }
        {
            jm_name_ID_map_t aliasConventionMap[] = {{"alias", 1},
                                                     {"negatedAlias", 2},
                                                     {"noAlias", 0}, {0,0}};
            unsigned int alias;
            /* <xs:attribute name="alias" default="noAlias"> */
            if(fmi1_xml_set_attr_enum(context, fmi1_xml_elmID_ScalarVariable, fmi_attr_id_alias ,0,&alias,0,aliasConventionMap))
                return -1;
            if(alias == 0) variable->aliasKind = fmi1_variable_is_not_alias;
            else if (alias == 1) variable->aliasKind = fmi1_variable_is_alias;
            else if (alias == 2) variable->aliasKind = fmi1_variable_is_negated_alias;
            else assert(0);
        }
    } else {
        if(context->skipOneVariableFlag) {
            context->skipOneVariableFlag = 0;
        }
        else {
            /* check that the type for the variable is set */
            fmi1_xml_model_description_t* md = context->modelDescription;
            fmi1_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
            if(!variable->typeBase) {
                jm_log_error(context->callbacks, module, "No variable type element for variable %s. Assuming Real.", variable->name);

                return fmi1_xml_handle_Real(context, NULL);
            }
        }
        /* might give out a warning if(data[0] != 0) */
    }
    return 0;
}


int fmi1_xml_handle_DirectDependency(fmi1_xml_parser_context_t *context, const char* data) {
    if(context->skipOneVariableFlag) return 0;
    if(!data) {
        fmi1_xml_model_description_t* md = context->modelDescription;
        fmi1_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
        if(variable->causality != fmi1_causality_enu_output) {
            jm_log_error(context->callbacks,module,
                "DirectDependency XML element cannot be defined for '%s' since causality is not output. Skipping.", variable->name);
            context->skipElementCnt = 1;
            return 0;
        }
    }
    else {
        fmi1_xml_model_description_t* md = context->modelDescription;
        fmi1_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
        /*
        - always create the list to be able to differentiate no depedendencies from all dependencies (no DirectDependency element present)
        if(jm_vector_get_size(jm_voidp)(&context->directDependencyBuf))
        */
        {
            variable->directDependency = jm_vector_clone(jm_voidp)(&context->directDependencyBuf);
            if(!variable->directDependency) {
                fmi1_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
        }
        jm_vector_resize(jm_voidp)(&context->directDependencyBuf,0);
    }
    return 0;
}

int fmi1_xml_handle_Name(fmi1_xml_parser_context_t *context, const char* data) {
    if(context->skipOneVariableFlag) return 0;

    if(!data) {
        return 0;
    }
    else {
        fmi1_xml_model_description_t* md = context->modelDescription;
        fmi1_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
        size_t namelen = strlen(data), i = 0, j;
        char* name = 0;
        jm_voidp* itemp;
        jm_string* namep;
#define TRIM_SPACE " \n\r\t"
        if(namelen) {
            while(strchr(TRIM_SPACE, data[i])) i++;
            while(strchr(TRIM_SPACE, data[namelen-1])) namelen--;
        }
        if(i>=namelen) {
            jm_log_error(context->callbacks, module,
                "Unexpected empty Name element for DirectDependency of variable %s. Ignoring.", variable->name);
            return 0;
        }
        namep = jm_vector_push_back(jm_string)(&context->directDependencyStringsStore, name);
        if(namep) *namep = name = context->callbacks->malloc(namelen + 1);
        itemp = jm_vector_push_back(jm_voidp)(&context->directDependencyBuf, name);
        if(!namep || !itemp || !name)  {
            fmi1_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }
        for(j = 0; i<namelen;i++) {
            name[j++] = data[i];
        }
        name[j] = 0;
    }
    return 0;
}

static void fmi1_log_error_if_start_required(
    fmi1_xml_parser_context_t *context,
    fmi1_xml_variable_t *variable)
{
    if (fmi1_xml_is_attr_defined(context, fmi_attr_id_fixed)) {
        jm_log_error(context->callbacks, module,
                       "Error: variable %s: 'fixed' attributed is only allowed when start is defined",
                       variable->name);

    } else if (variable->causality == fmi1_causality_enu_input) {
        jm_log_error(context->callbacks, module,
                       "Error: variable %s: start value required for input variables",
                       variable->name);
    }
}

int fmi1_xml_handle_Real(fmi1_xml_parser_context_t *context, const char* data) {
    fmi1_xml_model_description_t* md = context->modelDescription;
    fmi1_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
    fmi1_xml_type_definitions_t* td = &md->typeDefinitions;
    fmi1_xml_variable_type_base_t * declaredType = 0;
    fmi1_xml_real_type_props_t * type = 0;
    int hasStart;

    if(context->skipOneVariableFlag){
        return 0;
    } else if(data) {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    assert(!variable->typeBase);

    declaredType = fmi1_get_declared_type(context, fmi1_xml_elmID_Real, &td->defaultRealType.typeBase);

    if(!declaredType) return -1;

    {
        int hasUnit = fmi1_xml_is_attr_defined(context, fmi_attr_id_unit) ||
                fmi1_xml_is_attr_defined(context, fmi_attr_id_displayUnit);
        int hasMin =  fmi1_xml_is_attr_defined(context, fmi_attr_id_min);
        int hasMax = fmi1_xml_is_attr_defined(context, fmi_attr_id_max);
        int hasNom = fmi1_xml_is_attr_defined(context, fmi_attr_id_nominal);
        int hasQuan = fmi1_xml_is_attr_defined(context, fmi_attr_id_quantity);
        int hasRelQ = fmi1_xml_is_attr_defined(context, fmi_attr_id_relativeQuantity);


        if(hasUnit || hasMin || hasMax || hasNom || hasQuan || hasRelQ) {
            fmi1_xml_real_type_props_t* props = 0;

            if(declaredType->structKind == fmi1_xml_type_struct_enu_typedef)
                props = (fmi1_xml_real_type_props_t*)(declaredType->baseTypeStruct);
            else
                props = (fmi1_xml_real_type_props_t* )declaredType;

            fmi1_xml_reserve_parse_buffer(context, 1, 0);
            fmi1_xml_reserve_parse_buffer(context, 2, 0);

            type = fmi1_xml_parse_real_type_properties(context, fmi1_xml_elmID_Real);

            if(!type) return -1;
            type->typeBase.baseTypeStruct = declaredType;
            if( !hasUnit) type->displayUnit = props->displayUnit;
            if( !hasMin)  type->typeMin = props->typeMin;
            if( !hasMax) type->typeMax = props->typeMax;
            if( !hasNom) type->typeNominal = props->typeNominal;
            if( !hasQuan) type->quantity = props->quantity;
            if( !hasRelQ) type->typeBase.relativeQuantity = props->typeBase.relativeQuantity;
        }
        else
            type = (fmi1_xml_real_type_props_t*)declaredType;
    }
    variable->typeBase = &type->typeBase;

    hasStart = fmi1_xml_is_attr_defined(context, fmi_attr_id_start);
    if(hasStart) {
        fmi1_xml_variable_start_real_t * start = (fmi1_xml_variable_start_real_t*)fmi1_xml_alloc_variable_type_start(td, &type->typeBase, sizeof(fmi1_xml_variable_start_real_t));
        int isFixedBuf;
        if(!start) {
            fmi1_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }
        if(
            /*  <xs:attribute name="start" type="xs:double"/> */
                fmi1_xml_set_attr_double(context, fmi1_xml_elmID_Real, fmi_attr_id_start, 0, &start->start, 0) ||
            /*  <xs:attribute name="fixed" type="xs:boolean"> */
                fmi1_xml_set_attr_boolean(context, fmi1_xml_elmID_Real, fmi_attr_id_fixed, 0, &(isFixedBuf), 1)
            )
                return -1;
        start->typeBase.isFixed = isFixedBuf;
        variable->typeBase = &start->typeBase;
    } else {
        fmi1_log_error_if_start_required(context, variable);
    }

    return 0;
}

int fmi1_xml_handle_Integer(fmi1_xml_parser_context_t *context, const char* data) {
    fmi1_xml_model_description_t* md = context->modelDescription;
    fmi1_xml_type_definitions_t* td = &md->typeDefinitions;
    fmi1_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
    fmi1_xml_variable_type_base_t * declaredType = 0;
    fmi1_xml_integer_type_props_t * type = 0;
    int hasStart;

    if (context->skipOneVariableFlag) {
        return 0;
    } else if (data) {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }

    declaredType = fmi1_get_declared_type(context, fmi1_xml_elmID_Integer,&td->defaultIntegerType.typeBase) ;

    if(!declaredType) return -1;

    {
        int hasQuan = fmi1_xml_is_attr_defined(context, fmi_attr_id_quantity);
        int hasMin =  fmi1_xml_is_attr_defined(context, fmi_attr_id_min);
        int hasMax = fmi1_xml_is_attr_defined(context, fmi_attr_id_max);

        if(hasQuan ||hasMin || hasMax) {
                fmi1_xml_integer_type_props_t* props = 0;

                if(declaredType->structKind != fmi1_xml_type_struct_enu_typedef)
                    props = (fmi1_xml_integer_type_props_t*)declaredType;
                else
                    props = (fmi1_xml_integer_type_props_t*)(declaredType->baseTypeStruct);
                assert((props->typeBase.structKind == fmi1_xml_type_struct_enu_props) || (props->typeBase.structKind == fmi1_xml_type_struct_enu_base));
                fmi1_xml_reserve_parse_buffer(context, 1, 0);
                fmi1_xml_reserve_parse_buffer(context, 2, 0);
                type = fmi1_xml_parse_integer_type_properties(context, fmi1_xml_elmID_Integer);
                if(!type) return -1;
                type->typeBase.baseTypeStruct = declaredType;
                if(!hasMin) type->typeMin = props->typeMin;
                if(!hasMax) type->typeMax = props->typeMax;
                if(!hasQuan) type->quantity = props->quantity;
        }
        else
            type = (fmi1_xml_integer_type_props_t*)declaredType;
    }
    variable->typeBase = &type->typeBase;

    hasStart = fmi1_xml_is_attr_defined(context,fmi_attr_id_start);
    if(hasStart) {
        fmi1_xml_variable_start_integer_t * start = (fmi1_xml_variable_start_integer_t*)fmi1_xml_alloc_variable_type_start(td, &type->typeBase, sizeof(fmi1_xml_variable_start_integer_t));
        int isFixedBuf;
        if(!start) {
            fmi1_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }

            /*  <xs:attribute name="start" type="xs:integer"/> */
                fmi1_xml_set_attr_int(context, fmi1_xml_elmID_Integer, fmi_attr_id_start, 0, &start->start, 0);
            /*  <xs:attribute name="fixed" type="xs:boolean"> */
                fmi1_xml_set_attr_boolean(context, fmi1_xml_elmID_Integer, fmi_attr_id_fixed, 0, &isFixedBuf, 1);

        start->typeBase.isFixed = isFixedBuf;
        variable->typeBase = &start->typeBase;
    } else {
        fmi1_log_error_if_start_required(context, variable);
    }

    return 0;
}

int fmi1_xml_handle_Boolean(fmi1_xml_parser_context_t *context, const char* data) {
    fmi1_xml_model_description_t* md = context->modelDescription;
    fmi1_xml_type_definitions_t* td = &md->typeDefinitions;
    fmi1_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
    int hasStart;

    if (context->skipOneVariableFlag) {
        return 0;
    } else if (data) {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }

    assert(!variable->typeBase);

    variable->typeBase = fmi1_get_declared_type(context, fmi1_xml_elmID_Boolean, &td->defaultBooleanType) ;

    if(!variable->typeBase) return -1;

    hasStart = fmi1_xml_is_attr_defined(context,fmi_attr_id_start);
    if(hasStart) {
        int isFixedBuf;
        fmi1_xml_variable_start_integer_t * start = (fmi1_xml_variable_start_integer_t*)fmi1_xml_alloc_variable_type_start(td, variable->typeBase, sizeof(fmi1_xml_variable_start_integer_t ));
        if(!start) {
            fmi1_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }
        if(
              /*  <xs:attribute name="start" type="xs:boolean"/> */
                fmi1_xml_set_attr_boolean(context, fmi1_xml_elmID_Boolean, fmi_attr_id_start, 0, (int*)&start->start, 0) ||
            /*  <xs:attribute name="fixed" type="xs:boolean"> */
                fmi1_xml_set_attr_boolean(context, fmi1_xml_elmID_Boolean, fmi_attr_id_fixed, 0, &isFixedBuf, 1)
            )
                return -1;
        start->typeBase.isFixed = isFixedBuf;
        variable->typeBase = &start->typeBase;
    } else {
        fmi1_log_error_if_start_required(context, variable);
    }

    return 0;
}

int fmi1_xml_handle_String(fmi1_xml_parser_context_t *context, const char* data) {
    fmi1_xml_model_description_t* md = context->modelDescription;
    fmi1_xml_type_definitions_t* td = &md->typeDefinitions;
    fmi1_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
    int hasStart;

    if (context->skipOneVariableFlag) {
        return 0;
    } else if (data) {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }

    assert(!variable->typeBase);

    variable->typeBase = fmi1_get_declared_type(context, fmi1_xml_elmID_String,&td->defaultStringType) ;

    if(!variable->typeBase) return -1;

    hasStart = fmi1_xml_is_attr_defined(context,fmi_attr_id_start);
    if(hasStart) {
        jm_vector(char)* bufStartStr = fmi1_xml_reserve_parse_buffer(context,1, 100);
        size_t strlen;
        int isFixed;
        fmi1_xml_variable_start_string_t * start;
        if(
             /*   <xs:attribute name="start" type="xs:string"/> */
                fmi1_xml_set_attr_string(context, fmi1_xml_elmID_String, fmi_attr_id_start, 0, bufStartStr) ||
            /*  <xs:attribute name="fixed" type="xs:boolean"> */
                fmi1_xml_set_attr_boolean(context, fmi1_xml_elmID_Boolean, fmi_attr_id_fixed, 0, &isFixed, 1)
            )
                return -1;
        strlen = jm_vector_get_size_char(bufStartStr);

        start = (fmi1_xml_variable_start_string_t*)fmi1_xml_alloc_variable_type_start(td, variable->typeBase, sizeof(fmi1_xml_variable_start_string_t) + strlen);

        if(!start) {
            fmi1_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }
        if (strlen != 0) { /* No need to memcpy empty strings (gives assetion error) */
            memcpy(start->start, jm_vector_get_itemp_char(bufStartStr,0), strlen);
        }
        start->start[strlen] = 0;
        variable->typeBase = &start->typeBase;
    } else {
        fmi1_log_error_if_start_required(context, variable);
    }

    return 0;
}

int fmi1_xml_handle_Enumeration(fmi1_xml_parser_context_t *context, const char* data) {

    fmi1_xml_model_description_t* md = context->modelDescription;
    fmi1_xml_type_definitions_t* td = &md->typeDefinitions;
    fmi1_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
    fmi1_xml_variable_type_base_t * declaredType = 0;
    fmi1_xml_integer_type_props_t * type = 0;
    int hasStart;

    if (context->skipOneVariableFlag) {
        return 0;
    } else if (data) {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }

    assert(!variable->typeBase);

    declaredType = fmi1_get_declared_type(context, fmi1_xml_elmID_Enumeration,&td->defaultEnumType.typeBase);

    if(!declaredType) return -1;

    {
        int hasQuan = fmi1_xml_is_attr_defined(context, fmi_attr_id_quantity);
        int hasMin =  fmi1_xml_is_attr_defined(context, fmi_attr_id_min);
        int hasMax = fmi1_xml_is_attr_defined(context, fmi_attr_id_max);

        if(hasQuan || hasMin || hasMax) {
            fmi1_xml_integer_type_props_t* props = 0;

            if(declaredType->structKind != fmi1_xml_type_struct_enu_typedef)
                props = (fmi1_xml_integer_type_props_t*)declaredType;
            else
                props = (fmi1_xml_integer_type_props_t*)declaredType->baseTypeStruct;
            assert(props->typeBase.structKind == fmi1_xml_type_struct_enu_props);
            fmi1_xml_reserve_parse_buffer(context, 1, 0);
            fmi1_xml_reserve_parse_buffer(context, 2, 0);
            type = fmi1_xml_parse_integer_type_properties(context, fmi1_xml_elmID_Enumeration);
            if(!type) return -1;
            type->typeBase.baseTypeStruct = declaredType;
            if(!hasMin) type->typeMin = props->typeMin;
            if(!hasMax) type->typeMax = props->typeMax;
            if(!hasQuan) type->quantity = props->quantity;
        }
        else
            type = (fmi1_xml_integer_type_props_t*)declaredType;
    }

    variable->typeBase = &type->typeBase;

    hasStart = fmi1_xml_is_attr_defined(context,fmi_attr_id_start);
    if(hasStart) {
        fmi1_xml_variable_start_integer_t * start = (fmi1_xml_variable_start_integer_t*)fmi1_xml_alloc_variable_type_start(td, &type->typeBase, sizeof(fmi1_xml_variable_start_integer_t ));
        int isFixedBuf;
        if(!start) {
            fmi1_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }
        if(
            /*  <xs:attribute name="start" type="xs:integer"/> */
                fmi1_xml_set_attr_int(context, fmi1_xml_elmID_Enumeration, fmi_attr_id_start, 0, &start->start, 0) ||
            /*  <xs:attribute name="fixed" type="xs:boolean"> */
                fmi1_xml_set_attr_boolean(context, fmi1_xml_elmID_Enumeration, fmi_attr_id_fixed, 0, &isFixedBuf, 1)
            )
                return -1;
        start->typeBase.isFixed = isFixedBuf;
        variable->typeBase = &start->typeBase;
    } else {
        fmi1_log_error_if_start_required(context, variable);
    }
    return 0;
}

static int fmi1_xml_compare_variable_original_index (const void* first, const void* second) {
    size_t a = (*(fmi1_xml_variable_t**)first)->originalIndex;
    size_t b = (*(fmi1_xml_variable_t**)second)->originalIndex;
    if(a < b) return -1;
    if(a > b) return 1;
    return 0;
}

void fmi1_xml_eliminate_bad_alias(fmi1_xml_parser_context_t *context, jm_vector(jm_voidp)* varByVR, size_t indexVR) {
    size_t n, index;

    fmi1_xml_variable_t* v = (fmi1_xml_variable_t*)jm_vector_get_item(jm_voidp)(varByVR, indexVR);
    fmi1_value_reference_t vr = v->vr;
    fmi1_base_type_enu_t vt = fmi1_xml_get_variable_base_type(v);
    jm_named_ptr key;

    n = jm_vector_get_size(jm_voidp)(varByVR);
    v = (fmi1_xml_variable_t*)jm_vector_get_item(jm_voidp)(varByVR, indexVR);

    jm_vector_remove_item_jm_voidp(varByVR, indexVR);
    key.name = v->name;
    index = jm_vector_bsearch_index(jm_named_ptr)(&context->modelDescription->variablesByName, &key, jm_compare_named);
    assert(index <= n);
    jm_vector_remove_item(jm_named_ptr)(&context->modelDescription->variablesByName, index);

    index = jm_vector_bsearch_index(jm_voidp)(context->modelDescription->variablesOrigOrder, (jm_voidp*)&v, fmi1_xml_compare_variable_original_index);
    assert(index <= n);

    jm_vector_remove_item(jm_voidp)(context->modelDescription->variablesOrigOrder,index);

    jm_log_error(context->callbacks, module,"Removing incorrect alias variable '%s'", v->name);
    context->callbacks->free(v);
}

static size_t fmi1_xml_eliminate_bad_alias_set(fmi1_xml_parser_context_t *context, size_t indexVR) {
    size_t i, n, removed_aliases;

    jm_vector(jm_voidp)* varByVR = context->modelDescription->variablesByVR;
    fmi1_xml_variable_t* v = (fmi1_xml_variable_t*)jm_vector_get_item(jm_voidp)(varByVR, indexVR);
    fmi1_value_reference_t vr = v->vr;
    fmi1_base_type_enu_t vt = fmi1_xml_get_variable_base_type(v);

    jm_log_error(context->callbacks, module, "Alias set with vr=%u "
                "(type=%s) do not have a 'noAlias' variable.", v->vr,
                fmi1_base_type_to_string(fmi1_xml_get_variable_base_type(v)));

    removed_aliases = 0;
    n = jm_vector_get_size(jm_voidp)(varByVR);
    for(i = 0; i< n; i++) {
        v = (fmi1_xml_variable_t*)jm_vector_get_item(jm_voidp)(varByVR, i);
        if ((v->vr == vr) && (vt == fmi1_xml_get_variable_base_type(v))) {
            fmi1_xml_eliminate_bad_alias(context, varByVR, i);
            n--; i--; removed_aliases++;
        }
    }

    return removed_aliases;
}

static int fmi1_xml_compare_vr_and_original_index (const void* first, const void* second) {
    int ret = fmi1_xml_compare_vr(first, second);
    if(ret == 0) {
        fmi1_xml_variable_t* a = *(fmi1_xml_variable_t**)first;
        fmi1_xml_variable_t* b = *(fmi1_xml_variable_t**)second;
        size_t ai = a->originalIndex;
        size_t bi = b->originalIndex;
        if(ai > bi) ret = 1;
        if(ai < bi) ret = -1;
    }

    return ret;
}

static int fmi1_same_vr_and_base_type(
    fmi1_xml_variable_t* a,
    fmi1_xml_variable_t* b)
{
    return (a->vr == b->vr) &&
        (fmi1_xml_get_variable_base_type(a) == fmi1_xml_get_variable_base_type(b));
}

static fmi1_xml_variable_t* findNextBaseAliasIdx(
    fmi1_xml_parser_context_t *context,
    jm_vector(jm_voidp)* varByVR,
    size_t cur_list_idx)
{
    size_t n, i;
    fmi1_xml_variable_t* a;

    n = jm_vector_get_size(jm_voidp)(varByVR);
    a = (fmi1_xml_variable_t*)jm_vector_get_item(jm_voidp)(varByVR, cur_list_idx);
    if (a->aliasKind == fmi1_variable_is_not_alias) {
        return a;
    }

    for (i = cur_list_idx + 1; i < n; i++) {
        fmi1_xml_variable_t* b =
            (fmi1_xml_variable_t*)jm_vector_get_item(jm_voidp)(varByVR, i);

        if (!fmi1_same_vr_and_base_type(a, b)) {
            size_t aliases_removed;

            /* End of alias set, did not find base in current so remove it */
            aliases_removed = fmi1_xml_eliminate_bad_alias_set(context, i - 1);

            /* Setup for iterating through the next alias set */
            n -= aliases_removed; i -= aliases_removed;
            a = b;
        }
        if (b->aliasKind == fmi1_variable_is_not_alias) {
            return b;
        }
    }

    /* Did not find a next base alias */
    fmi1_xml_eliminate_bad_alias_set(context, i - 1);
    return NULL;
}

static int fmi1_alias_check_sign(
    fmi1_xml_variable_t* a,
    fmi1_xml_variable_t* b)
{
    if ((a->aliasKind == fmi1_variable_is_negated_alias) !=
        (b->aliasKind == fmi1_variable_is_negated_alias))
    {
        return -1;
    } else {
        return 1;
    }
}

static const char* fmi1_alias_negated_string(fmi1_xml_variable_t* a) {
    return  a->aliasKind == fmi1_variable_is_negated_alias ?
        "(negated alias)" : "";
}

#define FMIL_ABS(X) (((X) > 0) ? (X) : -(X))
#define FMIL_MAX(X, Y) ((X) > (Y) ? (X) : (Y))

static int fmi1_real_alias_consistent_start_values(
    jm_callbacks* cb,
    fmi1_xml_variable_t* a,
    fmi1_xml_variable_t* b)
{
    double a_start = fmi1_xml_get_real_variable_start(fmi1_xml_get_variable_as_real(a));
    double b_start = fmi1_xml_get_real_variable_start(fmi1_xml_get_variable_as_real(b));
    double scaling;
    int check_sign = fmi1_alias_check_sign(a, b);
    int consistent;

    scaling = FMIL_MAX(FMIL_ABS(a_start), 1.0);
    consistent = FMIL_ABS(a_start - check_sign*b_start) < 1e-14 * scaling;
    if (!consistent) {
        jm_log_error(cb, module, "Inconsistent start values in alias set, "
            "start value '%16.16f' of '%s'%s does not match "
            "start value '%16.16f' of '%s'%s.",
            a_start, a->name, fmi1_alias_negated_string(a),
            b_start, b->name, fmi1_alias_negated_string(b));
    }

    return consistent;
}

static int fmi1_int_alias_consistent_start_values(
    jm_callbacks* cb,
    fmi1_xml_variable_t* a,
    fmi1_xml_variable_t* b)
{
    int a_start = fmi1_xml_get_integer_variable_start(fmi1_xml_get_variable_as_integer(a));
    int b_start = fmi1_xml_get_integer_variable_start(fmi1_xml_get_variable_as_integer(b));
    int check_sign = fmi1_alias_check_sign(a, b);
    int consistent;

    consistent = (a_start == check_sign*b_start);
    if (!consistent) {
        jm_log_error(cb, module, "Inconsistent start values in alias set, "
            "start value '%d' of '%s'%s does not match "
            "start value '%d' of '%s'%s.",
            a_start, a->name, fmi1_alias_negated_string(a),
            b_start, b->name, fmi1_alias_negated_string(b));
    }

    return consistent;
}

static int fmi1_enum_alias_consistent_start_values(
    jm_callbacks* cb,
    fmi1_xml_variable_t* a,
    fmi1_xml_variable_t* b)
{
    int a_start = fmi1_xml_get_enum_variable_start(fmi1_xml_get_variable_as_enum(a));
    int b_start = fmi1_xml_get_enum_variable_start(fmi1_xml_get_variable_as_enum(b));
    int check_sign = fmi1_alias_check_sign(a, b);
    int consistent;

    consistent = (check_sign > 0) && (a_start == b_start);
    if (!consistent) {
        jm_log_error(cb, module, "Inconsistent start values in alias set, "
            "start value '%d' of '%s'%s does not match "
            "start value '%d' of '%s'%s.",
            a_start, a->name, fmi1_alias_negated_string(a),
            b_start, b->name, fmi1_alias_negated_string(b));
    }

    return consistent;
}

static int fmi1_bool_alias_consistent_start_values(
    jm_callbacks* cb,
    fmi1_xml_variable_t* a,
    fmi1_xml_variable_t* b)
{
    fmi1_boolean_t a_start = fmi1_xml_get_boolean_variable_start(fmi1_xml_get_variable_as_boolean(a));
    fmi1_boolean_t b_start = fmi1_xml_get_boolean_variable_start(fmi1_xml_get_variable_as_boolean(b));
    int check_sign = fmi1_alias_check_sign(a, b);
    int consistent;

    if (check_sign > 0) {
        consistent = ((!a_start) == (!b_start));
    } else {
        consistent = ((!a_start) != (!b_start));
    }
    if (!consistent) {
        jm_log_error(cb, module, "Inconsistent start values in alias set, "
            "start value '%s' of '%s'%s does not match "
            "start value '%s' of '%s'%s.",
            a_start ? "true" : "false", a->name, fmi1_alias_negated_string(a),
            b_start ? "true" : "false", b->name, fmi1_alias_negated_string(b));
    }

    return consistent;
}

static int fmi1_str_alias_consistent_start_values(
    jm_callbacks* cb,
    fmi1_xml_variable_t* a,
    fmi1_xml_variable_t* b)
{
    const char* a_start = fmi1_xml_get_string_variable_start(fmi1_xml_get_variable_as_string(a));
    const char* b_start = fmi1_xml_get_string_variable_start(fmi1_xml_get_variable_as_string(b));
    int check_sign = fmi1_alias_check_sign(a, b);
    int consistent;

    consistent = check_sign > 0 && strcmp(a_start, b_start) == 0;
    if (!consistent) {
        jm_log_error(cb, module, "Inconsistent start values in alias set, "
            "start value '%s' of '%s'%s does not match "
            "start value '%s' of '%s'%s.",
            a_start, a->name, fmi1_alias_negated_string(a),
            b_start, b->name, fmi1_alias_negated_string(b));
    }

    return consistent;
}

static int fmi1_alias_consistent_start_values(
    jm_callbacks* cb,
    fmi1_xml_variable_t* a,
    fmi1_xml_variable_t* b)
{
    fmi1_base_type_enu_t type = fmi1_xml_get_variable_base_type(a);
    assert(fmi1_same_vr_and_base_type(a, b));

    switch (type) {
        case fmi1_base_type_real:
            return fmi1_real_alias_consistent_start_values(cb, a, b);
        case fmi1_base_type_int:
            return fmi1_int_alias_consistent_start_values(cb, a, b);
        case fmi1_base_type_bool:
            return fmi1_bool_alias_consistent_start_values(cb, a, b);
        case fmi1_base_type_str:
            return fmi1_str_alias_consistent_start_values(cb, a, b);
        case fmi1_base_type_enum:
            return fmi1_enum_alias_consistent_start_values(cb, a, b);
        default: assert(0); return 0;
    }
}

static size_t handleAliasSet(
    fmi1_xml_parser_context_t *context,
    jm_vector(jm_voidp)* varByVR,
    fmi1_xml_variable_t* base_alias,
    size_t start_idx)
{
    size_t numvar, cur_list_idx;
    fmi1_xml_variable_t* v_with_start = NULL;

    numvar = jm_vector_get_size(jm_voidp)(varByVR);
    if (numvar <= start_idx) {
        /* Nothing to do if past the end of the list */
        return start_idx + 1;
    }

    assert(base_alias->aliasKind == fmi1_variable_is_not_alias);
    if (fmi1_xml_get_variable_has_start(base_alias)) {
        /* If base alias have start, use it as the correct value
         * when comparing against other vars so we don'r remove
         * it from the alias set */
        v_with_start = base_alias;
    }
    for(cur_list_idx = start_idx; cur_list_idx < numvar; cur_list_idx++) {
        fmi1_xml_variable_t* v = (fmi1_xml_variable_t*)jm_vector_get_item(jm_voidp)(varByVR, cur_list_idx);

        if (v == base_alias) {
            /* The base alias, nothing to do */
            continue;
        }
        if (!fmi1_same_vr_and_base_type(base_alias, v)) {
            /* Not the same vr and type, end of the alias set */
            break;
        } else {
            /* Next variable in list have same type and valueRef */
            if (v->aliasKind == fmi1_variable_is_not_alias) {
                /* But is for some reason marked as 'noAlias' => make it an alias */
                fmi1_xml_variable_t* c;
                size_t i, j;

                jm_log_error(context->callbacks, module, "Variables %s and %s reference the "
                    "same vr %u. Marking '%s' as alias.",
                     base_alias->name, v->name, v->vr, v->name);
                v->aliasKind = fmi1_variable_is_alias;

                /* Ok, now we sort b into a later position in the var list
                 * which is consistent with the lists sorting */
                i = cur_list_idx;
                j = cur_list_idx + 1;
                while(j < numvar) {
                    c = (fmi1_xml_variable_t*)jm_vector_get_item(jm_voidp)(varByVR, j);
                    if(fmi1_xml_compare_vr(&v,&c) <= 0) break;
                    j++;
                }
                j--;
                if(i != j) {
                    c = (fmi1_xml_variable_t*)jm_vector_get_item(jm_voidp)(varByVR, j);
                    jm_vector_set_item(jm_voidp)(varByVR, j, v);
                    jm_vector_set_item(jm_voidp)(varByVR, i, c);
                }

                /* Now that this inconsistency is taken care of we repeat
                 * current loop iteration: */
                cur_list_idx--;
                continue;
            } else if (fmi1_xml_get_variable_has_start(v)) {
                if (v_with_start == NULL) {
                    v_with_start = v;
                } else if (!fmi1_alias_consistent_start_values(
                                context->callbacks, v_with_start, v))
                {
                    fmi1_xml_eliminate_bad_alias(context, varByVR, cur_list_idx);
                    cur_list_idx--; numvar--;
                }
            }
        }
    }

    return cur_list_idx;
}

static void fmi1_checking_alias_info(
    fmi1_xml_parser_context_t *context,
    jm_vector(jm_voidp)* varByVR)
{
    size_t cur_list_idx = 0;

    jm_log_verbose(context->callbacks, module,"Checking alias information");
    while (cur_list_idx < jm_vector_get_size(jm_voidp)(varByVR)) {
        fmi1_xml_variable_t* base_alias = findNextBaseAliasIdx(context, varByVR, cur_list_idx);
        if (base_alias == NULL) break;
        cur_list_idx = handleAliasSet(context, varByVR, base_alias, cur_list_idx);
    }
}

int fmi1_xml_handle_ModelVariables(fmi1_xml_parser_context_t *context, const char* data) {
    if(!data) {
        jm_log_verbose(context->callbacks, module,"Parsing XML element ModelVariables");
    }
    else {
         /* postprocess variable list */

        fmi1_xml_model_description_t* md = context->modelDescription;
        jm_vector(jm_voidp)* varByVR;
        jm_vector(jm_voidp)* inputVars;
        jm_vector(jm_voidp)* outputVars;
        size_t i, numvar;
        size_t num_in = 0;
        size_t num_out = 0;
        numvar = jm_vector_get_size(jm_named_ptr)(&md->variablesByName);
        inputVars = jm_vector_alloc(jm_voidp)(numvar,numvar,md->callbacks);
        outputVars = jm_vector_alloc(jm_voidp)(numvar,numvar,md->callbacks);
        if (!inputVars || !outputVars) {
            fmi1_xml_parse_fatal(context, "Could not allocate memory");
        }
        /* vars with  vr = fmiUndefinedValueReference were already skipped. Just sanity: */
        /* remove any variable with vr = fmiUndefinedValueReference */
        for(i = 0; i< numvar; i++) {
            jm_named_ptr named = jm_vector_get_item(jm_named_ptr)(&md->variablesByName, i);
            fmi1_xml_variable_t* v = named.ptr;
            if(v->vr == fmi1_undefined_value_reference) {
                jm_vector_remove_item(jm_named_ptr)(&md->variablesByName,i);
                numvar--; i--;
                fmi1_xml_free_direct_dependencies(named);
                md->callbacks->free(v);
                assert(0);
            }
            if (v->causality == fmi1_causality_enu_input){
                jm_vector_set_item(jm_voidp)(inputVars, num_in, jm_vector_get_item(jm_named_ptr)(&md->variablesByName,i).ptr);
                num_in++;
            }
            if (v->causality == fmi1_causality_enu_output){
                jm_vector_set_item(jm_voidp)(outputVars, num_out, jm_vector_get_item(jm_named_ptr)(&md->variablesByName,i).ptr);
                num_out++;
            }

        }

        /* store the list of vars in origianl order */
        {
            size_t size = jm_vector_get_size(jm_named_ptr)(&md->variablesByName);
            md->variablesOrigOrder = jm_vector_alloc(jm_voidp)(size,size,md->callbacks);
            if(md->variablesOrigOrder) {
                size_t i;
                for(i= 0; i < size; ++i) {
                    jm_vector_set_item(jm_voidp)(md->variablesOrigOrder, i, jm_vector_get_item(jm_named_ptr)(&md->variablesByName,i).ptr);
                }
            }

        }

        /* store the list of input vars */
        md->inputVariables = jm_vector_alloc(jm_voidp)(num_in,num_in,md->callbacks);
        md->outputVariables = jm_vector_alloc(jm_voidp)(num_out,num_out,md->callbacks);
        if(md->inputVariables) {
                size_t i;
                for(i= 0; i < num_in; ++i) {
                    jm_vector_set_item(jm_voidp)(md->inputVariables, i, jm_vector_get_item(jm_voidp)(inputVars,i));
                }
            }
        else {
            fmi1_xml_parse_fatal(context, "Could not allocate memory");
        }
        if(md->outputVariables) {
                size_t i;
                for(i= 0; i < num_out; ++i) {
                    jm_vector_set_item(jm_voidp)(md->outputVariables, i, jm_vector_get_item(jm_voidp)(outputVars,i));
                }
            }
        else {
            fmi1_xml_parse_fatal(context, "Could not allocate memory");
        }
        jm_vector_free(jm_voidp)(inputVars);
        jm_vector_free(jm_voidp)(outputVars);

        /* sort the variables by names */
        jm_vector_qsort(jm_named_ptr)(&md->variablesByName,jm_compare_named);

        /* create VR index */
        md->status = fmi1_xml_model_description_enu_ok;
        {
            size_t size = jm_vector_get_size(jm_named_ptr)(&md->variablesByName);
            md->variablesByVR = jm_vector_alloc(jm_voidp)(size,size,md->callbacks);
            if(md->variablesByVR) {
                size_t i;
                for(i= 0; i < size; ++i) {
                    jm_vector_set_item(jm_voidp)(md->variablesByVR, i, jm_vector_get_item(jm_named_ptr)(&md->variablesByName,i).ptr);

                }
            }
        }

        md->status = fmi1_xml_model_description_enu_empty;
        if(!md->variablesByVR || !md->variablesOrigOrder || !md->inputVariables || !md->outputVariables) {
            fmi1_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }
        varByVR = md->variablesByVR;
        jm_vector_qsort(jm_voidp)(varByVR, fmi1_xml_compare_vr_and_original_index);

        fmi1_checking_alias_info(context, varByVR);

        numvar = jm_vector_get_size(jm_named_ptr)(&md->variablesByName);
        jm_log_verbose(context->callbacks, module,"Setting up direct dependencies cross-references");
        /* postprocess direct dependencies */
        for(i = 0; i< numvar; i++) {
            size_t numdep, j, var_i = 0;
            jm_vector(jm_voidp)* dep;
            fmi1_xml_variable_t* variable = (fmi1_xml_variable_t*)jm_vector_get_item(jm_named_ptr)(&md->variablesByName, i).ptr;

            if(!variable->directDependency) continue;
            dep = variable->directDependency;
            numdep = jm_vector_get_size(jm_voidp)(dep);
            for(j = 0; j < numdep; j++) {
                jm_string name = jm_vector_get_item(jm_voidp)(dep, j);
                jm_named_ptr key, *found;
                fmi1_xml_variable_t* depvar;
                key.name = name;
                key.ptr = 0;
                found = jm_vector_bsearch(jm_named_ptr)(&md->variablesByName, &key, jm_compare_named);
                if(found)
                    depvar = found->ptr;
                else
                    depvar = 0;
                if(!depvar) {
                    jm_log_error(context->callbacks,module, "Could not find variable %s mentioned in dependecies of %s. Ignoring", name, variable->name);
                    continue;
                }
                if(depvar->causality != fmi1_causality_enu_input) {
                    jm_log_error(context->callbacks,module, "Only input variables are allowed in DirectDependecies, but %s is not input. Ignoring", name);
                    continue;
                }
                jm_vector_set_item(jm_voidp)(dep,var_i++, depvar);
            }
            jm_vector_resize(jm_voidp)(dep,var_i);
        }
        jm_vector_foreach(jm_string)(&context->directDependencyStringsStore, (void(*)(jm_string))context->callbacks->free);
        jm_vector_free_data(jm_string)(&context->directDependencyStringsStore);

        /* might give out a warning if(data[0] != 0) */
    }
    return 0;
}
