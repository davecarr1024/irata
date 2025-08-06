#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/memory/rom.hpp>
#include <stdexcept>

using ::testing::IsEmpty;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;

namespace irata::sim::components::memory {

TEST(RomTest, Size) {
  ROM rom(1024);
  EXPECT_EQ(rom.size(), 1024);
}

TEST(RomTest, SizeNotPowerOfTwo) {
  EXPECT_THROW(ROM(1023), std::invalid_argument);
}

TEST(RomTest, DataTooLarge) {
  EXPECT_THROW(ROM(1024, "rom", {{Word(0x0400), Byte(0x34)}}),
               std::invalid_argument);
}

TEST(RomTest, Data) {
  ROM rom(1024, "rom",
          {{Word(0x0001), Byte(0x12)}, {Word(0x0002), Byte(0x34)}});
  EXPECT_THAT(rom.data(), UnorderedElementsAre(Pair(Word(0x0001), Byte(0x12)),
                                               Pair(Word(0x0002), Byte(0x34))));
}

TEST(RomTest, Read) {
  ROM rom(1024, "rom", {{Word(0x0001), Byte(0x12)}});
  EXPECT_EQ(rom.read(Word(0x0001)), Byte(0x12));
}

TEST(RomTest, ReadDefaultZero) {
  ROM rom(1024);
  EXPECT_EQ(rom.read(Word(0x0000)), Byte(0x00));
}

TEST(RomTest, ReadOutOfBounds) {
  ROM rom(1024);
  EXPECT_THROW(rom.read(Word(0x0400)), std::out_of_range);
}

TEST(RomTest, Write) {
  ROM rom(1024);
  EXPECT_FALSE(rom.can_write());
  EXPECT_THROW(rom.write(Word(0x0001), Byte(0x12)), std::runtime_error);
}

TEST(RomTest, Load_Empty) {
  std::istringstream is("");
  auto rom = ROM(1024, is);
  EXPECT_THAT(rom.data(), IsEmpty());
  EXPECT_EQ(rom.size(), 1024);
}

TEST(RomTest, Load_Bytes) {
  std::string s;
  s += char(0x12);
  s += char(0x34);
  s += char(0x56);
  std::istringstream is(s);
  auto rom = ROM(1024, is);
  EXPECT_THAT(rom.data(), UnorderedElementsAre(Pair(Word(0x0000), Byte(0x12)),
                                               Pair(Word(0x0001), Byte(0x34)),
                                               Pair(Word(0x0002), Byte(0x56))));
  EXPECT_EQ(rom.size(), 1024);
}

TEST(RomTest, Load_Zero) {
  std::string s;
  s += char(0x00);
  s += char(0x12);
  std::istringstream is(s);
  const auto rom = ROM(1024, is);
  EXPECT_THAT(rom.data(), UnorderedElementsAre(Pair(Word(1), Byte(0x12))));
  EXPECT_EQ(rom.size(), 1024);
}

TEST(RomTest, Load_TooLarge) {
  std::string s(1025, char(0x12));
  std::istringstream is(s);
  EXPECT_THROW(ROM(1024, is), std::invalid_argument);
}

} // namespace irata::sim::components::memory
