# Overview

Irata is a handcrafted 8-bit virtual computer system inspired by retro hardware like the Atari 2600. It provides a complete vertical stack for understanding low-level computing:

- A custom assembly language with 6502-inspired instruction set
- An assembler that converts assembly programs to executable cartridges
- A hardware simulator that executes programs at the signal level
- A microcode system defining instruction behavior through control line sequences

The project emphasizes understanding over performance, making every instruction, signal, and clock cycle inspectable. It supports writing, assembling, loading, and executing real programs end-to-end on this virtual platform.

# User Preferences

Preferred communication style: Simple, everyday language.

# System Architecture

## Build System

**CMake-based modular architecture**: The project uses CMake 3.15+ with C++17 as the standard. Build configuration is divided into multiple subdirectories (`common`, `asm`, `sim`, `assembler`, `test`) allowing independent compilation of components. Two build variants exist:
- `build/` - with Address Sanitizer enabled for memory debugging
- `build_nosani/` - without sanitizers for production

**Rationale**: Modular CMake structure allows vertical slices of the system to be modified together as a unit, enabling rapid feature development. Separate build directories support both debugging and performance testing workflows.

## Core Components

### Common Library (`common/`)

**Primitive type system**: Provides fundamental types (`Byte`, `Word`) and string utilities that form the foundation for all other components. These types enforce hardware-accurate semantics.

**Rationale**: Centralizing primitive types ensures consistent behavior across assembler, simulator, and instruction definitions. Hardware-specific types prevent accidental misuse of standard integers.

### Assembly Definition (`asm/`)

**Data-driven instruction set**: Instructions are defined in `asm.yaml` with a JSON schema for validation. A Python code generator (`generate_cpp.py`) transforms the YAML into C++ source code that populates instruction tables.

**Rationale**: Separating instruction definitions from implementation code allows the instruction set to evolve independently. Code generation eliminates manual synchronization errors between documentation and implementation.

**Instruction properties**:
- Mnemonic name
- 8-bit opcode
- Addressing mode (immediate, absolute, zero page variants, etc.)
- Human-readable description

### Simulator (`sim/`)

**Signal-level hardware simulation**: Implements a virtual computer with components including:
- Registers (A, X, Y, PC, SP, status flags)
- Memory (RAM + ROM)
- ALU for arithmetic operations
- Bus system for component communication
- Microcode engine for instruction execution

**Microcode architecture** (documented in `microcode_design.md`):
1. Hardware model layer - Declarative component definitions
2. Dataflow IR - Abstract instruction behavior specifications
3. Compiler/scheduler - Converts dataflow to cycle-by-cycle control sequences
4. Execution engine - Interprets control sequences at runtime

**Rationale**: Multi-layer microcode design separates concerns: hardware declarations ensure validity, abstract IR enables expressiveness, and compilation handles timing/scheduling. This makes instruction implementation more maintainable than hand-coded sequences.

### Assembler (`assembler/`)

**Two-pass assembly process**: Parses assembly text files, resolves labels, encodes instructions and operands, and produces binary cartridge files. Integrates with the instruction table from the `asm` library.

**Rationale**: Standard two-pass design handles forward references while maintaining simplicity. Cartridge output format provides a self-contained executable for the simulator.

### Test Infrastructure (`test/`)

**End-to-end validation**: Assembly programs (`.asm` files) are automatically assembled into cartridges and executed by the simulator. CMake orchestrates the build-test-run pipeline. Each instruction has dedicated test coverage.

**Rationale**: Testing real assembly programs validates the entire stack (assembler + simulator + microcode) together, catching integration issues that unit tests miss.

## Data Flow

1. **Instruction definition**: YAML → Python generator → C++ instruction table
2. **Program creation**: Assembly source → Assembler → Binary cartridge
3. **Execution**: Cartridge → Simulator → Microcode interpreter → Hardware components
4. **Validation**: Test suite runs cartridges and verifies expected behavior

## Technology Stack

- **Language**: C++17
- **Build**: CMake 3.15+, Clang 14.0.6
- **Testing**: Google Test/Google Mock
- **Code generation**: Python 3.12 with PyYAML and jsonschema
- **Build environment**: Nix (evident from store paths)

# External Dependencies

## Testing Framework

**Google Test/Google Mock**: Integrated as a Git submodule in `extern/googletest/`. Provides unit testing and mocking capabilities across all components.

## Code Generation Tools

**Python 3.12**: Used for instruction set code generation
- **PyYAML**: Parses instruction definitions from YAML
- **jsonschema**: Validates instruction definitions against schema

## Build Toolchain

**Nix package manager**: Provides hermetic build environment with pinned compiler and tool versions. Ensures reproducible builds across different machines.

**LLVM toolchain**: Clang 14.0.6 compiler with LLVM binutils for linking and archiving.