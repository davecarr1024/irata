# 🧾 Irata Project Status

_Last updated: July 16, 2025_

---

## ✅ Current State

The Irata simulated computer is transitioning from isolated component testing to full system integration. Memory mapping, control flow, and component communication are in place, enabling work on the fetch-decode-execute loop.

### 🧠 Instruction Set + Microcode
- Instruction set defined in `asm.yaml`
- Compiled to `InstructionSet` C++ structs with status-based conditional paths
- `InstructionMemory` holds microcode:
  - Indexed by opcode, step, and status
  - Drives controller's output lines per tick

### 🧠 Controller
- Drives control lines using `InstructionMemory` and `InstructionSet`
- Supports opcode + step-based resolution with optional conditionals
- Built with tick-phase integration
- Fully tested in isolation

### 🧰 Registers
- `Register` and `WordRegister` components
  - Bus-compatible
  - Support control lines for gated R/W
- `WordCounter` (extends `WordRegister`) supports incrementing
- `status_register` inputs available to control unit

### 🔌 Bus System
- `Bus<T>` shares signal between components
- Implements read/write logic with gate control
- Enforces propagation during tick phases

### 🧰 Component Model
- Tree of `Component`s with named children
- Tick phases:
  - `control → write → read → process → clear`
- Components can resolve and control arbitrary named lines (local or relative path)


### 🧩 Memory Subsystem

Memory is now a fully developed subsystem:

#### 📦 `Memory` Component
- Owns address decoder and memory regions
- Connects via buses for external CPU interaction
- Performs read/write routing by address

#### 📦 `Module` Interface
- Modules like `RAM`, `ROM`, `MMIO` must implement:
  - `read(uint16_t)`
  - Optional `write(uint16_t, uint8_t)`
  - `size()`, `can_write()`
- `Memory` owns regions and their lifecycle
- Full unit test suite for:
  - Region registration
  - Address decoding
  - Read/write behavior
  - Error conditions (unmapped, readonly, bus errors)

---

## 🔨 Upcoming: Canonical System Build

### 🧠 Planned Components
- `RAM`, `ROM` modules
- Static `Memory::canonical()` layout method
  - Attaches system RAM and cartridge ROM
- Static `Controller::from_instruction_set()` method
  - Loads microcode from `asm.yaml`
- `CPU` component
  - Instantiates and wires registers, buses, controller
- `IrataSystem` top-level component
  - Builds CPU + Memory with canonical layout
  - Accepts a cartridge (ROM)
  - Starts ticking from a known entry point

---

## 🧪 Tooling and End-to-End Testing

Once the canonical Irata computer is constructed, tooling will be built to support vertical integration tests:

### 🧰 Testing Framework
- `Cartridge` file format for memory-mapped ROMs
- CLI simulator runner: 
  - loads cartridge
  - simulates ticks until halt
- Assembler tool:
  - Converts `.asm` → `.rom`
- E2E test harness:
  - Takes test `.asm` + expected outcome
  - Verifies memory, registers, system state after halt
- Goal: validate all new behavior with end-to-end assembly-based tests

---

## 🔮 Vision

This is not just a simulator. It’s a full virtual machine with real development workflows.

### 🐞 Future Ideas
- **Interactive Debugger**
  - Step through ticks, watch registers/buses
  - Inspect memory and control flow live
- **Dev Tooling**
  - Language server for Irata assembly
  - Visualizer for bus + signal propagation
- **Self-hosting goal?**
  - Eventually write the assembler in Irata Assembly

---

## 🧠 Meta Principles

- Build one specific, realistic simulated computer
- Design for vertical slices over general abstractions
- Use real tick-based flow to model physical constraints
- Prioritize end-to-end correctness over raw performance
- Every instruction should be testable from cartridge → halt

---

## 🔜 Immediate Next Steps

- [ ] Implement `RAM` and `ROM` modules
- [ ] Build canonical `Memory` layout
- [ ] Implement static controller constructor from `asm.yaml`
- [ ] Create `CPU` component with full internal wiring
- [ ] Create `IrataSystem` root-level simulator
- [ ] Define cartridge format and boot-from-ROM flow
- [ ] Write assembler and run E2E test with `LDA #$42`

---
