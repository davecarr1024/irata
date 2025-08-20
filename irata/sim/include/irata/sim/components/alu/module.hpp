#pragma once

#include <irata/sim/components/component.hpp>
#include <irata/sim/hdl/alu_decl.hpp>

namespace irata::sim::components::alu {

class ALU;

class Module : public Component {
public:
  struct Result {
    static Result for_result(bool carry_in, Byte lhs, Byte rhs,
                             unsigned int result);

    const Byte value;
    const bool set_flags = true;
    const bool carry = false;
    const bool zero = false;
    const bool negative = false;
    const bool overflow = false;

    bool operator==(const Result &other) const;
    bool operator!=(const Result &other) const;
  };

  Module(hdl::AluOpcode opcode, std::string_view name, ALU &parent);

  hdl::ComponentType type() const override final;

  hdl::AluOpcode opcode() const;

  virtual Result apply(bool carry_in, Byte lhs, Byte rhs) = 0;

private:
  const hdl::AluOpcode opcode_;
};

std::ostream &operator<<(std::ostream &os, const Module::Result &result);

} // namespace irata::sim::components::alu
