
AM_CFLAGS = -Wall -Wwerror

SUBDIRS = src tests

ACLOCAL_AMFLAGS = -I m4

include $(top_srcdir)/Makefile.am.coverage

test: check

memcheck: tests/test_cparse
	libtool --mode=execute CK_FORK=no valgrind tests/test_cparse