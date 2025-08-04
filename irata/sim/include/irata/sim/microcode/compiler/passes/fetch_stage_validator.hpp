#pragma once

#include <irata/sim/microcode/compiler/passes/pass.hpp>

namespace irata::sim::microcode::compiler::passes {

// Validates that the fetch stage steps in all instructions are the same.
class FetchStageValidator final : public Pass {
public:
  ir::InstructionSet
  run(const ir::InstructionSet &instruction_set) override final;
};

} // namespace irata::sim::microcode::compiler::passes
