# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


## [0.2.0] - 2026-05-05

### Added

* `list` subcommand for displaying configuration values.
* `--tabulate` flag to render `list` output as a table.
* New `hardware.meminfo` function.
* Support for multiple `--path` options.
* Expanded naming rules: group and key names can contain additional characters;
  group names may be numeric or include an “@”.
* Boolean values now accept `yes`/`no` literals.

### Fixed

* `cpu_cores` now correctly counts cores on multi‑CPU systems.


## [0.1.0] - 2023-06-08

First release
