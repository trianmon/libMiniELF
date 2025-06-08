# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [v1.1.2] - 2025-06-08

### Changed
- Improved symbol name extraction in ELF parsing: now uses `memchr` to find the first `'\0'` within the string table, preventing buffer overrun and increasing robustness against malformed ELF files.
- Added an extra check for an empty string table (`strtab.empty()`) when parsing symbols.

### Fixed
- Fixed a potential out-of-bounds read when extracting symbol names from the string table.
  
---

## [v1.1.1] - 2025-06-08

### Added
- CMake option `MINIELF_BUILD_TESTS` (default: `ON`) to optionally disable building internal tests

---

## [v1.1.0] - 2025-06-03

### Added
- Error handling interface: errors are now captured and available via `getLastError()`, not printed directly to stderr
- `getSectionByName()` method for fast section lookup by name
- CLI command `section <name>` to find a section by name
- Example usage of section lookup by name and error handling added to README and tests
- Improved unit tests: check for section lookup by name

### Changed
- All internal parsing and ELF errors are now stored and accessible via API, not printed directly
- Documentation and examples updated to reflect new error handling and section lookup features

### Fixed
- Fixed potential issues with repeated filling of sections and symbols on multiple parses
- Improved CLI stability and predictability when handling errors and invalid input

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

