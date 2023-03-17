#!/bin/bash
#
# This script is only called within the docker container.
# This script wraps the top-level 'make' command, and ensures that the host
# user can access the build artifacts.
#
# This script must be called from the top-level of the repo.

set -eu -o pipefail

TARGET=$1
BUILD_DIR=$2
INSTALL_DIR=$3
CONFIG=$4

# Call the top-level Makefile
if [[ $TARGET == 'test' ]]; then
    # Allow the tests to fail, so we can get the test log
    make $TARGET CONFIG_FILE=./build/config/$CONFIG || true
else
    make $TARGET CONFIG_FILE=./build/config/$CONFIG
fi

# Build dir needs chmod since it contains the test log
chmod -R a+rwx $INSTALL_DIR $BUILD_DIR
