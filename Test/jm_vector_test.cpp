/*
 * Dummy tests currently only used to verify that the integration of Catch2
 * works as intended.
 * Running the generated executable should result in output similar to below.
 * Example output when running ./jm_vector_test.cpp:
 *  Total Test time (real) =   1.73 sec
 *  ===============================================================================
 *  All tests passed (2 assertions in 2 test cases)
 *
 */

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include "config_test.h"
#include <JM/jm_vector.h>
#include <JM/jm_stack.h>
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

std::string hello() {
    return "hello";
}

int return_five() {
    return 5;
}

SCENARIO( "Check that hello returns 'hello'", "[jm_vector_test.cc]")
{
    REQUIRE( hello() == "hello" );
}

SCENARIO( "Check that return_five returns the integer 5", "[jm_vector_test.cc]")
{
    REQUIRE( return_five() == 5 );
}


SCENARIO( "Check return jm_vector_init", "[jm_vector_test.cc]")
{
    int size_check = 0;
    jm_vector(int) stackv;
    jm_vector(int)* v = &stackv;
    size_check = jm_vector_init(int)(v, 2, 0);
    REQUIRE( size_check == 2 );
}