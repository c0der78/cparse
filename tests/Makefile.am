
check_PROGRAMS = test_cparse

test_cparse_SOURCES = cparse.test.c json.test.c object.test.c parse.test.c query.test.c util.test.c user.test.c client.test.c acl.test.c role.test.c

test_cparse_CFLAGS = $(TEST_CPARSE_CFLAGS) -I ../src -DROOT_PATH="\".\"" @X_CFLAGS@ @COVERAGE_CFLAGS@

test_cparse_LDADD = ../src/libcparse.la -lcheck $(LIBS)

check: test_cparse

CLEANFILES = *.gcno
