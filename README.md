# MMIX Assembler and Simulator
An implementation of an assembler and virtual machine for the MMIX architecture, designed to assemble and execute programs written in the MMIX assembly language as described in Donald Knuth's "The Art of Computer Programming".

## Overview

MMIX is a 64-bit RISC architecture designed by Donald Knuth as a replacement for the MIX architecture used in earlier editions of "The Art of Computer Programming". This project provides:

1. An **assembler** that translates MMIX assembly code into executable binary format
2. A **simulator** that executes the assembled MMIX programs

## Project Status 🚧
This project is currently **unfinished** and is a **work in progress**. While the core functionality is implemented, it requires further development, testing, and refinement.

### What’s Left to Do
- **Testing**: The project needs more rigorous testing.
- **Documentation**: The codebase requires better documentation and comments.
- **Simulator**: The current version of the simulator implements the minimum of features needed to execute the binary code. I'm planning to develop the simulator into a proper virtual machine in the future.

### Roadmap

## Features

- Full implementation of the MMIX instruction set (256 opcodes) (WIP)
- Support for all MMIX registers (256 general registers and 32 special registers)
- Assembler directives (`IS`, `LOC`, `BYTE`, `WYDE`, `TETRA`, `OCTA`, etc.)
- Label resolution and symbol table management
- Virtual memory implementation using hash tables to simulate the 2^64 byte address space
- Command-line options for debugging and tracing program execution (WIP)

## Building the Project

### Assembler

```bash
cd src
make -f Makefile_assembler
```

### Simulator

```bash
cd src
make -f Makefile_simulator
```

## Usage

### Assembling a Program

```bash
./mmix_assembler input_file [output_file]
```

If no output file is specified, the default output file is `./output`.

### Running a Program

```bash
./mmix_sim [options] input_file
```

### Command Line Options

- `-s`: Display running time statistics when the program ends
- `-P`: Show a profile of how often each instruction was executed
- `-v`: Verbose mode, showing details about simulator operations
- `-t<n>`: Trace each instruction the first n times it is performed
- `-i`: Run simulator in interactive mode

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
  - `table.c/h`: Symbol table implementation
  - `trie.c/h`: Trie data structure for instruction lookup
  - `debug.c/h`: Debugging utilities
- `tests/`: Example MMIX programs

## References

- Knuth, Donald E. "The Art of Computer Programming, Volume 1 Fascicle 1: A RISC Computer for the New Millennium",
- Knuth, Donald E. "MMIXware, A RISC Computer for the Third Millennium",
- MMIX documentation: http://mmix.cs.hm.edu/
