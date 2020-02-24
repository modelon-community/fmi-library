#!/bin/bash
set -eu -o pipefail

# This script runs the C preprocessor and makes the output readable.
# This is done for each file in the ./input directory.

for f in $(ls input | grep -v .gitignore); do
    # run cpp
    cpp -E -P "input/$f" -o "output/$f"

    # add real newlines
    sed -i 's/__NEWLINE__/\n/g' "output/$f"

    # add real comments
    sed -i 's@__COMMENT_START__@/**\n *@g' "output/$f"
    sed -i 's@__COMMENT_END__@\n*/@g' "output/$f"

    # indent all lines (vim ex mode)
    ex -s -c 'norm gg=G' -c wq "output/$f"
done

