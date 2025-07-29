#include <irata/sim/hdl/component_decl.hpp>

namespace irata::sim::hdl {

std::string ComponentDecl::path() const {
  const auto *parent = this->parent();
  if (parent == nullptr) {
    return "/";
  } else if (parent->parent() == nullptr) {
    return "/" + this->name();
  } else {
    return parent->path() + "/" + this->name();
  }
}

const ComponentDecl *ComponentDecl::root() const {
  if (const auto *parent = this->parent(); parent != nullptr) {
    return parent->root();
  } else {
    return this;
  }
}

std::ostream &operator<<(std::ostream &os, const ComponentDecl &component) {
  return os << component.type() << "(" << component.path() << ")";
}

} // namespace irata::sim::hdl
