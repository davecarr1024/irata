#include <irata/sim/components/component.hpp>

namespace irata::sim::components {

class FakeComponent : public Component {
public:
  FakeComponent(hdl::ComponentType type, std::string_view name,
                Component *parent = nullptr)
      : Component(name, parent), type_(type) {}

  hdl::ComponentType type() const override { return type_; }

private:
  hdl::ComponentType type_;
};

} // namespace irata::sim::components
