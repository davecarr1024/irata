#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/memory/rom.hpp>

using ::testing::Pair;
using ::testing::UnorderedElementsAre;

namespace irata::sim::components::memory {

TEST(ROMTest, Size) {
  ROM rom(1024);
  EXPECT_EQ(rom.size(), 1024);
}

TEST(RomTest, SizeNotPowerOfTwo) {
  EXPECT_THROW(ROM(1023), std::invalid_argument);
}

TEST(RomTest, Data) {
  ROM rom(1024, "rom",
          {{Word(0x0000), Byte(0x12)}, {Word(0x0001), Byte(0x34)}});
  EXPECT_THAT(rom.data(), UnorderedElementsAre(Pair(Word(0x0000), Byte(0x12)),
                                               Pair(Word(0x0001), Byte(0x34))));
}

TEST(RomTest, DataOutOfRange) {
  EXPECT_THROW(ROM(1024, "rom", {{Word(0x0400), Byte(0x12)}}),
               std::invalid_argument);
}

TEST(ROMTest, Read) {
  ROM rom(1024, "rom", {{Word(0x0000), Byte(0x12)}});
  EXPECT_EQ(rom.read(Word(0x0000)), Byte(0x12));
}

TEST(ROMTest, DefaultReadZero) {
  ROM rom(1024);
  EXPECT_EQ(rom.read(Word(0x0000)), Byte(0x00));
}

TEST(ROMTest, ReadOutOfRange) {
  ROM rom(1024);
  EXPECT_THROW(rom.read(Word(0x0400)), std::out_of_range);
}

TEST(ROMTest, CannotWrite) {
  ROM rom(1024);
  EXPECT_FALSE(rom.can_write());
}

} // namespace irata::sim::components::memory