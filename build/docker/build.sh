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

# Copy artifacts to host
if [[ $TARGET == 'install' ]] || [[ $TARGET == 'documentation' ]]; then
    chmod -R a+rwx $INSTALL_DIR
    ls -la $ARTIFACT_DIR
    grep "[[:space:]]ro[[:space:],]" /proc/mounts 
    cp -r $INSTALL_DIR $ARTIFACT_DIR
fi

if [[ $TARGET == 'test' ]]; then
    ARTIFACT_TEST_LOG_DIR=$ARTIFACT_DIR/$TEST_LOG_DIR
    mkdir -p $ARTIFACT_TEST_LOG_DIR || true
    rm -rf $ARTIFACT_TEST_LOG_DIR/*
    chmod -R a+rwx $TEST_LOG_DIR
    cp $TEST_LOG_DIR/* $ARTIFACT_TEST_LOG_DIR
fi

