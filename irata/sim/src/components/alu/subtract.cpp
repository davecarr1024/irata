#include <irata/sim/components/alu/subtract.hpp>

namespace irata::sim::components::alu {

Subtract::Subtract(ALU &parent)
    : Module(hdl::AluOpcode::Subtract, "subtract", parent) {}

Module::Result Subtract::apply(bool carry_in, Byte lhs, Byte rhs) const {
  return Module::Result::for_result(carry_in, lhs, rhs,
                                    int(lhs.value()) - int(rhs.value()) -
                                        (1 - int(carry_in)));
}

} // namespace irata::sim::components::alu
