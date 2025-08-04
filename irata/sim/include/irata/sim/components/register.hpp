#pragma once

#include <irata/sim/bytes/byte.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/component.hpp>
#include <irata/sim/components/control.hpp>
#include <optional>

namespace irata::sim::components {

// A register is a component that stores a single byte of data.
// It can be connected to a bus, in which case it will have read and write
// control lines that enable reading from and writing to the bus.
// It can also be reset to zero using a reset control line that is independent
// of bus connection.
class Register : public Component {
public:
  // Constructs a register with the given name.
  // If bus is non-nullptr, the register will be connected to the bus and read
  // and write control lines will be created. If parent is non-nullptr, the
  // register will be added to the parent component.
  explicit Register(std::string_view name, Bus<Byte> *bus = nullptr,
                    Component *parent = nullptr);
  virtual ~Register() = default;

  hdl::ComponentType type() const override final;

  // Returns the current value of the register.
  Byte value() const;
  // Sets the current value of the register.
  void set_value(Byte value);

  // Returns true if the register is connected to a bus.
  bool has_bus() const;
  // Returns the connected bus, or nullptr if the register isn't connected to a
  // bus.
  Bus<Byte> *bus();
  const Bus<Byte> *bus() const;

  // Returns true if the register is connected to a bus and has a write control.
  bool has_write() const;
  // Returns the value of the write control line, if it exists.
  std::optional<bool> write() const;
  // Sets the value of the write control line.
  // If this register isn't connected to a bus, this will throw an error.
  void set_write(bool value);

  // Returns true if the register is connected to a bus and has a read control.
  bool has_read() const;
  // Returns the value of the read control line, if it exists.
  std::optional<bool> read() const;
  // Sets the value of the read control line.
  // If this register isn't connected to a bus, this will throw an error.
  void set_read(bool value);

  // Returns the value of the reset control line.
  bool reset() const;
  // Sets the value of the reset control line.
  void set_reset(bool value);

protected:
  // Handle writing to the bus.
  void tick_write(Logger &logger) override;
  // Handle reading from the bus.
  void tick_read(Logger &logger) override;
  // Handle processing the register.
  void tick_process(Logger &logger) override;

  void serialize(Serializer &serializer) const override;

private:
  Byte value_;
  Bus<Byte> *const bus_;
  const std::unique_ptr<Control> write_;
  const std::unique_ptr<Control> read_;
  Control reset_;
};

} // namespace irata::sim::components