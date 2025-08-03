#pragma once

#include <irata/sim/hdl/bus_decl.hpp>
#include <irata/sim/hdl/component_decl.hpp>
#include <irata/sim/hdl/tick_phase.hpp>

namespace irata::sim::hdl {

// ControlDecl is a declaration of a control line.
// This is used to verify that the control line exists in the component tree
// and has the correct properties.
class ControlDecl : public virtual ComponentDecl {
public:
  // The tick phase during which this control line value is consumed.
  // This is used as a timing constraint to allow reordering control line access
  // in the microcode compiler. See step_merger for more details.
  virtual TickPhase phase() const = 0;
  // Whether this control line is automatically cleared at the end of the tick.
  // If not, a clear child control line will be created for explicit clearing.
  virtual bool auto_clear() const = 0;

  // Verify that the sim component matches the decl.
  void verify(const components::Component *component) const override;
};

std::ostream &operator<<(std::ostream &os, const ControlDecl &control);

// ControlWithPhaseDecl is a mixin for ControlDecl that provides a default
// implementation of phase() based on compile-time constant Phase.
template <TickPhase Phase>
class ControlWithPhaseDecl : public virtual ControlDecl {
public:
  static constexpr TickPhase phase_v = Phase;

  TickPhase phase() const override final { return Phase; }
};

// ControlWithAutoClearDecl is a mixin for ControlDecl that provides an
// interface for compile-time constant auto_clear.
template <bool AutoClear> class ControlWithAutoClearDecl;

// ControlWithAutoClearDecl<true> is a mixin for ControlDecl that provides a
// default implementation of auto clearing controls.
template <> class ControlWithAutoClearDecl<true> : public virtual ControlDecl {
public:
  static constexpr bool auto_clear_v = true;

  bool auto_clear() const override final { return true; }
};

// ControlWithBusDecl is a mixin for ControlDecl that provides an interface
// for a control line that is associated with a bus.
// Read and Write controls declare their connection to the buses they read and
// write to, in order to allow the microcode compiler to reason about bus
// conflicts.
class ControlWithBusDecl : public virtual ControlDecl {
public:
  // The bus associated with this control line.
  const BusDecl &bus() const;

protected:
  ControlWithBusDecl(const BusDecl &bus);

private:
  const BusDecl &bus_;
};

// WriteControlDecl is a declaration of a write control line.
// A write control line causes a bus-connected component to write its values to
// the bus. Write control lines are always auto-cleared.
class WriteControlDecl final
    : public ComponentWithParentDecl<ComponentType::Control>,
      public ControlWithPhaseDecl<TickPhase::Write>,
      public ControlWithAutoClearDecl<true>,
      public ControlWithBusDecl {
public:
  WriteControlDecl(std::string_view name, const ComponentDecl &parent,
                   const BusDecl &bus);

  void verify(const components::Component *component) const override final;
};

// ReadControlDecl is a declaration of a read control line.
// A read control line causes a bus-connected component to read its values from
// the bus. Read control lines are always auto-cleared.
class ReadControlDecl final
    : public ComponentWithParentDecl<ComponentType::Control>,
      public ControlWithPhaseDecl<TickPhase::Read>,
      public ControlWithAutoClearDecl<true>,
      public ControlWithBusDecl {
public:
  ReadControlDecl(std::string_view name, const ComponentDecl &parent,
                  const BusDecl &bus);

  void verify(const components::Component *component) const override final;
};

// ProcessControlDecl is a declaration of a process control line.
// A process control line controls an internal process-phase operation in
// a sim component. Examples are internal calculations and state transitions.
// Process control lines are always auto-cleared.
class ProcessControlDecl final
    : public ComponentWithParentDecl<ComponentType::Control>,
      public ControlWithPhaseDecl<TickPhase::Process>,
      public ControlWithAutoClearDecl<true> {
public:
  ProcessControlDecl(std::string_view name, const ComponentDecl &parent);

  void verify(const components::Component *component) const override final;
};

// ControlWithAutoClearDecl<false> is a mixin for non auto-clearing controls.
// These persistent controls have a clear child control line for explicit
// clearing, which is itself an auto-clearing process control line.
template <> class ControlWithAutoClearDecl<false> : public virtual ControlDecl {
public:
  static constexpr bool auto_clear_v = false;

  bool auto_clear() const override final { return false; }

  const ProcessControlDecl &clear() const { return clear_; }

  void verify(const components::Component *component) const override {
    ControlDecl::verify(component);
    verify_child(clear_, component);
  }

protected:
  ControlWithAutoClearDecl() : clear_("clear", *this) {}

private:
  const ProcessControlDecl clear_;
};

// PersistentProcessControlDecl is a declaration of a persistent process
// control line. These control lines are not auto-cleared and have a reset
// child control line for explicit clearing.
// Persistent process control lines are used to control internal processes
// that need to be reasoned about across ticks, such as instruction state
// latching.
class PersistentProcessControlDecl final
    : public ComponentWithParentDecl<ComponentType::Control>,
      public ControlWithPhaseDecl<TickPhase::Process>,
      public ControlWithAutoClearDecl<false> {
public:
  PersistentProcessControlDecl(std::string_view name,
                               const ComponentDecl &parent);

  void verify(const components::Component *component) const override final;
};

// PersistentClearControlDecl is a declaration of a persistent clear
// control line. These control lines are not auto-cleared and have a reset
// child control line for explicit clearing.
// Persistent clear control lines are used to control internal processes
// that need to be reasoned about across ticks, such as instruction state
// latching.
class PersistentClearControlDecl final
    : public ComponentWithParentDecl<ComponentType::Control>,
      public ControlWithPhaseDecl<TickPhase::Clear>,
      public ControlWithAutoClearDecl<false> {
public:
  PersistentClearControlDecl(std::string_view name,
                             const ComponentDecl &parent);

  void verify(const components::Component *component) const override final;
};

} // namespace irata::sim::hdl
