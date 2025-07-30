#pragma once

#include <irata/sim/hdl/bus_decl.hpp>
#include <irata/sim/hdl/component_decl.hpp>
#include <irata/sim/hdl/component_with_bus_decl.hpp>
#include <irata/sim/hdl/control_decl.hpp>

namespace irata::sim::hdl {

class RegisterDecl : public virtual ComponentDecl {};

class RegisterWithResetDecl : public virtual RegisterDecl {
public:
  const ProcessControlDecl &reset() const { return reset_; }

  void verify(const components::Component *component) const override {
    verify_child(reset_, component);
  }

protected:
  RegisterWithResetDecl() : reset_("reset", *this) {}

private:
  const ProcessControlDecl reset_;
};

class RegisterWithIncrementDecl : public virtual RegisterDecl {
public:
  const ProcessControlDecl &increment() const { return increment_; }

  void verify(const components::Component *component) const override {
    verify_child(increment_, component);
  }

protected:
  RegisterWithIncrementDecl() : increment_("increment", *this) {}

private:
  const ProcessControlDecl increment_;
};

template <typename Bus>
class RegisterWithBusDecl : public virtual RegisterDecl,
                            public virtual ComponentWithBusDecl<Bus> {
protected:
  RegisterWithBusDecl(const Bus &bus) : ComponentWithBusDecl<Bus>(bus) {}
};

using RegisterWithByteBusDecl = RegisterWithBusDecl<ByteBusDecl>;
using RegisterWithWordBusDecl = RegisterWithBusDecl<WordBusDecl>;

class DisconnectedByteRegisterDecl final
    : public ComponentWithParentDecl<ComponentType::Register>,
      public RegisterWithResetDecl {
public:
  DisconnectedByteRegisterDecl(std::string_view name,
                               const ComponentDecl &parent)
      : ComponentWithParentDecl<ComponentType::Register>(name, parent),
        ComponentWithTypeDecl<ComponentType::Register>(name) {}

  void verify(const components::Component *component) const override final {
    ComponentWithParentDecl<ComponentType::Register>::verify(component);
    RegisterWithResetDecl::verify(component);
  }
};

class ConnectedByteRegisterDecl final
    : public ComponentWithParentDecl<ComponentType::Register>,
      public RegisterWithResetDecl,
      public RegisterWithByteBusDecl {
public:
  ConnectedByteRegisterDecl(std::string_view name, const ComponentDecl &parent,
                            const ByteBusDecl &bus)
      : ComponentWithParentDecl<ComponentType::Register>(name, parent),
        ComponentWithTypeDecl<ComponentType::Register>(name),
        ComponentWithByteBusDecl(bus), RegisterWithByteBusDecl(bus) {}

  void verify(const components::Component *component) const override final {
    ComponentWithParentDecl<ComponentType::Register>::verify(component);
    RegisterWithResetDecl::verify(component);
    RegisterWithByteBusDecl::verify(component);
  }
};

class IncrementableConnectedByteRegisterDecl final
    : public ComponentWithParentDecl<ComponentType::Register>,
      public RegisterWithResetDecl,
      public RegisterWithIncrementDecl,
      public RegisterWithByteBusDecl {
public:
  IncrementableConnectedByteRegisterDecl(std::string_view name,
                                         const ComponentDecl &parent,
                                         const ByteBusDecl &bus)
      : ComponentWithParentDecl<ComponentType::Register>(name, parent),
        ComponentWithTypeDecl<ComponentType::Register>(name),
        ComponentWithByteBusDecl(bus), RegisterWithByteBusDecl(bus) {}

  void verify(const components::Component *component) const override final {
    ComponentWithParentDecl<ComponentType::Register>::verify(component);
    RegisterWithResetDecl::verify(component);
    RegisterWithIncrementDecl::verify(component);
    RegisterWithByteBusDecl::verify(component);
  }
};

class ConnectedWordRegisterDecl final
    : public ComponentWithParentDecl<ComponentType::WordRegister>,
      public RegisterWithResetDecl,
      public RegisterWithWordBusDecl {
public:
  ConnectedWordRegisterDecl(std::string_view name, const ComponentDecl &parent,
                            const WordBusDecl &bus)
      : ComponentWithParentDecl<ComponentType::WordRegister>(name, parent),
        ComponentWithTypeDecl<ComponentType::WordRegister>(name),
        ComponentWithWordBusDecl(bus), RegisterWithWordBusDecl(bus),
        low_("low", *this), high_("high", *this) {}

  const DisconnectedByteRegisterDecl &low() const { return low_; }

  const DisconnectedByteRegisterDecl &high() const { return high_; }

  void verify(const components::Component *component) const override final {
    ComponentWithParentDecl<ComponentType::WordRegister>::verify(component);
    RegisterWithResetDecl::verify(component);
    RegisterWithWordBusDecl::verify(component);
    verify_child(low_, component);
    verify_child(high_, component);
  }

private:
  const DisconnectedByteRegisterDecl low_;
  const DisconnectedByteRegisterDecl high_;
};

class ProgramCounterDecl final
    : public ComponentWithParentDecl<ComponentType::Register>,
      public RegisterWithResetDecl,
      public RegisterWithIncrementDecl,
      public RegisterWithWordBusDecl {
public:
  ProgramCounterDecl(std::string_view name, const ComponentDecl &parent,
                     const WordBusDecl &address_bus,
                     const ByteBusDecl &data_bus)
      : ComponentWithParentDecl<ComponentType::Register>(name, parent),
        ComponentWithTypeDecl<ComponentType::Register>(name),
        ComponentWithWordBusDecl(address_bus),
        RegisterWithWordBusDecl(address_bus), low_("low", *this, data_bus),
        high_("high", *this, data_bus) {}

  const ConnectedByteRegisterDecl &low() const { return low_; }

  const ConnectedByteRegisterDecl &high() const { return high_; }

  void verify(const components::Component *component) const override final {
    ComponentWithParentDecl<ComponentType::Register>::verify(component);
    RegisterWithResetDecl::verify(component);
    RegisterWithIncrementDecl::verify(component);
    RegisterWithWordBusDecl::verify(component);
    verify_child(low_, component);
    verify_child(high_, component);
  }

private:
  const ConnectedByteRegisterDecl low_;
  const ConnectedByteRegisterDecl high_;
};

} // namespace irata::sim::hdl
