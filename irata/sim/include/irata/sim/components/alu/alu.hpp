#pragma once

#include <irata/sim/components/component.hpp>
#include <irata/sim/components/register.hpp>
#include <irata/sim/components/status.hpp>
#include <irata/sim/hdl/alu_opcode.hpp>

namespace irata::sim::components::alu {

class Module;

class ALU final : public Component {
public:
  ALU(Component &parent, ByteBus &data_bus);

  hdl::ComponentType type() const override final;

  const std::vector<std::unique_ptr<Module>> &modules() const;

  const std::vector<std::unique_ptr<Control>> &opcode_controls() const;

  std::vector<Control *> opcode_controls_for_opcode(hdl::AluOpcode opcode);
  std::vector<const Control *>
  opcode_controls_for_opcode(hdl::AluOpcode opcode) const;

  hdl::AluOpcode opcode() const;
  void set_opcode(hdl::AluOpcode opcode);

  Module *module(hdl::AluOpcode opcode);
  const Module *module(hdl::AluOpcode opcode) const;
  Module *module();
  const Module *module() const;

  bool carry_in() const;
  void set_carry_in(bool value);
  Control &carry_in_control();
  const Control &carry_in_control() const;

  Byte lhs() const;
  void set_lhs(Byte value);
  Register &lhs_register();
  const Register &lhs_register() const;

  Byte rhs() const;
  void set_rhs(Byte value);
  Register &rhs_register();
  const Register &rhs_register() const;

  Byte result() const;
  void set_result(Byte value);
  Register &result_register();
  const Register &result_register() const;

  bool carry_out() const;
  void set_carry_out(bool value);
  Status &carry_out_status();
  const Status &carry_out_status() const;

  bool zero() const;
  void set_zero(bool value);
  Status &zero_status();
  const Status &zero_status() const;

  bool negative() const;
  void set_negative(bool value);
  Status &negative_status();
  const Status &negative_status() const;

  bool overflow() const;
  void set_overflow(bool value);
  Status &overflow_status();
  const Status &overflow_status() const;

protected:
  void tick_process(Logger &logger) override;

private:
  const std::vector<std::unique_ptr<Module>> modules_;
  const std::vector<std::unique_ptr<Control>> opcode_controls_;
  Register lhs_;
  Register rhs_;
  Register result_;
  Control carry_in_;
  Status carry_out_;
  Status zero_;
  Status negative_;
  Status overflow_;

  std::vector<int>
  opcode_control_indices_for_opcode(hdl::AluOpcode opcode) const;
};

} // namespace irata::sim::components::alu
