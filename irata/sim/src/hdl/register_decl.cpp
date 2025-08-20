#include <irata/sim/components/memory/address.hpp>
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

RegisterWithIncrementDecl::RegisterWithIncrementDecl()
    : increment_("increment", *this), decrement_("decrement", *this) {}

const ProcessControlDecl &RegisterWithIncrementDecl::increment() const {
  return increment_;
}

const ProcessControlDecl &RegisterWithIncrementDecl::decrement() const {
  return decrement_;
}

void RegisterWithIncrementDecl::verify(
    const components::Component *component) const {
  verify_child(increment_, component);
  verify_child(decrement_, component);
}

DisconnectedByteRegisterDecl::DisconnectedByteRegisterDecl(
    std::string_view name, const ComponentDecl &parent)
    : ComponentWithTypeDecl<ComponentType::Register>(name),
      ComponentWithParentDecl<ComponentType::Register>(name, parent) {}

void DisconnectedByteRegisterDecl::verify(
    const components::Component *component) const {
  ComponentWithParentDecl<ComponentType::Register>::verify(component);
  RegisterWithResetDecl::verify(component);
}

ConnectedByteRegisterDecl::ConnectedByteRegisterDecl(
    std::string_view name, const ComponentDecl &parent, const ByteBusDecl &bus)
    : ComponentWithTypeDecl<ComponentType::Register>(name),
      ComponentWithByteBusDecl(bus),
      ComponentWithParentDecl<ComponentType::Register>(name, parent),
      RegisterWithByteBusDecl(bus) {}

void ConnectedByteRegisterDecl::verify(
    const components::Component *component) const {
  ComponentWithParentDecl<ComponentType::Register>::verify(component);
  RegisterWithResetDecl::verify(component);
  RegisterWithByteBusDecl::verify(component);
}

IncrementableConnectedByteRegisterDecl::IncrementableConnectedByteRegisterDecl(
    std::string_view name, const ComponentDecl &parent, const ByteBusDecl &bus)
    : ComponentWithTypeDecl<ComponentType::IncrementableRegister>(name),
      ComponentWithByteBusDecl(bus),
      ComponentWithParentDecl<ComponentType::IncrementableRegister>(name,
                                                                    parent),
      RegisterWithByteBusDecl(bus) {}

void IncrementableConnectedByteRegisterDecl::verify(
    const components::Component *component) const {
  ComponentWithParentDecl<ComponentType::IncrementableRegister>::verify(
      component);
  RegisterWithResetDecl::verify(component);
  RegisterWithIncrementDecl::verify(component);
  RegisterWithByteBusDecl::verify(component);
}

ConnectedWordRegisterDecl::ConnectedWordRegisterDecl(
    std::string_view name, const ComponentDecl &parent, const WordBusDecl &bus)
    : ComponentWithTypeDecl<ComponentType::WordRegister>(name),
      ComponentWithWordBusDecl(bus),
      ComponentWithParentDecl<ComponentType::WordRegister>(name, parent),
      RegisterWithWordBusDecl(bus), low_("low", *this), high_("high", *this) {}

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

DataAndAddressBusWordRegisterDecl::DataAndAddressBusWordRegisterDecl(
    std::string_view name, const ComponentDecl &parent,
    const WordBusDecl &address_bus, const ByteBusDecl &data_bus)
    : ComponentWithTypeDecl<ComponentType::WordRegister>(name),
      ComponentWithWordBusDecl(address_bus),
      ComponentWithParentDecl<ComponentType::WordRegister>(name, parent),
      RegisterWithWordBusDecl(address_bus), low_("low", *this, data_bus),
      high_("high", *this, data_bus) {}

const ConnectedByteRegisterDecl &
DataAndAddressBusWordRegisterDecl::low() const {
  return low_;
}

const ConnectedByteRegisterDecl &
DataAndAddressBusWordRegisterDecl::high() const {
  return high_;
}

void DataAndAddressBusWordRegisterDecl::verify(
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
    : ComponentWithTypeDecl<ComponentType::IncrementableWordRegister>(name),
      ComponentWithWordBusDecl(address_bus),
      ComponentWithParentDecl<ComponentType::IncrementableWordRegister>(name,
                                                                        parent),
      RegisterWithWordBusDecl(address_bus), low_("low", *this, data_bus),
      high_("high", *this, data_bus) {}

const ConnectedByteRegisterDecl &ProgramCounterDecl::low() const {
  return low_;
}

const ConnectedByteRegisterDecl &ProgramCounterDecl::high() const {
  return high_;
}

void ProgramCounterDecl::verify(const components::Component *component) const {
  ComponentWithParentDecl<ComponentType::IncrementableWordRegister>::verify(
      component);
  RegisterWithResetDecl::verify(component);
  RegisterWithIncrementDecl::verify(component);
  RegisterWithWordBusDecl::verify(component);
  verify_child(low_, component);
  verify_child(high_, component);
}

MemoryAddressRegisterDecl::MemoryAddressRegisterDecl(
    std::string_view name, const ComponentDecl &parent,
    const WordBusDecl &address_bus, const ByteBusDecl &data_bus,
    const StatusDecl &address_add_carry)
    : ComponentWithTypeDecl<ComponentType::MemoryAddress>(name),
      ComponentWithWordBusDecl(address_bus),
      ComponentWithParentDecl<ComponentType::MemoryAddress>(name, parent),
      RegisterWithWordBusDecl(address_bus), low_("low", *this, data_bus),
      high_("high", *this, data_bus), address_add_carry_(address_add_carry) {}

const IncrementableConnectedByteRegisterDecl &
MemoryAddressRegisterDecl::low() const {
  return low_;
}

const IncrementableConnectedByteRegisterDecl &
MemoryAddressRegisterDecl::high() const {
  return high_;
}

void MemoryAddressRegisterDecl::verify(
    const components::Component *component) const {
  ComponentWithParentDecl<ComponentType::MemoryAddress>::verify(component);
  RegisterWithResetDecl::verify(component);
  RegisterWithWordBusDecl::verify(component);
  RegisterWithIncrementDecl::verify(component);
  verify_child(low_, component);
  verify_child(high_, component);
  const auto &address_component =
      dynamic_cast<const components::memory::Address &>(*component);
  const auto &address_add_carry_component = address_component.carry_status();
  if (address_add_carry_component.path() != address_add_carry_.path()) {
    std::ostringstream os;
    os << "Memory address register address add carry status path does not "
          "match. Expected: "
       << address_add_carry_.path()
       << ", actual: " << address_add_carry_component.path();
    throw std::invalid_argument(os.str());
  }
}

} // namespace irata::sim::hdl
