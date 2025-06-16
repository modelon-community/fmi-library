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

/**
 * This test mainly just prints things instead of doing asserts (it's old), but
 * there's still some value in it since it can detect segfaults, and it's using the
 * API on a model (BouncingBall) which is a bit larger and diverse than the usual test
 * modelDescription files.
 *
 * It also gives an exmaple on how to define callbacks for custom Annotations.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <time.h>
#include <inttypes.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#include "catch.hpp"


int annotation_start_handle(void *context, const char *parentName, void *parent, const char *elm, const char **attr) {
    int i = 0;
    printf("Annotation element %s start (tool: %s, parent:%s)\n", elm, parentName,
        parent?fmi3_import_get_variable_name((fmi3_import_variable_t*)parent):"model");
    while(attr[i]) {
        printf("Attribute %s = %s\n", attr[i], attr[i+1]);
        i+=2;
    }
    printf("Annotation data:\n");
    return 0;
}

int annotation_data_handle(void* context, const char *s, int len) {
    int i;
    for (i = 0; i < len; i++)
        printf("%c", s[i]);
    return 0;
}

int annotation_end_handle(void *context, const char *elm) {
    printf("\nAnnotation element %s end\n", elm);
    return 0;
}

/** \brief XML callbacks are used to process parts of XML that are not handled by the library */
fmi3_xml_callbacks_t annotation_callbacks = {
        annotation_start_handle,
        annotation_data_handle,
        annotation_end_handle,
        NULL};

void mylogger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message)
{
    printf("[%s][%s] %s\n", module, jm_log_level_to_string(log_level), message);
}

void printTypeInfo(fmi3_import_variable_typedef_t* vt) {
    const char* quan;

    if (!vt) {
        printf("No type definition\n");
        return;
    }

    quan = fmi3_import_get_type_quantity(vt);

    printf("Type %s\n description: %s\n",  fmi3_import_get_type_name(vt), fmi3_import_get_type_description(vt));

    printf("Base type: %s\n", fmi3_base_type_to_string(fmi3_import_get_base_type(vt)));

    if (quan) {
        printf("Quantity: %s\n", quan);
    }
    switch (fmi3_import_get_base_type(vt)) {
    case fmi3_base_type_float64: {
        fmi3_import_float_typedef_t* rt = fmi3_import_get_type_as_float(vt);
        fmi3_float64_t min = fmi3_import_get_float64_type_min(rt);
        fmi3_float64_t max = fmi3_import_get_float64_type_max(rt);
        fmi3_float64_t nom = fmi3_import_get_float64_type_nominal(rt);
        fmi3_import_unit_t* u = fmi3_import_get_float64_type_unit(rt);
        fmi3_import_display_unit_t* du = fmi3_import_get_float64_type_display_unit(rt);

        printf("Min %g, max %g, nominal %g\n", min, max, nom);

        if (u) {
            char buf[1000];
            fmi3_SI_base_unit_exp_to_string(fmi3_import_get_SI_unit_exponents(u), 1000, buf);
            printf("Unit: %s, base unit %s, factor %g, offset %g\n",
                fmi3_import_get_unit_name(u),
                buf,
                fmi3_import_get_SI_unit_factor(u),
                fmi3_import_get_SI_unit_offset(u));
        }
        if (du) {
            printf("Display unit: %s, factor: %g, offset: %g, is relative: %s\n",
                   fmi3_import_get_display_unit_name(du),
                   fmi3_import_get_display_unit_factor(du),
                   fmi3_import_get_display_unit_offset(du),
                   fmi3_import_get_float64_type_is_relative_quantity(rt)?"yes":"no"
                   );
        }

        break;
    }
    case fmi3_base_type_int32:{
        fmi3_import_int_typedef_t* it = fmi3_import_get_type_as_int(vt);
        int min = fmi3_import_get_int32_type_min(it);
        int max = fmi3_import_get_int32_type_max(it);
        printf("Min %d, max %d\n", min, max);
        break;
    }
    case fmi3_base_type_bool:{
        break;
    }
    case fmi3_base_type_str:{
        break;
    }
    case fmi3_base_type_enum:{
        fmi3_import_enumeration_typedef_t* et = fmi3_import_get_type_as_enum(vt);
        int min = fmi3_import_get_enum_type_min(et);
        int max = fmi3_import_get_enum_type_max(et);
        printf("Min %d, max %d\n", min, max);
        {
            unsigned ni, i;
            ni = fmi3_import_get_enum_type_size(et);
            printf("There are %d items \n",ni);
            for (i = 1; i <= ni; i++) {
                int val = fmi3_import_get_enum_type_item_value(et, i);
                const char* str = fmi3_import_get_enum_type_value_name(et, val);
                const char* itnm = fmi3_import_get_enum_type_item_name(et, i);
                REQUIRE(strcmp(itnm, str) == 0);
                printf("[%d] %s=%d (%s) \n", i,
                    itnm,
                    val,
                    fmi3_import_get_enum_type_item_description(et, i));
            }
        }
        break;
    }
    default:
        FAIL("Error in fmiGetBaseType()");
    }
}

void testVariableSearch(fmi3_import_t* fmu, fmi3_import_variable_t* v) {
    const char * a_name = fmi3_import_get_variable_name(v);
    fmi3_import_variable_t* found;
    
    found = fmi3_import_get_variable_by_name(fmu, a_name);
    if (!found) {
        FAIL("Search by name failed for variable: " << a_name);
    }
    else if (found != v) {
        FAIL("Searched by name for " << a_name << " found incorrect variable: " << fmi3_import_get_variable_name(found));
    }

    found = fmi3_import_get_variable_by_vr(fmu, fmi3_import_get_variable_vr(v));
    if (!found) {
        FAIL("Search by VR failed for variable: " << a_name);
    }
    else if (fmi3_import_get_variable_base_type(v) != fmi3_import_get_variable_base_type(found)) {
        FAIL("Search by VR for " << a_name << " found var with incorrect base type: " << fmi3_import_get_variable_name(found));
    }
    else if (fmi3_import_get_variable_vr(v) != fmi3_import_get_variable_vr(found)) {
        FAIL("Searching by VR for " << a_name << " found var with incorrect VR: " << fmi3_import_get_variable_name(found));
    }
}

void printVariableInfo(fmi3_import_t* fmu,
                       fmi3_import_variable_t* v) {
    fmi3_base_type_enu_t bt;
    size_t vr = fmi3_import_get_variable_vr(v);
    REQUIRE(vr == (unsigned)vr);
    printf("Variable name: %s\n", fmi3_import_get_variable_name(v));
    printf("Description: %s\n", fmi3_import_get_variable_description(v));
    printf("VR: %u\n", (unsigned)vr);
    printf("Variability: %s\n", fmi3_variability_to_string(fmi3_import_get_variable_variability(v)));
    printf("Causality: %s\n", fmi3_causality_to_string(fmi3_import_get_variable_causality(v)));
    printf("Initial: %s\n", fmi3_initial_to_string(fmi3_import_get_variable_initial(v)));

    bt = fmi3_import_get_variable_base_type(v);
    printf("Base type: %s\n", fmi3_base_type_to_string(bt));

    printTypeInfo(fmi3_import_get_variable_declared_type(v));
    if (bt == fmi3_base_type_float64) {
        fmi3_import_float64_variable_t* rv = fmi3_import_get_variable_as_float64(v);
        fmi3_import_unit_t* u = fmi3_import_get_float64_variable_unit(rv);
        fmi3_import_display_unit_t* du = fmi3_import_get_float64_variable_display_unit(rv);
        printf("Unit: %s, display unit: %s\n", u ? fmi3_import_get_unit_name(u) : 0, du ? fmi3_import_get_display_unit_name(du) : "not provided");
    }

    if (fmi3_import_get_variable_has_start(v)) {
        printf("There is a start value\n");

        switch(fmi3_import_get_variable_base_type(v)) {
        case fmi3_base_type_float64: {
            fmi3_import_float64_variable_t *rv = fmi3_import_get_variable_as_float64(v);
            printf("start =%g\n", fmi3_import_get_float64_variable_start(rv));
            break;
        }
        case fmi3_base_type_int32:{
            printf("start =%d\n", fmi3_import_get_int32_variable_start(fmi3_import_get_variable_as_int32(v)));
            break;
        }
        case fmi3_base_type_bool:{
            printf("start = %d\n", fmi3_import_get_boolean_variable_start(fmi3_import_get_variable_as_boolean(v)));
            break;
        }
        case fmi3_base_type_str:{
            printf("start = '%s'\n", fmi3_import_get_string_variable_start(fmi3_import_get_variable_as_string(v)));
            break;
        }
        case fmi3_base_type_enum:{
            // doesn't work on mingw_64, but there are enums in bouncing ball
            // printf("start = %" PRId64 "\n", fmi3_import_get_enum_variable_start(fmi3_import_get_variable_as_enum(v)));
            break;
        }
        default:
            printf("Error in fmiGetBaseType()\n");
        }
    }
    // Print aliases:
    fmi3_import_alias_variable_list_t* aliases = fmi3_import_get_variable_alias_list(v);
    fmi3_import_alias_variable_t* alias;
    for (size_t i = 0; i < fmi3_import_get_alias_variable_list_size(aliases); i++) {
        if (i == 0) {
            printf("Listing aliases: \n");
        }
        alias = fmi3_import_get_alias(aliases, i);
        printf("\t%s\n", fmi3_import_get_alias_variable_name(alias));
    }
}

void printCapabilitiesInfo(fmi3_import_t* fmu) {
    size_t i;

    for (i = 0; i < fmi3_capabilities_num; ++i) {
        printf("%s = %u\n",
            fmi3_capability_to_string((fmi3_capabilities_enu_t)i),
            fmi3_import_get_capability(fmu, (fmi3_capabilities_enu_t)i));
    }
}

void printDependenciesInfo(fmi3_import_t* fmu, fmi3_import_variable_list_t* rows, fmi3_import_variable_list_t* cols, size_t* start, size_t *dep, char* factor) {
    size_t i, j, nr;
    if (!rows || !cols || !start) {
        printf("Dependencies are not available\n");
        if (rows) {
            nr = fmi3_import_get_variable_list_size(rows);
            for (i = 0; i < nr; i++) {
                printf("\t%s\n",fmi3_import_get_variable_name(fmi3_import_get_variable(rows, i)));
            }
        }
        return;
    }
    nr = fmi3_import_get_variable_list_size(rows);
    for (i = 0; i < nr; i++) {
        if (start[i] == start[i+1]) {
            printf("\t%s has no dependencies\n",fmi3_import_get_variable_name(fmi3_import_get_variable(rows, i)));
        }
        else if ((start[i] + 1 == start[i+1]) && (dep[start[i]] == 0)) {
            printf("\t%s depends on all\n",fmi3_import_get_variable_name(fmi3_import_get_variable(rows, i)));
        }
        else {
            printf("\t%s depends on:\n",fmi3_import_get_variable_name(fmi3_import_get_variable(rows, i)));
            for (j = start[i]; j < start[i+1]; j++) {
                printf("\t\t%s (factor kind: %s)\n",fmi3_import_get_variable_name(fmi3_import_get_variable(cols, dep[j]-1)),
                    fmi3_dependencies_kind_to_string((fmi3_dependencies_kind_enu_t)factor[j]));
            }
        }
    }
}

static void test_parse_bouncingball(const char* xmldir, bool expectParseFailure) {
    clock_t start, stop;
    double t = 0.0;
    jm_callbacks callbacks;
    fmi_import_context_t* context;
    int res = 0;
    size_t n_states;
    size_t n_event_indicators;

    fmi3_import_t* fmu;

    printf("Testing XML in directory: %s\n", xmldir);

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

    /* time the parsing */
    start = clock();
    // TODO: Re-factor this one and use fmi3_testutil_get_num_problems
    // XXX: Requires fmi3_testutil_parse_xml_with_log equivalent with annotations_callbacks support
    fmu = fmi3_import_parse_xml(context, xmldir, &annotation_callbacks);
    stop = clock();
    t = (double)(stop - start) / CLOCKS_PER_SEC;
    printf("Parsing took %g seconds\n", t);

    fmi_import_free_context(context);
    
    if (expectParseFailure) {
        REQUIRE(fmu == nullptr);
        return;
    } else {
        REQUIRE(fmu != nullptr);
    }

    printf("Model name: %s\n", fmi3_import_get_model_name(fmu));
    printf("Model instantiationToken: %s\n", fmi3_import_get_instantiation_token(fmu));
    printf("FMU kind: %s\n", fmi3_fmu_kind_to_string(fmi3_import_get_fmu_kind(fmu)));
    printf("Description: %s\n", fmi3_import_get_description(fmu));
    printf("Author: %s\n", fmi3_import_get_author(fmu));
    printf("FMI Version: %s\n", fmi3_import_get_model_standard_version(fmu));
    printf("Generation tool: %s\n", fmi3_import_get_generation_tool(fmu));
    printf("Generation date and time: %s\n", fmi3_import_get_generation_date_and_time(fmu));
    printf("Version: %s\n", fmi3_import_get_model_version(fmu));
    printf("Naming : %s\n", fmi3_naming_convention_to_string(fmi3_import_get_naming_convention(fmu)));

    if (fmi3_import_get_fmu_kind(fmu) == fmi3_fmu_kind_me)
        printf("Model identifier ME: %s\n", fmi3_import_get_model_identifier_ME(fmu));
    if (fmi3_import_get_fmu_kind(fmu) == fmi3_fmu_kind_cs)
        printf("Model identifier CS: %s\n", fmi3_import_get_model_identifier_CS(fmu));
    if (fmi3_import_get_fmu_kind(fmu) == fmi3_fmu_kind_se)
        printf("Model identifier SE: %s\n", fmi3_import_get_model_identifier_SE(fmu));
    printCapabilitiesInfo(fmu);

    fmi3_import_get_number_of_continuous_states(fmu, &n_states);
    fmi3_import_get_number_of_event_indicators(fmu, &n_event_indicators);
    printf("NumberOfContinuousStates = " FMILIB_SIZET_FORMAT "\n", n_states);
    printf("NumberOfEventIndicators = " FMILIB_SIZET_FORMAT "\n", n_event_indicators);

    printf("Default experiment start = %g, end = %g, tolerance = %g, step = %g\n",
           fmi3_import_get_default_experiment_start(fmu),
           fmi3_import_get_default_experiment_stop(fmu),
           fmi3_import_get_default_experiment_tolerance(fmu),
           fmi3_import_get_default_experiment_step_size(fmu));

    {
        int n_sources = fmi3_import_get_source_files_me_num(fmu);
        int k;
        printf("There are %d source files for ME\n", n_sources);
        for (k=0; k < n_sources; k++) {
            printf("\t%s\n", fmi3_import_get_source_file_me(fmu, k));
        }
    }
    {
        int n_sources = fmi3_import_get_source_files_cs_num(fmu);
        int k;
        printf("There are %d source files for CS\n", n_sources);
        for (k=0; k < n_sources; k++) {
            printf("\t%s\n", fmi3_import_get_source_file_cs(fmu, k));
        }
    }


    {
        size_t i, nv = fmi3_import_get_vendors_num(fmu);
        printf("There are %u tool annotation records \n", (unsigned)nv);
        for (i = 0; i < nv; i++) {
            printf("Vendor name [%u] %s", (unsigned)i, fmi3_import_get_vendor_name(fmu, i));
        }
    }
    {
        fmi3_import_unit_definition_list_t* ud = fmi3_import_get_unit_definition_list(fmu);
        if (ud) {
            unsigned  i, nu = fmi3_import_get_unit_definition_list_size(ud);
            printf("There are %d different units used \n", nu);

            for (i = 0; i < nu; i++) {
                fmi3_import_unit_t* u = fmi3_import_get_unit(ud, i);
                char buf[1000];
                if (!u) {
                    printf("Error getting unit for index %d (%s)\n", i, fmi3_import_get_last_error(fmu));
                    break;
                }
                fmi3_SI_base_unit_exp_to_string(fmi3_import_get_SI_unit_exponents(u), 1000, buf);
                printf("Unit [%d] is %s, base unit %s, factor %g, offset %g, it has %zu display units\n",
                    i, fmi3_import_get_unit_name(u),
                    buf,
                    fmi3_import_get_SI_unit_factor(u),
                    fmi3_import_get_SI_unit_offset(u),
                    fmi3_import_get_unit_display_unit_list_size(u));
            }
        }
        else
            printf("Error getting unit definitions (%s)\n", fmi3_import_get_last_error(fmu));
    }
    {
        fmi3_import_type_definition_list_t* td = fmi3_import_get_type_definition_list(fmu);
        if (td) {
            {
                unsigned i, ntd = fmi3_import_get_type_definition_list_size(td);
                printf("There are %d defs\n", ntd);
                for (i = 0; i < ntd; i++) {
                    fmi3_import_variable_typedef_t* vt = fmi3_import_get_typedef(td, i);
                    if (!vt) {
                        printf("Error getting vartype for index %d (%s)\n", i, fmi3_import_get_last_error(fmu));
                        break;
                    }
                    printTypeInfo(vt);
                }
            }
        }
        else
            printf("Error getting type definitions (%s)\n", fmi3_import_get_last_error(fmu));
    }
    {
        size_t nv, i;
        fmi3_import_variable_list_t* vl = fmi3_import_get_variable_list(fmu, 0);
        fmi3_import_variable_list_t* ders = fmi3_import_get_continuous_state_derivatives_list(fmu);
        const fmi3_value_reference_t* vrl = fmi3_import_get_value_reference_list(vl);

        REQUIRE(vl);

        nv = fmi3_import_get_variable_list_size(vl);
        printf("There are %u variables in total \n",(unsigned)nv);
        for (i = 0; i < nv; i++) {
            fmi3_import_variable_t* var = fmi3_import_get_variable(vl, i);
            REQUIRE(vrl[i] == fmi3_import_get_variable_vr(var));
            REQUIRE(var != nullptr);
            printVariableInfo(fmu, var);
            testVariableSearch(fmu, var);
        }
        fmi3_import_free_variable_list(vl);
        fmi3_import_free_variable_list(ders);
    }

    fmi3_import_free(fmu);
}

TEST_CASE("Test exercising the XML API on BouncingBall") {
    SECTION("ME") {
        test_parse_bouncingball(TEST_OUTPUT_FOLDER "/BouncingBall3_me", false);
    }
    SECTION("CS") {
        test_parse_bouncingball(TEST_OUTPUT_FOLDER "/BouncingBall3_cs", false);
    }
    SECTION("Malformed XML") {
        test_parse_bouncingball(TEST_OUTPUT_FOLDER "/BouncingBall3_malformed_mf", true);
    }
    SECTION("Brief (a close to minimal XML - not BouncingBall really...)") {
        test_parse_bouncingball(FMIL_TEST_DIR "/FMI3/fmu_dummy", false);
    }
}
