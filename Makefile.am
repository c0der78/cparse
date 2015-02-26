
AM_CFLAGS = -Wall -Wwerror

SUBDIRS = src tests

ACLOCAL_AMFLAGS = -I m4

include $(top_srcdir)/Makefile.am.coverage

test: check
	tests/test_cparse

memcheck: check
	CK_FORK=no $(top_srcdir)/libtool --mode=execute valgrind --leak-check=yes --show-reachable=yes --suppressions=suppression.map --error-exitcode=5 --track-origins=yes tests/test_cparse

docs:
	doxygen Doxyfile
