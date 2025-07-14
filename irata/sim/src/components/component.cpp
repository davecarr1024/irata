#include <irata/sim/components/component.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace irata::sim {

Component::Component(std::string_view name) : name_(std::string(name)) {}

std::string Component::name() const { return name_; }

Component *Component::parent() const { return parent_; }

void Component::add_child(std::unique_ptr<Component> child) {
  if (child->parent() != nullptr) {
    throw std::logic_error("Component already has a parent");
  }
  if (children_.find(child->name()) != children_.end()) {
    throw std::logic_error("Component already has a child with this name");
  }
  child->parent_ = this;
  children_[child->name()] = std::move(child);
}

std::map<std::string, Component *> Component::children() const {
  std::map<std::string, Component *> result;
  for (const auto &[name, child] : children_) {
    result[name] = child.get();
  }
  return result;
}

Component *Component::child(std::string_view name) const {
  auto it = children_.find(std::string(name));
  return it == children_.end() ? nullptr : it->second.get();
}

} // namespace irata::sim