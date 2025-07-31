#include <irata/sim/hdl/component_decl.hpp>

namespace irata::sim::hdl {

class FakeComponentDecl : public ComponentDecl {
public:
  FakeComponentDecl(ComponentType type, std::string_view name,
                    FakeComponentDecl *parent = nullptr)
      : type_(type), name_(name), parent_(parent) {
    if (parent != nullptr) {
      parent->add_child(this);
    }
  }

  ComponentType type() const override { return type_; }

  std::string name() const override { return name_; }

  const ComponentDecl *parent() const override { return parent_; }

  void verify(const components::Component *component) const override {
    ComponentDecl::verify(component);
    for (const auto *child : children_) {
      verify_child(*child, component);
    }
  }

private:
  void add_child(const FakeComponentDecl *child) { children_.push_back(child); }

  const ComponentType type_;
  const std::string name_;
  const FakeComponentDecl *parent_;
  std::vector<const FakeComponentDecl *> children_;
};

} // namespace irata::sim::hdl
