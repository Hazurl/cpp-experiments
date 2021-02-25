project('cpp-experiments', 'cpp',
    version : '0.1',
    default_options : [
        'warning_level=3',
        'cpp_std=c++20',
        'buildtype=release',
        'native=true',
        'b_coverage=true',
        'b_lto_threads=4',
        'b_lto=true',
		'b_ndebug=if-release',
    ])

# Set error limit.
if meson.get_compiler('cpp').get_id() == 'clang'
	add_project_arguments('-ferror-limit=2', language: 'cpp')

elif meson.get_compiler('cpp').get_id() == 'gcc'
	add_project_arguments('-fmax-errors=2', language: 'cpp')
endif

# Add supports for coroutines
add_project_arguments('-fcoroutines', language: 'cpp')


sources = [
    'source/main.cpp',

    'source/cppexp/deleter.cpp',
]

includes = [
    'include',
]

dependencies = [
    dependency('threads'),
]


exe = executable('cpp_experiments', sources, dependencies: dependencies, include_directories: includes)

# test('basic', exe)
