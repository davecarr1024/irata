#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/bytes/byte.hpp>
#include <irata/sim/bytes/word.hpp>
#include <irata/sim/components/component.hpp>
#include <irata/sim/components/memory/ram.hpp>
#include <irata/sim/components/memory/region.hpp>
#include <irata/sim/components/memory/rom.hpp>
#include <stdexcept>

namespace irata::sim::components::memory {

TEST(RegionTest, Constructor) {
  Component root("root");
  Region region("ram", std::make_unique<RAM>(0x400), Word(0x1000), &root);
  EXPECT_EQ(region.name(), "ram");
  EXPECT_EQ(region.size(), 0x400);
  EXPECT_EQ(region.offset(), Word(0x1000));
  EXPECT_EQ(region.parent(), &root);
  EXPECT_EQ(root.child("ram"), &region);
  EXPECT_EQ(region.path(), "/ram");
}

TEST(RegionTest, NullModule) {
  EXPECT_THROW(Region("ram", nullptr, Word(0x1000)), std::invalid_argument);
}

TEST(RegionTest, ModuleSizeZero) {
  EXPECT_THROW(Region("ram", std::make_unique<RAM>(0x0000), Word(0x1000)),
               std::invalid_argument);
}

TEST(RegionTest, ModuleSizeNotAPowerOfTwo) {
  EXPECT_THROW(Region("ram", std::make_unique<RAM>(0x3FF), Word(0x1000)),
               std::invalid_argument);
}

TEST(RegionTest, UnalignedModule) {
  EXPECT_THROW(Region("ram", std::make_unique<RAM>(0x400), Word(0x1001)),
               std::invalid_argument);
}

TEST(RegionTest, ModuleSizeOverflows) {
  EXPECT_THROW(Region("ram", std::make_unique<RAM>(0x1FFFFFFFF), Word(0x0000)),
               std::invalid_argument);
}

TEST(RegionTest, ModuleSizePlusOffsetOverflows) {
  EXPECT_THROW(Region("ram", std::make_unique<RAM>(0x2000), Word(0xF000)),
               std::invalid_argument);
}

TEST(RegionTest, ModuleIsChildOfRegion) {
  auto ram = std::make_unique<RAM>(0x400);
  auto *ram_ptr = ram.get();
  Region region("ram", std::move(ram), Word(0x1000));
  EXPECT_EQ(ram_ptr->parent(), &region);
  EXPECT_EQ(region.child("ram"), ram_ptr);
}

TEST(RegionTest, ContainsAddress) {
  Region region("ram", std::make_unique<RAM>(0x400), Word(0x1000));
  EXPECT_TRUE(region.contains_address(Word(0x1000)));
  EXPECT_TRUE(region.contains_address(Word(0x13FF)));
  EXPECT_FALSE(region.contains_address(Word(0x0FFF)));
  EXPECT_FALSE(region.contains_address(Word(0x1400)));
}

TEST(RegionTest, Read) {
  std::map<Word, Byte> data = {{Word(0x100), Byte(0x12)}};
  Region region("ram", std::make_unique<RAM>(0x400, "ram", std::move(data)),
                Word(0x1000));
  EXPECT_EQ(region.read(Word(0x1100)), Byte(0x12));
  EXPECT_EQ(region.read(Word(0x1101)), Byte(0x00));
}

TEST(RegionTest, ReadOutOfRange) {
  Region region("ram", std::make_unique<RAM>(0x400), Word(0x1000));
  EXPECT_THROW(region.read(Word(0x1400)), std::out_of_range);
  EXPECT_THROW(region.read(Word(0x0FFF)), std::out_of_range);
}

TEST(RegionTest, Write) {
  Region region("ram", std::make_unique<RAM>(0x400), Word(0x1000));
  EXPECT_TRUE(region.can_write());
  EXPECT_EQ(region.read(Word(0x1100)), Byte(0x00));
  region.write(Word(0x1100), Byte(0x12));
  EXPECT_EQ(region.read(Word(0x1100)), Byte(0x12));
}

TEST(RegionTest, WriteOutOfRange) {
  Region region("ram", std::make_unique<RAM>(0x400), Word(0x1000));
  EXPECT_TRUE(region.can_write());
  EXPECT_THROW(region.write(Word(0x1400), Byte(0x12)), std::out_of_range);
  EXPECT_THROW(region.write(Word(0x0FFF), Byte(0x12)), std::out_of_range);
}

TEST(RegionTest, WriteToReadOnlyModule) {
  Region region("rom", std::make_unique<ROM>(0x400), Word(0x1000));
  EXPECT_FALSE(region.can_write());
  EXPECT_THROW(region.write(Word(0x1100), Byte(0x12)), std::runtime_error);
}

} // namespace irata::sim::components::memory
