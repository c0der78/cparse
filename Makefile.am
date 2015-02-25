
AM_CFLAGS = -Wall -Wwerror

SUBDIRS = src tests

ACLOCAL_AMFLAGS = -I m4

include $(top_srcdir)/Makefile.am.coverage

test: check
	tests/test_cparse

memcheck: check
	CK_FORK=no glibtool --mode=execute valgrind --leak-check=yes --error-exitcode=5 tests/test_cparse

docs:
	doxygen Doxyfile
