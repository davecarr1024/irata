#include <irata/sim/components/alu/rotate_left.hpp>

namespace irata::sim::components::alu {

RotateLeft::RotateLeft(ALU &parent)
    : Module(hdl::AluOpcode::RotateLeft, "rotate_left", parent) {}

Module::Result RotateLeft::apply(bool carry_in, Byte lhs, Byte rhs) const {
  const auto value = (int(lhs.value()) << 1) | carry_in;
  const Byte value_byte = value;
  return {
      .value = value_byte,
      .carry = lhs.bit(7),
      .zero = value_byte == 0,
  };
}

} // namespace irata::sim::components::alu
