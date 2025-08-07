#pragma once

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
    ModuleDecl(const AluDecl &parent, std::string_view name, uint8_t opcode);

    uint8_t opcode() const;

  private:
    const uint8_t opcode_;
  };

  AluDecl(const ComponentDecl &parent, const ByteBusDecl &data_bus);

  void verify(const components::Component *component) const override final;

  const std::set<std::unique_ptr<ModuleDecl>> &modules() const;

  uint8_t max_opcode() const;

  size_t num_opcode_controls() const;

  const std::vector<std::unique_ptr<ProcessControlDecl>> &
  opcode_controls() const;

  std::vector<const ProcessControlDecl *>
  opcode_controls_for_opcode(uint8_t opcode) const;

  const ConnectedByteRegisterDecl &lhs() const;

  const ConnectedByteRegisterDecl &rhs() const;

  const ConnectedByteRegisterDecl &result() const;

  const StatusDecl &carry() const;

  const StatusDecl &zero() const;

  const StatusDecl &negative() const;

  const StatusDecl &overflow() const;

  const StatusDecl &half_carry() const;

private:
  std::set<std::unique_ptr<ModuleDecl>> modules_;
  const uint8_t max_opcode_;
  const std::vector<std::unique_ptr<ProcessControlDecl>> opcode_controls_;
  const ConnectedByteRegisterDecl lhs_;
  const ConnectedByteRegisterDecl rhs_;
  const ConnectedByteRegisterDecl result_;
  const StatusDecl carry_;
  const StatusDecl zero_;
  const StatusDecl negative_;
  const StatusDecl overflow_;
  const StatusDecl half_carry_;
};

} // namespace irata::sim::hdl
