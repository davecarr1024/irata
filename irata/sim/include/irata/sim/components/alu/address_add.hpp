#pragma once

#include <irata/sim/components/alu/module.hpp>
#include <irata/sim/components/status.hpp>

namespace irata::sim::components::alu {

// A specialized bytewise add that
class AddressAdd final : public Module {
public:
  explicit AddressAdd(ALU &parent);

  Result apply(bool carry_in, Byte lhs, Byte rhs) override final;

  bool carry() const;
  void set_carry(bool carry);
  const Status &carry_status() const;
  Status &carry_status();

private:
  Status carry_;
};

} // namespace irata::sim::components::alu
