#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

namespace irata::sim::microcode::hdl {

// This file contains the declarations for the hardware description language for
// the Irata CPU and associated components. The hardware description language is
// used to generate the microcode for the Irata CPU and to verify the correct
// structure of simulated hardware. The hdl is a const tree of component
// declarations.
// Components are described at compile time and can't be looked up by name, they
// must be accessed programmatically through their parent.

// Base class for all component declarations.
class ComponentDecl {
public:
  // Creates a new component declaration with the given name and parent.
  ComponentDecl(std::string_view name, const ComponentDecl *parent)
      : name_(name), parent_(parent) {}
  ~ComponentDecl() = default;

  // No copying.
  ComponentDecl(const ComponentDecl &) = delete;
  ComponentDecl &operator=(const ComponentDecl &) = delete;

  // The name of the component.
  const std::string &name() const { return name_; }

  // The parent of the component.
  // If the component is the root, this will be nullptr.
  const ComponentDecl *parent() const { return parent_; }

  // The root of the component tree.
  const ComponentDecl *root() const { return parent_ ? parent_->root() : this; }

  // The path of the component.
  // This is the unix-style path of the component, starting with a slash and
  // ending with the name of the component.
  std::string path() const {
    if (parent_ == nullptr) {
      return "/";
    } else if (parent_->parent_ == nullptr) {
      return "/" + name_;
    } else {
      return parent_->path() + "/" + name_;
    }
  }

private:
  const std::string name_;
  ComponentDecl const *const parent_;
};

// Base class for all components with a parent.
class ComponentWithParentDecl : public ComponentDecl {
public:
  // Creates a new component declaration with the given name and parent.
  // Note that the parent has to be specified.
  ComponentWithParentDecl(std::string_view name, const ComponentDecl &parent)
      : ComponentDecl(name, &parent) {}
};

// A control signal.
// This is a signal that is asserted or deasserted by the CPU, this is the basic
// output of the microcode.
class ControlDecl final : public ComponentWithParentDecl {
public:
  ControlDecl(std::string_view name, const ComponentDecl &parent)
      : ComponentWithParentDecl(name, parent) {}
};

// A bus for 8-bit data.
class ByteBusDecl final : public ComponentWithParentDecl {
public:
  ByteBusDecl(std::string_view name, const ComponentDecl &parent)
      : ComponentWithParentDecl(name, parent) {}
};

// A bus for 16-bit data.
class WordBusDecl final : public ComponentWithParentDecl {
public:
  WordBusDecl(std::string_view name, const ComponentDecl &parent)
      : ComponentWithParentDecl(name, parent) {}
};

// A register for 8-bit data that is not connected to a bus.
class DisconnectedByteRegisterDecl : public ComponentWithParentDecl {
public:
  DisconnectedByteRegisterDecl(std::string_view name,
                               const ComponentDecl &parent)
      : ComponentWithParentDecl(name, parent) {}
};

// A register for 8-bit data that is connected to a bus.
// This register has a write and read control signal.
class ConnectedByteRegisterDecl : public DisconnectedByteRegisterDecl {
public:
  ConnectedByteRegisterDecl(std::string_view name, const ComponentDecl &parent,
                            const ByteBusDecl &bus)
      : DisconnectedByteRegisterDecl(name, parent), bus_(bus),
        write_("write", *this), read_("read", *this) {}

  // The bus that the register is connected to.
  const ByteBusDecl &bus() const { return bus_; }

  // The control signal that writes to the register.
  const ControlDecl &write() const { return write_; }

  // The control signal that reads from the register.
  const ControlDecl &read() const { return read_; }

private:
  const ByteBusDecl &bus_;
  const ControlDecl write_;
  const ControlDecl read_;
};

// A register for 8-bit data that is connected to a bus and can be incremented.
// This register has a write and read control signal and an increment control.
class IncrementableConnectedByteRegisterDecl final
    : public ConnectedByteRegisterDecl {
public:
  IncrementableConnectedByteRegisterDecl(std::string_view name,
                                         const ComponentDecl &parent,
                                         const ByteBusDecl &bus)
      : ConnectedByteRegisterDecl(name, parent, bus),
        increment_("increment", *this) {}

  // The control signal that increments the register.
  const ControlDecl &increment() const { return increment_; }

private:
  const ControlDecl increment_;
};

// A register for 16-bit data that is connected to a bus.
// This register is made of two 8-bit registers, high and low, and has a write
// and read control signal.
class ConnectedWordRegisterDecl : public ComponentWithParentDecl {
public:
  ConnectedWordRegisterDecl(std::string_view name, const ComponentDecl &parent,
                            const WordBusDecl &bus)
      : ComponentWithParentDecl(name, parent), bus_(bus),
        write_("write", *this), read_("read", *this), high_("high", *this),
        low_("low", *this) {}

  // The bus that the register is connected to.
  const WordBusDecl &bus() const { return bus_; }

  // The control signal that writes to the register.
  const ControlDecl &write() const { return write_; }

  // The control signal that reads from the register.
  const ControlDecl &read() const { return read_; }

  // The high byte of the register.
  const DisconnectedByteRegisterDecl &high() const { return high_; }

  // The low byte of the register.
  const DisconnectedByteRegisterDecl &low() const { return low_; }

private:
  const WordBusDecl &bus_;
  const ControlDecl write_;
  const ControlDecl read_;
  const DisconnectedByteRegisterDecl high_;
  const DisconnectedByteRegisterDecl low_;
};

// A register for 16-bit data that is connected to a bus and can be incremented.
// This register is made of two 8-bit registers, high and low, and has a write
// and read control signal and an increment control.
class IncrementableConnectedWordRegisterDecl final
    : public ConnectedWordRegisterDecl {
public:
  IncrementableConnectedWordRegisterDecl(std::string_view name,
                                         const ComponentDecl &parent,
                                         const WordBusDecl &bus)
      : ConnectedWordRegisterDecl(name, parent, bus),
        increment_("increment", *this) {}

  // The control signal that increments the register.
  const ControlDecl &increment() const { return increment_; }

private:
  const ControlDecl increment_;
};

// A memory component.
// This component has an address bus, a data bus, an address register, and write
// and read control signals.
class MemoryDecl final : public ComponentWithParentDecl {
public:
  MemoryDecl(std::string_view name, const ComponentDecl &parent,
             const WordBusDecl &address_bus, const ByteBusDecl &data_bus)
      : ComponentWithParentDecl(name, parent), address_bus_(address_bus),
        data_bus_(data_bus), address_register_("address", *this, address_bus),
        write_("write", *this), read_("read", *this) {}

  // The address bus of the memory.
  const WordBusDecl &address_bus() const { return address_bus_; }

  // The data bus of the memory.
  const ByteBusDecl &data_bus() const { return data_bus_; }

  // The address register of the memory.
  // This is the register that holds the address of the memory location that is
  // being read from or written to. It is connected to the address bus.
  const ConnectedWordRegisterDecl &address_register() const {
    return address_register_;
  }

  // The control signal that writes to the memory.
  // This controls the write operation of the memory to the data bus.
  const ControlDecl &write() const { return write_; }

  // The control signal that reads from the memory.
  // This controls the read operation of the memory from the data bus.
  const ControlDecl &read() const { return read_; }

private:
  const WordBusDecl &address_bus_;
  const ByteBusDecl &data_bus_;
  const ConnectedWordRegisterDecl address_register_;
  const ControlDecl write_;
  const ControlDecl read_;
};

// The controller component of the CPU.
// This component has a step counter and an opcode register, it is responsible
// for controlling the execution of the CPU using the encoded microcode.
class ControllerDecl final : public ComponentWithParentDecl {
public:
  ControllerDecl(std::string_view name, const ComponentDecl &parent,
                 const ByteBusDecl &bus)
      : ComponentWithParentDecl(name, parent),
        step_counter_("step_counter", *this, bus),
        opcode_("opcode", *this, bus) {}

  // The step counter of the controller.
  // This is the register that holds the current step of the microcode.
  // It is connected to the data bus and can be incremented.
  const IncrementableConnectedByteRegisterDecl &step_counter() const {
    return step_counter_;
  }

  // The opcode register of the controller.
  // This is the register that holds the current opcode of the CPU.
  // It is connected to the data bus.
  const ConnectedByteRegisterDecl &opcode() const { return opcode_; }

private:
  const IncrementableConnectedByteRegisterDecl step_counter_;
  const ConnectedByteRegisterDecl opcode_;
};

// The CPU component.
// This component has a data bus, an address bus, a program counter, registers,
// and a controller. It is responsible for executing instructions.
class CpuDecl final : public ComponentWithParentDecl {
public:
  CpuDecl(std::string_view name, const ComponentDecl &parent,
          const WordBusDecl &address_bus, const ByteBusDecl &data_bus)
      : ComponentWithParentDecl(name, parent), a_("a", *this, data_bus),
        x_("x", *this, data_bus), y_("y", *this, data_bus),
        program_counter_("program_counter", *this, address_bus),
        controller_("controller", *this, data_bus) {}

  // The accumulator register of the CPU.
  const ConnectedByteRegisterDecl &a() const { return a_; }

  // The x register of the CPU.
  const ConnectedByteRegisterDecl &x() const { return x_; }

  // The y register of the CPU.
  const ConnectedByteRegisterDecl &y() const { return y_; }

  // The program counter of the CPU.
  // This is the register that holds the current address of the instruction that
  // is being executed. It is connected to the address bus and can be
  // incremented.
  const IncrementableConnectedWordRegisterDecl &program_counter() const {
    return program_counter_;
  }

  // The controller of the CPU.
  // This is the component that controls the execution of the CPU using the
  // encoded microcode.
  const ControllerDecl &controller() const { return controller_; }

private:
  const ConnectedByteRegisterDecl a_;
  const ConnectedByteRegisterDecl x_;
  const ConnectedByteRegisterDecl y_;
  const IncrementableConnectedWordRegisterDecl program_counter_;
  const ControllerDecl controller_;
};

// The root component of the Irata computer and associated components.
// This component has a data bus, an address bus, a memory, and a CPU.
class IrataDecl final : public ComponentDecl {
public:
  IrataDecl()
      : ComponentDecl("irata", nullptr), data_bus_("data_bus", *this),
        address_bus_("address_bus", *this),
        memory_("memory", *this, address_bus_, data_bus_),
        cpu_("cpu", *this, address_bus_, data_bus_) {}

  // The data bus of the Irata computer.
  const ByteBusDecl &data_bus() const { return data_bus_; }

  // The address bus of the Irata computer.
  const WordBusDecl &address_bus() const { return address_bus_; }

  // The memory of the Irata computer.
  // This is the memory that the CPU can read from and write to using the data
  // and address buses.
  const MemoryDecl &memory() const { return memory_; }

  // The CPU of the Irata computer.
  const CpuDecl &cpu() const { return cpu_; }

private:
  const ByteBusDecl data_bus_;
  const WordBusDecl address_bus_;
  const MemoryDecl memory_;
  const CpuDecl cpu_;
};

// Returns the root component of the Irata computer and associated components.
static const IrataDecl &irata() {
  static const IrataDecl irata;
  return irata;
}

} // namespace irata::sim::microcode::hdl
