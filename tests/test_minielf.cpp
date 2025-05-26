#include "minielf/MiniELF.hpp"
#include <cassert>
#include <iostream>

/**
 * @brief Basic tests for the MiniELF parser.
 *
 * This test checks:
 *   - ELF file validity
 *   - Presence of sections and symbols
 *   - Existence and resolution of the "main" symbol
 */
int main() {
    const char* path = "../tests/test_elf_file";  // Relative path from build/
    minielf::MiniELF elf(path);
    assert(elf.isValid());

    auto sections = elf.getSections();
    assert(!sections.empty());

    auto symbols = elf.getSymbols();
    assert(!symbols.empty());

    bool has_main = false;
    for (const auto& sym : symbols) {
        if (sym.name == "main") {
            has_main = true;
            const auto* resolved = elf.getSymbolByAddress(sym.address);
            assert(resolved && resolved->name == "main");
        }
    }

    assert(has_main);
    std::cout << "All MiniELF tests passed.\n";
    return 0;
}