#include <irata/sim/hdl/bus_decl.hpp>

namespace irata::sim::hdl {

ByteBusDecl::ByteBusDecl(std::string_view name, const ComponentDecl &parent)
    : ComponentWithParentDecl<ComponentType::ByteBus>(name, parent),
      ComponentWithTypeDecl<ComponentType::ByteBus>(name) {}

WordBusDecl::WordBusDecl(std::string_view name, const ComponentDecl &parent)
    : ComponentWithParentDecl<ComponentType::WordBus>(name, parent),
      ComponentWithTypeDecl<ComponentType::WordBus>(name) {}

} // namespace irata::sim::hdl
