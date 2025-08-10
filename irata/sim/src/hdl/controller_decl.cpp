#include <irata/sim/hdl/controller_decl.hpp>

namespace irata::sim::hdl {

ControllerDecl::ControllerDecl(const std::string &name,
                               const ComponentDecl &parent,
                               const ByteBusDecl &data_bus)
    : ComponentWithTypeDecl<ComponentType::Controller>(name),
      ComponentWithParentDecl<ComponentType::Controller>(name, parent),
      data_bus_(data_bus), step_counter_("step_counter", *this, data_bus),
      opcode_("opcode", *this, data_bus) {}

const ByteBusDecl &ControllerDecl::data_bus() const { return data_bus_; }

const IncrementableConnectedByteRegisterDecl &
ControllerDecl::step_counter() const {
  return step_counter_;
}

const ConnectedByteRegisterDecl &ControllerDecl::opcode() const {
  return opcode_;
}

void ControllerDecl::verify(const components::Component *component) const {
  ComponentWithParentDecl<ComponentType::Controller>::verify(component);
  verify_child(step_counter_, component);
  verify_child(opcode_, component);
}

} // namespace irata::sim::hdl
