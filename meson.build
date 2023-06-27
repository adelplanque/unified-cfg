project(
  'unified-cfg', 'cpp',
  version: '0.2.0',
  license: 'LGPL3',
  meson_version: '>=0.55.0',
  default_options: [
    'b_lto=true',
    'cpp_std=c++17',
  ],
)

cxx = meson.get_compiler('cpp')

add_project_arguments(
  cxx.get_supported_arguments(
    '-ffunction-sections',
    '-fdata-sections'
  ),
  language: 'cpp'
)

add_project_link_arguments(
  cxx.get_supported_link_arguments(
    '-Wl,--gc-sections',
    '-Wl,--exclude-libs,ALL'
  ),
  language: 'cpp'
)

add_project_link_arguments('-lstdc++fs', language: 'cpp')

cxx.has_header('ext/stdio_filebuf.h')

# libfmt_dep = dependency('fmt', version: '>=8.0.0')
boost_dep = dependency('boost')
thread_dep = dependency('threads')

subdir('ext')
subdir('include')
subdir('src')
subdir('tests')

meson.add_dist_script('scripts/dist.sh')
