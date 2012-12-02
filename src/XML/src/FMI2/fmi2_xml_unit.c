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

#include "fmi2_xml_model_description_impl.h"
#include "fmi2_xml_unit_impl.h"

static const char* module = "FMI2XML";

fmi2_xml_unit_t* fmi2_xml_get_unit(fmi2_xml_unit_definitions_t* ud, unsigned int  index) {
    if(index >= fmi2_xml_get_unit_definitions_number(ud)) return 0;
    return jm_vector_get_item(jm_named_ptr)(&ud->definitions, index).ptr;
}

const char* fmi2_xml_get_unit_name(fmi2_xml_unit_t* u) {
    return u->baseUnit;
}

/**
	\brief Get fmi2_SI_base_units_Num SI base units exponents associated with the unit.
*/
const int* fmi2_xml_get_SI_unit_exponents(fmi2_xml_unit_t* u) {
	return u->SI_base_unit_exp;
}

/**
	\brief Get factor to the corresponding SI base units.
*/
double fmi2_xml_get_SI_unit_factor(fmi2_xml_unit_t* u){
	return u->factor;
}


/**
	\brief Get offset to the corresponding SI base units.
*/
double fmi2_xml_get_SI_unit_offset(fmi2_xml_unit_t* u) {
	return u->offset;
}

/**
	\brief Convert a value with respect to the unit to the
	value with respect to the SI base unit.
*/
double fmi2_xml_convert_to_SI_base_unit(double uv, fmi2_xml_unit_t* u) {
	return u->factor * uv + u->offset;
}

/**
	\brief Convert a value with respect to the SI base unit to the
	value with respect to the unit.
*/
double fmi2_xml_convert_from_SI_base_unit(double SIv, fmi2_xml_unit_t* u) {
	return (SIv - u->offset)/u->factor;
}

unsigned int fmi2_xml_get_unit_display_unit_number(fmi2_xml_unit_t* u) {
    return (unsigned int)jm_vector_get_size(jm_voidp)(&u->displayUnits);
}

fmi2_xml_display_unit_t* fmi2_xml_get_unit_display_unit(fmi2_xml_unit_t* u, size_t index) {
    if(index >= fmi2_xml_get_unit_display_unit_number(u)) return 0;
    return jm_vector_get_item(jm_voidp)(&u->displayUnits, index);
}


fmi2_xml_unit_t* fmi2_xml_get_base_unit(fmi2_xml_display_unit_t* du) {
    return du->baseUnit;
}

const char* fmi2_xml_get_display_unit_name(fmi2_xml_display_unit_t* du) {
    return du->displayUnit;
}

double fmi2_xml_get_display_unit_factor(fmi2_xml_display_unit_t* du) {
    return du->factor;
}

double fmi2_xml_get_display_unit_offset(fmi2_xml_display_unit_t* du) {
    return du->offset;
}

double fmi2_xml_convert_to_display_unit(double val , fmi2_xml_display_unit_t* du, int isRelativeQuantity) {
    double gain = fmi2_xml_get_display_unit_factor(du);
    double offset = fmi2_xml_get_display_unit_offset(du);
    if(isRelativeQuantity)
        return val *gain;
    else
        return (val*gain + offset);
}

double fmi2_xml_convert_from_display_unit(double val, fmi2_xml_display_unit_t* du, int isRelativeQuantity) {
    double factor = fmi2_xml_get_display_unit_factor(du);
    double offset = fmi2_xml_get_display_unit_offset(du);
    if(isRelativeQuantity)
        return val/factor;
    else
        return (val - offset)/factor;
}

int fmi2_xml_handle_UnitDefinitions(fmi2_xml_parser_context_t *context, const char* data) {
    fmi2_xml_model_description_t* md = context->modelDescription;
    if(!data) {
 		jm_log_verbose(context->callbacks, module, "Parsing XML element UnitDefinitions");
	}
    else {
        jm_vector_qsort(jm_named_ptr)(&(md->unitDefinitions),jm_compare_named);
        jm_vector_qsort(jm_named_ptr)(&(md->displayUnitDefinitions),jm_compare_named);
        /* might give out a warning if(data[0] != 0) */
    }
    return 0;
}


fmi2_xml_display_unit_t* fmi2_xml_get_parsed_unit(fmi2_xml_parser_context_t *context, jm_vector(char)* name, int sorted) {
    fmi2_xml_unit_t dummy, *unit;
    jm_named_ptr named, *pnamed;
    fmi2_xml_model_description_t* md = context->modelDescription;
	int i;
	if(jm_vector_get_size(char)(name))
		named.name = jm_vector_get_itemp(char)(name,0);
	else
		named.name = "";

	if(sorted)
        pnamed = jm_vector_bsearch(jm_named_ptr)(&(md->unitDefinitions), &named,jm_compare_named);
    else
        pnamed = jm_vector_find(jm_named_ptr)(&(md->unitDefinitions), &named,jm_compare_named);

    if(pnamed) {
        unit = pnamed->ptr;
        return &unit->defaultDisplay;
    }

    named.ptr = 0;
    pnamed = jm_vector_push_back(jm_named_ptr)(&(md->unitDefinitions),named);
    if(pnamed) *pnamed = named = jm_named_alloc_v(name,sizeof(fmi2_xml_unit_t),dummy.baseUnit - (char*)&dummy,context->callbacks);

    if(!pnamed || !named.ptr) {
        fmi2_xml_parse_fatal(context, "Could not allocate memory");
        return 0;
    }

    unit = named.ptr;
	unit->factor = 1.0;
	unit->offset = 0.0;
	for(i = 0; i < fmi2_SI_base_units_Num; i++)
		unit->SI_base_unit_exp[i] = 0;
    unit->defaultDisplay.baseUnit = unit;
    unit->defaultDisplay.offset = 0;
    unit->defaultDisplay.factor = 1.0;
    unit->defaultDisplay.displayUnit[0] = 0;
    jm_vector_init(jm_voidp)(&(unit->displayUnits),0,context->callbacks);

    if(sorted) jm_vector_qsort_jm_named_ptr(&(md->unitDefinitions), jm_compare_named);
    return &unit->defaultDisplay;
}

int fmi2_xml_handle_BaseUnit(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
			int ret;
            /* fmi2_xml_model_description_t* md = context->modelDescription; */
            /* this base unit belongs to the last created base unit */
            fmi2_xml_unit_t* unit = context->lastBaseUnit;

            /* process the attributes */

			/* <xs:attribute name="xxx" type="xs:int" default="0"> */
#define FMI2_PARSE_SI_BASE_UNIT_ENU(c) \
	fmi2_xml_set_attr_int(context, fmi2_xml_elmID_BaseUnit, fmi_attr_id_ ## c,0,&(unit->SI_base_unit_exp[fmi2_SI_base_unit_ ## c]),0) ||
			ret =     	
				FMI2_SI_BASE_UNITS(FMI2_PARSE_SI_BASE_UNIT_ENU) 
				/*  <xs:attribute name="factor" type="xs:double" default="1"/>  */
                fmi2_xml_set_attr_double(context, fmi2_xml_elmID_BaseUnit, fmi_attr_id_factor, 0, &unit->factor, 1)  ||
                /*  <xs:attribute name="offset" type="xs:double" default="0"/>  */
                fmi2_xml_set_attr_double(context, fmi2_xml_elmID_BaseUnit, fmi_attr_id_offset, 0, &unit->offset, 0);

			if(unit->factor == 0) {
				unit->factor = 1.0;
				if(!ret) {
					fmi2_xml_parse_error(context, "Attribute 'factor' cannot be equal to zero");
				}
			}

            return ( ret );
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}



int fmi2_xml_handle_Unit(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
            fmi2_xml_display_unit_t* unit;
            jm_vector(char)* buf = fmi2_xml_reserve_parse_buffer(context,1,100);

            if(!buf) return -1;
            if( 
				/*  <xs:attribute name="name" type="xs:normalizedString" use="required"> */
                fmi2_xml_set_attr_string(context, fmi2_xml_elmID_BaseUnit, fmi_attr_id_name, 1, buf) ||
                !(unit = fmi2_xml_get_parsed_unit(context, buf, 0))
               ) return -1;
            context->lastBaseUnit = unit->baseUnit;
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi2_xml_handle_DisplayUnit(fmi2_xml_parser_context_t *context, const char* data) {
    if(!data) {
			int ret;
            fmi2_xml_model_description_t* md = context->modelDescription;
            jm_vector(char)* buf = fmi2_xml_reserve_parse_buffer(context,1,100);
            /* this display unit belongs to the last created base unit */
            fmi2_xml_unit_t* unit = context->lastBaseUnit;
            fmi2_xml_display_unit_t *dispUnit = 0;
            fmi2_xml_display_unit_t dummyDU;
            jm_named_ptr named, *pnamed;

            if(!buf) return -1;
            /* first read the required name attribute */
            /*  <xs:attribute name="name" type="xs:normalizedString" use="required"/> */
            ret = fmi2_xml_set_attr_string(context, fmi2_xml_elmID_DisplayUnit, fmi_attr_id_name, 1, buf);
            if(ret) return ret;
            /* alloc memory to the correct size and put display unit on the list for the base unit */
            named.ptr = 0;
            pnamed = jm_vector_push_back(jm_named_ptr)(&(md->displayUnitDefinitions),named);
            if(pnamed) *pnamed = jm_named_alloc(jm_vector_get_itemp_char(buf,0),sizeof(fmi2_xml_display_unit_t), dummyDU.displayUnit - (char*)&dummyDU,context->callbacks);
            dispUnit = pnamed->ptr;
            if( !pnamed || !dispUnit ||
                !jm_vector_push_back(jm_voidp)(&unit->displayUnits, dispUnit) ) {
                fmi2_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            dispUnit->baseUnit = unit;
            /* finally process the attributes */
			ret =     
				/*  <xs:attribute name="factor" type="xs:double" default="1"/>  */
                fmi2_xml_set_attr_double(context, fmi2_xml_elmID_DisplayUnit, fmi_attr_id_factor, 0, &dispUnit->factor, 1)  ||
                /*  <xs:attribute name="offset" type="xs:double" default="0"/>  */
                fmi2_xml_set_attr_double(context, fmi2_xml_elmID_DisplayUnit, fmi_attr_id_offset, 0, &dispUnit->offset, 0);
			if(dispUnit->factor == 0) {
				dispUnit->factor = 1.0;
				if(!ret) {
					fmi2_xml_parse_error(context, "Attribute 'factor' cannot be equal to zero");
				}
			}

            return ( ret );
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

