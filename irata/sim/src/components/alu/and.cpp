#include <irata/sim/components/alu/and.hpp>

namespace irata::sim::components::alu {

And::And(ALU &parent) : Module(hdl::AluOpcode::And, "and", parent) {}

Module::Result And::apply(bool carry_in, Byte lhs, Byte rhs) const {
  return Module::Result::for_result(carry_in, lhs, rhs,
                                    int(lhs.value()) & int(rhs.value()));
}

} // namespace irata::sim::components::alu
