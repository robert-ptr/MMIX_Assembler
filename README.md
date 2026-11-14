# MMIX Assembler and Simulator
An implementation of an assembler and virtual machine for the MMIX architecture, designed to assemble and execute programs written in the MMIX assembly language as described in Donald Knuth's "The Art of Computer Programming".

Originally written for a university course on computer architecture.

## Overview

MMIX is a 64-bit RISC architecture designed by Donald Knuth as a replacement for the MIX architecture used in earlier editions of "The Art of Computer Programming". This project provides:

1. An **assembler** that translates MMIX assembly code into executable binary format
2. A **simulator** that executes the assembled MMIX programs

## Features

### Assembler

- Assembles all 256 MMIX opcodes and common pseudo-instructions
- Directives: `IS`, `GREG`, `LOC`, `BYTE`, `WYDE`, `TETRA`, `OCTA`, `PREFIX`
- Label resolution, symbol table, prefixes and local labels

### Simulator

- 256 general and 32 special registers
- Sparse hash-table memory covering the 2^64-byte address space
- Integer, logical, shift, compare, branch, load/store and bit-matrix (`MOR`/`MXOR`) instructions

### Not implemented

- Floating-point instructions, `TRAP`/`TRIP` handling, the register stack (`PUSHJ`/`POP`/`SAVE`/`UNSAVE`), and interrupt handling

## Building the Project

### Prerequisites

- CMake (3.10 or higher)
- C compiler with C99 support (e.g., GCC or Clang)

### Building with CMake

To ensure a reliable, clean, and isolated build environment, configure and build the project from the root directory:

```bash
cmake -B build
cmake --build build
```

This compiles both the assembler and simulator targets:
- Assembler executable: `build/mmix_assembler`
- Simulator / VM executable: `build/mmix_sim`

To build only the simulator (VM), you can specify the target:

```bash
cmake --build build --target mmix_sim
```

## Usage

### Assembling a Program

```bash
./build/mmix_assembler input_file [output_file]
```

If no output file is specified, the default output file is `./output`.

### Running a Program

```bash
./build/mmix_sim [options] input_file
```

## Example Programs

The `tests` directory contains several example MMIX programs, including:

- Basic examples demonstrating MMIX syntax and features
- Implementation of common algorithms
- More complex programs like an elevator simulation
- An implementation of a simplified MMIX Assembler in MMIX

## Project Structure

- `src/`: Source code for the assembler and simulator
  - `main.c`: Entry point for the application
  - `scanner.c/h`: Lexical analysis of MMIX assembly code
  - `parser.c/h`: Parsing and code generation
  - `vm.c/h`: Virtual machine implementation
  - `common.c/h`: Common utilities and data structures
  - `dynamic_list.c/h`: Generic dynamic array data structure
  - `table.c/h`: Symbol table implementation
  - `trie.c/h`: Trie data structure for instruction lookup
  - `debug.c/h`: Debugging utilities
- `tests/`: Example MMIX programs

## References

- Knuth, Donald E. "The Art of Computer Programming, Volume 1 Fascicle 1: A RISC Computer for the New Millennium",
- Knuth, Donald E. "MMIXware, A RISC Computer for the Third Millennium",
- MMIX documentation: http://mmix.cs.hm.edu/
