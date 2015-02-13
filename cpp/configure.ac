#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libcparse], [0.1], [c0der78@gmail.com])
AC_CONFIG_SRCDIR([src/array.cpp])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE

LT_INIT

# Checks for programs.
AC_PROG_CXX([clang++ g++])
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([curl], [curl_easy_init])
AC_CHECK_LIB([arg3json], [main])

# Checks for header files.
AC_CHECK_HEADERS([curl/curl.h arg3json/json.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MKTIME

AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile])
AC_OUTPUT
