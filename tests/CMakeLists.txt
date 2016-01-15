
add_executable(${PROJECT_NAME}-test acl.test.c client.test.c config.test.c cparse.test.c json.test.c object.test.c parse.test.c query.test.c role.test.c user.test.c util.test.c)

include(FindCheck)

add_definitions(-DROOT_PATH="${CMAKE_SOURCE_DIR}/tests")

include_directories(${CMAKE_SOURCE_DIR}/src ${CHECK_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME} ${CHECK_LIBRARIES})

add_subdirectory(afl)

