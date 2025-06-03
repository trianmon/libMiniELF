#include "minielf/MiniELF.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

namespace minielf {

/**
 * @brief Construct a MiniELF object and parse the ELF file.
 * @param filepath Path to the ELF file.
 */
MiniELF::MiniELF(const std::string& filepath) : _filepath(filepath) {
    parse();
    // Prepare sorted pointers for fast lookup
    for (const auto& sym : _symbols) _symbolsSortedByAddr.push_back(&sym);
    for (const auto& sec : _sections) _sectionsSortedByAddr.push_back(&sec);
    _lookupBuilt = false;
}


/**
 * @brief Build fast lookup tables for symbols and sections.
 *
 * This method initializes and sorts internal data structures to enable efficient
 * symbol and section lookups by name and address. It constructs a hash map for
 * symbol name lookup and sorts vectors of pointers for binary search by address.
 * The lookup tables are built only once and reused for subsequent queries.
 */
void MiniELF::buildLookups() const {
    if (_lookupBuilt) return;
    _symbolByName.clear();
    for (const auto& sym : _symbols) {
        _symbolByName[sym.name] = &sym;
    }
    // Add section name lookup
    _sectionByName.clear();
    for (const auto& sec : _sections) {
        _sectionByName[sec.name] = &sec;
    }
    // Sort symbols and sections by address for binary search
    std::sort(_symbolsSortedByAddr.begin(), _symbolsSortedByAddr.end(),
        [](const Symbol* a, const Symbol* b) { return a->address < b->address; });
    std::sort(_sectionsSortedByAddr.begin(), _sectionsSortedByAddr.end(),
        [](const Section* a, const Section* b) { return a->address < b->address; });
    _lookupBuilt = true;
}

/**
 * @brief Check if the ELF file was parsed successfully.
 * @return true if valid, false otherwise.
 */
bool MiniELF::isValid() const {
    return _valid;
}

/**
 * @brief Get the list of ELF sections.
 * @return Vector of Section objects.
 */
std::vector<Section> MiniELF::getSections() const {
    return _sections;
}

/**
 * @brief Get the list of ELF symbols.
 * @return Vector of Symbol objects.
 */
std::vector<Symbol> MiniELF::getSymbols() const {
    return _symbols;
}

/**
 * @brief Find a symbol by its address.
 * @param addr Address to search for.
 * @return Pointer to Symbol if found, nullptr otherwise.
 */
const Symbol* MiniELF::getSymbolByAddress(uint64_t addr) const {
    buildLookups();
    auto it = std::lower_bound(
        _symbolsSortedByAddr.begin(), _symbolsSortedByAddr.end(), addr,
        [](const Symbol* sym, uint64_t address) {
            return sym->address + sym->size <= address;
        });
    if (it != _symbolsSortedByAddr.begin()) --it;
    if (it != _symbolsSortedByAddr.end()) {
        const Symbol* sym = *it;
        if (sym->address <= addr && addr < sym->address + sym->size)
            return sym;
    }
    return nullptr;
}

/**
 * @brief Find a symbol by its name.
 * @param name Name of the symbol to search for.
 * @return Pointer to Symbol if found, nullptr otherwise.
 */
const Symbol* MiniELF::getSymbolByName(const std::string& name) const {
    buildLookups();
    auto it = _symbolByName.find(name);
    return it != _symbolByName.end() ? it->second : nullptr;
}

/**
 * @brief Find the nearest symbol with address <= given address.
 * @param address The address to resolve.
 * @return Pointer to nearest Symbol if found, nullptr otherwise.
 */
const Symbol* MiniELF::getNearestSymbol(uint64_t address) const {
    buildLookups();
    auto it = std::upper_bound(
        _symbolsSortedByAddr.begin(), _symbolsSortedByAddr.end(), address,
        [](uint64_t address, const Symbol* sym) {
            return address < sym->address;
        });
    if (it == _symbolsSortedByAddr.begin()) return nullptr;
    --it;
    return *it;
}

/**
 * @brief Get a section by its address.
 * @param addr Address of the section to find.
 * @return Pointer to Section if found, nullptr otherwise.
 */
const Section* MiniELF::getSectionByAddress(uint64_t addr) const {
    buildLookups();

    auto it = std::lower_bound(
        _sectionsSortedByAddr.begin(), _sectionsSortedByAddr.end(), addr,
        [](const Section* sec, uint64_t address) {
            return sec->address + sec->size <= address;
        });

    if (it != _sectionsSortedByAddr.end()) {
        const Section* sec = *it;
        if (sec->address <= addr && addr < sec->address + sec->size)
            return sec;
    }

    if (it != _sectionsSortedByAddr.begin()) {
        --it;
        const Section* sec = *it;
        if (sec->address <= addr && addr < sec->address + sec->size)
            return sec;
    }

    return nullptr;
}

/**
 * @brief Get a section by its name.
 * @param name Name of the section to search for.
 * @return Pointer to Section if found, nullptr otherwise.
 */
const Section* MiniELF::getSectionByName(const std::string& name) const {
    buildLookups();
    auto it = _sectionByName.find(name);
    return it != _sectionByName.end() ? it->second : nullptr;
}

/**
 * @brief Get the metadata of the ELF file.
 * @return ElfMetadata containing entry point, architecture, type, and flags.
 */
ElfMetadata MiniELF::getMetadata() const {
    ElfMetadata meta{};
    if (!isValid()) return meta;

    meta.type = _elfHeader.e_type;
    meta.machine = _elfHeader.e_machine;
    meta.version = _elfHeader.e_version;
    meta.entry = _elfHeader.e_entry;
    meta.flags = _elfHeader.e_flags;

    return meta;
}

/**
 * @brief Parse the ELF file and populate sections and symbols.
 */
void MiniELF::parse() {
    std::ifstream file(_filepath, std::ios::binary);
    if (!file) {
        setError("MiniELF error: failed to open file: " + _filepath);
        return;
    }

    Elf64_Ehdr ehdr{};
    file.read(reinterpret_cast<char*>(&ehdr), sizeof(ehdr));
    if (file.gcount() != sizeof(ehdr)) {
        setError("MiniELF error: failed to read ELF header");
        return;
    }

    if (ehdr.e_ident[0] != 0x7f || ehdr.e_ident[1] != 'E' ||
        ehdr.e_ident[2] != 'L'  || ehdr.e_ident[3] != 'F') {
        setError("MiniELF error: not an ELF file");
        return;
    }

    if (ehdr.e_ident[4] != 2 /* ELFCLASS64 */) {
        setError("MiniELF error: ELF32 not supported yet");
        return;
    }

    if (ehdr.e_shoff == 0 || ehdr.e_shnum == 0) {
        setError("MiniELF error: no section headers");
        return;
    }

    // Read section headers
    file.seekg(ehdr.e_shoff, std::ios::beg);
    std::vector<Elf64_Shdr> shdrs(ehdr.e_shnum);
    for (auto& sh : shdrs) {
        file.read(reinterpret_cast<char*>(&sh), sizeof(sh));
    }

    // Read section header string table
    const auto& shstrtab = shdrs[ehdr.e_shstrndx];
    std::vector<char> shstr(shstrtab.sh_size);
    file.seekg(shstrtab.sh_offset, std::ios::beg);
    file.read(shstr.data(), shstr.size());

    _elfHeader = ehdr;

    // Populate sections
    for (const auto& sh : shdrs) {
        Section sec;
        sec.name = std::string(&shstr[sh.sh_name]);
        sec.address = sh.sh_addr;
        sec.size = sh.sh_size;
        _sections.push_back(sec);
    }

    parseSymbols(file, shstr, shdrs, ehdr);
    _valid = true;
}

/**
 * @brief Parse symbols from the ELF file.
 * @param file      Input file stream (already open).
 * @param shstrtab  Section header string table.
 * @param shdrs     Section headers.
 * @param ehdr      ELF header.
 */
void MiniELF::parseSymbols(std::ifstream& file, const std::vector<char>& shstrtab,
                           const std::vector<Elf64_Shdr>& shdrs, const Elf64_Ehdr& ehdr) {
    Elf64_Shdr symtab_hdr{};
    Elf64_Shdr strtab_hdr{};
    bool found_symtab = false;
    bool found_strtab = false;

    // 1. Try to find .symtab + .strtab
    for (size_t i = 0; i < shdrs.size(); ++i) {
        const auto& sh = shdrs[i];
        if (sh.sh_type == 2 /* SHT_SYMTAB */) {
            symtab_hdr = sh;
            found_symtab = true;
        } else if (sh.sh_type == 3 /* SHT_STRTAB */ &&
            i != ehdr.e_shstrndx) {
            strtab_hdr = sh;
            found_strtab = true;
        }
    }

    // 2. If not found, try .dynsym + .dynstr
    if (!found_symtab) {
        for (size_t i = 0; i < shdrs.size(); ++i) {
            const auto& sh = shdrs[i];
            if (sh.sh_type == 11 /* SHT_DYNSYM */) {
                symtab_hdr = sh;
                found_symtab = true;
            } else if (sh.sh_type == 3 /* SHT_STRTAB */ &&
                       i != ehdr.e_shstrndx &&
                       (symtab_hdr.sh_link == i)) {
                strtab_hdr = sh;
                found_strtab = true;
            }
        }
    }

    if (!found_symtab || !found_strtab) return;

    // Read symbols
    size_t num_symbols = symtab_hdr.sh_size / symtab_hdr.sh_entsize;
    file.seekg(symtab_hdr.sh_offset, std::ios::beg);
    std::vector<Elf64_Sym> symbols(num_symbols);
    file.read(reinterpret_cast<char*>(symbols.data()), symtab_hdr.sh_size);

    // Read string table for symbols
    std::vector<char> strtab(strtab_hdr.sh_size);
    file.seekg(strtab_hdr.sh_offset, std::ios::beg);
    file.read(strtab.data(), strtab.size());

    // Populate symbols
    for (const auto& sym : symbols) {
        if (sym.st_name >= strtab.size()) continue;
        Symbol s;
        s.name = std::string(&strtab[sym.st_name]);
        s.address = sym.st_value;
        s.size = sym.st_size;
        s.type = static_cast<SymbolType>(sym.st_info & 0x0F);
        _symbols.push_back(s);
    }
}

} // namespace minielf