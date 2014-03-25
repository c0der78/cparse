cmake_minimum_required (VERSION 2.8)

SET(CMAKE_BUILD_TYPE Debug)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/")

project (TEST)

#INCLUDE(CodeCoverage)

set(CMAKE_C_FLAGS "-DROOT_PATH=\\\"${CMAKE_SOURCE_DIR}\\\"" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -O0 -ggdb  -fprofile-arcs -ftest-coverage" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")

enable_testing ()

add_subdirectory (src)
add_subdirectory (tests)

#SETUP_TARGET_FOR_COVERAGE( cparse_coverage test_cparse coverage )
