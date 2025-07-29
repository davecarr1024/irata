#include <irata/sim/components/control.hpp>
#include <irata/sim/hdl/control_decl.hpp>
#include <sstream>
#include <stdexcept>

namespace irata::sim::hdl {

void ControlDecl::verify(const components::Component *component) const {
  const auto *control = dynamic_cast<const components::Control *>(component);
  if (control == nullptr) {
    std::ostringstream os;
    os << *this << ": unable to cast component " << component->path()
       << " to Control";
    throw std::runtime_error(os.str());
  }
  if (control->phase() != phase()) {
    std::ostringstream os;
    os << *this << ": control phase " << control->phase()
       << " does not match decl phase " << phase();
    throw std::runtime_error(os.str());
  }
  if (control->auto_clear() != auto_clear()) {
    std::ostringstream os;
    os << *this << ": control auto_clear " << control->auto_clear()
       << " does not match decl auto_clear " << auto_clear();
    throw std::runtime_error(os.str());
  }
}

std::ostream &operator<<(std::ostream &os, const ControlDecl &control) {
  return os << control.type() << "(path = " << control.path()
            << ", phase = " << control.phase()
            << ", auto_clear = " << control.auto_clear() << ")";
}

ControlWithBusDecl::ControlWithBusDecl(const BusDecl &bus) : bus_(bus) {}

const BusDecl &ControlWithBusDecl::bus() const { return bus_; }

WriteControlDecl::WriteControlDecl(std::string_view name,
                                   const ComponentDecl &parent,
                                   const BusDecl &bus)
    : ComponentWithParentDecl<ComponentType::Control>(name, parent),
      TypedComponentDecl<ComponentType::Control>(name),
      ControlWithBusDecl(bus) {}

void WriteControlDecl::verify(const components::Component *component) const {
  ComponentWithParentDecl<ComponentType::Control>::verify(component);
  ControlDecl::verify(component);
}

ReadControlDecl::ReadControlDecl(std::string_view name,
                                 const ComponentDecl &parent,
                                 const BusDecl &bus)
    : ComponentWithParentDecl<ComponentType::Control>(name, parent),
      TypedComponentDecl<ComponentType::Control>(name),
      ControlWithBusDecl(bus) {}

void ReadControlDecl::verify(const components::Component *component) const {
  ComponentWithParentDecl<ComponentType::Control>::verify(component);
  ControlDecl::verify(component);
}

ProcessControlDecl::ProcessControlDecl(std::string_view name,
                                       const ComponentDecl &parent)
    : ComponentWithParentDecl<ComponentType::Control>(name, parent),
      TypedComponentDecl<ComponentType::Control>(name) {}

void ProcessControlDecl::verify(const components::Component *component) const {
  ComponentWithParentDecl<ComponentType::Control>::verify(component);
  ControlDecl::verify(component);
}

PersistentProcessControlDecl::PersistentProcessControlDecl(
    std::string_view name, const ComponentDecl &parent)
    : ComponentWithParentDecl<ComponentType::Control>(name, parent),
      TypedComponentDecl<ComponentType::Control>(name) {}

void PersistentProcessControlDecl::verify(
    const components::Component *component) const {
  ComponentWithParentDecl<ComponentType::Control>::verify(component);
  ControlWithAutoClearDecl<false>::verify(component);
}

PersistentClearControlDecl::PersistentClearControlDecl(
    std::string_view name, const ComponentDecl &parent)
    : ComponentWithParentDecl<ComponentType::Control>(name, parent),
      TypedComponentDecl<ComponentType::Control>(name) {}

void PersistentClearControlDecl::verify(
    const components::Component *component) const {
  ComponentWithParentDecl<ComponentType::Control>::verify(component);
  ControlWithAutoClearDecl<false>::verify(component);
}

} // namespace irata::sim::hdl
