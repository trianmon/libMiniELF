#include "minielf/MiniELF.hpp"
#include <cassert>
#include <iostream>

/**
 * @file test_minielf.cpp
 * @brief Unit tests for the MiniELF parser.
 *
 * This file contains basic tests to verify the functionality of the MiniELF class.
 *
 * The tests ensure that:
 *   - The ELF file is valid and can be parsed.
 *   - Sections and symbols are present in the ELF file.
 *   - The "main" symbol exists and can be resolved by both name and address.
 *   - The getSymbolByAddress and getSymbolByName methods work as expected.
 *   - The getNearestSymbol method finds the closest symbol at or before a given address.
 *   - The getSectionByAddress method resolves the section containing a symbol's address.
 *   - ELF metadata (entry point, version, machine, type) is correct and accessible.
 *
 * Usage:
 *   Compile and run this test to verify the core MiniELF functionality.
 */

int main() {
    // Path to a test ELF file (ensure this file exists for the test to pass)
    const char* path = "../tests/test_elf_file";
    minielf::MiniELF elf(path);
    assert(elf.isValid());

    // Retrieve and check ELF sections
    auto sections = elf.getSections();
    assert(!sections.empty());

    // Retrieve and check ELF symbols
    auto symbols = elf.getSymbols();
    assert(!symbols.empty());

    // Check existence of "main" symbol via getSymbols()
    bool has_main = false;
    for (const auto& sym : symbols) {
        if (sym.name == "main") {
            has_main = true;
            // Verify symbol can be resolved by address
            const auto* resolved = elf.getSymbolByAddress(sym.address);
            assert(resolved && resolved->name == "main");

            // Section resolution by address of symbol
            const auto* sec = elf.getSectionByAddress(sym.address);
            assert(sec);
        }
    }
    assert(has_main);

    // Verify symbol can be resolved by name
    const auto* sym_by_name = elf.getSymbolByName("main");
    assert(sym_by_name && sym_by_name->name == "main");

    // Verify nearest symbol resolution
    const auto* nearest = elf.getNearestSymbol(sym_by_name->address + 1);  // addr > main
    assert(nearest && nearest->name == "main");

    // Check metadata
    auto meta = elf.getMetadata();
    assert(meta.entry != 0);
    assert(meta.version == 1);
    assert(meta.machine == 62);  // EM_X86_64
    assert(meta.type != 0);

    std::cout << "All MiniELF tests passed.\n";
    return 0;
}