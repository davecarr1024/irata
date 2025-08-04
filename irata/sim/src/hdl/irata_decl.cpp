#include <irata/sim/hdl/irata_decl.hpp>

namespace irata::sim::hdl {

IrataDecl::IrataDecl()
    : ComponentWithTypeDecl<ComponentType::Irata>("irata"),
      address_bus_("address_bus", *this), data_bus_("data_bus", *this),
      memory_("memory", *this, address_bus_, data_bus_),
      cpu_(*this, address_bus_, data_bus_), halt_("halt", *this) {}

void IrataDecl::verify(const components::Component *component) const {
  ComponentWithTypeDecl<ComponentType::Irata>::verify(component);
  verify_child(address_bus_, component);
  verify_child(data_bus_, component);
  verify_child(memory_, component);
  verify_child(cpu_, component);
  verify_child(halt_, component);
}

const IrataDecl &IrataDecl::irata() {
  static const IrataDecl irata;
  return irata;
}

const IrataDecl &irata() { return IrataDecl::irata(); }

const WordBusDecl &IrataDecl::address_bus() const { return address_bus_; }

const ByteBusDecl &IrataDecl::data_bus() const { return data_bus_; }

const MemoryDecl &IrataDecl::memory() const { return memory_; }

const CpuDecl &IrataDecl::cpu() const { return cpu_; }

const ProcessControlDecl &IrataDecl::halt() const { return halt_; }

} // namespace irata::sim::hdl
