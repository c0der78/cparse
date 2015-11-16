
AM_CFLAGS = -Wall -Wwerror

SUBDIRS = src tests

ACLOCAL_AMFLAGS = -I m4

include $(top_srcdir)/Makefile.am.coverage

test: check
	tests/test_cparse

memcheck: check
	CK_FORK=no $(top_srcdir)/libtool --mode=execute valgrind --leak-check=yes --show-reachable=yes --suppressions=suppression.map --error-exitcode=5 --track-origins=yes tests/test_cparse

htmldir:
	@if [ -d html ]; then\
		rm -rf html/*;\
	else\
		mkdir html;\
	fi

doxygen: htmldir
	doxygen Doxyfile

headerdoc: htmldir
	headerdoc2html -o html src/cparse
	gatherheaderdoc html
	@mv html/masterTOC.html html/index.html
