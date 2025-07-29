#pragma once

#include <irata/sim/hdl/component_decl.hpp>
#include <irata/sim/hdl/control_decl.hpp>

namespace irata::sim::hdl {

template <typename Bus>
class ComponentWithBusDecl : public virtual ComponentDecl {
public:
  static_assert(std::is_base_of_v<BusDecl, Bus>, "Bus must be a BusDecl");

  const Bus &bus() const { return bus_; }

  const WriteControlDecl &write() const { return write_; }

  const ReadControlDecl &read() const { return read_; }

  void verify(const components::Component *component) const override {
    verify_child(write_, component);
    verify_child(read_, component);
  }

protected:
  ComponentWithBusDecl(const Bus &bus)
      : bus_(bus), write_("write", *this, bus), read_("read", *this, bus) {}

private:
  const Bus &bus_;
  const WriteControlDecl write_;
  const ReadControlDecl read_;
};

using ComponentWithByteBusDecl = ComponentWithBusDecl<ByteBusDecl>;
using ComponentWithWordBusDecl = ComponentWithBusDecl<WordBusDecl>;

} // namespace irata::sim::hdl
