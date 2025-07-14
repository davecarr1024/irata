#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>

namespace irata::sim {

class Control;

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
  void tick(std::ostream &log_output = std::cout);

  enum class TickPhase {
    Control,
    Write,
    Read,
    Process,
    Clear,
  };

  class Logger {
  public:
    explicit Logger(std::ostream &output, TickPhase tick_phase);
    ~Logger();

    Logger &for_component(const Component *component);

    template <typename T> Logger &operator<<(const T &value) {
      os_ << value;
      logged_ = true;
      return *this;
    }

  private:
    std::ostringstream os_;
    TickPhase tick_phase_;
    const Component *component_ = nullptr;
    std::ostream &output_;
    bool logged_ = false;
  };

  // Set up any control lines that will be used this tick.
  virtual void tick_control(Logger &logger) {}

  // Write any data to the bus that was requested by control lines.
  virtual void tick_write(Logger &logger) {}

  // Read any data from the bus that was requested by control lines.
  virtual void tick_read(Logger &logger) {}

  // Do any local processing for this tick.
  virtual void tick_process(Logger &logger) {}

  // Clear any control lines and any other state that was set up this tick.
  virtual void tick_clear(Logger &logger) {}

  // Returns a list of all controls in the component tree.
  virtual std::vector<Control *> controls();

private:
  // The name of the component.
  std::string name_;

  // The parent of the component, or nullptr if the component has no parent.
  Component *parent_ = nullptr;

  // The children of the component, keyed by name.
  // Note that the children are not owned by this map.
  std::map<std::string, Component *> children_;

  // Traverse this component's subtree, calling func on each component.
  template <typename Func> void traverse(Func func) {
    func(this);
    for (auto &[_, child] : children_) {
      child->traverse(func);
    }
  }
  template <typename Func> void traverse(Func func) const {
    func(this);
    for (const auto &[_, child] : children_) {
      child->traverse(func);
    }
  }
};

std::ostream &operator<<(std::ostream &os, Component::TickPhase phase);

} // namespace irata::sim