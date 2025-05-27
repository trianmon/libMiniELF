# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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

