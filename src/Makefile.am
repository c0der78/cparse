
lib_LTLIBRARIES = libcparse.la

libcparse_la_LDFLAGS = $(LIBCPARSE_LA_LDFLAGS) -version-info 0:0:0

subdirheadersdir = $(pkgincludedir)/cparse

subdirheaders_HEADERS = cparse/defines.h cparse/error.h cparse/json.h cparse/object.h cparse/operator.h cparse/parse.h cparse/query.h cparse/types.h cparse/user.h cparse/util.h

libcparse_la_SOURCES = client.c error.c json.c object.c parse.c query.c types.c user.c util.c operators.c log.c

libcparse_la_CFLAGS = $(LIBCPARSE_LA_CFLAGS) @X_CFLAGS@ @COVERAGE_CFLAGS@

CLEANFILES = *.gcno
