#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([minizip], [1.2.6], [bugzilla.redhat.com])
AC_CONFIG_SRCDIR([minizip.c])
AM_INIT_AUTOMAKE([foreign])
LT_INIT

# Zlib
AC_ARG_WITH([zlib],
   [AS_HELP_STRING([--with-zlib],
     [Zlib installation directory @<:@default=check@:>@])],
   [zlib=$withval],
   [with_zlib=no])

AS_IF([test "x$with_zlib" = xno],
      [zlib="$CURR_DIR/zlib_install"])

AS_IF([test "x$with_zlib" = xno],
      [with_zlib=automatic])

# Set output variables to be exported to the Makefiles
AC_SUBST(ZLIB_HOME)
AC_SUBST(WITH_ZLIB)

AM_CONDITIONAL([WITH_ZLIB],[test "x$with_zlib" != xno])

AC_SUBST(ZLIB_HOME)
ZLIB_HOME=$zlib

AC_MSG_CHECKING([whether to build example programs])
AC_ARG_ENABLE([demos], AC_HELP_STRING([--enable-demos], [build example programs]))
AM_CONDITIONAL([COND_DEMOS], [test "$enable_demos" = yes])
if test "$enable_demos" = yes
then
	AC_MSG_RESULT([yes])
else
	AC_MSG_RESULT([no])
fi

AC_SUBST([HAVE_UNISTD_H], [0])
AC_CHECK_HEADER([unistd.h], [HAVE_UNISTD_H=1], [])
AC_CONFIG_FILES([Makefile minizip.pc])
AC_OUTPUT
