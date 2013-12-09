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

#ifndef FMI2_XML_XMLPARSER_H
#define FMI2_XML_XMLPARSER_H

#include <expat.h>

#include <JM/jm_vector.h>
#include <JM/jm_stack.h>
#include <JM/jm_named_ptr.h>
#include <FMI2/fmi2_xml_callbacks.h>

#include <FMI2/fmi2_enums.h>
#include <FMI2/fmi2_xml_model_description.h>


#ifdef __cplusplus
extern "C" {
#endif

#define FMI2_XML_ATTRLIST(EXPAND_XML_ATTRNAME) \
    EXPAND_XML_ATTRNAME(fmiVersion) \
    EXPAND_XML_ATTRNAME(factor) \
    EXPAND_XML_ATTRNAME(offset) \
	FMI2_SI_BASE_UNITS(EXPAND_XML_ATTRNAME) \
    EXPAND_XML_ATTRNAME(name) \
    EXPAND_XML_ATTRNAME(description) \
    EXPAND_XML_ATTRNAME(quantity) \
    EXPAND_XML_ATTRNAME(unit) \
    EXPAND_XML_ATTRNAME(displayUnit) \
    EXPAND_XML_ATTRNAME(relativeQuantity) \
    EXPAND_XML_ATTRNAME(unbounded) \
	EXPAND_XML_ATTRNAME(min) \
    EXPAND_XML_ATTRNAME(max) \
    EXPAND_XML_ATTRNAME(nominal) \
    EXPAND_XML_ATTRNAME(declaredType) \
    EXPAND_XML_ATTRNAME(start) \
    EXPAND_XML_ATTRNAME(derivative) \
    EXPAND_XML_ATTRNAME(reinit) \
    EXPAND_XML_ATTRNAME(startTime) \
    EXPAND_XML_ATTRNAME(stopTime) \
    EXPAND_XML_ATTRNAME(tolerance) \
    EXPAND_XML_ATTRNAME(stepSize) \
    EXPAND_XML_ATTRNAME(value) \
    EXPAND_XML_ATTRNAME(valueReference) \
    EXPAND_XML_ATTRNAME(variability) \
    EXPAND_XML_ATTRNAME(causality) \
    EXPAND_XML_ATTRNAME(initial) \
    EXPAND_XML_ATTRNAME(previous) \
    EXPAND_XML_ATTRNAME(canHandleMultipleSetPerTimeInstant) \
    EXPAND_XML_ATTRNAME(index) \
    EXPAND_XML_ATTRNAME(dependencies) \
    EXPAND_XML_ATTRNAME(dependenciesKind) \
    EXPAND_XML_ATTRNAME(modelName) \
    EXPAND_XML_ATTRNAME(modelIdentifier) \
    EXPAND_XML_ATTRNAME(guid) \
    EXPAND_XML_ATTRNAME(author) \
    EXPAND_XML_ATTRNAME(copyright) \
    EXPAND_XML_ATTRNAME(license) \
    EXPAND_XML_ATTRNAME(version) \
    EXPAND_XML_ATTRNAME(generationTool) \
    EXPAND_XML_ATTRNAME(generationDateAndTime) \
    EXPAND_XML_ATTRNAME(variableNamingConvention) \
    EXPAND_XML_ATTRNAME(numberOfEventIndicators) \
    EXPAND_XML_ATTRNAME(input) \
    EXPAND_XML_ATTRNAME(needsExecutionTool) \
    EXPAND_XML_ATTRNAME(canHandleVariableCommunicationStepSize) \
	EXPAND_XML_ATTRNAME(completedIntegratorStepNotNeeded) \
	EXPAND_XML_ATTRNAME(canBeInstantiatedOnlyOncePerProcess) \
	EXPAND_XML_ATTRNAME(canNotUseMemoryManagementFunctions) \
	EXPAND_XML_ATTRNAME(canGetAndSetFMUstate) \
	EXPAND_XML_ATTRNAME(canSerializeFMUstate) \
	EXPAND_XML_ATTRNAME(providesDirectionalDerivatives) /* Beta4 spelling. TODO: remove */ \
	EXPAND_XML_ATTRNAME(providesDirectionalDerivative) \
    EXPAND_XML_ATTRNAME(canInterpolateInputs) \
    EXPAND_XML_ATTRNAME(maxOutputDerivativeOrder) \
    EXPAND_XML_ATTRNAME(canRunAsynchronuously)

#define FMI2_XML_ATTR_ID(attr) fmi_attr_id_##attr,
typedef enum fmi2_xml_attr_enu_t {
    FMI2_XML_ATTRLIST(FMI2_XML_ATTR_ID)
    fmi2_xml_attr_number
} fmi2_xml_attr_enu_t;

/** \brief Element names used in XML */
#define FMI2_XML_ELMLIST(EXPAND_XML_ELMNAME) \
    EXPAND_XML_ELMNAME(fmiModelDescription) \
	EXPAND_XML_ELMNAME(ModelExchange) \
    EXPAND_XML_ELMNAME(CoSimulation) \
    EXPAND_XML_ELMNAME(SourceFiles) \
    EXPAND_XML_ELMNAME(File) \
    EXPAND_XML_ELMNAME(UnitDefinitions) \
    EXPAND_XML_ELMNAME(Unit) \
    EXPAND_XML_ELMNAME(BaseUnit) \
    EXPAND_XML_ELMNAME(DisplayUnit) \
    EXPAND_XML_ELMNAME(TypeDefinitions) \
    EXPAND_XML_ELMNAME(SimpleType) \
    EXPAND_XML_ELMNAME(Item) \
    EXPAND_XML_ELMNAME(DefaultExperiment) \
    EXPAND_XML_ELMNAME(VendorAnnotations) \
    EXPAND_XML_ELMNAME(Tool) \
    EXPAND_XML_ELMNAME(ModelVariables) \
    EXPAND_XML_ELMNAME(ScalarVariable) \
    EXPAND_XML_ELMNAME(Annotations) \
	EXPAND_XML_ELMNAME(LogCategories) \
	EXPAND_XML_ELMNAME(Category) \
    EXPAND_XML_ELMNAME(Real) \
    EXPAND_XML_ELMNAME(Integer) \
    EXPAND_XML_ELMNAME(Boolean) \
    EXPAND_XML_ELMNAME(String) \
    EXPAND_XML_ELMNAME(Enumeration) \
    EXPAND_XML_ELMNAME(ModelStructure) \
    EXPAND_XML_ELMNAME(Outputs) \
    EXPAND_XML_ELMNAME(Derivatives) \
    EXPAND_XML_ELMNAME(DiscreteStates) \
    EXPAND_XML_ELMNAME(InitialUnknowns) \
    EXPAND_XML_ELMNAME(Unknown)


/** \brief Element that can be placed under different parents get alternative names from the info struct */
#define FMI2_XML_ELMLIST_ALT(EXPAND_XML_ELMNAME) \
    EXPAND_XML_ELMNAME(RealVariable) \
    EXPAND_XML_ELMNAME(IntegerVariable) \
    EXPAND_XML_ELMNAME(BooleanVariable) \
    EXPAND_XML_ELMNAME(StringVariable) \
    EXPAND_XML_ELMNAME(EnumerationVariable)  \
    EXPAND_XML_ELMNAME(VariableTool) \
    EXPAND_XML_ELMNAME(SourceFilesCS) \
    EXPAND_XML_ELMNAME(FileCS) \
    EXPAND_XML_ELMNAME(DerivativeUnknown) \
    EXPAND_XML_ELMNAME(DiscreteStateUnknown) \
    EXPAND_XML_ELMNAME(InitialUnknown)


typedef struct fmi2_xml_parser_context_t fmi2_xml_parser_context_t;
#define EXPAND_ELM_HANDLE(elm) extern int fmi2_xml_handle_##elm(fmi2_xml_parser_context_t *context, const char* data);
FMI2_XML_ELMLIST(EXPAND_ELM_HANDLE)
FMI2_XML_ELMLIST_ALT(EXPAND_ELM_HANDLE)

#define FMI2_XML_ELM_ID(elm) fmi2_xml_elmID_##elm
#define FMI2_XML_LIST_ELM_ID(elm) ,FMI2_XML_ELM_ID(elm)
typedef enum fmi2_xml_elm_enu_t {
	fmi2_xml_elmID_none = -1
    FMI2_XML_ELMLIST(FMI2_XML_LIST_ELM_ID)
	,fmi2_xml_elm_actual_number
	FMI2_XML_ELMLIST_ALT(FMI2_XML_LIST_ELM_ID)
    ,fmi2_xml_elm_number
} fmi2_xml_elm_enu_t;

typedef int (*fmi2_xml_element_handle_ft)(fmi2_xml_parser_context_t *context, const char* data);

typedef struct fmi2_xml_element_handle_map_t fmi2_xml_element_handle_map_t;

/** Keeps information about the allowed parent element ID, index among siblings in a sequence and if
	multiple elements of this type are allowed in a sequence.
*/
typedef struct {
	fmi2_xml_elm_enu_t parentID; /* expected parent ID for an element */
	int siblingIndex;       /* index among siblings */
	int multipleAllowed;	/* multiple elements of this kind kan come in a sequence as siblings*/
} fmi2_xml_scheme_info_t;

struct fmi2_xml_element_handle_map_t {
    const char* elementName;
    fmi2_xml_element_handle_ft elementHandle;
	fmi2_xml_elm_enu_t elemID;
};


jm_vector_declare_template(fmi2_xml_element_handle_map_t)

#define fmi2_xml_diff_elmName(a, b) strcmp(a.elementName,b.elementName)

jm_define_comp_f(fmi2_xml_compare_elmName, fmi2_xml_element_handle_map_t, fmi2_xml_diff_elmName)

#define XML_BLOCK_SIZE 16000

struct fmi2_xml_parser_context_t {
    fmi2_xml_model_description_t* modelDescription;
    jm_callbacks* callbacks;

    XML_Parser parser;
    jm_vector(jm_voidp) parseBuffer;

    jm_vector(jm_named_ptr)* attrMap;
    jm_vector(fmi2_xml_element_handle_map_t)* elmMap;
    jm_vector(jm_string)* attrBuffer;

    fmi2_xml_unit_t* lastBaseUnit;

    int skipOneVariableFlag;
	int skipElementCnt;
	int has_produced_data_warning;

    jm_stack(int) elmStack;
    jm_vector(char) elmData;

	fmi2_xml_elm_enu_t lastElmID;
	fmi2_xml_elm_enu_t currentElmID;

	int anyElmCount;
	int useAnyHandleFlg;
	char* anyToolName;
	void* anyParent;
	fmi2_xml_callbacks_t* anyHandle;
};

jm_vector(char) * fmi2_xml_reserve_parse_buffer(fmi2_xml_parser_context_t *context, size_t index, size_t size);
jm_vector(char) * fmi2_xml_get_parse_buffer(fmi2_xml_parser_context_t *context, size_t index);
int fmi2_xml_alloc_parse_buffer(fmi2_xml_parser_context_t *context, size_t items);

void fmi2_xml_free_parse_buffer(fmi2_xml_parser_context_t *context);

void fmi2_xml_parse_fatal(fmi2_xml_parser_context_t *context, const char* fmt, ...);
void fmi2_xml_parse_error(fmi2_xml_parser_context_t *context, const char* fmt, ...);

int fmi2_xml_set_attr_string(fmi2_xml_parser_context_t *context, fmi2_xml_elm_enu_t elmID, fmi2_xml_attr_enu_t attrID, int required, jm_vector(char)* field);
int fmi2_xml_set_attr_uint(fmi2_xml_parser_context_t *context, fmi2_xml_elm_enu_t elmID, fmi2_xml_attr_enu_t attrID, int required, unsigned int* field, unsigned int defaultVal);
int fmi2_xml_set_attr_enum(fmi2_xml_parser_context_t *context, fmi2_xml_elm_enu_t elmID, fmi2_xml_attr_enu_t attrID, int required, unsigned int* field, unsigned int defaultVal, jm_name_ID_map_t* nameMap);
int fmi2_xml_set_attr_boolean(fmi2_xml_parser_context_t *context, fmi2_xml_elm_enu_t elmID, fmi2_xml_attr_enu_t attrID, int required, unsigned int* field, unsigned int defaultVal);
int fmi2_xml_set_attr_int(fmi2_xml_parser_context_t *context, fmi2_xml_elm_enu_t elmID, fmi2_xml_attr_enu_t attrID, int required, int* field, int defaultVal);
int fmi2_xml_set_attr_double(fmi2_xml_parser_context_t *context, fmi2_xml_elm_enu_t elmID, fmi2_xml_attr_enu_t attrID, int required, double* field, double defaultVal);
int fmi2_xml_is_attr_defined(fmi2_xml_parser_context_t *context, fmi2_xml_attr_enu_t attrID);
int fmi2_xml_get_attr_str(fmi2_xml_parser_context_t *context, fmi2_xml_elm_enu_t elmID, fmi2_xml_attr_enu_t attrID, int required,const char** valp);

void fmi2_xml_set_element_handle(fmi2_xml_parser_context_t *context, const char* elm, fmi2_xml_elm_enu_t id);


#ifdef __cplusplus
}
#endif

#endif /* FMI2_XML_XMLPARSER_H */

