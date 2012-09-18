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

#include "fmi2_xml_parser.h"
#include "fmi2_xml_type_impl.h"
#include "fmi2_xml_model_description_impl.h"

#include "fmi2_xml_variable_impl.h"

static const char* module = "FMI2XML";

const char* fmi2_xml_get_variable_name(fmi2_xml_variable_t* v) {
    return v->name;
}

const char* fmi2_xml_get_variable_description(fmi2_xml_variable_t* v) {
    return v->description;
}

size_t fmi2_xml_get_variable_original_order(fmi2_xml_variable_t* v) {
	return v->originalIndex;
}

fmi2_value_reference_t fmi2_xml_get_variable_vr(fmi2_xml_variable_t* v) {
    return v->vr;
}

fmi2_variable_alias_kind_enu_t fmi2_xml_get_variable_alias_kind(fmi2_xml_variable_t* v) {
    return (fmi2_variable_alias_kind_enu_t)v->aliasKind;
}

fmi2_xml_variable_t* fmi2_xml_get_variable_alias_base(fmi2_xml_model_description_t* md, fmi2_xml_variable_t* v) {
    fmi2_xml_variable_t key;
    fmi2_xml_variable_t *pkey = &key, *base;
    void ** found;
	if(!md->variablesByVR) return 0;
    if(v->aliasKind == fmi2_variable_is_not_alias) return v;
    key = *v;
    key.aliasKind = fmi2_variable_is_not_alias;

    found = jm_vector_bsearch(jm_voidp)(md->variablesByVR,(void**)&pkey, fmi2_xml_compare_vr);
    assert(found);
    base = *found;
    return base;
}

/*
    Return the list of all the variables aliased to the given one (including the base one.
    The list is ordered: base variable, aliases, negated aliases.
*/
jm_status_enu_t fmi2_xml_get_variable_aliases(fmi2_xml_model_description_t* md,fmi2_xml_variable_t* v, jm_vector(jm_voidp)* list) {
    fmi2_xml_variable_t key, *cur;
    fmi2_value_reference_t vr = fmi2_xml_get_variable_vr(v);
    size_t baseIndex, i, num = jm_vector_get_size(jm_voidp)(md->variablesByVR);
    key = *v;
    key.aliasKind = 0;
    cur = &key;
    baseIndex = jm_vector_bsearch_index(jm_voidp)(md->variablesByVR,(void**)&cur, fmi2_xml_compare_vr);
    cur = (fmi2_xml_variable_t*)jm_vector_get_item(jm_voidp)(md->variablesByVR, baseIndex);
    assert(cur);
    i = baseIndex + 1;
    while(fmi2_xml_get_variable_vr(cur) == vr) {
        if(!jm_vector_push_back(jm_voidp)(list, cur)) {
			jm_log_fatal(md->callbacks,module,"Could not allocate memory");
            return jm_status_error;
        };
        if(i >= num) break;
        cur = (fmi2_xml_variable_t*)jm_vector_get_item(jm_voidp)(md->variablesByVR, i);
        assert(cur);
        i++;
    }
    if(baseIndex) {
        i = baseIndex - 1;
        cur = (fmi2_xml_variable_t*)jm_vector_get_item(jm_voidp)(md->variablesByVR, i);
        while(fmi2_xml_get_variable_vr(cur) == vr) {
            if(!jm_vector_push_back(jm_voidp)(list, cur)) {
                jm_log_fatal(md->callbacks,module,"Could not allocate memory");                
                return jm_status_error;
            };
            i--;
            if(!i) break;
            cur = (fmi2_xml_variable_t*)jm_vector_get_item(jm_voidp)(md->variablesByVR, i - 1);
            assert(cur);
        }
    }
    return jm_status_success;
}


fmi2_xml_variable_typedef_t* fmi2_xml_get_variable_declared_type(fmi2_xml_variable_t* v) {
    return (fmi2_xml_variable_typedef_t*)(fmi2_xml_find_type_struct(v->typeBase, fmi2_xml_type_struct_enu_typedef));
}

fmi2_base_type_enu_t fmi2_xml_get_variable_base_type(fmi2_xml_variable_t* v) {
    fmi2_xml_variable_type_base_t* type = v->typeBase;
    type = fmi2_xml_find_type_struct(type, fmi2_xml_type_struct_enu_base);
    return (type->baseType);
}

int fmi2_xml_get_variable_has_start(fmi2_xml_variable_t* v) {
    return (v->typeBase->structKind == fmi2_xml_type_struct_enu_start);
}

int   fmi2_xml_get_variable_is_fixed(fmi2_xml_variable_t* v) {
    fmi2_xml_variable_type_base_t* type = v->typeBase;
    return ((type->structKind == fmi2_xml_type_struct_enu_start) && (type->isFixed));
}

fmi2_variability_enu_t fmi2_xml_get_variability(fmi2_xml_variable_t* v) {
    return (fmi2_variability_enu_t)v->variability;
}

fmi2_causality_enu_t fmi2_xml_get_causality(fmi2_xml_variable_t* v) {
    return (fmi2_causality_enu_t)v->causality;
}

double fmi2_xml_get_real_variable_start(fmi2_xml_real_variable_t* v) {
    fmi2_xml_variable_t* vv = (fmi2_xml_variable_t*)v;
    if(fmi2_xml_get_variable_has_start(vv)) {
        fmi2_xml_variable_start_real_t* start = (fmi2_xml_variable_start_real_t*)(vv->typeBase);
        return start->start;
    }
        return fmi2_xml_get_real_variable_nominal(v);
}

fmi2_xml_unit_t* fmi2_xml_get_real_variable_unit(fmi2_xml_real_variable_t* v) {
    fmi2_xml_variable_t* vv = (fmi2_xml_variable_t*)v;
    fmi2_xml_real_type_props_t* props = (fmi2_xml_real_type_props_t*)(fmi2_xml_find_type_struct(vv->typeBase, fmi2_xml_type_struct_enu_props));
    if(!props || !props->displayUnit) return 0;
    return props->displayUnit->baseUnit;
}

fmi2_xml_display_unit_t* fmi2_xml_get_real_variable_display_unit(fmi2_xml_real_variable_t* v) {
    fmi2_xml_variable_t* vv = (fmi2_xml_variable_t*)v;
    fmi2_xml_real_type_props_t* props = (fmi2_xml_real_type_props_t*)(fmi2_xml_find_type_struct(vv->typeBase, fmi2_xml_type_struct_enu_props));
    if(!props || !props->displayUnit || !props->displayUnit->displayUnit[0]) return 0;
    return props->displayUnit;
}

double fmi2_xml_get_real_variable_max(fmi2_xml_real_variable_t* v) {
    fmi2_xml_variable_t* vv = (fmi2_xml_variable_t*)v;
    fmi2_xml_real_type_props_t* props = (fmi2_xml_real_type_props_t*)(fmi2_xml_find_type_props(vv->typeBase));
	assert(props);
	return props->typeMax;
}

double fmi2_xml_get_real_variable_min(fmi2_xml_real_variable_t* v) {
    fmi2_xml_variable_t* vv = (fmi2_xml_variable_t*)v;
    fmi2_xml_real_type_props_t* props = (fmi2_xml_real_type_props_t*)(fmi2_xml_find_type_props(vv->typeBase));
	assert(props);
	return props->typeMin;
}

double fmi2_xml_get_real_variable_nominal(fmi2_xml_real_variable_t* v){
    fmi2_xml_variable_t* vv = (fmi2_xml_variable_t*)v;
    fmi2_xml_real_type_props_t* props = (fmi2_xml_real_type_props_t*)(fmi2_xml_find_type_props(vv->typeBase));
	assert(props);
	return props->typeNominal;
}

int fmi2_xml_get_integer_variable_start(fmi2_xml_integer_variable_t* v){
    fmi2_xml_variable_t* vv = (fmi2_xml_variable_t*)v;
    if(fmi2_xml_get_variable_has_start(vv)) {
        fmi2_xml_variable_start_integer_t* start = (fmi2_xml_variable_start_integer_t*)(vv->typeBase);
        return start->start;
    }
        return 0;
}

int fmi2_xml_get_integer_variable_min(fmi2_xml_integer_variable_t* v){
    fmi2_xml_variable_t* vv = (fmi2_xml_variable_t*)v;
    fmi2_xml_integer_type_props_t* props = (fmi2_xml_integer_type_props_t*)(fmi2_xml_find_type_props(vv->typeBase));
	assert(props);
	return props->typeMin;
}

int fmi2_xml_get_integer_variable_max(fmi2_xml_integer_variable_t* v){
    fmi2_xml_variable_t* vv = (fmi2_xml_variable_t*)v;
    fmi2_xml_integer_type_props_t* props = (fmi2_xml_integer_type_props_t*)(fmi2_xml_find_type_props(vv->typeBase));
	assert(props);
	return props->typeMax;
}

int fmi2_xml_get_enum_variable_min(fmi2_xml_enum_variable_t* v){
    fmi2_xml_variable_t* vv = (fmi2_xml_variable_t*)v;
    fmi2_xml_enum_type_props_t* props = (fmi2_xml_enum_type_props_t*)(fmi2_xml_find_type_props(vv->typeBase));
	assert(props);
	return props->typeMin;
}

int fmi2_xml_get_enum_variable_max(fmi2_xml_enum_variable_t* v){
    fmi2_xml_variable_t* vv = (fmi2_xml_variable_t*)v;
    fmi2_xml_enum_type_props_t* props = (fmi2_xml_enum_type_props_t*)(fmi2_xml_find_type_props(vv->typeBase));
	assert(props);
	return props->typeMax;
}

const char* fmi2_xml_get_string_variable_start(fmi2_xml_string_variable_t* v){
    fmi2_xml_variable_t* vv = (fmi2_xml_variable_t*)v;
    if(fmi2_xml_get_variable_has_start(vv)) {
        fmi2_xml_variable_start_string_t* start = (fmi2_xml_variable_start_string_t*)(vv->typeBase);
        return start->start;
    }
    return 0;
}

int fmi2_xml_get_enum_variable_start(fmi2_xml_enum_variable_t* v) {
    fmi2_xml_variable_t* vv = (fmi2_xml_variable_t*)v;
    if(fmi2_xml_get_variable_has_start(vv)) {
        fmi2_xml_variable_start_integer_t* start = (fmi2_xml_variable_start_integer_t*)(vv->typeBase);
        return start->start;
    }
        return 0;
}

fmi2_boolean_t fmi2_xml_get_boolean_variable_start(fmi2_xml_bool_variable_t* v) {
    fmi2_xml_variable_t* vv = (fmi2_xml_variable_t*)v;
    if(fmi2_xml_get_variable_has_start(vv)) {
        fmi2_xml_variable_start_integer_t* start = (fmi2_xml_variable_start_integer_t*)(vv->typeBase);
        return start->start;
    }
        return 0;
}

size_t fmi2_xml_get_direct_dependency_size(fmi2_xml_model_description_t* md,fmi2_xml_variable_t*v) {
	if(v->directDependency) {
		return jm_vector_get_size(jm_voidp)(v->directDependency);
	}
	else
		return 0;
}

/* DirectDependency is returned for variables with causality Output. Null pointer for others. */
jm_status_enu_t fmi2_xml_get_direct_dependency(fmi2_xml_model_description_t* md, fmi2_xml_variable_t* v, jm_vector(jm_voidp)* list) {
	size_t size = 0;
	if(fmi2_xml_get_causality(v) != fmi2_causality_enu_output) return jm_status_error;
	jm_vector_resize(jm_voidp)(list, 0);
	if(v->directDependency) {
		size = jm_vector_get_size(jm_voidp)(v->directDependency);
		if(jm_vector_reserve(jm_voidp)(list, size) < size) return jm_status_error;
	    jm_vector_copy(jm_voidp)(list,v->directDependency);
	}
	return jm_status_success;
}

fmi2_xml_real_variable_t* fmi2_xml_get_variable_as_real(fmi2_xml_variable_t* v) {
    if(fmi2_xml_get_variable_base_type(v) == fmi2_base_type_real)  return (void*)v;
    return 0;
}

fmi2_xml_integer_variable_t* fmi2_xml_get_variable_as_integer(fmi2_xml_variable_t*v){
    if(fmi2_xml_get_variable_base_type(v) == fmi2_base_type_int)  return (void*)v;
    return 0;
}
fmi2_xml_enum_variable_t* fmi2_xml_get_variable_as_enum(fmi2_xml_variable_t* v){
    if(fmi2_xml_get_variable_base_type(v) == fmi2_base_type_enum)  return (void*)v;
    return 0;
}
fmi2_xml_string_variable_t* fmi2_xml_get_variable_as_string(fmi2_xml_variable_t* v){
    if(fmi2_xml_get_variable_base_type(v) == fmi2_base_type_str)  return (void*)v;
    return 0;
}
fmi2_xml_bool_variable_t* fmi2_xml_get_variable_as_boolean(fmi2_xml_variable_t* v){
    if(fmi2_xml_get_variable_base_type(v) == fmi2_base_type_bool)  return (void*)v;
    return 0;
}

void fmi2_xml_free_direct_dependencies(jm_named_ptr named) {
        fmi2_xml_variable_t* v = named.ptr;
        if(v->directDependency) {
                jm_vector_free(jm_voidp)(v->directDependency);
                v->directDependency = 0;
        }
}

int fmi2_xml_handle_ScalarVariable(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
            fmi2_xml_model_description_t* md = context->modelDescription;
            fmi2_xml_variable_t* variable;
            fmi2_xml_variable_t dummyV;
            const char* description = 0;
            jm_named_ptr named, *pnamed;
            jm_vector(char)* bufName = fmi2_xml_reserve_parse_buffer(context,1,100);
            jm_vector(char)* bufDescr = fmi2_xml_reserve_parse_buffer(context,2,100);
            unsigned int vr;

            if(!bufName || !bufDescr) return -1;

            /*   <xs:attribute name="valueReference" type="xs:unsignedInt" use="required"> */
            if(fmi2_xml_set_attr_uint(context, fmi2_xml_elmID_ScalarVariable, fmi_attr_id_valueReference, 1, &vr, 0)) return -1;

            if(vr == fmi2_undefined_value_reference) {
                context->skipOneVariableFlag = 1;
            }

            if(
            /*  <xs:attribute name="name" type="xs:normalizedString" use="required"/> */
                fmi2_xml_set_attr_string(context, fmi2_xml_elmID_ScalarVariable, fmi_attr_id_name, 1, bufName) ||
            /* <xs:attribute name="description" type="xs:string"/> */
                fmi2_xml_set_attr_string(context, fmi2_xml_elmID_ScalarVariable, fmi_attr_id_description, 0, bufDescr)
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

            if(pnamed) *pnamed = named = jm_named_alloc_v(bufName,sizeof(fmi2_xml_variable_t), dummyV.name - (char*)&dummyV, context->callbacks);
            variable = named.ptr;
            if( !pnamed || !variable ) {
                fmi2_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            variable->vr = vr;
            variable->description = description;
            variable->typeBase = 0;
            variable->directDependency = 0;
			variable->originalIndex = jm_vector_get_size(jm_named_ptr)(&md->variablesByName) - 1;

              {
                jm_name_ID_map_t variabilityConventionMap[] = {{"continuous",fmi2_variability_enu_continuous},
                                                               {"constant", fmi2_variability_enu_constant},
                                                               {"parameter", fmi2_variability_enu_parameter},
                                                               {"discrete", fmi2_variability_enu_discrete},{0,0}};
                unsigned int variability;
                /*  <xs:attribute name="variability" default="continuous"> */
                if(fmi2_xml_set_attr_enum(context, fmi2_xml_elmID_ScalarVariable, fmi_attr_id_variability,0,&variability,fmi2_variability_enu_continuous,variabilityConventionMap))
                    return -1;
                variable->variability = variability;
            }
            {
                jm_name_ID_map_t causalityConventionMap[] = {{"internal",fmi2_causality_enu_internal},
                                                             {"input",fmi2_causality_enu_input},
                                                             {"output",fmi2_causality_enu_output},
                                                             {"none",fmi2_causality_enu_none},{0,0}};
                /* <xs:attribute name="causality" default="internal"> */
                unsigned int causality;
                if(fmi2_xml_set_attr_enum(context, fmi2_xml_elmID_ScalarVariable, fmi_attr_id_causality,0,&causality,fmi2_causality_enu_internal,causalityConventionMap))
                    return -1;
                variable->causality = causality;
            }
            {
                jm_name_ID_map_t aliasConventionMap[] = {{"alias", 1},
                                                         {"negatedAlias", 2},
                                                         {"noAlias", 0}, {0,0}};
                unsigned int alias;
                /* <xs:attribute name="alias" default="noAlias"> */
                if(fmi2_xml_set_attr_enum(context, fmi2_xml_elmID_ScalarVariable, fmi_attr_id_alias ,0,&alias,0,aliasConventionMap))
                    return -1;
                if(alias == 0) variable->aliasKind = fmi2_variable_is_not_alias;
                else if (alias == 1) variable->aliasKind = fmi2_variable_is_alias;
                else if (alias == 2) variable->aliasKind = fmi2_variable_is_negated_alias;
                else assert(0);
            }
    }
    else {
        if(context->skipOneVariableFlag) {
            context->skipOneVariableFlag = 0;
        }
        else {
            /* check that the type for the variable is set */
            fmi2_xml_model_description_t* md = context->modelDescription;
            fmi2_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
            if(!variable->typeBase) {
				jm_log_error(context->callbacks, module, "No variable type element for variable %s. Assuming Real.", variable->name);

				return fmi2_xml_handle_Real(context, data);
            }
        }
        /* might give out a warning if(data[0] != 0) */
    }
    return 0;
}


int fmi2_xml_handle_DirectDependency(fmi2_xml_parser_context_t *context, const char* data) {
    if(context->skipOneVariableFlag) return 0;
    if(!data) {
        fmi2_xml_model_description_t* md = context->modelDescription;
        fmi2_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
        if(variable->causality != fmi2_causality_enu_output) {
			jm_log_error(context->callbacks,module, 
				"DirectDependency XML element cannot be defined for '%s' since causality is not output. Skipping.", variable->name);
			context->skipElementCnt = 1;
            return 0;
        }
    }
    else {
        fmi2_xml_model_description_t* md = context->modelDescription;
        fmi2_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
        if(jm_vector_get_size(jm_voidp)(&context->directDependencyBuf)) {
            variable->directDependency = jm_vector_clone(jm_voidp)(&context->directDependencyBuf);
            if(!variable->directDependency) {
                fmi2_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
        }
        jm_vector_resize(jm_voidp)(&context->directDependencyBuf,0);
    }
    return 0;
}

int fmi2_xml_handle_Name(fmi2_xml_parser_context_t *context, const char* data) {
    if(context->skipOneVariableFlag) return 0;

    if(!data) {
		return 0;
    }
    else {
        fmi2_xml_model_description_t* md = context->modelDescription;
        fmi2_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
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
            fmi2_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }
		for(j = 0; i<namelen;i++) {
			name[j++] = data[i];
		}
		name[j] = 0;
    }
    return 0;
}

int fmi2_xml_handle_Real(fmi2_xml_parser_context_t *context, const char* data) {
    if(context->skipOneVariableFlag) return 0;

    if(!data) {
        fmi2_xml_model_description_t* md = context->modelDescription;
        fmi2_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
        fmi2_xml_type_definitions_t* td = &md->typeDefinitions;
        fmi2_xml_variable_type_base_t * declaredType = 0;
        fmi2_xml_real_type_props_t * type = 0;
        int hasStart;

        assert(!variable->typeBase);

        declaredType = fmi2_get_declared_type(context, fmi2_xml_elmID_Real, &td->defaultRealType.typeBase);

        if(!declaredType) return -1;

        {
            int hasUnit = fmi2_xml_is_attr_defined(context, fmi_attr_id_unit) ||
                    fmi2_xml_is_attr_defined(context, fmi_attr_id_displayUnit);
            int hasMin =  fmi2_xml_is_attr_defined(context, fmi_attr_id_min);
            int hasMax = fmi2_xml_is_attr_defined(context, fmi_attr_id_max);
            int hasNom = fmi2_xml_is_attr_defined(context, fmi_attr_id_nominal);
            int hasQuan = fmi2_xml_is_attr_defined(context, fmi_attr_id_quantity);
            int hasRelQ = fmi2_xml_is_attr_defined(context, fmi_attr_id_relativeQuantity);


            if(hasUnit || hasMin || hasMax || hasNom || hasQuan || hasRelQ) {
                fmi2_xml_real_type_props_t* props = 0;

                if(declaredType->structKind == fmi2_xml_type_struct_enu_typedef)
                    props = (fmi2_xml_real_type_props_t*)(declaredType->baseTypeStruct);
                else
                    props = (fmi2_xml_real_type_props_t* )declaredType;

                fmi2_xml_reserve_parse_buffer(context, 1, 0);
                fmi2_xml_reserve_parse_buffer(context, 2, 0);

                type = fmi2_xml_parse_real_type_properties(context, fmi2_xml_elmID_Real);

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
                type = (fmi2_xml_real_type_props_t*)declaredType;
        }
        variable->typeBase = &type->typeBase;

        hasStart = fmi2_xml_is_attr_defined(context, fmi_attr_id_start);
        if(hasStart) {
            fmi2_xml_variable_start_real_t * start = (fmi2_xml_variable_start_real_t*)fmi2_xml_alloc_variable_type_start(td, &type->typeBase, sizeof(fmi2_xml_variable_start_real_t));
            int isFixedBuf;
            if(!start) {
                fmi2_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            if(
                /*  <xs:attribute name="start" type="xs:double"/> */
                    fmi2_xml_set_attr_double(context, fmi2_xml_elmID_Real, fmi_attr_id_start, 0, &start->start, 0) ||
                /*  <xs:attribute name="fixed" type="xs:boolean"> */
                    fmi2_xml_set_attr_boolean(context, fmi2_xml_elmID_Real, fmi_attr_id_fixed, 0, &(isFixedBuf), 1)
                )
                    return -1;
            start->typeBase.isFixed = isFixedBuf;
            variable->typeBase = &start->typeBase;
        }
        else {
            if(fmi2_xml_is_attr_defined(context,fmi_attr_id_fixed)) {
                jm_log_warning(context->callbacks, module, "When parsing variable %s: 'fixed' attributed is only allowed when start is defined", variable->name);
            }
        }
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi2_xml_handle_Integer(fmi2_xml_parser_context_t *context, const char* data) {
    if(context->skipOneVariableFlag) return 0;

    if(!data) {
        fmi2_xml_model_description_t* md = context->modelDescription;
        fmi2_xml_type_definitions_t* td = &md->typeDefinitions;
        fmi2_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
        fmi2_xml_variable_type_base_t * declaredType = 0;
        fmi2_xml_integer_type_props_t * type = 0;
        int hasStart;

        declaredType = fmi2_get_declared_type(context, fmi2_xml_elmID_Integer,&td->defaultIntegerType.typeBase) ;

        if(!declaredType) return -1;

        if(
                fmi2_xml_is_attr_defined(context,fmi_attr_id_min) ||
                fmi2_xml_is_attr_defined(context,fmi_attr_id_max) ||
                fmi2_xml_is_attr_defined(context,fmi_attr_id_quantity)
                ) {
            fmi2_xml_integer_type_props_t* props = 0;

            if(declaredType->structKind != fmi2_xml_type_struct_enu_typedef)
                props = (fmi2_xml_integer_type_props_t*)declaredType;
            else
                props = (fmi2_xml_integer_type_props_t*)(declaredType->baseTypeStruct);
            assert((props->typeBase.structKind == fmi2_xml_type_struct_enu_props) || (props->typeBase.structKind == fmi2_xml_type_struct_enu_base));
            fmi2_xml_reserve_parse_buffer(context, 1, 0);
            fmi2_xml_reserve_parse_buffer(context, 2, 0);
            type = fmi2_xml_parse_integer_type_properties(context, fmi2_xml_elmID_Integer);
            if(!type) return -1;
            type->typeBase.baseTypeStruct = declaredType;
            if(!fmi2_xml_is_attr_defined(context,fmi_attr_id_min)) type->typeMin = props->typeMin;
            if(!fmi2_xml_is_attr_defined(context,fmi_attr_id_max)) type->typeMax = props->typeMax;
            if(!fmi2_xml_is_attr_defined(context,fmi_attr_id_quantity)) type->quantity = props->quantity;
        }
        else
            type = (fmi2_xml_integer_type_props_t*)declaredType;

        variable->typeBase = &type->typeBase;

        hasStart = fmi2_xml_is_attr_defined(context,fmi_attr_id_start);
        if(hasStart) {
            fmi2_xml_variable_start_integer_t * start = (fmi2_xml_variable_start_integer_t*)fmi2_xml_alloc_variable_type_start(td, &type->typeBase, sizeof(fmi2_xml_variable_start_integer_t));
            int isFixedBuf;
            if(!start) {
                fmi2_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            if(
                /*  <xs:attribute name="start" type="xs:integer"/> */
                    fmi2_xml_set_attr_int(context, fmi2_xml_elmID_Integer, fmi_attr_id_start, 0, &start->start, 0) ||
                /*  <xs:attribute name="fixed" type="xs:boolean"> */
                    fmi2_xml_set_attr_boolean(context, fmi2_xml_elmID_Integer, fmi_attr_id_fixed, 0, &isFixedBuf, 1)
                )
                    return -1;
            start->typeBase.isFixed = isFixedBuf;
            variable->typeBase = &start->typeBase;
        }
        else {
            if(fmi2_xml_is_attr_defined(context,fmi_attr_id_fixed)) {
                jm_log_warning(context->callbacks, module, "When parsing variable %s: 'fixed' attributed is only allowed when start is defined", variable->name);
            }
        }
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi2_xml_handle_Boolean(fmi2_xml_parser_context_t *context, const char* data) {
    if(context->skipOneVariableFlag) return 0;

    if(!data) {
        fmi2_xml_model_description_t* md = context->modelDescription;
        fmi2_xml_type_definitions_t* td = &md->typeDefinitions;
        fmi2_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
        int hasStart;

		assert(!variable->typeBase);

        variable->typeBase = fmi2_get_declared_type(context, fmi2_xml_elmID_Boolean, &td->defaultBooleanType) ;

        if(!variable->typeBase) return -1;

        hasStart = fmi2_xml_is_attr_defined(context,fmi_attr_id_start);
        if(hasStart) {
            int isFixedBuf;
            fmi2_xml_variable_start_integer_t * start = (fmi2_xml_variable_start_integer_t*)fmi2_xml_alloc_variable_type_start(td, variable->typeBase, sizeof(fmi2_xml_variable_start_integer_t ));
            if(!start) {
                fmi2_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            if(
                  /*  <xs:attribute name="start" type="xs:boolean"/> */
                    fmi2_xml_set_attr_boolean(context, fmi2_xml_elmID_Boolean, fmi_attr_id_start, 0, (int*)&start->start, 0) ||
                /*  <xs:attribute name="fixed" type="xs:boolean"> */
                    fmi2_xml_set_attr_boolean(context, fmi2_xml_elmID_Boolean, fmi_attr_id_fixed, 0, &isFixedBuf, 1)
                )
                    return -1;
            start->typeBase.isFixed = isFixedBuf;
            variable->typeBase = &start->typeBase;
        }
        else {
            if(fmi2_xml_is_attr_defined(context,fmi_attr_id_fixed)) {
                jm_log_warning(context->callbacks, module,"When parsing variable %s: 'fixed' attributed is only allowed when start is defined", variable->name);
            }            
        }
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi2_xml_handle_String(fmi2_xml_parser_context_t *context, const char* data) {
    if(context->skipOneVariableFlag) return 0;

    if(!data) {
        fmi2_xml_model_description_t* md = context->modelDescription;
        fmi2_xml_type_definitions_t* td = &md->typeDefinitions;
        fmi2_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
        int hasStart;

		assert(!variable->typeBase);

        variable->typeBase = fmi2_get_declared_type(context, fmi2_xml_elmID_String,&td->defaultStringType) ;

        if(!variable->typeBase) return -1;

        hasStart = fmi2_xml_is_attr_defined(context,fmi_attr_id_start);
        if(hasStart) {
            jm_vector(char)* bufStartStr = fmi2_xml_reserve_parse_buffer(context,1, 100);
            size_t strlen;
            int isFixed;
            fmi2_xml_variable_start_string_t * start;
            if(
                 /*   <xs:attribute name="start" type="xs:string"/> */
                    fmi2_xml_set_attr_string(context, fmi2_xml_elmID_String, fmi_attr_id_start, 0, bufStartStr) ||
                /*  <xs:attribute name="fixed" type="xs:boolean"> */
                    fmi2_xml_set_attr_boolean(context, fmi2_xml_elmID_Boolean, fmi_attr_id_fixed, 0, &isFixed, 1)
                )
                    return -1;
            strlen = jm_vector_get_size_char(bufStartStr);

            start = (fmi2_xml_variable_start_string_t*)fmi2_xml_alloc_variable_type_start(td, variable->typeBase, sizeof(fmi2_xml_variable_start_string_t) + strlen);

            if(!start) {
                fmi2_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            memcpy(start->start, jm_vector_get_itemp_char(bufStartStr,0), strlen);
            start->start[strlen] = 0;
            variable->typeBase = &start->typeBase;
        }
        else {
            if(fmi2_xml_is_attr_defined(context,fmi_attr_id_fixed)) {
                jm_log_warning(context->callbacks, module, "When parsing variable %s: 'fixed' attributed is only allowed when start is defined", variable->name);
            }
        }
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi2_xml_handle_Enumeration(fmi2_xml_parser_context_t *context, const char* data) {
    if(context->skipOneVariableFlag) return 0;

    if(!data) {
        fmi2_xml_model_description_t* md = context->modelDescription;
        fmi2_xml_type_definitions_t* td = &md->typeDefinitions;
        fmi2_xml_variable_t* variable = jm_vector_get_last(jm_named_ptr)(&md->variablesByName).ptr;
        fmi2_xml_variable_type_base_t * declaredType = 0;
        fmi2_xml_integer_type_props_t * type = 0;
        int hasStart;

		assert(!variable->typeBase);

        declaredType = fmi2_get_declared_type(context, fmi2_xml_elmID_Enumeration,&td->defaultEnumType.typeBase);

        if(!declaredType) return -1;

        if(
                fmi2_xml_is_attr_defined(context,fmi_attr_id_min) ||
                fmi2_xml_is_attr_defined(context,fmi_attr_id_max) ||
                fmi2_xml_is_attr_defined(context,fmi_attr_id_quantity)
                ) {
            fmi2_xml_integer_type_props_t* props = 0;

            if(declaredType->structKind != fmi2_xml_type_struct_enu_typedef)
                props = (fmi2_xml_integer_type_props_t*)declaredType;
            else
                props = (fmi2_xml_integer_type_props_t*)declaredType->baseTypeStruct;
            assert(props->typeBase.structKind == fmi2_xml_type_struct_enu_props);
            fmi2_xml_reserve_parse_buffer(context, 1, 0);
            fmi2_xml_reserve_parse_buffer(context, 2, 0);
            type = fmi2_xml_parse_integer_type_properties(context, fmi2_xml_elmID_Enumeration);
            if(!type) return -1;
            type->typeBase.baseTypeStruct = declaredType;
            if(!fmi2_xml_is_attr_defined(context,fmi_attr_id_min)) type->typeMin = props->typeMin;
            if(!fmi2_xml_is_attr_defined(context,fmi_attr_id_max)) type->typeMax = props->typeMax;
            if(!fmi2_xml_is_attr_defined(context,fmi_attr_id_quantity)) type->quantity = props->quantity;
        }
        else
            type = (fmi2_xml_integer_type_props_t*)declaredType;

        variable->typeBase = &type->typeBase;

        hasStart = fmi2_xml_is_attr_defined(context,fmi_attr_id_start);
        if(hasStart) {
            fmi2_xml_variable_start_integer_t * start = (fmi2_xml_variable_start_integer_t*)fmi2_xml_alloc_variable_type_start(td, &type->typeBase, sizeof(fmi2_xml_variable_start_integer_t ));
            int isFixedBuf;
            if(!start) {
                fmi2_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            if(
                /*  <xs:attribute name="start" type="xs:integer"/> */
                    fmi2_xml_set_attr_int(context, fmi2_xml_elmID_Enumeration, fmi_attr_id_start, 0, &start->start, 0) ||
                /*  <xs:attribute name="fixed" type="xs:boolean"> */
                    fmi2_xml_set_attr_boolean(context, fmi2_xml_elmID_Enumeration, fmi_attr_id_fixed, 0, &isFixedBuf, 1)
                )
                    return -1;
            start->typeBase.isFixed = isFixedBuf;
            variable->typeBase = &start->typeBase;
        }
        else {
            if(fmi2_xml_is_attr_defined(context,fmi_attr_id_fixed)) {
                jm_log_warning(context->callbacks, module, "When parsing variable %s: 'fixed' attributed is only allowed when start is defined", variable->name);
            }            
        }
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

#define fmi2_xml_diff_variable_original_index(a, b) (((fmi2_xml_variable_t*)a)->originalIndex - ((fmi2_xml_variable_t*)b)->originalIndex)

jm_define_comp_f(fmi2_xml_compare_variable_original_index, jm_voidp, fmi2_xml_diff_variable_original_index)


void fmi2_xml_eliminate_bad_alias(fmi2_xml_parser_context_t *context, size_t indexVR) {
    fmi2_xml_model_description_t* md = context->modelDescription;
    jm_vector(jm_voidp)* varByVR = md->variablesByVR;
    fmi2_xml_variable_t* v = (fmi2_xml_variable_t*)jm_vector_get_item(jm_voidp)(varByVR, indexVR);
    fmi2_value_reference_t vr = v->vr;
    fmi2_base_type_enu_t vt = fmi2_xml_get_variable_base_type(v);
    size_t i, n = jm_vector_get_size(jm_voidp)(varByVR);
    for(i = 0; i< n; i++) {
        jm_named_ptr key;
        size_t index;
        v = (fmi2_xml_variable_t*)jm_vector_get_item(jm_voidp)(varByVR, i);
        if((v->vr != vr)||(vt != fmi2_xml_get_variable_base_type(v))) continue;
        jm_vector_remove_item_jm_voidp(varByVR,i);
        n--; i--;
        key.name = v->name;
        index = jm_vector_bsearch_index(jm_named_ptr)(&md->variablesByName, &key, jm_compare_named);
        assert(index <= n);
        jm_vector_remove_item(jm_named_ptr)(&md->variablesByName,index);

		index = jm_vector_bsearch_index(jm_voidp)(md->variablesOrigOrder, (jm_voidp*)&v, fmi2_xml_compare_variable_original_index);
        assert(index <= n);
		
		jm_vector_remove_item(jm_voidp)(md->variablesOrigOrder,index);
		
		jm_log_error(context->callbacks, module,"Removing incorrect alias variable '%s'", v->name);
        md->callbacks->free(v);
    }
}

static int fmi2_xml_compare_vr_and_original_index (const void* first, const void* second) {
	int ret = fmi2_xml_compare_vr(first, second);
	if(ret == 0) {
	    fmi2_xml_variable_t* a = *(fmi2_xml_variable_t**)first;
		fmi2_xml_variable_t* b = *(fmi2_xml_variable_t**)second;
		ret = (a->originalIndex - b->originalIndex);
	}
	
	return ret;
}

int fmi2_xml_handle_ModelVariables(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
		jm_log_verbose(context->callbacks, module,"Parsing XML element ModelVariables");
    }
    else {
         /* postprocess variable list */

        fmi2_xml_model_description_t* md = context->modelDescription;
        jm_vector(jm_voidp)* varByVR;
        size_t i, numvar;

        numvar = jm_vector_get_size(jm_named_ptr)(&md->variablesByName);
        /* vars with  vr = fmiUndefinedValueReference were already skipped. Just sanity: */
        /* remove any variable with vr = fmiUndefinedValueReference */
        for(i = 0; i< numvar; i++) {
            jm_named_ptr named = jm_vector_get_item(jm_named_ptr)(&md->variablesByName, i);
            fmi2_xml_variable_t* v = named.ptr;
            if(v->vr == fmi2_undefined_value_reference) {
                jm_vector_remove_item(jm_named_ptr)(&md->variablesByName,i);
                numvar--; i--;
                fmi2_xml_free_direct_dependencies(named);
                md->callbacks->free(v);
                assert(0);
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

        /* sort the variables by names */
        jm_vector_qsort(jm_named_ptr)(&md->variablesByName,jm_compare_named);

        /* create VR index */
        md->status = fmi2_xml_model_description_enu_ok;
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

        md->status = fmi2_xml_model_description_enu_empty;
		if(!md->variablesByVR || !md->variablesOrigOrder) {
            fmi2_xml_parse_fatal(context, "Could not allocate memory");
            return -1;
        }
        varByVR = md->variablesByVR;
        jm_vector_qsort(jm_voidp)(varByVR, fmi2_xml_compare_vr_and_original_index);

        {
            int foundBadAlias;

			jm_log_verbose(context->callbacks, module,"Checking alias information");
            do {
                fmi2_xml_variable_t* a = (fmi2_xml_variable_t*)jm_vector_get_item(jm_voidp)(varByVR, 0);
                foundBadAlias = 0;

                if(a->aliasKind == fmi2_variable_is_alias) {
					jm_log_error(context->callbacks, module, "All variables with vr %d (base type %s) are marked as aliases.",
                                          a->vr, fmi2_base_type_to_string(fmi2_xml_get_variable_base_type(a)));
                    fmi2_xml_eliminate_bad_alias(context,0);
                    foundBadAlias = 1;
                    continue;
                }
                numvar = jm_vector_get_size(jm_voidp)(varByVR);

                for(i = 1; i< numvar; i++) {
                    fmi2_xml_variable_t* b = (fmi2_xml_variable_t*)jm_vector_get_item(jm_voidp)(varByVR, i);
                    if((fmi2_xml_get_variable_base_type(a)!=fmi2_xml_get_variable_base_type(b))
                            || (a->vr != b->vr)) {
                        /* a different vr */
                        if(a->aliasKind == fmi2_variable_is_negated_alias) {
                            jm_log_error(context->callbacks,module,"All variables with vr %u (base type %s) are marked as negated aliases",
                                                  a->vr, fmi2_base_type_to_string(fmi2_xml_get_variable_base_type(a)));
                            fmi2_xml_eliminate_bad_alias(context,i-1);
                            foundBadAlias = 1;
                            break;
                        }
                        if(b->aliasKind == fmi2_variable_is_alias) {
                            jm_log_error(context->callbacks,module,"All variables with vr %u (base type %s) are marked as aliases",
                                                b->vr, fmi2_base_type_to_string(fmi2_xml_get_variable_base_type(b)));
                          fmi2_xml_eliminate_bad_alias(context,i);
                          foundBadAlias = 1;
                          break;
                        }
                    }
                    else {
                        if(   (a->aliasKind == fmi2_variable_is_negated_alias)
                                && (b->aliasKind == fmi2_variable_is_alias)) {
                            jm_log_error(context->callbacks,module, "All variables with vr %u (base type %s) are marked as aliases",
                                                b->vr, fmi2_base_type_to_string(fmi2_xml_get_variable_base_type(b)));
                          fmi2_xml_eliminate_bad_alias(context,i);
                          foundBadAlias = 1;
                          break;
                        }
                        if((a->aliasKind == fmi2_variable_is_not_alias) && (a->aliasKind == b->aliasKind)) {
                            fmi2_xml_variable_t* c;
                            size_t j = i+1;
                            jm_log_error(context->callbacks,module,"Variables %s and %s reference the same vr %u. Marking '%s' as alias.",
                                                a->name, b->name, b->vr, b->name);
                            b->aliasKind = fmi2_variable_is_alias;

                            while(j < numvar) {
                                c = (fmi2_xml_variable_t*)jm_vector_get_item(jm_voidp)(varByVR, j);
                                if(fmi2_xml_compare_vr(&b,&c) <= 0) break;
                                j++;
                            }
                            j--;
                            if(i != j) {
                                c = (fmi2_xml_variable_t*)jm_vector_get_item(jm_voidp)(varByVR, j);
                                jm_vector_set_item(jm_voidp)(varByVR, j, b);
                                jm_vector_set_item(jm_voidp)(varByVR, i, c);
                            }
                            foundBadAlias = 1;
                            i--;
                            continue;
                        }
                    }
                    a = b;
                }
            } while(foundBadAlias);
        }

        numvar = jm_vector_get_size(jm_named_ptr)(&md->variablesByName);
		jm_log_verbose(context->callbacks, module,"Setting up direct dependencies cross-references");
        /* postprocess direct dependencies */
        for(i = 0; i< numvar; i++) {
            size_t numdep, j, var_i = 0;
            jm_vector(jm_voidp)* dep;
            fmi2_xml_variable_t* variable = (fmi2_xml_variable_t*)jm_vector_get_item(jm_named_ptr)(&md->variablesByName, i).ptr;

            if(!variable->directDependency) continue;
            dep = variable->directDependency;
            numdep = jm_vector_get_size(jm_voidp)(dep);
            for(j = 0; j < numdep; j++) {
                jm_string name = jm_vector_get_item(jm_voidp)(dep, j);
                jm_named_ptr key, *found;
                fmi2_xml_variable_t* depvar;
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
                if(depvar->causality != fmi2_causality_enu_input) {
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
