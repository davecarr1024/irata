#pragma once

#include <map>
#include <memory>
#include <string>

namespace irata::sim {

// A component is a part of a simulation. It can have children, which are
// also components.
class Component {
public:
  // Constructs a new component with the given name.
  explicit Component(std::string_view name);
  virtual ~Component() = default;

  // Disable copying and moving.
  Component(const Component &) = delete;
  Component(Component &&) = delete;
  Component &operator=(const Component &) = delete;
  Component &operator=(Component &&) = delete;

  // Returns the name of the component.
  std::string name() const;

  // Returns the parent of the component, or nullptr if the component has no
  // parent. The parent is set when the component is added to another component.
  Component *parent() const;

  // Returns a map of the children of the component, keyed by name.
  std::map<std::string, Component *> children() const;

  // Adds a child to the component.
  // Note that the caller retains owership of the child, which must outlive this
  // component.
  // Child can't be null and can't already have a parent.
  void add_child(Component *child);

  // Returns the child with the given name, or nullptr if no such child exists.
  const Component *child(std::string_view path) const;
  Component *child(std::string_view path);

  // Returns the root of the component tree.
  const Component *root() const;
  Component *root();

  // Returns the path of the component, which is the path from the root to the
  // component, separated by slashes. For example, if the component is named
  // "child" and its parent is named "parent", then the path is "/parent/child".
  std::string path() const;

private:
  std::string name_;
  Component *parent_ = nullptr;

  // The children of the component, keyed by name.
  // Note that the children are not owned by this map.
  std::map<std::string, Component *> children_;
};

} // namespace irata::sim