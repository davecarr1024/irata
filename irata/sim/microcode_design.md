# Microcode Design

This document outlines the design goals, constraints, and architecture for the Irata microcode system, which defines the behavior of each instruction in terms of hardware-level control line sequences. The system is structured as a staged pipeline with clearly separated concerns and validation boundaries.

## Goals

The microcode system must:

- Define instruction behavior abstractly, without reference to timing or control lines.
- Compile declarative instruction definitions into control-line-based step sequences.
- Validate hardware interactions for safety, correctness, and consistency.
- Generate instruction memory data structures for execution in the `chuu` simulator.
- Support expressive, reusable, and composable microcode declarations.

## Architectural Layers

The system is organized into four distinct layers:

### 1. Hardware Model (Meta-HDL)

A declarative, structured model of the hardware platform, including:

- All components (registers, ALUs, memory modules, buses, status lines).
- Valid control lines and access modes for each component.
- Bus structures and conflict rules for shared resources.
- Legal status flags and branching conditions.

The hardware model provides named handles and a tree of hardware declarations. All microcode must reference only declared hardware. This enables static validation of hardware-level misuse and ensures simulation and compilation safety.

### 2. Microcode Declaration DSL (Dataflow IR)

An abstract intermediate representation of instruction behavior, referred to as the "spec" or dataflow IR. It is designed to be expressive and composable, and it defines a program as an unordered set of operations with dependencies. Key features include:

- Each operation represents exactly one clock cycle.
- Operations may have zero or more inputs and outputs.
- Inputs and outputs may be registers, memory, buses, or other operations.
- Operations may assert arbitrary sets of control lines.
- Status-aware operations may have conditional branches or alternatives.
- Programs are declarative and unordered; control flow and scheduling are not encoded.
- Programs may be nested or composed to allow for reuse of common microcode patterns.
- Flattening routines allow composed programs to be reduced to a linear form for compilation.

This layer serves as the implementation language for the Irata instruction set. Programs may be written manually or built via reusable functional building blocks.

### 3. Microcode Compiler (Scheduler / Linearizer)

This stage compiles declarative microcode programs into per-cycle execution steps suitable for building instruction memory. It performs:

- Topological sort of operations respecting data and control dependencies.
- Cycle-by-cycle grouping of non-conflicting operations.
- Static analysis and validation of control line assertions and hardware access.
- Handling of branching behavior and conditional expansions.
- Fan-in and fan-out resolution.
- Conflict resolution for buses, components, and shared resources.
- Generation of one or more execution traces for each instruction, depending on branching.

The compiler defines how declarative semantics map to clock-based execution. Future passes may include optimizations or parallelization transforms.

### 4. Microcode Table (Execution Representation)

The compiled result, suitable for use by the simulator. This table:

- Maps `(opcode, step, status)` tuples to sets of control lines.
- Is indexed by the controller to fetch the correct micro-operations during simulation.
- Contains no declarative or structural information.
- Is the final, authoritative source of CPU behavior at runtime.

## Design Principles

- **Separation of Concerns**: Declarations, validation, and compilation are distinct and independently testable.
- **Explicitness**: No implicit conventions; all behavior must be structurally defined.
- **Fail Fast**: Errors in hardware usage, conflicts, or undefined behavior are surfaced at build time.
- **Reusability**: Common operations and instruction behaviors can be factored and reused.
- **Traceability**: All compiled micro-operations are traceable back to their declarative sources.
- **Declarative First**: Instruction definitions express *what* should happen, not *how* or *when*.
- **Scheduler-Driven Execution**: Timing is determined by the compiler, not by declaration.

## Branching and Conditional Behavior

Operations may include status flag dependencies and conditional alternatives. The DSL supports branching logic within a declarative structure, allowing instructions to specify alternate paths depending on processor state. The compiler resolves these into multiple microcode table entries as needed, ensuring each condition is accounted for explicitly.

## Program Composition

Programs may be composed of smaller program fragments. These sub-programs may be reused or parameterized. Composition is recursive, and programs may be inputs or outputs of operations. The compiler supports flattening this structure into a dependency-aware DAG for scheduling and linearization.

## Deliverables

- HDL structure definition and validator
- Microcode spec DSL for declarative instruction behavior
- Compiler for transforming specs into executable microcode
- Instruction memory table format for use in simulation
- Unit and integration tests across all layers
- Optional tooling for visualization and debugging

## Future Directions

- Graph-based microcode visualizer and debugger
- Optimizing scheduler and instruction compactor
- External definition formats (e.g., YAML, TOML)
- Instruction templating and macro support
- Status-aware speculative execution or delayed branching
