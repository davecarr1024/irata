#pragma once

#include <irata/sim/hdl/bus_decl.hpp>
#include <irata/sim/hdl/component_decl.hpp>
#include <irata/sim/hdl/component_with_bus_decl.hpp>
#include <irata/sim/hdl/control_decl.hpp>
#include <irata/sim/hdl/register_decl.hpp>

namespace irata::sim::hdl {

// A memory is a component that stores bytes, made up of mapped regions that can
// be read-only or read-write.
// It is connected to a word bus for the address and a byte bus for the data.
class MemoryDecl final : public ComponentWithParentDecl<ComponentType::Memory>,
                         public virtual ComponentWithByteBusDecl {
public:
  MemoryDecl(const std::string &name, const ComponentDecl &parent,
             const WordBusDecl &address_bus, const ByteBusDecl &data_bus);

  // A word register that is connected to the address bus.
  // It is used to store the address of the memory region to read from or write
  // to.
  const ConnectedWordRegisterDecl &address() const;

  void verify(const components::Component *component) const override final;

private:
  const ConnectedWordRegisterDecl address_;
};

} // namespace irata::sim::hdl
