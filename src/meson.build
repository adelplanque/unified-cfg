libcfg_sources = [
  'config.cpp',
  'env.cpp',
  'hardware.cpp',
  'parser.cpp',
  'render.cpp',
  'render_interface.cpp',
  'settings.cpp',
  'value.cpp',
]

libcfg = shared_library(
  'cfg',
  libcfg_sources,
  dependencies: [boost_dep, libfmt_dep, libsubprocess_dep, libjinja2cpp_dep, thread_dep],
  include_directories: ['../include'],
  install: true
)

pkg = import('pkgconfig')
pkg.generate(libcfg)

libcfg_dep = declare_dependency(link_with: libcfg)

cfg = executable(
  'cfg',
  ['main.cpp',],
  dependencies: [libcfg_dep, libfmt_dep, boost_dep, libcli11_dep, libtabulate_dep],
  include_directories: ['../include'],
  link_args: cxx.get_supported_link_arguments('-fvisibility=hidden'),
  install: true
)
