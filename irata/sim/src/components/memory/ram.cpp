#include <irata/sim/components/memory/ram.hpp>
#include <stdexcept>

namespace irata::sim::components::memory {

RAM::RAM(size_t size, std::string_view name, std::map<Word, Byte> data)
    : Module(name), size_(size), data_(data) {
  // Throw an exception if the size is not a power of 2.
  if ((size & (size - 1)) != 0) {
    throw std::invalid_argument("size " + std::to_string(size) +
                                " is not a power of 2");
  }
  // Throw an exception if the data is not within the range of the RAM.
  for (const auto &[address, _] : data) {
    if (address.value() >= size) {
      throw std::out_of_range("address " + std::to_string(address.value()) +
                              " out of range for RAM " + path());
    }
  }
}

size_t RAM::size() const { return size_; }

const std::map<Word, Byte> &RAM::data() const { return data_; }
std::map<Word, Byte> &RAM::data() { return data_; }

Byte RAM::read(Word address) const {
  if (address.value() >= size_) {
    throw std::out_of_range("address " + std::to_string(address.value()) +
                            " out of range for RAM " + path());
  }
  if (const auto it = data_.find(address); it != data_.end()) {
    return it->second;
  } else {
    return Byte(0x00);
  }
}

void RAM::write(Word address, Byte value) {
  if (address.value() >= size_) {
    throw std::out_of_range("address " + std::to_string(address.value()) +
                            " out of range for RAM " + path());
  }
  data_[address] = value;
}

} // namespace irata::sim::components::memory
