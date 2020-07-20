project('telabrium', 'cpp', 'c',
    version : '0.3.0',
    debug : true,
    default_options : ['cpp_std=c++17'],
    license : 'MIT')

include = include_directories('include')

micropak_proj = subproject('micropak')
micropak_dep = micropak_proj.get_variable('micropak_dep')

jsoncpp_proj = subproject('jsoncpp')
jsoncpp_dep = jsoncpp_proj.get_variable('jsoncpp_dep')

core_dep = [
    dependency('GL', method: 'auto'),
    # dependency('wayland-scanner'),
    # dependency('wayland-protocols'),
    dependency('threads'),
    # dependency('XRandR'),
    # dependency('Xi'),
    # dependency('zlib'),
    dependency('assimp'),
    dependency('glfw3', link_args: ['-lglfw']),
    dependency('lua5.3'),
    declare_dependency(link_args: [ '-ldl' ]),
    declare_dependency(link_args: [ '-lstdc++fs' ]),
    jsoncpp_dep,
    micropak_dep
]

subdir('src')
subdir('tests')

