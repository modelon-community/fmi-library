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

#ifndef FMI3_ENUMS_H_
#define FMI3_ENUMS_H_

#include <fmilib_config.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \file fmi3_enums.h
    \brief Definions the enum types used with FMI 3.0 libs
*/

/**    \addtogroup fmi3_utils
    @{
    \addtogroup fmi3_enums
    @}
*/
/** \addtogroup fmi3_enums Enum types used with FMI 3.0 libs
    @{
*/
/** \brief Naming convention for the variables in XML file*/
typedef enum fmi3_variable_naming_convension_enu_t
{
        fmi3_naming_enu_flat,
        fmi3_naming_enu_structured,
        fmi3_naming_enu_unknown
} fmi3_variable_naming_convension_enu_t;

/** \brief Convert a #fmi3_variable_naming_convension_enu_t constant into string */
FMILIB_EXPORT const char* fmi3_naming_convention_to_string(fmi3_variable_naming_convension_enu_t convention);

/**  \brief FMU 3.0 kinds */
typedef enum fmi3_fmu_kind_enu_t
{
        fmi3_fmu_kind_unknown = 0,
        fmi3_fmu_kind_me = 1,
        fmi3_fmu_kind_cs = 2,
        fmi3_fmu_kind_me_and_cs = 3 /* defined to be equal to (fmi3_fmu_kind_me|fmi3_fmu_kind_cs) */
} fmi3_fmu_kind_enu_t;

/** \brief Convert a #fmi3_fmu_kind_enu_t constant into string  */
FMILIB_EXPORT const char* fmi3_fmu_kind_to_string(fmi3_fmu_kind_enu_t kind);

/**  \brief Variability property for variables */
typedef enum fmi3_variability_enu_t {
        fmi3_variability_enu_constant = 0,
        fmi3_variability_enu_fixed  = 1,
        fmi3_variability_enu_tunable  = 2,
        fmi3_variability_enu_discrete  = 3,
        fmi3_variability_enu_continuous  = 4,
        fmi3_variability_enu_unknown  = 5
} fmi3_variability_enu_t;

/** \brief Convert a #fmi3_variability_enu_t constant into string  */
FMILIB_EXPORT const char* fmi3_variability_to_string(fmi3_variability_enu_t v);

/**  \brief Causality property for variables */
typedef enum fmi3_causality_enu_t {
        fmi3_causality_enu_parameter = 0,
        fmi3_causality_enu_calculated_parameter = 1,
        fmi3_causality_enu_input  = 2,
        fmi3_causality_enu_output  = 3,
        fmi3_causality_enu_local  = 4,
        fmi3_causality_enu_independent = 5,
        fmi3_causality_enu_unknown = 6
} fmi3_causality_enu_t;

/** \brief Convert a #fmi3_causality_enu_t constant into string  */
FMILIB_EXPORT const char* fmi3_causality_to_string(fmi3_causality_enu_t c);

/**
 * \brief Get the default variability for a given causality. NOTE: the FMI 3.0
 *        defines the default to always be continuous, but this is incompatible
 *        with causalities "parameter" and "calculatedParameter". These will
 *        instead be treated as having variability "fixed".
 *
 * \return A default variability compatible with the given causality.
 *
 */
FMILIB_EXPORT fmi3_variability_enu_t fmi3_get_default_valid_variability(fmi3_causality_enu_t c);

/**
 * \brief Check if a given combination of variablity and causality is valid.
 *
 * \return 0 if not valid, non-zero otherwise
 */
FMILIB_EXPORT int fmi3_is_valid_variability_causality(fmi3_variability_enu_t v,
                                                      fmi3_causality_enu_t c);

/**  \brief Initial property for variables */
typedef enum fmi3_initial_enu_t {
        fmi3_initial_enu_exact,
        fmi3_initial_enu_approx,
        fmi3_initial_enu_calculated,
        fmi3_initial_enu_unknown /* must be last*/
} fmi3_initial_enu_t;

/** \brief Convert a #fmi3_initial_enu_t constant into string  */
FMILIB_EXPORT const char* fmi3_initial_to_string(fmi3_initial_enu_t c);

/**
    \brief Get default initial attribute value for the given variability and causality combination.
    \return The default initial attribute or fmi3_initial_enu_unknown if combination of causality
            and variability is not valid.
*/
FMILIB_EXPORT fmi3_initial_enu_t fmi3_get_default_initial(fmi3_variability_enu_t v, fmi3_causality_enu_t c);

/**
    \brief Check if the combination of variability, causality and initial is valid.
    \return Same initial as submitted if the combination is valid. Otherwise, same as fmi3_get_default_initial.
*/
FMILIB_EXPORT fmi3_initial_enu_t fmi3_get_valid_initial(fmi3_variability_enu_t v, fmi3_causality_enu_t c, fmi3_initial_enu_t i);

/** \brief Alias property for variables */
typedef enum fmi3_variable_alias_kind_enu_t {
    fmi3_variable_is_not_alias = 0,
    fmi3_variable_is_alias = 1
} fmi3_variable_alias_kind_enu_t;

/** \brief Base types used in type definitions */
typedef enum fmi3_base_type_enu_t
{
    fmi3_base_type_float64,
    fmi3_base_type_float32,
    fmi3_base_type_real,
    fmi3_base_type_int,
    fmi3_base_type_bool,
    fmi3_base_type_str,
    fmi3_base_type_enum
} fmi3_base_type_enu_t;

/**  \brief Convert base type constant to string
    \param bt Base type identifier.
    \return Corresponding base type name.
    */
FMILIB_EXPORT const char* fmi3_base_type_to_string(fmi3_base_type_enu_t bt);

/** \brief List of capability flags for ModelExchange */
#define FMI3_ME_CAPABILITIES(H) \
    H(needsExecutionTool) \
    H(completedIntegratorStepNotNeeded) \
    H(canBeInstantiatedOnlyOncePerProcess) \
    H(canNotUseMemoryManagementFunctions) \
    H(canGetAndSetFMUstate) \
    H(canSerializeFMUstate) \
    H(providesDirectionalDerivatives)\
    H(completedEventIterationIsProvided)

/** \brief List of capability flags for CoSimulation */
#define FMI3_CS_CAPABILITIES(H) \
    H(needsExecutionTool) \
    H(canHandleVariableCommunicationStepSize) \
    H(canInterpolateInputs) \
    H(maxOutputDerivativeOrder) \
    H(canRunAsynchronuously) \
    H(canBeInstantiatedOnlyOncePerProcess) \
    H(canNotUseMemoryManagementFunctions) \
    H(canGetAndSetFMUstate) \
    H(canSerializeFMUstate) \
    H(providesDirectionalDerivatives)

/** \brief Capability flags for ModelExchange and CoSimulation */
typedef enum fmi3_capabilities_enu_t {
#define FMI3_EXPAND_ME_CAPABILITIES_ENU(c) fmi3_me_ ## c,
#define FMI3_EXPAND_CS_CAPABILITIES_ENU(c) fmi3_cs_ ## c,
    FMI3_ME_CAPABILITIES(FMI3_EXPAND_ME_CAPABILITIES_ENU)
    FMI3_CS_CAPABILITIES(FMI3_EXPAND_CS_CAPABILITIES_ENU)
    fmi3_capabilities_Num
} fmi3_capabilities_enu_t;

/** \brief Convert capability flag to a string
    \param id Capability flag ID.
    \return Name of the flag or Unknown if the id is out of range.
*/
FMILIB_EXPORT const char * fmi3_capability_to_string(fmi3_capabilities_enu_t id);

/** \brief List of SI base units used in Unit defitions*/
#define FMI3_SI_BASE_UNITS(H) \
    H(kg) H(m) H(s) H(A) H(K) H(mol) H(cd) H(rad)

/** \brief SI base units used in Unit defitions*/
typedef enum fmi3_SI_base_units_enu_t {
#define FMI3_EXPAND_SI_BASE_UNIT_ENU(c) fmi3_SI_base_unit_ ## c,
    FMI3_SI_BASE_UNITS(FMI3_EXPAND_SI_BASE_UNIT_ENU)
    fmi3_SI_base_units_Num
} fmi3_SI_base_units_enu_t;

/** \brief Convert SI base unit ID a string
    \param id SI base unit ID.
    \return Name of the base unit or "unknown" if the id is out of range.
*/
FMILIB_EXPORT const char * fmi3_SI_base_unit_to_string(fmi3_SI_base_units_enu_t id);

/** \brief Convert a list of SI base unit exponents (corresponding to the IDs from  fmi3_SI_base_units_enu_t)
    to a string of the form kg*m^2/s^2. Prints '-' if all the exponents are zero.
    \param exp An array of SI base units exponents.
    \param bufSize Size of the buffer to store the string.
    \param buf Buffer to store the string
    \return Required size of the buffer to store the string. This most likely be under [8*fmi3_SI_base_units_Num].
    If the return value is larger or equal than bufSize than the string could not be fitted in the buffer.
*/
FMILIB_EXPORT size_t fmi3_SI_base_unit_exp_to_string(const int exp[fmi3_SI_base_units_Num], size_t bufSize, char buf[]);


/** \brief Dependency factor kinds are used as part of ModelStructure definition */
typedef enum fmi3_dependency_factor_kind_enu_t
{
    fmi3_dependency_factor_kind_dependent = 0,
    fmi3_dependency_factor_kind_constant,
    fmi3_dependency_factor_kind_fixed,
    fmi3_dependency_factor_kind_tunable,
    fmi3_dependency_factor_kind_discrete,
    fmi3_dependency_factor_kind_num
} fmi3_dependency_factor_kind_enu_t;


/** \brief Bitness for types such as Float32/Float64 */
typedef enum fmi3_bitness_enu_t {
    fmi3_bitness_64,
    fmi3_bitness_32
} fmi3_bitness_enu_t;

/**  \brief Convert dependency factor kind constant to string
    \param fc Dependency factor kind identifier.
    \return Corresponding factor kind as string.
    */
FMILIB_EXPORT const char* fmi3_dependency_factor_kind_to_string(fmi3_dependency_factor_kind_enu_t fc);
/**
 @}
*/
#ifdef __cplusplus
}
#endif

#endif /* End of header file FMI3_ENUMS_H_ */
