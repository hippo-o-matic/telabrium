core_source = [
    'glad.c',
    'camera.cpp',
    'config.cpp',
    'input.cpp',
    'jsoncpp.cpp',
    'level.cpp',
    'light.cpp',
    'luafile.cpp',
    'mesh.cpp',
    'model.cpp',
    'object.cpp',
	'safe_val.cpp',
    'shader.cpp',
    'skybox.cpp',
    'task.cpp',
    'utility.cpp'
]

core_dep = [
    dependency('GL'),
    # dependency('wayland-scanner'),
    # dependency('wayland-protocols'),
    dependency('threads'),
    # dependency('XRandR'),
    # dependency('Xi'),
    # dependency('zlib'),
    dependency('assimp'),
    dependency('glfw3'),
    dependency('lua')
]
# message(meson.current_source_dir() + '/../lib')
# core_ext_deps = meson.get_compiler('cpp').find_library('micropak', dirs : meson.current_source_dir() + '/../lib')
micropak_proj = subproject('micropak')
core_dep += micropak_proj.get_variable('micropak_dep')

subdir('generic')

add_project_link_arguments(['-ldl','-lstdc++fs','-lglfw'], language : 'cpp')
telabrium = shared_library('telabrium', 
                core_source,
                include_directories : include,
                dependencies: core_dep)
