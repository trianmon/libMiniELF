# libMiniELF

**libMiniELF** is a minimal, dependency-free C++ library for parsing ELF64 binaries.  
It allows inspection of ELF headers, sections, symbol tables (`.symtab` and `.dynsym`), and address resolution — all in a clean and embeddable API.

---

## Features

| Feature                   | Description                                      |
|---------------------------|--------------------------------------------------|
| **ELF64 support**         | Parses modern 64-bit ELF binaries                |
| **Symbol table parsing**  | Reads `.symtab` and `.dynsym` symbols            |
| **Section/symbol access** | Lists ELF sections, functions, and symbols       |
| **Address resolution**    | Resolves addresses to closest matching symbols   |
| **ELF32 detection**       | Gracefully skips unsupported 32-bit binaries     |
| **CLI utility**           | Includes `dump_elf` tool for ELF inspection      |
| **Unit tested**           | CTest-based validation with real ELF files       |
| **Easy integration**      | Header-only usage + `make install` support       |

---

## Build & Install

```bash
# Clone repository
git clone https://github.com/trianmon/libMiniELF.git
cd libMiniELF

# Configure & build
mkdir build && cd build
cmake ..
make -j$(nproc)

# Optional install
sudo make install
```

---

## CLI Usage

The included tool `dump_elf` provides quick introspection:

```bash
./dump_elf <binary> [symbols | functions | resolve <address> | resolve-nearest <address> | find <name> | sections | section-of <address> | metadata]
```

### Supported commands:

| Command                   | Description                                   |
|---------------------------|-----------------------------------------------|
| `sections`                | Show ELF sections                             |
| `symbols`                 | List all symbols                              |
| `functions`               | List only function symbols                    |
| `resolve <addr>`          | Find symbol at exact virtual address (hex)    |
| `resolve-nearest <addr>`  | Find closest symbol before address            |
| `find <name>`             | Look up symbol by name                        |
| `section-of <addr>`       | Find section containing the given address     |
| `metadata`                | Show ELF metadata (entry point, arch, type)   |

### Examples:

```bash
./dump_elf                                                 # List available commands
./dump_elf ../tests/test_elf_file sections                 # Print ELF sections
./dump_elf ../tests/test_elf_file symbols                  # Print all symbols
./dump_elf ../tests/test_elf_file functions                # Show only function symbols
./dump_elf ../tests/test_elf_file resolve 0x1129           # Exact address match
./dump_elf ../tests/test_elf_file resolve-nearest 0x1130   # Closest symbol ≤ address
./dump_elf ../tests/test_elf_file find main                # Find symbol by name
./dump_elf ../tests/test_elf_file section-of 0x1129        # Find section containing address
./dump_elf ../tests/test_elf_file metadata                 # Show ELF metadata
```

---

## API Example

```cpp
#include <minielf/MiniELF.hpp>

minielf::MiniELF elf("binary.elf");
if (elf.isValid()) {
    auto symbols = elf.getSymbols();
    const auto* exact = elf.getSymbolByAddress(0x1234);
    const auto* nearest = elf.getNearestSymbol(0x1234);
    const auto* byName = elf.getSymbolByName("main");
}
```

---

## Run Tests

```bash
cd build
ctest --verbose
```

---

## Roadmap

### Core Enhancements

| Status | Feature                  | Description                                    |
|--------|--------------------------|------------------------------------------------|
| [x]    | ELF64 parsing            | Parse headers and sections                     |
| [x]    | `.symtab` / `.dynsym`    | Symbol extraction                              |
| [x]    | CLI tool                 | `dump_elf` utility                             |
| [x]    | Unit tests via CTest     | Automated test coverage                        |
| [x]    | `getSymbolByName(name)`  | Resolve symbols by name                        |
| [x]    | `getNearestSymbol(addr)` | Find closest symbol before an address          |

### Planned Extensions

| Status | Feature                    | Description                                         |
|--------|----------------------------|-----------------------------------------------------|
| [ ]    | DWARF support              | Separate module `libTinyDWARF` for debug symbols    |
| [x]    | Address-to-section mapping | Map address → section via `getSectionByAddress()`   |
| [x]    | ELF metadata API           | Expose architecture, entry point, flags, etc.       |

---

## License

MIT © 2025 [trianmon](https://github.com/trianmon)