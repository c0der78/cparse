
include_directories(.)

set(HEADER_FILES cparse/clientinterface.h cparse/exception.h cparse/json.h cparse/object.h cparse/parse.h cparse/user.h cparse/op/array.h cparse/op/decrement.h cparse/op/increment.h cparse/type/bytes.h cparse/type/date.h cparse/type/file.h cparse/type/geopoint.h cparse/type/parsetype.h cparse/type/pointer.h)

add_library(cparse array.cpp bytes.cpp client.cpp client.h date.cpp decrement.cpp file.cpp geopoint.cpp increment.cpp object.cpp parse.cpp pointer.cpp protocol.cpp protocol.h user.cpp ${HEADER_FILES})

