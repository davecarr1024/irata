#include <irata/sim/components/alu/or.hpp>

namespace irata::sim::components::alu {

Or::Or(ALU &parent) : Module(hdl::AluOpcode::Or, "or", parent) {}

Module::Result Or::apply(bool carry_in, Byte lhs, Byte rhs) const {
  return Module::Result::for_result(carry_in, lhs, rhs,
                                    int(lhs.value()) | int(rhs.value()));
}

} // namespace irata::sim::components::alu
