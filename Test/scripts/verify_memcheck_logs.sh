#!/bin/bash

# This script checks if memcheck generated any unexpected warnings/errors.
# - Run this script after running memcheck.
# - Read the result file created in the same directory as the memcheck logs
#   to determine success/failure.

set -eu -o pipefail

# --- LOCAL_FUNCTIONS ---------------------------------------------- #

#  echoes all arguments to stderr
err() {
    >&2 echo "$0: ERROR: $@"
}

# --- INPUT_VALIDATION ---------------------------------------------- #
n_args_exp=1

if [[ $# -ne $n_args_exp ]]; then
    err "invalid number of input args, exp: $n_args_exp, act: $#"
    err "arg1: path to the directory containing memcheck logs"
    exit 1
fi

logdir="$1"

# --- SCRIPT_START -------------------------------------------------- #

# Find the number of log files that contain warnings or errors after running
# memcheck.
memcheck_logs="$(find "$logdir" -not -empty -name "MemoryChecker.*.log")"

if [[ -n "$memcheck_logs" ]]; then
    err "memcheck gave warnings on tests: $memcheck_logs"
    exit 1
fi

echo "$0: memcheck OK"
