#!/bin/bash

# This script can be used to build some pre-defined targets. Run it without
# any arguments to see which that are available.
# This script must be run from the FMIL repo's root directory.

set -o errexit
set -o pipefail
set -o nounset
# set -o xtrace

fmil_log() {
    log_prefix="FMIL_BUILD"
    echo "${log_prefix}: $1"
}

print_usage() {
    echo "${0}:"
    echo 'Usage:'
    printf '\targ1:\t\ttarget {win64, win64_static_runtime, linux64}\n'
    printf '\targ2-...:\tmake_targets [test, install]\n'
}

cmd_install_win64() {
    cmake --build . --config MinSizeRel --target 'install'
}

cmd_install_linux64() {
    make install
}

cmd_test() {
    ctest -C MinSizeRel
}

cmd_archive_test_logs() {
    # Create create test-log directory
    testLogDir="${repoRoot}/testlogs-${target}"
    if [[ -e "$testLogDir" ]]; then
        rm -rf "$testLogDir"
    fi
    mkdir "$testLogDir"
    cp "${repoRoot}/build-${target}"/Testing/Temporary/* "$testLogDir/"
}

# Validate args
if [[ $# -lt 2 ]]; then
    print_usage
    exit 1
fi

# Save repoRoot path
repoRoot=$(pwd)

# Platform specific setup
# Currently trying to keep platform specific settings at one place
target=$1
install_prefix="../install-${target}"
build="build-${target}"
src=".."
if [[ $target == "win64" ]]; then
    cmd_gen() {
        cmake -DFMILIB_INSTALL_PREFIX="${install_prefix}" \
              -DFMILIB_GENERATE_DOXYGEN_DOC=0 \
              -G "Visual Studio 10" \
              "$src"
    }
    cmd_install() { cmd_install_win64; }
elif [[ $target == "win64_static_runtime" ]]; then
    cmd_gen() {
        cmake -DFMILIB_INSTALL_PREFIX="${install_prefix}" \
              -DFMILIB_GENERATE_DOXYGEN_DOC=0 \
              -DFMILIB_BUILD_WITH_STATIC_RTLIB=1 \
              -G "Visual Studio 10" \
              "$src"
    }
    cmd_install() { cmd_install_win64; }
elif [[ $target == "linux64" ]]; then
    cmd_gen() {
        cmake -DFMILIB_INSTALL_PREFIX="${install_prefix}" \
              -DFMILIB_GENERATE_DOXYGEN_DOC=0 \
              "$src"
    }
    cmd_install() { cmd_install_linux64; }
else
    print_usage
    exit 1
fi

# Create build directory
if [[ ! -e $build ]]; then
    mkdir $build
fi
cd $build

# Generate
cmd_gen
if [[ $? -ne 0 ]]; then
    fmil_log "Generate makefiles/project step failed. Exiting."
    exit 1
fi

for make_target in ${@:2}; do
    if [[ "$make_target" == install ]]; then
        cmd_install
    elif [[ "$make_target" == test ]]; then
        cmd_test
        cmd_archive_test_logs
    else
        fmil_log "Error: Invalid make target '"$make_target"'"
        exit 1
    fi
done

fmil_log "Build successful!"
