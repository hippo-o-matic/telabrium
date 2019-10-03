include_directories("${PROJECT_SOURCE_DIR}/include" "/usr/include" "usr/local/include")

set(telabrium_CORE_SOURCES 
    camera.cpp 
    config.cpp 
    input.cpp 
    level.cpp 
    light.cpp 
    luafile.cpp 
    mesh.cpp 
    model.cpp 
    object.cpp 
    shader.cpp 
    skybox.cpp 
    task.cpp 
    utility.cpp 

    glad.c 
    jsoncpp.cpp
)

if(BUILD_EXTRA)
    add_subdirectory(generic)
endif()

add_library(telabrium ${telabrium_CORE_SOURCES})