#pragma once

#include <functional>
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

  // Tick the whole component tree, doing all tick phases in order top down.
  void tick();

  // Set up any control lines that will be used this tick.
  virtual void tick_control() {}

  // Write any data to the bus that was requested by control lines.
  virtual void tick_write() {}

  // Read any data from the bus that was requested by control lines.
  virtual void tick_read() {}

  // Do any local processing for this tick.
  virtual void tick_process() {}

  // Clear any control lines and any other state that was set up this tick.
  virtual void tick_clear() {}

private:
  // The name of the component.
  std::string name_;

  // The parent of the component, or nullptr if the component has no parent.
  Component *parent_ = nullptr;

  // The children of the component, keyed by name.
  // Note that the children are not owned by this map.
  std::map<std::string, Component *> children_;

  // Traverse this component's subtree, calling func on each component.
  void traverse(std::function<void(Component *)> func);
};

} // namespace irata::sim