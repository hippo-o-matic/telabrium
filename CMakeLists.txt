cmake_minimum_required (VERSION 3.15)

set (CMAKE_C_COMPILER "/usr/bin/gcc")
set (CMAKE_CXX_COMPILER "/usr/bin/g++")

project(hippotest)

set (hippotest_VERSION_MAJOR 0)
set (hippotest_VERSION_MINOR 5)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/include/luarium/config.h.in"
  "${PROJECT_BINARY_DIR}/include/luarium/config.h"
  )

include_directories("${PROJECT_SOURCE_DIR}/include", "/usr/include", "usr/local/include")

add_library(libtelabrium)

file(GLOB SOURCES "src/*.cpp")

add_executable(hippotest )