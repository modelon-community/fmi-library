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

#include "fmi1_xml_model_description_impl.h"
#include "fmi1_xml_parser.h"

static const char * module = "FMI1XML";

#define ATTR_STR(attr) #attr
const char *fmi1_xmlAttrNames[] = {
    FMI1_XML_ATTRLIST(ATTR_STR)
};

/* fmi1_xml_scheme_ defines give parent ID, the index in a sequence among siblings, flag if multiple elems are allowed */
#define fmi1_xml_scheme_fmiModelDescription {fmi1_xml_elmID_none, 0, 0}
#define fmi1_xml_scheme_UnitDefinitions {fmi1_xml_elmID_fmiModelDescription, 0, 0}
#define fmi1_xml_scheme_BaseUnit {fmi1_xml_elmID_UnitDefinitions, 0, 1}
#define fmi1_xml_scheme_DisplayUnitDefinition {fmi1_xml_elmID_BaseUnit, 0, 1}
#define fmi1_xml_scheme_TypeDefinitions {fmi1_xml_elmID_fmiModelDescription, 1, 0}
#define fmi1_xml_scheme_Type {fmi1_xml_elmID_TypeDefinitions, 0, 1}
#define fmi1_xml_scheme_RealType {fmi1_xml_elmID_Type, 0, 0}
#define fmi1_xml_scheme_IntegerType {fmi1_xml_elmID_Type, 0, 0}
#define fmi1_xml_scheme_BooleanType {fmi1_xml_elmID_Type, 0, 0}
#define fmi1_xml_scheme_StringType {fmi1_xml_elmID_Type, 0, 0}
#define fmi1_xml_scheme_EnumerationType {fmi1_xml_elmID_Type, 0, 0}
#define fmi1_xml_scheme_Item {fmi1_xml_elmID_EnumerationType, 0, 1}
#define fmi1_xml_scheme_DefaultExperiment {fmi1_xml_elmID_fmiModelDescription, 2, 0}
#define fmi1_xml_scheme_VendorAnnotations {fmi1_xml_elmID_fmiModelDescription, 3, 0}
#define fmi1_xml_scheme_Tool {fmi1_xml_elmID_VendorAnnotations, 0, 1}
#define fmi1_xml_scheme_Annotation {fmi1_xml_elmID_Tool, 0, 1}
#define fmi1_xml_scheme_ModelVariables {fmi1_xml_elmID_fmiModelDescription, 4, 0}
#define fmi1_xml_scheme_ScalarVariable {fmi1_xml_elmID_ModelVariables, 0, 1}
#define fmi1_xml_scheme_DirectDependency {fmi1_xml_elmID_ScalarVariable, 1, 0}
#define fmi1_xml_scheme_Name {fmi1_xml_elmID_DirectDependency, 0, 1}
#define fmi1_xml_scheme_Real {fmi1_xml_elmID_ScalarVariable, 0, 0}
#define fmi1_xml_scheme_Integer {fmi1_xml_elmID_ScalarVariable, 0, 0}
#define fmi1_xml_scheme_Boolean {fmi1_xml_elmID_ScalarVariable, 0, 0}
#define fmi1_xml_scheme_String {fmi1_xml_elmID_ScalarVariable, 0, 0}
#define fmi1_xml_scheme_Enumeration {fmi1_xml_elmID_ScalarVariable, 0, 0}
#define fmi1_xml_scheme_Implementation {fmi1_xml_elmID_fmiModelDescription, 5, 0}
#define fmi1_xml_scheme_CoSimulation_StandAlone {fmi1_xml_elmID_Implementation, 0, 0}
/* NOTE: Capabilities need special handling since it can appear both under 
	CoSimulation_StandAlone and CoSimulation_Tool
*/
#define fmi1_xml_scheme_Capabilities {fmi1_xml_elmID_CoSimulation_StandAlone, 0, 0}
#define fmi1_xml_scheme_CoSimulation_Tool {fmi1_xml_elmID_Implementation, 0, 0}
#define fmi1_xml_scheme_Model {fmi1_xml_elmID_CoSimulation_Tool, 1, 0}
#define fmi1_xml_scheme_File {fmi1_xml_elmID_Model, 0, 1}

#define EXPAND_ELM_SCHEME(elm) fmi1_xml_scheme_##elm ,

fmi1_xml_scheme_info_t fmi1_xml_scheme_info[fmi1_xml_elm_number] = {
    FMI1_XML_ELMLIST(EXPAND_ELM_SCHEME)
};

#define EXPAND_ELM_NAME(elm) { #elm, fmi1_xml_handle_##elm, fmi1_xml_elmID_##elm},

fmi1_xml_element_handle_map_t fmi1_element_handle_map[fmi1_xml_elm_number] = {
    FMI1_XML_ELMLIST(EXPAND_ELM_NAME)
};

void fmi1_xml_parse_free_context(fmi1_xml_parser_context_t *context) {
    if(!context) return;
    if(context->modelDescription)
        fmi1_xml_clear_model_description(context->modelDescription);
    if(context->parser) {
        XML_ParserFree(context->parser);
        context->parser = 0;
    }
    fmi1_xml_free_parse_buffer(context);
    if(context->attrMap) {
        jm_vector_free(jm_named_ptr)(context->attrMap);
        context->attrMap = 0;
    }
    if(context->elmMap) {
        jm_vector_free(fmi1_xml_element_handle_map_t)(context->elmMap);
        context->elmMap = 0;
    }
    if(context->attrBuffer) {
        jm_vector_free(jm_string)(context->attrBuffer);
        context->attrBuffer = 0;
    }
    jm_stack_free_data(int)(& context->elmStack );
    jm_vector_free_data(char)( &context->elmData );

    jm_vector_free_data(jm_voidp)(&context->directDependencyBuf);
    jm_vector_foreach(jm_string)(&context->directDependencyStringsStore, (void(*)(jm_string))context->callbacks->free);
    jm_vector_free_data(jm_string)(&context->directDependencyStringsStore);

    context->callbacks->free(context);
}

void fmi1_xml_parse_fatal(fmi1_xml_parser_context_t *context, const char* fmt, ...) {
    va_list args;
    va_start (args, fmt);
	jm_log_fatal_v(context->callbacks, module, fmt, args);
    va_end (args);
    XML_StopParser(context->parser,0);
}

void fmi1_xml_parse_error(fmi1_xml_parser_context_t *context, const char* fmt, ...) {
    va_list args;
    va_start (args, fmt);
	if(context->parser) {		
        jm_log_error(context->callbacks, module, "Detected on line:%u of modelDescription.xml", XML_GetCurrentLineNumber(context->parser));
        jm_log_error_v(context->callbacks, module, fmt, args);
    }
	else
		jm_log_error_v(context->callbacks, module, fmt, args);
    va_end (args);
}


int fmi1_xml_is_attr_defined(fmi1_xml_parser_context_t *context, fmi1_xml_attr_enu_t attrID) {
    return ( jm_vector_get_item(jm_string)(context->attrBuffer, attrID) != 0);
}

static int fmi1_get_attr_str(fmi1_xml_parser_context_t *context, fmi1_xml_elm_enu_t elmID, fmi1_xml_attr_enu_t attrID, int required,const char** valp) {

    jm_string elmName, attrName, value;

    elmName = fmi1_element_handle_map[elmID].elementName;
    attrName = fmi1_xmlAttrNames[attrID];
    value = jm_vector_get_item(jm_string)(context->attrBuffer, attrID);
    *valp =  value;
    jm_vector_set_item(jm_string)(context->attrBuffer, attrID, 0);
    if(!(*valp)) {
        if (required) {
            fmi1_xml_parse_fatal(context, "Parsing XML element '%s': required attribute '%s' not found", elmName, attrName);
            return -1;
        }
        else
            return 0;
    }
    return 0;
}

int fmi1_xml_set_attr_string(fmi1_xml_parser_context_t *context, fmi1_xml_elm_enu_t elmID, fmi1_xml_attr_enu_t attrID, int required, jm_vector(char)* field) {
    int ret;
    jm_string elmName, attrName, val;
    size_t len;
    ret = fmi1_get_attr_str(context, elmID, attrID,required,&val);
    if(ret) return ret;
    if((!val || !val[0]) && !required) {
        jm_vector_resize(char)(field, 1);
        jm_vector_set_item(char)(field, 0, 0);
        jm_vector_resize(char)(field, 0);
        return 0;
    }
    elmName = fmi1_element_handle_map[elmID].elementName;
    attrName = fmi1_xmlAttrNames[attrID];

    len = strlen(val) + 1;
    if(jm_vector_resize(char)(field, len) < len) {
        fmi1_xml_parse_fatal(context, "XML element '%s': could not allocate memory for setting '%s'='%s'", elmName, attrName, val);
        return -1;
    }
    /* copy terminating 0 as well but set vector size to be actual string length */
    memcpy(jm_vector_get_itemp(char)(field,0), val, len);
    jm_vector_resize(char)(field, len - 1);
    return 0;
}

int fmi1_xml_set_attr_uint(fmi1_xml_parser_context_t *context, fmi1_xml_elm_enu_t elmID, fmi1_xml_attr_enu_t attrID, int required, unsigned int* field, unsigned int defaultVal) {
    int ret;
    jm_string elmName, attrName, strVal;    

    ret = fmi1_get_attr_str(context, elmID, attrID,required,&strVal);
    if(ret) return ret;
    if(!strVal && !required) {
        *field = defaultVal;
        return 0;
    }

    elmName = fmi1_element_handle_map[elmID].elementName;
    attrName = fmi1_xmlAttrNames[attrID];

    if(sscanf(strVal, "%u", field) != 1) {
        fmi1_xml_parse_error(context, "XML element '%s': could not parse value for attribute '%s'='%s'", elmName, attrName, strVal);
        return -1;
    }
    return 0;
}


int fmi1_xml_set_attr_enum(fmi1_xml_parser_context_t *context, fmi1_xml_elm_enu_t elmID, fmi1_xml_attr_enu_t attrID, int required, unsigned int* field, unsigned int defaultVal, jm_name_ID_map_t* nameMap) {
    int ret, i;
    jm_string elmName, attrName, strVal;

    ret = fmi1_get_attr_str(context, elmID, attrID,required,&strVal);
    if(ret) return ret;
    if(!strVal && !required) {
        *field = defaultVal;
        return 0;
    }

    elmName = fmi1_element_handle_map[elmID].elementName;
    attrName = fmi1_xmlAttrNames[attrID];

    i = 0;
    while(nameMap[i].name && strcmp(nameMap[i].name, strVal)) i++;
    if(!nameMap[i].name) {
        fmi1_xml_parse_fatal(context, "XML element '%s': could not parse value for attribute '%s'='%s'", elmName, attrName, strVal);
        return -1;
    }
    *field = nameMap[i].ID;
    return 0;
}

int fmi1_xml_set_attr_boolean(fmi1_xml_parser_context_t *context, fmi1_xml_elm_enu_t elmID, fmi1_xml_attr_enu_t attrID, int required, int* field, int defaultVal) {
    jm_name_ID_map_t fmi_boolean_i_dMap[] = {{"true", 1},{"false", 0}, {"1", 1},{"0", 0}, {0,0}};
    return fmi1_xml_set_attr_enum(context,elmID, attrID,required, (unsigned*)field, (unsigned)defaultVal, fmi_boolean_i_dMap);
}

int fmi1_xml_set_attr_int(fmi1_xml_parser_context_t *context, fmi1_xml_elm_enu_t elmID, fmi1_xml_attr_enu_t attrID, int required, int* field, int defaultVal) {
    int ret;
    jm_string elmName, attrName, strVal;

    ret = fmi1_get_attr_str(context, elmID, attrID,required,&strVal);
    if(ret) return ret;
    if(!strVal && !required) {
        *field = defaultVal;
        return 0;
    }

    elmName = fmi1_element_handle_map[elmID].elementName;
    attrName = fmi1_xmlAttrNames[attrID];

    if(sscanf(strVal, "%d", field) != 1) {
        fmi1_xml_parse_error(context, "XML element '%s': could not parse value for attribute '%s'='%s'", elmName, attrName, strVal);
        return -1;
    }
    return 0;
}

int fmi1_xml_set_attr_double(fmi1_xml_parser_context_t *context, fmi1_xml_elm_enu_t elmID, fmi1_xml_attr_enu_t attrID, int required, double* field, double defaultVal) {

    int ret;
    jm_string elmName, attrName, strVal;


    ret = fmi1_get_attr_str(context, elmID, attrID,required,&strVal);
    if(ret) return ret;
    if(!strVal && !required) {
        *field = defaultVal;
        return 0;
    }

    elmName = fmi1_element_handle_map[elmID].elementName;
    attrName = fmi1_xmlAttrNames[attrID];

    if(sscanf(strVal, "%lf", field) != 1) {
        fmi1_xml_parse_fatal(context, "XML element '%s': could not parse value for attribute '%s'='%s'", elmName, attrName, strVal);
        return -1;
    }
    return 0;
}

int fmi1_xml_alloc_parse_buffer(fmi1_xml_parser_context_t *context, size_t items) {

    jm_vector(jm_voidp)* parseBuffer = &context->parseBuffer;

    if(jm_vector_init(jm_voidp)(parseBuffer,items,context->callbacks) < items) {
        fmi1_xml_parse_fatal(context, "Could not allocate buffer for parsing XML");
        return -1;
    }
    jm_vector_zero(jm_voidp)(parseBuffer);
    return 0;
}

void fmi1_xml_free_parse_buffer(fmi1_xml_parser_context_t *context) {
    size_t i;
    jm_vector(jm_voidp)* parseBuffer = &context->parseBuffer;

    for(i=0; i < jm_vector_get_size(jm_voidp)(parseBuffer); i++) {
        jm_vector(char) * item = jm_vector_get_item(jm_voidp)(parseBuffer,i);
        if(item) jm_vector_free(char)(item);
    }
    jm_vector_free_data(jm_voidp)(parseBuffer);
}

jm_vector(char) * fmi1_xml_reserve_parse_buffer(fmi1_xml_parser_context_t *context, size_t index, size_t size) {

    jm_vector(jm_voidp)* parseBuffer = &context->parseBuffer;
    jm_vector(char) * item = jm_vector_get_item(jm_voidp)(parseBuffer,index);
    if(!item) {
        item = jm_vector_alloc(char)(size,size,context->callbacks);
        jm_vector_set_item(jm_voidp)(parseBuffer,index,item);
        if(!item) {
            fmi1_xml_parse_fatal(context, "Could not allocate a buffer for parsing XML");
            return 0;
        }
    }
    else {
        if(jm_vector_resize(char)(item, size) < size ) {
            fmi1_xml_parse_fatal(context, "Could not allocate a buffer for parsing XML");
            return 0;
        }
    }
    return item;
}

jm_vector(char) * fmi1_xml_get_parse_buffer(fmi1_xml_parser_context_t *context, size_t index) {
    jm_vector(jm_voidp)* parseBuffer = &context->parseBuffer;
    return jm_vector_get_item(jm_voidp)(parseBuffer,index);
}



int fmi1_create_attr_map(fmi1_xml_parser_context_t* context) {
    int i;
    context->attrBuffer = jm_vector_alloc(jm_string)(fmi1_xml_attr_number, fmi1_xml_attr_number, context->callbacks);
    if(!context->attrBuffer) return -1;
    context->attrMap = jm_vector_alloc(jm_named_ptr)(fmi1_xml_attr_number, fmi1_xml_attr_number, context->callbacks);
    if(!context->attrMap) return -1;
    for(i = 0; i < fmi1_xml_attr_number; i++) {
        jm_named_ptr map;
        jm_vector_set_item(jm_string)(context->attrBuffer, i, 0);
        map.name = fmi1_xmlAttrNames[i];
        map.ptr = (void*)(jm_vector_get_itemp(jm_string)(context->attrBuffer, i));
        jm_vector_set_item(jm_named_ptr)(context->attrMap, i, map);
    }
    jm_vector_qsort(jm_named_ptr)(context->attrMap, jm_compare_named);
    return 0;
}

int fmi1_create_elm_map(fmi1_xml_parser_context_t* context) {
    size_t i;
    context->elmMap = jm_vector_alloc(fmi1_xml_element_handle_map_t)(fmi1_xml_elm_number, fmi1_xml_elm_number, context->callbacks);
    if(!context->elmMap) return -1;
    for(i = 0; i < fmi1_xml_elm_number; i++) {
        fmi1_xml_element_handle_map_t item = fmi1_element_handle_map[i];
        jm_vector_set_item(fmi1_xml_element_handle_map_t)(context->elmMap, i, item);
    }
    jm_vector_qsort(fmi1_xml_element_handle_map_t)(context->elmMap, fmi1_xml_compare_elmName);
    return 0;
}

static void XMLCALL fmi1_parse_element_start(void *c, const char *elm, const char **attr) {
    jm_named_ptr key;
    fmi1_xml_element_handle_map_t keyEl;
    fmi1_xml_element_handle_map_t* currentElMap;
    jm_named_ptr* currentMap;
	fmi1_xml_elm_enu_t currentID;
    int i;
    fmi1_xml_parser_context_t *context = c;
    
	if(context->skipElementCnt) {
		context->skipElementCnt++;
        jm_log_warning(context->callbacks, module, "[Line:%u] Skipping nested XML element '%s'",
			XML_GetCurrentLineNumber(context->parser), elm);
		return;
	}
	
	keyEl.elementName = elm;
	/* find the element handle by name */
    currentElMap = jm_vector_bsearch(fmi1_xml_element_handle_map_t)(context->elmMap, &keyEl, fmi1_xml_compare_elmName);
    if(!currentElMap) {
        /* not found error*/
        jm_log_error(context->callbacks, module, "[Line:%u] Unknown element '%s' in XML, skipping",
			XML_GetCurrentLineNumber(context->parser), elm);
		context->skipElementCnt = 1;
        return;
    }

    currentID = currentElMap->elemID;
	/* Check that parent-child & siblings are fine */
	{
		fmi1_xml_elm_enu_t parentID = context->currentElmID;
		fmi1_xml_elm_enu_t siblingID =  context->lastElmID;

		if((fmi1_xml_scheme_info[currentID].parentID != parentID) &&
			((currentID != fmi1_xml_elmID_Capabilities) || (parentID != fmi1_xml_elmID_CoSimulation_Tool))) {
				jm_log_error(context->callbacks, module, 
					"[Line:%u] XML element '%s' cannot be placed inside '%s', skipping",
					XML_GetCurrentLineNumber(context->parser), elm, fmi1_element_handle_map[parentID].elementName);
				context->skipElementCnt = 1;
				return;
		}
		if(siblingID != fmi1_xml_elmID_none) {
			if(siblingID == currentID) {
				if(!fmi1_xml_scheme_info[currentID].multipleAllowed) {
					jm_log_error(context->callbacks, module, 
						"[Line:%u] Multiple instances of XML element '%s' are not allowed, skipping",
						XML_GetCurrentLineNumber(context->parser), elm);
					context->skipElementCnt = 1;
					return;
				}
			}
			else {
				int lastSiblingIndex = fmi1_xml_scheme_info[siblingID].siblingIndex;
				int curSiblingIndex = fmi1_xml_scheme_info[currentID].siblingIndex;

				if(lastSiblingIndex >= curSiblingIndex) {
					jm_log_error(context->callbacks, module, 
						"[Line:%u] XML element '%s' cannot be placed after element '%s', skipping",
						XML_GetCurrentLineNumber(context->parser), elm, fmi1_element_handle_map[siblingID].elementName);
					context->skipElementCnt = 1;
					return;
				}
			}
		}
		context->lastElmID = fmi1_xml_elmID_none;
	}

    /* process the attributes  */
    i = 0;
    while(attr[i]) {
        key.name = attr[i];
        /* find attribute by name  */
        currentMap = jm_vector_bsearch(jm_named_ptr)(context->attrMap, &key, jm_compare_named);
        if(!currentMap) {
            /* not found error*/
			jm_log_error(context->callbacks, module, "Unknown attribute '%s' in XML", attr[i]);
        }
		else  {
            /* save attr value (still as string) for further handling  */
            const char** mapItem = (const char**)currentMap->ptr;
            *mapItem = attr[i+1];
        }
        i += 2;
    }

    /* handle the element */
	if( currentElMap->elementHandle(context, 0) ) {
        return;
    }
	if(context->skipElementCnt) return;
    /* check that the element handle had process all the attributes */
    for(i = 0; i < fmi1_xml_attr_number; i++) {
        if(jm_vector_get_item(jm_string)(context->attrBuffer, i)) {
            if(!context->skipOneVariableFlag)
                jm_log_warning(context->callbacks,module, "Attribute '%s' not processed by element '%s' handle", fmi1_xmlAttrNames[i], elm);
            jm_vector_set_item(jm_string)(context->attrBuffer, i,0);
        }
    }
    if(context -> currentElmID != fmi1_xml_elmID_none) { /* with nested elements: put the parent on the stack*/
        jm_stack_push(int)(&context->elmStack, context -> currentElmID);
    }
    context -> currentElmID = currentID;
}

static void XMLCALL fmi1_parse_element_end(void* c, const char *elm) {

    fmi1_xml_element_handle_map_t keyEl;
    fmi1_xml_element_handle_map_t* currentElMap;
	fmi1_xml_elm_enu_t currentID;
    fmi1_xml_parser_context_t *context = c;

	if(context->skipElementCnt) {
		context->skipElementCnt--;
		return;
	}

    keyEl.elementName = elm;
    currentElMap = jm_vector_bsearch(fmi1_xml_element_handle_map_t)(context->elmMap, &keyEl, fmi1_xml_compare_elmName);
    if(!currentElMap) {
        /* not found error*/
        fmi1_xml_parse_fatal(context, "Unknown element end in XML (element: %s)", elm);
        return;
    }
    currentID = currentElMap->elemID;

    if(currentID != context -> currentElmID) {
        /* missmatch error*/
        fmi1_xml_parse_fatal(context, "Element end '%s' does not match element start '%s' in XML", elm, 
			fmi1_element_handle_map[context -> currentElmID].elementName);
        return;
    }

    jm_vector_push_back(char)(&context->elmData, 0);

	if( currentElMap->elementHandle(context, jm_vector_get_itemp(char)(&context->elmData, 0) )) {
        return;
    }
    jm_vector_resize(char)(&context->elmData, 0);

    /* record the last handle and pop the stack */
    context->lastElmID = currentID;

    if(jm_stack_is_empty(int)(&context->elmStack)) {
        context -> currentElmID = fmi1_xml_elmID_none;
    }
    else {
        context -> currentElmID = (fmi1_xml_elm_enu_t)jm_stack_pop(int)(&context->elmStack);
    }
}

/*
*  Called to handle element data, e.g. "xy" in <Name>xy</Name>
*  Can be called many times, e.g. with "x" and then with "y" in the example above.
*  Feature in expat:
*  For some reason, if the element data is the empty string (Eg. <a></a>)
*  instead of an empty string with len == 0 we get "\n". The workaround is
*  to replace this with the empty string whenever we encounter "\n".
*/
static void XMLCALL fmi1_parse_element_data(void* c, const XML_Char *s, int len) {
        fmi1_xml_parser_context_t *context = c;
        int i;
        jm_vector_reserve(char)(&context->elmData, len + jm_vector_get_size(char)(&context->elmData) + 1);
        for(i = 0; i< len;i++) {
            char ch = s[i];
            if(ch != '\n') {
                jm_vector_push_back(char)(&context->elmData, ch);
            }
        }
}

int fmi1_xml_parse_model_description(fmi1_xml_model_description_t* md, const char* filename) {
    XML_Memory_Handling_Suite memsuite;
    fmi1_xml_parser_context_t* context;
    XML_Parser parser = NULL;
    FILE* file;

    context = (fmi1_xml_parser_context_t*)md->callbacks->calloc(1, sizeof(fmi1_xml_parser_context_t));
    if(!context) {
        jm_log_fatal(md->callbacks, "FMIXML", "Could not allocate memory for XML parser context");
    }
    context->callbacks = md->callbacks;
    context->modelDescription = md;
    if(fmi1_xml_alloc_parse_buffer(context, 16)) return -1;
    if(fmi1_create_attr_map(context) || fmi1_create_elm_map(context)) {
        fmi1_xml_parse_fatal(context, "Error in parsing initialization");
        fmi1_xml_parse_free_context(context);
        return -1;
    }
    context->lastBaseUnit = 0;
    jm_vector_init(jm_voidp)(&context->directDependencyBuf, 0, context->callbacks);
    jm_vector_init(jm_string)(&context->directDependencyStringsStore, 0, context->callbacks);
    context->skipOneVariableFlag = 0;
	context->skipElementCnt = 0;
    jm_stack_init(int)(&context->elmStack,  context->callbacks);
    jm_vector_init(char)(&context->elmData, 0, context->callbacks);
    context->lastElmID = fmi1_xml_elmID_none;
    context->currentElmID = fmi1_xml_elmID_none;

    memsuite.malloc_fcn = context->callbacks->malloc;
    memsuite.realloc_fcn = context->callbacks->realloc;
    memsuite.free_fcn = context->callbacks->free;
    context -> parser = parser = XML_ParserCreate_MM(0, &memsuite, 0);

    if(! parser) {
        fmi1_xml_parse_fatal(context, "Could not initialize XML parsing library.");
        fmi1_xml_parse_free_context(context);
        return -1;
    }

    XML_SetUserData( parser, context);

    XML_SetElementHandler(parser, fmi1_parse_element_start, fmi1_parse_element_end);

    XML_SetCharacterDataHandler(parser, fmi1_parse_element_data);

    file = fopen(filename, "rb");
    if (file == NULL) {
        fmi1_xml_parse_fatal(context, "Cannot open file '%s' for parsing", filename);
        fmi1_xml_parse_free_context(context);
        return -1;
    }

    while (!feof(file)) {
        char * text = jm_vector_get_itemp(char)(fmi1_xml_reserve_parse_buffer(context,0,XML_BLOCK_SIZE),0);
        int n = (int)fread(text, sizeof(char), XML_BLOCK_SIZE, file);
        if(ferror(file)) {
            fmi1_xml_parse_fatal(context, "Error reading from file %s", filename);
            fclose(file);
	        fmi1_xml_parse_free_context(context);
            return -1;
        }
        if (!XML_Parse(parser, text, n, feof(file))) {
             fmi1_xml_parse_fatal(context, "Parse error at line %d:\n%s",
                         (int)XML_GetCurrentLineNumber(parser),
                         XML_ErrorString(XML_GetErrorCode(parser)));
             fclose(file);
		     fmi1_xml_parse_free_context(context);
             return -1; /* failure */
        }        
    }
    fclose(file);
    /* done later XML_ParserFree(parser);*/
    if(!jm_stack_is_empty(int)(&context->elmStack)) {
        fmi1_xml_parse_fatal(context, "Unexpected end of file (not all elements ended) when parsing %s", filename);
        fmi1_xml_parse_free_context(context);
        return -1;
    }

    md->status = fmi1_xml_model_description_enu_ok;
    context->modelDescription = 0;
    fmi1_xml_parse_free_context(context);

    return 0;
}

#define JM_TEMPLATE_INSTANCE_TYPE fmi1_xml_element_handle_map_t
#include "JM/jm_vector_template.h"
