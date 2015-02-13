lib_LTLIBRARIES = libcparse.la

libcparse_la_LDFLAGS = $(LIBCPARSE_LA_LDFLAGS) -version-info 0:0:0

subdirheadersdir = $(pkgincludedir)/cparse

opheadersdir = $(subdirheadersdir)/op

typeheadersdir = $(subdirheadersdir)/type

subdirheaders_HEADERS = cparse/clientinterface.h cparse/exception.h cparse/json.h cparse/object.h cparse/parse.h cparse/user.h

opheaders_HEADERS = cparse/op/array.h cparse/op/decrement.h cparse/op/increment.h

typeheaders_HEADERS = cparse/type/bytes.h cparse/type/date.h cparse/type/file.h cparse/type/geopoint.h cparse/type/parsetype.h cparse/type/pointer.h

libcparse_la_SOURCES = array.cpp bytes.cpp client.cpp date.cpp decrement.cpp file.cpp geopoint.cpp increment.cpp object.cpp parse.cpp pointer.cpp protocol.cpp user.cpp

libcparse_la_CPPFLAGS = $(LIBCPARSE_LA_CPPFLAGS) -std=c++11 -stdlib=libc++ $(COVERAGE_CFLAGS)

