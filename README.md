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
| **Raw ELF access**        | Access raw ELF headers, section/program headers, and string tables |
| **Diagnostics**           | Detailed validation log and error stage reporting |
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
| `section <name>`          | Find section by name                          |
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
./dump_elf ../tests/test_elf_file section .text            # Find section by name
./dump_elf ../tests/test_elf_file metadata                 # Show ELF metadata
```

---

## API Example

```cpp
#include <minielf/MiniELF.hpp>
#include <iostream>

int main() {
    minielf::MiniELF elf("binary.elf");
    if (!elf.isValid()) {
        std::cerr << "Error: " << elf.getLastError() << std::endl;
        return 1;
    }

    // Get all sections
    auto sections = elf.getSections();
    std::cout << "Sections count: " << sections.size() << std::endl;

    // Find section by name
    const auto* textSec = elf.getSectionByName(".text");
    if (textSec) {
        std::cout << ".text section at 0x" << std::hex << textSec->address
                  << " (" << std::dec << textSec->size << " bytes)\n";
    }

    // Get all symbols
    auto symbols = elf.getSymbols();
    std::cout << "Symbols count: " << symbols.size() << std::endl;

    // Find symbol by name
    const auto* mainSym = elf.getSymbolByName("main");
    if (mainSym) {
        std::cout << "main symbol at 0x" << std::hex << mainSym->address
                  << " (" << std::dec << mainSym->size << " bytes)\n";
    }

    // Find symbol by address
    const auto* symByAddr = elf.getSymbolByAddress(0x1234);
    if (symByAddr) {
        std::cout << "Symbol at 0x1234: " << symByAddr->name << std::endl;
    }

    // Find nearest symbol before address
    const auto* nearest = elf.getNearestSymbol(0x1234);
    if (nearest) {
        std::cout << "Nearest symbol before 0x1234: " << nearest->name << std::endl;
    }

    // Find section by address
    const auto* secByAddr = elf.getSectionByAddress(0x1234);
    if (secByAddr) {
        std::cout << "Section for 0x1234: " << secByAddr->name << std::endl;
    }

    // Get ELF metadata
    auto meta = elf.getMetadata();
    std::cout << "Entry point: 0x" << std::hex << meta.entry << std::dec << std::endl;
    std::cout << "Machine: " << meta.machine << ", Type: " << meta.type << std::endl;
    
    // Access raw ELF header
    const auto& ehdr = elf.getRawHeader();
    std::cout << "ELF entry point: 0x" << std::hex << ehdr.e_entry << std::dec << std::endl;

    // Access raw section headers
    const auto& shdrs = elf.getSectionHeaders();
    std::cout << "Section headers count: " << shdrs.size() << std::endl;

    // Access raw program headers
    const auto& phdrs = elf.getProgramHeaders();
    std::cout << "Program headers count: " << phdrs.size() << std::endl;

    // Access raw section header string table
    const auto& shstrtab = elf.getSectionStringTableRaw();
    std::cout << "Section string table size: " << shstrtab.size() << std::endl;

    // Get file size
    uint64_t fileSize = elf.getFileSize();
    std::cout << "ELF file size: " << fileSize << " bytes" << std::endl;

    // Get validation log
    std::cout << elf.getValidationLog() << std::endl;

    return 0;
}
```

---

## Error Handling

If parsing fails or the ELF file is invalid, you can retrieve detailed error information:

```cpp
minielf::MiniELF elf("binary.elf");
if (!elf.isValid()) {
    // Print last error message
    std::cerr << elf.getLastError() << std::endl;

    // Print the stage where parsing failed
    auto stage = elf.getFailureStage();
    std::cerr << "Failure stage: ";
    switch (stage) {
        case minielf::MiniELF::ParseStage::Header: std::cerr << "Header"; break;
        case minielf::MiniELF::ParseStage::SectionHeaders: std::cerr << "SectionHeaders"; break;
        case minielf::MiniELF::ParseStage::Symbols: std::cerr << "Symbols"; break;
        case minielf::MiniELF::ParseStage::ProgramHeaders: std::cerr << "ProgramHeaders"; break;
        default: std::cerr << "Unknown"; break;
    }
    std::cerr << std::endl;

    // Print detailed validation log
    std::cerr << elf.getValidationLog() << std::endl;
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