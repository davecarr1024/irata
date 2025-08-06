#include <irata/sim/components/memory/rom.hpp>
#include <stdexcept>

namespace irata::sim::components::memory {

ROM::ROM(size_t size, std::string_view name, std::map<Word, Byte> data)
    : Module(name), size_(size), data_(std::move(data)) {
  // Throw an exception if any data is outside [0,size).
  for (const auto &[address, _] : data_) {
    if (address.value() >= size) {
      throw std::invalid_argument("address " + std::to_string(address.value()) +
                                  " out of range for ROM " + path());
    }
  }
  // Throw an exception if the size is not a power of 2.
  if ((size & (size - 1)) != 0) {
    throw std::invalid_argument("size " + std::to_string(size) +
                                " is not a power of 2");
  }
}

namespace {

std::map<Word, Byte> load(size_t size, std::istream &is) {
  std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(is)),
                             (std::istreambuf_iterator<char>()));
  if (bytes.size() >= size) {
    throw std::invalid_argument("loaded data size " + std::to_string(size) +
                                " larger than rom size " +
                                std::to_string(size));
  }
  std::map<Word, Byte> data;
  for (size_t i = 0; i < bytes.size(); ++i) {
    if (const auto byte = bytes[i]; byte != 0) {
      data[Word(i)] = byte;
    }
  }
  return data;
}

} // namespace

ROM::ROM(size_t size, std::istream &is, std::string_view name)
    : ROM(size, name, load(size, is)) {}

size_t ROM::size() const { return size_; }

const std::map<Word, Byte> &ROM::data() const { return data_; }

Byte ROM::read(Word address) const {
  if (address.value() >= size_) {
    throw std::out_of_range("address " + std::to_string(address.value()) +
                            " out of range for ROM " + path());
  }
  if (auto it = data_.find(address); it != data_.end()) {
    return it->second;
  } else {
    return Byte(0x00);
  }
}

void ROM::write(Word address, Byte value) {
  throw std::runtime_error("ROM " + path() + " is read-only");
}

} // namespace irata::sim::components::memory
