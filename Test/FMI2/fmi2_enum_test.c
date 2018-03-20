#include "fmilib.h"
#include "fmil_test.h"
#include "config_test.h"

int test_default_valid_variablity()
{
    fmi2_causality_enu_t c;
    fmi2_variability_enu_t v;

    v = fmi2_get_default_valid_variability(fmi2_causality_enu_parameter);
    ASSERT_MSG(v == fmi2_variability_enu_fixed,
               "Default parameter variability should be fixed");

    v = fmi2_get_default_valid_variability(fmi2_causality_enu_calculated_parameter);
    ASSERT_MSG(v == fmi2_variability_enu_fixed,
               "Default calc param variability should be fixed");

    for (c = fmi2_causality_enu_input; c <= fmi2_causality_enu_unknown; c++) {
        v = fmi2_get_default_valid_variability(c);
        ASSERT_MSG(v == fmi2_variability_enu_continuous,
                "Default variability for non-constants should be continuous");
    }

    return TEST_OK;
}

#define N_VARIABILITIES (6)

static int validate_initial(fmi2_causality_enu_t c,
                            fmi2_initial_enu_t expected[])
{
    size_t i;
    fmi2_variability_enu_t v = fmi2_variability_enu_constant;
    for (i = 0; i < N_VARIABILITIES; i++, v++) {
        fmi2_initial_enu_t initial = fmi2_get_default_initial(v, c);
        ASSERT_MSG(initial == expected[i], fmi2_variability_to_string(v));
    }

    return TEST_OK;
}

int test_default_initial_param()
{
    fmi2_initial_enu_t expected[N_VARIABILITIES] = {
        fmi2_initial_enu_unknown,
        fmi2_initial_enu_exact,
        fmi2_initial_enu_exact,
        fmi2_initial_enu_unknown,
        fmi2_initial_enu_unknown,
        fmi2_initial_enu_unknown
    };

    return validate_initial(fmi2_causality_enu_parameter, expected);
}

int test_default_initial_calc_param()
{
    fmi2_initial_enu_t expected[N_VARIABILITIES] = {
        fmi2_initial_enu_unknown,
        fmi2_initial_enu_calculated,
        fmi2_initial_enu_calculated,
        fmi2_initial_enu_unknown,
        fmi2_initial_enu_unknown,
        fmi2_initial_enu_unknown
    };

    return validate_initial(fmi2_causality_enu_calculated_parameter, expected);
}

int test_default_initial_input()
{
    fmi2_initial_enu_t expected[N_VARIABILITIES] = {
        fmi2_initial_enu_unknown,
        fmi2_initial_enu_unknown,
        fmi2_initial_enu_unknown,
        fmi2_initial_enu_unknown,
        fmi2_initial_enu_unknown,
        fmi2_initial_enu_unknown
    };

    return validate_initial(fmi2_causality_enu_input, expected);
}

int test_default_initial_output()
{
    fmi2_initial_enu_t expected[N_VARIABILITIES] = {
        fmi2_initial_enu_exact,
        fmi2_initial_enu_unknown,
        fmi2_initial_enu_unknown,
        fmi2_initial_enu_calculated,
        fmi2_initial_enu_calculated,
        fmi2_initial_enu_unknown,
    };

    return validate_initial(fmi2_causality_enu_output, expected);
}

int test_default_initial_local()
{
    fmi2_initial_enu_t expected[N_VARIABILITIES] = {
        fmi2_initial_enu_exact,
        fmi2_initial_enu_calculated,
        fmi2_initial_enu_calculated,
        fmi2_initial_enu_calculated,
        fmi2_initial_enu_calculated,
        fmi2_initial_enu_unknown,
    };

    return validate_initial(fmi2_causality_enu_local, expected);
}

int test_default_initial_independent()
{
    fmi2_initial_enu_t expected[N_VARIABILITIES] = {
        fmi2_initial_enu_unknown,
        fmi2_initial_enu_unknown,
        fmi2_initial_enu_unknown,
        fmi2_initial_enu_unknown,
        fmi2_initial_enu_unknown,
        fmi2_initial_enu_unknown
    };

    return validate_initial(fmi2_causality_enu_independent, expected);
}

int test_default_initial_unknown()
{
    fmi2_initial_enu_t expected[N_VARIABILITIES] = {
        fmi2_initial_enu_unknown,
        fmi2_initial_enu_unknown,
        fmi2_initial_enu_unknown,
        fmi2_initial_enu_unknown,
        fmi2_initial_enu_unknown,
        fmi2_initial_enu_unknown
    };

    return validate_initial(fmi2_causality_enu_unknown, expected);
}

int validate_variability_causality(fmi2_causality_enu_t c, int expected[])
{
    fmi2_variability_enu_t v = fmi2_variability_enu_constant;
    int i;

    for (i = 0; i < N_VARIABILITIES; i++, v++) {
        int is_valid = fmi2_is_valid_variability_causality(v, c);
        ASSERT_MSG(is_valid == expected[i], fmi2_variability_to_string(v));
    }

    return TEST_OK;
}

int test_valid_combination_param()
{
    int expected[N_VARIABILITIES] = {0, 1, 1, 0, 0, 0};
    return validate_variability_causality(fmi2_causality_enu_parameter, expected);
}

int test_valid_combination_calc_param()
{
    int expected[N_VARIABILITIES] = {0, 1, 1, 0, 0, 0};
    return validate_variability_causality(fmi2_causality_enu_calculated_parameter,
                                          expected);
}

int test_valid_combination_input()
{
    int expected[N_VARIABILITIES] = {0, 0, 0, 1, 1, 0};
    return validate_variability_causality(fmi2_causality_enu_input, expected);
}

int test_valid_combination_output()
{
    int expected[N_VARIABILITIES] = {1, 0, 0, 1, 1, 0};
    return validate_variability_causality(fmi2_causality_enu_output, expected);
}

int test_valid_combination_local()
{
    int expected[N_VARIABILITIES] = {1, 1, 1, 1, 1, 0};
    return validate_variability_causality(fmi2_causality_enu_local, expected);
}

int test_valid_combination_independent()
{
    int expected[N_VARIABILITIES] = {0, 0, 0, 0, 1, 0};
    return validate_variability_causality(fmi2_causality_enu_independent,
                                          expected);
}

int test_valid_combination_unknown()
{
    int expected[N_VARIABILITIES] = {0, 0, 0, 0, 0, 0};
    return validate_variability_causality(fmi2_causality_enu_unknown, expected);
}

int main(void)
{
    int ret = TEST_OK;

    ret &= test_default_valid_variablity();
    ret &= test_default_initial_param();
    ret &= test_default_initial_calc_param();
    ret &= test_default_initial_input();
    ret &= test_default_initial_output();
    ret &= test_default_initial_local();
    ret &= test_default_initial_independent();
    ret &= test_default_initial_unknown();
    ret &= test_valid_combination_param();
    ret &= test_valid_combination_calc_param();
    ret &= test_valid_combination_input();
    ret &= test_valid_combination_output();
    ret &= test_valid_combination_local();
    ret &= test_valid_combination_independent();
    ret &= test_valid_combination_unknown();


    return ret == TEST_OK ? CTEST_RETURN_SUCCESS : CTEST_RETURN_FAIL;
}
