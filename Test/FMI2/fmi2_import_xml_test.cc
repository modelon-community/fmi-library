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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <time.h>

#include "config_test.h"

#include <FMI/fmi_import_context.h>

#include <FMI2/fmi2_import.h>


void mylogger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message)
{
        printf("module = %s, log level = %d: %s\n", module, log_level, message);
}

int annotation_start_handle(void *context, const char *parentName, void *parent, const char *elm, const char **attr) {
	int i = 0;
	printf("Annotation element %s start (tool: %s, parent:%s)\n", elm, parentName, 
		parent?fmi2_import_get_variable_name((fmi2_import_variable_t*)parent):"model");
	while(attr[i]) {
		printf("Attribute %s = %s\n", attr[i], attr[i+1]);
		i+=2;
	}
	printf("Annotation data:\n");
	return 0;
}

int annotation_data_handle(void* context, const char *s, int len) {
	int i;
	for(i = 0; i < len; i++)
		printf("%c", s[i]);
	return 0;
}

int annotation_end_handle(void *context, const char *elm) {
	printf("\nAnnotation element %s end\n", elm);
	return 0;
}

/** \brief XML callbacks are used to process parts of XML that are not handled by the library */
fmi2_xml_callbacks_t annotation_callbacks = {
	annotation_start_handle,
	annotation_data_handle,
	annotation_end_handle, NULL};


void do_exit(int code)
{
	printf("Press 'Enter' to exit\n");
/*	getchar(); */
	exit(code);
}

void print_int(int i,void* data) {
    printf("%d\n", i);
}

void print_dbl(double d,void* data) {
    printf("%g\n", d);
}

void printTypeInfo(fmi2_import_variable_typedef_t* vt) {
    const char* quan;

    if(!vt) {
        printf("No type definition\n");
        return;
    }

    quan = fmi2_import_get_type_quantity(vt);

    printf("Type %s\n description: %s\n",  fmi2_import_get_type_name(vt), fmi2_import_get_type_description(vt));

    printf("Base type: %s\n", fmi2_base_type_to_string(fmi2_import_get_base_type(vt)));

    if(quan) {
        printf("Quantity: %s\n", quan);
    }
    switch(fmi2_import_get_base_type(vt)) {
    case fmi2_base_type_real: {
        fmi2_import_real_typedef_t* rt = fmi2_import_get_type_as_real(vt);
        fmi2_real_t min = fmi2_import_get_real_type_min(rt);
        fmi2_real_t max = fmi2_import_get_real_type_max(rt);
        fmi2_real_t nom = fmi2_import_get_real_type_nominal(rt);
        fmi2_import_unit_t* u = fmi2_import_get_real_type_unit(rt);
        fmi2_import_display_unit_t* du = fmi2_import_get_type_display_unit(rt);

        printf("Min %g, max %g, nominal %g\n", min, max, nom);

        if(u) {
			char buf[1000];
			fmi2_SI_base_unit_exp_to_string(fmi2_import_get_SI_unit_exponents(u), 1000, buf);
            printf("Unit: %s, base unit %s, factor %g, offset %g\n", 
				fmi2_import_get_unit_name(u), 
				buf,
				fmi2_import_get_SI_unit_factor(u),
				fmi2_import_get_SI_unit_offset(u));
        }
        if(du) {
            printf("Display unit: %s, factor: %g, offset: %g, is relative: %s\n",
                   fmi2_import_get_display_unit_name(du),
                   fmi2_import_get_display_unit_factor(du),
                   fmi2_import_get_display_unit_offset(du),
                   fmi2_import_get_real_type_is_relative_quantity(rt)?"yes":"no"
                   );
        }

        break;
    }
    case fmi2_base_type_int:{
        fmi2_import_integer_typedef_t* it = fmi2_import_get_type_as_int(vt);
        int min = fmi2_import_get_integer_type_min(it);
        int max = fmi2_import_get_integer_type_max(it);
        printf("Min %d, max %d\n", min, max);
        break;
    }
    case fmi2_base_type_bool:{
        break;
    }
    case fmi2_base_type_str:{
        break;
    }
    case fmi2_base_type_enum:{
        fmi2_import_enumeration_typedef_t* et = fmi2_import_get_type_as_enum(vt);
        int min = fmi2_import_get_enum_type_min(et);
        int max = fmi2_import_get_enum_type_max(et);
        printf("Min %d, max %d\n", min, max);
        {
            unsigned ni, i;
            ni = fmi2_import_get_enum_type_size(et);
            printf("There are %d items \n",ni);
            for(i = 1; i <= ni; i++) {
				int val = fmi2_import_get_enum_type_item_value(et, i);
				const char* str = fmi2_import_get_enum_type_value_name(et, val);
				const char* itnm = fmi2_import_get_enum_type_item_name(et, i);
				assert(strcmp(itnm, str)==0);
                printf("[%d] %s=%d (%s) \n", i, 
					itnm, 
					val,
					fmi2_import_get_enum_type_item_description(et, i));
			}
        }
        break;
    }
    default:
        printf("Error in fmiGetBaseType()\n");
    }

}

void testVariableSearch(fmi2_import_t* fmu,
	fmi2_import_variable_t* v) {

		const char * a_name = fmi2_import_get_variable_name(v);
		fmi2_import_variable_t* found = fmi2_import_get_variable_by_name(fmu, a_name);
		if(found != v) {
			printf("Searching by name %s found var %s\n", a_name, found?fmi2_import_get_variable_name(found):"nothing");
			do_exit(1);
		}
		else {
			printf("Searching by name worked fine\n");
		}
		found = fmi2_import_get_variable_by_vr(fmu, fmi2_import_get_variable_base_type(v),fmi2_import_get_variable_vr(v));
		if(!found) {
			printf("Searching by vr failed for variable '%s'\n", a_name);
			do_exit(1);
		}
		else if(fmi2_import_get_variable_base_type(v) != fmi2_import_get_variable_base_type(found)) {			
			printf("Searching %s found var %s", a_name, fmi2_import_get_variable_name(found));
			do_exit(1);
		}
		else if(fmi2_import_get_variable_vr(v) != fmi2_import_get_variable_vr(found)) {			
			printf("Searching %s found var %s", a_name, fmi2_import_get_variable_name(found));
			do_exit(1);
		}
		else {
			printf("Searching by vr worked fine\n");
		}
}

void printVariableInfo(fmi2_import_t* fmu,
                       fmi2_import_variable_t* v) {
    fmi2_base_type_enu_t bt;
    size_t vr = fmi2_import_get_variable_vr(v);
    assert(vr == (unsigned)vr);
    printf("Variable name: %s\n", fmi2_import_get_variable_name(v));
    printf("Description: %s\n", fmi2_import_get_variable_description(v));
    printf("VR: %u\n", (unsigned)vr);
    printf("Variability: %s\n", fmi2_variability_to_string(fmi2_import_get_variability(v)));
    printf("Causality: %s\n", fmi2_causality_to_string(fmi2_import_get_causality(v)));
    printf("Initial: %s\n", fmi2_initial_to_string(fmi2_import_get_initial(v)));

    bt = fmi2_import_get_variable_base_type(v);
    printf("Base type: %s\n", fmi2_base_type_to_string(bt));

    printTypeInfo(fmi2_import_get_variable_declared_type(v));
    if(bt == fmi2_base_type_real) {
        fmi2_import_real_variable_t *rv = fmi2_import_get_variable_as_real(v);
        fmi2_import_unit_t * u = fmi2_import_get_real_variable_unit(rv);
        fmi2_import_display_unit_t * du = fmi2_import_get_real_variable_display_unit(rv);
        printf("Unit: %s, display unit: %s\n", u ? fmi2_import_get_unit_name(u):0, du?fmi2_import_get_display_unit_name(du):"not provided");
    }

    if(fmi2_import_get_variable_has_start(v)) {
        printf("There is a start value\n");

        switch(fmi2_import_get_variable_base_type(v)) {
        case fmi2_base_type_real: {
            fmi2_import_real_variable_t *rv = fmi2_import_get_variable_as_real(v);
            printf("start =%g\n", fmi2_import_get_real_variable_start(rv));
            break;
        }
        case fmi2_base_type_int:{
            printf("start =%d\n", fmi2_import_get_integer_variable_start(fmi2_import_get_variable_as_integer(v)));
            break;
        }
        case fmi2_base_type_bool:{
            printf("start = %d\n", fmi2_import_get_boolean_variable_start(fmi2_import_get_variable_as_boolean(v)));
            break;
        }
        case fmi2_base_type_str:{
            printf("start = '%s'\n", fmi2_import_get_string_variable_start(fmi2_import_get_variable_as_string(v)));
            break;
        }
        case fmi2_base_type_enum:{
            printf("start = %d\n", fmi2_import_get_enum_variable_start(fmi2_import_get_variable_as_enum(v)));
            break;
        }
        default:
            printf("Error in fmiGetBaseType()\n");
        }
    }
    if(fmi2_import_get_variable_alias_kind(v) != fmi2_variable_is_not_alias) {
        printf("The variable is aliased to %s\n",
               fmi2_import_get_variable_name( fmi2_import_get_variable_alias_base(fmu, v)));
    }
    else {
        printf("The variable is not an alias\n");
    }
    {
        fmi2_import_variable_list_t* vl = fmi2_import_get_variable_aliases(fmu, v);
        size_t i, n = fmi2_import_get_variable_list_size(vl);
        if(n>1) {
            printf("Listing aliases: \n");
            for(i = 0;i<n;i++)
                printf("\t%s\n",fmi2_import_get_variable_name(fmi2_import_get_variable(vl, i)));
        }
        fmi2_import_free_variable_list(vl);
    }
}

void printCapabilitiesInfo(fmi2_import_t* fmu) {
	size_t i;

	for( i = 0; i < fmi2_capabilities_Num; ++i) {
		printf("%s = %u\n", 
			fmi2_capability_to_string((fmi2_capabilities_enu_t)i), 
			fmi2_import_get_capability(fmu, (fmi2_capabilities_enu_t)i));
	}
}

void printDependenciesInfo(	fmi2_import_t* fmu, fmi2_import_variable_list_t* rows, fmi2_import_variable_list_t* cols, size_t* start, size_t *dep, char* factor) {
	size_t i, j, nr;
	if(!rows || !cols || !start) {
		printf("Dependencies are not available\n");
		if(rows) {
			nr = fmi2_import_get_variable_list_size(rows);
			for(i = 0; i < nr; i++) {
				printf("\t%s\n",fmi2_import_get_variable_name(fmi2_import_get_variable(rows, i)));
			}
		}
		return;
	}
	nr = fmi2_import_get_variable_list_size(rows);
	for(i = 0; i < nr; i++) {
		if(start[i] == start[i+1]) {
			printf("\t%s has no dependencies\n",fmi2_import_get_variable_name(fmi2_import_get_variable(rows, i)));
		}
		else if((start[i] + 1 == start[i+1]) && (dep[start[i]] == 0)) {
			printf("\t%s depends on all\n",fmi2_import_get_variable_name(fmi2_import_get_variable(rows, i)));
		}
		else {
			printf("\t%s depends on:\n",fmi2_import_get_variable_name(fmi2_import_get_variable(rows, i)));
			for(j = start[i]; j < start[i+1]; j++) {
				printf("\t\t%s (factor kind: %s)\n",fmi2_import_get_variable_name(fmi2_import_get_variable(cols, dep[j]-1)), 
					fmi2_dependency_factor_kind_to_string((fmi2_dependency_factor_kind_enu_t)factor[j]));
			}
		}
	}
}

int main(int argc, char *argv[])
{
    clock_t start, stop;
    double t = 0.0;
	const char* tmpPath;
	jm_callbacks callbacks;
	fmi_import_context_t* context;

	fmi2_import_t* fmu;

	if(argc < 2) {
		printf("Usage: %s <path to a dir with modelDescription.xml>\n", argv[0]);
		do_exit(1);
	}

	tmpPath = argv[1];

	callbacks.malloc = malloc;
    callbacks.calloc = calloc;
    callbacks.realloc = realloc;
    callbacks.free = free;
    callbacks.logger = mylogger;
    callbacks.context = 0;
	callbacks.log_level = jm_log_level_debug;

#ifdef FMILIB_GENERATE_BUILD_STAMP
	printf("Library build stamp:\n%s\n", fmilib_get_build_stamp());
#endif

	context = fmi_import_allocate_context(&callbacks);

	start = clock();
	fmu = fmi2_import_parse_xml(context, tmpPath, &annotation_callbacks);

	    /* Stop timer */
    stop = clock();
    t = (double) (stop-start)/CLOCKS_PER_SEC;
    printf("Parsing took %g seconds\n", t);
	if(!fmu) {
		printf("Error parsing XML, exiting\n");
        fmi_import_free_context(context);
		do_exit(1);
	}


    printf("Model name: %s\n", fmi2_import_get_model_name(fmu));
    printf("Model GUID: %s\n", fmi2_import_get_GUID(fmu));
    printf("FMU kind: %s\n", fmi2_fmu_kind_to_string(fmi2_import_get_fmu_kind(fmu)));
    printf("Description: %s\n", fmi2_import_get_description(fmu));
    printf("Author: %s\n", fmi2_import_get_author(fmu));
    printf("FMI Version: %s\n", fmi2_import_get_model_standard_version(fmu));
    printf("Generation tool: %s\n", fmi2_import_get_generation_tool(fmu));
    printf("Generation date and time: %s\n", fmi2_import_get_generation_date_and_time(fmu));
    printf("Version: %s\n", fmi2_import_get_model_version(fmu));
    printf("Naming : %s\n", fmi2_naming_convention_to_string(fmi2_import_get_naming_convention(fmu)));

    if(fmi2_import_get_fmu_kind(fmu) != fmi2_fmu_kind_cs)
	    printf("Model identifier ME: %s\n", fmi2_import_get_model_identifier_ME(fmu));
    if(fmi2_import_get_fmu_kind(fmu) != fmi2_fmu_kind_me)
	    printf("Model identifier CS: %s\n", fmi2_import_get_model_identifier_CS(fmu));
    printCapabilitiesInfo(fmu);

    printf("NumberOfContinuousStates = " FMILIB_SIZET_FORMAT "\n", fmi2_import_get_number_of_continuous_states(fmu));
    printf("NumberOfEventIndicators = " FMILIB_SIZET_FORMAT "\n", fmi2_import_get_number_of_event_indicators(fmu));

    printf("Default experiment start = %g, end = %g, tolerance = %g, step = %g\n",
           fmi2_import_get_default_experiment_start(fmu),
           fmi2_import_get_default_experiment_stop(fmu),
           fmi2_import_get_default_experiment_tolerance(fmu),
           fmi2_import_get_default_experiment_step(fmu));

    {
        int n_sources = fmi2_import_get_source_files_me_num(fmu);
        int k;
        printf("There are %d source files for ME\n", n_sources);
        for (k=0; k < n_sources; k++) {
            printf("\t%s\n", fmi2_import_get_source_file_me(fmu, k));
        }
    }
    {
        int n_sources = fmi2_import_get_source_files_cs_num(fmu);
        int k;
        printf("There are %d source files for CS\n", n_sources);
        for (k=0; k < n_sources; k++) {
            printf("\t%s\n", fmi2_import_get_source_file_cs(fmu, k));
        }
    }


    {
        size_t i, nv = fmi2_import_get_vendors_num(fmu);
        printf("There are %u tool annotation records \n", (unsigned)nv);
        for( i = 0; i < nv; i++) {
            printf("Vendor name [%u] %s", (unsigned)i, fmi2_import_get_vendor_name(fmu, i));
		}
	}
    {
        fmi2_import_unit_definitions_t* ud = fmi2_import_get_unit_definitions(fmu);
        if(ud) {
            unsigned  i, nu = fmi2_import_get_unit_definitions_number(ud);
            printf("There are %d different units used \n", nu);

            for(i = 0; i < nu; i++) {
                fmi2_import_unit_t* u = fmi2_import_get_unit(ud, i);
				char buf[1000];
                if(!u) {
                    printf("Error getting unit for index %d (%s)\n", i, fmi2_import_get_last_error(fmu));
                    break;
                }
				fmi2_SI_base_unit_exp_to_string(fmi2_import_get_SI_unit_exponents(u), 1000, buf);
                printf("Unit [%d] is %s, base unit %s, factor %g, offset %g, it has %d display units\n", 
					i, fmi2_import_get_unit_name(u), 
					buf,
					fmi2_import_get_SI_unit_factor(u),
					fmi2_import_get_SI_unit_offset(u),
					fmi2_import_get_unit_display_unit_number(u));
            }
        }
        else
            printf("Error getting unit definitions (%s)\n", fmi2_import_get_last_error(fmu));
    }
    {
        fmi2_import_type_definitions_t* td = fmi2_import_get_type_definitions(fmu);
        if(td) {
            {
                unsigned i, ntd = fmi2_import_get_type_definition_number(td);
                printf("There are %d defs\n", ntd);
                for(i = 0; i < ntd; i++) {
                    fmi2_import_variable_typedef_t* vt = fmi2_import_get_typedef(td, i);
                    if(!vt) {
                        printf("Error getting vartype for index %d (%s)\n", i, fmi2_import_get_last_error(fmu));
                        break;
                    }
                    printTypeInfo(vt);
                }
            }
        }
        else
            printf("Error getting type definitions (%s)\n", fmi2_import_get_last_error(fmu));
    }
    {
        size_t nv, i;
        fmi2_import_variable_list_t* vl = fmi2_import_get_variable_list(fmu, 0);
/*		fmi2_import_variable_list_t* ders = fmi2_import_get_derivatives_list( fmu); */
		const fmi2_value_reference_t* vrl = fmi2_import_get_value_referece_list(vl);


        assert(vl);
		
        nv = fmi2_import_get_variable_list_size(vl);
        printf("There are %u variables in total \n",(unsigned)nv);
        for(i = 0; i < nv; i++) {
            fmi2_import_variable_t* var = fmi2_import_get_variable(vl, i);
			assert(vrl[i] == fmi2_import_get_variable_vr(var));
            if(!var) {
				printf("Something wrong with variable %u \n",(unsigned)i);
				do_exit(1);
			}
            else {
                printVariableInfo(fmu, var);
/*				size_t stateIndex = fmi2_import_get_state_index(var);
				if(stateIndex) {
					printf("This variable is a state. Its derivative: %s\n", 
						fmi2_import_get_variable_name(fmi2_import_get_variable(ders, stateIndex-1)));
				} */
				testVariableSearch(fmu, var);
			}
        }
        fmi2_import_free_variable_list(vl);
/*        fmi2_import_free_variable_list(ders); */
    }
/*	{
		fmi2_import_variable_list_t* vl = fmi2_import_get_inputs_list( fmu);
        size_t i, n = 0;
		if(vl) 
			n = fmi2_import_get_variable_list_size(vl);
        if(n>0) {
            printf("Listing inputs: \n");
            for(i = 0;i<n;i++) 
                printf("\t%s\n",fmi2_import_get_variable_name(fmi2_import_get_variable(vl, i)));
        }
		else {
            printf("There are no inputs\n");
		}
        fmi2_import_free_variable_list(vl);
	}	
	{
		fmi2_import_variable_list_t* states = fmi2_import_get_states_list( fmu);
		fmi2_import_variable_list_t* inputs = fmi2_import_get_inputs_list( fmu);
        size_t n = 0;
		if(states) 
			n = fmi2_import_get_variable_list_size(states);
        if(n>0) {
			size_t *start, *dep;
			char* factor;
            printf("Listing states and dependencies on inputs: \n");
			fmi2_import_get_dependencies_derivatives_on_inputs(fmu, &start, &dep, &factor);
			printDependenciesInfo(	fmu, states, inputs, start, dep, factor);

			fmi2_import_get_dependencies_derivatives_on_states(fmu, &start, &dep, &factor);
			if(start) {
				printf("Listing states and dependencies on other states: \n");
				printDependenciesInfo(	fmu, states, states, start, dep, factor);
			}
			else {
				printf("No dependencies on states available\n");
			}
        }
		else {
            printf("There are no states\n");
		}
        fmi2_import_free_variable_list(inputs);
        fmi2_import_free_variable_list(states);
	}	
	{
		fmi2_import_variable_list_t* states = fmi2_import_get_states_list( fmu);
		fmi2_import_variable_list_t* inputs = fmi2_import_get_inputs_list( fmu);
		fmi2_import_variable_list_t* outputs = fmi2_import_get_outputs_list( fmu);
        size_t n = 0;
		if(outputs) 
			n = fmi2_import_get_variable_list_size(outputs);
        if(n>0) {
			size_t *start, *dep;
			char* factor;
            printf("Listing outputs and dependencies on inputs: \n");
			fmi2_import_get_dependencies_outputs_on_inputs(fmu, &start, &dep, &factor);
			printDependenciesInfo(	fmu, outputs, inputs, start, dep, factor);

			fmi2_import_get_dependencies_outputs_on_states(fmu, &start, &dep, &factor);
			if(start) {
				printf("Listing outputs and dependencies on states: \n");
				printDependenciesInfo(	fmu, outputs, states, start, dep, factor);
			}
			else {
				printf("No dependencies on states available\n");
			}
        }
		else {
            printf("There are no outputs\n");
		}
        fmi2_import_free_variable_list(outputs);
        fmi2_import_free_variable_list(inputs);
        fmi2_import_free_variable_list(states);
	}	
*/
	fmi2_import_free(fmu);
	fmi_import_free_context(context);
	
	printf("Everything seems to be OK since you got this far=)!\n");

	do_exit(0);
}


