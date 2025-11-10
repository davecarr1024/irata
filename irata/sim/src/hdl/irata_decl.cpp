#include <irata/sim/hdl/irata_decl.hpp>

#ifdef __has_include
#if __has_include(<sanitizer/lsan_interface.h>)
#include <sanitizer/lsan_interface.h>
#define HAS_LSAN 1
#endif
#endif

namespace irata::sim::hdl {

IrataDecl::IrataDecl()
    : ComponentWithTypeDecl<ComponentType::Irata>("irata"),
      address_bus_("address_bus", *this), data_bus_("data_bus", *this),
      cpu_(*this, address_bus_, data_bus_),
      memory_("memory", *this, address_bus_, data_bus_,
              cpu_.alu().address_add_carry()),
      halt_("halt", *this), crash_("crash", *this) {}

void IrataDecl::verify(const components::Component *component) const {
  ComponentWithTypeDecl<ComponentType::Irata>::verify(component);
  verify_child(address_bus_, component);
  verify_child(data_bus_, component);
  verify_child(memory_, component);
  verify_child(cpu_, component);
  verify_child(halt_, component);
  verify_child(crash_, component);
}

const IrataDecl &IrataDecl::irata() {
  // Intentionally leak to avoid static destruction order issues
  // This singleton's lifetime should extend to program termination
  static const IrataDecl *irata = new IrataDecl();
#ifdef HAS_LSAN
  __lsan_ignore_object(irata);
#endif
  return *irata;
}

const IrataDecl &irata() { return IrataDecl::irata(); }

const WordBusDecl &IrataDecl::address_bus() const { return address_bus_; }

const ByteBusDecl &IrataDecl::data_bus() const { return data_bus_; }

const MemoryDecl &IrataDecl::memory() const { return memory_; }

const CpuDecl &IrataDecl::cpu() const { return cpu_; }

const ProcessControlDecl &IrataDecl::halt() const { return halt_; }

const ProcessControlDecl &IrataDecl::crash() const { return crash_; }

} // namespace irata::sim::hdl
