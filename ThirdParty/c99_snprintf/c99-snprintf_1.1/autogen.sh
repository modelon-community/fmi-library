#!/bin/sh
#
# $Id: autogen.sh,v 1.1.1.1 2008/01/06 03:24:00 holger Exp $

for f in \
    "autoheader261" \
    "autoheader260" \
    "autoheader"
do
	which $f >/dev/null 2>&1
	if [ $? -eq 0 ]; then
		AUTOHEADER="${f}"
		break
	fi
done

for f in \
    "aclocal-1.10" \
    "aclocal110" \
    "aclocal-1.9" \
    "aclocal19" \
    "aclocal"
do
	which $f >/dev/null 2>&1
	if [ $? -eq 0 ]; then
		ACLOCAL="${f} -I m4"
		break
	fi
done

for f in \
    "autoconf261" \
    "autoconf260" \
    "autoconf"
do
	which $f >/dev/null 2>&1
	if [ $? -eq 0 ]; then
		AUTOCONF="${f}"
		break
	fi
done

for f in \
    "automake-1.10" \
    "automake110" \
    "automake-1.9" \
    "automake19" \
    "automake"
do
	which $f >/dev/null 2>&1
	if [ $? -eq 0 ]; then
		AUTOMAKE="${f} -a -c"
		break
	fi
done

if [ -z "${AUTOHEADER}" -o \
     -z "${ACLOCAL}" -o \
     -z "${AUTOCONF}" -o \
     -z "${AUTOMAKE}" ]; then
	echo "$0: Cannot find (all) GNU autotools, aborting." >&2
	exit 1
fi

echo "Running GNU autotools:"
echo "* ${AUTOHEADER}" ; ${AUTOHEADER}
echo "* ${ACLOCAL}" ; ${ACLOCAL}
echo "* ${AUTOCONF}" ; ${AUTOCONF}
echo "* ${AUTOMAKE}" ; ${AUTOMAKE}
