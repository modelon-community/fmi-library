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
#include <assert.h>
#include <stdio.h>

#include <FMI3/fmi3_function_types.h>
#include <FMI3/fmi3_enums.h>


bool fmi3_base_type_enu_is_bool(fmi3_base_type_enu_t baseType) {
    return baseType == fmi3_base_type_bool;
}

bool fmi3_base_type_enu_is_int(fmi3_base_type_enu_t baseType) {
    switch (baseType) {
        case fmi3_base_type_int64:    return true;
        case fmi3_base_type_int32:    return true;
        case fmi3_base_type_int16:    return true;
        case fmi3_base_type_int8:     return true;
        case fmi3_base_type_uint64:   return true;
        case fmi3_base_type_uint32:   return true;
        case fmi3_base_type_uint16:   return true;
        case fmi3_base_type_uint8:    return true;
        default: return false;
    }
    return false;
}
bool fmi3_base_type_enu_is_float(fmi3_base_type_enu_t baseType) {
    return baseType==fmi3_base_type_float64 || baseType==fmi3_base_type_float32;
}

bool fmi3_base_type_enu_is_enum(fmi3_base_type_enu_t baseType) {
    return baseType == fmi3_base_type_enum;
}

const char* fmi3_naming_convention_to_string(fmi3_variable_naming_convension_enu_t convention) {
    if(convention == fmi3_naming_enu_flat)       return "flat";
    if(convention == fmi3_naming_enu_structured) return "structured";
    return "Unknown";
}

const char* fmi3_fmu_kind_to_string(fmi3_fmu_kind_enu_t kind) {
    switch (kind) {
    case fmi3_fmu_kind_unknown: return "Unknown";
    case fmi3_fmu_kind_me:      return "ModelExchange";
    case fmi3_fmu_kind_cs:      return "CoSimulation";
    case fmi3_fmu_kind_se:      return "Scheduled Execution";
    default: break;
    }
    return "Multi-Type";
}

const char* fmi3_dependencies_kind_to_string(fmi3_dependencies_kind_enu_t fc) {
    switch(fc) {
    case fmi3_dependencies_kind_dependent: return "dependent";
    case fmi3_dependencies_kind_constant:  return "constant";
    case fmi3_dependencies_kind_fixed:     return "fixed";
    case fmi3_dependencies_kind_tunable:   return "tunable";
    case fmi3_dependencies_kind_discrete:  return "discrete";
    default:break;
    }
    return "illegal dependencies kind";
}

const char* fmi3_variability_to_string(fmi3_variability_enu_t v) {
    switch(v) {
    case fmi3_variability_enu_constant:   return "constant";
    case fmi3_variability_enu_fixed:      return "fixed";
    case fmi3_variability_enu_tunable:    return "tunable";
    case fmi3_variability_enu_discrete:   return "discrete";
    case fmi3_variability_enu_continuous: return "continuous";
    default: break;
    }
    return "Unknown";
}

const char* fmi3_causality_to_string(fmi3_causality_enu_t c) {
    switch(c) {
    case fmi3_causality_enu_parameter:            return "parameter";
    case fmi3_causality_enu_calculated_parameter: return "calculatedParameter";
    case fmi3_causality_enu_input:                return "input";
    case fmi3_causality_enu_output:               return "output";
    case fmi3_causality_enu_local:                return "local";
    case fmi3_causality_enu_independent:          return "independent";
    case fmi3_causality_enu_structural_parameter: return "structuralParameter";
    default: break;
    };
    return "Unknown";
}

const char* fmi3_status_to_string(fmi3_status_t status) {
    switch(status) {
    case fmi3_status_ok:
        return "OK";
    case fmi3_status_warning:
        return "Warning";
    case fmi3_status_discard:
        return "Discard";
    case fmi3_status_error:
        return "Error";
    case fmi3_status_fatal:
        return "Fatal";
    default: break;
    }
    return "Undefined";
}

const char* fmi3_base_type_to_string(fmi3_base_type_enu_t bt) {
    switch(bt) {
    case fmi3_base_type_float64: return "Float64";
    case fmi3_base_type_float32: return "Float32";
    case fmi3_base_type_int64:   return "Int64";
    case fmi3_base_type_int32:   return "Int32";
    case fmi3_base_type_int16:   return "Int16";
    case fmi3_base_type_int8:    return "Int8";
    case fmi3_base_type_uint64:  return "UInt64";
    case fmi3_base_type_uint32:  return "UInt32";
    case fmi3_base_type_uint16:  return "UInt16";
    case fmi3_base_type_uint8:   return "UInt8";
    case fmi3_base_type_bool:    return "Boolean";
    case fmi3_base_type_str:     return "String";
    case fmi3_base_type_enum:    return "Enumeration";
    default: break;
    }
    return "Error";
}

const char* fmi3_initial_to_string(fmi3_initial_enu_t ini) {
    switch (ini) {
    case fmi3_initial_enu_exact:      return "exact";
    case fmi3_initial_enu_approx:     return "approx";
    case fmi3_initial_enu_calculated: return "calculated";
        default: break;
    }
    return "Undefined";
}

fmi3_initial_enu_t fmi3InitialDefaultsTable[fmi3_variability_enu_unknown][fmi3_causality_enu_unknown] = {
    /*              parameter                  calculated parameter,        input                     output                       local                        independent               structural parameter*/
    /* constant */  {fmi3_initial_enu_unknown, fmi3_initial_enu_unknown,    fmi3_initial_enu_unknown, fmi3_initial_enu_exact,      fmi3_initial_enu_exact,      fmi3_initial_enu_unknown, fmi3_initial_enu_unknown},
    /* fixed   */   {fmi3_initial_enu_exact,   fmi3_initial_enu_calculated, fmi3_initial_enu_unknown, fmi3_initial_enu_unknown,    fmi3_initial_enu_calculated, fmi3_initial_enu_unknown, fmi3_initial_enu_exact  },
    /* tunable */   {fmi3_initial_enu_exact,   fmi3_initial_enu_calculated, fmi3_initial_enu_unknown, fmi3_initial_enu_unknown,    fmi3_initial_enu_calculated, fmi3_initial_enu_unknown, fmi3_initial_enu_exact  },
    /* discrete */  {fmi3_initial_enu_unknown, fmi3_initial_enu_unknown,    fmi3_initial_enu_exact  , fmi3_initial_enu_calculated, fmi3_initial_enu_calculated, fmi3_initial_enu_unknown, fmi3_initial_enu_unknown},
    /* continuous */{fmi3_initial_enu_unknown, fmi3_initial_enu_unknown,    fmi3_initial_enu_exact  , fmi3_initial_enu_calculated, fmi3_initial_enu_calculated, fmi3_initial_enu_unknown, fmi3_initial_enu_unknown}
};

FMILIB_EXPORT fmi3_variability_enu_t fmi3_get_default_valid_variability(fmi3_causality_enu_t c)
{
    switch(c){
    case fmi3_causality_enu_parameter:            return fmi3_variability_enu_fixed;
    case fmi3_causality_enu_calculated_parameter: return fmi3_variability_enu_fixed;
    case fmi3_causality_enu_input:                return fmi3_variability_enu_continuous;
    case fmi3_causality_enu_output:               return fmi3_variability_enu_continuous;
    case fmi3_causality_enu_local:                return fmi3_variability_enu_continuous;
    case fmi3_causality_enu_independent:          return fmi3_variability_enu_continuous;
    case fmi3_causality_enu_structural_parameter: return fmi3_variability_enu_fixed;
    default:                                      return fmi3_variability_enu_unknown;
    }
}

FMILIB_EXPORT fmi3_initial_enu_t fmi3_get_default_initial(fmi3_variability_enu_t v, fmi3_causality_enu_t c) {
    if((unsigned)v >= (unsigned)fmi3_variability_enu_unknown) return fmi3_initial_enu_unknown;
    if((unsigned)c >= (unsigned)fmi3_causality_enu_unknown)   return fmi3_initial_enu_unknown;
    return fmi3InitialDefaultsTable[v][c];
}

static int valid_variability_causality[fmi3_variability_enu_unknown][fmi3_causality_enu_unknown] = {
    {0, 0, 0, 1, 1, 0, 0},
    {1, 1, 0, 0, 1, 0, 1},
    {1, 1, 0, 0, 1, 0, 1},
    {0, 0, 1, 1, 1, 0, 0},
    {0, 0, 1, 1, 1, 1, 0}
};

FMILIB_EXPORT int fmi3_is_valid_variability_causality(fmi3_variability_enu_t v, fmi3_causality_enu_t c)
{
    if (v < fmi3_variability_enu_constant ||
        v >= fmi3_variability_enu_unknown ||
        c < fmi3_causality_enu_parameter ||
        c >= fmi3_causality_enu_unknown) {

        return 0;
    }

    return valid_variability_causality[v][c];
}

FMILIB_EXPORT fmi3_initial_enu_t fmi3_get_valid_initial(fmi3_variability_enu_t v, fmi3_causality_enu_t c, fmi3_initial_enu_t i) {
    fmi3_initial_enu_t defaultInitial = fmi3_get_default_initial(v, c);
    if( (defaultInitial == i) ||
        ((unsigned)i >= (unsigned)fmi3_initial_enu_unknown) ||
        (defaultInitial == fmi3_initial_enu_unknown))
        return defaultInitial;
    // At this point we know that v, c & i are fine (non unknown) and that (defaultInitial != i)
    // Check the other allowed combinations, reference: Table 22 in FMI 3.0 spec
    switch (c) {
    case fmi3_causality_enu_calculated_parameter:
        if ((i == fmi3_initial_enu_approx) && ((v == fmi3_variability_enu_fixed) || (v == fmi3_variability_enu_tunable)) ) {
            return i;
        } else {
            break;
        }
    case fmi3_causality_enu_output:
        if ((v == fmi3_variability_enu_discrete) || (v == fmi3_variability_enu_continuous)) {
            return i;
        } else {
            break;
        }
    case fmi3_causality_enu_local:
        if ((i == fmi3_initial_enu_approx) && (v > fmi3_variability_enu_constant)) {
            return i;
        }
        if ((i == fmi3_initial_enu_exact) && ((v == fmi3_variability_enu_discrete) || (v == fmi3_variability_enu_continuous))) {
            return i;
        } else {
            break;
        }
    default: break;
    }
    /* in all other cases the combination is not valid and default should be used */
    return defaultInitial;
}

#define FMI3_ME_CAPABILITIES_ENU_TO_STR(c) case fmi3_me_ ## c: return "me_"#c;
#define FMI3_CS_CAPABILITIES_ENU_TO_STR(c) case fmi3_cs_ ## c: return "cs_"#c;
#define FMI3_SE_CAPABILITIES_ENU_TO_STR(c) case fmi3_se_ ## c: return "se_"#c;

const char* fmi3_capability_to_string(fmi3_capabilities_enu_t id) {
    switch (id) {
    FMI3_ME_CAPABILITIES(FMI3_ME_CAPABILITIES_ENU_TO_STR)
    FMI3_CS_CAPABILITIES(FMI3_CS_CAPABILITIES_ENU_TO_STR)
    FMI3_SE_CAPABILITIES(FMI3_SE_CAPABILITIES_ENU_TO_STR)
    default: break;
    }
    return "Unknown";
}

const char * fmi3_SI_base_unit_to_string(fmi3_SI_base_units_enu_t id) {
#define FMI3_SI_BASE_UNIT_ENU_TO_STR(c) case fmi3_SI_base_unit_ ## c: return #c;
    switch(id) {
        FMI3_SI_BASE_UNITS(FMI3_SI_BASE_UNIT_ENU_TO_STR)
    default: break;
    }
    return "unknown";
}

/** \brief Convert a list of SI base unit exponents (corresponding to the IDs from  fmi3_SI_base_units_enu_t)
    to a string of the form kg*m^2/s^2.

    \param exp An array of SI base units exponents.
    \param bufSize Size of the buffer to store the string.
    \param buf Buffer to store the string
    \return Required size of the buffer to store the string including the terminating zero.
        This most likely be under [8*fmi3_SI_base_units_Num]. If the return value is larger or equal
        than bufSize than the string could not be fitted in the buffer.
*/
size_t fmi3_SI_base_unit_exp_to_string(const int exp[fmi3_SI_base_units_Num], size_t bufSize, char buf[]){
    int i = 0;
    int num_pos_exp = 0, num_neg_exp = 0; /* number of exponents */
    size_t len = 0;
    char tmp[fmi3_SI_base_units_Num * 20]; /* tmp is always enough size */
    if(bufSize) buf[0] = 0; /* just to prevent errors */

    /* count the exponents */
    for(i = 0; i < fmi3_SI_base_units_Num; i++) {
        if(exp[i] == 0) continue;
        if(exp[i] < 0)
            num_neg_exp ++;
        else
            num_pos_exp ++;
    }
    if((num_neg_exp + num_pos_exp) == 0) { /* return "-" */
         if (bufSize < 2) return 2;
         buf[0] = '-';
         buf[1] = 0;
         return 2;
    }
    if(num_pos_exp == 0) {
        /* we start with "one over", i.e., 1 */
        tmp[0] = '1';
        len = 1;
    }
    else {
        len = 0;
        /* print positive exp */
        for(i = 0; i < fmi3_SI_base_units_Num; i++) {
            char* curp = tmp + len;
            int expi = exp[i];
            if(expi <= 0)  continue;
            if(len) {
                *curp = '*';
                curp++; len++;
            }

            strcpy(curp, fmi3_SI_base_unit_to_string((fmi3_SI_base_units_enu_t)i)) ;
            len += strlen(curp);
            curp = tmp + len;

            if(expi > 1) {
                *curp = '^';
                curp++; len++;

                sprintf(curp, "%d", expi); /*safe */
                len += strlen(curp);
            }
        }
    }
    /* print negative exp */
    if(num_neg_exp > 0) {
        tmp[len++] = '/';
        if(num_neg_exp > 1)
            tmp[len++] = '(';
        num_neg_exp = 0;

        for(i = 0; i < fmi3_SI_base_units_Num; i++) {
            char* curp = tmp + len;
            int expi = exp[i];
            if(expi >= 0)  continue;
            expi = -expi;
            if(num_neg_exp > 0) {
                *curp = '*';
                curp++; len++;
            }
            num_neg_exp++;
            strcpy(curp, fmi3_SI_base_unit_to_string((fmi3_SI_base_units_enu_t)i)) ;
            len += strlen(curp);
            curp = tmp + len;

            if(expi > 1) {
                *curp = '^';
                curp++; len++;

                sprintf(curp, "%d", expi); /*safe */
                len += strlen(curp);
            }
        }
        if(num_neg_exp > 1)
            tmp[len++] = ')';
    }
    strncpy(buf, tmp, bufSize);
    if(len < bufSize) buf[len] = 0;
    return len + 1;
}
