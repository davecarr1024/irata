#pragma once

#include <irata/sim/hdl/bus_decl.hpp>
#include <irata/sim/hdl/component_decl.hpp>
#include <irata/sim/hdl/register_decl.hpp>

namespace irata::sim::hdl {

// A controller is a component that executes instructions.
// It has a step counter and an opcode register, and it executes instructions
// from an instruction memory based on the opcode and step counter.
class ControllerDecl final
    : public ComponentWithParentDecl<ComponentType::Controller> {
public:
  ControllerDecl(const std::string &name, const ComponentDecl &parent,
                 const ByteBusDecl &data_bus);

  const ByteBusDecl &data_bus() const;

  const IncrementableConnectedByteRegisterDecl &step_counter() const;

  const ConnectedByteRegisterDecl &opcode() const;

  void verify(const components::Component *component) const override final;

private:
  const ByteBusDecl &data_bus_;
  const IncrementableConnectedByteRegisterDecl step_counter_;
  const ConnectedByteRegisterDecl opcode_;
};

} // namespace irata::sim::hdl
