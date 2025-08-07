#pragma once

#include <irata/sim/components/alu/module.hpp>

namespace irata::sim::components::alu {

class Xor final : public Module {
public:
  explicit Xor(ALU &parent);

  Result apply(bool carry_in, Byte lhs, Byte rhs) const override final;
};

} // namespace irata::sim::components::alu
