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

void Component::add_child(Component *child) {
  if (child == nullptr) {
    throw std::invalid_argument("child cannot be null");
  }
  if (child->parent_ != nullptr) {
    throw std::invalid_argument("child already has a parent");
  }
  if (children_.find(child->name_) != children_.end()) {
    throw std::invalid_argument("child with same name already exists");
  }
  children_[child->name_] = child;
  child->parent_ = this;
}

std::map<std::string, Component *> Component::children() const {
  return children_;
}

const Component *Component::child(std::string_view path) const {
  if (path == "/") {
    return root();
  } else if (path == "..") {
    return parent_;
  } else if (path.find('/') == 0) {
    return root()->child(path.substr(1));
  } else if (const auto pos = path.find('/'); pos != std::string::npos) {
    const auto name = path.substr(0, pos);
    const auto rest = path.substr(pos + 1);
    return child(name)->child(rest);
  } else if (const auto it = children_.find(std::string(path));
             it != children_.end()) {
    return it->second;
  } else {
    return nullptr;
  }
}

Component *Component::child(std::string_view path) {
  return const_cast<Component *>(std::as_const(*this).child(path));
}

std::string Component::path() const {
  if (parent_ == nullptr) {
    return "/";
  } else if (parent_->parent_ == nullptr) {
    return "/" + name_;
  } else {
    return parent_->path() + "/" + name_;
  }
}

const Component *Component::root() const {
  if (parent_ == nullptr) {
    return this;
  } else {
    return parent_->root();
  }
}

Component *Component::root() {
  if (parent_ == nullptr) {
    return this;
  } else {
    return parent_->root();
  }
}

} // namespace irata::sim