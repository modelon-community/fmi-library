/*
    Copyright (C) 2012-2023 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

     This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#ifndef FMI3_XML_XMLPARSER_H
#define FMI3_XML_XMLPARSER_H

#include <expat.h>

#include <JM/jm_vector.h>
#include <JM/jm_stack.h>
#include <JM/jm_named_ptr.h>
#include <JM/jm_portability.h>
#include <FMI3/fmi3_xml_callbacks.h>

#include <FMI3/fmi3_enums.h>
#include <FMI3/fmi3_xml_model_description.h>


#ifdef __cplusplus
extern "C" {
#endif


jm_vector_declare_template(fmi3_value_reference_t)

#define FMI3_XML_ATTRLIST(EXPAND_XML_ATTRNAME) \
    EXPAND_XML_ATTRNAME(fmiVersion) \
    EXPAND_XML_ATTRNAME(factor) \
    EXPAND_XML_ATTRNAME(offset) \
    EXPAND_XML_ATTRNAME(inverse) \
    FMI3_SI_BASE_UNITS(EXPAND_XML_ATTRNAME) \
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
    EXPAND_XML_ATTRNAME(clocks) \
    EXPAND_XML_ATTRNAME(canHandleMultipleSetPerTimeInstant) \
    EXPAND_XML_ATTRNAME(mimeType) \
    EXPAND_XML_ATTRNAME(maxSize) \
    EXPAND_XML_ATTRNAME(intervalVariability) \
    EXPAND_XML_ATTRNAME(canBeDeactivated) \
    EXPAND_XML_ATTRNAME(priority) \
    EXPAND_XML_ATTRNAME(intervalDecimal) \
    EXPAND_XML_ATTRNAME(shiftDecimal) \
    EXPAND_XML_ATTRNAME(supportsFraction) \
    EXPAND_XML_ATTRNAME(resolution) \
    EXPAND_XML_ATTRNAME(intervalCounter) \
    EXPAND_XML_ATTRNAME(shiftCounter) \
    EXPAND_XML_ATTRNAME(dependencies) \
    EXPAND_XML_ATTRNAME(dependenciesKind) \
    EXPAND_XML_ATTRNAME(modelName) \
    EXPAND_XML_ATTRNAME(modelIdentifier) \
    EXPAND_XML_ATTRNAME(instantiationToken) \
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
    EXPAND_XML_ATTRNAME(canBeInstantiatedOnlyOncePerProcess) \
    EXPAND_XML_ATTRNAME(canGetAndSetFMUState) \
    EXPAND_XML_ATTRNAME(canSerializeFMUState) \
    EXPAND_XML_ATTRNAME(providesDirectionalDerivatives) \
    EXPAND_XML_ATTRNAME(providesDirectionalDerivative) /* Removed in FMI3. Used in error checking. */ \
    EXPAND_XML_ATTRNAME(providesAdjointDerivatives) \
    EXPAND_XML_ATTRNAME(providesPerElementDependencies) \
    EXPAND_XML_ATTRNAME(providesEvaluateDiscreteStates) \
    EXPAND_XML_ATTRNAME(needsCompletedIntegratorStep) \
    EXPAND_XML_ATTRNAME(canHandleVariableCommunicationStepSize) \
    EXPAND_XML_ATTRNAME(fixedInternalStepSize) \
    EXPAND_XML_ATTRNAME(maxOutputDerivativeOrder) \
    EXPAND_XML_ATTRNAME(recommendedIntermediateInputSmoothness) \
    EXPAND_XML_ATTRNAME(providesIntermediateUpdate) \
    EXPAND_XML_ATTRNAME(mightReturnEarlyFromDoStep) \
    EXPAND_XML_ATTRNAME(canReturnEarlyAfterIntermediateUpdate) \
    EXPAND_XML_ATTRNAME(hasEventMode)


#define FMI3_XML_ATTR_ID(attr) fmi_attr_id_##attr,
typedef enum fmi3_xml_attr_enu_t {
    FMI3_XML_ATTRLIST(FMI3_XML_ATTR_ID)
    fmi3_xml_attr_number
} fmi3_xml_attr_enu_t;

/** \brief Element names used in XML */
#define FMI3_XML_ELMLIST(EXPAND_XML_ELMNAME) \
    EXPAND_XML_ELMNAME(fmiModelDescription) \
    EXPAND_XML_ELMNAME(ModelExchange) \
    EXPAND_XML_ELMNAME(CoSimulation) \
    EXPAND_XML_ELMNAME(ScheduledExecution) \
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
    EXPAND_XML_ELMNAME(Dimension) \
    EXPAND_XML_ELMNAME(Start) \
    EXPAND_XML_ELMNAME(Alias) \
    EXPAND_XML_ELMNAME(Annotations) \
    EXPAND_XML_ELMNAME(LogCategories) \
    EXPAND_XML_ELMNAME(Category) \
    EXPAND_XML_ELMNAME(Float64) \
    EXPAND_XML_ELMNAME(Float32) \
    EXPAND_XML_ELMNAME(Int64) \
    EXPAND_XML_ELMNAME(Int32) \
    EXPAND_XML_ELMNAME(Int16) \
    EXPAND_XML_ELMNAME(Int8) \
    EXPAND_XML_ELMNAME(UInt64) \
    EXPAND_XML_ELMNAME(UInt32) \
    EXPAND_XML_ELMNAME(UInt16) \
    EXPAND_XML_ELMNAME(UInt8) \
    EXPAND_XML_ELMNAME(Boolean) \
    EXPAND_XML_ELMNAME(Binary) \
    EXPAND_XML_ELMNAME(Clock) \
    EXPAND_XML_ELMNAME(String) \
    EXPAND_XML_ELMNAME(Enumeration) \
    EXPAND_XML_ELMNAME(ModelStructure) \
    EXPAND_XML_ELMNAME(Output) \
    EXPAND_XML_ELMNAME(ContinuousStateDerivative) \
    EXPAND_XML_ELMNAME(ClockedState) \
    EXPAND_XML_ELMNAME(InitialUnknown) \
    EXPAND_XML_ELMNAME(EventIndicator)

/** \brief Element that can be placed under different parents get alternative names from the info struct */
#define FMI3_XML_ELMLIST_ALT(EXPAND_XML_ELMNAME) \
    EXPAND_XML_ELMNAME(Float64Variable) \
    EXPAND_XML_ELMNAME(Float32Variable) \
    EXPAND_XML_ELMNAME(Int64Variable) \
    EXPAND_XML_ELMNAME(Int32Variable) \
    EXPAND_XML_ELMNAME(Int16Variable) \
    EXPAND_XML_ELMNAME(Int8Variable) \
    EXPAND_XML_ELMNAME(UInt64Variable) \
    EXPAND_XML_ELMNAME(UInt32Variable) \
    EXPAND_XML_ELMNAME(UInt16Variable) \
    EXPAND_XML_ELMNAME(UInt8Variable) \
    EXPAND_XML_ELMNAME(BooleanVariable) \
    EXPAND_XML_ELMNAME(BinaryVariable) \
    EXPAND_XML_ELMNAME(ClockVariable) \
    EXPAND_XML_ELMNAME(StringVariable) \
    EXPAND_XML_ELMNAME(EnumerationVariable) \
    EXPAND_XML_ELMNAME(BinaryVariableStart) \
    EXPAND_XML_ELMNAME(StringVariableStart) \
    EXPAND_XML_ELMNAME(VariableTool) \
    EXPAND_XML_ELMNAME(SourceFilesCS) \
    EXPAND_XML_ELMNAME(FileCS)

/** \brief Abstract elements that are only used in the scheme verification */
#define FMI3_XML_ELMLIST_ABSTRACT(EXPAND_XML_ELMNAME) \
    EXPAND_XML_ELMNAME(Variable)

/* Build prototypes for all elm_handle_* functions */
typedef struct fmi3_xml_parser_context_t fmi3_xml_parser_context_t;
#define EXPAND_ELM_HANDLE(elm) extern int fmi3_xml_handle_##elm(fmi3_xml_parser_context_t *context, const char* data);
FMI3_XML_ELMLIST(EXPAND_ELM_HANDLE)
FMI3_XML_ELMLIST_ALT(EXPAND_ELM_HANDLE)
FMI3_XML_ELMLIST_ABSTRACT(EXPAND_ELM_HANDLE)

/**
 * Create an enum over all XML elements. This enum can be used to index
 * the following arrays:
 *      - fmi3_xml_scheme_info
 *      - fmi3_element_handle_map
 */
#define FMI3_XML_ELM_ID(elm) fmi3_xml_elmID_##elm
#define FMI3_XML_LIST_ELM_ID(elm) ,FMI3_XML_ELM_ID(elm)
typedef enum fmi3_xml_elm_enu_t {
    fmi3_xml_elmID_none = -1
    FMI3_XML_ELMLIST(FMI3_XML_LIST_ELM_ID)
    ,fmi3_xml_elm_actual_number
    FMI3_XML_ELMLIST_ALT(FMI3_XML_LIST_ELM_ID)
    FMI3_XML_ELMLIST_ABSTRACT(FMI3_XML_LIST_ELM_ID)
    ,fmi3_xml_elm_number
} fmi3_xml_elm_enu_t;

typedef int (*fmi3_xml_element_handle_ft)(fmi3_xml_parser_context_t *context, const char* data);

typedef struct fmi3_xml_element_handle_map_t fmi3_xml_element_handle_map_t;

/** Keeps information about the allowed parent element ID, index among siblings in a sequence and if
    multiple elements of this type are allowed in a sequence.
*/
typedef struct {
    fmi3_xml_elm_enu_t superID; /* ID of super type or NULL if none */
    fmi3_xml_elm_enu_t parentID; /* expected parent ID for an element */
    int siblingIndex;       /* index among siblings */
    int multipleAllowed;    /* multiple elements of this kind kan come in a sequence as siblings*/
} fmi3_xml_scheme_info_t;

struct fmi3_xml_element_handle_map_t {
    const char* elementName;
    fmi3_xml_element_handle_ft elementHandle;
    fmi3_xml_elm_enu_t elemID;
};


jm_vector_declare_template(fmi3_xml_element_handle_map_t)

#define fmi3_xml_diff_elmName(a, b) strcmp(a.elementName,b.elementName)

jm_define_comp_f(fmi3_xml_compare_elmName, fmi3_xml_element_handle_map_t, fmi3_xml_diff_elmName)

#define XML_BLOCK_SIZE 16000

/**
 * Struct for saving and accessing data between element handlers.
 */
struct fmi3_xml_parser_context_t {

    /**
     * This is where the parsed XML is saved.
     */
    fmi3_xml_model_description_t* modelDescription;

    jm_callbacks* callbacks;

    XML_Parser parser;

    /* Temporary storage of start values for String and Binary variables. */
    jm_vector(jm_voidp) currentStartVariableValues;

    /**
     * Actual type: jm_vector of jm_vector(char).
     *
     * Purpose seems to be to reduce the cost of converting char* to jm_vector(char)
     * by using an object pool. Typically for attributes.
     *
     * It's possible that it was also used as a way to keep strings from one element
     * handler to another, but that's not how it should be used moving forward.
     *
     * TODO: Rename to charvecPool?
     */
    jm_vector(jm_voidp) parseBuffer;

    /**
     * Used for writing to attrBuffer. Uses lookup by attribute name instead
     * of attribute ID. The .ptr field points to attrBuffer[id(attr_name)].
     * Currently used ONLY for writing.
     *
     * TODO: Rename to attrMapByName?
     */
    jm_vector(jm_named_ptr)* attrMap;

    /**
     * Vector with a slot for every attribute for every element to allow constant lookup:
     *     attrBuffer[<attr_id>] = <attr_value>
     *
     * Is populated with all parsed attributes for the current element before that element
     * handler is invoked.
     *
     * Typically attributes values are cleared when they are read, such that at the end of
     * parsing an element all attributes should be cleared.
     *
     * NOTE:
     * The pointers point to expat's internal memory. It's not allowed to save references
     * to this memory between element handle calls.
     *
     * TODO: Rename to attrMapById?
     */
    jm_vector(jm_string)* attrBuffer;

    /**
     * A vector that only contains mappings from element names to element
     * handlers. Remember that there can be several mappings for the same
     * element name, but with "alternative names".
     * The mapping for an XML name therefore depends on the context. For
     * example, when the ModelVariable element is found, the mapping for
     * 'Int32' must change the handler for the "alternative name":
     * 'fmi3_xml_handle_Int32Variable'.
     */
    jm_vector(fmi3_xml_element_handle_map_t)* elmMap;

    fmi3_xml_unit_t* lastBaseUnit;

    /**
     * If there's an issue with the variable element, this flag says that its
     * children should be skipped.
     *
     * XXX: Seems to only be set FMI 1.
     */
    int skipOneVariableFlag;

    /**
     * Incremented when an invalid element(or nested elements of invalid root
     * element) is found. Decremented when invalid element end tags are parsed.
     */
    int skipElementCnt;

    /**
     * There is no guarantee that all text will be handled in one call to the
     * function implementing the XML_CharacterDataHandler, and this variable
     * saves if a warning has already been generated.
     */
    int has_produced_data_warning;

    /**
     * Used to get parent element.
     * Top of stack:
     *   XML_StartElementHandler:
     *     on enter: grandparent or empty
     *     on exit: parent or empty (push)
     *   XML_EndElementHandler:
     *     on enter: parent or empty
     *     on exit: grandparent or empty (pop)
     */
    jm_stack(int) elmStack;

    /**
     * Contains the latest element text. For an MD without tool specific
     * annotations, this will always be empty. This variable is currently
     * only used as a bool-switch though...
     * TODO: Refactor to bool
     */
    jm_vector(char) elmData;

    /**
     * Contains a Variable's start attribute. Purpose is to allow it to be
     * processed in the Variable's end-handler.
     */
    jm_vector(char) variableStartAttr;

    /**
     * Element ID of the last processed sibling, or fmi3_xml_elmID_none if
     * no siblings have been processed.
     */
    fmi3_xml_elm_enu_t lastElmID;

    /**
     * Used for error checking and scheme verification.
     * Values:
     *   XML_StartElementHandler:
     *     on enter: parent
     *     on exit: self
     *   XML_EndElementHandler:
     *     on enter: self
     *     on exit: parent
     */
    fmi3_xml_elm_enu_t currentElmID;

    fmi3_xml_elm_enu_t currentElemIdStartTag;

    /* Variables for handling tool-specific XML elements */
    int anyElmCount;
    int useAnyHandleFlg;
    char* anyToolName;
    void* anyParent;
    fmi3_xml_callbacks_t* anyHandle;

    /* Data for restoring locale after parsing */
    jm_locale_t* jm_locale;
};

/* Meta data about primitive types */
/* NOTE: elemID is should not be included because it's different during parsing of TypeDef / Variable */
typedef struct fmi3_xml_primitive_type_t {
    const char*             name;               /* human-readable name */
    size_t                  size;               /* size in bytes */
    fmi3_bitness_enu_t      bitness;            /* type's bitness */
    int                     isSigned;           /* if the type is signed */
    fmi3_base_type_enu_t    baseType;           /* enum value for primitive type (for a typedef object) */
} fmi3_xml_primitive_type_t;

typedef struct fmi3_xml_primitive_types_t {
    fmi3_xml_primitive_type_t float64;
    fmi3_xml_primitive_type_t float32;
    fmi3_xml_primitive_type_t int64;
    fmi3_xml_primitive_type_t int32;
    fmi3_xml_primitive_type_t int16;
    fmi3_xml_primitive_type_t int8;
    fmi3_xml_primitive_type_t uint64;
    fmi3_xml_primitive_type_t uint32;
    fmi3_xml_primitive_type_t uint16;
    fmi3_xml_primitive_type_t uint8;
    fmi3_xml_primitive_type_t boolean;
    fmi3_xml_primitive_type_t enumeration;
} fmi3_xml_primitive_types_t;

extern const fmi3_xml_primitive_types_t PRIMITIVE_TYPES;

jm_vector(char) * fmi3_xml_reserve_parse_buffer(fmi3_xml_parser_context_t *context, size_t index, size_t size);
jm_vector(char) * fmi3_xml_get_parse_buffer(fmi3_xml_parser_context_t *context, size_t index);
int fmi3_xml_alloc_parse_buffer(fmi3_xml_parser_context_t *context, size_t items);
void fmi3_xml_free_variable_start_values(fmi3_xml_parser_context_t *context);
void fmi3_xml_free_parse_buffer(fmi3_xml_parser_context_t *context);

void fmi3_xml_parse_fatal(fmi3_xml_parser_context_t *context, const char* fmt, ...);
void fmi3_xml_parse_error(fmi3_xml_parser_context_t *context, const char* fmt, ...);

/**
 * Raises generic parse error for given attribute.
 */
void fmi3_xml_parse_attr_error(fmi3_xml_parser_context_t *context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID, const char* attrStr);

/* XXX: Let's rename all these to fmi3_xml_parse_attr_as_<type> */
int fmi3_xml_set_attr_string (fmi3_xml_parser_context_t* context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID, int required, jm_vector(char)* field);
int fmi3_xml_set_attr_enum   (fmi3_xml_parser_context_t* context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID, int required, unsigned int*    field, unsigned int   defaultVal, jm_name_ID_map_t* nameMap);
int fmi3_xml_set_attr_bool   (fmi3_xml_parser_context_t* context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID, int required, bool*            field, bool           defaultVal);
int fmi3_xml_set_attr_boolean(fmi3_xml_parser_context_t* context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID, int required, unsigned int*    field, unsigned int   defaultVal);
int fmi3_xml_set_attr_intXX  (fmi3_xml_parser_context_t* context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID, int required, void*            field, void*          defaultVal, const fmi3_xml_primitive_type_t* primType);
int fmi3_xml_set_attr_int32  (fmi3_xml_parser_context_t* context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID, int required, fmi3_int32_t*    field, fmi3_int32_t   defaultVal);
int fmi3_xml_set_attr_uint64 (fmi3_xml_parser_context_t* context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID, int required, fmi3_uint64_t*   field, fmi3_uint64_t  defaultVal);
int fmi3_xml_set_attr_uint32 (fmi3_xml_parser_context_t* context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID, int required, fmi3_uint32_t*   field, fmi3_uint32_t  defaultVal);
int fmi3_xml_set_attr_floatXX(fmi3_xml_parser_context_t* context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID, int required, void*            field, void*          defaultVal, const fmi3_xml_primitive_type_t* primType);
int fmi3_xml_set_attr_float64(fmi3_xml_parser_context_t* context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID, int required, fmi3_float64_t*  field, fmi3_float64_t defaultVal);
int fmi3_xml_set_attr_float32(fmi3_xml_parser_context_t* context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID, int required, fmi3_float32_t*  field, fmi3_float32_t defaultVal);
int fmi3_xml_set_attr_sizet  (fmi3_xml_parser_context_t* context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID, int required, size_t*          field, size_t*        defaultVal);

int fmi3_xml_parse_attr_valueref_list(fmi3_xml_parser_context_t* context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID, int required, jm_vector(fmi3_value_reference_t)* vrs);

int fmi3_xml_is_attr_defined( fmi3_xml_parser_context_t *context, fmi3_xml_attr_enu_t attrID);
jm_string fmi3_xml_peek_attr_str(fmi3_xml_parser_context_t* context, fmi3_xml_attr_enu_t attrID);
int fmi3_xml_get_attr_str(fmi3_xml_parser_context_t *context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID, int required,const char** valp);

int fmi3_xml_set_attr_array(fmi3_xml_parser_context_t* context, fmi3_xml_elm_enu_t elmID, fmi3_xml_attr_enu_t attrID, int required, void** arrPtr, size_t* arrSize, jm_string str, const fmi3_xml_primitive_type_t* primType);

void fmi3_xml_set_element_handle(fmi3_xml_parser_context_t *context, const char* elm, fmi3_xml_elm_enu_t id);

int fmi3_xml_is_valid_parent(fmi3_xml_elm_enu_t child_id, fmi3_xml_elm_enu_t parent_id);
int fmi3_xml_get_super_type_rec(fmi3_xml_elm_enu_t id);
int fmi3_xml_are_same_type(fmi3_xml_elm_enu_t id1, fmi3_xml_elm_enu_t id2);

const char* fmi3_xml_elmid_to_name(fmi3_xml_elm_enu_t id);

#ifdef __cplusplus
}
#endif

#endif /* FMI3_XML_XMLPARSER_H */
