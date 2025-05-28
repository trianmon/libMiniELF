# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [v1.0.0] - 2025-05-28

### Added
- Optimized symbol and section lookup:
  - Fast symbol lookup by name using hash map (`_symbolByName`)
  - Fast symbol and section lookup by address using sorted vectors and binary search (`_symbolsSortedByAddr`, `_sectionsSortedByAddr`)
  - Lookup tables are built once and reused for all queries
- Comprehensive Doxygen-style documentation for all public and private API, including struct fields and internal methods
- Improved internal code structure for maintainability and clarity

### Changed
- Refactored `MiniELF` implementation to use efficient lookup tables for all symbol and section queries
- Updated all relevant methods (`getSymbolByAddress`, `getSymbolByName`, `getNearestSymbol`, `getSectionByAddress`) to use optimized lookups
- Enhanced documentation and inline comments throughout the codebase

### Fixed
- Corrected edge cases in address-based symbol and section resolution
- Improved robustness of ELF parsing and error handling

---

## [v0.5.0] - 2025-05-28

### Added
- `MiniELF::getMetadata()` — retrieve ELF file metadata (entry point, machine, type, version, flags)
- CLI command `metadata` in `dump_elf` to display parsed ELF metadata
- Internal storage of `Elf64_Ehdr` as `MiniELF::_elfHeader` for structured access

### Changed
- `MiniELF::parse()` now retains ELF header for future inspection

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

