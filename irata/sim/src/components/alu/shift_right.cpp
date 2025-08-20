#include <irata/sim/components/alu/shift_right.hpp>

namespace irata::sim::components::alu {

ShiftRight::ShiftRight(ALU &parent)
    : Module(hdl::AluOpcode::ShiftRight, "shift_right", parent) {}

Module::Result ShiftRight::apply(bool carry_in, Byte lhs, Byte rhs) {
  const auto value = int(lhs.value()) >> 1;
  const Byte value_byte = value;
  return {
      .value = value_byte,
      .carry = lhs.bit(0),
      .zero = value_byte == 0,
  };
}

} // namespace irata::sim::components::alu
