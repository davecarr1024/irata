# Irata Project Status

_Last updated: August 10, 2025_

---

## Major Milestone: Implementing Absolute Instructions

Irata now supports tests for absolute addressing instructions:

* The assembler supports the .byte directive for directly embedding data.
* Absolute modes for LDA, ADC, SBC, AND, ORA, and EOR are all implemented and tested using test programs.
* Register management: LDA, LDX, LDY, STA, STX, STY, TAX, TXA, TAY, TYA
* Increment: INC, DEC, INX, DEX, INY, DEY
* Stack operations for A: PHA, PLA

The development loop of testing through test programs has enabled quick progress for implementing and testing new instructions.

---

## Current State

### Hardware Definition Language (HDL)

- The HDL fully describes the Irata computer’s architecture.
- It drives both the simulator structure and instruction compatibility.
- Simulator components are verified against the HDL before execution.
- This guarantees structural correctness between declared hardware and actual execution.

### Simulation

- The simulator includes:
  - Registers (A, X, Y, PC, etc.)
  - Memory (RAM + ROM)
  - Controller with instruction memory
- All components are wired through simulated buses.
- Instruction execution follows a **true fetch-decode-execute cycle**, with no shortcuts or magic behavior.
- Every CPU action is performed via real control lines.
- The entire system matches HDL and is tested using assembled programs.

### Microcode

- Instructions are defined using a domain-specific YAML-based microcode language.
- A multi-pass microcode compiler:
  - Validates instruction structure
  - Enforces invariants (e.g., unified fetch stage, proper step counter use)
  - Merges compatible steps for faster execution
- Validators and transformers are modular, allowing for easy extension and enforcement of new rules.

---

## In Progress

* More stack management: PHX, PLX, PHY, PLY, PHP, PLP
* Subroutine support: JSR, RTS