#pragma once

#include <irata/sim/microcode/compiler/ir/instruction_set.hpp>
#include <irata/sim/microcode/compiler/passes/pass.hpp>

namespace irata::sim::microcode::compiler::passes {

class BusValidator : public Pass {
public:
  BusValidator() = default;
  virtual ~BusValidator() = default;

  ir::InstructionSet run(const ir::InstructionSet &instruction_set) override;
};

} // namespace irata::sim::microcode::compiler::passes
