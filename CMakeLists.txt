cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

set(T_INCLUDE_PATH "${PROJECT_SOURCE_DIR}/include" "/usr/include" "/usr/local/include")
set(T_LIBRARY_PATH "${PROJECT_SOURCE_DIR}/lib")
set(T_LIBRARIES -lGL glfw -lassimp -lX11 -lpthread -lXrandr -lXi -ldl -llua5.3 -lstdc++fs -lz -lmicropak)

add_compile_options(-Wall -g -std=c++17)
add_link_options(-Wl,-rpath,${T_LIBRARY_PATH})

project(telabrium)
set(telabrium_VERSION_MAJOR 0)
set(telabrium_VERSION_MINOR 5)

# configure a header file to pass some of the CMake settings to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/include/telabrium/config.h.in"
  "${PROJECT_BINARY_DIR}/include/telabrium/config.h"
)

option(BUILD_EXTRA "Builds the default object libraries" OFF)

add_subdirectory(src)

add_executable(hippotest src/main.cpp)

include_directories("${PROJECT_SOURCE_DIR}/include" "/usr/include" "usr/local/include")
link_directories("${PROJECT_SOURCE_DIR}/lib")

target_link_libraries(hippotest telabrium ${T_LIBRARIES})
# if(BUILD_EXTRA)
#   target_link_libraries(hippotest libtelabriumextra)
# endif()