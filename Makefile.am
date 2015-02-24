
AM_CFLAGS = -Wall -Wwerror

SUBDIRS = src tests

ACLOCAL_AMFLAGS = -I m4

include $(top_srcdir)/Makefile.am.coverage

test: check
	tests/test_cparse

memcheck: check
	CK_FORK=no libtool --mode=execute valgrind --leak-check=full --error-exitcode=5 tests/test_cparse

docs:
	doxygen Doxyfile
