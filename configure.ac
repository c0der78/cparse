#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([cparse], [0.1], [c0der78@gmail.com])
AC_CONFIG_SRCDIR([src/parse.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE

LT_INIT

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([json-c], [json_object_get], [], AC_CHECK_LIB(json, [json_object_get], [], AC_MSG_ERROR([json library is required!])))

AC_CHECK_LIB([json-c], [json_tokener_error_desc], [], 
	AC_CHECK_LIB([json], [json_tokener_error_desc]))

AC_CHECK_LIB([curl], [curl_easy_init], [], AC_MSG_ERROR([libcurl library is required!]))

if test x"$ac_cv_lib_json_c_json_tokener_error_desc" = x"yes" || x"$ac_cv_lib_json_json_tokener_error_desc" = x"yes"; then
	AC_DEFINE(HAVE_JSON_TOKENER_ERROR_DESC, 1, [function for json error handling])
fi

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h json-c/json.h json/json.h json.h curl/curl.h curl.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strdup])

AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile])
AC_OUTPUT
