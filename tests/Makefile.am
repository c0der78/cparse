
check_PROGRAMS = test_cparse

test_cparse_SOURCES = object.test.cpp parse.test.cpp user.test.cpp

test_cparse_LDADD = ../src/libcparse.la -larg3json -lcurl

check: test_cparse
	./test_cparse

