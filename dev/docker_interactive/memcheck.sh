#!/bin/bash

# -- DESCRIPTION --
# This script synchs the source directory from the host with the container,
# and then rebuilds and runs the test again, then displays the valgrind output.
#
# -- PURPOSE --
# Allow develop on the host and test the changes in an interactive docker shell.


set -eu -o pipefail

# --- LOCAL_FUNCTIONS ---------------------------------------------- #

#  echoes all arguments to stderr
echoerr() {
    >&2 echo "$0: ERROR: $@"
}

# --- INPUT_VALIDATION ---------------------------------------------- #
n_args_exp=2

if [[ $# -ne $n_args_exp ]]; then
    echoerr "invalid number of input args, exp: $n_args_exp, act: $#"
    echoerr "arg1: test name as used for build target"
    echoerr "arg2: ctest index"
    exit 1
fi

test_name="$1"
test_idx="$2"

# --- SCRIPT_START -------------------------------------------------- #

# synch changes from host
cp -r -T /artifacts/src /fmi-library/src
cp -r -T /artifacts/Test /fmi-library/Test

# build the test
cmake --build . --config Debug --clean-first --target "$test_name"

# run the test with valgrind
ctest -C Debug -R "$test_name" -T memcheck

# examine valgrind result
vim Testing/Temporary/MemoryChecker.$test_idx.log

