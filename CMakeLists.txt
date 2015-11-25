cmake_minimum_required (VERSION 2.8)

# set path to custom modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# add options for testing
option(CODE_COVERAGE "Enable code coverage testing." OFF)
option(MEMORY_CHECK "Enable testing for memory leaks." OFF)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

# define project name
project (cparse)

include(FindJSONC)
include(FindCURL)

if (NOT JSON_C_FOUND)
	message(FATAL_ERROR "Json-C library is required")
elseif (JSON_C_EXTENDED)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DHAVE_JSON_EXTENDED=1")
endif()

if (NOT CURL_FOUND)
	message(FATAL_ERROR "Curl library is required")
endif()

# add directories
add_subdirectory(src)
add_subdirectory(tests)

# Setup testing
enable_testing()

if (MEMORY_CHECK)
	include(MemCheck)
	add_memcheck_test(${PROJECT_NAME}-test ${CMAKE_BINARY_DIR}/tests/${PROJECT_NAME}_test)
else ()
	add_test(${PROJECT_NAME}-test ${CMAKE_BINARY_DIR}/tests/${PROJECT_NAME}_test)
endif()

# add target for code coverage

if(CODE_COVERAGE)
	include(CodeCoverage)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_COVERAGE}")
	setup_target_for_coverage(${PROJECT_NAME}-coverage ${CMAKE_BINARY_DIR}/tests/${PROJECT_NAME}_test ${CMAKE_SOURCE_DIR}/coverage)
endif()

