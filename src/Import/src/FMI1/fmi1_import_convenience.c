/*
    Copyright (C) 2012 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include <FMI1/fmi1_xml_model_description.h>

#include "fmi1_import_impl.h"

/**
	\brief Collect model information by counting the number of variables with specific properties and fillinf in fmi1_import_model_counts_t struct.
	\param fmu - An fmu object as returned by fmi1_import_parse_xml().
	\param counts - a pointer to a preallocated struct.
*/
void fmi1_import_collect_model_counts(fmi1_import_t* fmu, fmi1_import_model_counts_t* counts) {
	jm_vector(jm_voidp)* vars = fmi1_xml_get_variables_original_order(fmu->md);
    size_t nv, i;
    nv = jm_vector_get_size(jm_voidp)(vars);
	memset(counts,0,sizeof(fmi1_import_model_counts_t));
    for(i = 0; i< nv; i++) {
		fmi1_xml_variable_t* var = (fmi1_xml_variable_t*)jm_vector_get_item(jm_voidp)(vars, i); 
		switch (fmi1_xml_get_variability(var)) {
		case fmi1_variability_enu_constant:
			counts->num_constants++;
			break;
		case fmi1_variability_enu_parameter:
			counts->num_parameters++;
			break;
		case fmi1_variability_enu_discrete:
			counts->num_discrete++;
			break;
		case fmi1_variability_enu_continuous:
			counts->num_continuous++;
			break;
		default:
			assert(0);
		}
		switch(fmi1_xml_get_causality(var)) {
		case fmi1_causality_enu_none:
			counts->num_causality_none++;
			break;
		case fmi1_causality_enu_input:
			counts->num_inputs++;
			break;
		case fmi1_causality_enu_output:
			counts->num_outputs++;
			break;
		case fmi1_causality_enu_internal:
			counts->num_internal++;
			break;
		default: assert(0);
		}
		switch(fmi1_xml_get_variable_base_type(var)) {
		case fmi1_base_type_real:
			counts->num_real_vars++;
			break;
		case fmi1_base_type_int:
			counts->num_integer_vars++;
			break;
		case fmi1_base_type_bool:
			counts->num_bool_vars++;
			break;
		case fmi1_base_type_str:
			counts->num_string_vars++;
			break;
		case fmi1_base_type_enum:
			counts->num_enum_vars++;
			break;
		default:
			assert(0);
		}
    }
    return;
}