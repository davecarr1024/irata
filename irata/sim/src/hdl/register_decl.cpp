#include <irata/sim/hdl/register_decl.hpp>

namespace irata::sim::hdl {

const ProcessControlDecl &RegisterWithResetDecl::reset() const {
  return reset_;
}

void RegisterWithResetDecl::verify(
    const components::Component *component) const {
  verify_child(reset_, component);
}

RegisterWithResetDecl::RegisterWithResetDecl() : reset_("reset", *this) {}

const ProcessControlDecl &RegisterWithIncrementDecl::increment() const {
  return increment_;
}

void RegisterWithIncrementDecl::verify(
    const components::Component *component) const {
  verify_child(increment_, component);
}

RegisterWithIncrementDecl::RegisterWithIncrementDecl()
    : increment_("increment", *this) {}

DisconnectedByteRegisterDecl::DisconnectedByteRegisterDecl(
    std::string_view name, const ComponentDecl &parent)
    : ComponentWithParentDecl<ComponentType::Register>(name, parent),
      ComponentWithTypeDecl<ComponentType::Register>(name) {}

void DisconnectedByteRegisterDecl::verify(
    const components::Component *component) const {
  ComponentWithParentDecl<ComponentType::Register>::verify(component);
  RegisterWithResetDecl::verify(component);
}

ConnectedByteRegisterDecl::ConnectedByteRegisterDecl(
    std::string_view name, const ComponentDecl &parent, const ByteBusDecl &bus)
    : ComponentWithParentDecl<ComponentType::Register>(name, parent),
      ComponentWithTypeDecl<ComponentType::Register>(name),
      ComponentWithByteBusDecl(bus), RegisterWithByteBusDecl(bus) {}

void ConnectedByteRegisterDecl::verify(
    const components::Component *component) const {
  ComponentWithParentDecl<ComponentType::Register>::verify(component);
  RegisterWithResetDecl::verify(component);
  RegisterWithByteBusDecl::verify(component);
}

IncrementableConnectedByteRegisterDecl::IncrementableConnectedByteRegisterDecl(
    std::string_view name, const ComponentDecl &parent, const ByteBusDecl &bus)
    : ComponentWithParentDecl<ComponentType::Register>(name, parent),
      ComponentWithTypeDecl<ComponentType::Register>(name),
      ComponentWithByteBusDecl(bus), RegisterWithByteBusDecl(bus) {}

void IncrementableConnectedByteRegisterDecl::verify(
    const components::Component *component) const {
  ComponentWithParentDecl<ComponentType::Register>::verify(component);
  RegisterWithResetDecl::verify(component);
  RegisterWithIncrementDecl::verify(component);
  RegisterWithByteBusDecl::verify(component);
}

ConnectedWordRegisterDecl::ConnectedWordRegisterDecl(
    std::string_view name, const ComponentDecl &parent, const WordBusDecl &bus)
    : ComponentWithParentDecl<ComponentType::WordRegister>(name, parent),
      ComponentWithTypeDecl<ComponentType::WordRegister>(name),
      ComponentWithWordBusDecl(bus), RegisterWithWordBusDecl(bus),
      low_("low", *this), high_("high", *this) {}

const DisconnectedByteRegisterDecl &ConnectedWordRegisterDecl::low() const {
  return low_;
}

const DisconnectedByteRegisterDecl &ConnectedWordRegisterDecl::high() const {
  return high_;
}

void ConnectedWordRegisterDecl::verify(
    const components::Component *component) const {
  ComponentWithParentDecl<ComponentType::WordRegister>::verify(component);
  RegisterWithResetDecl::verify(component);
  RegisterWithWordBusDecl::verify(component);
  verify_child(low_, component);
  verify_child(high_, component);
}

ProgramCounterDecl::ProgramCounterDecl(std::string_view name,
                                       const ComponentDecl &parent,
                                       const WordBusDecl &address_bus,
                                       const ByteBusDecl &data_bus)
    : ComponentWithParentDecl<ComponentType::WordRegister>(name, parent),
      ComponentWithTypeDecl<ComponentType::WordRegister>(name),
      ComponentWithWordBusDecl(address_bus),
      RegisterWithWordBusDecl(address_bus), low_("low", *this, data_bus),
      high_("high", *this, data_bus) {}

const ConnectedByteRegisterDecl &ProgramCounterDecl::low() const {
  return low_;
}

const ConnectedByteRegisterDecl &ProgramCounterDecl::high() const {
  return high_;
}

void ProgramCounterDecl::verify(const components::Component *component) const {
  ComponentWithParentDecl<ComponentType::WordRegister>::verify(component);
  RegisterWithResetDecl::verify(component);
  RegisterWithIncrementDecl::verify(component);
  RegisterWithWordBusDecl::verify(component);
  verify_child(low_, component);
  verify_child(high_, component);
}

} // namespace irata::sim::hdl
