#pragma once

#include <irata/sim/hdl/alu_opcode.hpp>
#include <irata/sim/hdl/component_decl.hpp>
#include <irata/sim/hdl/control_decl.hpp>
#include <irata/sim/hdl/register_decl.hpp>
#include <irata/sim/hdl/status_decl.hpp>

#include <set>

namespace irata::sim::hdl {

// AluDecl is a declaration of an ALU component.
// This is used to verify that the ALU component exists in the component tree
// and has the correct properties.
// The ALU is a collection of modules, each of which implements a specific
// operation. The ALU is controlled by a set of opcode control lines which are
// encoded as a binary number based on the ALU opcode corresponding to each
// module.
class AluDecl final : public ComponentWithParentDecl<ComponentType::Alu> {
public:
  class ModuleDecl : public ComponentWithParentDecl<ComponentType::AluModule> {
  public:
    ModuleDecl(const AluDecl &parent, std::string_view name, AluOpcode opcode);

    AluOpcode opcode() const;

    void verify(const components::Component *component) const override final;

  private:
    const AluOpcode opcode_;
  };

  AluDecl(const ComponentDecl &parent, const ByteBusDecl &data_bus);

  void verify(const components::Component *component) const override final;

  const std::set<std::unique_ptr<ModuleDecl>> &modules() const;

  uint8_t max_opcode() const;

  size_t num_opcode_controls() const;

  const std::vector<std::unique_ptr<ProcessControlDecl>> &
  opcode_controls() const;

  std::vector<const ProcessControlDecl *>
  opcode_controls_for_opcode(AluOpcode opcode) const;

  const ConnectedByteRegisterDecl &lhs() const;

  const ConnectedByteRegisterDecl &rhs() const;

  const ConnectedByteRegisterDecl &result() const;

  const ProcessControlDecl &carry_in() const;

  const StatusDecl &carry_out() const;

  const StatusDecl &zero() const;

  const StatusDecl &negative() const;

  const StatusDecl &overflow() const;

private:
  std::set<std::unique_ptr<ModuleDecl>> modules_;
  const uint8_t max_opcode_;
  const std::vector<std::unique_ptr<ProcessControlDecl>> opcode_controls_;
  const ConnectedByteRegisterDecl lhs_;
  const ConnectedByteRegisterDecl rhs_;
  const ConnectedByteRegisterDecl result_;
  const ProcessControlDecl carry_in_;
  const StatusDecl carry_out_;
  const StatusDecl zero_;
  const StatusDecl negative_;
  const StatusDecl overflow_;
};

} // namespace irata::sim::hdl
