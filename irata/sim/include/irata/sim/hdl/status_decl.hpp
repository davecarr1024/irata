#include <irata/sim/hdl/component_decl.hpp>

namespace irata::sim::hdl {

class StatusDecl final : public ComponentWithParentDecl<ComponentType::Status> {
public:
  StatusDecl(std::string_view name, const ComponentDecl &parent);
};

} // namespace irata::sim::hdl
