#pragma once

#include <functional>
#include <iomanip>
#include <iostream>
#include <irata/sim/bytes/byte.hpp>
#include <irata/sim/bytes/word.hpp>
#include <irata/sim/hdl/component_type.hpp>
#include <irata/sim/hdl/tick_phase.hpp>
#include <map>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

namespace irata::sim::components {

class Control;
class Status;

// A component is a part of a simulation. It can have children, which are
// also components.
class Component {
public:
  // Constructs a new component with the given name.
  explicit Component(std::string_view name, Component *parent = nullptr);
  virtual ~Component() = default;

  // Disable copying and moving.
  Component(const Component &) = delete;
  Component(Component &&) = delete;
  Component &operator=(const Component &) = delete;
  Component &operator=(Component &&) = delete;

  // The hdl component type of this component.
  // This must match the type of the corresponding hdl component.
  // This defaults to unknown.
  virtual hdl::ComponentType type() const;

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
  void tick(std::ostream &log_output = std::cerr);

  // Returns a list of all controls in the component tree.
  virtual std::vector<Control *> controls();
  std::vector<const Control *> controls() const;

  // Returns a list of all statuses in the component tree.
  virtual std::vector<Status *> statuses();
  std::vector<const Status *> statuses() const;

  // Serialize the whole component tree to the given output stream.
  void serialize_all(std::ostream &os) const;

  // Returns the currently active tick phase, or nullopt if no tick phase is
  // currently active.
  // Note that this is set during a component's tick and its children's ticks,
  // for each phase of the tick.
  std::optional<hdl::TickPhase> active_tick_phase() const;

protected:
  // Helper class used for logging during a tick.
  // Logs a message to the output stream when destroyed, if a message was
  // written to the logger.
  // Output includes the tick phase and the component's path.
  class Logger {
  public:
    explicit Logger(std::ostream &output, hdl::TickPhase tick_phase,
                    const Component &component);
    ~Logger();

    template <typename T> Logger &operator<<(const T &value) {
      os_ << value;
      logged_ = true;
      return *this;
    }

  private:
    std::ostringstream os_;
    hdl::TickPhase tick_phase_;
    const Component &component_;
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

  class Serializer {
  public:
    explicit Serializer(std::ostream &os, size_t tabs = 0);
    Serializer(const Serializer &) = delete;
    Serializer &operator=(const Serializer &) = delete;

    void output(std::string_view value);

    template <typename T> void property(std::string_view name, const T &value) {
      std::ostringstream os;
      os << name << ": " << value;
      output(os.str());
    }

    Serializer with_tab() const;

  private:
    std::ostream &os_;
    const size_t tabs_;
  };

  // Virtual entry point for serializing components.
  virtual void serialize(Serializer &serializer) const {}

private:
  // The name of the component.
  std::string name_;

  // The parent of the component, or nullptr if the component has no parent.
  Component *parent_ = nullptr;

  // The children of the component, keyed by name.
  // Note that the children are not owned by this map.
  std::map<std::string, Component *> children_;

  void tick_traverse(std::ostream &log_output, hdl::TickPhase tick_phase);

  // Traverse this component's subtree, calling serialize on each component.
  void serialize_traverse(Serializer &serializer) const;

  // The currently active tick phase, only set during a tick.
  std::optional<hdl::TickPhase> active_tick_phase_ = std::nullopt;

  void set_active_tick_phase(std::optional<hdl::TickPhase> tick_phase);
};

} // namespace irata::sim::components