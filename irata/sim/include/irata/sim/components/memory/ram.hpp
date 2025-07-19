#include <irata/sim/bytes/byte.hpp>
#include <irata/sim/bytes/word.hpp>
#include <irata/sim/components/memory/module.hpp>
#include <map>
#include <string_view>

namespace irata::sim::components::memory {

// A RAM module that can be read from and written to.
class RAM : public Module {
public:
  // Creates a new RAM module with the given size in bytes.
  // The size must be a power of 2.
  // The data is a map from addresses to bytes. If an address is not in the
  // map, it is assumed to be zero.
  // Addresses must be in the range [0, size).
  explicit RAM(size_t size, std::string_view name = "ram",
               std::map<Word, Byte> data = {});
  virtual ~RAM() = default;

  // Returns the size of the RAM in bytes.
  size_t size() const override;

  // Returns the data in the RAM.
  // This is a map from addresses to bytes. If an address is not in the map, it
  // is assumed to be zero.
  const std::map<Word, Byte> &data() const;
  std::map<Word, Byte> &data();

  // Returns the byte at the given address.
  Byte read(Word address) const override;

  // Writes the given byte to the given address.
  void write(Word address, Byte value) override;

  // RAM is writable.
  bool can_write() const override { return true; }

private:
  const size_t size_;
  std::map<Word, Byte> data_;
};

} // namespace irata::sim::components::memory