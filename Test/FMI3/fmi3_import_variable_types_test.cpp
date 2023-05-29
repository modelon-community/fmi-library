#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <float.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#include "catch.hpp"

// Tests float32/64 variables for correct handling of precision

/* Parse small Float64 variable */
static void test_small_float64(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "var1");
    REQUIRE(v != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(v) == 1);
    REQUIRE(fmi3_import_get_variable_description(v) == nullptr);
    REQUIRE(fmi3_import_get_causality(v) == fmi3_causality_enu_local);
    REQUIRE(fmi3_import_get_variability(v) == fmi3_variability_enu_continuous);

    fmi3_float64_t min_ref =        -DBL_MAX;
    fmi3_float64_t max_ref =        DBL_MAX;
    fmi3_float64_t nominal_ref =    1.000000000000000;
    fmi3_float64_t start_ref =      3.141592653589793;

    fmi3_import_float64_variable_t* var = fmi3_import_get_variable_as_float64(v);
    REQUIRE(var != nullptr);

    REQUIRE(fmi3_import_get_float64_variable_start(var) == start_ref); /* user defined */
    REQUIRE(fmi3_import_get_float64_variable_min(var) == min_ref); /* default */
    REQUIRE(fmi3_import_get_float64_variable_max(var) == max_ref); /* default */
    REQUIRE(fmi3_import_get_float64_variable_nominal(var) == nominal_ref); /* default */

    /* TODO: add checks for the type */
}

/* Parse small Float32 variable */
static void test_small_float32(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "var2");
    REQUIRE(v != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(v) == 2);
    REQUIRE(fmi3_import_get_variable_description(v) == nullptr);
    REQUIRE(fmi3_import_get_causality(v) == fmi3_causality_enu_local);
    REQUIRE(fmi3_import_get_variability(v) == fmi3_variability_enu_continuous);

    fmi3_float32_t min_ref =        -FLT_MAX;
    fmi3_float32_t max_ref =        FLT_MAX;
    fmi3_float32_t nominal_ref =    1.00000000f;
    fmi3_float32_t start_ref =      3.14159274f;

    fmi3_import_float32_variable_t* var = fmi3_import_get_variable_as_float32(v);
    REQUIRE(var != nullptr);

    REQUIRE(fmi3_import_get_float32_variable_start(var) == start_ref); /* user defined */
    REQUIRE(fmi3_import_get_float32_variable_min(var) == min_ref); /* default */
    REQUIRE(fmi3_import_get_float32_variable_max(var) == max_ref); /* default */
    REQUIRE(fmi3_import_get_float32_variable_nominal(var) == nominal_ref); /* default */
}

/* Parse user defined: min, max, nominal for float64 variable*/
static void test_full_float64(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "var3");
    REQUIRE(v != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(v) == 3);
    
    /* Arbitrarily chosen values that don't get truncated/rounded */
    fmi3_float64_t min_ref =       -3500.0000000000000;
    fmi3_float64_t max_ref =        3343492.7982000001;
    fmi3_float64_t nominal_ref =    7.7776999499999997;

    fmi3_import_float64_variable_t* var = fmi3_import_get_variable_as_float64(v);
    REQUIRE(var != nullptr);

    REQUIRE(fmi3_import_get_float64_variable_min(var) == min_ref); /* user defined */
    REQUIRE(fmi3_import_get_float64_variable_max(var) == max_ref); /* user defined */
    REQUIRE(fmi3_import_get_float64_variable_nominal(var) == nominal_ref); /* user defined */
}

/* Parse user defined: min, max, nominal for float32 variable*/
static void test_full_float_32(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "var4");
    REQUIRE(v != nullptr);
    REQUIRE(fmi3_import_get_variable_vr(v) == 4);

    /* Arbitrarily chosen values that don't get truncated/rounded */
    fmi3_float32_t min_ref =       -3500.00000f;
    fmi3_float32_t max_ref =        3343492.75f;
    fmi3_float32_t nominal_ref =    7.77769995f;

    fmi3_import_float32_variable_t* var = fmi3_import_get_variable_as_float32(v);;
    REQUIRE(var != nullptr);

    REQUIRE(fmi3_import_get_float32_variable_min(var) == min_ref); /* user defined */
    REQUIRE(fmi3_import_get_float32_variable_max(var) == max_ref); /* user defined */
    REQUIRE(fmi3_import_get_float32_variable_nominal(var) == nominal_ref); /* user defined */
}

/* Parse unit and display unit info for float32 variable */

// TODO: copy this over to unit & display unit cpp test?
static void test_float64_unit(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "float64WithUnit");
    REQUIRE(v != nullptr);

    fmi3_import_unit_t* u;
    fmi3_import_display_unit_t* du;
    fmi3_float64_t du_val_orig;
    fmi3_float64_t du_val_exp;
    fmi3_float64_t du_val_conv;
    fmi3_float64_t du_val_reconv;

    /* Arbitrarily chosen values that don't get truncated/rounded */
    fmi3_import_float64_variable_t* var = fmi3_import_get_variable_as_float64(v);
    REQUIRE(var != nullptr);

    /* just a few checks on unit and display unit, full check is done in fmi3_import_xml test (TODO: create bouncing ball example with different numeric types, for fmi3_import_xml test) */
    /* unit */
    u = fmi3_import_get_float64_variable_unit(var);
    REQUIRE(u != nullptr);
    REQUIRE(!strcmp("K", fmi3_import_get_unit_name(u)));

    /* display unit */
    du = fmi3_import_get_float64_variable_display_unit(var);
    REQUIRE(du != nullptr);
    REQUIRE(!strcmp("degC", fmi3_import_get_display_unit_name(du)));

    du_val_orig = 222.22;
    du_val_exp = du_val_orig - 273.15;
    // TODO: Fix
    // du_val_conv = fmi3_import_float64_convert_to_display_unit(du_val_orig, du, 0);
    // REQUIRE(du_val_exp == du_val_conv);

    // du_val_reconv = fmi3_import_float64_convert_from_display_unit(du_val_conv, du, 0);
    // REQUIRE(du_val_orig == du_val_reconv);
}

/* Parse unit and display unit info for float32 variable */
static void test_float32_unit(fmi3_import_t* xml) {
    fmi3_import_variable_t* v = fmi3_import_get_variable_by_name(xml, "float32WithUnit");
    REQUIRE(v != nullptr);

    fmi3_import_unit_t* u;
    fmi3_import_display_unit_t* du;
    fmi3_float32_t du_val_orig;
    fmi3_float32_t du_val_exp;
    fmi3_float32_t du_val_conv;
    fmi3_float32_t du_val_reconv;

    /* Arbitrarily chosen values that don't get truncated/rounded */
    fmi3_import_float32_variable_t* var = fmi3_import_get_variable_as_float32(v);
    REQUIRE(var != nullptr);

    /* just a few checks on unit and display unit, full check is done in fmi3_import_xml test (TODO: create bouncing ball example with different numeric types, for fmi3_import_xml test) */
    /* unit */
    u = fmi3_import_get_float32_variable_unit(var);
    REQUIRE(u != nullptr);
    REQUIRE(!strcmp("K", fmi3_import_get_unit_name(u)));

    du = fmi3_import_get_float32_variable_display_unit(var);
    REQUIRE(du != nullptr); /* display unit */
    REQUIRE(!strcmp("degC", fmi3_import_get_display_unit_name(du)));

    du_val_orig = 222.22f;
    du_val_exp = du_val_orig - 273.15f;
    // TODO: FIX
    // du_val_conv = fmi3_import_float32_convert_to_display_unit(du_val_orig, du, 0);
    // REQUIRE(du_val_exp == du_val_conv);

    // du_val_reconv = fmi3_import_float32_convert_from_display_unit(du_val_conv, du, 0);
    // REQUIRE(du_val_orig == du_val_reconv);
}

TEST_CASE("Varibles types testing") {
    const char* xmldir = FMI3_TEST_XML_DIR "/variable_types/float";

    jm_callbacks* cb = jm_get_default_callbacks();
    fmi_import_context_t* ctx = fmi_import_allocate_context(cb);
    fmi3_import_t* xml = fmi3_import_parse_xml(ctx, xmldir, nullptr);

    REQUIRE(ctx != nullptr);
    REQUIRE(xml != nullptr);

    test_small_float64(xml);
    test_small_float32(xml);

    test_full_float64(xml);
    test_full_float_32(xml);

    test_float64_unit(xml);
    test_float32_unit(xml);

    fmi_import_free_context(ctx);
    fmi3_import_free(xml);
}
