
add_library(cparse client.c error.c json.c log.c object.c operators.c parse.c query.c role.c types.c user.c util.c)

include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/src SYSTEM ${CURL_INCLUDE_DIR} SYSTEM ${JSON_C_INCLUDE_DIR})

target_link_libraries(cparse ${CURL_LIBRARIES} ${JSON_C_LIBRARY})


