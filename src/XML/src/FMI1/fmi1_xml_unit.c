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

#include "fmi1_xml_model_description_impl.h"
#include "fmi1_xml_unit_impl.h"

static const char* module = "FMI1XML";

fmi1_xml_unit_t* fmi1_xml_get_unit(fmi1_xml_unit_definitions_t* ud, unsigned int  index) {
    if(index >= fmi1_xml_get_unit_definitions_number(ud)) return 0;
    return jm_vector_get_item(jm_named_ptr)(&ud->definitions, index).ptr;
}

const char* fmi1_xml_get_unit_name(fmi1_xml_unit_t* u) {
    return u->baseUnit;
}

unsigned int fmi1_xml_get_unit_display_unit_number(fmi1_xml_unit_t* u) {
    return (unsigned int)jm_vector_get_size(jm_voidp)(&u->displayUnits);
}

fmi1_xml_display_unit_t* fmi1_xml_get_unit_display_unit(fmi1_xml_unit_t* u, size_t index) {
    if(index >= fmi1_xml_get_unit_display_unit_number(u)) return 0;
    return jm_vector_get_item(jm_voidp)(&u->displayUnits, index);
}


fmi1_xml_unit_t* fmi1_xml_get_base_unit(fmi1_xml_display_unit_t* du) {
    return du->baseUnit;
}

const char* fmi1_xml_get_display_unit_name(fmi1_xml_display_unit_t* du) {
    return du->displayUnit;
}

double fmi1_xml_get_display_unit_gain(fmi1_xml_display_unit_t* du) {
    return du->gain;
}

double fmi1_xml_get_display_unit_offset(fmi1_xml_display_unit_t* du) {
    return du->offset;
}

double fmi1_xml_convert_to_display_unit(double val , fmi1_xml_display_unit_t* du, int isRelativeQuantity) {
    double gain = fmi1_xml_get_display_unit_gain(du);
    double offset = fmi1_xml_get_display_unit_offset(du);
    if(isRelativeQuantity)
        return val *gain;
    else
        return (val*gain + offset);
}

double fmi1_xml_convert_from_display_unit(double val, fmi1_xml_display_unit_t* du, int isRelativeQuantity) {
    double gain = fmi1_xml_get_display_unit_gain(du);
    double offset = fmi1_xml_get_display_unit_offset(du);
    if(isRelativeQuantity)
        return val/gain;
    else
        return (val - offset)/gain;
}

int fmi1_xml_handle_UnitDefinitions(fmi1_xml_parser_context_t *context, const char* data) {
    fmi1_xml_model_description_t* md = context->modelDescription;
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


fmi1_xml_display_unit_t* fmi1_xml_get_parsed_unit(fmi1_xml_parser_context_t *context, jm_vector(char)* name, int sorted) {
    fmi1_xml_unit_t dummy, *unit;
    jm_named_ptr named, *pnamed;
    fmi1_xml_model_description_t* md = context->modelDescription;

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
    if(pnamed) *pnamed = named = jm_named_alloc_v(name,sizeof(fmi1_xml_unit_t),dummy.baseUnit - (char*)&dummy,context->callbacks);

    if(!pnamed || !named.ptr) {
        fmi1_xml_parse_fatal(context, "Could not allocate memory");
        return 0;
    }

    unit = named.ptr;
    unit->defaultDisplay.baseUnit = unit;
    unit->defaultDisplay.offset = 0;
    unit->defaultDisplay.gain = 1.0;
    unit->defaultDisplay.displayUnit[0] = 0;
    jm_vector_init(jm_voidp)(&(unit->displayUnits),0,context->callbacks);

    if(sorted) jm_vector_qsort_jm_named_ptr(&(md->unitDefinitions), jm_compare_named);
    return &unit->defaultDisplay;
}

int fmi1_xml_handle_BaseUnit(fmi1_xml_parser_context_t *context, const char* data) {
    if(!data) {
            fmi1_xml_display_unit_t* unit;
            jm_vector(char)* buf = fmi1_xml_reserve_parse_buffer(context,1,100);

            if(!buf) return -1;
            if( /*  <xs:attribute name="unit" type="xs:normalizedString" use="required"/> */
                fmi1_xml_set_attr_string(context, fmi1_xml_elmID_BaseUnit, fmi_attr_id_unit, 1, buf) ||
                !(unit = fmi1_xml_get_parsed_unit(context, buf, 0))
               ) return -1;
            context->lastBaseUnit = unit->baseUnit;
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

int fmi1_xml_handle_DisplayUnitDefinition(fmi1_xml_parser_context_t *context, const char* data) {
    if(!data) {
            fmi1_xml_model_description_t* md = context->modelDescription;
            jm_vector(char)* buf = fmi1_xml_reserve_parse_buffer(context,1,100);
            /* this display unit belongs to the last created base unit */
            fmi1_xml_unit_t* unit = context->lastBaseUnit;
            fmi1_xml_display_unit_t *dispUnit = 0;
            fmi1_xml_display_unit_t dummyDU;
            jm_named_ptr named, *pnamed;
            int ret;

            if(!buf) return -1;
            /* first read the required name attribute */
            /*  <xs:attribute name="displayUnit" type="xs:normalizedString" use="required"/> */
            ret = fmi1_xml_set_attr_string(context, fmi1_xml_elmID_DisplayUnitDefinition, fmi_attr_id_displayUnit, 1, buf);
            if(ret) return ret;
            /* alloc memory to the correct size and put display unit on the list for the base unit */
            named.ptr = 0;
            pnamed = jm_vector_push_back(jm_named_ptr)(&(md->displayUnitDefinitions),named);
            if(pnamed) *pnamed = jm_named_alloc(jm_vector_get_itemp_char(buf,0),sizeof(fmi1_xml_display_unit_t), dummyDU.displayUnit - (char*)&dummyDU,context->callbacks);
            dispUnit = pnamed->ptr;
            if( !pnamed || !dispUnit ||
                !jm_vector_push_back(jm_voidp)(&unit->displayUnits, dispUnit) ) {
                fmi1_xml_parse_fatal(context, "Could not allocate memory");
                return -1;
            }
            dispUnit->baseUnit = unit;
            /* finally process the attributes */
            return (
                        /*  <xs:attribute name="gain" type="xs:double" default="1"/>  */
                        fmi1_xml_set_attr_double(context, fmi1_xml_elmID_DisplayUnitDefinition, fmi_attr_id_gain, 0, &dispUnit->gain, 1)  ||
                         /*  <xs:attribute name="offset" type="xs:double" default="0"/>  */
                        fmi1_xml_set_attr_double(context, fmi1_xml_elmID_DisplayUnitDefinition, fmi_attr_id_offset, 0, &dispUnit->offset, 0)
                     );
    }
    else {
        /* don't do anything. might give out a warning if(data[0] != 0) */
        return 0;
    }
    return 0;
}

