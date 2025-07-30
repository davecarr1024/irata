#include <irata/sim/hdl/status_decl.hpp>

namespace irata::sim::hdl {

StatusDecl::StatusDecl(std::string_view name, const ComponentDecl &parent)
    : ComponentWithParentDecl<ComponentType::Status>(name, parent),
      ComponentWithTypeDecl<ComponentType::Status>(name) {}

} // namespace irata::sim::hdl
