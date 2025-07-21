# Microcode Design

This document outlines the design goals and constraints for the Irata microcode system, which serves as the bridge between the instruction set defined in `irata-asm` and the low-level control operations executed by the simulator.

---

## Goals

The microcode system must:

- Build a structure consumable by the `InstructionMemory` module with minimal coupling.
- Implement the full instruction set defined in `irata-asm`.
- Be testable and debuggable independently of the simulator.

---

## Requirements

### ✅ Must-Haves

- **Microcode Table Generation**  
  A consistent and easily serializable structure mapping opcodes (or instruction names) to micro-op sequences.

- **Instruction Set Coverage**  
  The system must support every instruction defined in `irata-asm`, generating valid microcode for each.

- **Stable Interfaces**  
  Must integrate cleanly into the Irata system without tight coupling to implementation details of the CPU or memory.

---

### 🌟 Nice-to-Haves

- **Composable Micro-Op Building Blocks**  
  Ability to define instructions using reusable building blocks such as:
  - `Copy(Register, Bus)`
  - `Write(ControlLine)`
  - `TickCycle(...)`

- **Validation System**  
  Microcode should be validated at build time to ensure:
  - No interaction with non-existent hardware
  - No conflicting control line usage within a single cycle
  - Address bounds and read/write constraints are obeyed

- **Structured Microcode Representation**  
  Micro-ops per instruction should be stored in a structure that supports:
  - Per-cycle grouping (`std::vector<std::set<MicroOp>>`)
  - Resource usage tracking
  - Dependency analysis

- **Future Optimization Support**  
  The system should allow for future enhancements including:
  - Dead cycle elimination
  - Parallel operation scheduling
  - Automatic compaction / hazard analysis

- **Diagnostic Output (Optional)**  
  Tools for dumping or visualizing microcode as text.

---

## Design Principles

- **Fail Fast**: Validate microcode early and loudly — no silent fallbacks or undefined behavior.
- **Keep Tools Thin**: Build tools on top of shared libraries for maximum testability and reuse.
- **Structure Over Convention**: Prefer structured data formats over hand-coded logic where possible.
- **Explicit is Better Than Implicit**: No "magic" behavior; microcode operations should be transparent and traceable.
- **Modular and Layered**: Keep the microcode system independent from simulator internals, and avoid assumptions about downstream timing or scheduling.

---

## Deliverables

- `MicroOp` enum and data structures
- `MicrocodeBuilder` class for composing instructions
- `InstructionMicrocodeTable` type (e.g., `std::map<Opcode, MicrocodeProgram>`)
- Validation and diagnostic tooling
- Unit tests for all major components

---

## Future Directions

- Graph-based dependency tracking
- Microcode visualizer
- Integration with a `microcode.yaml` format for data-driven instruction definitions
- Microcode compiler/optimizer

