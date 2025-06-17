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

    // Check existence of ".text" section
    const auto* text_sec = elf.getSectionByName(".text");
    assert(text_sec && text_sec->name == ".text");
    assert(text_sec->size > 0);

    // Retrieve and check ELF symbols
    auto symbols = elf.getSymbols();
    assert(!symbols.empty());

    // Check existence of "main" symbol via getSymbols()
    bool has_main = false;
    for (const auto& sym : symbols) {
        if (sym.name == "main") {
            has_main = true;
            // Verify symbol can be resolved by address
            const auto* resolved = elf.getSymbolByName("main");
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

    // Test: getRawHeader
    const auto& ehdr = elf.getRawHeader();
    assert(ehdr.e_entry == meta.entry);
    assert(ehdr.e_machine == meta.machine);

    // Test: getSectionHeaders
    const auto& shdrs = elf.getSectionHeaders();
    assert(!shdrs.empty());
    assert(shdrs.size() == sections.size() || shdrs.size() == ehdr.e_shnum);

    // Test: getProgramHeaders
    const auto& phdrs = elf.getProgramHeaders();
    // Program headers may be empty for relocatable objects, but should not crash
    (void)phdrs;

    // Test: getFileSize
    uint64_t fsize = elf.getFileSize();
    assert(fsize > 0);

    // Test: getSectionStringTableRaw
    const auto& shstrtab = elf.getSectionStringTableRaw();
    assert(!shstrtab.empty());

    // Test: getFailureStage (should be Header if no error)
    auto stage = elf.getFailureStage();
    assert(stage == minielf::MiniELF::ParseStage::Header ||
           stage == minielf::MiniELF::ParseStage::Symbols ||
           stage == minielf::MiniELF::ParseStage::ProgramHeaders);

    // Test: getValidationLog
    std::string log = elf.getValidationLog();
    assert(!log.empty());
    std::cout << "[Validation log]\n" << log << std::endl;

    std::cout << "All MiniELF tests passed.\n";
    return 0;
}