#!/bin/bash

# This script is only called within the docker container.
# This script wraps the top-level 'make' command, and then moves the build
# results (artifact files) to the Unix host, and ensures that the host's user
# has access to them.

set -o errexit
set -o nounset

TARGET=$1
BUILD_DIR=$2
INSTALL_DIR=$3
ARTIFACT_DIR=$4
CONFIG=$5

TEST_LOG_DIR=$BUILD_DIR/Testing/Temporary

# Call fmi-library/Makefile
if [[ $TARGET == 'test' ]]; then
    # Allow the tests to fail, so we can get the test log
    make $TARGET CONFIG_FILE=./build/config/$CONFIG || true
else
    make $TARGET CONFIG_FILE=./build/config/$CONFIG
fi

# Copy build artifacts to host
if [[ $TARGET == 'install' ]] || [[ $TARGET == 'documentation' ]]; then
    cp -r $INSTALL_DIR $ARTIFACT_DIR
    # Maybe common Unix knowledge, but make to set permissions for the
    # directory on host directory, and not on docker directory. Or use 'cp -p'
    # when copying, since permissions aren't copied otherwise.
    chmod -R a+rwx $ARTIFACT_DIR/$INSTALL_DIR
fi

# Copy test logs to host
if [[ $TARGET == 'test' ]]; then
    ARTIFACT_TEST_LOG_DIR=$ARTIFACT_DIR/$TEST_LOG_DIR
    mkdir -p $ARTIFACT_TEST_LOG_DIR || true
    rm -rf $ARTIFACT_TEST_LOG_DIR/*
    cp $TEST_LOG_DIR/* $ARTIFACT_TEST_LOG_DIR
    chmod -R a+rwx $ARTIFACT_DIR/$BUILD_DIR
fi

