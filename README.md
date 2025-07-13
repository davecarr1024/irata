# Irata

**Irata** is a handcrafted 8-bit virtual computer system, built from scratch with deliberate design and retro charm. Inspired by classic machines like the Atari 2600, Commodore 64, and NES, Irata is a full-stack simulation platform — complete with compiler, assembler, interpreter, and signal-level simulator — designed for exploration, clarity, and joy.

The name comes from "Atari" spelled backwards, pronounced "errata", reflecting its nostalgic inspiration and inverted, homebrew spirit.

---

## ✨ What Is Irata?

Irata is a complete, from-the-ground-up virtual computer, designed to be:

- **Understandable** – every layer is readable and teachable
- **Modular** – clean interfaces between components
- **Hackable** – meant for play, learning, and reimagination
- **Expressive** – favoring clarity and delight over raw performance

You can write programs in a custom C-like language, compile them into bytecode, run them in a fast interpreter, or simulate the hardware signals cycle-by-cycle.

---

## 🧱 Project Structure

| Module         | Description |
|----------------|-------------|
| `irata-c`      | High-level systems language compiler (C-like, statically typed) |
| `irata-asm`    | Shared bytecode and assembly interface for the Irata system |
| `irata-vm`     | Fast bytecode interpreter for running programs |
| `chuu`         | Tick-based, signal-level simulator of the Irata hardware |
| `irata-cart`   | Cartridge format and loader |
| `irata-test`   | Test harnesses and golden test validation |
| `irata-docs`   | Auto-generated reference documentation |
| `cmd/`         | CLI tools (`chuu`, `irc`, etc.) |

---

## 🔩 Core Components

### Irata-C
A small, statically typed systems language with:

- `u8`, `u16`, `bool`, and `struct` types
- Explicit memory and reference handling
- Braced syntax and function calls
- Designed to compile to Irata Assembly

### Irata Assembly
A 6502-inspired ISA with potential ergonomic extensions. It is:

- The compilation target of Irata-C
- A standalone low-level language
- The interface for tooling, debugging, and system code

### Irata Virtual Computer
A simulated hardware platform with:

- Memory-mapped I/O
- 64KB address space
- Cartridge-based program loading
- Optional graphics and peripheral expansions

---

## ⚙️ chuu: Signal-Level Simulator

**chuu** is the tick-based, clock-cycle-accurate simulator of Irata’s internals. Inspired by the `flip` project, it features:

- Real microcoded control lines
- Bus-based communication between components
- Register- and byte-level signal simulation
- Named component tree for inspection and debugging

Chuu is designed to:
- Run real programs, not toy snippets
- Mirror hardware accurately down to micro-op sequencing
- Make every internal signal visible and testable

---

## 🛠 Goals

- ✅ Define Irata-C MVP and backend
- ✅ Build fast VM interpreter for running bytecode
- ✅ Create signal-level simulation via `chuu`
- ✅ Support cartridge loading and memory-mapped devices
- ✅ Run full Irata-C programs on the virtual machine
- ⏳ Develop debugging tools and visual interfaces

---

## 🧪 Philosophy

> "Irata is a love letter to hardware, 90s anime, and the joy of making every layer of a system yourself."

The project values:
- Personal craftsmanship over public polish
- Clear, composable modules
- Versioned and inspectable artifacts
- Play, whimsy, and vertical feature slices

---

