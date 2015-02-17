
AM_CFLAGS = -Wall -Wwerror

SUBDIRS = src tests

ACLOCAL_AMFLAGS = -I m4

include $(top_srcdir)/Makefile.am.coverage

test: check

memcheck: tests/test_cparse
	CK_FORK=no libtool --mode=execute valgrind --leak-check=full tests/test_cparse

docs:
	doxygen Doxyfile
