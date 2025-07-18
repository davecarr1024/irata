# 🧾 Irata Project Status

_Last updated: July 17, 2025_

---

## ✅ Current State

The Irata simulated computer system continues to advance with key subsystems implemented, tested, and integrated. The project has moved from basic components to early CPU structure and now toward memory mapping.

### 🧠 Instruction Set + Microcode
- Instruction set defined in `asm.yaml`, converted to `Instruction` structs in C++.
- `InstructionSet` maps opcodes to control sequences, with status-based conditional paths.
- `InstructionMemory` holds microcode:
  - Addressable by opcode, step, and status mask
  - Used directly by the `Controller`

### 🧠 Controller
- `Controller` component drives control line assertions via resolved paths.
- Supports both absolute and prefix-relative addressing.
- Tested end-to-end with `InstructionMemory`, registers, and status inputs.

### 🧰 Registers & Word Registers
- `Register` and `WordRegister` both support:
  - Optional connection to bus(es)
  - Read/write via control lines
  - Reset behavior
- `WordRegister` contains `.high` and `.low` sub-registers
- `WordCounter` extends `WordRegister` with increment control
- Fully tested with GTest

### 🔌 Bus System
- `Bus<T>` supports shared signal propagation with read/write gate control
- Registers and devices connect via control lines and buses
- Tick phases govern signal flow and update order

### 🧰 Component Model
- Hierarchical `Component` tree with named children
- Supports `tick()` with defined phases (`control → write → read → process → clear`)
- Control/status lines resolved by absolute or relative path
- Logging and tick phase state encapsulated internally

---

## 🧩 Memory Mapping (New Design)

Memory is being designed as a separate subsystem outside of `/cpu`, composed of:

### 📦 `Memory` Component
- Owns the address decoder and all memory modules
- Exposed to the CPU via external buses and read/write lines

### 🧰 `Module` Interface
- Abstract interface for memory-mapped devices:
  - `uint8_t read(uint16_t address)`
  - Optional `void write(uint16_t address, uint8_t value)`
  - `size()` must return a power-of-two ≤ 2¹⁶
- Modules can be RAM, ROM, MMIO, etc.
- May or may not inherit from `Component`

### 🧠 `AddressDecoder`
- Routes read/write controls and address bits to the correct module
- Maps address ranges via offset and size (must not overlap)
- Computes address masking/shifting internally
- Validates module fit within 16-bit address space
- Forwards requests by computing in-module address and dispatching

### Design Tradeoffs
- Not simulating real logic gates (no AND trees for decoder output lines)
- Module interaction via C++ method calls, not buses — for now
- This simplifies correctness and composability without going full circuit sim

---

## 🔍 Open Design Questions

### 1. **ALU Design**
- Should the ALU be a full subsystem like the controller/memory?
- How to express operations (control lines? internal opcodes?)

---

## 🔜 Next Steps

- [ ] Implement `components/memory/memory.hpp` and `module.hpp`
- [ ] Write `AddressDecoder` with overlap checking and dispatch logic
- [ ] Create RAM/ROM module implementations
- [ ] Build minimal ALU or temporary register-based placeholder
- [ ] Finalize `CPU` component wiring registers + controller
- [ ] End-to-end test: `LDA #$42` or similar instruction

---

## 🌱 Guiding Principles

- Build **one concrete simulated machine**, not a general framework
- Emulate physical signal flow via **tick phases**
- Favor **end-to-end vertical slices** with full integration tests
- Avoid premature abstraction — **build what you need**
- Control and status lines are real — **topology matters**
- Memory-mapped devices should feel like **real hardware**

---

## 🧠 Meta

- Inspired by Ben Eater’s breadboard computer — but cleaner, faster, and easier to debug.
- Flip3 may one day be a full circuit simulation, but Irata focuses on component-level realism with clear interfaces and behavior.

