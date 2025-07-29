#pragma once

#include <irata/sim/hdl/bus_decl.hpp>
#include <irata/sim/hdl/component_decl.hpp>
#include <irata/sim/hdl/controller_decl.hpp>
#include <irata/sim/hdl/register_decl.hpp>

namespace irata::sim::hdl {

class CpuDecl final : public ComponentWithParentDecl<ComponentType::Cpu> {
public:
  CpuDecl(const ComponentDecl &parent, const WordBusDecl &address_bus,
          const ByteBusDecl &data_bus);

  void verify(const components::Component *component) const override final;

  const WordBusDecl &address_bus() const;

  const ByteBusDecl &data_bus() const;

  const ConnectedByteRegisterDecl &a() const;

  const ConnectedByteRegisterDecl &x() const;

  const ConnectedByteRegisterDecl &y() const;

  const ProgramCounterDecl &pc() const;

  const ControllerDecl &controller() const;

private:
  const WordBusDecl &address_bus_;
  const ByteBusDecl &data_bus_;
  const ConnectedByteRegisterDecl a_;
  const ConnectedByteRegisterDecl x_;
  const ConnectedByteRegisterDecl y_;
  const ProgramCounterDecl pc_;
  const ControllerDecl controller_;
};

} // namespace irata::sim::hdl
