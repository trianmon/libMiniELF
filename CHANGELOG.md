# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [v0.4.0] - 2025-05-28

### Added
- `MiniELF::getSectionByAddress()` — resolve section containing a given virtual address
- CLI command `section-of <addr>` to map address → section

---

## [v0.3.1] - 2025-05-28

### Changed
- Refactored CLI argument parsing for `dump_elf`
- Improved output formatting: aligned columns, consistent `0x` prefix
- Enhanced error handling for malformed addresses in `resolve` and `resolve-nearest`

### Added
- Automatic usage message when no subcommand is provided
- Graceful fallback for invalid arguments (e.g. `resolve asdf`, `resolve 0xz`)

---

## [v0.3.0] - 2025-05-28

### Added
- `MiniELF::getNearestSymbol()` — resolve nearest symbol before a given address
- CLI flag `--resolve-nearest <addr>` in `dump_elf`
- Unit test for `getNearestSymbol()`

---

## [v0.2.0] - 2025-05-27

### Added
- `MiniELF::getSymbolByName()` — search for a symbol by name
- CLI flag `--find <symbol>` in `dump_elf`
- Unit test for `getSymbolByName()`

---

## [v0.1.0] - 2025-05-27

### Added
- Initial ELF64 binary parser core
- Section parsing (`.text`, `.data`, `.bss`, etc.)
- Symbol table parsing from `.symtab` and `.dynsym`
- Symbol resolution by virtual address
- CLI tool `dump_elf` with support for:
  - `--symbols` — print all symbols
  - `--functions` — print only function symbols
  - `--resolve <addr>` — resolve address to symbol
- CTest-based unit testing
- Minimal `test_elf_file` auto-generated for testing
- MIT License, README, and GitHub metadata

