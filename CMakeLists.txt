cmake_minimum_required (VERSION 2.8)

# set path to custom modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# add options for testing
option(CODE_COVERAGE "Enable code coverage testing." OFF)
option(MEMORY_CHECK "Enable testing for memory leaks." OFF)

if(CODE_COVERAGE)
	include(CodeCoverage)
	list(APPEND CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_COVERAGE})
endif()

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

# define project name
project (cparse)

# add directories
add_subdirectory(src)
add_subdirectory(tests)

# Setup testing
enable_testing()

if (MEMORY_CHECK)
	include(MemCheck)
	add_memcheck_test(cparse-test ${CMAKE_BINARY_DIR}/tests/cparse_test)
else ()
	add_test(cparse-test ${CMAKE_BINARY_DIR}/tests/cparse_test)
endif()

# add target for code coverage
if(CODE_COVERAGE)
	setup_target_for_coverage(coverage ${CMAKE_BINARY_DIR}/tests/cparse_test ${CMAKE_SOURCE_DIR}/coverage)
endif()

