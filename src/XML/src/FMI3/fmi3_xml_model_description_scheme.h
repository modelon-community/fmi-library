/*
    Copyright (C) 2023 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#ifndef FMI3_XML_MODEL_DESCRIPTION_SCHEME_H
#define FMI3_XML_MODEL_DESCRIPTION_SCHEME_H

#include <FMI3/fmi3_enums.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Attribute names found in modelDescription.xml */
#define FMI3_XML_ATTRLIST_MODEL_DESCR(EXPAND_XML_ATTRNAME) \
    EXPAND_XML_ATTRNAME(fmiVersion) \
    EXPAND_XML_ATTRNAME(name) \
    EXPAND_XML_ATTRNAME(description) \
    EXPAND_XML_ATTRNAME(factor) \
    EXPAND_XML_ATTRNAME(offset) \
    EXPAND_XML_ATTRNAME(inverse) \
    FMI3_SI_BASE_UNITS(EXPAND_XML_ATTRNAME) \
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
    EXPAND_XML_ATTRNAME(intermediateUpdate) \
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

/* Element names found in modelDescription.xml */
#define FMI3_XML_ELMLIST_MODEL_DESCR(EXPAND_XML_ELMNAME) \
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
    EXPAND_XML_ELMNAME(Float64Type) \
    EXPAND_XML_ELMNAME(Float32Type) \
    EXPAND_XML_ELMNAME(Int64Type) \
    EXPAND_XML_ELMNAME(Int32Type) \
    EXPAND_XML_ELMNAME(Int16Type) \
    EXPAND_XML_ELMNAME(Int8Type) \
    EXPAND_XML_ELMNAME(UInt64Type) \
    EXPAND_XML_ELMNAME(UInt32Type) \
    EXPAND_XML_ELMNAME(UInt16Type) \
    EXPAND_XML_ELMNAME(UInt8Type) \
    EXPAND_XML_ELMNAME(BooleanType) \
    EXPAND_XML_ELMNAME(BinaryType) \
    EXPAND_XML_ELMNAME(ClockType) \
    EXPAND_XML_ELMNAME(StringType) \
    EXPAND_XML_ELMNAME(EnumerationType) \
    EXPAND_XML_ELMNAME(ModelStructure) \
    EXPAND_XML_ELMNAME(Output) \
    EXPAND_XML_ELMNAME(ContinuousStateDerivative) \
    EXPAND_XML_ELMNAME(ClockedState) \
    EXPAND_XML_ELMNAME(InitialUnknown) \
    EXPAND_XML_ELMNAME(EventIndicator) \
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
    EXPAND_XML_ELMNAME(Enumeration)

/** \brief Element that can be placed under different parents get alternative names from the info struct */
#define FMI3_XML_ELMLIST_ALT_MODEL_DESCR(EXPAND_XML_ELMNAME) \
    EXPAND_XML_ELMNAME(BinaryVariableStart) \
    EXPAND_XML_ELMNAME(StringVariableStart) \
    EXPAND_XML_ELMNAME(VariableTool) \
    EXPAND_XML_ELMNAME(SourceFilesCS) \
    EXPAND_XML_ELMNAME(FileCS)

/** \brief Abstract elements that are only used in the scheme verification */
#define FMI3_XML_ELMLIST_ABSTRACT_MODEL_DESCR(EXPAND_XML_ELMNAME) \
    EXPAND_XML_ELMNAME(Variable)

// XXX: fmi3_xml_modelDescription_elmID_none & fmi3_xml_elmID_Start are defined in fmi3_xml_parser.h, got good style
/*
    Define XML schema structure. Used to build the 'fmi3_xml_modelDescription_scheme_info_t' type (in fmi3_xml_parser.c).

    @sib_idx:
        the index in a sequence among siblings
    @multi_elem:
        if the parent can have multiple elements of this type
*/
/*      scheme_ID,                                super_type,                 parent_ID,                          sib_idx, multi_elem */
#define fmi3_xml_scheme_fmiModelDescription       {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_modelDescription_elmID_none,                 0,       0}
#define fmi3_xml_scheme_ModelExchange             {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_fmiModelDescription,  0,       0}
#define fmi3_xml_scheme_SourceFiles               {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_ModelExchange,        0,       0}
#define fmi3_xml_scheme_File                      {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_SourceFiles,          0,       1}
#define fmi3_xml_scheme_CoSimulation              {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_fmiModelDescription,  1,       0}
#define fmi3_xml_scheme_ScheduledExecution        {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_fmiModelDescription,  2,       0}
#define fmi3_xml_scheme_SourceFilesCS             {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_CoSimulation,         0,       0}
#define fmi3_xml_scheme_FileCS                    {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_SourceFilesCS,        0,       1}
#define fmi3_xml_scheme_UnitDefinitions           {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_fmiModelDescription,  2,       0}
#define fmi3_xml_scheme_Unit                      {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_UnitDefinitions,      0,       1}
#define fmi3_xml_scheme_BaseUnit                  {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_Unit,                 0,       0}
#define fmi3_xml_scheme_DisplayUnit               {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_Unit,                 1,       1}
#define fmi3_xml_scheme_TypeDefinitions           {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_fmiModelDescription,  3,       0}
#define fmi3_xml_scheme_SimpleType                {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_TypeDefinitions,      0,       1}
#define fmi3_xml_scheme_Float64Type               {fmi3_xml_elmID_SimpleType, fmi3_xml_elmID_TypeDefinitions,      0,       1}
#define fmi3_xml_scheme_Float32Type               {fmi3_xml_elmID_SimpleType, fmi3_xml_elmID_TypeDefinitions,      0,       1}
#define fmi3_xml_scheme_Int64Type                 {fmi3_xml_elmID_SimpleType, fmi3_xml_elmID_TypeDefinitions,      0,       1}
#define fmi3_xml_scheme_Int32Type                 {fmi3_xml_elmID_SimpleType, fmi3_xml_elmID_TypeDefinitions,      0,       1}
#define fmi3_xml_scheme_Int16Type                 {fmi3_xml_elmID_SimpleType, fmi3_xml_elmID_TypeDefinitions,      0,       1}
#define fmi3_xml_scheme_Int8Type                  {fmi3_xml_elmID_SimpleType, fmi3_xml_elmID_TypeDefinitions,      0,       1}
#define fmi3_xml_scheme_UInt64Type                {fmi3_xml_elmID_SimpleType, fmi3_xml_elmID_TypeDefinitions,      0,       1}
#define fmi3_xml_scheme_UInt32Type                {fmi3_xml_elmID_SimpleType, fmi3_xml_elmID_TypeDefinitions,      0,       1}
#define fmi3_xml_scheme_UInt16Type                {fmi3_xml_elmID_SimpleType, fmi3_xml_elmID_TypeDefinitions,      0,       1}
#define fmi3_xml_scheme_UInt8Type                 {fmi3_xml_elmID_SimpleType, fmi3_xml_elmID_TypeDefinitions,      0,       1}
#define fmi3_xml_scheme_BooleanType               {fmi3_xml_elmID_SimpleType, fmi3_xml_elmID_TypeDefinitions,      0,       1}
#define fmi3_xml_scheme_BinaryType                {fmi3_xml_elmID_SimpleType, fmi3_xml_elmID_TypeDefinitions,      0,       1}
#define fmi3_xml_scheme_ClockType                 {fmi3_xml_elmID_SimpleType, fmi3_xml_elmID_TypeDefinitions,      0,       1}
#define fmi3_xml_scheme_StringType                {fmi3_xml_elmID_SimpleType, fmi3_xml_elmID_TypeDefinitions,      0,       1}
#define fmi3_xml_scheme_EnumerationType           {fmi3_xml_elmID_SimpleType, fmi3_xml_elmID_TypeDefinitions,      0,       1}
#define fmi3_xml_scheme_Item                      {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_EnumerationType,      0,       1}
#define fmi3_xml_scheme_LogCategories             {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_fmiModelDescription,  4,       0}
#define fmi3_xml_scheme_Category                  {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_LogCategories,        0,       1}
#define fmi3_xml_scheme_DefaultExperiment         {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_fmiModelDescription,  5,       0}
#define fmi3_xml_scheme_VendorAnnotations         {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_fmiModelDescription,  6,       0}
#define fmi3_xml_scheme_Tool                      {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_VendorAnnotations,    0,       1}
#define fmi3_xml_scheme_ModelVariables            {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_fmiModelDescription,  7,       0}

#define fmi3_xml_scheme_ModelStructure            {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_fmiModelDescription,  8,       0}
#define fmi3_xml_scheme_Output                    {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_ModelStructure,       0,       1}
#define fmi3_xml_scheme_ContinuousStateDerivative {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_ModelStructure,       1,       1}
#define fmi3_xml_scheme_ClockedState              {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_ModelStructure,       2,       1}
#define fmi3_xml_scheme_InitialUnknown            {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_ModelStructure,       3,       1}
#define fmi3_xml_scheme_EventIndicator            {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_ModelStructure,       4,       1}

#define fmi3_xml_scheme_Variable                  {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_ModelVariables,       0,       1}
#define fmi3_xml_scheme_Float64                   {fmi3_xml_elmID_Variable,   fmi3_xml_elmID_ModelVariables,       0,       1}
#define fmi3_xml_scheme_Float32                   {fmi3_xml_elmID_Variable,   fmi3_xml_elmID_ModelVariables,       0,       1}
#define fmi3_xml_scheme_Int64                     {fmi3_xml_elmID_Variable,   fmi3_xml_elmID_ModelVariables,       0,       1}
#define fmi3_xml_scheme_Int32                     {fmi3_xml_elmID_Variable,   fmi3_xml_elmID_ModelVariables,       0,       1}
#define fmi3_xml_scheme_Int16                     {fmi3_xml_elmID_Variable,   fmi3_xml_elmID_ModelVariables,       0,       1}
#define fmi3_xml_scheme_Int8                      {fmi3_xml_elmID_Variable,   fmi3_xml_elmID_ModelVariables,       0,       1}
#define fmi3_xml_scheme_UInt64                    {fmi3_xml_elmID_Variable,   fmi3_xml_elmID_ModelVariables,       0,       1}
#define fmi3_xml_scheme_UInt32                    {fmi3_xml_elmID_Variable,   fmi3_xml_elmID_ModelVariables,       0,       1}
#define fmi3_xml_scheme_UInt16                    {fmi3_xml_elmID_Variable,   fmi3_xml_elmID_ModelVariables,       0,       1}
#define fmi3_xml_scheme_UInt8                     {fmi3_xml_elmID_Variable,   fmi3_xml_elmID_ModelVariables,       0,       1}
#define fmi3_xml_scheme_Boolean                   {fmi3_xml_elmID_Variable,   fmi3_xml_elmID_ModelVariables,       0,       1}
#define fmi3_xml_scheme_Binary                    {fmi3_xml_elmID_Variable,   fmi3_xml_elmID_ModelVariables,       0,       1}
#define fmi3_xml_scheme_Clock                     {fmi3_xml_elmID_Variable,   fmi3_xml_elmID_ModelVariables,       0,       1}
#define fmi3_xml_scheme_String                    {fmi3_xml_elmID_Variable,   fmi3_xml_elmID_ModelVariables,       0,       1}
#define fmi3_xml_scheme_Enumeration               {fmi3_xml_elmID_Variable,   fmi3_xml_elmID_ModelVariables,       0,       1}

#define fmi3_xml_scheme_Dimension                 {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_Variable,             0,       1}
#define fmi3_xml_scheme_BinaryVariableStart       {fmi3_xml_elmID_Start,      fmi3_xml_elmID_Binary,               1,       1}
#define fmi3_xml_scheme_StringVariableStart       {fmi3_xml_elmID_Start,      fmi3_xml_elmID_String,               1,       1}
#define fmi3_xml_scheme_Alias                     {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_Variable,             2,       1}

#define fmi3_xml_scheme_Annotations               {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_Variable,             1,       0}
#define fmi3_xml_scheme_VariableTool              {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_elmID_Annotations,          0,       1}

// Not used except for setting up the element handler framework:
#define fmi3_xml_scheme_Start                     {fmi3_xml_modelDescription_elmID_none,       fmi3_xml_modelDescription_elmID_none,                 1,       0}

// TODO: List formatting + use terminals and Icons type
// TODO: rename to include modelDescription in name?

#ifdef __cplusplus
}
#endif

#endif /* FMI3_XML_MODEL_DESCRIPTION_SCHEME_H */
