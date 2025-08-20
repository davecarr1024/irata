#include <irata/sim/components/alu/xor.hpp>

namespace irata::sim::components::alu {

Xor::Xor(ALU &parent) : Module(hdl::AluOpcode::Xor, "xor", parent) {}

Module::Result Xor::apply(bool carry_in, Byte lhs, Byte rhs) {
  return Module::Result::for_result(carry_in, lhs, rhs,
                                    int(lhs.value()) ^ int(rhs.value()));
}

} // namespace irata::sim::components::alu
