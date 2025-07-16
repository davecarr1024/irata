# 🧾 Irata Project Status

_Last updated: July 15, 2025_

---

## ✅ Current State

The Irata simulated computer system is progressing well, with the following core structures in place:

### 🧠 Instruction Set + Microcode
- Instruction set defined via `asm.yaml` and exported to C++ as `Instruction` structs.
- Microcode paths and steps are defined separately in `instructions.cpp` with associated status flag conditions.
- `StatusEncoder` implemented and tested:
  - Encodes status conditions into 8-bit microcode address bits
  - Supports permutations for "don't care" paths
  - Fully tested with GTest

### 🔗 Encoders
- `StatusEncoder` working as part of the instruction memory address generator.
- Planned: Instruction address = `opcode | step | status` layout, not finalized yet.

### 🔌 Component Design
- CPU being reimagined as a pluggable `Component` with:
  - `DataBus<uint8_t>` (shared)
  - `AddressBus<uint16_t>` (shared)
  - Read/Write control lines
  - Implicit clock via shared `Tick()` phase
  - Component-local control lines resolved via prefix paths

### 🧱 Buses
- Address and data buses are separate physical entities.
- Components connect via shared `Bus<T>` instances gated by control lines.
- Future: generalize bus types with templated `Bus<T>` (byte, word, etc.)

### 🧰 WordRegister
- Design in progress:
  - Composed of two 8-bit subregisters: `.hi`, `.lo`
  - Connects to 16-bit `AddressBus` for PC-style use
  - Connects optionally to 8-bit `DataBus` for microcode-controlled loads
  - Increment and reset behavior customizable via control lines

### 🧩 Memory Mapping Model
- Moving away from flat `std::map<uint16_t, uint8_t>` memory (Flip-era)
- Designing realistic, address-decoded memory model:
  - Chips like RAM, ROM, IO are standalone `Component`s with buses and controls
  - A `MemoryDecoder` or `AddressDecoder` component will:
    - Observe address bus and control lines
    - Enable routing of read/write/data to a subcomponent if address range matches
    - Remap global address to local offset
  - Each memory chip (RAM, ROM, etc.) gets its own local view of memory space

---

## 🔍 Open Design Questions

### 1. **Instruction Memory Address Layout**
- How many bits for status vs step vs opcode?
- Should address layout be abstracted into a `MicrocodeAddress` struct?

### 2. **Control Path Resolution**
- Controllers must address internal control lines relative to CPU or coprocessor root
- Plan: support prefix pathing (`cpu.Registers.A.Enable`) for controller wiring

### 3. **CPU as a Component**
- What is the standard CPU interface?
  - Data/Address buses + control lines?
  - Externalized tick behavior?
- What’s the cleanest way to model internal logic + external interface?

### 4. **Bus System Design**
- Should `Bus<T>` be a simple shared pointer? ✅ Leaning yes
- Should buses support write-gating only, or also simulate contention/direction?
- Generalization: can `Bus<T>` work across byte, word, etc.?

### 5. **Memory Mapping / Decoding**
- Should memory routing be handled via a dedicated `MemoryDecoder`?
- How to cleanly express address range, address offsetting, and chip enable logic?
- Can MMIO/paging (like MMC3) be modeled as normal components hanging off decoder?

### 6. **Tick Phase Semantics**
- Current tick phases: `control → write → read → process → clear`
- Concern: when should memory routing (e.g. decoder logic) happen?
- Option: introduce `tick_predecode()` for signal preparation in address decoders
- Avoid modeling physical signal propagation — not worth the pain

### 7. **WordRegister Generalization**
- Only `PC` needs address + data bus connection
- Other word-wide registers (indirect pointers, temporary) don’t
- Plan: make bus/control hookups optional via configuration struct

---

## 🔜 Next Steps

- [ ] Finalize instruction memory address layout (status/step/opcode)
- [ ] Implement `MicrocodeAddress` encoder
- [ ] Finish `WordRegister` with `.lo`, `.hi`, and increment logic
- [ ] Build `MemoryDecoder` and minimal `RAMComponent`
- [ ] Wire up CPU to external bus and memory as a component
- [ ] Write end-to-end test that loads a value (`LDA #$42`) into memory

---

## 🌱 Guiding Principles

- Build one real machine — not a simulation framework
- Keep components modular, but don’t abstract prematurely
- Favor working vertical slices (load a value, tick it through the system)
- Treat control lines and buses as physical phenomena
- Embrace testability: all behavior should be observable and unit-testable
