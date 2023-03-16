#!/bin/bash

# --- Script Description ---
#
# This script wraps the preprocess.sh script, and allows a list of tuples
# as input arguments:
#   <script> [src1 dst1 [src2 dst2] ...]
#

set -eu -o pipefail

# --- Helper Functions ---
echoerr() {
    >&2 echo "$0: ERROR: $@"
}

# --- Input Verification ---
if [[ $(( $# % 2 )) -ne 0 ]]; then
    echoerr "invalid number of args: $#, expected even number"
    exit 1
fi

# --- Script Start ---
for (( i=1; i<=$#; i+=2 )); do
    dst_idx=$(( $i + 1 ))
    src=${!i}
    dst=${!dst_idx}
    $(dirname $0)/preprocess.sh "$src" "$dst"
done

