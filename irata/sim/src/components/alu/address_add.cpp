#include <irata/sim/components/alu/address_add.hpp>

namespace irata::sim::components::alu {

AddressAdd::AddressAdd(ALU &parent)
    : Module(hdl::AluOpcode::AddressAdd, "address_add", parent),
      carry_("carry", this) {}

Module::Result AddressAdd::apply(bool carry_in, Byte lhs, Byte rhs) {
  uint16_t value = uint16_t(lhs) + uint16_t(rhs);
  set_carry(value > 0xFF);
  return Result{
      .value = value,
      .set_flags = false,
  };
}

bool AddressAdd::carry() const { return carry_.value(); }
void AddressAdd::set_carry(bool value) { carry_.set_value(value); }
Status &AddressAdd::carry_status() { return carry_; }
const Status &AddressAdd::carry_status() const { return carry_; }

} // namespace irata::sim::components::alu
