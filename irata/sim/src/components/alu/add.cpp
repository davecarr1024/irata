#include <irata/sim/components/alu/add.hpp>

namespace irata::sim::components::alu {

Add::Add(ALU &parent) : Module(hdl::AluOpcode::Add, "add", parent) {}

Module::Result Add::apply(bool carry_in, Byte lhs, Byte rhs) {
  uint16_t result = lhs.unsigned_value() + rhs.unsigned_value() + carry_in;
  return Result::for_result(carry_in, lhs, rhs, result);
}

} // namespace irata::sim::components::alu
