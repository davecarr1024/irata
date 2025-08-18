#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/component.hpp>
#include <irata/sim/components/control.hpp>
#include <irata/sim/components/counter.hpp>

namespace irata::sim::components::memory {

// Memory address is a specialized word register, used to hold the pointer to
// the active memory byte.
// Both bytes of the address are individually accessible on the data bus, and
// both bytes are incrementable. This allows direct manipulation of the address
// by the CPU.
class Address final : public Component {
public:
  Address(Component &parent, WordBus &address_bus, ByteBus &data_bus);

  hdl::ComponentType type() const override final;

  const WordBus &address_bus() const;
  WordBus &address_bus();

  const ByteBus &data_bus() const;
  ByteBus &data_bus();

  bool read() const;
  void set_read(bool read);
  Control &read_control();
  const Control &read_control() const;

  bool write() const;
  void set_write(bool write);
  Control &write_control();
  const Control &write_control() const;

  bool reset() const;
  void set_reset(bool reset);
  Control &reset_control();
  const Control &reset_control() const;

  Byte high() const;
  void set_high(Byte high);
  Counter &high_register();
  const Counter &high_register() const;

  Byte low() const;
  void set_low(Byte low);
  Counter &low_register();
  const Counter &low_register() const;

  Word value() const;
  void set_value(Word value);

protected:
  void tick_write(Logger &logger) override final;

  void tick_read(Logger &logger) override final;

  void tick_process(Logger &logger) override final;

  void serialize(Serializer &serializer) const override final;

private:
  WordBus &address_bus_;
  ByteBus &data_bus_;
  Control read_;
  Control write_;
  Control reset_;
  Counter high_;
  Counter low_;
};

} // namespace irata::sim::components::memory
