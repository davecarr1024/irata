#include <irata/sim/components/alu/shift_left.hpp>

namespace irata::sim::components::alu {

ShiftLeft::ShiftLeft(ALU &parent)
    : Module(hdl::AluOpcode::ShiftLeft, "shift_left", parent) {}

Module::Result ShiftLeft::apply(bool carry_in, Byte lhs, Byte rhs) const {
  const auto value = int(lhs.value()) << 1;
  const Byte value_byte = value;
  return {
      .value = value_byte,
      .carry = lhs.bit(7),
      .zero = value_byte == 0,
  };
}

} // namespace irata::sim::components::alu
