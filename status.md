# Irata Project Status

_Last updated: August 18, 2025_

---

## Major Milestone: Programs

```
ldx #$03
ldy #$04
jsr add_x_and_y
cmp #$07
jne fail

hlt

fail:
crs

add_x_and_y:
txa
sta $00
tya
adc $00
rts
```

Compiles, runs, and passes. This calculation of `3 + 4 = 7` sits on top of ~20k lines of code, a functional language, and a simulated computer. The instruction set is growing rapidly and currently sits at 63 with basic ALU, stack, and register operations with multiple addressing modes. 

Focused project planning has gotten to the point where vertical slices of the system can be easily modified together as one unit, allowing amazing feature velocity. I'm excited about how fast the project is moving and where it can go next. 

This is now about as far as flip got when it destabilized and bogged down, and irata shows no signs of stopping.

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

Irata assembly goals:

* Indexed addressing modes.
* More branching modes: BCS, BVS, BPL, etc.
* Setting Z/V status bits for most operations and not just ALU operations.
* Interrupts?

Since irata assembly is getting to a functional point and I'm able to write simple programs, I think it might also be time to think about some higher level goals, such as:

* Simple memory mapped devices such as a text display or controller input.
* A rough draft of the irata-c compiler.
* A debugging interface.