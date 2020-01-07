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

/* For checking variable naming conventions */
#include <fmi3_xml_variable_name_parser.tab.h>
#define YYSTYPE YYFMI3STYPE
#include <fmi3_xml_variable_name_lex.h>

#include "fmi3_xml_model_description_impl.h"
#include "fmi3_xml_parser.h"

static const char * module = "FMI3XML";

#define ATTR_STR(attr) #attr,
const char *fmi3_xmlAttrNames[fmi3_xml_attr_number] = {
    FMI3_XML_ATTRLIST(ATTR_STR)
};

/* fmi3_xml_scheme_<elem> params: super-type, parent ID, the index in a sequence among siblings, flag if multiple elems are allowed */
#define fmi3_xml_scheme_fmiModelDescription  {fmi3_xml_elmID_none,       fmi3_xml_elmID_none,                0, 0}
#define fmi3_xml_scheme_ModelExchange        {fmi3_xml_elmID_none,       fmi3_xml_elmID_fmiModelDescription, 0, 0}
#define fmi3_xml_scheme_SourceFiles          {fmi3_xml_elmID_none,       fmi3_xml_elmID_ModelExchange,       0, 0}
#define fmi3_xml_scheme_File                 {fmi3_xml_elmID_none,       fmi3_xml_elmID_SourceFiles,         0, 1}
#define fmi3_xml_scheme_CoSimulation         {fmi3_xml_elmID_none,       fmi3_xml_elmID_fmiModelDescription, 1, 0}
#define fmi3_xml_scheme_SourceFilesCS        {fmi3_xml_elmID_none,       fmi3_xml_elmID_CoSimulation,        0, 0}
#define fmi3_xml_scheme_FileCS               {fmi3_xml_elmID_none,       fmi3_xml_elmID_SourceFilesCS,       0, 1}
#define fmi3_xml_scheme_UnitDefinitions      {fmi3_xml_elmID_none,       fmi3_xml_elmID_fmiModelDescription, 2, 0}
#define fmi3_xml_scheme_Unit                 {fmi3_xml_elmID_none,       fmi3_xml_elmID_UnitDefinitions,     0, 1}
#define fmi3_xml_scheme_BaseUnit             {fmi3_xml_elmID_none,       fmi3_xml_elmID_Unit,                0, 0}
#define fmi3_xml_scheme_DisplayUnit          {fmi3_xml_elmID_none,       fmi3_xml_elmID_Unit,                1, 1}
#define fmi3_xml_scheme_TypeDefinitions      {fmi3_xml_elmID_none,       fmi3_xml_elmID_fmiModelDescription, 3, 0}
#define fmi3_xml_scheme_SimpleType           {fmi3_xml_elmID_none,       fmi3_xml_elmID_TypeDefinitions,     0, 1}
#define fmi3_xml_scheme_Float64              {fmi3_xml_elmID_SimpleType, fmi3_xml_elmID_TypeDefinitions,     0, 1}
#define fmi3_xml_scheme_Float32              {fmi3_xml_elmID_SimpleType, fmi3_xml_elmID_TypeDefinitions,     0, 1}
#define fmi3_xml_scheme_Real                 {fmi3_xml_elmID_none,       fmi3_xml_elmID_SimpleType,          0, 0}
#define fmi3_xml_scheme_Integer              {fmi3_xml_elmID_none,       fmi3_xml_elmID_SimpleType,          0, 0}
#define fmi3_xml_scheme_Boolean              {fmi3_xml_elmID_none,       fmi3_xml_elmID_SimpleType,          0, 0}
#define fmi3_xml_scheme_String               {fmi3_xml_elmID_none,       fmi3_xml_elmID_SimpleType,          0, 0}
#define fmi3_xml_scheme_Enumeration          {fmi3_xml_elmID_none,       fmi3_xml_elmID_SimpleType,          0, 0}
#define fmi3_xml_scheme_Item                 {fmi3_xml_elmID_none,       fmi3_xml_elmID_Enumeration,         0, 1}
#define fmi3_xml_scheme_LogCategories        {fmi3_xml_elmID_none,       fmi3_xml_elmID_fmiModelDescription, 4, 0}
#define fmi3_xml_scheme_Category             {fmi3_xml_elmID_none,       fmi3_xml_elmID_LogCategories,       0, 1}
#define fmi3_xml_scheme_DefaultExperiment    {fmi3_xml_elmID_none,       fmi3_xml_elmID_fmiModelDescription, 5, 0}
#define fmi3_xml_scheme_VendorAnnotations    {fmi3_xml_elmID_none,       fmi3_xml_elmID_fmiModelDescription, 6, 0}
#define fmi3_xml_scheme_Tool                 {fmi3_xml_elmID_none,       fmi3_xml_elmID_VendorAnnotations,   0, 1}
#define fmi3_xml_scheme_ModelVariables       {fmi3_xml_elmID_none,       fmi3_xml_elmID_fmiModelDescription, 7, 0}

#define fmi3_xml_scheme_ModelStructure       {fmi3_xml_elmID_none,       fmi3_xml_elmID_fmiModelDescription, 8, 0}
#define fmi3_xml_scheme_Outputs              {fmi3_xml_elmID_none,       fmi3_xml_elmID_ModelStructure,      0, 0}
#define fmi3_xml_scheme_Unknown              {fmi3_xml_elmID_none,       fmi3_xml_elmID_Outputs,             0, 1}
#define fmi3_xml_scheme_Derivatives          {fmi3_xml_elmID_none,       fmi3_xml_elmID_ModelStructure,      1, 0}
#define fmi3_xml_scheme_DerivativeUnknown    {fmi3_xml_elmID_none,       fmi3_xml_elmID_Derivatives,         0, 1}
#define fmi3_xml_scheme_DiscreteStates       {fmi3_xml_elmID_none,       fmi3_xml_elmID_ModelStructure,      2, 0}
#define fmi3_xml_scheme_DiscreteStateUnknown {fmi3_xml_elmID_none,       fmi3_xml_elmID_DiscreteStates,      0, 1}
#define fmi3_xml_scheme_InitialUnknowns      {fmi3_xml_elmID_none,       fmi3_xml_elmID_ModelStructure,      3, 0}
#define fmi3_xml_scheme_InitialUnknown       {fmi3_xml_elmID_none,       fmi3_xml_elmID_InitialUnknowns,     0, 1}

#define fmi3_xml_scheme_Variable             {fmi3_xml_elmID_none,       fmi3_xml_elmID_ModelVariables,      0, 1}
#define fmi3_xml_scheme_Float64Variable      {fmi3_xml_elmID_Variable,   fmi3_xml_elmID_ModelVariables,      0, 1}
#define fmi3_xml_scheme_Float32Variable      {fmi3_xml_elmID_Variable,   fmi3_xml_elmID_ModelVariables,      0, 1}
#define fmi3_xml_scheme_RealVariable         {fmi3_xml_elmID_Variable,   fmi3_xml_elmID_ModelVariables,      0, 1}
#define fmi3_xml_scheme_IntegerVariable      {fmi3_xml_elmID_Variable,   fmi3_xml_elmID_ModelVariables,      0, 1}
#define fmi3_xml_scheme_BooleanVariable      {fmi3_xml_elmID_Variable,   fmi3_xml_elmID_ModelVariables,      0, 1}
#define fmi3_xml_scheme_StringVariable       {fmi3_xml_elmID_Variable,   fmi3_xml_elmID_ModelVariables,      0, 1}
#define fmi3_xml_scheme_EnumerationVariable  {fmi3_xml_elmID_Variable,   fmi3_xml_elmID_ModelVariables,      0, 1}
#define fmi3_xml_scheme_Dimension            {fmi3_xml_elmID_none,       fmi3_xml_elmID_Variable,            0, 1}

#define fmi3_xml_scheme_Annotations          {fmi3_xml_elmID_none,       fmi3_xml_elmID_Variable,            1, 0}
#define fmi3_xml_scheme_VariableTool         {fmi3_xml_elmID_none,       fmi3_xml_elmID_Annotations,         0, 1}

/**
 * The expansion of below macro is also a macro. Example:
 * EXPAND_ELM_SCHEME(Real) -> fmi3_xml_scheme_Real -> {fmi3_xml_elmID_SimpleType, 0, 0}
 */
#define EXPAND_ELM_SCHEME(elm) fmi3_xml_scheme_##elm ,

/* Global array of all scheme_info_t. Index it with fmi3_xml_elm_enu_t entries. */
fmi3_xml_scheme_info_t fmi3_xml_scheme_info[fmi3_xml_elm_number] = {
    FMI3_XML_ELMLIST(EXPAND_ELM_SCHEME)
	{fmi3_xml_elm_actual_number,0,0},
	FMI3_XML_ELMLIST_ALT(EXPAND_ELM_SCHEME)
	FMI3_XML_ELMLIST_ABSTRACT(EXPAND_ELM_SCHEME)
};

#define EXPAND_ELM_NAME(elm) { #elm, fmi3_xml_handle_##elm, fmi3_xml_elmID_##elm},

/**
 * Global array of all defined fmi3_xml_element_handle_map_t structs.
 * Typical use:
 *      Parse element name, from element name find ID, use ID to index this
 *      array.
 */
fmi3_xml_element_handle_map_t fmi3_element_handle_map[fmi3_xml_elm_number] = {
    FMI3_XML_ELMLIST(EXPAND_ELM_NAME)
	{ NULL, NULL, fmi3_xml_elm_actual_number},
	FMI3_XML_ELMLIST_ALT(EXPAND_ELM_NAME)
	FMI3_XML_ELMLIST_ABSTRACT(EXPAND_ELM_NAME)
};

const fmi3_xml_primitive_types_t PRIMITIVE_TYPES = {
    { /* float64 */
        sizeof(fmi3_float64_t),
        "%lf",
        "0.0"
    },
    { /* float32 */
        sizeof(fmi3_float32_t),
        "%f",
        "0.0"
    }
};

void fmi3_xml_parse_free_context(fmi3_xml_parser_context_t *context) {
    if(!context) return;
    if(context->modelDescription)
        fmi3_xml_clear_model_description(context->modelDescription);
    if(context->parser) {
        XML_ParserFree(context->parser);
        context->parser = 0;
    }
    fmi3_xml_free_parse_buffer(context);
    if(context->attrMap) {
        jm_vector_free(jm_named_ptr)(context->attrMap);
        context->attrMap = 0;
    }
    if(context->elmMap) {
        jm_vector_free(fmi3_xml_element_handle_map_t)(context->elmMap);
        context->elmMap = 0;
    }
    if(context->attrBuffer) {
        jm_vector_free(jm_string)(context->attrBuffer);
        context->attrBuffer = 0;
    }
    jm_stack_free_data(int)(& context->elmStack );
    jm_vector_free_data(char)( &context->elmData );

    context->callbacks->free(context);
}

void fmi3_xml_parse_fatal(fmi3_xml_parser_context_t *context, const char* fmt, ...) {
    va_list args;
    va_start (args, fmt);
	jm_log_fatal_v(context->callbacks, module, fmt, args);
    va_end (args);
    XML_StopParser(context->parser,0);
}

void fmi3_xml_parse_error(fmi3_xml_parser_context_t *context, const char* fmt, ...) {
    va_list args;
    va_start (args, fmt);
	if(context->parser)
		jm_log_info(context->callbacks, module, "[Line:%u] Detected during parsing:", XML_GetCurrentLineNumber(context->parser));
	jm_log_error_v(context->callbacks, module, fmt, args);
    va_end (args);
}

static int fmi3_xml_string_char_count(const char* str, char ch) {
    int n = 0;
    int i;

    for (i = 0; i < strlen(str); i++) {
        if (str[i] == ch) {
            n++;
        }
    }

    return n;
}

/* Get attribute as string without clearing the buffer entry */
jm_string fmi3_xml_peek_attr_str(fmi3_xml_parser_context_t* context, fmi3_xml_attr_enu_t attrID) {
    return jm_vector_get_item(jm_string)(context->attrBuffer, attrID);
}

int fmi3_xml_is_attr_defined(fmi3_xml_parser_context_t *context, fmi3_xml_attr_enu_t attrID) {
    return (fmi3_xml_peek_attr_str(context, attrID) != 0);
}

/** 
 * Read value from parse buffer "as is". Also resets the buffer's entry.
 *    valp (return arg): points to attribute value 
 */
int fmi3_xml_get_attr_str(fmi3_xml_parser_context_t *context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID, int required, const char** valp) {

    /* Read and clear attribute */
    jm_string value = fmi3_xml_peek_attr_str(context, attrID);
    jm_vector_set_item(jm_string)(context->attrBuffer, attrID, 0);

    if (!value && required) {
        jm_string elmName = fmi3_element_handle_map[elmID].elementName;
        jm_string attrName = fmi3_xmlAttrNames[attrID];
        fmi3_xml_parse_fatal(context, "Parsing XML element '%s': required attribute '%s' not found", elmName, attrName);
        return -1;
    }

    *valp = value; /* can be NULL */
    return 0;
}

/**
 * Reads the attribute from attribute buffer as jm_vector(char). This will clear the attribute from the buffer.
 *   field (return arg): contains value after function call
 * TODO: Name is misleading for all _set_attr_<type> functions
 */
int fmi3_xml_set_attr_string(fmi3_xml_parser_context_t *context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID, int required, jm_vector(char)* field) {
    int ret;
    jm_string val;
    size_t len;

    /* Get existing from attribute value */
    ret = fmi3_xml_get_attr_str(context, elmID, attrID, required, &val);
    if (ret) return ret;

    if ((!val || !val[0]) && !required) {
        /* Return empty string */
        jm_vector_resize(char)(field, 1);       /* Allocate space for null character */
        jm_vector_set_item(char)(field, 0, 0);  /* Push null character */
        jm_vector_resize(char)(field, 0);       /* Make length same as for strlen */
        return 0;
    }

    len = strlen(val) + 1;

    /* Error check */
    if (jm_vector_resize(char)(field, len) < len) {
        jm_string elmName = fmi3_element_handle_map[elmID].elementName;
        jm_string attrName = fmi3_xmlAttrNames[attrID];
        fmi3_xml_parse_fatal(context, "XML element '%s': could not allocate memory for setting '%s'='%s'", elmName, attrName, val);
        return -1;
    }

    /* Write to buffer */
    /* copy terminating 0 as well but set vector size to be actual string length */
    memcpy(jm_vector_get_itemp(char)(field, 0), val, len);
    jm_vector_resize(char)(field, len - 1);
    return 0;
}

int fmi3_xml_set_attr_uint(fmi3_xml_parser_context_t *context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID, int required, unsigned int* field, unsigned int defaultVal) {
    int ret;
    jm_string elmName, attrName, strVal;    

    ret = fmi3_xml_get_attr_str(context, elmID, attrID,required,&strVal);
    if(ret) return ret;
    if(!strVal && !required) {
        *field = defaultVal;
        return 0;
    }

    elmName = fmi3_element_handle_map[elmID].elementName;
    attrName = fmi3_xmlAttrNames[attrID];

    if(sscanf(strVal, "%u", field) != 1) {
        fmi3_xml_parse_error(context, "XML element '%s': could not parse value for unsigned attribute '%s'='%s'", elmName, attrName, strVal);
        return -1;
    }
    return 0;
}


int fmi3_xml_set_attr_enum(fmi3_xml_parser_context_t *context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID, int required, unsigned int* field, unsigned int defaultVal, jm_name_ID_map_t* nameMap) {
    int ret, i;
    jm_string elmName, attrName, strVal;

    ret = fmi3_xml_get_attr_str(context, elmID, attrID,required,&strVal);
    if(ret) return ret;
    if(!strVal && !required) {
        *field = defaultVal;
        return 0;
    }

    elmName = fmi3_element_handle_map[elmID].elementName;
    attrName = fmi3_xmlAttrNames[attrID];

    i = 0;
    while(nameMap[i].name && strcmp(nameMap[i].name, strVal)) i++;
    if(!nameMap[i].name) {
        fmi3_xml_parse_error(context, "XML element '%s': could not parse value for enumeration attribute '%s'='%s'", elmName, attrName, strVal);
        return -1;
    }
    *field = nameMap[i].ID;
    return 0;
}

int fmi3_xml_set_attr_boolean(fmi3_xml_parser_context_t *context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID, int required, unsigned int* field, unsigned int defaultVal) {
    jm_name_ID_map_t fmi_boolean_i_dMap[] = {{"true", 1},{"false", 0}, {"1", 1},{"0", 0}, {0,0}};
    return fmi3_xml_set_attr_enum(context,elmID, attrID,required, field, defaultVal, fmi_boolean_i_dMap);
}

int fmi3_xml_set_attr_int(fmi3_xml_parser_context_t *context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID, int required, int* field, int defaultVal) {
    int ret;
    jm_string elmName, attrName, strVal;

    ret = fmi3_xml_get_attr_str(context, elmID, attrID,required,&strVal);
    if(ret) return ret;
    if(!strVal && !required) {        
		*field = defaultVal;
        return 0;
    }

    elmName = fmi3_element_handle_map[elmID].elementName;
    attrName = fmi3_xmlAttrNames[attrID];

    if(sscanf(strVal, "%d", field) != 1) {
        fmi3_xml_parse_error(context, "XML element '%s': could not parse value for integer attribute '%s'='%s'", elmName, attrName, strVal);
        return -1;
    }
    return 0;
}

/**
 * Get attribute as float. This will clear the attribute from the parser buffer.
 *
 *  field: where the float value will be stored (return arg)
 *  defaultVal: pointer to default value if attribute wasn't defined
 */
int fmi3_xml_set_attr_float(fmi3_xml_parser_context_t *context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID,
        int required, void* field, void* defaultVal, fmi3_bitness_enu_t bitness) {
    int ret;
    jm_string strVal;
    char formatter[10]; /* used to format string to float */

    /* Set formatter */
    if (bitness == fmi3_bitness_64) {
        strncpy(formatter, "%lf", sizeof(formatter));
    }else if (bitness == fmi3_bitness_32) {
        strncpy(formatter, "%f", sizeof(formatter));
    } else {
        assert(!"Invalid bitness for type float");
    }

    /* Get attribute as string */
    ret = fmi3_xml_get_attr_str(context, elmID, attrID, required, &strVal);
    if (ret) return ret;
    if (!strVal && !required) {
        if (bitness == fmi3_bitness_64) {
            *(fmi3_float64_t*)field = *(fmi3_float64_t*)defaultVal;
        } else if (bitness == fmi3_bitness_32) {
            *(fmi3_float32_t*)field = *(fmi3_float32_t*)defaultVal;
        } else {
            assert(!"Invalid bitness for type float");
        }
        return 0;
    }

    /* Write attribute as float */
    if (sscanf(strVal, formatter, field) != 1) {
        jm_string elmName = fmi3_element_handle_map[elmID].elementName;
        jm_string attrName = fmi3_xmlAttrNames[attrID];
        fmi3_xml_parse_error(context, "XML element '%s': could not parse value for real attribute '%s'='%s'", elmName, attrName, strVal);
        return -1;
    }

    return 0;
}

int fmi3_xml_set_attr_float64(fmi3_xml_parser_context_t *context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID,
        int required, fmi3_float64_t* field, fmi3_float64_t defaultVal) {
    return fmi3_xml_set_attr_float(context, elmID, attrID, required, field, &defaultVal, fmi3_bitness_64);
}

int fmi3_xml_set_attr_float32(fmi3_xml_parser_context_t *context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID,
        int required, fmi3_float32_t* field, fmi3_float32_t defaultVal) {
    return fmi3_xml_set_attr_float(context, elmID, attrID, required, field, &defaultVal, fmi3_bitness_32);
}

/* TODO: Move this somewhere more accessible */
#define JM_LOG_ERROR_NO_MEM()                                                                                               \
    do {                                                                                                                    \
        jm_log_error(context->callbacks, module, "Unable to allocate memory. File: %s, Line: %d", __FILE__, __LINE__);      \
    } while (0);

/**
 * Parses a string to an array of float64 values
 * 
 * str: the string containing the float64 values, they must be separated by exactly one <space> character, must not be NULL
 * arrPtr: pointer to the array where the values will be stored. Memory is dynamically allocated for this array, and must be freed by caller
 * nArr: the number of values in 'arrPtr'
 * 
 * LIMITATION:
 * some expressions that are not a valid target type will be incorrectly formatted (this only happen with invalid XMLs)
 * If schema verification is done before parsing, this limitation won't surface
 * example: float: "1.1;2.2", "1a3", "1e999"
 *
 */
static int fmi3_xml_str_to_array(fmi3_xml_parser_context_t* context, const char* str, void** arrPtr, int* nArr, const fmi3_xml_primitive_type_t* primType) {
    char* strCopy;
    char delim = ' ';
    int nVals = fmi3_xml_string_char_count(str, delim) + 1;
    void* vals;
    char* token;
    const char* formatter = primType->formatter;
    void* writeAddr;

    assert(str);

    /* create a copy that it's OK that strtok mutates */
    strCopy = context->callbacks->malloc(strlen(str) + 1); /* plus one for null character */
    if (!strCopy) {
        JM_LOG_ERROR_NO_MEM();
        return -1;
    }
    strncpy(strCopy, str, strlen(str) + 1);

    /* allocate memory for the start values */
    vals = context->callbacks->malloc(nVals * primType->size); /* freed in fmi3_xml_clear_model_description */
    if (!vals) {
        JM_LOG_ERROR_NO_MEM();
        context->callbacks->free(strCopy);
        return -1;
    }

    /* write start value(s) to correct type */
    if (nVals == 1) { /* scalar */
        if (sscanf(str, formatter, vals) != 1) {
            jm_log_error(context->callbacks, module, "Unable to parse to float: %s", str);

            context->callbacks->free(strCopy);
            return -1;
        }
    } else { /* array */

        /* get the first token */
        token = strtok(strCopy, &delim);

        /* walk through other tokens */
        writeAddr = vals;
        while (token != NULL) {

            /* write attribute as float */
            if (sscanf(token, formatter, writeAddr) != 1) {
                jm_log_error(context->callbacks, module, "Unable to parse to float: %s", token);

                context->callbacks->free(strCopy);
                return -1;
            }

            /* update where to write next value */
            writeAddr = (char*)writeAddr + primType->size; 

            /* get next token */
            token = strtok(NULL, &delim); /* strtok maintains internal buffer - pass NULL as first arg to continue with previous string */
        }
    }

    /* clean up */
    context->callbacks->free(strCopy);
    strCopy = NULL;

    /* assign return arguments */
    *arrPtr = vals;
    *nArr = nVals;

    return 0;
}
/**
 * Get attribute as float array. This will clear the attribute from the parser buffer.
 *
 *  field (return arg): where the float value will be stored
 *
 * TODO: Name is misleading for all _set_attr_<type> functions, I would call it "get_attr_as_float" or similar
 */
int fmi3_xml_set_attr_array(fmi3_xml_parser_context_t *context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID,
        int required, void** arrPtr, int* arrSize, jm_string str, const fmi3_xml_primitive_type_t* primType) {

    if (!str) {
        /* attribute must always be supplied by caller */
        jm_log_error(context->callbacks, module, "Start attribute for array variable not found");
        return -1;
    }

    /* write all attributes to array of correct type */
    if (fmi3_xml_str_to_array(context, str, arrPtr, arrSize, primType)) {
        jm_string elmName = fmi3_element_handle_map[elmID].elementName;
        jm_string attrName = fmi3_xmlAttrNames[attrID];
        fmi3_xml_parse_error(context, "XML element '%s': could not parse value for float attribute '%s'='%s'", elmName, attrName, str);
        return -1;
    }

    return 0;
}

int fmi3_xml_alloc_parse_buffer(fmi3_xml_parser_context_t *context, size_t items) {

    jm_vector(jm_voidp)* parseBuffer = &context->parseBuffer;

    if(jm_vector_init(jm_voidp)(parseBuffer,items,context->callbacks) < items) {
        fmi3_xml_parse_fatal(context, "Could not allocate buffer for parsing XML");
        return -1;
    }
    jm_vector_zero(jm_voidp)(parseBuffer);
    return 0;
}

void fmi3_xml_free_parse_buffer(fmi3_xml_parser_context_t *context) {
    size_t i;
    jm_vector(jm_voidp)* parseBuffer = &context->parseBuffer;

    for(i=0; i < jm_vector_get_size(jm_voidp)(parseBuffer); i++) {
        jm_vector(char) * item = jm_vector_get_item(jm_voidp)(parseBuffer,i);
        if(item) jm_vector_free(char)(item);
    }
    jm_vector_free_data(jm_voidp)(parseBuffer);
}

jm_vector(char) * fmi3_xml_reserve_parse_buffer(fmi3_xml_parser_context_t *context, size_t index, size_t size) {

    jm_vector(jm_voidp)* parseBuffer = &context->parseBuffer;
    jm_vector(char) * item = jm_vector_get_item(jm_voidp)(parseBuffer,index);
    if(!item) {
        item = jm_vector_alloc(char)(size,size,context->callbacks);
        jm_vector_set_item(jm_voidp)(parseBuffer,index,item);
        if(!item) {
            fmi3_xml_parse_fatal(context, "Could not allocate a buffer for parsing XML");
            return 0;
        }
    }
    else {
        if(jm_vector_resize(char)(item, size) < size ) {
            fmi3_xml_parse_fatal(context, "Could not allocate a buffer for parsing XML");
            return 0;
        }
    }
    return item;
}

jm_vector(char) * fmi3_xml_get_parse_buffer(fmi3_xml_parser_context_t *context, size_t index) {
    jm_vector(jm_voidp)* parseBuffer = &context->parseBuffer;
    return jm_vector_get_item(jm_voidp)(parseBuffer,index);
}



int fmi3_create_attr_map(fmi3_xml_parser_context_t* context) {
    int i;
    context->attrBuffer = jm_vector_alloc(jm_string)(fmi3_xml_attr_number, fmi3_xml_attr_number, context->callbacks);
    if(!context->attrBuffer) return -1;
    context->attrMap = jm_vector_alloc(jm_named_ptr)(fmi3_xml_attr_number, fmi3_xml_attr_number, context->callbacks);
    if(!context->attrMap) return -1;
    for(i = 0; i < fmi3_xml_attr_number; i++) {
        jm_named_ptr map;
        jm_vector_set_item(jm_string)(context->attrBuffer, i, 0);
        map.name = fmi3_xmlAttrNames[i];
        map.ptr = (void*)(jm_vector_get_itemp(jm_string)(context->attrBuffer, i));
        jm_vector_set_item(jm_named_ptr)(context->attrMap, i, map);
    }
    jm_vector_qsort(jm_named_ptr)(context->attrMap, jm_compare_named);
    return 0;
}

int fmi3_create_elm_map(fmi3_xml_parser_context_t* context) {
    size_t i;
    context->elmMap = jm_vector_alloc(fmi3_xml_element_handle_map_t)(fmi3_xml_elm_actual_number, fmi3_xml_elm_number, context->callbacks);
    if(!context->elmMap) return -1;
    for(i = 0; i < fmi3_xml_elm_actual_number; i++) {
        fmi3_xml_element_handle_map_t item = fmi3_element_handle_map[i];
        jm_vector_set_item(fmi3_xml_element_handle_map_t)(context->elmMap, i, item);
    }
    jm_vector_qsort(fmi3_xml_element_handle_map_t)(context->elmMap, fmi3_xml_compare_elmName);
    return 0;
}

/**
 * Sets the elementHandle and elemID for an item in the'fmi3_element_handle_map'
 * array.
 * Sample use case:
 *      When we parse an XML element name, we must know its context. For example,
 *      if we parse element "Integer", we must know if it should be handled as
 *      ModelVariables/Integer, or as SimpleType/Integer. At the
 *      "XML element start" tag for ModelVariables, we can therefore set the
 *      correct "handle" functions.
 *
 * arg elm:
 *      the name of a real XML element, the item to change will be the one in
 *      'fmi3_element_handle_map' that has this name
 * arg id:
 *      get the handle and ID corresponding to this real or alternative XML
 *      element
 */
void fmi3_xml_set_element_handle(fmi3_xml_parser_context_t *context, const char* elm, fmi3_xml_elm_enu_t id) {
    fmi3_xml_element_handle_map_t keyEl;
    fmi3_xml_element_handle_map_t* currentElMap;
	keyEl.elementName = elm;
    currentElMap = jm_vector_bsearch(fmi3_xml_element_handle_map_t)(context->elmMap, &keyEl, fmi3_xml_compare_elmName);
	
	currentElMap->elementHandle = fmi3_element_handle_map[id].elementHandle;;
	currentElMap->elemID = id;
}

/**
/* Returns true if parent element's type or super type (recursively) matches
 * the expected type.
 */
int fmi3_xml_is_valid_parent(fmi3_xml_elm_enu_t child_id, fmi3_xml_elm_enu_t parent_id) {
    fmi3_xml_elm_enu_t p_id_expected = fmi3_xml_scheme_info[child_id].parentID;

    while (parent_id != p_id_expected && parent_id != fmi3_xml_elmID_none) {
        parent_id = fmi3_xml_scheme_info[parent_id].superID;
    }
    return parent_id == p_id_expected;
}

/**
 * Returns top level super type of the element.
 */
int fmi3_xml_get_super_type_rec(fmi3_xml_elm_enu_t id) {
    fmi3_xml_elm_enu_t id_top = id;
    while (fmi3_xml_scheme_info[id_top].superID != fmi3_xml_elmID_none) {
        id_top = fmi3_xml_scheme_info[id_top].superID;
    }
    return id_top;
}

/**
/* Returns true if the top-level super types are the same.
 */
int fmi3_xml_are_same_type(fmi3_xml_elm_enu_t id1, fmi3_xml_elm_enu_t id2) {
    return fmi3_xml_get_super_type_rec(id1) == fmi3_xml_get_super_type_rec(id2);
}


static void XMLCALL fmi3_parse_element_start(void *c, const char *elm, const char **attr) {
    fmi3_xml_element_handle_map_t keyEl;
    fmi3_xml_element_handle_map_t* currentElMap;
	fmi3_xml_elm_enu_t currentID;
    int i;
    fmi3_xml_parser_context_t *context = c;
    context->has_produced_data_warning = 0;

	if (context->useAnyHandleFlg) {
		fmi3_xml_callbacks_t* anyH = context->anyHandle;
		context->anyElmCount++;
		if(anyH && anyH->startHandle) {
            int ret = anyH->startHandle(anyH->context, context->anyToolName, context->anyParent, elm, attr);            
			if(ret != 0) {
				fmi3_xml_parse_fatal(context, "User element handle returned non-zero error code %d", ret);
			}
		}
		return;
	}

	if (context->skipElementCnt) {
		context->skipElementCnt++;
        jm_log_warning(context->callbacks, module, "[Line:%u] Skipping nested XML element '%s'",
			XML_GetCurrentLineNumber(context->parser), elm);
		return;
	}
	
	keyEl.elementName = elm;
	/* find the element handle by name */
    currentElMap = jm_vector_bsearch(fmi3_xml_element_handle_map_t)(context->elmMap, &keyEl, fmi3_xml_compare_elmName);
    if (!currentElMap) {
        /* not found error*/
        jm_log_error(context->callbacks, module, "[Line:%u] Unknown element '%s' in XML, skipping",
			XML_GetCurrentLineNumber(context->parser), elm);
        /* skipElementCnt:
         *  Instead of calling exit when we find a first 'Unknown' element, we set skipElementCnt to 1, and continue parsing.
         *  We then increase 'skipElementCnt' by 1 for each nested element we encounter (and decrease by 1 when leaving it).
         *  'skipElementCnt' will be 0 when we leave the root 'Unknown' element.
         */
		context->skipElementCnt = 1;
        return;
    }

    currentID = currentElMap->elemID;
    context->currentElemIdStartTag = currentID;
	/* Check that parent-child & siblings are fine */
	{
		fmi3_xml_elm_enu_t parentID = context->currentElmID;
		fmi3_xml_elm_enu_t siblingID =  context->lastElmID;

		if (!fmi3_xml_is_valid_parent(currentID, parentID)) {
				jm_log_error(context->callbacks, module, 
					"[Line:%u] XML element '%s' cannot be placed inside '%s', skipping",
					XML_GetCurrentLineNumber(context->parser), elm, fmi3_element_handle_map[parentID].elementName);
				context->skipElementCnt = 1;
				return;
		}
		if (siblingID != fmi3_xml_elmID_none) {
            if (fmi3_xml_are_same_type(currentID, siblingID)) {
				if (!(fmi3_xml_scheme_info[currentID].multipleAllowed && fmi3_xml_scheme_info[siblingID].multipleAllowed)) {
					jm_log_error(context->callbacks, module, 
						"[Line:%u] Multiple instances of XML element '%s' are not allowed, skipping",
						XML_GetCurrentLineNumber(context->parser), elm);
					context->skipElementCnt = 1;
					return;
				}
			} else {
				int lastSiblingIndex = fmi3_xml_scheme_info[siblingID].siblingIndex;
				int curSiblingIndex = fmi3_xml_scheme_info[currentID].siblingIndex;

                if (lastSiblingIndex >= curSiblingIndex) {
					jm_log_error(context->callbacks, module, 
						"[Line:%u] XML element '%s' cannot be placed after element '%s', skipping",
						XML_GetCurrentLineNumber(context->parser), elm, fmi3_element_handle_map[siblingID].elementName);
					context->skipElementCnt = 1;
					return;
				}
			}
		}
        /* lastElmID references the previous sibling-element's id - set it to 'none' before handling children*/
        /* TODO: rename lastElmName --> lastSiblingElemId, in */
		context->lastElmID = fmi3_xml_elmID_none;
	}

    /* process the attributes  */
    i = 0;
    while (attr[i]) {
        jm_named_ptr key;
        jm_named_ptr* attrMapping;

        /* find attribute by name */
        key.name = attr[i];
        attrMapping = jm_vector_bsearch(jm_named_ptr)(context->attrMap, &key, jm_compare_named);
        if (!attrMapping) {
#define XMLSchema_instance "http://www.w3.org/2001/XMLSchema-instance"
			const size_t stdNSlen = strlen(XMLSchema_instance);
            const size_t attrStrLen = strlen(attr[i]);
			if((attrStrLen > stdNSlen) && (attr[i][stdNSlen] == '|') && (strncmp(attr[i], XMLSchema_instance, stdNSlen) == 0)) {
				const char* localName = attr[i] + stdNSlen + 1;
				if(	strcmp(localName, "noNamespaceSchemaLocation") == 0)
					jm_log_warning(context->callbacks, module, "Attribute noNamespaceSchemaLocation='%s' is ignored. Using standard fmiModelDescription.xsd.",
					attr[i+1]);
				else if((strcmp(localName, "nil") == 0)
					||  (strcmp(localName, "type") == 0)) {
						jm_log_warning(context->callbacks, module, "Attribute {" XMLSchema_instance "}%s=%s is ignored",
							localName, attr[i+1]);
				}
				else if(strcmp(localName, "schemaLocation") == 0) {
					/* just skip this */
				}
				else {
					jm_log_error(context->callbacks, module, "Unknown attribute '%s=%s' in XML", attr[i], attr[i+1]);
				}
			}
			else if(
				(strcmp("providesPartialDerivativesOf_DerivativeFunction_wrt_States", attr[i]) == 0) ||
				(strcmp("providesPartialDerivativesOf_DerivativeFunction_wrt_Inputs", attr[i]) == 0) ||
				(strcmp("providesPartialDerivativesOf_OutputFunction_wrt_States", attr[i]) == 0) ||
				(strcmp("providesPartialDerivativesOf_OutputFunction_wrt_Inputs", attr[i]) == 0)
				) {
					jm_log_warning(context->callbacks, module, 
						"FMI API function fmiGetPartialDerivatives is removed from the specification. Attribute %s will be ignored.", attr[i]);
			}
			else {
				/* not found error*/
				jm_log_error(context->callbacks, module, "Unknown attribute '%s=%s' in XML", attr[i], attr[i+1]);
			}
        } else  {
            /* save attr value (still as string) for further handling  */
            const char** mapItem = (const char**)attrMapping->ptr;
            *mapItem = attr[i+1];
        }
        i += 2;
    }

    /* handle the element */
	if ( currentElMap->elementHandle(context, 0) ) {
		/* try to skip and continue anyway */
        if(!context->skipElementCnt) context->skipElementCnt = 1; 
    }
	if (context->skipElementCnt) return;
    /* check that the element handler has processed all the attributes */
    for (i = 0; i < fmi3_xml_attr_number; i++) {
        if (jm_vector_get_item(jm_string)(context->attrBuffer, i)) {
            if (!context->skipOneVariableFlag)
                jm_log_warning(context->callbacks,module, "Attribute '%s' not processed by element '%s' handle", fmi3_xmlAttrNames[i], elm);
            jm_vector_set_item(jm_string)(context->attrBuffer, i,0);
        }
    }
    if (context -> currentElmID != fmi3_xml_elmID_none) { /* with nested elements: put the parent on the stack */
        jm_stack_push(int)(&context->elmStack, context -> currentElmID);
    }
    context -> currentElmID = currentID;
}

static void XMLCALL fmi3_parse_element_end(void* c, const char *elm) {

    fmi3_xml_element_handle_map_t keyEl;
    fmi3_xml_element_handle_map_t* currentElMap;
	fmi3_xml_elm_enu_t currentID;
    fmi3_xml_parser_context_t *context = c;

	if(context->useAnyHandleFlg && (context->anyElmCount > 0)) {
		fmi3_xml_callbacks_t* anyH = context->anyHandle;
		context->anyElmCount--;
		if(anyH && anyH->endHandle) {
			int ret = anyH->endHandle(anyH->context, elm);
			if(ret != 0) {
				fmi3_xml_parse_fatal(context, "User element handle returned non-zero error code %d", ret);
			}
		}
		return;
	}

	if(context->skipElementCnt) {
		context->skipElementCnt--;
		return;
	}

    keyEl.elementName = elm;
    currentElMap = jm_vector_bsearch(fmi3_xml_element_handle_map_t)(context->elmMap, &keyEl, fmi3_xml_compare_elmName);
    if(!currentElMap) {
        /* not found error*/
        fmi3_xml_parse_fatal(context, "Unknown element end in XML (element: %s)", elm);
        return;
    }
    currentID = currentElMap->elemID;

    if(currentID != context -> currentElmID) {
        /* missmatch error */
        fmi3_xml_parse_fatal(context, "Element end '%s' does not match element start '%s' in XML", elm, 
			fmi3_element_handle_map[context -> currentElmID].elementName);
        return;
    }

    jm_vector_push_back(char)(&context->elmData, 0);

	if( currentElMap->elementHandle(context, jm_vector_get_itemp(char)(&context->elmData, 0) )) {
        /* context->modelDescription->hasParsingError = 1;*/
        return;
    }
    jm_vector_resize(char)(&context->elmData, 0);

    /* record the last handle and pop the stack */
    context->lastElmID = currentID;

    if(jm_stack_is_empty(int)(&context->elmStack)) {
        context -> currentElmID = fmi3_xml_elmID_none;
    }
    else {
        context -> currentElmID = (fmi3_xml_elm_enu_t)jm_stack_pop(int)(&context->elmStack);
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
static void XMLCALL fmi3_parse_element_data(void* c, const XML_Char *s, int len) {
		int i;
        fmi3_xml_parser_context_t *context = c;
		if(context->useAnyHandleFlg && (context->anyElmCount > 0)) {
			fmi3_xml_callbacks_t* anyH = context->anyHandle;
			if(anyH && anyH->dataHandle) {
				int ret = anyH->dataHandle(anyH->context, s, len);
				if(ret != 0) {
					fmi3_xml_parse_fatal(context, "User element handle returned non-zero error code %d", ret);
				}
			}
			return;
		}
		if(context->skipElementCnt) {
			return;
		}
		for(i = 0; i< len;i++) {
            char ch = s[i];
            if((ch != '\n') && (ch != ' ') && (ch != '\t')) {
                break;
            }
        }

		if((i != len) && !context->has_produced_data_warning) {
			jm_log_warning(context->callbacks, module, "[Line:%u] Skipping unexpected XML element data",
					XML_GetCurrentLineNumber(context->parser));
			context->has_produced_data_warning = 1;
		}
}

void fmi3_check_variable_naming_conventions(fmi3_xml_model_description_t *md) {
    size_t n = jm_vector_get_size(jm_named_ptr)(&md->variablesByName);
    size_t k;
    yyscan_t scanner;
    YY_BUFFER_STATE buf;

    /* check for duplicate variable names */
    for (k = 1; k < n; k++) {
        const char *v1 = jm_vector_get_item(jm_named_ptr)(&md->variablesByName, k - 1).name;
        const char *v2 = jm_vector_get_item(jm_named_ptr)(&md->variablesByName, k).name;
        if(strcmp(v1, v2) == 0) {
            jm_log_error(md->callbacks, module,
                    "Two variables with the same name %s found. This is not allowed.",
                    v1);
        }
    }

    /* check variable name syntax */
    if (md->namingConvension == fmi3_naming_enu_structured) {
        yyfmi3lex_init(&scanner);
        for (k = 0; k < n; k++) {
            char *name = ((fmi3_xml_variable_t *) jm_vector_get_item(jm_voidp)(
                    md->variablesOrigOrder, k))->name;
            buf = yyfmi3_scan_string(name, scanner);
            yyfmi3parse(scanner, md->callbacks, name);
            yyfmi3_delete_buffer(buf, scanner);
        }
        yyfmi3lex_destroy(scanner);
    }
}

int fmi3_xml_parse_model_description(fmi3_xml_model_description_t* md,
                                     const char* filename,
                                     fmi3_xml_callbacks_t* xml_callbacks,
                                     int configuration) {
    XML_Memory_Handling_Suite memsuite;
    fmi3_xml_parser_context_t* context;
    XML_Parser parser = NULL;
    FILE* file;

    context = (fmi3_xml_parser_context_t*)md->callbacks->calloc(1, sizeof(fmi3_xml_parser_context_t));
    if(!context) {
        jm_log_fatal(md->callbacks, "FMIXML", "Could not allocate memory for XML parser context");
    }
    context->callbacks = md->callbacks;
    context->modelDescription = md;
    if(fmi3_xml_alloc_parse_buffer(context, 16)) return -1;
    if(fmi3_create_attr_map(context) || fmi3_create_elm_map(context)) {
        fmi3_xml_parse_fatal(context, "Error in parsing initialization");
        fmi3_xml_parse_free_context(context);
        return -1;
    }
    context->lastBaseUnit = 0;
    context->skipOneVariableFlag = 0;
	context->skipElementCnt = 0;
    jm_stack_init(int)(&context->elmStack,  context->callbacks);
    jm_vector_init(char)(&context->elmData, 0, context->callbacks);
    context->lastElmID = fmi3_xml_elmID_none;
    context->currentElmID = fmi3_xml_elmID_none;
	context->anyElmCount = 0;
	context->useAnyHandleFlg = 0;
    context->anyParent = 0;
	context->anyHandle = xml_callbacks;

    memsuite.malloc_fcn = context->callbacks->malloc;
    memsuite.realloc_fcn = context->callbacks->realloc;
    memsuite.free_fcn = context->callbacks->free;
    context -> parser = parser = XML_ParserCreate_MM(0, &memsuite, "|");

    if(! parser) {
        fmi3_xml_parse_fatal(context, "Could not initialize XML parsing library.");
        fmi3_xml_parse_free_context(context);
        return -1;
    }

    XML_SetUserData( parser, context);

    XML_SetElementHandler(parser, fmi3_parse_element_start, fmi3_parse_element_end);

    XML_SetCharacterDataHandler(parser, fmi3_parse_element_data);

    file = fopen(filename, "rb");
    if (file == NULL) {
        fmi3_xml_parse_fatal(context, "Cannot open file '%s' for parsing", filename);
        fmi3_xml_parse_free_context(context);
        return -1;
    }

    while (!feof(file)) {
        char * text = jm_vector_get_itemp(char)(fmi3_xml_reserve_parse_buffer(context,0,XML_BLOCK_SIZE),0);
        int n = (int)fread(text, sizeof(char), XML_BLOCK_SIZE, file);
        if(ferror(file)) {
            fmi3_xml_parse_fatal(context, "Error reading from file %s", filename);
            fclose(file);
	        fmi3_xml_parse_free_context(context);
            return -1;
        }
        if (!XML_Parse(parser, text, n, feof(file))) {
             fmi3_xml_parse_fatal(context, "Parse error at line %d:\n%s",
                         (int)XML_GetCurrentLineNumber(parser),
                         XML_ErrorString(XML_GetErrorCode(parser)));
             fclose(file);
		     fmi3_xml_parse_free_context(context);
             return -1; /* failure */
        }        
    }
    fclose(file);
    /* done later XML_ParserFree(parser);*/
    if(!jm_stack_is_empty(int)(&context->elmStack)) {
        fmi3_xml_parse_fatal(context, "Unexpected end of file (not all elements ended) when parsing %s", filename);
        fmi3_xml_parse_free_context(context);
        return -1;
    }

    if (configuration & FMI3_XML_NAME_CHECK) {
        fmi3_check_variable_naming_conventions(md);
    }

    md->status = fmi3_xml_model_description_enu_ok;
    context->modelDescription = 0;
    fmi3_xml_parse_free_context(context);

    return 0;
}

#define JM_TEMPLATE_INSTANCE_TYPE fmi3_xml_element_handle_map_t
#include "JM/jm_vector_template.h"
