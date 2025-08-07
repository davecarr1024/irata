#include <irata/sim/components/alu/alu.hpp>
#include <irata/sim/components/alu/module.hpp>

namespace irata::sim::components::alu {

Module::Module(hdl::AluOpcode opcode, std::string_view name, ALU &parent)
    : Component(name, &parent), opcode_(opcode) {}

hdl::ComponentType Module::type() const {
  return hdl::ComponentType::AluModule;
}

hdl::AluOpcode Module::opcode() const { return opcode_; }

Module::Result Module::Result::for_result(bool carry_in, Byte lhs, Byte rhs,
                                          unsigned int result) {
  // Value is truncated from result.
  const Byte value = Byte::from_unsigned(result);
  return {
      .value = value,
      // Carry is set if result is greater than 0xFF.
      .carry = result > 0xFF,
      // Zero is set if the result is zero.
      .zero = value == 0,
      // Negative is set if the most significant bit is set.
      .negative = value.bit(7),
      // Overflow is set if the sign of the result is different from the
      // sign of the operands.
      .overflow = ((~(lhs.value() ^ rhs.value()) &
                    (lhs.value() ^ value.value()) & 0x80) != 0),
  };
}

bool Module::Result::operator==(const Result &other) const {
  return value == other.value && carry == other.carry && zero == other.zero &&
         negative == other.negative && overflow == other.overflow;
}

bool Module::Result::operator!=(const Result &other) const {
  return !(*this == other);
}

std::ostream &operator<<(std::ostream &os, const Module::Result &result) {
  return os << "Alu.Module.Result(value = " << result.value
            << ", carry = " << result.carry << ", zero = " << result.zero
            << ", negative = " << result.negative
            << ", overflow = " << result.overflow << ")";
}

} // namespace irata::sim::components::alu
