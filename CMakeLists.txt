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
endif()

if (NOT CURL_FOUND)
	message(FATAL_ERROR "Curl library is required")
endif()

# add target for code coverage
if(CODE_COVERAGE)
	message(STATUS "Setting up for code coverage")
	include(CodeCoverage)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_COVERAGE}")
	setup_target_for_coverage(${PROJECT_NAME}-coverage ${CMAKE_BINARY_DIR}/tests/${PROJECT_NAME}_test ${CMAKE_SOURCE_DIR}/coverage)
endif()

# add directories
add_subdirectory(src)
add_subdirectory(tests)

# Setup testing
message(STATUS "Setting up testing")
enable_testing()
include(MemCheck)
add_memcheck_test(${PROJECT_NAME}-test ${CMAKE_BINARY_DIR}/tests/${PROJECT_NAME}-test  "--suppressions=${CMAKE_SOURCE_DIR}/suppression.map")

find_program(HEADERDOC_FOUND headerdoc2html)

if (HEADERDOC_FOUND)
	add_custom_target(generate-docs mkdir -p ${CMAKE_SOURCE_DIR}/html COMMAND headerdoc2html -o ${CMAKE_SOURCE_DIR}/html ${CMAKE_SOURCE_DIR}/src/cparse COMMAND gatherheaderdoc ${CMAKE_SOURCE_DIR}/html COMMAND mv ${CMAKE_SOURCE_DIR}/html/masterTOC.html ${CMAKE_SOURCE_DIR}/html/index.html)
endif()

find_program(DOXYGEN_FOUND doxygen)

if (DOXYGEN_FOUND)
	add_custom_target(generate-doxygen doxygen ${CMAKE_SOURCE_DIR}/Doxyfile)
endif()

