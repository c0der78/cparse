include_directories(. ../src)
add_executable(test_cparse cparse.test.c parse.test.c json.test.c object.test.c query.test.c util.test.c)
target_link_libraries(test_cparse cparse curl json-c check)
add_test(test_cparse ${CMAKE_CURRENT_BINARY_DIR}/test_cparse)

ADD_CUSTOM_TARGET(lcov)
ADD_CUSTOM_COMMAND(TARGET lcov
    COMMAND mkdir -p coverage
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
ADD_CUSTOM_COMMAND(TARGET lcov
    COMMAND lcov --directory . --zerocounters
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
ADD_CUSTOM_COMMAND(TARGET lcov
    COMMAND make test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
ADD_CUSTOM_COMMAND(TARGET lcov
    COMMAND lcov --directory . --capture --output-file ./coverage/stap.info
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

ADD_CUSTOM_COMMAND(TARGET lcov
    COMMAND lcov --remove ./coverage/stap.info /usr/include\\* tests\\* --output ./coverage/clean.info
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

ADD_CUSTOM_COMMAND(TARGET lcov
    COMMAND genhtml -o ./coverage ./coverage/clean.info --branch-coverage --demangle-cpp
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
ADD_CUSTOM_COMMAND(TARGET lcov
    COMMAND echo "Open ${CMAKE_BINARY_DIR}/coverage/index.html to view the coverage analysis results."
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
ADD_DEPENDENCIES(lcov test_cparse)
