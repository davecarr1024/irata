#pragma once

#include <irata/sim/bytes/byte.hpp>
#include <irata/sim/bytes/word.hpp>
#include <irata/sim/components/component.hpp>
#include <string_view>

namespace irata::sim::components::memory {

// A module is a component that can be read from and written to.
// It has a size in bytes and can be read from and written to at a given
// address.
// Modules are not connected to a bus, but can be read from and written to
// by other components.
// Modules are by default read-only but can be made writable by overriding
// the can_write and write methods.
class Module : public Component {
public:
  // Deliberately don't allow parent to be set here. Modules are always
  // children of a region and are connected when the region is created.
  explicit Module(std::string_view name) : Component(name, nullptr) {}
  virtual ~Module() = default;

  // Returns the size of the module in bytes.
  // Must be a power of 2.
  virtual size_t size() const = 0;

  // Returns the byte at the given address.
  virtual Byte read(Word address) const = 0;

  // Writes the given byte to the given address.
  // To make a read-write module, override this method and the can_write method.
  virtual void write(Word address, Byte value) = 0;

  // Returns true if the module can be written to.
  // Returns false by default.
  virtual bool can_write() const { return false; }
};

} // namespace irata::sim::components::memory
