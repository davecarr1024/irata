#include <irata/sim/hdl/bus_decl.hpp>

namespace irata::sim::hdl {

ByteBusDecl::ByteBusDecl(std::string_view name, const ComponentDecl &parent)
    : ComponentWithTypeDecl<ComponentType::ByteBus>(name),
      ComponentWithParentDecl<ComponentType::ByteBus>(name, parent) {}

WordBusDecl::WordBusDecl(std::string_view name, const ComponentDecl &parent)
    : ComponentWithTypeDecl<ComponentType::WordBus>(name),
      ComponentWithParentDecl<ComponentType::WordBus>(name, parent) {}

} // namespace irata::sim::hdl
