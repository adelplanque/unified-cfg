libfmt_sources = [
  'fmt/src/format.cc',
  'fmt/src/os.cc'
]

libfmt_includedirs = ['fmt/include']

libfmt = static_library(
  'fmt',
  libfmt_sources,
  include_directories: libfmt_includedirs,
  install: false
)

libfmt_dep = declare_dependency(
  link_with: libfmt,
  include_directories: libfmt_includedirs
)

libjinja2cpp_sources = [
  'Jinja2Cpp/src/binding/rapid_json_serializer.cpp',
  'Jinja2Cpp/src/error_info.cpp',
  'Jinja2Cpp/src/expression_evaluator.cpp',
  'Jinja2Cpp/src/expression_parser.cpp',
  'Jinja2Cpp/src/filesystem_handler.cpp',
  'Jinja2Cpp/src/filters.cpp',
  'Jinja2Cpp/src/generic_list.cpp',
  'Jinja2Cpp/src/internal_value.cpp',
  'Jinja2Cpp/src/lexer.cpp',
  'Jinja2Cpp/src/serialize_filters.cpp',
  'Jinja2Cpp/src/statements.cpp',
  'Jinja2Cpp/src/string_converter_filter.cpp',
  'Jinja2Cpp/src/string_functions.cpp',
  'Jinja2Cpp/src/template.cpp',
  'Jinja2Cpp/src/template_env.cpp',
  'Jinja2Cpp/src/template_parser.cpp',
  'Jinja2Cpp/src/testers.cpp',
  'Jinja2Cpp/src/value.cpp'
]

libjinja2cpp_includedirs = [
  'Jinja2Cpp/include',
  'Jinja2Cpp/thirdparty/nonstd/string-view-lite/include',
  'Jinja2Cpp/thirdparty/nonstd/optional-lite/include',
  'Jinja2Cpp/thirdparty/nonstd/variant-lite/include',
  'Jinja2Cpp/thirdparty/nonstd/expected-lite/include',
  'Jinja2Cpp/thirdparty/json/rapid/include',
]

libjinja2cpp = static_library(
  'jinja2cpp',
  libjinja2cpp_sources,
  dependencies: [libfmt_dep, boost_dep],
  include_directories: libjinja2cpp_includedirs,
  install: false
)

libjinja2cpp_dep = declare_dependency(
  link_with: libjinja2cpp,
  include_directories: libjinja2cpp_includedirs
)

libsubprocess_sources = [
  'subprocess/src/cpp/subprocess/CowData.cpp',
  'subprocess/src/cpp/subprocess/PipeVar.cpp',
  'subprocess/src/cpp/subprocess/ProcessBuilder.cpp',
  'subprocess/src/cpp/subprocess/ProcessBuilder_posix.cpp',
  'subprocess/src/cpp/subprocess/ProcessBuilder_windows.cpp',
  'subprocess/src/cpp/subprocess/environ.cpp',
  'subprocess/src/cpp/subprocess/pipe.cpp',
  'subprocess/src/cpp/subprocess/shell_utils.cpp',
  'subprocess/src/cpp/subprocess/utf8_to_utf16.cpp'
]

libsubprocess = static_library(
  'subprocess',
  libsubprocess_sources,
  install: false
)

libsubprocess_dep = declare_dependency(
  link_with: libsubprocess,
  include_directories: ['subprocess/src/cpp']
)
