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

void ComponentDecl::verify(const components::Component *component) const {
  if (component == nullptr) {
    std::ostringstream os;
    os << "hdl component " << *this
       << " failed to find a corresponding sim component";
    throw std::invalid_argument(os.str());
  }
  if (component->type() != type()) {
    std::ostringstream os;
    os << "hdl component " << *this << " has type " << type()
       << " but sim component " << component->path() << " has type "
       << component->type();
    throw std::invalid_argument(os.str());
  }
  if (component->path() != path()) {
    std::ostringstream os;
    os << "hdl component " << *this << " has path " << path() << " but sim "
       << "component " << component->path() << " has path "
       << component->path();
    throw std::invalid_argument(os.str());
  }
  std::cout << *this << " verified sim component " << component->path()
            << std::endl;
}

void ComponentDecl::verify_child(const ComponentDecl &child,
                                 const components::Component *component) const {
  // This shouldn't happen since this is called in verify() using child
  // components of the caller.
  if (child.parent() != this) {
    throw std::logic_error("trying to validate child " + child.path() + " of " +
                           path() + " but child.parent != this");
  }
  // This shouldn't happen since this is called in verify() after the base
  // class verifies that the component is not null.
  if (component == nullptr) {
    throw std::logic_error("trying to validate child " + child.path() + " of " +
                           path() + " but component == nullptr");
  }
  // Note that child can return a nullptr, which is handled in verify() with a
  // not found error.
  child.verify(component->child(child.name()));
}

} // namespace irata::sim::hdl
