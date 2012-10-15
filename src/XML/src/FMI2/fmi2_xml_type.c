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
#include <JM/jm_named_ptr.h>

#include "fmi2_xml_model_description_impl.h"
#include "fmi2_xml_type_impl.h"
#include "fmi2_xml_unit_impl.h"
#include "fmi2_xml_parser.h"

static const char* module = "FMI2XML";

fmi2_xml_display_unit_t* fmi2_xml_get_type_display_unit(fmi2_xml_real_typedef_t* t) {
    fmi2_xml_variable_typedef_t* vt = (void*)t;
    fmi2_xml_real_type_props_t * props = (fmi2_xml_real_type_props_t*)vt->typeBase.baseTypeStruct;
    fmi2_xml_display_unit_t* du = props->displayUnit;
	if(!du || (&du->baseUnit->defaultDisplay == du)) return 0;
    return du;
}

unsigned int fmi2_xml_get_type_definition_number(fmi2_xml_type_definitions_t* td) {
    return (unsigned int)jm_vector_get_size(jm_named_ptr)(&td->typeDefinitions);
}

fmi2_xml_variable_typedef_t* fmi2_xml_get_typedef(fmi2_xml_type_definitions_t* td, unsigned int  index) {
    if(index >= fmi2_xml_get_type_definition_number(td)) return 0;
    return (fmi2_xml_variable_typedef_t*)jm_vector_get_item(jm_named_ptr)(&td->typeDefinitions, index).ptr;
}

const char* fmi2_xml_get_type_name(fmi2_xml_variable_typedef_t* t) {   
    return t->typeName;
}

const char* fmi2_xml_get_type_description(fmi2_xml_variable_typedef_t* t) {
	const char * ret = t->description;
    return (ret ? ret : "");
}

fmi2_base_type_enu_t fmi2_xml_get_base_type(fmi2_xml_variable_typedef_t* t) {
    return t->typeBase.baseType;
}

fmi2_xml_real_typedef_t* fmi2_xml_get_type_as_real(fmi2_xml_variable_typedef_t* t) {
    if(fmi2_xml_get_base_type(t) == fmi2_base_type_real) return (fmi2_xml_real_typedef_t*)t;
    return 0;
}
fmi2_xml_integer_typedef_t* fmi2_xml_get_type_as_int(fmi2_xml_variable_typedef_t* t) {
    if(fmi2_xml_get_base_type(t) == fmi2_base_type_int) return (fmi2_xml_integer_typedef_t*)t;
    return 0;
}

fmi2_xml_enumeration_typedef_t* fmi2_xml_get_type_as_enum(fmi2_xml_variable_typedef_t* t) {
    if(fmi2_xml_get_base_type(t) == fmi2_base_type_enum) return (fmi2_xml_enumeration_typedef_t*)t;
    return 0;
}

/* Note that 0-pointer is returned for strings and booleans, empty string quantity if not defined*/
const char* fmi2_xml_get_type_quantity(fmi2_xml_variable_typedef_t* t) {
    fmi2_xml_variable_type_base_t* props = t->typeBase.baseTypeStruct;
	const char * ret;
    if(props->structKind != fmi2_xml_type_struct_enu_props) return 0;
    switch(props->baseType) {
    case fmi2_base_type_real:
        ret = ((fmi2_xml_real_type_props_t*)props)->quantity;
		break;
    case fmi2_base_type_int:
        ret = ((fmi2_xml_integer_type_props_t*)props)->quantity;
		break;
    case fmi2_base_type_bool:
        return 0;
    case fmi2_base_type_str:
        return 0;
    case fmi2_base_type_enum:
		ret = ((fmi2_xml_enum_variable_props_t*)props)->quantity;
		break;
    default:
        return 0;
    }
	return (ret ? ret : 0);
}

double fmi2_xml_get_real_type_min(fmi2_xml_real_typedef_t* t) {
    fmi2_xml_variable_typedef_t* vt = (void*)t;
    fmi2_xml_real_type_props_t* props = (fmi2_xml_real_type_props_t*)(vt->typeBase.baseTypeStruct);
    return props->typeMin;
}

double fmi2_xml_get_real_type_max(fmi2_xml_real_typedef_t* t) {
    fmi2_xml_variable_typedef_t* vt = (void*)t;
    fmi2_xml_real_type_props_t* props = (fmi2_xml_real_type_props_t*)(vt->typeBase.baseTypeStruct);
    return props->typeMax;
}

double fmi2_xml_get_real_type_nominal(fmi2_xml_real_typedef_t* t) {
    fmi2_xml_variable_typedef_t* vt = (void*)t;
    fmi2_xml_real_type_props_t* props = (fmi2_xml_real_type_props_t*)(vt->typeBase.baseTypeStruct);
    return props->typeNominal;
}

fmi2_xml_unit_t* fmi2_xml_get_real_type_unit(fmi2_xml_real_typedef_t* t) {    
    fmi2_xml_variable_typedef_t* vt = (void*)t;
    fmi2_xml_real_type_props_t* props = (fmi2_xml_real_type_props_t*)(vt->typeBase.baseTypeStruct);
    fmi2_xml_display_unit_t* du = props->displayUnit;
    if(du) return du->baseUnit;
    return 0;
}

int fmi2_xml_get_real_type_is_relative_quantity(fmi2_xml_real_typedef_t* t) {
    fmi2_xml_variable_typedef_t* vt = (void*)t;
    fmi2_xml_real_type_props_t* props = (fmi2_xml_real_type_props_t*)(vt->typeBase.baseTypeStruct);
	return props->typeBase.isRelativeQuantity;
}

int fmi2_xml_get_real_type_is_unbounded(fmi2_xml_real_typedef_t* t) {
    fmi2_xml_variable_typedef_t* vt = (void*)t;
    fmi2_xml_real_type_props_t* props = (fmi2_xml_real_type_props_t*)(vt->typeBase.baseTypeStruct);
	return props->typeBase.isUnbounded;
}


fmi2_integer_t fmi2_xml_get_integer_type_min(fmi2_xml_integer_typedef_t* t) {
    fmi2_xml_variable_typedef_t* vt = (void*)t;
    fmi2_xml_integer_type_props_t* props = (fmi2_xml_integer_type_props_t*)(vt->typeBase.baseTypeStruct);
    return props->typeMin;
}

fmi2_integer_t fmi2_xml_get_integer_type_max(fmi2_xml_integer_typedef_t* t){
    fmi2_xml_variable_typedef_t* vt = (void*)t;
    fmi2_xml_integer_type_props_t* props = (fmi2_xml_integer_type_props_t*)(vt->typeBase.baseTypeStruct);
    return props->typeMax;
}

int fmi2_xml_get_enum_type_min(fmi2_xml_enumeration_typedef_t* t){
    fmi2_xml_variable_typedef_t* vt = (void*)t;
    fmi2_xml_enum_typedef_props_t* props = (fmi2_xml_enum_typedef_props_t*)(vt->typeBase.baseTypeStruct);
	fmi2_xml_enum_type_item_t* item;
	if(jm_vector_get_size(jm_named_ptr)(&props->enumItems) == 0) return 0;
	item = jm_vector_get_item(jm_named_ptr)(&props->enumItems, 0).ptr;
	return item->value;
}

int fmi2_xml_get_enum_type_max(fmi2_xml_enumeration_typedef_t* t){
    fmi2_xml_variable_typedef_t* vt = (void*)t;
    fmi2_xml_enum_typedef_props_t* props = (fmi2_xml_enum_typedef_props_t*)(vt->typeBase.baseTypeStruct);
	jm_named_ptr* itemp = jm_vector_get_lastp(jm_named_ptr)(&props->enumItems);
	fmi2_xml_enum_type_item_t* item;
	if(!itemp) return 0;
	item = itemp->ptr;
	return item->value;
}

unsigned int  fmi2_xml_get_enum_type_size(fmi2_xml_enumeration_typedef_t* t) {
    fmi2_xml_variable_typedef_t* vt = (void*)t;
    fmi2_xml_enum_typedef_props_t* props = (fmi2_xml_enum_typedef_props_t*)(vt->typeBase.baseTypeStruct);
    return (unsigned int)jm_vector_get_size(jm_named_ptr)(&props->enumItems);
}

const char* fmi2_xml_get_enum_type_item_name(fmi2_xml_enumeration_typedef_t* t, unsigned int  item) {
    fmi2_xml_variable_typedef_t* vt = (fmi2_xml_variable_typedef_t*)(void*)t;
    fmi2_xml_enum_typedef_props_t* props = (fmi2_xml_enum_typedef_props_t*)(vt->typeBase.baseTypeStruct);
    if((item == 0) || (item > fmi2_xml_get_enum_type_size(t) )) return  0;
    return jm_vector_get_item(jm_named_ptr)(&props->enumItems,item-1).name;
}

int fmi2_xml_get_enum_type_item_value(fmi2_xml_enumeration_typedef_t* t, unsigned int  item) {
    fmi2_xml_variable_typedef_t* vt = (fmi2_xml_variable_typedef_t*)(void*)t;
    fmi2_xml_enum_typedef_props_t* props = (fmi2_xml_enum_typedef_props_t*)(vt->typeBase.baseTypeStruct);
	fmi2_xml_enum_type_item_t* eitem;
    if((item == 0) || (item > fmi2_xml_get_enum_type_size(t) )) return  0;
	eitem = jm_vector_get_item(jm_named_ptr)(&props->enumItems,item-1).ptr;
    return eitem->value;
}

const char* fmi2_xml_get_enum_type_value_name(fmi2_xml_enumeration_typedef_t* t, int value) {
    fmi2_xml_variable_typedef_t* vt = (fmi2_xml_variable_typedef_t*)(void*)t;
    fmi2_xml_enum_typedef_props_t* props = (fmi2_xml_enum_typedef_props_t*)(vt->typeBase.baseTypeStruct);
	fmi2_xml_enum_type_item_t keyitem;
	jm_named_ptr* itemp, key;
	keyitem.value = value;
	key.ptr = &keyitem;
	itemp = jm_vector_bsearch(jm_named_ptr)(&props->enumItems, &key, fmi1_xml_compare_enum_val);
    if(!itemp) return  0;
	return itemp->name;
}


const char* fmi2_xml_get_enum_type_item_description(fmi2_xml_enumeration_typedef_t* t, unsigned int  item){
    fmi2_xml_variable_typedef_t* vt = (void*)t;
    fmi2_xml_enum_typedef_props_t* props = (fmi2_xml_enum_typedef_props_t*)(vt->typeBase.baseTypeStruct);
    fmi2_xml_enum_type_item_t* e;
    if(item > fmi2_xml_get_enum_type_size(t) ) return  0;
    e = jm_vector_get_item(jm_named_ptr)(&props->enumItems,item-1).ptr;
    return e->itemDesciption;
}

void fmi2_xml_init_variable_type_base(fmi2_xml_variable_type_base_t* type, fmi2_xml_type_struct_kind_enu_t kind, fmi2_base_type_enu_t baseType) {
    type->baseTypeStruct = 0;
    type->next = 0;
    type->structKind = kind;
    type->baseType = baseType;
	type->isRelativeQuantity = 0;
	type->isUnbounded = 0;
}

void fmi2_xml_init_real_type_properties(fmi2_xml_real_type_props_t* type) {
    fmi2_xml_init_variable_type_base(&type->typeBase, fmi2_xml_type_struct_enu_props,fmi2_base_type_real);
    type->quantity = 0;    
    type->typeMin = -DBL_MAX;
    type->typeMax = DBL_MAX;
    type->typeNominal = 1.0;
    type->displayUnit = 0;
}

void fmi2_xml_init_integer_type_properties(fmi2_xml_integer_type_props_t* type) {
    fmi2_xml_init_variable_type_base(&type->typeBase, fmi2_xml_type_struct_enu_props,fmi2_base_type_int);
    type->quantity = 0;
    type->typeMin = INT_MIN;
    type->typeMax = INT_MAX;
}

void fmi2_xml_init_enumeration_variable_properties(fmi2_xml_enum_variable_props_t* type, jm_callbacks* cb) {
    fmi2_xml_init_variable_type_base(&type->typeBase, fmi2_xml_type_struct_enu_props,fmi2_base_type_enum);
	type->quantity = 0;
    type->typeMin = 0;
    type->typeMax = 0;
}

void fmi2_xml_init_enumeration_type_properties(fmi2_xml_enum_typedef_props_t* type, jm_callbacks* cb) {
	fmi2_xml_init_enumeration_variable_properties(&type->base, cb);
    jm_vector_init(jm_named_ptr)(&type->enumItems,0,cb);
}

void fmi2_xml_free_enumeration_type_props(fmi2_xml_enum_typedef_props_t* type) {
    jm_named_vector_free_data(&type->enumItems);
}

void fmi2_xml_init_type_definitions(fmi2_xml_type_definitions_t* td, jm_callbacks* cb) {
    jm_vector_init(jm_named_ptr)(&td->typeDefinitions,0,cb);

    jm_vector_init(jm_string)(&td->quantities, 0, cb);

    fmi2_xml_init_real_type_properties(&td->defaultRealType);
    fmi2_xml_init_enumeration_type_properties(&td->defaultEnumType,cb);
    fmi2_xml_init_integer_type_properties(&td->defaultIntegerType);

    fmi2_xml_init_variable_type_base(&td->defaultBooleanType, fmi2_xml_type_struct_enu_props,fmi2_base_type_bool);
    fmi2_xml_init_variable_type_base(&td->defaultStringType, fmi2_xml_type_struct_enu_props,fmi2_base_type_str);

    td->typePropsList = 0;
}

void fmi2_xml_free_type_definitions_data(fmi2_xml_type_definitions_t* td) {
    jm_callbacks* cb = td->typeDefinitions.callbacks;

    jm_vector_foreach(jm_string)(&td->quantities,(void(*)(const char*))cb->free);
    jm_vector_free_data(jm_string)(&td->quantities);

    {
        fmi2_xml_variable_type_base_t* next;
        fmi2_xml_variable_type_base_t* cur = td->typePropsList;
        while(cur) {
            next = cur->next;
            if(    (cur->baseType == fmi2_base_type_enum) 
				&& (cur->structKind == fmi2_xml_type_struct_enu_props)
				&& (cur->baseTypeStruct == 0)
				) {
                fmi2_xml_enum_typedef_props_t* props = (fmi2_xml_enum_typedef_props_t*)cur;
                fmi2_xml_free_enumeration_type_props(props);
            }
            cb->free(cur);
            cur = next;
        }
		td->typePropsList = 0;
    }

    jm_named_vector_free_data(&td->typeDefinitions);
}

int fmi2_xml_handle_TypeDefinitions(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
		jm_log_verbose(context->callbacks, module, "Parsing XML element TypeDefinitions");
    }
    else {
        fmi2_xml_type_definitions_t* defs =  &context->modelDescription->typeDefinitions;

        jm_vector_qsort(jm_named_ptr)(&defs->typeDefinitions, jm_compare_named);
        /* might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi2_xml_handle_SimpleType(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
            fmi2_xml_model_description_t* md = context->modelDescription;
            fmi2_xml_type_definitions_t* td = &md->typeDefinitions;
            jm_named_ptr named, *pnamed;
            jm_vector(char)* bufName = fmi2_xml_reserve_parse_buffer(context,1,100);
            jm_vector(char)* bufDescr = fmi2_xml_reserve_parse_buffer(context,2,100);

            if(!bufName || !bufDescr) return -1;
            if(
            /*  <xs:attribute name="name" type="xs:normalizedString" use="required"/> */
                fmi2_xml_set_attr_string(context, fmi2_xml_elmID_SimpleType, fmi_attr_id_name, 1, bufName) ||
            /* <xs:attribute name="description" type="xs:string"/> */
                fmi2_xml_set_attr_string(context, fmi2_xml_elmID_SimpleType, fmi_attr_id_description, 0, bufDescr)
                    ) return -1;
            named.ptr = 0;
			named.name = 0;
            pnamed = jm_vector_push_back(jm_named_ptr)(&td->typeDefinitions,named);
            if(pnamed) {
                fmi2_xml_variable_typedef_t dummy;
                *pnamed = named = jm_named_alloc_v(bufName, sizeof(fmi2_xml_variable_typedef_t), dummy.typeName - (char*)&dummy,  context->callbacks);
            }
            if(!pnamed || !named.ptr) {
                fmi2_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            else {
                fmi2_xml_variable_typedef_t* type = named.ptr;
                fmi2_xml_init_variable_type_base(&type->typeBase,fmi2_xml_type_struct_enu_typedef,fmi2_base_type_real);
                if(jm_vector_get_size(char)(bufDescr)) {
                    const char* description = jm_string_set_put(&md->descriptions, jm_vector_get_itemp(char)(bufDescr,0));
                    type->description = description;
                }
                else type->description = "";
            }
    }
    else {
        jm_named_ptr named = jm_vector_get_last(jm_named_ptr)(&(context->modelDescription->typeDefinitions.typeDefinitions));
        fmi2_xml_variable_typedef_t* type = named.ptr;
        if(type->typeBase.baseTypeStruct == 0) {
            fmi2_xml_parse_fatal(context, "No specific type given for type definition %s", type->typeName);
            return -1;
        }
        /* might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

fmi2_xml_variable_type_base_t* fmi2_xml_alloc_variable_type_props(fmi2_xml_type_definitions_t* td, fmi2_xml_variable_type_base_t* base, size_t typeSize) {
    jm_callbacks* cb = td->typeDefinitions.callbacks;
    fmi2_xml_variable_type_base_t* type = cb->malloc(typeSize);
    if(!type) return 0;
    fmi2_xml_init_variable_type_base(type,fmi2_xml_type_struct_enu_props,base->baseType);
    type->baseTypeStruct = base;
    type->next = td->typePropsList;
    td->typePropsList = type;
    return type;
}

fmi2_xml_variable_type_base_t* fmi2_xml_alloc_variable_type_start(fmi2_xml_type_definitions_t* td,fmi2_xml_variable_type_base_t* base, size_t typeSize) {
    jm_callbacks* cb = td->typeDefinitions.callbacks;
    fmi2_xml_variable_type_base_t* type = cb->malloc(typeSize);
    if(!type) return 0;
    fmi2_xml_init_variable_type_base(type,fmi2_xml_type_struct_enu_start,base->baseType);
    type->baseTypeStruct = base;
    type->next = td->typePropsList;
    td->typePropsList = type;
    return type;
}


fmi2_xml_real_type_props_t* fmi2_xml_parse_real_type_properties(fmi2_xml_parser_context_t* context, fmi2_xml_elm_enu_t elmID) {
    jm_named_ptr named, *pnamed;
    fmi2_xml_model_description_t* md = context->modelDescription;
    fmi2_xml_real_type_props_t* props;
    const char* quantity = 0;
    unsigned int relQuanBuf, unboundedBuf;

/*        jm_vector(char)* bufName = fmi_get_parse_buffer(context,1);
    jm_vector(char)* bufDescr = fmi_get_parse_buffer(context,2); */
    jm_vector(char)* bufQuantity = fmi2_xml_reserve_parse_buffer(context,3,100);
    jm_vector(char)* bufUnit = fmi2_xml_reserve_parse_buffer(context,4,100);
    jm_vector(char)* bufDispUnit = fmi2_xml_reserve_parse_buffer(context,5,100);

    props = (fmi2_xml_real_type_props_t*)fmi2_xml_alloc_variable_type_props(&md->typeDefinitions, &md->typeDefinitions.defaultRealType.typeBase, sizeof(fmi2_xml_real_type_props_t));

    if(!bufQuantity || !bufUnit || !bufDispUnit || !props ||
            /* <xs:attribute name="quantity" type="xs:normalizedString"/> */
            fmi2_xml_set_attr_string(context, elmID, fmi_attr_id_quantity, 0, bufQuantity) ||
            /* <xs:attribute name="unit" type="xs:normalizedString"/>  */
            fmi2_xml_set_attr_string(context, elmID, fmi_attr_id_unit, 0, bufUnit) ||
            /* <xs:attribute name="displayUnit" type="xs:normalizedString">  */
            fmi2_xml_set_attr_string(context, elmID, fmi_attr_id_displayUnit, 0, bufDispUnit)
            ) {
        fmi2_xml_parse_fatal(context, "Error parsing real type properties");
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
            fmi2_xml_parse_fatal(context, "Unknown display unit %s in real type definition", jm_vector_get_itemp(char)(bufDispUnit, 0));
            return 0;
        }
        props->displayUnit = pnamed->ptr;
    }
    else {
        if(jm_vector_get_size(char)(bufUnit)) {
            props->displayUnit = fmi2_xml_get_parsed_unit(context, bufUnit, 1);
        }
    }
    if(    /*    <xs:attribute name="relativeQuantity" type="xs:boolean" default="false"> */
            fmi2_xml_set_attr_boolean(context, elmID, fmi_attr_id_relativeQuantity, 0, &relQuanBuf, 0) ||
		    /*    <xs:attribute name="unbounded" type="xs:boolean" default="false"> */
            fmi2_xml_set_attr_boolean(context, elmID, fmi_attr_id_unbounded, 0, &unboundedBuf, 0) ||
            /* <xs:attribute name="min" type="xs:double"/> */
            fmi2_xml_set_attr_double(context, elmID, fmi_attr_id_min, 0, &props->typeMin, -DBL_MAX) ||
            /* <xs:attribute name="max" type="xs:double"/> */
            fmi2_xml_set_attr_double(context, elmID, fmi_attr_id_max, 0, &props->typeMax, DBL_MAX) ||
            /*  <xs:attribute name="nominal" type="xs:double"/> */
            fmi2_xml_set_attr_double(context, elmID, fmi_attr_id_nominal, 0, &props->typeNominal, 1)
            ) return 0;
	props->typeBase.isRelativeQuantity = (relQuanBuf) ? 1:0;
	props->typeBase.isUnbounded = (unboundedBuf) ? 1 : 0;
    return props;
}

int fmi2_xml_handle_Real(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi2_xml_model_description_t* md = context->modelDescription;
        jm_named_ptr named;
        fmi2_xml_variable_typedef_t* type;
        fmi2_xml_real_type_props_t * props;

        props = fmi2_xml_parse_real_type_properties(context, fmi2_xml_elmID_Real);
        if(!props) return -1;
        named = jm_vector_get_last(jm_named_ptr)(&md->typeDefinitions.typeDefinitions);
        type = named.ptr;
        type->typeBase.baseType = fmi2_base_type_real;
        type->typeBase.baseTypeStruct = &props->typeBase;
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

fmi2_xml_integer_type_props_t * fmi2_xml_parse_integer_type_properties(fmi2_xml_parser_context_t* context, fmi2_xml_elm_enu_t elmID) {

    fmi2_xml_model_description_t* md = context->modelDescription;
    fmi2_xml_integer_type_props_t * props = 0;
    const char* quantity = 0;

    /*        jm_vector(char)* bufName = fmi_get_parse_buffer(context,1);
            jm_vector(char)* bufDescr = fmi_get_parse_buffer(context,2); */
    jm_vector(char)* bufQuantity = fmi2_xml_reserve_parse_buffer(context,3,100);

    props = (fmi2_xml_integer_type_props_t*)fmi2_xml_alloc_variable_type_props(&md->typeDefinitions, &md->typeDefinitions.defaultIntegerType.typeBase, sizeof(fmi2_xml_integer_type_props_t));

    if(!bufQuantity || !props ||
            /* <xs:attribute name="quantity" type="xs:normalizedString"/> */
            fmi2_xml_set_attr_string(context, elmID, fmi_attr_id_quantity, 0, bufQuantity)
            )
        return 0;
    if(jm_vector_get_size(char)(bufQuantity))
        quantity = jm_string_set_put(&md->typeDefinitions.quantities, jm_vector_get_itemp(char)(bufQuantity, 0));

    props->quantity = quantity;

    if(
            /* <xs:attribute name="min" type="xs:int"/> */
            fmi2_xml_set_attr_int(context, elmID, fmi_attr_id_min, 0, &props->typeMin, INT_MIN) ||
            /* <xs:attribute name="max" type="xs:int"/> */
            fmi2_xml_set_attr_int(context, elmID, fmi_attr_id_max, 0, &props->typeMax, INT_MAX)
            ) return 0;
    return props;
}

int fmi2_xml_handle_Integer(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi2_xml_model_description_t* md = context->modelDescription;
        jm_named_ptr named;
        fmi2_xml_variable_typedef_t* type;
        fmi2_xml_integer_type_props_t * props;

        props = fmi2_xml_parse_integer_type_properties(context, fmi2_xml_elmID_Integer);
        if(!props) return -1;
        named = jm_vector_get_last(jm_named_ptr)(&md->typeDefinitions.typeDefinitions);
        type = named.ptr;
        type->typeBase.baseType = fmi2_base_type_int;
        type->typeBase.baseTypeStruct = &props->typeBase;
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}


int fmi2_xml_handle_Boolean(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi2_xml_model_description_t* md = context->modelDescription;
        jm_named_ptr named;
        fmi2_xml_variable_typedef_t* type;

        named = jm_vector_get_last(jm_named_ptr)(&context->modelDescription->typeDefinitions.typeDefinitions);
        type = named.ptr;
        type->typeBase.baseType = fmi2_base_type_bool;
        type->typeBase.baseTypeStruct = &md->typeDefinitions.defaultBooleanType;
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi2_xml_handle_String(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
        fmi2_xml_model_description_t* md = context->modelDescription;
        jm_named_ptr named;
        fmi2_xml_variable_typedef_t* type;

        named = jm_vector_get_last(jm_named_ptr)(&context->modelDescription->typeDefinitions.typeDefinitions);
        type = named.ptr;
        type->typeBase.baseType = fmi2_base_type_str;
        type->typeBase.baseTypeStruct = &md->typeDefinitions.defaultStringType;
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi2_xml_handle_Enumeration(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
        jm_named_ptr named;
        fmi2_xml_model_description_t* md = context->modelDescription;
        fmi2_xml_enum_typedef_props_t * props;
        fmi2_xml_variable_typedef_t* type;
        const char * quantity = 0;
        jm_vector(char)* bufQuantity = fmi2_xml_reserve_parse_buffer(context,3,100);

        props = (fmi2_xml_enum_typedef_props_t*)fmi2_xml_alloc_variable_type_props(
													&md->typeDefinitions, 
													&md->typeDefinitions.defaultEnumType.base.typeBase,
													sizeof(fmi2_xml_enum_typedef_props_t));
        props->base.typeBase.baseTypeStruct = 0;
        if(props) jm_vector_init(jm_named_ptr)(&props->enumItems,0,context->callbacks);
        if(!bufQuantity || !props ||
                /* <xs:attribute name="quantity" type="xs:normalizedString"/> */
                fmi2_xml_set_attr_string(context, fmi2_xml_elmID_Integer, fmi_attr_id_quantity, 0, bufQuantity)
                )
            return -1;
        if(jm_vector_get_size(char)(bufQuantity))
            quantity = jm_string_set_put(&md->typeDefinitions.quantities, jm_vector_get_itemp(char)(bufQuantity, 0));

        props->base.quantity = quantity;

        named = jm_vector_get_last(jm_named_ptr)(&context->modelDescription->typeDefinitions.typeDefinitions);
        type = named.ptr;
        type->typeBase.baseType = fmi2_base_type_enum;
        type->typeBase.baseTypeStruct = &props->base.typeBase;
    }
    else {
		/* sort enum items, check that there are no duplicates */
		jm_named_ptr named = jm_vector_get_last(jm_named_ptr)(&context->modelDescription->typeDefinitions.typeDefinitions);
		fmi2_xml_variable_typedef_t* type = named.ptr;
		fmi2_xml_enum_typedef_props_t * props = (fmi2_xml_enum_typedef_props_t *)type->typeBase.baseTypeStruct;
		jm_vector(jm_named_ptr)* items = &props->enumItems;
		size_t i, n = jm_vector_get_size(jm_named_ptr)(items);
		jm_vector_qsort(jm_named_ptr)(items, fmi1_xml_compare_enum_val);
		for(i = 1; i < n; i++) {
			fmi2_xml_enum_type_item_t* a = jm_vector_get_itemp(jm_named_ptr)(items, i-1)->ptr;
			fmi2_xml_enum_type_item_t* b = jm_vector_get_itemp(jm_named_ptr)(items, i)->ptr;
			if(a->value == b->value) {
				jm_log_error(context->callbacks, module, "Enum items '%s' and '%s' within enumeration '%s' have the same value %d", 
					a->itemName, b->itemName, type->typeName, a->value);
			}
		}
        return 0;
    }
    return 0;
}

int fmi2_xml_handle_Item(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
        {
            fmi2_xml_model_description_t* md = context->modelDescription;
            jm_vector(char)* bufName = fmi2_xml_reserve_parse_buffer(context,1,100);
            jm_vector(char)* bufDescr = fmi2_xml_reserve_parse_buffer(context,2,100);
            /* this enum item belongs to the last created enum = head of typePropsList */
            fmi2_xml_enum_typedef_props_t * enumProps = (fmi2_xml_enum_typedef_props_t*)md->typeDefinitions.typePropsList;
            fmi2_xml_enum_type_item_t * item;
            jm_named_ptr named, *pnamed;
            size_t descrlen;
			int value;

            assert((enumProps->base.typeBase.structKind == fmi2_xml_type_struct_enu_props) 
				&& (enumProps->base.typeBase.baseType == fmi2_base_type_enum));

            if(!bufName || !bufDescr ||
            /*  <xs:attribute name="name" type="xs:normalizedString" use="required"/> */
                fmi2_xml_set_attr_string(context, fmi2_xml_elmID_Item, fmi_attr_id_name, 1, bufName) ||
            /* <xs:attribute name="description" type="xs:string"/> */
                fmi2_xml_set_attr_string(context, fmi2_xml_elmID_Item, fmi_attr_id_description, 0, bufDescr) ||
            /* <xs:attribute name="value" type="xs:int" use="required"> */
				fmi2_xml_set_attr_int(context, fmi2_xml_elmID_Item, fmi_attr_id_value, 1, &value, 0)
                    )
                return -1;
            descrlen = jm_vector_get_size(char)(bufDescr);
            named.ptr = 0;
			named.name = 0;
            pnamed = jm_vector_push_back(jm_named_ptr)(&enumProps->enumItems, named);

            if(pnamed) *pnamed = named = jm_named_alloc_v(bufName,sizeof(fmi2_xml_enum_type_item_t)+descrlen+1,sizeof(fmi2_xml_enum_type_item_t)+descrlen,context->callbacks);
            item = named.ptr;
            if( !pnamed || !item ) {
                fmi2_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            item->itemName = named.name;
			item->value = value;
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

fmi2_xml_variable_type_base_t* fmi2_get_declared_type(fmi2_xml_parser_context_t *context, fmi2_xml_elm_enu_t elmID, fmi2_xml_variable_type_base_t* defaultType) {
    jm_named_ptr key, *found;
    jm_vector(char)* bufDeclaredType = fmi2_xml_reserve_parse_buffer(context,1, 100);
    /*         <xs:attribute name="declaredType" type="xs:normalizedString"> */
    fmi2_xml_set_attr_string(context, elmID, fmi_attr_id_declaredType, 0, bufDeclaredType);
    if(! jm_vector_get_size(char)(bufDeclaredType) ) return defaultType;
    key.name = jm_vector_get_itemp(char)(bufDeclaredType,0);
    found = jm_vector_bsearch(jm_named_ptr)(&(context->modelDescription->typeDefinitions.typeDefinitions),&key, jm_compare_named);
    if(!found) {
        jm_log_error(context->callbacks, module, "Declared type %s not found in type definitions. Ignoring.", key.name);
        return defaultType;
    }
    else  {
        fmi2_xml_variable_type_base_t* retType = found->ptr;
        if(retType->baseType != defaultType->baseType) {
            jm_log_error(context->callbacks, module, "Declared type %s does not match variable type. Ignoring.", key.name);
            return defaultType;
        }
        return retType;
    }
}
