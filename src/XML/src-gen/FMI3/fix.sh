#!/bin/bash

# For some reason the forward slashes must be escaped even in metaquoted string...
read -r -d '' pattern << EOM
\\Q#ifndef YY_NO_UNISTD_H\\E.
\\Q\\/* Special case for "unistd.h", since it is non-ANSI. We include it way\\E.
\\Q * down here because we want the user's section 1 to have been scanned first.\\E.
\\Q * The user has a chance to override it with an option.\\E.
\\Q *\\/\\E.
\\Q#include <unistd.h>\\E.
\\Q#endif\\E.
EOM

read -r -d '' substitute << EOM
#ifndef YY_NO_UNISTD_H
\/* Special case for "unistd.h", since it is non-ANSI. We include it way
 * down here because we want the user's section 1 to have been scanned first.
 * The user has a chance to override it with an option.
 *\/
#include <unistd.h>
#else
\/*windows compatibility case*\/
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#endif
EOM


perl -0777 -i.original -pe "s/${pattern}/${substitute}/igs" *.{c,h}
rm ./*.original

#perl -0777 -i.orig -pe "s/${pattern}/${substitute}/igs" testy.txtr && \
#cat testy.txtr

#if [[ -e testy.txtr.orig ]]; then
    #rm testy.txtr && \
    #mv testy.txtr.orig testy.txtr
#fi

