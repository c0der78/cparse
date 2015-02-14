
AM_CFLAGS = -Wall -Wwerror

SUBDIRS = src tests

memcheck: tests/test_cparse
	libtool --mode=execute CK_FORK=no valgrind tests/test_cparse