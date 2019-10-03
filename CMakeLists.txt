cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

set(T_COMPILE_OPTIONS "-Wall" "-g" "-std=c++17")
# set(T_LINKER_OPTIONS)

project(telabrium)
set(telabrium_VERSION_MAJOR 0)
set(telabrium_VERSION_MINOR 5)

# configure a header file to pass some of the CMake settings to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/include/telabrium/config.h.in"
  "${PROJECT_BINARY_DIR}/include/telabrium/config.h"
)

option(BUILD_EXTRA "Builds the default object libraries" ON)

add_subdirectory(src)

add_executable(hippotest src/main.cpp)

target_compile_options(hippotest PUBLIC ${T_COMPILE_OPTIONS})
# target_link_options(hippotest)

target_link_libraries(hippotest libtelabrium)
if(BUILD_EXTRA)
  target_link_libraries(hippotest libtelabriumextra)
endif()