#include <irata/sim/components/memory/region.hpp>

namespace irata::sim::components::memory {

Region::Region(std::string_view name, std::unique_ptr<Module> module,
               Word offset, Component *parent)
    : Component(name, parent), module_(std::move(module)), offset_(offset) {
  // Throw an exception if the module is null.
  if (!module_) {
    throw std::invalid_argument("module is null");
  }
  // Throw an exception if the module size is zero.
  if (module_->size() == 0) {
    throw std::invalid_argument("module size is zero");
  }
  // Throw an exeception if the module size overflows the 32 bit address space.
  if (module_->size() > 0xFFFFFFFF) {
    throw std::invalid_argument("module size " +
                                std::to_string(module_->size()) +
                                " overflows the 32 bit address space");
  }
  // Throw an exception if offset + size overflows the 32 bit address space.
  if (offset.value() + module_->size() > 0xFFFFFFFF) {
    throw std::invalid_argument("offset " + std::to_string(offset.value()) +
                                " + size " + std::to_string(module_->size()) +
                                " overflows the 32 bit address space");
  }
  // Throw an exception if the module size is not a power of 2.
  if ((module_->size() & (module_->size() - 1)) != 0) {
    throw std::invalid_argument("module size " +
                                std::to_string(module_->size()) +
                                " is not a power of 2");
  }
  // Throw an exception if the offset is not aligned to the size of the module.
  if (offset.value() % module_->size() != 0) {
    throw std::invalid_argument("offset " + std::to_string(offset.value()) +
                                " is not aligned to the size of the module " +
                                std::to_string(module_->size()));
  }
  // Add the module as a child component.
  add_child(module_.get());
}

size_t Region::size() const { return module_->size(); }

Word Region::offset() const { return offset_; }

bool Region::contains_address(Word address) const {
  return address >= offset_ && address < offset_ + size();
}

Byte Region::read(Word address) const {
  if (!contains_address(address)) {
    throw std::out_of_range("address " + std::to_string(address.value()) +
                            " out of range for region " + path());
  }
  return module_->read(module_address(address));
}

void Region::write(Word address, Byte value) {
  if (!contains_address(address)) {
    throw std::out_of_range("address " + std::to_string(address.value()) +
                            " out of range for region " + path());
  }
  if (!can_write()) {
    throw std::runtime_error("region " + path() + " is not writable");
  }
  module_->write(module_address(address), value);
}

bool Region::can_write() const { return module_->can_write(); }

Word Region::module_address(Word address) const {
  return Word(address.value() - offset_.value());
}

bool Region::overlaps(const Region &other) const {
  return offset_.value() < other.offset_.value() + other.size() &&
         other.offset_.value() < offset_.value() + size();
}

} // namespace irata::sim::components::memory
