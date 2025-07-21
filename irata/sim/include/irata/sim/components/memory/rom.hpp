#pragma once

#include <irata/sim/bytes/byte.hpp>
#include <irata/sim/bytes/word.hpp>
#include <irata/sim/components/memory/module.hpp>
#include <map>
#include <string_view>

namespace irata::sim::components::memory {

// A ROM is a read-only memory module.
class ROM : public Module {
public:
  // Creates a new ROM with the given size and data.
  // The size must be a power of 2.
  // The data is a map from addresses to bytes.
  // Addresses must be within the range [0, size).
  explicit ROM(size_t size, std::string_view name = "rom",
               std::map<Word, Byte> data = {});
  virtual ~ROM() = default;

  // Returns the size of the ROM in bytes.
  size_t size() const override;

  // Returns the data of the ROM.
  const std::map<Word, Byte> &data() const;

  // Returns the byte at the given address.
  // Throws an exception if the address is out of range [0,size).
  Byte read(Word address) const override;

  // Read only.
  bool can_write() const override { return false; }

private:
  const size_t size_;
  const std::map<Word, Byte> data_;
};

} // namespace irata::sim::components::memory
