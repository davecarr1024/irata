#pragma once

#include <irata/sim/hdl/bus_decl.hpp>
#include <irata/sim/hdl/component_decl.hpp>
#include <irata/sim/hdl/register_decl.hpp>

namespace irata::sim::hdl {

class ControllerDecl final
    : public ComponentWithParentDecl<ComponentType::Controller> {
public:
  ControllerDecl(const std::string &name, const ComponentDecl &parent,
                 const ByteBusDecl &data_bus)
      : ComponentWithParentDecl<ComponentType::Controller>(name, parent),
        TypedComponentDecl<ComponentType::Controller>(name),
        data_bus_(data_bus), step_counter_("step_counter", *this, data_bus),
        opcode_("opcode", *this, data_bus) {}

  const ByteBusDecl &data_bus() const { return data_bus_; }

  const IncrementableConnectedByteRegisterDecl &step_counter() const {
    return step_counter_;
  }

  const ConnectedByteRegisterDecl &opcode() const { return opcode_; }

  void verify(const components::Component *component) const override final {
    ComponentWithParentDecl<ComponentType::Controller>::verify(component);
    verify_child(step_counter_, component);
    verify_child(opcode_, component);
  }

private:
  const ByteBusDecl &data_bus_;
  const IncrementableConnectedByteRegisterDecl step_counter_;
  const ConnectedByteRegisterDecl opcode_;
};

} // namespace irata::sim::hdl
