
add_executable(${PROJECT_NAME}-fuzz harness.test.c ../config.test.c)

target_link_libraries(${PROJECT_NAME}-fuzz ${PROJECT_NAME} ${CURL_LIBRARIES})

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/tests)
