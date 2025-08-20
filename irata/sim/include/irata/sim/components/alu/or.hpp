#pragma once

#include <irata/sim/components/alu/module.hpp>

namespace irata::sim::components::alu {

class Or final : public Module {
public:
  explicit Or(ALU &parent);

  Result apply(bool carry_in, Byte lhs, Byte rhs) override final;
};

} // namespace irata::sim::components::alu
