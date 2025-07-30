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
  virtual void verify(const components::Component *component) const;

  std::string path() const;
  const ComponentDecl *root() const;

protected:
  ComponentDecl() = default;

  void verify_child(const ComponentDecl &child,
                    const components::Component *component) const;
};

std::ostream &operator<<(std::ostream &os, const ComponentDecl &component);

template <ComponentType Type>
class ComponentWithTypeDecl : public virtual ComponentDecl {
public:
  static_assert(Type != ComponentType::Unknown,
                "HDL component types cannot be unknown");

  static constexpr ComponentType type_v = Type;

  ComponentWithTypeDecl(std::string_view name) : name_(name) {}

  ComponentType type() const override final { return Type; }
  std::string name() const override final { return name_; }
  const ComponentDecl *parent() const override { return nullptr; }

private:
  const std::string name_;
};

template <ComponentType Type>
class ComponentWithParentDecl : public virtual ComponentWithTypeDecl<Type> {
public:
  ComponentWithParentDecl(std::string_view name, const ComponentDecl &parent)
      : ComponentWithTypeDecl<Type>(name), parent_(parent) {}

  const ComponentDecl *parent() const override final { return &parent_; }

private:
  const ComponentDecl &parent_;
};

} // namespace irata::sim::hdl
