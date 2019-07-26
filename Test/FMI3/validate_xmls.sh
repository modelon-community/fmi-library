#!/bin/bash

# This script verifies that the modelDescriptions in the subdirectories
# (recursively) follow basic XML syntax. No scheme validation is
# performed.

find . -type f -iname "modelDescription*.xml" \
    | grep --invert-match "malformed" \
    | xargs xmllint "--format" > /dev/null

if [[ "$?" == "0" ]]; then
    echo All XMLs OK
fi

