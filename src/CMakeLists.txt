
message(STATUS "Configuring ${PROJECT_NAME} library")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic")

include_directories(${THIS_OUTPUT_DIR})

add_library(${PROJECT_NAME} client.c data_list.c error.c json.c log.c object.c operators.c parse.c query.c request.c role.c types.c user.c util.c)

include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/src SYSTEM ${CURL_INCLUDE_DIR} SYSTEM ${JSON_C_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} ${CURL_LIBRARIES} ${JSON_C_LIBRARY})

install(DIRECTORY ${PROJECT_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/include")

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)


