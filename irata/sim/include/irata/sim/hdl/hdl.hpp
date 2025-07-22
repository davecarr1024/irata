#pragma once

// Hardware Description Language (HDL) declarations for the Irata CPU.
// This file defines the static structure of the Irata simulated hardware,
// including buses, registers, control lines, and core components.
// The HDL is used for:
//   - Generating and validating microcode
//   - Describing and checking the architecture of simulated hardware
//   - Providing a reference topology for compiler and test tooling
//
// All components are immutable at compile time and structured as a tree.
// Components are accessed through hierarchical composition, not by name lookup.

#include <string>
#include <string_view>

namespace irata::sim::hdl {

// Defines the ordered phases of a single tick in the CPU.
// Used by the microcode compiler to:
//   - enforce correct execution order
//   - identify when control lines are consumed
//   - support collapsing compatible operations within a tick
enum class TickPhase {
  Control, // Control lines are asserted
  Write,   // Values are written onto buses
  Read,    // Values are read from buses
  Process, // Local, internal component updates
  Clear    // Cleanup of latched or asserted control signals
};

//------------------------------------------------------------------------------
// Component Base Types
//------------------------------------------------------------------------------

class ComponentDecl {
public:
  ComponentDecl(std::string_view name, const ComponentDecl *parent)
      : name_(name), parent_(parent) {}
  ~ComponentDecl() = default;

  ComponentDecl(const ComponentDecl &) = delete;
  ComponentDecl &operator=(const ComponentDecl &) = delete;

  const std::string &name() const { return name_; }
  const ComponentDecl *parent() const { return parent_; }

  const ComponentDecl *root() const { return parent_ ? parent_->root() : this; }

  std::string path() const {
    if (!parent_)
      return "/";
    return parent_->path() + (parent_->parent() ? "/" : "") + name_;
  }

private:
  const std::string name_;
  ComponentDecl const *const parent_;
};

class ComponentWithParentDecl : public ComponentDecl {
public:
  ComponentWithParentDecl(std::string_view name, const ComponentDecl &parent)
      : ComponentDecl(name, &parent) {}
};

//------------------------------------------------------------------------------
// Buses
//------------------------------------------------------------------------------

class BusDecl : public ComponentWithParentDecl {
public:
  BusDecl(std::string_view name, const ComponentDecl &parent)
      : ComponentWithParentDecl(name, parent) {}
};

class ByteBusDecl final : public BusDecl {
public:
  ByteBusDecl(std::string_view name, const ComponentDecl &parent)
      : BusDecl(name, parent) {}
};

class WordBusDecl final : public BusDecl {
public:
  WordBusDecl(std::string_view name, const ComponentDecl &parent)
      : BusDecl(name, parent) {}
};

//------------------------------------------------------------------------------
// Control Signals
//------------------------------------------------------------------------------

class ControlDecl final : public ComponentWithParentDecl {
public:
  ControlDecl(std::string_view name, const ComponentDecl &parent,
              TickPhase phase)
      : ComponentWithParentDecl(name, parent), phase_(phase) {}

  TickPhase phase() const { return phase_; }

private:
  const TickPhase phase_;
};

//------------------------------------------------------------------------------
// Status Signals
// -----------------------------------------------------------------------------

class StatusDecl final : public ComponentWithParentDecl {
public:
  StatusDecl(std::string_view name, const ComponentDecl &parent)
      : ComponentWithParentDecl(name, parent) {}
};

//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------

class DisconnectedByteRegisterDecl : public ComponentWithParentDecl {
public:
  DisconnectedByteRegisterDecl(std::string_view name,
                               const ComponentDecl &parent)
      : ComponentWithParentDecl(name, parent),
        reset_("reset", *this, TickPhase::Process) {}

  const ControlDecl &reset() const { return reset_; }

private:
  const ControlDecl reset_;
};

class ConnectedByteRegisterDecl : public DisconnectedByteRegisterDecl {
public:
  ConnectedByteRegisterDecl(std::string_view name, const ComponentDecl &parent,
                            const ByteBusDecl &bus)
      : DisconnectedByteRegisterDecl(name, parent), bus_(bus),
        write_("write", *this, TickPhase::Write),
        read_("read", *this, TickPhase::Read) {}

  const ByteBusDecl &bus() const { return bus_; }
  const ControlDecl &write() const { return write_; }
  const ControlDecl &read() const { return read_; }

private:
  const ByteBusDecl &bus_;
  const ControlDecl write_;
  const ControlDecl read_;
};

class IncrementableConnectedByteRegisterDecl final
    : public ConnectedByteRegisterDecl {
public:
  IncrementableConnectedByteRegisterDecl(std::string_view name,
                                         const ComponentDecl &parent,
                                         const ByteBusDecl &bus)
      : ConnectedByteRegisterDecl(name, parent, bus),
        increment_("increment", *this, TickPhase::Process) {}

  const ControlDecl &increment() const { return increment_; }

private:
  const ControlDecl increment_;
};

class ConnectedWordRegisterDecl : public ComponentWithParentDecl {
public:
  ConnectedWordRegisterDecl(std::string_view name, const ComponentDecl &parent,
                            const WordBusDecl &bus)
      : ComponentWithParentDecl(name, parent), bus_(bus),
        write_("write", *this, TickPhase::Write),
        read_("read", *this, TickPhase::Read),
        reset_("reset", *this, TickPhase::Process), high_("high", *this),
        low_("low", *this) {}

  const WordBusDecl &bus() const { return bus_; }
  const ControlDecl &write() const { return write_; }
  const ControlDecl &read() const { return read_; }
  const ControlDecl &reset() const { return reset_; }

  const DisconnectedByteRegisterDecl &high() const { return high_; }
  const DisconnectedByteRegisterDecl &low() const { return low_; }

private:
  const WordBusDecl &bus_;
  const ControlDecl write_;
  const ControlDecl read_;
  const ControlDecl reset_;
  const DisconnectedByteRegisterDecl high_;
  const DisconnectedByteRegisterDecl low_;
};

class IncrementableConnectedWordRegisterDecl final
    : public ConnectedWordRegisterDecl {
public:
  IncrementableConnectedWordRegisterDecl(std::string_view name,
                                         const ComponentDecl &parent,
                                         const WordBusDecl &bus)
      : ConnectedWordRegisterDecl(name, parent, bus),
        increment_("increment", *this, TickPhase::Process) {}

  const ControlDecl &increment() const { return increment_; }

private:
  const ControlDecl increment_;
};

//------------------------------------------------------------------------------
// Memory
//------------------------------------------------------------------------------

class MemoryDecl final : public ComponentWithParentDecl {
public:
  MemoryDecl(std::string_view name, const ComponentDecl &parent,
             const WordBusDecl &address_bus, const ByteBusDecl &data_bus)
      : ComponentWithParentDecl(name, parent), address_bus_(address_bus),
        data_bus_(data_bus), address_register_("address", *this, address_bus),
        write_("write", *this, TickPhase::Write),
        read_("read", *this, TickPhase::Read) {}

  const WordBusDecl &address_bus() const { return address_bus_; }
  const ByteBusDecl &data_bus() const { return data_bus_; }
  const ConnectedWordRegisterDecl &address_register() const {
    return address_register_;
  }
  const ControlDecl &write() const { return write_; }
  const ControlDecl &read() const { return read_; }

private:
  const WordBusDecl &address_bus_;
  const ByteBusDecl &data_bus_;
  const ConnectedWordRegisterDecl address_register_;
  const ControlDecl write_;
  const ControlDecl read_;
};

//------------------------------------------------------------------------------
// Controller
//------------------------------------------------------------------------------

class ControllerDecl final : public ComponentWithParentDecl {
public:
  ControllerDecl(std::string_view name, const ComponentDecl &parent,
                 const ByteBusDecl &bus)
      : ComponentWithParentDecl(name, parent),
        step_counter_("step_counter", *this, bus),
        opcode_("opcode", *this, bus) {}

  const IncrementableConnectedByteRegisterDecl &step_counter() const {
    return step_counter_;
  }

  const ConnectedByteRegisterDecl &opcode() const { return opcode_; }

private:
  const IncrementableConnectedByteRegisterDecl step_counter_;
  const ConnectedByteRegisterDecl opcode_;
};

//------------------------------------------------------------------------------
// CPU
//------------------------------------------------------------------------------

class CpuDecl final : public ComponentWithParentDecl {
public:
  CpuDecl(std::string_view name, const ComponentDecl &parent,
          const WordBusDecl &address_bus, const ByteBusDecl &data_bus)
      : ComponentWithParentDecl(name, parent), a_("a", *this, data_bus),
        x_("x", *this, data_bus), y_("y", *this, data_bus),
        program_counter_("program_counter", *this, address_bus),
        controller_("controller", *this, data_bus) {}

  const ConnectedByteRegisterDecl &a() const { return a_; }
  const ConnectedByteRegisterDecl &x() const { return x_; }
  const ConnectedByteRegisterDecl &y() const { return y_; }
  const IncrementableConnectedWordRegisterDecl &program_counter() const {
    return program_counter_;
  }
  const ControllerDecl &controller() const { return controller_; }

private:
  const ConnectedByteRegisterDecl a_;
  const ConnectedByteRegisterDecl x_;
  const ConnectedByteRegisterDecl y_;
  const IncrementableConnectedWordRegisterDecl program_counter_;
  const ControllerDecl controller_;
};

//------------------------------------------------------------------------------
// Root Declaration
//------------------------------------------------------------------------------

class IrataDecl final : public ComponentDecl {
public:
  IrataDecl()
      : ComponentDecl("irata", nullptr), data_bus_("data_bus", *this),
        address_bus_("address_bus", *this),
        memory_("memory", *this, address_bus_, data_bus_),
        cpu_("cpu", *this, address_bus_, data_bus_) {}

  const ByteBusDecl &data_bus() const { return data_bus_; }
  const WordBusDecl &address_bus() const { return address_bus_; }
  const MemoryDecl &memory() const { return memory_; }
  const CpuDecl &cpu() const { return cpu_; }

private:
  const ByteBusDecl data_bus_;
  const WordBusDecl address_bus_;
  const MemoryDecl memory_;
  const CpuDecl cpu_;
};

// Singleton instance getter for the Irata root declaration.
static const IrataDecl &irata() {
  static const IrataDecl irata;
  return irata;
}

} // namespace irata::sim::hdl
