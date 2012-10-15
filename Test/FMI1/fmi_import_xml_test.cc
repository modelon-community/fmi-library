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

#include <FMI1/fmi1_import.h>


void mylogger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message)
{
        printf("module = %s, log level = %d: %s\n", module, log_level, message);
}

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

void printTypeInfo(fmi1_import_variable_typedef_t* vt) {
    const char* quan;

    if(!vt) {
        printf("No type definition\n");
        return;
    }

    quan = fmi1_import_get_type_quantity(vt);

    printf("Type %s\n description: %s\n",  fmi1_import_get_type_name(vt), fmi1_import_get_type_description(vt));

    printf("Base type: %s\n", fmi1_base_type_to_string(fmi1_import_get_base_type(vt)));

    if(quan) {
        printf("Quantity: %s\n", quan);
    }
    switch(fmi1_import_get_base_type(vt)) {
    case fmi1_base_type_real: {
        fmi1_import_real_typedef_t* rt = fmi1_import_get_type_as_real(vt);
        fmi1_real_t min = fmi1_import_get_real_type_min(rt);
        fmi1_real_t max = fmi1_import_get_real_type_max(rt);
        fmi1_real_t nom = fmi1_import_get_real_type_nominal(rt);
        fmi1_import_unit_t* u = fmi1_import_get_real_type_unit(rt);
        fmi1_import_display_unit_t* du = fmi1_import_get_type_display_unit(rt);

        printf("Min %g, max %g, nominal %g\n", min, max, nom);

        if(u) {
            printf("Unit: %s\n", fmi1_import_get_unit_name(u));
        }
        if(du) {
            printf("Display unit: %s, gain: %g, offset: %g, is relative: %s",
                   fmi1_import_get_display_unit_name(du),
                   fmi1_import_get_display_unit_gain(du),
                   fmi1_import_get_display_unit_offset(du),
                   fmi1_import_get_real_type_is_relative_quantity(rt)?"yes":"no"
                   );
        }

        break;
    }
    case fmi1_base_type_int:{
        fmi1_import_integer_typedef_t* it = fmi1_import_get_type_as_int(vt);
        int min = fmi1_import_get_integer_type_min(it);
        int max = fmi1_import_get_integer_type_max(it);
        printf("Min %d, max %d\n", min, max);
        break;
    }
    case fmi1_base_type_bool:{
        break;
    }
    case fmi1_base_type_str:{
        break;
    }
    case fmi1_base_type_enum:{
        fmi1_import_enumeration_typedef_t* et = fmi1_import_get_type_as_enum(vt);
        int min = fmi1_import_get_enum_type_min(et);
        int max = fmi1_import_get_enum_type_max(et);
        printf("Min %d, max %d\n", min, max);
        {
            size_t ni;
			unsigned i;
            ni = fmi1_import_get_enum_type_size(et);
			i = (unsigned)(ni);
			assert( i == ni);
            printf("There are %u items \n",(unsigned)ni);
            for(i = 0; i < ni; i++) {
                printf("[%u] %s (%s) \n", (unsigned)i+1, fmi1_import_get_enum_type_item_name(et, i), fmi1_import_get_enum_type_item_description(et, i));
            }
        }
        break;
    }
    default:
        printf("Error in fmiGetBaseType()\n");
    }

}

void testVariableSearch(fmi1_import_t* fmu,
	fmi1_import_variable_t* v) {

		const char * a_name = fmi1_import_get_variable_name(v);
		fmi1_import_variable_t* found = fmi1_import_get_variable_by_name(fmu, a_name);
		if(found != v) {
			printf("Searching by name %s found var %s\n", a_name, found?fmi1_import_get_variable_name(found):"nothing");
			do_exit(1);
		}
		else {
			printf("Searching by name worked fine\n");
		}
		found = fmi1_import_get_variable_by_vr(fmu, fmi1_import_get_variable_base_type(v),fmi1_import_get_variable_vr(v));
		if(!found) {
			printf("Searching by vr failed for variable '%s'\n", a_name);
			do_exit(1);
		}
		else if(fmi1_import_get_variable_base_type(v) != fmi1_import_get_variable_base_type(found)) {			
			printf("Searching %s found var %s", a_name, fmi1_import_get_variable_name(found));
			do_exit(1);
		}
		else if(fmi1_import_get_variable_vr(v) != fmi1_import_get_variable_vr(found)) {			
			printf("Searching %s found var %s", a_name, fmi1_import_get_variable_name(found));
			do_exit(1);
		}
		else {
			printf("Searching by vr worked fine\n");
		}
}

void printVariableInfo(fmi1_import_t* fmu,
                       fmi1_import_variable_t* v) {
    fmi1_base_type_enu_t bt;
    printf("Variable name: %s\n", fmi1_import_get_variable_name(v));
    printf("Description: %s\n", fmi1_import_get_variable_description(v));
    printf("VR: %d\n", fmi1_import_get_variable_vr(v));
    printf("Variability: %s\n", fmi1_variability_to_string(fmi1_import_get_variability(v)));
    printf("Causality: %s\n", fmi1_causality_to_string(fmi1_import_get_causality(v)));

    bt = fmi1_import_get_variable_base_type(v);
    printf("Base type: %s\n", fmi1_base_type_to_string(bt));

    printTypeInfo(fmi1_import_get_variable_declared_type(v));
    if(bt == fmi1_base_type_real) {
        fmi1_import_real_variable_t *rv = fmi1_import_get_variable_as_real(v);
        fmi1_import_unit_t * u = fmi1_import_get_real_variable_unit(rv);
        fmi1_import_display_unit_t * du = fmi1_import_get_real_variable_display_unit(rv);
        printf("Unit: %s, display unit: %s\n", u ? fmi1_import_get_unit_name(u):0, du?fmi1_import_get_display_unit_name(du):0);
    }

    if(fmi1_import_get_variable_has_start(v)) {
        printf("There is a start value, fixed attribute is '%s'\n", (fmi1_import_get_variable_is_fixed(v))?"true":"false");

        switch(fmi1_import_get_variable_base_type(v)) {
        case fmi1_base_type_real: {
            fmi1_import_real_variable_t *rv = fmi1_import_get_variable_as_real(v);
            printf("start =%g\n", fmi1_import_get_real_variable_start(rv));
            break;
        }
        case fmi1_base_type_int:{
            printf("start =%d\n", fmi1_import_get_integer_variable_start(fmi1_import_get_variable_as_integer(v)));
            break;
        }
        case fmi1_base_type_bool:{
            printf("start = %d\n", fmi1_import_get_boolean_variable_start(fmi1_import_get_variable_as_boolean(v)));
            break;
        }
        case fmi1_base_type_str:{
            printf("start = '%s'\n", fmi1_import_get_string_variable_start(fmi1_import_get_variable_as_string(v)));
            break;
        }
        case fmi1_base_type_enum:{
            printf("start = %d\n", fmi1_import_get_enum_variable_start(fmi1_import_get_variable_as_enum(v)));
            break;
        }
        default:
            printf("Error in fmiGetBaseType()\n");
        }
    }
    if(fmi1_import_get_variable_alias_kind(v) != fmi1_variable_is_not_alias) {
        printf("The variable is aliased to %s\n",
               fmi1_import_get_variable_name( fmi1_import_get_variable_alias_base(fmu, v)));
    }
    else {
        printf("The variable is not an alias\n");
    }
    {
        fmi1_import_variable_list_t* vl = fmi1_import_get_variable_aliases(fmu, v);
        size_t n = fmi1_import_get_variable_list_size(vl);
		unsigned i = (unsigned)n;
		assert( n == i);
        if(n>1) {
            printf("Listing aliases: \n");
            for(i = 0;i<n;i++)
                printf("\t%s\n",fmi1_import_get_variable_name(fmi1_import_get_variable(vl, i)));
        }
        fmi1_import_free_variable_list(vl);
    }
	{
		fmi1_import_variable_list_t* vl = fmi1_import_get_direct_dependency( fmu, v);
        size_t n = 0;
		unsigned i;
		if(vl) 
			n = fmi1_import_get_variable_list_size(vl);
		i = (unsigned)n;
		assert( n == i);		
        if(n>0) {
            printf("Listing direct dependencies: \n");
            for(i = 0;i<n;i++)
                printf("\t%s\n",fmi1_import_get_variable_name(fmi1_import_get_variable(vl, i)));
        }
        fmi1_import_free_variable_list(vl);
	}
}

void printCapabilitiesInfo(fmi1_import_capabilities_t* capabilities) {
    printf("canHandleVariableCommunicationStepSize = %u\n", fmi1_import_get_canHandleVariableCommunicationStepSize(capabilities ));
    printf("canHandleEvents = %u\n", fmi1_import_get_canHandleEvents(capabilities ));
    printf("canRejectSteps = %u\n", fmi1_import_get_canRejectSteps(capabilities ));
    printf("canInterpolateInputs = %u\n", fmi1_import_get_canInterpolateInputs(capabilities ));
    printf("maxOutputDerivativeOrder = %u\n", fmi1_import_get_maxOutputDerivativeOrder(capabilities ));
    printf("canRunAsynchronuously = %u\n", fmi1_import_get_canRunAsynchronuously(capabilities ));
    printf("canSignalEvents = %u\n", fmi1_import_get_canSignalEvents(capabilities ));
    printf("canBeInstantiatedOnlyOncePerProcess = %u\n", fmi1_import_get_canBeInstantiatedOnlyOncePerProcess(capabilities ));
    printf("canNotUseMemoryManagementFunctions = %u\n", fmi1_import_get_canNotUseMemoryManagementFunctions(capabilities ));
}


int main(int argc, char *argv[])
{
    clock_t start, stop;
    double t = 0.0;
	const char* tmpPath;
	jm_callbacks callbacks;
	fmi_import_context_t* context;

	fmi1_import_t* fmu;

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
	fmu = fmi1_import_parse_xml(context, tmpPath);

	if(!fmu) {
		printf("Error parsing XML, exiting\n");
        fmi_import_free_context(context);       
		do_exit(1);
	}

	    /* Stop timer */
    stop = clock();
    t = (double) (stop-start)/CLOCKS_PER_SEC;

    printf("Parsing took %g seconds\n", t);
    printf("Model name: %s\n", fmi1_import_get_model_name(fmu));
    printf("Model identifier: %s\n", fmi1_import_get_model_identifier(fmu));
    printf("Model GUID: %s\n", fmi1_import_get_GUID(fmu));
    printf("FMU kind: %s\n", fmi1_fmu_kind_to_string(fmi1_import_get_fmu_kind(fmu)));
    printf("Description: %s\n", fmi1_import_get_description(fmu));
    printf("Author: %s\n", fmi1_import_get_author(fmu));
    printf("FMI Version: %s\n", fmi1_import_get_model_standard_version(fmu));
    printf("Generation tool: %s\n", fmi1_import_get_generation_tool(fmu));
    printf("Generation date and time: %s\n", fmi1_import_get_generation_date_and_time(fmu));
    printf("Version: %s\n", fmi1_import_get_model_version(fmu));
    printf("Naming : %s\n", fmi1_naming_convention_to_string(fmi1_import_get_naming_convention(fmu)));

    if(fmi1_import_get_fmu_kind(fmu) != fmi1_fmu_kind_enu_me)
        printCapabilitiesInfo(fmi1_import_get_capabilities(fmu));

    printf("NumberOfContinuousStates = %d\n", fmi1_import_get_number_of_continuous_states(fmu));
    printf("NumberOfEventIndicators = %d\n", fmi1_import_get_number_of_event_indicators(fmu));

    printf("Default experiment start = %g, end = %g, tolerance = %g\n",
           fmi1_import_get_default_experiment_start(fmu),
           fmi1_import_get_default_experiment_stop(fmu),
           fmi1_import_get_default_experiment_tolerance(fmu));
    {
        fmi1_import_vendor_list_t* vl = fmi1_import_get_vendor_list(fmu);
        size_t nv = fmi1_import_get_number_of_vendors(vl);
		unsigned i;
		i = (unsigned)nv;
		assert( nv == i);		
        printf("There are %u tool annotation records \n", (unsigned)nv);
        for( i = 0; i < nv; i++) {
            fmi1_import_vendor_t* vendor = fmi1_import_get_vendor(vl, i);
            if(!vendor) {
                printf("Error getting vendor for index %u\n", (unsigned)i);
                break;
            }
            printf("Vendor name [%d] %s", i, fmi1_import_get_vendor_name(vendor));
            {
                unsigned j, na = fmi1_import_get_number_of_vendor_annotations(vendor);

                for(j = 0; j< na; j++) {
                    fmi1_import_annotation_t* a = fmi1_import_get_vendor_annotation(vendor, j);
                    if(!a) {
                        printf("Error getting vendor for index %d (%s)\n", j, fmi1_import_get_last_error(fmu));
                        break;
                    }

                    printf("Annotation: %s = %s", fmi1_import_get_annotation_name(a), fmi1_import_get_annotation_value(a));
                }
            }
        }
    }
    {
        fmi1_import_unit_definitions_t* ud = fmi1_import_get_unit_definitions(fmu);
        if(ud) {
            unsigned  i, nu = fmi1_import_get_unit_definitions_number(ud);
            printf("There are %d different units used \n", nu);

            for(i = 0; i < nu; i++) {
                fmi1_import_unit_t* u = fmi1_import_get_unit(ud, i);
                if(!u) {
                    printf("Error getting unit for index %d (%s)\n", i, fmi1_import_get_last_error(fmu));
                    break;
                }
                printf("Unit [%d] is %s, it has %d display units\n", i, fmi1_import_get_unit_name(u), fmi1_import_get_unit_display_unit_number(u));
            }
        }
        else
            printf("Error getting unit definitions (%s)\n", fmi1_import_get_last_error(fmu));
    }
    {
        fmi1_import_type_definitions_t* td = fmi1_import_get_type_definitions(fmu);
        if(td) {
            {
                unsigned i, ntd = (unsigned)fmi1_import_get_type_definition_number(td);
                printf("There are %d defs\n", ntd);
                for(i = 0; i < ntd; i++) {
                    fmi1_import_variable_typedef_t* vt = fmi1_import_get_typedef(td, i);
                    if(!vt) {
                        printf("Error getting vartype for index %d (%s)\n", i, fmi1_import_get_last_error(fmu));
                        break;
                    }
                    printTypeInfo(vt);
                }
            }
        }
        else
            printf("Error getting type definitions (%s)\n", fmi1_import_get_last_error(fmu));
    }
    {
        size_t nv;
		unsigned i;
        fmi1_import_variable_list_t* vl = fmi1_import_get_variable_list(fmu);

        assert(vl);
        nv = fmi1_import_get_variable_list_size(vl);
		i = (unsigned)nv;
		assert(i == nv);
		printf("There are %u variables in total \n",(unsigned)nv);
        for(i = 0; i < nv; i++) {
            fmi1_import_variable_t* var = fmi1_import_get_variable(vl, i);
            if(!var) {
				printf("Something wrong with variable %d \n",i);
				do_exit(1);
			}
            else {
                printVariableInfo(fmu, var);
				testVariableSearch(fmu, var);
			}
        }
        fmi1_import_free_variable_list(vl);
    }

	fmi1_import_free(fmu);
	fmi_import_free_context(context);
	
	printf("Everything seems to be OK since you got this far=)!\n");

	do_exit(0);
}


