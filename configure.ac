dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])

AC_INIT([swath],
        [m4_esyscmd(build-aux/git-version-gen)],
        [https://github.com/tlwg/swath/issues])
AC_CONFIG_SRCDIR([src/wordseg.cpp])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_LIBOBJ_DIR([lib])

AM_INIT_AUTOMAKE(dist-xz no-dist-gzip)

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
LT_INIT

dnl Checks for libraries.
PKG_CHECK_MODULES(DATRIE,datrie-0.2)

dnl Checks for header files.
AC_CHECK_HEADERS(limits.h)

dnl Checks for functions

dnl NetBSD is reported to lack wcpcpy()
AC_REPLACE_FUNCS([wcpcpy])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

dnl Check commandline options
AC_ARG_ENABLE(debug,
              [AS_HELP_STRING([--enable-debug],
                              [enable assertion checks])],
              , enable_debug="no")
if test "x$enable_debug" = "xno"; then
  CPPFLAGS="$CPPFLAGS -DNDEBUG"
fi

AC_ARG_ENABLE(catthai,
              [AS_HELP_STRING([--enable-catthai],
                              [enable catenation of Thai lines])],
              , enable_catthai="no")
if test "x$enable_catthai" = "xyes"; then
  CPPFLAGS="$CPPFLAGS -DCAT_THAI_LINES"
fi

dnl dictionary data generation
AC_ARG_ENABLE(dict,
              [AS_HELP_STRING([--disable-dict],
                              [disable dictionary data generation])],
              , enable_dict="yes")

if test "x$enable_dict" = "xyes"; then
  AC_CHECK_PROGS(TRIETOOL,[trietool-0.2 trietool],no)
  if test "x$TRIETOOL" = "xno"; then
    AC_MSG_ERROR([You need trietool[[-0.2]] (from libdatrie package) to generate dict, or just use --disable-dict to skip])
  fi
fi

AM_CONDITIONAL(ENABLE_DICT,test "x$enable_dict" = "xyes")

AC_CONFIG_FILES([Makefile
  lib/Makefile
  conv/Makefile
  src/Makefile
  data/Makefile
  tests/Makefile])
AC_OUTPUT
