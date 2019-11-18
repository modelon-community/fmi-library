#!/bin/bash

# This script verifies that the modelDescriptions in the subdirectories
# (recursively) follow basic XML syntax. No scheme validation is
# performed.
# This script is currently not called by any other process. You can use
# it during development when you change the XMLs though.

find . -type f -iname "modelDescription*.xml" \
    | grep --invert-match "malformed" \
    | xargs xmllint "--format" > /dev/null

if [[ "$?" == "0" ]]; then
    echo All XMLs OK
fi

