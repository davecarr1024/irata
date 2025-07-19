#pragma once

#include <irata/sim/bytes/byte.hpp>
#include <irata/sim/bytes/word.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/component.hpp>
#include <irata/sim/components/control.hpp>
#include <irata/sim/components/memory/region.hpp>
#include <irata/sim/components/word_register.hpp>
#include <string_view>
#include <vector>

namespace irata::sim::components::memory {

// A memory component that maps a bus of addresses to a bus of data.
// It contains a list of regions, each of which maps a contiguous range of
// addresses to a module.
// Regions must be non-overlapping and aligned.
class Memory : public Component {
public:
  // Creates a new memory component with the given name, regions, and buses.
  // The regions must be non-overlapping and aligned.
  // Throws an exception if the regions are not valid.
  explicit Memory(std::string_view name,
                  std::vector<std::unique_ptr<Region>> regions,
                  Bus<Word> &address_bus, Bus<Byte> &data_bus,
                  Component *parent = nullptr);
  virtual ~Memory() = default;

  // Returns the regions of this memory component.
  const std::vector<std::unique_ptr<Region>> &regions() const;

  // Returns the address bus of this memory component.
  const Bus<Word> &address_bus() const;
  Bus<Word> &address_bus();

  // Returns the data bus of this memory component.
  const Bus<Byte> &data_bus() const;
  Bus<Byte> &data_bus();

  // Returns the address register.
  const WordRegister &address_register() const;
  WordRegister &address_register();
  // Returns the value of the address register.
  Word address() const;
  // Sets the value of the address register.
  void set_address(Word address);

  // Returns the value of the write control line.
  bool write() const;
  // Sets the value of the write control line.
  void set_write(bool write);

  // Returns the value of the read control line.
  bool read() const;
  // Sets the value of the read control line.
  void set_read(bool read);

protected:
  void tick_write(Logger &logger) override;
  void tick_read(Logger &logger) override;

private:
  Bus<Word> &address_bus_;
  Bus<Byte> &data_bus_;
  std::vector<std::unique_ptr<Region>> regions_;
  WordRegister address_;
  Control write_;
  Control read_;

  // Returns the region that contains the given address.
  // Throws an exception if no region contains the given address.
  Region &region(Word address);
};

} // namespace irata::sim::components::memory
