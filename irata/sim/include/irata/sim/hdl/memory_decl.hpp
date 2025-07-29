#pragma once

#include <irata/sim/hdl/bus_decl.hpp>
#include <irata/sim/hdl/component_decl.hpp>
#include <irata/sim/hdl/component_with_bus_decl.hpp>
#include <irata/sim/hdl/control_decl.hpp>
#include <irata/sim/hdl/register_decl.hpp>

namespace irata::sim::hdl {

class MemoryDecl final : public ComponentWithParentDecl<ComponentType::Memory>,
                         public virtual ComponentWithByteBusDecl {
public:
  MemoryDecl(const std::string &name, const ComponentDecl &parent,
             const WordBusDecl &address_bus, const ByteBusDecl &data_bus)
      : ComponentWithParentDecl(name, parent),
        TypedComponentDecl<ComponentType::Memory>(name),
        ComponentWithByteBusDecl(data_bus),
        address_(ConnectedWordRegisterDecl("address", *this, address_bus)) {}

  const ConnectedWordRegisterDecl &address() const { return address_; }

  void verify(const components::Component *component) const override final {
    ComponentWithParentDecl::verify(component);
    ComponentWithByteBusDecl::verify(component);
    verify_child(address_, component);
  }

private:
  const ConnectedWordRegisterDecl address_;
};

} // namespace irata::sim::hdl
