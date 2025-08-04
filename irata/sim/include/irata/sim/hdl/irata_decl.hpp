#pragma once

#include <irata/sim/hdl/bus_decl.hpp>
#include <irata/sim/hdl/component_decl.hpp>
#include <irata/sim/hdl/cpu_decl.hpp>
#include <irata/sim/hdl/memory_decl.hpp>

namespace irata::sim::hdl {

class IrataDecl final : public ComponentWithTypeDecl<ComponentType::Irata> {
public:
  explicit IrataDecl();

  void verify(const components::Component *component) const override final;

  const WordBusDecl &address_bus() const;

  const ByteBusDecl &data_bus() const;

  const MemoryDecl &memory() const;

  const CpuDecl &cpu() const;

  const ProcessControlDecl &halt() const;

  static const IrataDecl &irata();

private:
  const WordBusDecl address_bus_;
  const ByteBusDecl data_bus_;
  const MemoryDecl memory_;
  const CpuDecl cpu_;
  const ProcessControlDecl halt_;
};

const IrataDecl &irata();

} // namespace irata::sim::hdl
