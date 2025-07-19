#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/memory/memory.hpp>
#include <irata/sim/components/register.hpp>

using ::testing::_;
using ::testing::Pointee;
using ::testing::Property;
using ::testing::Return;
using ::testing::UnorderedElementsAre;

namespace irata::sim::components::memory {

namespace {

class MockModule : public Module {
public:
  explicit MockModule(std::string_view name) : Module(name) {}

  MOCK_METHOD(size_t, size, (), (const, override));
  MOCK_METHOD(Byte, read, (Word address), (const, override));
  MOCK_METHOD(void, write, (Word address, Byte value), (override));
  MOCK_METHOD(bool, can_write, (), (const, override));
};

} // namespace

TEST(MemoryTest, Regions) {
  Component root("root");
  MockModule module1("module1");
  MockModule module2("module2");
  EXPECT_CALL(module1, size()).WillRepeatedly(Return(1024));
  EXPECT_CALL(module2, size()).WillRepeatedly(Return(1024));
  Bus<Word> address_bus("address_bus", &root);
  Bus<Byte> data_bus("data_bus", &root);
  std::vector<std::unique_ptr<Region>> regions;
  regions.emplace_back(std::make_unique<Region>("region1", module1, Word(0)));
  regions.emplace_back(
      std::make_unique<Region>("region2", module2, Word(1024)));
  Memory memory("memory", std::move(regions), address_bus, data_bus, &root);
  EXPECT_THAT(memory.regions(),
              UnorderedElementsAre(
                  Pointee(Property(&Region::path, "/memory/region1")),
                  Pointee(Property(&Region::path, "/memory/region2"))));
}

TEST(MemoryTest, OverlappingRegions) {
  Component root("root");
  MockModule module1("module1");
  MockModule module2("module2");
  EXPECT_CALL(module1, size()).WillRepeatedly(Return(1024));
  EXPECT_CALL(module2, size()).WillRepeatedly(Return(2048));
  Bus<Word> address_bus("address_bus", &root);
  Bus<Byte> data_bus("data_bus", &root);
  std::vector<std::unique_ptr<Region>> regions;
  regions.emplace_back(
      std::make_unique<Region>("region1", module1, Word(1024)));
  regions.emplace_back(std::make_unique<Region>("region2", module2, Word(0)));
  EXPECT_THROW(
      Memory("memory", std::move(regions), address_bus, data_bus, &root),
      std::invalid_argument);
}

TEST(MemoryTest, SetAddressThroughBus) {
  // Build a read-only 1k memory starting at address 0.
  Component root("root");
  MockModule module("module");
  EXPECT_CALL(module, size()).WillRepeatedly(Return(1024));
  EXPECT_CALL(module, can_write()).WillRepeatedly(Return(false));
  Bus<Word> address_bus("address_bus", &root);
  Bus<Byte> data_bus("data_bus", &root);
  std::vector<std::unique_ptr<Region>> regions;
  regions.emplace_back(std::make_unique<Region>("region", module, Word(0)));
  Memory memory("memory", std::move(regions), address_bus, data_bus, &root);

  // Create a word register sp to write the address from.
  WordRegister sp("sp", &address_bus, nullptr, &root);

  // Write the address to the memory address register.
  sp.set_value(Word(0x0012));
  sp.set_write(true);
  memory.address_register().set_read(true);
  root.tick();
  EXPECT_EQ(memory.address(), Word(0x0012));
}

TEST(MemoryTest, Read) {
  // Build a memory with a read only 1k region starting at address 0.
  Component root("root");
  MockModule module("module");
  EXPECT_CALL(module, size()).WillRepeatedly(Return(1024));
  EXPECT_CALL(module, can_write()).WillRepeatedly(Return(false));
  Bus<Word> address_bus("address_bus", &root);
  Bus<Byte> data_bus("data_bus", &root);
  std::vector<std::unique_ptr<Region>> regions;
  regions.emplace_back(std::make_unique<Region>("region", module, Word(0)));
  Memory memory("memory", std::move(regions), address_bus, data_bus, &root);

  // Create a data register a to write data to.
  Register a("a", &data_bus, &root);

  // Read from the memory at the selected address to a.
  memory.set_address(Word(0x0012));
  EXPECT_CALL(module, read(Word(0x0012))).WillOnce(Return(Byte(0xAB)));
  memory.set_write(true);
  a.set_read(true);
  root.tick();
  EXPECT_EQ(a.value(), Byte(0xAB));
}

TEST(MemoryTest, ReadUnmappedAddress) {
  // Build a memory with a read only 1k region starting at address 0.
  Component root("root");
  MockModule module("module");
  EXPECT_CALL(module, size()).WillRepeatedly(Return(1024));
  EXPECT_CALL(module, can_write()).WillRepeatedly(Return(false));
  Bus<Word> address_bus("address_bus", &root);
  Bus<Byte> data_bus("data_bus", &root);
  std::vector<std::unique_ptr<Region>> regions;
  regions.emplace_back(std::make_unique<Region>("region", module, Word(0)));
  Memory memory("memory", std::move(regions), address_bus, data_bus, &root);

  // Create a data register a to write data to.
  Register a("a", &data_bus, &root);

  // Try to read from an unmapped address to a.
  memory.set_address(Word(0x0400));
  a.set_read(true);
  memory.set_write(true);
  EXPECT_THROW(root.tick(), std::runtime_error);
}

TEST(MemoryTest, Write) {
  // Build a memory with a read-write 1k region starting at address 0.
  Component root("root");
  MockModule module("module");
  EXPECT_CALL(module, size()).WillRepeatedly(Return(1024));
  EXPECT_CALL(module, can_write()).WillRepeatedly(Return(true));
  Bus<Word> address_bus("address_bus", &root);
  Bus<Byte> data_bus("data_bus", &root);
  std::vector<std::unique_ptr<Region>> regions;
  regions.emplace_back(std::make_unique<Region>("region", module, Word(0)));
  Memory memory("memory", std::move(regions), address_bus, data_bus, &root);

  // Create a data register a to write data from.
  Register a("a", &data_bus, &root);

  // Write to the memory at the selected address from a.
  memory.set_address(Word(0x0012));
  EXPECT_CALL(module, write(Word(0x0012), Byte(0xAB)));
  a.set_value(Byte(0xAB));
  memory.set_read(true);
  a.set_write(true);
  root.tick();
}

TEST(MemoryTest, WriteUnmappedAddress) {
  // Build a memory with a read-write 1k region starting at address 0.
  Component root("root");
  MockModule module("module");
  EXPECT_CALL(module, size()).WillRepeatedly(Return(1024));
  EXPECT_CALL(module, can_write()).WillRepeatedly(Return(true));
  Bus<Word> address_bus("address_bus", &root);
  Bus<Byte> data_bus("data_bus", &root);
  std::vector<std::unique_ptr<Region>> regions;
  regions.emplace_back(std::make_unique<Region>("region", module, Word(0)));
  Memory memory("memory", std::move(regions), address_bus, data_bus, &root);

  // Create a data register a to write data from.
  Register a("a", &data_bus, &root);

  // Try to write to an unmapped address from a.
  a.set_write(true);
  memory.set_read(true);
  memory.set_address(Word(0x0400));
  EXPECT_CALL(module, write(_, _)).Times(0);
  EXPECT_THROW(root.tick(), std::runtime_error);
}

TEST(MemoryTest, WriteReadOnlyRegion) {
  // Build a memory with a read-only 1k region starting at address 0.
  Component root("root");
  MockModule module("module");
  EXPECT_CALL(module, size()).WillRepeatedly(Return(1024));
  EXPECT_CALL(module, can_write()).WillRepeatedly(Return(false));
  Bus<Word> address_bus("address_bus", &root);
  Bus<Byte> data_bus("data_bus", &root);
  std::vector<std::unique_ptr<Region>> regions;
  regions.emplace_back(std::make_unique<Region>("region", module, Word(0)));
  Memory memory("memory", std::move(regions), address_bus, data_bus, &root);

  // Create a data register a to write data from.
  Register a("a", &data_bus, &root);

  // Try to write to the read-only region from a.
  a.set_write(true);
  memory.set_read(true);
  memory.set_address(Word(0x0012));
  EXPECT_CALL(module, write(_, _)).Times(0);
  EXPECT_THROW(root.tick(), std::runtime_error);
}

TEST(MemoryTest, WriteNoDataOnBus) {
  // Build a memory with a read-write 1k region starting at address 0.
  Component root("root");
  MockModule module("module");
  EXPECT_CALL(module, size()).WillRepeatedly(Return(1024));
  EXPECT_CALL(module, can_write()).WillRepeatedly(Return(true));
  Bus<Word> address_bus("address_bus", &root);
  Bus<Byte> data_bus("data_bus", &root);
  std::vector<std::unique_ptr<Region>> regions;
  regions.emplace_back(std::make_unique<Region>("region", module, Word(0)));
  Memory memory("memory", std::move(regions), address_bus, data_bus, &root);

  // No source register is connected to the data bus, so there will be no
  // data on the bus during tick_read.

  // Try to write to the memory at the selected address from a, but don't
  // write anything to the bus.
  memory.set_address(Word(0x0012));
  memory.set_read(true);
  EXPECT_CALL(module, write(_, _)).Times(0);
  EXPECT_THROW(root.tick(), std::runtime_error);
}

} // namespace irata::sim::components::memory
