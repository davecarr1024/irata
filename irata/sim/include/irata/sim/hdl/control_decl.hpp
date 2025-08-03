#pragma once

#include <irata/sim/hdl/bus_decl.hpp>
#include <irata/sim/hdl/component_decl.hpp>
#include <irata/sim/hdl/tick_phase.hpp>

namespace irata::sim::hdl {

class ControlDecl : public virtual ComponentDecl {
public:
  virtual TickPhase phase() const = 0;
  virtual bool auto_clear() const = 0;

  void verify(const components::Component *component) const override;
};

std::ostream &operator<<(std::ostream &os, const ControlDecl &control);

template <TickPhase Phase>
class ControlWithPhaseDecl : public virtual ControlDecl {
public:
  static constexpr TickPhase phase_v = Phase;

  TickPhase phase() const override final { return Phase; }
};

template <bool AutoClear> class ControlWithAutoClearDecl;

template <> class ControlWithAutoClearDecl<true> : public virtual ControlDecl {
public:
  static constexpr bool auto_clear_v = true;

  bool auto_clear() const override final { return true; }
};

class ControlWithBusDecl : public virtual ControlDecl {
public:
  const BusDecl &bus() const;

protected:
  ControlWithBusDecl(const BusDecl &bus);

private:
  const BusDecl &bus_;
};

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

class ProcessControlDecl final
    : public ComponentWithParentDecl<ComponentType::Control>,
      public ControlWithPhaseDecl<TickPhase::Process>,
      public ControlWithAutoClearDecl<true> {
public:
  ProcessControlDecl(std::string_view name, const ComponentDecl &parent);

  void verify(const components::Component *component) const override final;
};

template <> class ControlWithAutoClearDecl<false> : public virtual ControlDecl {
public:
  static constexpr bool auto_clear_v = false;

  bool auto_clear() const override final { return false; }

  const ProcessControlDecl &reset() const { return clear_; }

  void verify(const components::Component *component) const override {
    ControlDecl::verify(component);
    verify_child(clear_, component);
  }

protected:
  ControlWithAutoClearDecl() : clear_("clear", *this) {}

private:
  const ProcessControlDecl clear_;
};

class PersistentProcessControlDecl final
    : public ComponentWithParentDecl<ComponentType::Control>,
      public ControlWithPhaseDecl<TickPhase::Process>,
      public ControlWithAutoClearDecl<false> {
public:
  PersistentProcessControlDecl(std::string_view name,
                               const ComponentDecl &parent);

  void verify(const components::Component *component) const override final;
};

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
