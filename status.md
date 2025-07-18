# 🧾 Irata Project Status

_Last updated: July 16, 2025_

---

## ✅ Current State

The Irata simulated computer system is progressing steadily, with the first full controller implementation now complete and fully tested.

### 🧠 Instruction Set + Microcode
- Instruction set defined via `asm.yaml` and exported to C++ as `Instruction` structs.
- Microcode paths and steps are defined separately in `InstructionSet`, supporting relative or absolute control/status paths.
- Status-based branching is supported per instruction, enabling conditional execution of control programs.
- `InstructionMemory` class implemented:
  - Encodes status bit permutations
  - Exposes addressable control sequences
  - Integrated with `Controller`

### 📦 Controller
- `Controller` component implemented and verified:
  - Fetches microcode from `InstructionMemory` based on opcode, step, and status values
  - Applies control line assertions via absolute or prefix-resolved paths
  - Supports relative addressing via a configurable `path_prefix`
- Thoroughly tested:
  - 6 end-to-end GTest cases:
    - Absolute + Relative addressing
    - With and without status conditions
    - Full integration with Registers and Status components
- ✅ First working one-step instruction with data movement verified (`a ← b`)

### 🔗 Encoders
- `StatusEncoder` working and tested.
- `InstructionMemory` currently assumes a 16x32 layout (opcode x step) with branching via status masks.
- Opcode, step, and status mapping is working — but final address scheme may still evolve.

### 🔌 Component Design
- Components use a tree-based hierarchy with absolute path addressing.
- Control/status lines are resolved via names like `/reg/write` or relative paths like `reg/write` with a prefix.
- All components respond to `tick()` cycles for state updates.
- Tick phases and logging now unified in base `Component` with a `Logger` helper class and overrideable phase hooks.

### 🧱 Buses
- Bus wiring verified:
  - `Bus` class for shared signal propagation
  - Registers and other devices connect via bus + gated control lines
- Components update state via `tick()` phases

### 🧰 Register + Counter
- `Register` and `Counter` components implemented and tested
- `Counter` supports increment and reset lines with override behavior
- `Register` supports read/write via control lines and bus connection

### 🧰 WordRegister
- ✅ Implemented and tested
- Composed of two `Register` subcomponents (`hi` + `lo`)
- Supports:
  - 16-bit bus connection via read/write lines
  - Optional byte-bus connection for subregister access
  - `reset` control line
- Tested behavior:
  - Read/write over word bus and byte bus
  - Reset behavior
  - Path-based control behavior matches top-level register model

### 🧮 WordCounter
- ✅ Implemented and tested
- Subclass of `WordRegister` with `increment` line
- Increments on tick when asserted unless reset is active
- Clean override priority: reset > increment > idle

---

## 🔍 Open Design Questions

### 1. **Instruction Memory Address Layout**
- Current guess: 8-bit opcode, 5-bit step, 3-bit status mask (?)
- Final layout TBD — should be abstracted into a `MicrocodeAddress`

### 2. **Control Path Resolution**
- Relative vs. absolute paths fully working
- Documented contract: control/status paths + prefix → must yield absolute `/...` path

### 3. **CPU as a Component**
- How to structure CPU internals:
  - Internal wires = bus
  - Controller provides brain
  - Registers, ALU, and memory connect via path + bus
- Clean interface TBD, but we're close

### 4. **Bus System Design**
- `Bus<T>` is working well as a shared signal
- Write gating and directionality implied by control signals
- May generalize later (byte, word, etc.)

### 5. **Memory Mapping / Decoding**
- Plan:
  - `AddressDecoder` observes bus + enables child components
  - Each RAM/ROM/MMIO region gets own `Component`
- Future idea: support for paging/MMC-like mappers

### 6. **Tick Phase Semantics**
- Current tick flow: `control → write → read → process → clear`
- Question: where should address decoding happen?
  - Idea: `tick_predecode()` to prep address decoder before write/read

### 7. **WordRegister Generalization**
- Fully supports modular hookup for word and byte buses
- Open: does it need config object for more flexible constructor?
- For now, works well as-is

---

## 🔜 Next Steps

- [ ] Finalize instruction memory address layout + `MicrocodeAddress`
- [ ] Build top-level `CPU` component:
  - Register wiring
  - ALU sketch
  - Controller hookup
- [ ] Write minimal ALU (even as a `Register` with op controls at first)
- [ ] Implement `MemoryComponent` (RAM or ROM)
- [ ] Implement `MemoryDecoder` to route global addresses to subcomponents
- [ ] Wire up a one-instruction CPU (e.g. `LDA #$42`)
- [ ] End-to-end test: opcode → controller → buses → registers → result

---

## 🌱 Guiding Principles

- Build **one real machine**, not a simulator framework
- Control lines are **physical**: wiring matters
- Favor **end-to-end vertical slices**
- Tests are the spec — every new component must be unit-testable
- Component trees, paths, and control wiring are **real topology**
- Avoid overengineering — build what’s needed, then refactor
