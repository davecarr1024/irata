#include <irata/sim/components/alu/rotate_right.hpp>

namespace irata::sim::components::alu {

RotateRight::RotateRight(ALU &parent)
    : Module(hdl::AluOpcode::RotateRight, "rotate_right", parent) {}

Module::Result RotateRight::apply(bool carry_in, Byte lhs, Byte rhs) {
  const auto value = (int(lhs.value()) >> 1) | (carry_in << 7);
  const Byte value_byte = value;
  return {
      .value = value_byte,
      .carry = lhs.bit(0),
      .zero = value_byte == 0,
  };
}

} // namespace irata::sim::components::alu
