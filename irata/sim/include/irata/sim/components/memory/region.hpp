#pragma once

#include <irata/sim/components/component.hpp>
#include <irata/sim/components/memory/module.hpp>
#include <optional>
#include <string_view>

namespace irata::sim::components::memory {

// A region is a component that maps a contiguous range of addresses in memory
// to a module.
class Region : public Component {
public:
  // Creates a new region with the given name, module, and offset.
  // The region will map the range [offset, offset + module.size()) to the
  // module.
  // Throws an exception if the offset is not aligned to the size of the module.
  explicit Region(std::string_view name, std::unique_ptr<Module> module,
                  Word offset, Component *parent = nullptr);
  virtual ~Region() = default;

  // Returns the size of the region in bytes.
  // This must be a power of 2.
  size_t size() const;

  // Returns the offset of the region in memory.
  // Must be aligned to the size of the region.
  Word offset() const;

  // Returns true if the region contains the given address.
  bool contains_address(Word address) const;

  // Returns the byte at the given address.
  // Throws an exception if the address is not within the range of the region.
  Byte read(Word address) const;

  // Writes the given byte to the given address.
  // Throws an exception if the address is not within the range of the region.
  // Throws an exception if the region is not writable.
  void write(Word address, Byte value);

  // Returns true if the region can be written to.
  bool can_write() const;

  // Returns true if this region overlaps with the given region.
  // This should never happen, as regions are required to be non-overlapping.
  bool overlaps(const Region &other) const;

private:
  // The contained module.
  std::unique_ptr<Module> module_;

  // The offset of the region in memory.
  const Word offset_;

  // Converts an address in the region to an address in the module.
  // Throws an exception if the address is not within the range of the region.
  Word module_address(Word address) const;
};

} // namespace irata::sim::components::memory
