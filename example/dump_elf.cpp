/**
 * @file dump_elf.cpp
 * @brief MiniELF CLI - ELF64 Inspection Tool
 *
 * This command-line tool demonstrates usage of the MiniELF library for inspecting ELF64 binaries.
 * It provides commands to:
 *   - List all ELF sections
 *   - List all symbols or only function symbols
 *   - Resolve a symbol by its exact address
 *   - Find the nearest symbol before a given address
 *   - Lookup a symbol by name
 *
 * Usage:
 *   dump_elf <binary> <command> [argument]
 *
 * Commands:
 *   sections                  Show all ELF sections (default)
 *   symbols                   List all symbols
 *   functions                 Show function symbols only
 *   resolve <hex_address>     Find symbol at exact address
 *   resolve-nearest <hex>     Find closest symbol before address
 *   find <symbol_name>        Lookup symbol by name
 *   section-of <hex_address>  Find section containing the given address
 *   metadata                  Show ELF metadata (entry point, architecture, type, flags)
 *
 * Examples:
 *   dump_elf my_binary.elf symbols
 *   dump_elf my_binary.elf resolve 0x401000
 */

#include "minielf/MiniELF.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cctype>

// Prints a formatted table of ELF sections.
void printSectionTable(const std::vector<minielf::Section>& sections) {
    std::cout << std::left << std::setw(20) << "Address"
              << std::setw(25) << "Name"
              << "Size (bytes)\n";
    std::cout << std::string(60, '-') << "\n";
    for (const auto& sec : sections) {
        std::cout << std::left
                  << "0x" << std::setw(18) << std::hex << sec.address
                  << std::setw(25) << sec.name
                  << std::dec << sec.size << "\n";
    }
}

// Prints a formatted table of ELF symbols, optionally filtering for functions only.
void printSymbolTable(const std::vector<minielf::Symbol>& symbols, bool functionsOnly) {
    std::cout << std::left << std::setw(20) << "Address"
              << std::setw(35) << "Name"
              << "Size (bytes)\n";
    std::cout << std::string(70, '-') << "\n";
    for (const auto& sym : symbols) {
        if (functionsOnly && !sym.isFunction()) continue;
        std::cout << std::left
                  << "0x" << std::setw(18) << std::hex << sym.address
                  << std::setw(35) << sym.name
                  << std::dec << sym.size << "\n";
    }
}

// Prints usage instructions for the CLI tool.
void printUsage() {
    std::cerr << "\nMiniELF CLI - ELF64 Inspection Tool\n";
    std::cerr << "-------------------------------------\n";
    std::cerr << "Usage:\n";
    std::cerr << "  dump_elf <binary> <command> [argument]\n\n";
    std::cerr << "Commands:\n";
    std::cerr << "  sections                  Show all ELF sections (default)\n";
    std::cerr << "  symbols                   List all symbols\n";
    std::cerr << "  functions                 Show function symbols only\n";
    std::cerr << "  resolve <hex_address>     Find symbol at exact address\n";
    std::cerr << "  resolve-nearest <hex>     Find closest symbol before address\n";
    std::cerr << "  find <symbol_name>        Lookup symbol by name\n";
    std::cerr << "  section-of <hex_address>  Find section containing the given address\n";
    std::cerr << "  section <section_name>    Lookup section by name\n";
    std::cerr << "  metadata                  Show ELF metadata (entry point, architecture, type, flags)\n\n";
    std::cerr << "Examples:\n";
    std::cerr << "  dump_elf my_binary.elf symbols\n";
    std::cerr << "  dump_elf my_binary.elf resolve 0x401000\n\n";
}

bool isValidHex(const std::string& s) {
    if (s.empty()) return false;
    size_t start = (s.rfind("0x", 0) == 0 || s.rfind("0X", 0) == 0) ? 2 : 0;
    if (start == s.size()) return false;
    for (size_t i = start; i < s.size(); ++i) {
        if (!std::isxdigit(static_cast<unsigned char>(s[i]))) return false;
    }
    return true;
}

static void printHex64(uint64_t addr) {
    std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0') << addr << std::dec;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printUsage();
        return 1;
    }

    minielf::MiniELF elf(argv[1]);
    if (!elf.isValid()) {
        std::cerr << elf.getLastError() << "\n";
        return 1;
    }

    std::string command = argc >= 3 ? argv[2] : "sections";

    if (command == "sections") {
        printSectionTable(elf.getSections());
    } else if (command == "symbols") {
        printSymbolTable(elf.getSymbols(), false);
    } else if (command == "functions") {
        printSymbolTable(elf.getSymbols(), true);
    } else if (command == "resolve" && argc == 4) {
        std::string input = argv[3];
        if (!isValidHex(input)) {
            std::cerr << "Invalid address format: " << input << '\n';
            return 1;
        }
        uint64_t addr = std::stoull(input, nullptr, 16);
        const auto* sym = elf.getSymbolByAddress(addr);
        if (sym) {
            std::cout << "Resolved: " << sym->name << "\t@ 0x"
                    << std::hex << sym->address << std::dec
                    << " (" << sym->size << " bytes)\n";
        } else {
            std::cout << "No symbol found at 0x" << std::hex << addr << '\n';
        }
        return 0;
    } else if (command == "resolve-nearest" && argc == 4) {
        std::string input = argv[3];
        if (!isValidHex(input)) {
            std::cerr << "Invalid address format: " << input << '\n';
            return 1;
        }
        uint64_t addr = std::stoull(input, nullptr, 16);
        const auto* sym = elf.getNearestSymbol(addr);
        if (sym) {
            std::cout << "Nearest: " << sym->name << "\t@ 0x"
                    << std::hex << sym->address << std::dec
                    << " (" << sym->size << " bytes)\n";
        } else {
            std::cout << "No symbol found before 0x" << std::hex << addr << '\n';
        }
        return 0;
    } else if (command == "find" && argc == 4) {
        const auto* sym = elf.getSymbolByName(argv[3]);
        if (sym) {
            std::cout << "Found: " << sym->name << " @ 0x"
                      << std::hex << sym->address << " (" << std::dec << sym->size << " bytes)\n";
        } else {
            std::cout << "Symbol not found: " << argv[3] << "\n";
        }
    } else if (command == "section-of" && argc == 4) {
        uint64_t addr = 0;
        try {
            addr = std::stoull(argv[3], nullptr, 16);
        } catch (...) {
            std::cerr << "Invalid address: " << argv[3] << '\n';
            return 1;
        }

        const auto* sec = elf.getSectionByAddress(addr);
        if (sec) {
            std::cout << "Address 0x" << std::hex << addr << " is in section: " << sec->name
                    << " @ 0x" << sec->address << " (" << std::dec << sec->size << " bytes)\n";
        } else {
            std::cout << "Address 0x" << std::hex << addr << " not found in any section.\n";
        }
        return 0;
    } else if (command == "section" && argc == 4) {
        // Lookup section by name
        const auto* sec = elf.getSectionByName(argv[3]);
        if (sec) {
            std::cout << "Section: " << sec->name
                    << " @ 0x" << std::hex << sec->address
                    << " (" << std::dec << sec->size << " bytes)\n";
        } else {
            std::cout << "Section not found: " << argv[3] << "\n";
        }
        return 0;
    } else if (command == "metadata") {
        const auto meta = elf.getMetadata();
        std::cout << "ELF Metadata:\n";
        std::cout << "  Entry point : ";
        printHex64(meta.entry);
        std::cout << "\n";
        std::cout << "  Machine     : " << meta.machine << "\n";
        std::cout << "  Type        : " << meta.type << "\n";
        std::cout << "  Version     : " << meta.version << "\n";
        std::cout << "  Flags       : " << meta.flags << "\n";
    } else {
        std::cerr << "Unknown or malformed command.\n";
        return 1;
    }

    return 0;
}