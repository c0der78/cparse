
include_directories(.)

set(HEADER_FILES cparse/acl.h cparse/defines.h cparse/error.h cparse/json.h cparse/object.h cparse/operator.h cparse/parse.h cparse/query.h cparse/types.h cparse/user.h cparse/util.h)
add_library(cparse parse.c object.c acl.c client.c error.c json.c query.c types.c user.c util.c ${HEADER_FILES})

