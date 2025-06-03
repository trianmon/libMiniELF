#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <optional>
#include <unordered_map>

namespace minielf {

/**
 * @brief ELF64 file header structure.
 */
struct Elf64_Ehdr {
    unsigned char e_ident[16]; ///< ELF identification
    uint16_t e_type;           ///< Object file type
    uint16_t e_machine;        ///< Machine type
    uint32_t e_version;        ///< Object file version
    uint64_t e_entry;          ///< Entry point address
    uint64_t e_phoff;          ///< Program header offset
    uint64_t e_shoff;          ///< Section header offset
    uint32_t e_flags;          ///< Processor-specific flags
    uint16_t e_ehsize;         ///< ELF header size
    uint16_t e_phentsize;      ///< Program header entry size
    uint16_t e_phnum;          ///< Number of program header entries
    uint16_t e_shentsize;      ///< Section header entry size
    uint16_t e_shnum;          ///< Number of section header entries
    uint16_t e_shstrndx;       ///< Section name string table index
};

/**
 * @brief ELF64 section header structure.
 */
struct Elf64_Shdr {
    uint32_t sh_name;      ///< Section name (string tbl index)
    uint32_t sh_type;      ///< Section type
    uint64_t sh_flags;     ///< Section flags
    uint64_t sh_addr;      ///< Section virtual addr at execution
    uint64_t sh_offset;    ///< Section file offset
    uint64_t sh_size;      ///< Section size in bytes
    uint32_t sh_link;      ///< Link to another section
    uint32_t sh_info;      ///< Additional section information
    uint64_t sh_addralign; ///< Section alignment
    uint64_t sh_entsize;   ///< Entry size if section holds table
};

/**
 * @brief ELF64 symbol table entry structure.
 */
struct Elf64_Sym {
    uint32_t st_name;   ///< Symbol name (string tbl index)
    uint8_t  st_info;   ///< Symbol type and binding
    uint8_t  st_other;  ///< Symbol visibility
    uint16_t st_shndx;  ///< Section index
    uint64_t st_value;  ///< Symbol value
    uint64_t st_size;   ///< Symbol size
};

/**
 * @brief Symbol type enumeration.
 */
enum class SymbolType : uint8_t {
    NOTYPE  = 0, ///< No type specified (e.g., an absolute symbol)
    OBJECT  = 1, ///< Data object
    FUNC    = 2, ///< Function entry point
    SECTION = 3, ///< Symbol is associated with a section
    FILE    = 4, ///< Source file associated with the object file
    COMMON  = 5, ///< Uninitialized common block
    TLS     = 6, ///< Thread local data object
    UNKNOWN = 255///< Unknown type
};

/**
 * @brief Symbol information.
 */
struct Symbol {
    std::string name;   ///< Symbol name
    uint64_t address;   ///< Symbol address
    uint64_t size;      ///< Symbol size
    SymbolType type;    ///< Symbol type

    /**
     * @brief Check if the symbol is a function.
     * @return true if function, false otherwise.
     */
    bool isFunction() const {
        return type == SymbolType::FUNC;
    }
};

/**
 * @brief Section information.
 */
struct Section {
    std::string name;   ///< Section name
    uint64_t address;   ///< Section address
    uint64_t size;      ///< Section size
};

/**
 * @brief Metadata for the ELF file.
 */
struct ElfMetadata {
    uint16_t type = 0;
    uint16_t machine = 0;
    uint32_t version = 0;
    uint64_t entry = 0;
    uint32_t flags = 0;
};

/**
 * @brief Minimal ELF file parser and accessor.
 */
class MiniELF {
public:
    /**
     * @brief Construct a MiniELF object and parse the ELF file.
     * @param filepath Path to the ELF file.
     */
    explicit MiniELF(const std::string& filepath);

    /**
     * @brief Check if the ELF file was parsed successfully.
     * @return true if valid, false otherwise.
     */
    bool isValid() const;

    /**
     * @brief Get the list of ELF sections.
     * @return Vector of Section objects.
     */
    std::vector<Section> getSections() const;

    /**
     * @brief Get the list of ELF symbols.
     * @return Vector of Symbol objects.
     */
    std::vector<Symbol> getSymbols() const;

    /**
     * @brief Find a symbol by its address.
     * @param addr Address to search for.
     * @return Pointer to Symbol if found, nullptr otherwise.
     */
    const Symbol* getSymbolByAddress(uint64_t addr) const;

    /**
     * @brief Find a symbol by its name.
     * @param name Name of the symbol to search for.
     * @return Pointer to Symbol if found, nullptr otherwise.
     */
    const Symbol* getSymbolByName(const std::string& name) const;

    /**
     * @brief Find the nearest symbol with address <= given address.
     * @param address The address to resolve.
     * @return Pointer to nearest Symbol if found, nullptr otherwise.
     */
    const Symbol* getNearestSymbol(uint64_t address) const;

    /**
     * @brief Get a section by its address.
     * @param addr Address of the section to find.
     * @return Pointer to Section if found, nullptr otherwise.
     */
    const Section* getSectionByAddress(uint64_t addr) const;

    /**
     * @brief Get a section by its name.
     * @param name Name of the section to search for.
     * @return Pointer to Section if found, nullptr otherwise.
     */
    const Section* getSectionByName(const std::string& name) const;

    /**
     * @brief Get the metadata of the ELF file.
     * @return ElfMetadata containing entry point, architecture, type, and flags.
     */
    ElfMetadata getMetadata() const;

    /**
     * @brief Get the last error message.
     * @return Last error message as a string.
     */
    std::string getLastError() const { return _lastError; }

private:
    std::string _filepath;           ///< Path to the ELF file
    bool _valid = false;             ///< ELF file validity flag
    std::vector<Section> _sections;  ///< Parsed sections
    std::vector<Symbol> _symbols;    ///< Parsed symbols
    Elf64_Ehdr _elfHeader{};         ///< ELF header structure
    std::string _lastError;          ///< Last error message

    /**
     * @brief Parse the ELF file and populate sections and symbols.
     */
    void parse();

    /**
     * @brief Set the last error message.
     * @param msg Error message to set.
     */
    void setError(const std::string& msg) { _lastError = msg; }

    /**
     * @brief Parse symbols from the ELF file.
     * @param file      Input file stream (already open).
     * @param shstrtab  Section header string table.
     * @param shdrs     Section headers.
     * @param ehdr      ELF header.
     */
    void parseSymbols(std::ifstream& file, const std::vector<char>& shstrtab,
                      const std::vector<Elf64_Shdr>& shdrs, const Elf64_Ehdr& ehdr);

    mutable std::unordered_map<std::string, const Symbol*> _symbolByName;
    mutable std::vector<const Symbol*> _symbolsSortedByAddr;
    mutable std::vector<const Section*> _sectionsSortedByAddr;
    mutable std::unordered_map<std::string, const Section*> _sectionByName;
    mutable bool _lookupBuilt = false;

    /**
     * @brief Build lookups for symbols and sections.
     * This is called lazily to avoid unnecessary overhead if not needed.
     */
    void buildLookups() const;
};

} // namespace minielf