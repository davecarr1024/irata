#include <irata/sim/hdl/status_decl.hpp>

namespace irata::sim::hdl {

StatusDecl::StatusDecl(std::string_view name, const ComponentDecl &parent)
    : ComponentWithTypeDecl<ComponentType::Status>(name),
      ComponentWithParentDecl<ComponentType::Status>(name, parent) {}

} // namespace irata::sim::hdl
