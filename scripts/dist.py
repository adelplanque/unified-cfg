#!/bin/env python3
import fnmatch
import pathlib
import os


dist_files = [
    "CLI11/CHANGELOG.md",
    "CLI11/LICENSE",
    "CLI11/README.md",
    "CLI11/include/**.hpp",
    "Jinga2Cpp/LICENSE",
    "Jinga2Cpp/README.md",
    "Jinja2Cpp/include/**.h",
    "Jinja2Cpp/src/**.cpp",
    "Jinja2Cpp/src/**.h",
    "Jinja2Cpp/thirdparty/json/rapid/include/**.h",
    "Jinja2Cpp/thirdparty/json/rapid/license.txt",
    "Jinja2Cpp/thirdparty/json/rapid/readme.md",
    "Jinja2Cpp/thirdparty/nonstd/**/include/**.hpp",
    "Jinja2Cpp/thirdparty/nonstd/**/LICENSE.txt",
    "Jinja2Cpp/thirdparty/nonstd/**/README.md",
    "fmt/LICENSE.rst",
    "fmt/README.rst",
    "fmt/include/**.h",
    "fmt/src/**.cc",
    "meson.build",
    "subprocess/LICENSE",
    "subprocess/README.md",
    "subprocess/src/**.cpp",
    "subprocess/src/**.hpp",
    "tabulate/include/**.hpp",
    "tabulate/LICENSE",
    "tabulate/README.md",
]


ext = pathlib.Path(os.environ["MESON_DIST_ROOT"]) / "ext"
for filename in ext.glob("**/*"):
    if not filename.is_file():
        continue
    relative = filename.relative_to(ext)
    if any(fnmatch.fnmatch(relative, pattern) for pattern in dist_files):
        continue
    filename.unlink()

while True:
    empty = [root for root, dirs, files in os.walk(ext) if not dirs and not files]
    if not empty:
        break
    for root in empty:
        (ext / root).rmdir()

for filename in pathlib.Path(os.environ["MESON_DIST_ROOT"]).glob("**/.*"):
    filename.unlink()
