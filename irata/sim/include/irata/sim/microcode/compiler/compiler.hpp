#pragma once

#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <irata/sim/microcode/table/table.hpp>
#include <stdexcept>

namespace irata::sim::microcode::compiler {

// Validates the given instruction set.
// Validation checks include checks for correct bus usage and instruction
// variant complementarity.
void validate(const dsl::InstructionSet &instruction_set);

// Compiles the given instruction set into a microcode table.
// This includes optimization and validation.
// Optimization merges steps that are compatible, which can be done safely
// when steps contain controls that are consumed either simultaneously or
// in tick-phase order.
table::Table compile(const dsl::InstructionSet &instruction_set);

} // namespace irata::sim::microcode::compiler
