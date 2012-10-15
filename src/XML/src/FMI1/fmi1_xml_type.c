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

#include <limits.h>
#include <float.h>
#include <string.h>

#include "fmi1_xml_model_description_impl.h"
#include "fmi1_xml_type_impl.h"
#include "fmi1_xml_unit_impl.h"
#include "fmi1_xml_parser.h"

static const char* module = "FMI1XML";

fmi1_xml_display_unit_t* fmi1_xml_get_type_display_unit(fmi1_xml_real_typedef_t* t) {
    fmi1_xml_variable_typedef_t* vt = (void*)t;
    fmi1_xml_real_type_props_t * props = (fmi1_xml_real_type_props_t*)vt->typeBase.baseTypeStruct;
    fmi1_xml_display_unit_t* du = props->displayUnit;
    if(du->displayUnit) return du;
    return 0;
}

size_t fmi1_xml_get_type_definition_number(fmi1_xml_type_definitions_t* td) {
    return jm_vector_get_size(jm_named_ptr)(&td->typeDefinitions);
}

fmi1_xml_variable_typedef_t* fmi1_xml_get_typedef(fmi1_xml_type_definitions_t* td, unsigned int  index) {
    if(index >= fmi1_xml_get_type_definition_number(td)) return 0;
    return (fmi1_xml_variable_typedef_t*)jm_vector_get_item(jm_named_ptr)(&td->typeDefinitions, index).ptr;
}

const char* fmi1_xml_get_type_name(fmi1_xml_variable_typedef_t* t) {   
    return t->typeName;
}

const char* fmi1_xml_get_type_description(fmi1_xml_variable_typedef_t* t) {
	const char * ret = t->description;
    return (ret ? ret : "");
}

fmi1_base_type_enu_t fmi1_xml_get_base_type(fmi1_xml_variable_typedef_t* t) {
    return t->typeBase.baseType;
}

fmi1_xml_real_typedef_t* fmi1_xml_get_type_as_real(fmi1_xml_variable_typedef_t* t) {
    if(fmi1_xml_get_base_type(t) == fmi1_base_type_real) return (fmi1_xml_real_typedef_t*)t;
    return 0;
}
fmi1_xml_integer_typedef_t* fmi1_xml_get_type_as_int(fmi1_xml_variable_typedef_t* t) {
    if(fmi1_xml_get_base_type(t) == fmi1_base_type_int) return (fmi1_xml_integer_typedef_t*)t;
    return 0;
}

fmi1_xml_enumeration_typedef_t* fmi1_xml_get_type_as_enum(fmi1_xml_variable_typedef_t* t) {
    if(fmi1_xml_get_base_type(t) == fmi1_base_type_enum) return (fmi1_xml_enumeration_typedef_t*)t;
    return 0;
}

/* Note that 0-pointer is returned for strings and booleans, empty string quantity if not defined*/
const char* fmi1_xml_get_type_quantity(fmi1_xml_variable_typedef_t* t) {
    fmi1_xml_variable_type_base_t* props = t->typeBase.baseTypeStruct;
	const char * ret;
    if(props->structKind != fmi1_xml_type_struct_enu_props) return 0;
    switch(props->baseType) {
    case fmi1_base_type_real:
        ret = ((fmi1_xml_real_type_props_t*)props)->quantity;
		break;
    case fmi1_base_type_int:
        ret = ((fmi1_xml_integer_type_props_t*)props)->quantity;
		break;
    case fmi1_base_type_bool:
        return 0;
    case fmi1_base_type_str:
        return 0;
    case fmi1_base_type_enum:
        ret = ((fmi1_xml_enum_type_props_t*)props)->quantity;
		break;
    default:
        return 0;
    }
	return (ret ? ret : 0);
}

double fmi1_xml_get_real_type_min(fmi1_xml_real_typedef_t* t) {
    fmi1_xml_variable_typedef_t* vt = (void*)t;
    fmi1_xml_real_type_props_t* props = (fmi1_xml_real_type_props_t*)(vt->typeBase.baseTypeStruct);
    return props->typeMin;
}

double fmi1_xml_get_real_type_max(fmi1_xml_real_typedef_t* t) {
    fmi1_xml_variable_typedef_t* vt = (void*)t;
    fmi1_xml_real_type_props_t* props = (fmi1_xml_real_type_props_t*)(vt->typeBase.baseTypeStruct);
    return props->typeMax;
}

double fmi1_xml_get_real_type_nominal(fmi1_xml_real_typedef_t* t) {
    fmi1_xml_variable_typedef_t* vt = (void*)t;
    fmi1_xml_real_type_props_t* props = (fmi1_xml_real_type_props_t*)(vt->typeBase.baseTypeStruct);
    return props->typeNominal;
}

fmi1_xml_unit_t* fmi1_xml_get_real_type_unit(fmi1_xml_real_typedef_t* t) {    
    fmi1_xml_variable_typedef_t* vt = (void*)t;
    fmi1_xml_real_type_props_t* props = (fmi1_xml_real_type_props_t*)(vt->typeBase.baseTypeStruct);
    fmi1_xml_display_unit_t* du = props->displayUnit;
    if(du) return du->baseUnit;
    return 0;
}

int fmi1_xml_get_real_type_is_relative_quantity(fmi1_xml_real_typedef_t* t) {
    fmi1_xml_variable_typedef_t* vt = (void*)t;
    fmi1_xml_real_type_props_t* props = (fmi1_xml_real_type_props_t*)(vt->typeBase.baseTypeStruct);
    return props->typeBase.relativeQuantity;
}

fmi1_integer_t fmi1_xml_get_integer_type_min(fmi1_xml_integer_typedef_t* t) {
    fmi1_xml_variable_typedef_t* vt = (void*)t;
    fmi1_xml_integer_type_props_t* props = (fmi1_xml_integer_type_props_t*)(vt->typeBase.baseTypeStruct);
    return props->typeMin;
}

fmi1_integer_t fmi1_xml_get_integer_type_max(fmi1_xml_integer_typedef_t* t){
    fmi1_xml_variable_typedef_t* vt = (void*)t;
    fmi1_xml_integer_type_props_t* props = (fmi1_xml_integer_type_props_t*)(vt->typeBase.baseTypeStruct);
    return props->typeMax;
}

int fmi1_xml_get_enum_type_min(fmi1_xml_enumeration_typedef_t* t){
    fmi1_xml_variable_typedef_t* vt = (void*)t;
    fmi1_xml_enum_type_props_t* props = (fmi1_xml_enum_type_props_t*)(vt->typeBase.baseTypeStruct);
    return props->typeMin;
}

int fmi1_xml_get_enum_type_max(fmi1_xml_enumeration_typedef_t* t){
    fmi1_xml_variable_typedef_t* vt = (void*)t;
    fmi1_xml_enum_type_props_t* props = (fmi1_xml_enum_type_props_t*)(vt->typeBase.baseTypeStruct);
    return props->typeMax;
}

unsigned int  fmi1_xml_get_enum_type_size(fmi1_xml_enumeration_typedef_t* t) {
    fmi1_xml_variable_typedef_t* vt = (void*)t;
    fmi1_xml_enum_type_props_t* props = (fmi1_xml_enum_type_props_t*)(vt->typeBase.baseTypeStruct);
    return (unsigned int)jm_vector_get_size(jm_named_ptr)(&props->enumItems);
}

const char* fmi1_xml_get_enum_type_item_name(fmi1_xml_enumeration_typedef_t* t, unsigned int  item) {
    fmi1_xml_variable_typedef_t* vt = (fmi1_xml_variable_typedef_t*)(void*)t;
    fmi1_xml_enum_type_props_t* props = (fmi1_xml_enum_type_props_t*)(vt->typeBase.baseTypeStruct);
    if((item == 0) || (item > fmi1_xml_get_enum_type_size(t) )) return  0;
    return jm_vector_get_item(jm_named_ptr)(&props->enumItems,item-1).name;
}

const char* fmi1_xml_get_enum_type_item_description(fmi1_xml_enumeration_typedef_t* t, unsigned int  item){
    fmi1_xml_variable_typedef_t* vt = (void*)t;
    fmi1_xml_enum_type_props_t* props = (fmi1_xml_enum_type_props_t*)(vt->typeBase.baseTypeStruct);
    fmi1_xml_enum_type_item_t* e;
    if((item == 0) || (item > fmi1_xml_get_enum_type_size(t) )) return  0;
    e = jm_vector_get_item(jm_named_ptr)(&props->enumItems,item-1).ptr;
    return e->itemDesciption;
}

void fmi1_xml_init_variable_type_base(fmi1_xml_variable_type_base_t* type, fmi1_xml_type_struct_kind_enu_t kind, fmi1_base_type_enu_t baseType) {
    type->baseTypeStruct = 0;
    type->next = 0;
    type->structKind = kind;
    type->baseType = baseType;
    type->relativeQuantity = 0;
    type->isFixed = 0;
}

void fmi1_xml_init_real_type_properties(fmi1_xml_real_type_props_t* type) {
    fmi1_xml_init_variable_type_base(&type->typeBase, fmi1_xml_type_struct_enu_props,fmi1_base_type_real);
    type->quantity = 0;    
    type->typeMin = -DBL_MAX;
    type->typeMax = DBL_MAX;
    type->typeNominal = 1.0;
    type->displayUnit = 0;
}

void fmi1_xml_init_integer_type_properties(fmi1_xml_integer_type_props_t* type) {
    fmi1_xml_init_variable_type_base(&type->typeBase, fmi1_xml_type_struct_enu_props,fmi1_base_type_int);
    type->quantity = 0;
    type->typeMin = INT_MIN;
    type->typeMax = INT_MAX;
}

void fmi1_xml_init_enumeration_type_properties(fmi1_xml_enum_type_props_t* type, jm_callbacks* cb) {
    fmi1_xml_init_variable_type_base(&type->typeBase, fmi1_xml_type_struct_enu_props,fmi1_base_type_enum);
    type->quantity = 0;
    type->typeMin = 0;
    type->typeMax = INT_MAX;
    jm_vector_init(jm_named_ptr)(&type->enumItems,0,cb);
}

void fmi1_xml_free_enumeration_type_props(fmi1_xml_enum_type_props_t* type) {
    jm_named_vector_free_data(&type->enumItems);
}


void fmi1_xml_init_type_definitions(fmi1_xml_type_definitions_t* td, jm_callbacks* cb) {
    jm_vector_init(jm_named_ptr)(&td->typeDefinitions,0,cb);

    jm_vector_init(jm_string)(&td->quantities, 0, cb);

    fmi1_xml_init_real_type_properties(&td->defaultRealType);
    td->defaultRealType.typeBase.structKind = fmi1_xml_type_struct_enu_base;
    fmi1_xml_init_enumeration_type_properties(&td->defaultEnumType,cb);
    td->defaultEnumType.typeBase.structKind = fmi1_xml_type_struct_enu_base;
    fmi1_xml_init_integer_type_properties(&td->defaultIntegerType);
    td->defaultIntegerType.typeBase.structKind = fmi1_xml_type_struct_enu_base;

    fmi1_xml_init_variable_type_base(&td->defaultBooleanType, fmi1_xml_type_struct_enu_base,fmi1_base_type_bool);
    fmi1_xml_init_variable_type_base(&td->defaultStringType, fmi1_xml_type_struct_enu_base,fmi1_base_type_str);

    td->typePropsList = 0;
}

void fmi1_xml_free_type_definitions_data(fmi1_xml_type_definitions_t* td) {
    jm_callbacks* cb = td->typeDefinitions.callbacks;

    jm_vector_foreach(jm_string)(&td->quantities,(void(*)(const char*))cb->free);
    jm_vector_free_data(jm_string)(&td->quantities);

    {
        fmi1_xml_variable_type_base_t* next;
        fmi1_xml_variable_type_base_t* cur = td->typePropsList;
        while(cur) {
            next = cur->next;
            if((cur->baseType == fmi1_base_type_enum) && (cur->structKind == fmi1_xml_type_struct_enu_props)) {
                fmi1_xml_enum_type_props_t* props = (fmi1_xml_enum_type_props_t*)cur;
                fmi1_xml_free_enumeration_type_props(props);
            }
            cb->free(cur);
            cur = next;
        }
		td->typePropsList = 0;
    }

    jm_named_vector_free_data(&td->typeDefinitions);
}

int fmi1_xml_handle_TypeDefinitions(fmi1_xml_parser_context_t *context, const char* data) {
    if(!data) {
		jm_log_verbose(context->callbacks, module, "Parsing XML element TypeDefinitions");
    }
    else {
        fmi1_xml_type_definitions_t* defs =  &context->modelDescription->typeDefinitions;

        jm_vector_qsort(jm_named_ptr)(&defs->typeDefinitions, jm_compare_named);
        /* might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi1_xml_handle_Type(fmi1_xml_parser_context_t *context, const char* data) {
    if(!data) {
            fmi1_xml_model_description_t* md = context->modelDescription;
            fmi1_xml_type_definitions_t* td = &md->typeDefinitions;
            jm_named_ptr named, *pnamed;
            jm_vector(char)* bufName = fmi1_xml_reserve_parse_buffer(context,1,100);
            jm_vector(char)* bufDescr = fmi1_xml_reserve_parse_buffer(context,2,100);

            if(!bufName || !bufDescr) return -1;
            if(
            /*  <xs:attribute name="name" type="xs:normalizedString" use="required"/> */
                fmi1_xml_set_attr_string(context, fmi1_xml_elmID_Type, fmi_attr_id_name, 1, bufName) ||
            /* <xs:attribute name="description" type="xs:string"/> */
                fmi1_xml_set_attr_string(context, fmi1_xml_elmID_Type, fmi_attr_id_description, 0, bufDescr)
                    ) return -1;
            named.ptr = 0;
			named.name = 0;
            pnamed = jm_vector_push_back(jm_named_ptr)(&td->typeDefinitions,named);
            if(pnamed) {
                fmi1_xml_variable_typedef_t dummy;
                *pnamed = named = jm_named_alloc_v(bufName, sizeof(fmi1_xml_variable_typedef_t), dummy.typeName - (char*)&dummy,  context->callbacks);
            }
            if(!pnamed || !named.ptr) {
                fmi1_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            else {
                fmi1_xml_variable_typedef_t* type = named.ptr;
                fmi1_xml_init_variable_type_base(&type->typeBase,fmi1_xml_type_struct_enu_typedef,fmi1_base_type_real);
                if(jm_vector_get_size(char)(bufDescr)) {
                    const char* description = jm_string_set_put(&md->descriptions, jm_vector_get_itemp(char)(bufDescr,0));
                    type->description = description;
                }
                else type->description = "";
            }
    }
    else {
        jm_named_ptr named = jm_vector_get_last(jm_named_ptr)(&(context->modelDescription->typeDefinitions.typeDefinitions));
        fmi1_xml_variable_typedef_t* type = named.ptr;
        if(type->typeBase.baseTypeStruct == 0) {
            fmi1_xml_parse_fatal(context, "No specific type given for type definition %s", type->typeName);
            return -1;
        }
        /* might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi1_check_last_elem_is_specific_type(fmi1_xml_parser_context_t *context) {
    if (
                (context->lastElmID == fmi1_xml_elmID_RealType)  ||
                (context->lastElmID == fmi1_xml_elmID_IntegerType)  ||
                (context->lastElmID == fmi1_xml_elmID_BooleanType)  ||
                (context->lastElmID == fmi1_xml_elmID_StringType)  ||
                (context->lastElmID == fmi1_xml_elmID_EnumerationType)
                ) {
        fmi1_xml_parse_fatal(context, "Multiple definitions for a type are not allowed");
        return -1;
    }
    return 0;
}

fmi1_xml_variable_type_base_t* fmi1_xml_alloc_variable_type_props(fmi1_xml_type_definitions_t* td, fmi1_xml_variable_type_base_t* base, size_t typeSize) {
    jm_callbacks* cb = td->typeDefinitions.callbacks;
    fmi1_xml_variable_type_base_t* type = cb->malloc(typeSize);
    if(!type) return 0;
    fmi1_xml_init_variable_type_base(type,fmi1_xml_type_struct_enu_props,base->baseType);
    type->baseTypeStruct = base;
    type->next = td->typePropsList;
    td->typePropsList = type;
    return type;
}

fmi1_xml_variable_type_base_t* fmi1_xml_alloc_variable_type_start(fmi1_xml_type_definitions_t* td,fmi1_xml_variable_type_base_t* base, size_t typeSize) {
    jm_callbacks* cb = td->typeDefinitions.callbacks;
    fmi1_xml_variable_type_base_t* type = cb->malloc(typeSize);
    if(!type) return 0;
    fmi1_xml_init_variable_type_base(type,fmi1_xml_type_struct_enu_start,base->baseType);
    type->baseTypeStruct = base;
    type->next = td->typePropsList;
    td->typePropsList = type;
    return type;
}


fmi1_xml_real_type_props_t* fmi1_xml_parse_real_type_properties(fmi1_xml_parser_context_t* context, fmi1_xml_elm_enu_t elmID) {
    jm_named_ptr named, *pnamed;
    fmi1_xml_model_description_t* md = context->modelDescription;
    fmi1_xml_real_type_props_t* props;
    const char* quantity = 0;
    int boolBuf;

/*        jm_vector(char)* bufName = fmi_get_parse_buffer(context,1);
    jm_vector(char)* bufDescr = fmi_get_parse_buffer(context,2); */
    jm_vector(char)* bufQuantity = fmi1_xml_reserve_parse_buffer(context,3,100);
    jm_vector(char)* bufUnit = fmi1_xml_reserve_parse_buffer(context,4,100);
    jm_vector(char)* bufDispUnit = fmi1_xml_reserve_parse_buffer(context,5,100);

    props = (fmi1_xml_real_type_props_t*)fmi1_xml_alloc_variable_type_props(&md->typeDefinitions, &md->typeDefinitions.defaultRealType.typeBase, sizeof(fmi1_xml_real_type_props_t));

    if(!bufQuantity || !bufUnit || !bufDispUnit || !props ||
            /* <xs:attribute name="quantity" type="xs:normalizedString"/> */
            fmi1_xml_set_attr_string(context, elmID, fmi_attr_id_quantity, 0, bufQuantity) ||
            /* <xs:attribute name="unit" type="xs:normalizedString"/>  */
            fmi1_xml_set_attr_string(context, elmID, fmi_attr_id_unit, 0, bufUnit) ||
            /* <xs:attribute name="displayUnit" type="xs:normalizedString">  */
            fmi1_xml_set_attr_string(context, elmID, fmi_attr_id_displayUnit, 0, bufDispUnit)
            ) {
        fmi1_xml_parse_fatal(context, "Error parsing real type properties");
        return 0;
    }
    if(jm_vector_get_size(char)(bufQuantity))
        quantity = jm_string_set_put(&md->typeDefinitions.quantities, jm_vector_get_itemp(char)(bufQuantity, 0));

    props->quantity = quantity;
    props->displayUnit = 0;
    if(jm_vector_get_size(char)(bufDispUnit)) {
        named.name = jm_vector_get_itemp(char)(bufDispUnit, 0);
        pnamed = jm_vector_bsearch(jm_named_ptr)(&(md->displayUnitDefinitions), &named, jm_compare_named);
        if(!pnamed) {
            fmi1_xml_parse_fatal(context, "Unknown display unit %s in real type definition", jm_vector_get_itemp(char)(bufDispUnit, 0));
            return 0;
        }
        props->displayUnit = pnamed->ptr;
    }
    else {
        if(jm_vector_get_size(char)(bufUnit)) {
            props->displayUnit = fmi1_xml_get_parsed_unit(context, bufUnit, 1);
        }
    }
    if(    /*    <xs:attribute name="relativeQuantity" type="xs:boolean" default="false"> */
            fmi1_xml_set_attr_boolean(context, elmID, fmi_attr_id_relativeQuantity, 0, &boolBuf, 0) ||
            /* <xs:attribute name="min" type="xs:double"/> */
            fmi1_xml_set_attr_double(context, elmID, fmi_attr_id_min, 0, &props->typeMin, -DBL_MAX) ||
            /* <xs:attribute name="max" type="xs:double"/> */
            fmi1_xml_set_attr_double(context, elmID, fmi_attr_id_max, 0, &props->typeMax, DBL_MAX) ||
            /*  <xs:attribute name="nominal" type="xs:double"/> */
            fmi1_xml_set_attr_double(context, elmID, fmi_attr_id_nominal, 0, &props->typeNominal, 1)
            ) return 0;
    props->typeBase.relativeQuantity = boolBuf;
    return props;
}

int fmi1_xml_handle_RealType(fmi1_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi1_xml_model_description_t* md = context->modelDescription;
        jm_named_ptr named;
        fmi1_xml_variable_typedef_t* type;
        fmi1_xml_real_type_props_t * props;
        if(fmi1_check_last_elem_is_specific_type(context)) return -1;

        props = fmi1_xml_parse_real_type_properties(context, fmi1_xml_elmID_RealType);
        if(!props) return -1;
        named = jm_vector_get_last(jm_named_ptr)(&md->typeDefinitions.typeDefinitions);
        type = named.ptr;
        type->typeBase.baseType = fmi1_base_type_real;
        type->typeBase.baseTypeStruct = &props->typeBase;
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

fmi1_xml_integer_type_props_t * fmi1_xml_parse_integer_type_properties(fmi1_xml_parser_context_t* context, fmi1_xml_elm_enu_t elmID) {

    fmi1_xml_model_description_t* md = context->modelDescription;
    fmi1_xml_integer_type_props_t * props = 0;
    const char* quantity = 0;

    /*        jm_vector(char)* bufName = fmi_get_parse_buffer(context,1);
            jm_vector(char)* bufDescr = fmi_get_parse_buffer(context,2); */
    jm_vector(char)* bufQuantity = fmi1_xml_reserve_parse_buffer(context,3,100);

    props = (fmi1_xml_integer_type_props_t*)fmi1_xml_alloc_variable_type_props(&md->typeDefinitions, &md->typeDefinitions.defaultIntegerType.typeBase, sizeof(fmi1_xml_integer_type_props_t));

    if(!bufQuantity || !props ||
            /* <xs:attribute name="quantity" type="xs:normalizedString"/> */
            fmi1_xml_set_attr_string(context, elmID, fmi_attr_id_quantity, 0, bufQuantity)
            )
        return 0;
    if(jm_vector_get_size(char)(bufQuantity))
        quantity = jm_string_set_put(&md->typeDefinitions.quantities, jm_vector_get_itemp(char)(bufQuantity, 0));

    props->quantity = quantity;

    if(
            /* <xs:attribute name="min" type="xs:int"/> */
            fmi1_xml_set_attr_int(context, elmID, fmi_attr_id_min, 0, &props->typeMin, INT_MIN) ||
            /* <xs:attribute name="max" type="xs:int"/> */
            fmi1_xml_set_attr_int(context, elmID, fmi_attr_id_max, 0, &props->typeMax, INT_MAX)
            ) return 0;
    return props;
}

int fmi1_xml_handle_IntegerType(fmi1_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi1_xml_model_description_t* md = context->modelDescription;
        jm_named_ptr named;
        fmi1_xml_variable_typedef_t* type;
        fmi1_xml_integer_type_props_t * props;
        if(fmi1_check_last_elem_is_specific_type(context)) return -1;

        props = fmi1_xml_parse_integer_type_properties(context, fmi1_xml_elmID_IntegerType);
        if(!props) return -1;
        named = jm_vector_get_last(jm_named_ptr)(&md->typeDefinitions.typeDefinitions);
        type = named.ptr;
        type->typeBase.baseType = fmi1_base_type_int;
        type->typeBase.baseTypeStruct = &props->typeBase;
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}


int fmi1_xml_handle_BooleanType(fmi1_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi1_xml_model_description_t* md = context->modelDescription;
        jm_named_ptr named;
        fmi1_xml_variable_typedef_t* type;
        if(fmi1_check_last_elem_is_specific_type(context)) return -1;

        named = jm_vector_get_last(jm_named_ptr)(&context->modelDescription->typeDefinitions.typeDefinitions);
        type = named.ptr;
        type->typeBase.baseType = fmi1_base_type_bool;
        type->typeBase.baseTypeStruct = &md->typeDefinitions.defaultBooleanType;
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi1_xml_handle_StringType(fmi1_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi1_xml_model_description_t* md = context->modelDescription;
        jm_named_ptr named;
        fmi1_xml_variable_typedef_t* type;
        if(fmi1_check_last_elem_is_specific_type(context)) return -1;

        named = jm_vector_get_last(jm_named_ptr)(&context->modelDescription->typeDefinitions.typeDefinitions);
        type = named.ptr;
        type->typeBase.baseType = fmi1_base_type_str;
        type->typeBase.baseTypeStruct = &md->typeDefinitions.defaultStringType;
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi1_xml_handle_EnumerationType(fmi1_xml_parser_context_t *context, const char* data) {
    if(!data) {
        jm_named_ptr named;
        fmi1_xml_model_description_t* md = context->modelDescription;
        fmi1_xml_enum_type_props_t * props;
        fmi1_xml_variable_typedef_t* type;
        const char * quantity = 0;
        /*        jm_vector(char)* bufName = fmi_get_parse_buffer(context,1);
                jm_vector(char)* bufDescr = fmi_get_parse_buffer(context,2); */
        jm_vector(char)* bufQuantity = fmi1_xml_reserve_parse_buffer(context,3,100);

        if(fmi1_check_last_elem_is_specific_type(context)) return -1;

        props = (fmi1_xml_enum_type_props_t*)fmi1_xml_alloc_variable_type_props(&md->typeDefinitions, &md->typeDefinitions.defaultEnumType.typeBase, sizeof(fmi1_xml_enum_type_props_t));
        if(props) jm_vector_init(jm_named_ptr)(&props->enumItems,0,context->callbacks);
        if(!bufQuantity || !props ||
                /* <xs:attribute name="quantity" type="xs:normalizedString"/> */
                fmi1_xml_set_attr_string(context, fmi1_xml_elmID_IntegerType, fmi_attr_id_quantity, 0, bufQuantity)
                )
            return -1;
        if(jm_vector_get_size(char)(bufQuantity))
            quantity = jm_string_set_put(&md->typeDefinitions.quantities, jm_vector_get_itemp(char)(bufQuantity, 0));

        props->quantity = quantity;



        if(
                /* <xs:attribute name="min" type="xs:int"/> */
                fmi1_xml_set_attr_int(context, fmi1_xml_elmID_EnumerationType, fmi_attr_id_min, 0,  &props->typeMin, 1) ||
                /* <xs:attribute name="max" type="xs:int"/> */
                fmi1_xml_set_attr_int(context, fmi1_xml_elmID_EnumerationType, fmi_attr_id_max, 0, &props->typeMax, INT_MAX)
                ) return -1;
        named = jm_vector_get_last(jm_named_ptr)(&context->modelDescription->typeDefinitions.typeDefinitions);
        type = named.ptr;
        type->typeBase.baseType = fmi1_base_type_enum;
        type->typeBase.baseTypeStruct = &props->typeBase;
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi1_xml_handle_Item(fmi1_xml_parser_context_t *context, const char* data) {
    if(!data) {
        {
            fmi1_xml_model_description_t* md = context->modelDescription;
            jm_vector(char)* bufName = fmi1_xml_reserve_parse_buffer(context,1,100);
            jm_vector(char)* bufDescr = fmi1_xml_reserve_parse_buffer(context,2,100);
            /* this enum item belongs to the last created enum = head of typePropsList */
            fmi1_xml_enum_type_props_t * enumProps = (fmi1_xml_enum_type_props_t*)md->typeDefinitions.typePropsList;
            fmi1_xml_enum_type_item_t * item;
            jm_named_ptr named, *pnamed;
            size_t descrlen;

            assert((enumProps->typeBase.structKind == fmi1_xml_type_struct_enu_props) && (enumProps->typeBase.baseType == fmi1_base_type_enum));

            if(!bufName || !bufDescr ||
            /*  <xs:attribute name="name" type="xs:normalizedString" use="required"/> */
                fmi1_xml_set_attr_string(context, fmi1_xml_elmID_Type, fmi_attr_id_name, 1, bufName) ||
            /* <xs:attribute name="description" type="xs:string"/> */
                fmi1_xml_set_attr_string(context, fmi1_xml_elmID_Type, fmi_attr_id_description, 0, bufDescr)
                    )
                return -1;
            descrlen = jm_vector_get_size(char)(bufDescr);
            named.ptr = 0;
			named.name = 0;
            pnamed = jm_vector_push_back(jm_named_ptr)(&enumProps->enumItems, named);

            if(pnamed) *pnamed = named = jm_named_alloc_v(bufName,sizeof(fmi1_xml_enum_type_item_t)+descrlen+1,sizeof(fmi1_xml_enum_type_item_t)+descrlen,context->callbacks);
            item = named.ptr;
            if( !pnamed || !item ) {
                fmi1_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            item->itemName = named.name;
            if(descrlen)
                memcpy(item->itemDesciption,jm_vector_get_itemp(char)(bufDescr,0), descrlen);
            item->itemDesciption[descrlen] = 0;
        }
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

fmi1_xml_variable_type_base_t* fmi1_get_declared_type(fmi1_xml_parser_context_t *context, fmi1_xml_elm_enu_t elmID, fmi1_xml_variable_type_base_t* defaultType) {
    jm_named_ptr key, *found;
    jm_vector(char)* bufDeclaredType = fmi1_xml_reserve_parse_buffer(context,1, 100);
    /*         <xs:attribute name="declaredType" type="xs:normalizedString"> */
    fmi1_xml_set_attr_string(context, elmID, fmi_attr_id_declaredType, 0, bufDeclaredType);
    if(! jm_vector_get_size(char)(bufDeclaredType) ) return defaultType;
    key.name = jm_vector_get_itemp(char)(bufDeclaredType,0);
    found = jm_vector_bsearch(jm_named_ptr)(&(context->modelDescription->typeDefinitions.typeDefinitions),&key, jm_compare_named);
    if(!found) {
        jm_log_error(context->callbacks, module, "Declared type %s not found in type definitions. Ignoring.", key.name);
        return defaultType;
    }
    else  {
        fmi1_xml_variable_type_base_t* retType = found->ptr;
        if(retType->baseType != defaultType->baseType) {
            jm_log_error(context->callbacks, module, "Declared type %s does not match variable type. Ignoring.", key.name);
            return defaultType;
        }
        return retType;
    }
}
