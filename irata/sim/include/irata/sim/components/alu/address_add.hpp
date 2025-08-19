#pragma once

#include <irata/sim/components/alu/module.hpp>

namespace irata::sim::components::alu {

  // A specialized bytewise add that 
class AddressAdd final : public Module {
public:
  explicit AddressAdd(ALU &parent);

  Result apply(bool carry_in, Byte lhs, Byte rhs) const override final;
};

} // namespace irata::sim::components::alu
