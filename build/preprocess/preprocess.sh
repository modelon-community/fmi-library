#!/bin/bash

# --- Script Description ---
#
# This script runs the C preprocessor and makes the output readable.
# This is done for each file in the ./input directory.
#

set -eu -o pipefail

# --- Helper Functions ---
echoerr() {
    >&2 echo "$0: ERROR: $@"
}

# --- Input Verification ---
n_args_exp=2
if [[ $# -ne $n_args_exp ]]; then
    echoerr "invalid number of args: $#, exp: $n_args_exp"
    exit 1
fi

src="$1"
dst="$2"
if [[ ! -f $src ]]; then
    echoerr "src doesn't exist: '$src'"
    exit 1
fi

# --- Script Start ---

mkdir -p "$(dirname "$dst")"

# create temp-file for modifications before sending it to cpp
# (we could pipe as well, but I think this is clearer)
dst_tmp="$dst.tmp" 
cp "$src" "$dst_tmp"

# disable '#include' tokens so they don't get expanded
sed -i 's@^#include@__INCLUDE__@g' "$dst_tmp"

# add newline tokens that later will be replaced with real newlines
sed -i 's@\\@__NEWLINE__ \\@g' "$dst_tmp"

# run cpp, and create dst file, which we will from now on modify
cpp -E -P "$dst_tmp" -o "$dst"

# add real newlines
sed -i 's/__NEWLINE__/\n/g' "$dst"

# add real comments
sed -i 's@__COMMENT_START__@/**\n *@g' "$dst"
sed -i 's@__COMMENT_END__@\n*/@g' "$dst"

# add back the '#include' tokens
sed -i 's@^__INCLUDE__@#include@g' "$dst"

# indent all lines (vim ex mode)
ex -s -c 'norm gg=G' -c wq "$dst"

# clean up
rm "$dst_tmp"
