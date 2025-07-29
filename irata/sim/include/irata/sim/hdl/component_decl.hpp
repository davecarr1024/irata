#pragma once

#include <irata/sim/components/component.hpp>
#include <irata/sim/hdl/component_type.hpp>
#include <ostream>
#include <string>
#include <string_view>

namespace irata::sim::hdl {

class ComponentDecl {
public:
  virtual ~ComponentDecl() = default;
  ComponentDecl(const ComponentDecl &) = delete;
  ComponentDecl &operator=(const ComponentDecl &) = delete;

  virtual std::string name() const = 0;
  virtual ComponentType type() const = 0;
  virtual const ComponentDecl *parent() const = 0;
  virtual void verify(const components::Component *component) const = 0;

  std::string path() const;
  const ComponentDecl *root() const;

protected:
  ComponentDecl() = default;

  void verify_child(const ComponentDecl &child,
                    const components::Component *component) const {
    // This shouldn't happen since this is called in verify() using child
    // components of the caller.
    if (child.parent() != this) {
      throw std::logic_error("trying to validate child " + child.path() +
                             " of " + path() + " but child.parent != this");
    }
    // This shouldn't happen since this is called in verify() after the base
    // class verifies that the component is not null.
    if (component == nullptr) {
      throw std::logic_error("trying to validate child " + child.path() +
                             " of " + path() + " but component == nullptr");
    }
    // Note that child can return a nullptr, which is handled in verify() with a
    // not found error.
    child.verify(component->child(child.name()));
  }
};

std::ostream &operator<<(std::ostream &os, const ComponentDecl &component);

template <ComponentType Type>
class TypedComponentDecl : public virtual ComponentDecl {
public:
  static_assert(Type != ComponentType::Unknown,
                "HDL component types cannot be unknown");

  static constexpr ComponentType type_v = Type;

  TypedComponentDecl(std::string_view name) : name_(name) {}

  ComponentType type() const override final { return Type; }
  std::string name() const override final { return name_; }
  const ComponentDecl *parent() const override { return nullptr; }

  void verify(const components::Component *component) const override {
    if (component == nullptr) {
      std::ostringstream os;
      os << "hdl component " << *this
         << " failed to find a corresponding sim component";
      throw std::invalid_argument(os.str());
    }
    if (component->type() != Type) {
      std::ostringstream os;
      os << "hdl component " << *this << " has type " << Type
         << " but sim component " << component->path() << " has type "
         << component->type();
      throw std::invalid_argument(os.str());
    }
  }

private:
  const std::string name_;
};

template <ComponentType Type>
class ComponentWithParentDecl : public virtual TypedComponentDecl<Type> {
public:
  ComponentWithParentDecl(std::string_view name, const ComponentDecl &parent)
      : TypedComponentDecl<Type>(name), parent_(parent) {}

  const ComponentDecl *parent() const override final { return &parent_; }

private:
  const ComponentDecl &parent_;
};

} // namespace irata::sim::hdl
