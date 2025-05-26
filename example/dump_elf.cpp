#include "minielf/MiniELF.hpp"
#include <iostream>

/**
 * @brief Minimal ELF file dumper utility.
 *
 * Usage:
 *   dump_elf <binary> [--symbols|--functions|--resolve <addr>]
 *
 * Options:
 *   --symbols      Print all symbols in the ELF file.
 *   --functions    Print only function symbols.
 *   --resolve <a>  Resolve symbol by address (hex).
 *   (default)      Print all sections.
 */
int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: dump_elf <binary> [--symbols|--functions|--resolve <addr>]\n";
        return 1;
    }

    minielf::MiniELF elf(argv[1]);
    if (!elf.isValid()) {
        std::cerr << "Failed to parse ELF file.\n";
        return 1;
    }

    // Print all symbols
    if (argc >= 3 && std::string(argv[2]) == "--symbols") {
        for (const auto& sym : elf.getSymbols()) {
            std::cout << "[SYM] " << sym.name << " @ 0x"
                      << std::hex << sym.address
                      << " (" << std::dec << sym.size << " bytes)\n";
        }
        return 0;
    }

    // Print only function symbols
    if (argc >= 3 && std::string(argv[2]) == "--functions") {
        for (const auto& sym : elf.getSymbols()) {
            if (sym.isFunction()) {
                std::cout << "[FUNC] " << sym.name << " @ 0x"
                          << std::hex << sym.address
                          << " (" << std::dec << sym.size << " bytes)\n";
            }
        }
        return 0;
    }

    // Resolve symbol by address
    if (argc >= 4 && std::string(argv[2]) == "--resolve") {
        uint64_t addr = std::stoull(argv[3], nullptr, 16);  // hex
        const auto* sym = elf.getSymbolByAddress(addr);
        if (sym) {
            std::cout << "Resolved: " << sym->name << " @ 0x" << std::hex << sym->address << "\n";
        } else {
            std::cout << "No symbol found for 0x" << std::hex << addr << "\n";
        }
        return 0;
    }

    // Default: print all sections
    auto sections = elf.getSections();
    for (const auto& sec : sections) {
        std::cout << sec.name << " @ 0x" << std::hex << sec.address
                  << " (" << std::dec << sec.size << " bytes)\n";
    }

    return 0;
}