#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(storyscript, 1, DanielCJones@gmail.com)
AC_CONFIG_SRCDIR([include/Anomaly.hpp])
AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE

AC_DISABLE_STATIC 

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIB(gmp, __gmpz_init)
AC_CHECK_LIB(mpfr, mpfr_add)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stddef.h wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_FUNC_OBSTACK
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_STRFTIME
AC_CHECK_FUNCS([localeconv sqrt])
AC_OUTPUT(Makefile src/Makefile include/Makefile)

