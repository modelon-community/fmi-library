#!/bin/bash

# NOTE: This file must be called from <repo-root>! Otherwise the Docker
# context will be incorrect.

set -o errexit
set -o pipefail
set -o nounset
# set -o xtrace

function print_usage() {
    echo "Usage:"
    echo "This script must be run from <repo-root>"
    printf "\targ1:\t\ttarget {linux64, documentation}\n"
    printf "\targ2...:\tmake targets for linux64 [test, install]\n"
}

function log() {
    echo "${0}: $1"
}

function printerr() {
    echo "${0}: $1" >&2
}

function cmd_linux64() {
    local make_targets=$@
    local cmd=(
        "set -o errexit;" \
        "chmod u+x ./build.sh;" \
        "./build.sh linux64 ${make_targets};" \
    )
    for make_target in "$@"; do
        if [[ "$make_target" == 'install' ]]; then
            cmd+=("cp -r install-linux64 /artifacts;")
        elif [[ "$make_target" == 'test' ]]; then
            cmd+=("cp -r testlogs-linux64 /artifacts;")
        else
            printerr "Internal error: unexpected execution path:$LINENO"
            exit 1
        fi
    done

    echo "${cmd[*]}"
}

function cmd_documentation() {
    local cmd=(
        "set -o errexit;" \
        "cd build-doc;" \
        "make doc;" \
        "cp -r ../install-documentation/doc/ /artifacts" \
    )
    echo "${cmd[*]}"
}

# Validate args
target="$1"
make_targets="${@:2}"

if [[ $# -lt 1 ]] || [[ $# -gt 3 ]]; then
    printerr "Error: invalid amount of args"
    print_usage
    exit 1
elif [[ ! -e $(pwd)/Dockerfiles/run.sh ]]; then
    # Checks that the user runs from the repo root
    printerr "Error: script ran from invalid path"
    print_usage
    exit 1
fi

if [[ "$target" != "linux64" ]] && [[ "$target" != "documentation" ]]; then
    printerr "Error: Invalid target: '${target}'"
    print_usage
    exit 1
fi

for make_target in ${make_targets}; do
    if [[ $make_target != "test" ]] && [[ $make_target != "install" ]]; then
        printerr "Error: Invalid make target: '${make_target}'"
        print_usage
        exit 1
    fi
done


# Actual script begins here:
if [[ $target == "linux64" ]]; then
    # Double slash '//bin/bash' is necessary, because (at least) git-bash
    # expands the path incorrectly otherwise. Same applies for '/$(pwd)'.
    cmd="$(cmd_linux64 $make_targets)"
    echo "Running command in docker: ${cmd}"
    docker run -v "/$(pwd):/artifacts" fmil_linux64:latest \
        "//bin/bash" -c "${cmd}"

    # Uncomment for interactive debugging:
    # winpty docker run -v "/$(pwd):/artifacts" -it fmil_linux64:latest

elif [[ $target == "documentation" ]]; then
    cmd="$(cmd_documentation)"
    echo "Running command in docker: ${cmd}"
    docker run -v "/$(pwd):/artifacts" fmil_documentation:latest \
        "//bin/bash" -c "${cmd}"

    # Uncomment for interactive debugging:
    # winpty docker run -v "/$(pwd):/artifacts" -it fmil_documentation:latest

else
    printerr "Error: Invalid arg1"
    print_usage
    exit 1
fi

log "Docker run successful!" 

