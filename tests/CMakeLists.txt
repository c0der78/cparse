
add_executable(cparse_test acl.test.c client.test.c cparse.test.c json.test.c object.test.c parse.test.c query.test.c role.test.c user.test.c util.test.c)

include(FindCheck)

list(APPEND CMAKE_C_FLAGS "-DROOT_PATH=\\\"${CMAKE_SOURCE_DIR}/tests\\\"")

include_directories(../src ${CHECK_INCLUDE_DIRS})

target_link_libraries(cparse_test cparse ${CHECK_LIBRARIES})

