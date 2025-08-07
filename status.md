# 🧾 Irata Project Status

_Last updated: August 6, 2025_

---

## 🎯 Major Milestone: Running Test Programs

Irata now supports a full end-to-end test flow:

1. Test programs are defined in `irata-assembly`.
2. Programs are assembled into bytecode.
3. Bytecode is executed by the simulator.
4. Final test result is determined by special instructions: `HLT` (halt) and `CRS` (crash).

This allows the Irata simulator to **run its own tests** — a major leap forward in making the system self-verifying. It enables vertical development with fewer brittle unit tests and more robust behavioral integration tests.

---

## ✅ Current State

### 🧠 Hardware Definition Language (HDL)

- The HDL fully describes the Irata computer’s architecture.
- It drives both the simulator structure and instruction compatibility.
- Simulator components are verified against the HDL before execution.
- This guarantees structural correctness between declared hardware and actual execution.

### 🖥️ Simulation

- The simulator includes:
  - Registers (A, X, Y, PC, etc.)
  - Memory (RAM + ROM)
  - Controller with instruction memory
- All components are wired through simulated buses.
- Instruction execution follows a **true fetch-decode-execute cycle**, with no shortcuts or magic behavior.
- Every CPU action is performed via real control lines.
- The entire system matches HDL and is tested using assembled programs.

### ⚙️ Microcode

- Instructions are defined using a domain-specific YAML-based microcode language.
- A multi-pass microcode compiler:
  - Validates instruction structure
  - Enforces invariants (e.g., unified fetch stage, proper step counter use)
  - Merges compatible steps for faster execution
- Validators and transformers are modular, allowing for easy extension and enforcement of new rules.

---

## 🛠️ In Progress

- **Test Program Infrastructure**:
  - Building out more test programs to validate instruction behavior
- **Upcoming Instructions**:
  - `CMP`: Enables value comparisons by updating CPU status flags (requires ALU)
  - `JNE`: Conditional branching based on status flags (already supported by microcode infrastructure)
- **Next Test Targets**:
  - Use `CMP + JNE` to create self-validating programs
  - Add more control flow and memory instructions
  - Scale up to full program-level correctness tests

---

## 🧠 Philosophy Snapshot

> "Declare the machine. Simulate the intent. Test every wire."

Irata is built with structural rigor at every level. Hardware declarations, simulation wiring, compiler logic, and execution behavior are all **explicit, testable, and real**.

---

🔥 **The machine runs. The tests are programs. There are no shortcuts.**
