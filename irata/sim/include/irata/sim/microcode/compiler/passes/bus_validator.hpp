#pragma once

#include <irata/sim/microcode/compiler/ir/instruction_set.hpp>
#include <irata/sim/microcode/compiler/passes/pass.hpp>

namespace irata::sim::microcode::compiler::passes {

// A pass that validates that each bus is written by exactly one control and
// read by at least one control in each step that writes to or reads from that
// bus.
// Note that this allows for multiple buses to be written and read in the same
// step, as long as each bus is written by exactly one control and read by at
// least one control.
class BusValidator : public Pass {
public:
  ir::InstructionSet run(const ir::InstructionSet &instruction_set) override;
};

} // namespace irata::sim::microcode::compiler::passes
