core_source = [
    'glad.c',
    'camera.cpp',
    'config.cpp',
    'input.cpp',
    'level.cpp',
    'light.cpp',
    'luafile.cpp',
    'mesh.cpp',
    'model.cpp',
    'material.cpp',
    'renderer.cpp',
    'object.cpp',
	'object3d.cpp',
	'safe_val.cpp',
    'shader.cpp',
    'skybox.cpp',
    'task.cpp',
    'utility.cpp'
]

subdir('generic')

telabrium = shared_library('telabrium', 
                core_source,
                include_directories : include,
                dependencies: core_dep)

telabrium_dep = declare_dependency(link_with: telabrium, include_directories: '../include/telabrium')
