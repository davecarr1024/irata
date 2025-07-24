#pragma once

#include <irata/sim/microcode/compiler/ir/instruction_set.hpp>
#include <irata/sim/microcode/compiler/stages/stage.hpp>

namespace irata::sim::microcode::compiler::stages {

class BusValidator : public Stage {
public:
  virtual ~BusValidator() = default;

  ir::InstructionSet run(const ir::InstructionSet &instruction_set) override;
};

} // namespace irata::sim::microcode::compiler::stages
