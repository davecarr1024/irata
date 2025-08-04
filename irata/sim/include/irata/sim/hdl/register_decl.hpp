#pragma once

#include <irata/sim/hdl/bus_decl.hpp>
#include <irata/sim/hdl/component_decl.hpp>
#include <irata/sim/hdl/component_with_bus_decl.hpp>
#include <irata/sim/hdl/control_decl.hpp>

namespace irata::sim::hdl {

// RegisterDecl is a declaration of a register component.
class RegisterDecl : public virtual ComponentDecl {};

// RegisterWithResetDecl is a mixin for RegisterDecl that provides a reset
// control line. The reset control sets this register to zero.
class RegisterWithResetDecl : public virtual RegisterDecl {
public:
  const ProcessControlDecl &reset() const;

  void verify(const components::Component *component) const override;

protected:
  RegisterWithResetDecl();

private:
  const ProcessControlDecl reset_;
};

// RegisterWithIncrementDecl is a mixin for RegisterDecl that provides an
// increment control line. The increment control increments this register by
// one.
class RegisterWithIncrementDecl : public virtual RegisterDecl {
public:
  const ProcessControlDecl &increment() const;

  void verify(const components::Component *component) const override;

protected:
  RegisterWithIncrementDecl();

private:
  const ProcessControlDecl increment_;
};

// RegisterWithBusDecl is a mixin for RegisterDecl that provides a bus
// connection. The bus connection allows the register to be read from and
// written to.
template <typename Bus>
class RegisterWithBusDecl : public virtual RegisterDecl,
                            public virtual ComponentWithBusDecl<Bus> {
protected:
  RegisterWithBusDecl(const Bus &bus) : ComponentWithBusDecl<Bus>(bus) {}
};

// RegisterWithByteBusDecl is a mixin for RegisterDecl that provides a byte
// bus connection. The bus connection allows the register to be read from and
// written to.
using RegisterWithByteBusDecl = RegisterWithBusDecl<ByteBusDecl>;

// RegisterWithWordBusDecl is a mixin for RegisterDecl that provides a word
// bus connection. The bus connection allows the register to be read from and
// written to.
using RegisterWithWordBusDecl = RegisterWithBusDecl<WordBusDecl>;

// DisconnectedByteRegisterDecl is a declaration of a byte register component
// that is not connected to any bus. This is used for the high and low byte
// registers of a word register.
class DisconnectedByteRegisterDecl final
    : public ComponentWithParentDecl<ComponentType::Register>,
      public RegisterWithResetDecl {
public:
  DisconnectedByteRegisterDecl(std::string_view name,
                               const ComponentDecl &parent);

  void verify(const components::Component *component) const override final;
};

// ConnectedByteRegisterDecl is a declaration of a byte register component
// that is connected to a byte bus. This is used for the A, X, and Y
// registers of the CPU.
class ConnectedByteRegisterDecl final
    : public ComponentWithParentDecl<ComponentType::Register>,
      public RegisterWithResetDecl,
      public RegisterWithByteBusDecl {
public:
  ConnectedByteRegisterDecl(std::string_view name, const ComponentDecl &parent,
                            const ByteBusDecl &bus);

  void verify(const components::Component *component) const override final;
};

// IncrementableConnectedByteRegisterDecl is a declaration of a byte register
// component that is connected to a byte bus and has an increment control line.
// This is used for the step counter register of the CPU.
class IncrementableConnectedByteRegisterDecl final
    : public ComponentWithParentDecl<ComponentType::Register>,
      public RegisterWithResetDecl,
      public RegisterWithIncrementDecl,
      public RegisterWithByteBusDecl {
public:
  IncrementableConnectedByteRegisterDecl(std::string_view name,
                                         const ComponentDecl &parent,
                                         const ByteBusDecl &bus);

  void verify(const components::Component *component) const override final;
};

// ConnectedWordRegisterDecl is a declaration of a word register component
// that is connected to a word bus. This is used for the memory's address
// register.
class ConnectedWordRegisterDecl final
    : public ComponentWithParentDecl<ComponentType::WordRegister>,
      public RegisterWithResetDecl,
      public RegisterWithWordBusDecl {
public:
  ConnectedWordRegisterDecl(std::string_view name, const ComponentDecl &parent,
                            const WordBusDecl &bus);

  const DisconnectedByteRegisterDecl &low() const;

  const DisconnectedByteRegisterDecl &high() const;

  void verify(const components::Component *component) const override final;

private:
  const DisconnectedByteRegisterDecl low_;
  const DisconnectedByteRegisterDecl high_;
};

// ProgramCounterDecl is a declaration of a program counter component.
// This is a word register that has a reset, increment, and bus connection.
// The low and high byte register are also connected to the data bus.
class ProgramCounterDecl final
    : public ComponentWithParentDecl<ComponentType::WordRegister>,
      public RegisterWithResetDecl,
      public RegisterWithIncrementDecl,
      public RegisterWithWordBusDecl {
public:
  ProgramCounterDecl(std::string_view name, const ComponentDecl &parent,
                     const WordBusDecl &address_bus,
                     const ByteBusDecl &data_bus);

  const ConnectedByteRegisterDecl &low() const;

  const ConnectedByteRegisterDecl &high() const;

  void verify(const components::Component *component) const override final;

private:
  const ConnectedByteRegisterDecl low_;
  const ConnectedByteRegisterDecl high_;
};

} // namespace irata::sim::hdl
