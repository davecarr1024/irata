#pragma once

#include <irata/sim/hdl/component_decl.hpp>
#include <irata/sim/hdl/control_decl.hpp>
#include <irata/sim/hdl/register_decl.hpp>
#include <irata/sim/hdl/status_decl.hpp>
#include <map>

namespace irata::sim::hdl {

// The status register holds the status flags of the CPU.
// It is connected to a set of incoming status lines and has a latch control to
// latch those incoming status lines into the register.
// It also has a set of outgoing status lines that are connected to the
// corresponding bits in the register. It is connected to the data bus to allow
// for interacting with the status register from the CPU.
class StatusRegisterDecl final
    : public ComponentWithParentDecl<ComponentType::StatusRegister>,
      public RegisterWithResetDecl,
      public RegisterWithByteBusDecl {
public:
  StatusRegisterDecl(const ComponentDecl &parent, const ByteBusDecl &bus,
                     const StatusDecl &carry_in, const StatusDecl &negative_in,
                     const StatusDecl &overflow_in, const StatusDecl &zero_in);

  void verify(const components::Component *component) const override final;

  const StatusDecl &carry_in() const;

  const StatusDecl &negative_in() const;

  const StatusDecl &overflow_in() const;

  const StatusDecl &zero_in() const;

  const StatusDecl &carry_out() const;

  const StatusDecl &negative_out() const;

  const StatusDecl &overflow_out() const;

  const StatusDecl &zero_out() const;

  const PersistentClearControlDecl &latch() const;

  const std::map<const StatusDecl *, int> &status_indices() const;

private:
  const StatusDecl &carry_in_;
  const StatusDecl &negative_in_;
  const StatusDecl &overflow_in_;
  const StatusDecl &zero_in_;
  const StatusDecl carry_out_;
  const StatusDecl negative_out_;
  const StatusDecl overflow_out_;
  const StatusDecl zero_out_;
  const PersistentClearControlDecl latch_;
  const std::map<const StatusDecl *, int> status_indices_;
};

} // namespace irata::sim::hdl
