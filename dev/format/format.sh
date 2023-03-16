#!/bin/bash

# This script converts all tabs to spaces in source files, removes all trailing
# white-space, and changes file ending to CRLF.

set -eu -o pipefail

# --- LOCAL_FUNCTIONS ---------------------------------------------- #

#  echoes all arguments to stderr
err() {
    >&2 echo "$0: ERROR: $@"
}

filter_files() {
    echo "$@" | grep -vP "(?:^\.\/\.|^\.\/build_|^\.\/install_|Makefile$|^\.\/ThirdParty|\.md$)"
}

format_files() {
    local eol="$1" # currently all calls use CRLF, but might be changed for .sh and Makefiles
    shift
    local files="$@"
    local ext_bak=.bak_whitespace

    # filter
    files="$(filter_files "$files")"

    # format
    perl -p -i"$ext_bak" -e 's@\t@    @g'              $files # tabs to spaces
    perl -p -i"$ext_bak" -e 's@ +([\r\n])@\1@g'        $files # trailing whitespace (meta char '$' didn't work)
    perl -p -i"$ext_bak" -e "s@(?:\\r\\n|\\n)@$eol@g"  $files # set consistent CRLF / LF

    # clean up
    find . -type f -iname "*$ext_bak" | xargs rm
}

# --- INPUT_VALIDATION ---------------------------------------------- #
n_args_exp=0

if [[ $# -ne $n_args_exp ]]; then
    err "invalid number of input args, exp: $n_args_exp, act: $#"
    exit 1
fi

# --- SCRIPT_START -------------------------------------------------- #

CRLF="\r\n"

# cd to repo's root dir
dir_abs="$(dirname "$(realpath "$0")")"
cd "$dir_abs/../.."

# CMake files
files="$(find . -type f \( -iname "CMakeLists.txt" -or -iname "*.cmake" \) )"
format_files "$CRLF" "$files"

# 'Test' dir
files="$(find ./Test -type f | grep -P '\.[ch]$')"
format_files "$CRLF" "$files"

# 'src' dir
files="$(find ./src -type f | grep -P '\.[ch]$' | grep -v "\/src-gen\/")"
format_files "$CRLF" "$files"
