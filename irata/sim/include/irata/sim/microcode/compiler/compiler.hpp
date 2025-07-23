#pragma once

#include <irata/sim/microcode/dsl/instruction_set.hpp>
#include <irata/sim/microcode/table/table.hpp>

namespace irata::sim::microcode::compiler {

void validate(const dsl::InstructionSet &instruction_set);

table::Table compile(const dsl::InstructionSet &instruction_set);

} // namespace irata::sim::microcode::compiler
