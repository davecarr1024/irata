#pragma once

#include <irata/sim/bytes/word.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/component.hpp>
#include <irata/sim/components/control.hpp>
#include <irata/sim/components/register.hpp>

namespace irata::sim::components {

// A word register is a register that stores a word (two bytes).
// It can be connected to a word bus, and if it is connected it will have
// read and write control lines.
// It has a reset control line that, when asserted, will reset the register
// to zero during tick_process.
// It has low and high registers that store the low and high bytes of the
// word, respectively. These can be connected separately to byte buses.
class WordRegister : public Component {
public:
  // Constructs a word register with the given name.
  // If bus is non-nullptr, the register will be connected to the word
  // bus and read and write control lines will be created.
  // If byte_bus is non-nullptr, the low and high registers will be connected
  // to the byte bus and read and write control lines will be created.
  // If parent is non-nullptr, the word register will be added to the parent
  // component.
  explicit WordRegister(std::string_view name, Bus<Word> *bus = nullptr,
                        Bus<Byte> *byte_bus = nullptr,
                        Component *parent = nullptr);

  // Returns the value of the register.
  Word value() const;
  // Sets the value of the register.
  void set_value(Word value);

  // Returns the high byte of the register.
  Byte high() const;
  // Sets the high byte of the register.
  void set_high(Byte high);
  // Returns the high register.
  Register &high_register();
  const Register &high_register() const;

  // Returns the low byte of the register.
  Byte low() const;
  // Sets the low byte of the register.
  void set_low(Byte low);
  // Returns the low register.
  Register &low_register();
  const Register &low_register() const;

  // Returns the bus that the register is connected to, if any.
  Bus<Word> *bus() const;

  // Returns true if the register has a read control line.
  bool has_read() const;
  // Returns the value of the read control line, if any.
  std::optional<bool> read() const;
  // Sets the value of the read control line, if any.
  // Throws an error if the register does not have a read control line.
  void set_read(bool read);

  // Returns true if the register has a write control line.
  bool has_write() const;
  // Returns the value of the write control line, if any.
  std::optional<bool> write() const;
  // Sets the value of the write control line, if any.
  void set_write(bool write);

  // Returns the value of the reset control line.
  bool reset() const;
  // Sets the value of the reset control line.
  void set_reset(bool reset);

protected:
  // Writes to the bus if the register is connected to a bus and the write
  // control line is asserted.
  void tick_write(Logger &logger) override;

  // Reads from the bus if the register is connected to a bus and the read
  // control line is asserted.
  void tick_read(Logger &logger) override;

  // Resets the register to zero if the reset control line is asserted.
  void tick_process(Logger &logger) override;

private:
  Bus<Word> *const bus_;
  const std::unique_ptr<Control> read_;
  const std::unique_ptr<Control> write_;
  Control reset_;
  Register high_;
  Register low_;
};

} // namespace irata::sim::components