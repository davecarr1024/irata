#include <irata/sim/components/memory/memory.hpp>
#include <stdexcept>
#include <utility>

namespace irata::sim::components::memory {

Memory::Memory(std::string_view name,
               std::vector<std::unique_ptr<Region>> regions,
               Bus<Word> &address_bus, Bus<Byte> &data_bus, Component *parent)
    : Component(name, parent), address_bus_(address_bus), data_bus_(data_bus),
      regions_(std::move(regions)),
      address_("address", &address_bus_, nullptr, this), write_("write", this),
      read_("read", this) {
  // Throw an exception if any regions overlap.
  for (size_t i = 0; i < regions_.size(); ++i) {
    for (size_t j = i + 1; j < regions_.size(); ++j) {
      if (regions_[i]->overlaps(*regions_[j]))
        throw std::invalid_argument("regions " + regions_[i]->path() + " and " +
                                    regions_[j]->path() + " in memory " +
                                    path() + " overlap");
    }
  }
  // Add regions as a child component.
  for (auto &region : regions_) {
    add_child(region.get());
  }
}

const std::vector<std::unique_ptr<Region>> &Memory::regions() const {
  return regions_;
}

const Bus<Word> &Memory::address_bus() const { return address_bus_; }
Bus<Word> &Memory::address_bus() { return address_bus_; }

const Bus<Byte> &Memory::data_bus() const { return data_bus_; }
Bus<Byte> &Memory::data_bus() { return data_bus_; }

const WordRegister &Memory::address_register() const { return address_; }
WordRegister &Memory::address_register() { return address_; }

Word Memory::address() const { return address_.value(); }
void Memory::set_address(Word address) { address_.set_value(address); }

bool Memory::write() const { return write_.value(); }
void Memory::set_write(bool write) { write_.set_value(write); }

bool Memory::read() const { return read_.value(); }
void Memory::set_read(bool read) { read_.set_value(read); }

Region &Memory::region(Word address) {
  for (auto &region : regions_) {
    if (region->contains_address(address)) {
      return *region;
    }
  }
  throw std::runtime_error("memory " + path() + " using unmapped address " +
                           std::to_string(address.value()));
}

void Memory::tick_write(Logger &logger) {
  if (write()) {
    // During tick_write, bus devices write their value to the bus. So we need
    // to read the value from memory and write it to the bus.
    const auto address = this->address();
    auto &region = this->region(address);
    const auto data = region.read(address);
    data_bus_.set_value(data, *this);
  }
}

void Memory::tick_read(Logger &logger) {
  if (read()) {
    // During tick_read, bus devices read their value from the bus. So we need
    // to read the value from the bus and write it to memory.
    const auto address = this->address();
    auto &region = this->region(address);
    const auto data = data_bus_.value();
    // Throw an error if there was no data on the bus: this means that nobody
    // was writing to the bus during tick_write.
    if (!data.has_value()) {
      throw std::runtime_error("memory " + path() + " read from address " +
                               std::to_string(address.value()) +
                               " but no data was written to the bus");
    }
    region.write(address, *data);
  }
}

} // namespace irata::sim::components::memory
