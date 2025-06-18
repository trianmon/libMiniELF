cmake_minimum_required(VERSION 3.16)
project(libMiniELF VERSION 1.2.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Library
add_library(minielf STATIC
    src/MiniELF.cpp
)

# Headers
target_include_directories(minielf PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Example CLI
add_executable(dump_elf example/dump_elf.cpp)
target_link_libraries(dump_elf minielf)

# Build test ELF binary (tests/test)
add_executable(test_elf_file tests/test.c)
set_target_properties(test_elf_file PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

# Tests
option(MINIELF_BUILD_TESTS "Build tests for MiniELF" ON)

if(MINIELF_BUILD_TESTS)
    enable_testing()
    add_executable(test_minielf tests/test_minielf.cpp)
    target_link_libraries(test_minielf minielf)
    add_test(NAME test_minielf COMMAND test_minielf)
endif()

# Installation
install(TARGETS minielf
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)

install(DIRECTORY include/minielf DESTINATION include)