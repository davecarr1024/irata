#include <irata/sim/hdl/memory_decl.hpp>

namespace irata::sim::hdl {

MemoryDecl::MemoryDecl(const std::string &name, const ComponentDecl &parent,
                       const WordBusDecl &address_bus,
                       const ByteBusDecl &data_bus)
    : ComponentWithTypeDecl<ComponentType::Memory>(name),
      ComponentWithByteBusDecl(data_bus), ComponentWithParentDecl(name, parent),
      address_("address", *this, address_bus, data_bus) {}

const MemoryAddressRegisterDecl &MemoryDecl::address() const {
  return address_;
}

void MemoryDecl::verify(const components::Component *component) const {
  ComponentWithParentDecl::verify(component);
  ComponentWithByteBusDecl::verify(component);
  verify_child(address_, component);
}

} // namespace irata::sim::hdl
