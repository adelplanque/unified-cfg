#!/bin/env bash
set -x

env -C "${MESON_DIST_ROOT}" rm -rf \
    ext/CLI11/{book,cmake,docs,examples,scripts,src,subprojects,test_package,tests} \
    ext/Jinja2Cpp/thirdparty/json/rapid/{CMakeModules,bin,contrib,doc,docker,example,test,thirdparty} \
    ext/Jinja2Cpp/thirdparty/{fmtlib,gtest,json/nlohmann} \
    ext/Jinja2Cpp/{cmake,scripts,test} \
    ext/fmt/{test,doc,support} \
    ext/subprocess/{docs,test}

find "${MESON_DIST_ROOT}" -name CMakeLists.txt -o -name ".*" | xargs rm -rf
