#include <irata/sim/components/memory/region.hpp>

namespace irata::sim::components::memory {

Region::Region(std::string_view name, Module &module, Word offset,
               Component *parent)
    : Component(name, parent), module_(module), offset_(offset) {
  // Throw an exception if the module size is not a power of 2.
  if ((module.size() & (module.size() - 1)) != 0) {
    throw std::invalid_argument("module size " + std::to_string(module.size()) +
                                " is not a power of 2");
  }
  // Throw an exception if the offset is not aligned to the size of the module.
  if (offset.value() % module.size() != 0) {
    throw std::invalid_argument("offset " + std::to_string(offset.value()) +
                                " is not aligned to the "
                                "size of the module " +
                                std::to_string(module.size()));
  }
  // Add the module as a child component.
  add_child(&module_);
}

size_t Region::size() const { return module_.size(); }

Word Region::offset() const { return offset_; }

bool Region::contains_address(Word address) const {
  return address.value() >= offset_.value() &&
         address.value() < offset_.value() + size();
}

Byte Region::read(Word address) const {
  if (!contains_address(address)) {
    throw std::out_of_range("address " + std::to_string(address.value()) +
                            " out of range for region " + path());
  }
  return module_.read(module_address(address));
}

void Region::write(Word address, Byte value) {
  if (!contains_address(address)) {
    throw std::out_of_range("address " + std::to_string(address.value()) +
                            " out of range for region " + path());
  }
  if (!can_write()) {
    throw std::runtime_error("region " + path() + " is not writable");
  }
  module_.write(module_address(address), value);
}

bool Region::can_write() const { return module_.can_write(); }

Word Region::module_address(Word address) const {
  return Word(address.value() - offset_.value());
}

bool Region::overlaps(const Region &other) const {
  return offset_.value() < other.offset_.value() + other.size() &&
         other.offset_.value() < offset_.value() + size();
}

} // namespace irata::sim::components::memory
