#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/bytes/byte.hpp>
#include <irata/sim/bytes/word.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/component.hpp>
#include <irata/sim/components/memory/memory.hpp>
#include <irata/sim/components/memory/ram.hpp>
#include <irata/sim/components/memory/rom.hpp>
#include <irata/sim/components/register.hpp>
#include <irata/sim/components/word_register.hpp>

using ::testing::Pair;
using ::testing::Pointee;
using ::testing::Property;
using ::testing::UnorderedElementsAre;

namespace irata::sim::components::memory {

namespace {

// A test fixture for the Memory class.
// This fixture provides a component hierarchy with an address bus, a data
// bus, a data register and an address register. The data register is
// connected to the data bus and the address register is connected to the
// address bus.
// Memory can be created with different regions and the data and address
// registers can be used to read and write data to the memory.
class MemoryTest : public ::testing::Test {
protected:
  // Creates a RAM region with the given size, offset, name and data.
  std::unique_ptr<Region> ram(size_t size, Word offset,
                              std::string_view name = "ram",
                              std::map<Word, Byte> data = {}) {
    auto ram = std::make_unique<RAM>(size, name, std::move(data));
    return std::make_unique<Region>(name, std::move(ram), offset);
  }

  // Creates a ROM region with the given size, offset, name and data.
  std::unique_ptr<Region> rom(size_t size, Word offset,
                              std::string_view name = "rom",
                              std::map<Word, Byte> data = {}) {
    auto rom = std::make_unique<ROM>(size, name, std::move(data));
    return std::make_unique<Region>(name, std::move(rom), offset);
  }

  // Creates a memory with the given regions and name.
  Memory memory(std::vector<std::unique_ptr<Region>> regions,
                std::string_view name = "memory") {
    return Memory(name, std::move(regions), address_bus_, data_bus_, &root);
  }

  // Creates a RAM memory with the given size, offset, name and data.
  Memory ram_memory(size_t size = 0x1000, Word offset = Word(0x0000),
                    std::string_view name = "ram",
                    std::map<Word, Byte> data = {}) {
    std::vector<std::unique_ptr<Region>> regions;
    regions.emplace_back(ram(size, offset, name, std::move(data)));
    return memory(std::move(regions), name);
  }

  // Creates a ROM memory with the given size, offset, name and data.
  Memory rom_memory(size_t size = 0x1000, Word offset = Word(0x0000),
                    std::string_view name = "rom",
                    std::map<Word, Byte> data = {}) {
    std::vector<std::unique_ptr<Region>> regions;
    regions.emplace_back(rom(size, offset, name, std::move(data)));
    return memory(std::move(regions), name);
  }

  // Returns the testing data register.
  // This register is connected to the data bus and is used to read and write
  // data to the memory.
  Register &data_register() { return data_register_; }
  // Returns the value of the testing data register.
  Byte data() const { return data_register_.value(); }
  // Sets the value of the testing data register.
  void set_data(Byte value) { data_register_.set_value(value); }

  // Returns the testing address register.
  // This register is connected to the address bus and is used to set the
  // address of the memory.
  WordRegister &address_register() { return address_register_; }
  // Returns the value of the testing address register.
  Word address() const { return address_register_.value(); }
  // Sets the value of the testing address register.
  void set_address(Word value) { address_register_.set_value(value); }

private:
  Component root = Component("root");
  Bus<Word> address_bus_ = Bus<Word>("address_bus", &root);
  Bus<Byte> data_bus_ = Bus<Byte>("data_bus", &root);
  Register data_register_ = Register("data_register", &data_bus_, &root);
  WordRegister address_register_ =
      WordRegister("address_register", &address_bus_, nullptr, &root);
};

} // namespace

TEST_F(MemoryTest, Type) {
  auto memory = this->memory({});
  EXPECT_EQ(memory.type(), hdl::ComponentType::Memory);
}

TEST_F(MemoryTest, Constructor) {
  std::vector<std::unique_ptr<Region>> regions;
  regions.emplace_back(ram(0x1000, Word(0x0000)));
  regions.emplace_back(rom(0x1000, Word(0x1000)));
  auto memory = this->memory(std::move(regions), "memory");
  EXPECT_EQ(memory.name(), "memory");
  EXPECT_EQ(memory.address_bus().path(), "/address_bus");
  EXPECT_EQ(memory.data_bus().path(), "/data_bus");
  EXPECT_THAT(memory.regions(),
              UnorderedElementsAre(
                  Pointee(Property("path", &Region::path, "/memory/ram")),
                  Pointee(Property("path", &Region::path, "/memory/rom"))));
}

TEST_F(MemoryTest, NullRegion) {
  std::vector<std::unique_ptr<Region>> regions;
  regions.emplace_back(nullptr);
  EXPECT_THROW(memory(std::move(regions)), std::invalid_argument);
}

TEST_F(MemoryTest, OverlappingRegions) {
  std::vector<std::unique_ptr<Region>> regions;
  regions.emplace_back(ram(0x2000, Word(0x0000)));
  regions.emplace_back(rom(0x1000, Word(0x1000)));
  EXPECT_THROW(memory(std::move(regions)), std::invalid_argument);
}

TEST_F(MemoryTest, RegionsAreChildren) {
  std::vector<std::unique_ptr<Region>> regions;
  auto ram = this->ram(0x1000, Word(0x0000), "ram");
  auto *ram_ptr = ram.get();
  auto rom = this->rom(0x1000, Word(0x1000), "rom");
  auto *rom_ptr = rom.get();
  regions.emplace_back(std::move(ram));
  regions.emplace_back(std::move(rom));
  auto memory = this->memory(std::move(regions));
  EXPECT_EQ(memory.child("ram"), ram_ptr);
  EXPECT_EQ(ram_ptr->parent(), &memory);
  EXPECT_EQ(memory.child("rom"), rom_ptr);
  EXPECT_EQ(rom_ptr->parent(), &memory);
}

TEST_F(MemoryTest, SetAddressRegister) {
  auto memory = this->memory({});
  EXPECT_EQ(memory.address(), Word(0x0000));
  memory.set_address(Word(0x1234));
  EXPECT_EQ(memory.address(), Word(0x1234));
}

TEST_F(MemoryTest, SetAddressThroughAddressBus) {
  auto memory = this->memory({});

  // Set up external address register for writing.
  set_address(Word(0x1234));
  EXPECT_EQ(address(), Word(0x1234));
  address_register().set_write(true);

  // Set up memory address register for reading.
  EXPECT_EQ(memory.address(), Word(0x0000));
  memory.address_register().set_read(true);

  // Tick the memory.
  memory.tick();

  // Check that the address register was written to.
  EXPECT_EQ(memory.address(), Word(0x1234));
}

TEST_F(MemoryTest, Read) {
  std::map<Word, Byte> data = {{Word(0x00BE), Byte(0xEF)}};
  auto memory = rom_memory(0x1000, Word(0x1000), "rom", std::move(data));
  memory.set_address(Word(0x10BE));
  memory.set_write(true);
  data_register().set_read(true);
  memory.tick();
  EXPECT_EQ(this->data(), Byte(0xEF));
}

TEST_F(MemoryTest, ReadDefaultZero) {
  auto memory = rom_memory(0x1000, Word(0x1000), "rom");
  memory.set_address(Word(0x10BE));
  memory.set_write(true);
  data_register().set_read(true);
  // Set data register to non-zero value to check that it is overwritten.
  set_data(Byte(0x12));
  memory.tick();
  EXPECT_EQ(this->data(), Byte(0x00));
}

TEST_F(MemoryTest, ReadOutOfRange) {
  auto memory = rom_memory(0x1000, Word(0x1000), "rom");
  memory.set_address(Word(0x2000));
  memory.set_write(true);
  data_register().set_read(true);
  EXPECT_THROW(memory.tick(), std::runtime_error);
}

TEST_F(MemoryTest, Write) {
  auto memory = ram_memory(0x1000, Word(0x1000), "ram");

  // Write to memory.
  memory.set_address(Word(0x10BE));
  memory.set_read(true);
  set_data(Byte(0xEF));
  data_register().set_write(true);
  memory.tick();

  // Read from memory.
  memory.set_write(true);
  // Reset data register to zero to check that it is overwritten.
  set_data(Byte(0x00));
  data_register().set_read(true);
  memory.tick();
  EXPECT_EQ(data(), Byte(0xEF));
}

TEST_F(MemoryTest, WriteOutOfRange) {
  auto memory = ram_memory(0x1000, Word(0x1000), "ram");
  memory.set_address(Word(0x2000));
  memory.set_read(true);
  set_data(Byte(0xEF));
  data_register().set_write(true);
  EXPECT_THROW(memory.tick(), std::runtime_error);
}

TEST_F(MemoryTest, WriteToReadOnlyMemory) {
  auto memory = rom_memory(0x1000, Word(0x1000), "rom");
  memory.set_address(Word(0x10BE));
  memory.set_read(true);
  set_data(Byte(0xEF));
  data_register().set_write(true);
  EXPECT_THROW(memory.tick(), std::runtime_error);
}

TEST_F(MemoryTest, WriteOpenBus) {
  auto memory = ram_memory(0x1000, Word(0x1000), "ram");
  memory.set_address(Word(0x10BE));
  memory.set_read(true);
  data_register().set_write(false);
  EXPECT_THROW(memory.tick(), std::runtime_error);
}

} // namespace irata::sim::components::memory
