#pragma once

#include <irata/sim/bytes/byte.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/component.hpp>
#include <irata/sim/components/control.hpp>

namespace irata::sim {

// A register is a component that stores a single byte of data.
// It has a write control line that enables writing to the bus,
// a read control line that enables reading from the bus, and a reset control
// line that resets the register to zero.
class Register : public Component {
public:
  // Constructs a register with the given name and bus.
  Register(std::string_view name, Bus &bus, Component *parent = nullptr);
  virtual ~Register() = default;

  // Returns the current value of the register.
  Byte value() const;
  // Sets the current value of the register.
  void set_value(Byte value);

  // Returns the write control line.
  bool write() const;
  // Sets the write control line.
  void set_write(bool value);

  // Returns the read control line.
  bool read() const;
  // Sets the read control line.
  void set_read(bool value);

  // Returns the reset control line.
  bool reset() const;
  // Sets the reset control line.
  void set_reset(bool value);

  // Handle writing to the bus.
  void tick_write(Logger &logger) override;
  // Handle reading from the bus.
  void tick_read(Logger &logger) override;
  // Handle processing the register.
  void tick_process(Logger &logger) override;

private:
  Byte value_;
  Bus &bus_;
  Control write_;
  Control read_;
  Control reset_;
};

} // namespace irata::sim