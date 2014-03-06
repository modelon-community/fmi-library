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
#include <assert.h>
#include <stdio.h>

#include <FMI2/fmi2_functions.h>
#include <FMI2/fmi2_enums.h>

const char* fmi2_naming_convention_to_string(fmi2_variable_naming_convension_enu_t convention) {
    if(convention == fmi2_naming_enu_flat) return "flat";
    if(convention == fmi2_naming_enu_structured) return "structured";
    return "Unknown";
}

const char* fmi2_fmu_kind_to_string(fmi2_fmu_kind_enu_t kind) {
    switch (kind) {
    case fmi2_fmu_kind_me: return "ModelExchange";
    case fmi2_fmu_kind_cs: return "CoSimulation";
    case fmi2_fmu_kind_me_and_cs: return "ModelExchange and CoSimulation";
	default: break;
    }
    return "Unknown";
}

const char* fmi2_dependency_factor_kind_to_string(fmi2_dependency_factor_kind_enu_t fc) {
	switch(fc) {
	case fmi2_dependency_factor_kind_dependent: return "dependent";
	case fmi2_dependency_factor_kind_constant: return "constant";
	case fmi2_dependency_factor_kind_fixed: return "fixed";
	case fmi2_dependency_factor_kind_tunable: return "tunable";
	case fmi2_dependency_factor_kind_discrete: return "discrete";
	default:break;
	}
	return "illegal factor kind";
}

const char* fmi2_variability_to_string(fmi2_variability_enu_t v) {
    switch(v) {
    case fmi2_variability_enu_constant: return "constant";
    case fmi2_variability_enu_fixed: return "fixed";
    case fmi2_variability_enu_tunable: return "tunable";
    case fmi2_variability_enu_discrete: return "discrete";
    case fmi2_variability_enu_continuous: return "continuous";
	default: break;
    }
    return "Unknown";
}

const char* fmi2_causality_to_string(fmi2_causality_enu_t c) {
    switch(c) {
    case fmi2_causality_enu_input: return "input";
    case fmi2_causality_enu_output: return "output";
    case fmi2_causality_enu_parameter: return "parameter";
    case fmi2_causality_enu_calculated_parameter: return "calculatedParameter";
    case fmi2_causality_enu_local: return "local";
    case fmi2_causality_enu_independent: return "independent";
	default: break;
    };
    return "Unknown";
}

const char* fmi2_status_to_string(fmi2_status_t status) {
	switch(status) {
	case fmi2_status_ok:
		return "OK";
	case fmi2_status_warning:
		return "Warning";
	case fmi2_status_discard:
		return "Discard";
	case fmi2_status_error:
		return "Error";
	case fmi2_status_fatal:
		return "Fatal";
	case fmi2_status_pending:
		return "Pending";
	default: break;
	}
	return "Undefined";
}

const char* fmi2_base_type_to_string(fmi2_base_type_enu_t bt) {
    switch(bt) {
    case fmi2_base_type_real: return "Real";
    case fmi2_base_type_int: return "Integer";
    case fmi2_base_type_bool: return "Boolean";
    case fmi2_base_type_str: return "String";
    case fmi2_base_type_enum: return "Enumeration";
	default: break;
    }
    return "Error";
}

const char* fmi2_initial_to_string(fmi2_initial_enu_t ini) {
	switch (ini) {
	case fmi2_initial_enu_exact: return "exact";
	case fmi2_initial_enu_approx: return "approx";
	case fmi2_initial_enu_calculated: return "calculated";
		default: break;
	}
	return "Undefined";
}

fmi2_initial_enu_t initialDefaultsTable[fmi2_variability_enu_unknown][fmi2_causality_enu_unknown] = {
	/*              parameter                  calculated parameter,        input                     output                       local                        independent */
	/* constant */  {fmi2_initial_enu_unknown, fmi2_initial_enu_unknown,    fmi2_initial_enu_unknown, fmi2_initial_enu_exact,      fmi2_initial_enu_exact,      fmi2_initial_enu_unknown},
	/* fixed   */   {fmi2_initial_enu_exact,   fmi2_initial_enu_calculated, fmi2_initial_enu_approx,  fmi2_initial_enu_calculated, fmi2_initial_enu_calculated, fmi2_initial_enu_unknown},
	/* tunable */   {fmi2_initial_enu_exact,   fmi2_initial_enu_calculated, fmi2_initial_enu_approx,  fmi2_initial_enu_calculated, fmi2_initial_enu_calculated, fmi2_initial_enu_unknown},
	/* discrete */  {fmi2_initial_enu_unknown, fmi2_initial_enu_unknown,    fmi2_initial_enu_approx,  fmi2_initial_enu_calculated, fmi2_initial_enu_calculated, fmi2_initial_enu_unknown},
	/* continuous */{fmi2_initial_enu_unknown, fmi2_initial_enu_unknown,    fmi2_initial_enu_approx,  fmi2_initial_enu_calculated, fmi2_initial_enu_calculated, fmi2_initial_enu_calculated}
};

fmi2_initial_enu_t fmi2_get_default_initial(fmi2_variability_enu_t v, fmi2_causality_enu_t c) {
	if((unsigned)v >= (unsigned)fmi2_variability_enu_unknown) return fmi2_initial_enu_unknown;
	if((unsigned)c >= (unsigned)fmi2_causality_enu_unknown) return fmi2_initial_enu_unknown;
	return initialDefaultsTable[v][c];
}

FMILIB_EXPORT fmi2_initial_enu_t fmi2_get_valid_initial(fmi2_variability_enu_t v, fmi2_causality_enu_t c, fmi2_initial_enu_t i) {
	fmi2_initial_enu_t defaultInitial = fmi2_get_default_initial(v, c);
	if( (defaultInitial == i) ||
		((unsigned)i >= (unsigned)fmi2_initial_enu_unknown) ||
		(defaultInitial == fmi2_initial_enu_unknown))
		return defaultInitial;
	/*  At this point we know that v, c & i are fine and that (defaultInitial != i) 
		Check the other allowed combinations: */
	if(defaultInitial == fmi2_initial_enu_calculated) {
		if(v >= fmi2_variability_enu_discrete) {
			return i;
		}
		else if(i != fmi2_initial_enu_exact)
			return i;
	}
	/* in all other cases the combination is not valid and default should be used */
	return defaultInitial;
}


const char * fmi2_capability_to_string(fmi2_capabilities_enu_t id) {
#define FMI2_ME_CAPABILITIES_ENU_TO_STR(c) case fmi2_me_ ## c: return "me_"#c;
#define FMI2_CS_CAPABILITIES_ENU_TO_STR(c) case fmi2_cs_ ## c: return "cs_"#c;
	switch (id) {
	FMI2_ME_CAPABILITIES(FMI2_ME_CAPABILITIES_ENU_TO_STR)
	FMI2_CS_CAPABILITIES(FMI2_CS_CAPABILITIES_ENU_TO_STR)
	default: break;
	}
	return "Unknown";
}

const char * fmi2_SI_base_unit_to_string(fmi2_SI_base_units_enu_t id) {
#define FMI2_SI_BASE_UNIT_ENU_TO_STR(c) case fmi2_SI_base_unit_ ## c: return #c;
	switch(id) {
		FMI2_SI_BASE_UNITS(FMI2_SI_BASE_UNIT_ENU_TO_STR)
	default: break;
	}
	return "unknown";
}

/** \brief Convert a list of SI base unit exponents (corresponding to the IDs from  fmi2_SI_base_units_enu_t)
	to a string of the form kg*m^2/s^2.

	\param exp An array of SI base units exponents.
	\param bufSize Size of the buffer to store the string. 
	\param buf Buffer to store the string
	\return Required size of the buffer to store the string including the terminating zero.
		This most likely be under [8*fmi2_SI_base_units_Num]. If the return value is larger or equal 
		than bufSize than the string could not be fitted in the buffer. 
*/
size_t fmi2_SI_base_unit_exp_to_string(const int exp[fmi2_SI_base_units_Num], size_t bufSize, char buf[]){
	int i = 0;
	int num_pos_exp = 0, num_neg_exp = 0; /* number of exponents */
	size_t len = 0;
	char tmp[fmi2_SI_base_units_Num * 20]; /* tmp is always enough size */
	if(bufSize) buf[0] = 0; /* just to prevent errors */

	/* count the exponents */
	for(i = 0; i < fmi2_SI_base_units_Num; i++) {
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
		for(i = 0; i < fmi2_SI_base_units_Num; i++) {
			char* curp = tmp + len;
			int expi = exp[i];
			if(expi <= 0)  continue;
			if(len) {
				*curp = '*';
				curp++; len++;
			}

			strcpy(curp, fmi2_SI_base_unit_to_string((fmi2_SI_base_units_enu_t)i)) ;
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

		for(i = 0; i < fmi2_SI_base_units_Num; i++) {
			char* curp = tmp + len;
			int expi = exp[i];
			if(expi >= 0)  continue;
			expi = -expi;
			if(num_neg_exp > 0) {
				*curp = '*';
				curp++; len++;
			}
			num_neg_exp++;
			strcpy(curp, fmi2_SI_base_unit_to_string((fmi2_SI_base_units_enu_t)i)) ;
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
