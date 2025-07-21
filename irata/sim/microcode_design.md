# Microcode Design

This document outlines the design goals, constraints, and architecture for the Irata microcode system, which defines the behavior of each instruction in terms of hardware-level control line sequences. The system is structured as a staged pipeline with clearly separated concerns and validation boundaries.



## Goals

The microcode system must:

- Define instruction behavior abstractly, without reference to timing or control lines.
- Compile declarative instruction definitions into control-line-based step sequences.
- Validate hardware interactions for safety, correctness, and consistency.
- Generate instruction memory data structures for execution in the `chuu` simulator.



## Architectural Layers

The system is organized into four distinct layers:

### 1. Hardware Model (Meta-HDL)

A declarative model of the hardware platform, including:

- All components (registers, ALUs, memory modules, buses, status lines).
- Valid control lines and access modes for each component.
- Bus structures and associated conflict rules.
- Legal status flags and branching conditions.

The hardware model provides named handles and ensures that all microcode operates only on declared and valid hardware features. It also supports static validation of hardware-level conflicts, such as illegal simultaneous bus usage.

### 2. Microcode Declaration DSL (Dataflow IR)

An abstract, high-level description of each instruction's behavior, expressed in terms of data movement, transformations, and control logic. This layer includes:

- Register-to-register or register-to-bus copies
- Arithmetic and logic operations
- Status flag updates
- Branching on flag values
- Label-based flow control

This declaration format does not reference steps or ticks. It is purely functional and represents the desired behavior without committing to any particular execution schedule.

### 3. Microcode Compiler (Scheduler / Linearizer)

A transformation layer that compiles dataflow declarations into executable microcode programs. Responsibilities include:

- Scheduling data operations into valid per-cycle groupings
- Resolving and sequencing control line activations
- Inserting tick boundaries where needed
- Evaluating resource constraints and dependencies
- Handling conditional execution and branching paths
- Validating hardware safety (e.g., bus conflicts, conflicting line usage)

This stage is where dataflow turns into execution steps. It may eventually support optimization passes, such as dead cycle elimination or micro-op compaction.

### 4. Microcode Table (Execution Representation)

The compiled result of the microcode compiler. This structure:

- Maps opcodes and status flags to per-cycle control line sequences
- Is directly consumed by the `InstructionMemory` and `Controller` in the simulator
- Has no dependency on the dataflow DSL or compiler internals

The microcode table defines the final, runtime behavior of the processor.



## Design Principles

- **Separation of Concerns**: Declaration, validation, and execution are fully decoupled.
- **Structure Over Convention**: All behavior must be explicitly defined through structured representations.
- **Fail Fast**: All invalid microcode, hardware references, or resource conflicts are reported at build time.
- **Traceability**: Each compiled micro-op is traceable to its declarative source.
- **Declarative First**: Instruction definitions should describe what happens, not how or when it happens.
- **Scheduler-Driven Timing**: The compiler is responsible for all timing and cycle sequencing decisions.



## Branching and Conditional Behavior

Branching is supported at the declarative level via conditional expressions and labeled flow control. Instructions may define alternative execution paths based on status flag values. The compiler expands these into appropriate instruction memory entries, keyed by opcode and status combinations.



## Deliverables

- Hardware Model definition structure and validator
- Dataflow DSL for microcode declaration
- Microcode compiler with scheduler and validator
- Instruction microcode table format
- Unit and integration tests for each layer
- Diagnostic tooling for validation and visualization (optional)



## Future Directions

- Microcode visualizer and flow debugger
- Graph-based hazard and dependency analysis
- Optimizing scheduler (dead cycle removal, parallelization)
- YAML or external formats for instruction import/export
- Pattern reuse and instruction templating
