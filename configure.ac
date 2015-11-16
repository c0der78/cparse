#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([cparse], [0.1], [c0der78@gmail.com])
AC_CONFIG_SRCDIR([src/parse.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE

LT_INIT

AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC

AC_SUBST(X_CFLAGS, "-pedantic -Wall -Werror")

# Checks for libraries.
AC_CHECK_LIB([curl], [curl_easy_init], [], AC_MSG_ERROR([libcurl library is required!]))

# Checks for header files.
AC_CHECK_HEADERS([stdbool.h stdlib.h string.h json-c/json.h json/json.h json.h curl/curl.h curl.h pthread.h])

AC_CHECK_FUNC(strptime, AC_DEFINE(HAVE_STRPTIME, 1, [a posix time function]), [])

PKG_CHECK_MODULES(JSONC, [json-c >= 0.10])

m4_include([m4/gcov.m4])
AC_TDD_GCOV
AC_SUBST(COVERAGE_CFLAGS)
AC_SUBST(COVERAGE_CXXFLAGS)
AC_SUBST(COVERAGE_LDFLAGS)

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strdup dladdr])

AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile])
AC_OUTPUT
