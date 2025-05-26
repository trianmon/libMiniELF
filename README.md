# libMiniELF

**libMiniELF** is a minimal, dependency-free C++ library for parsing ELF64 binaries. It provides access to ELF headers, sections, symbols (from `.symtab` and `.dynsym`), and supports symbol resolution.

---

## Features

| Feature                   | Description                                    |
| ------------------------- | ---------------------------------------------- |
| **ELF64 support**         | Parses modern 64-bit ELF binaries              |
| **Symbol table parsing**  | Reads both `.symtab` and `.dynsym` sections    |
| **Section/symbol access** | Lists sections, symbols, and functions         |
| **Address resolution**    | Resolves virtual addresses to symbol names     |
| **ELF32 detection**       | Gracefully skips unsupported ELF32 files       |
| **CLI utility**           | Provides `dump_elf` tool for binary inspection |
| **Unit tested**           | Includes basic tests using `CTest`             |
| **Easy integration**      | Installable via `make install`                 |

---

## Build & Install

```bash
# Clone repository
git clone https://github.com/trianmon/libMiniELF.git
cd libMiniELF

# Build the project
mkdir build && cd build
cmake ..
make -j$(nproc)

# Optionally install
sudo make install
```

---

## CLI Usage

The project includes a demo CLI tool called `dump_elf`.

```bash
./dump_elf <binary> [--symbols | --functions | --resolve <address>]
```

### Examples:

```bash
./dump_elf /bin/ls                  # List ELF sections
./dump_elf /bin/ls --symbols        # List all symbols
./dump_elf /bin/ls --functions      # List function symbols only
./dump_elf /bin/ls --resolve 401000 # Resolve a virtual address to a symbol
```

---

## API Example

```cpp
#include <minielf/MiniELF.hpp>

minielf::MiniELF elf("/path/to/binary");
if (elf.isValid()) {
    auto sections = elf.getSections();
    auto symbols = elf.getSymbols();
    const auto* sym = elf.getSymbolByAddress(0x1234);
}
```

---

## Run Tests

The project uses `CTest` for unit testing. A minimal ELF file is compiled automatically for testing:

```bash
ctest --verbose
```

---

## Roadmap

### Core Enhancements

| Status | Feature                  | Description                                    |
| ------ | ------------------------ | ---------------------------------------------- |
| \[x]   | ELF64 parsing            | Parse headers and sections from ELF64 binaries |
| \[x]   | `.symtab` / `.dynsym`    | Extract symbols from standard ELF tables       |
| \[x]   | CLI tool                 | Command-line utility `dump_elf`                |
| \[x]   | Unit tests via CTest     | Basic test coverage using CTest                |
| \[ ]   | `getSymbolByName(name)`  | Retrieve symbol by name                        |
| \[ ]   | `getNearestSymbol(addr)` | Nearest symbol matching an address             |

### Planned Extensions

| Status | Feature                    | Description                                         |
| ------ | -------------------------- | --------------------------------------------------- |
| \[ ]   | DWARF support              | Separate library (`libTinyDWARF`) for debug info    |
| \[ ]   | Address-to-section mapping | Map address to section with `getSectionByAddress()` |
| \[ ]   | ELF metadata API           | Provide entry point, architecture, etc.             |

---

## License

This project is licensed under the MIT License.
