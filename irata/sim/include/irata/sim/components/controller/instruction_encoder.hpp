#pragma once

#include <irata/sim/components/controller/control_encoder.hpp>
#include <irata/sim/components/controller/status_encoder.hpp>

namespace irata::sim::components::controller {

class InstructionEncoder {
public:
  explicit InstructionEncoder(const microcode::InstructionSet &instruction_set);

private:
  const ControlEncoder control_encoder_;
  const StatusEncoder status_encoder_;
};

} // namespace irata::sim::components::controller