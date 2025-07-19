#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/memory/ram.hpp>
#include <stdexcept>

using ::testing::Pair;
using ::testing::UnorderedElementsAre;

namespace irata::sim::components::memory {

TEST(RamTest, Size) {
  RAM ram(1024);
  EXPECT_EQ(ram.size(), 1024);
}

TEST(RamTest, SizeNotPowerOfTwo) {
  EXPECT_THROW(RAM(1023), std::invalid_argument);
}

TEST(RamTest, Data) {
  RAM ram(1024, "ram",
          {{Word(0x0001), Byte(0xAB)}, {Word(0x0002), Byte(0xCD)}});
  EXPECT_THAT(ram.data(), UnorderedElementsAre(Pair(Word(0x0001), Byte(0xAB)),
                                               Pair(Word(0x0002), Byte(0xCD))));
}

TEST(RamTest, Read) {
  RAM ram(1024, "ram", {{Word(0x0001), Byte(0xAB)}});
  EXPECT_EQ(ram.read(Word(0x0001)), Byte(0xAB));
}

TEST(RamTest, ReadOutOfRange) {
  RAM ram(1024);
  EXPECT_THROW(ram.read(Word(0x0400)), std::out_of_range);
}

TEST(RamTest, ReadDefaultZero) {
  RAM ram(1024);
  EXPECT_EQ(ram.read(Word(0x0001)), Byte(0x00));
}

TEST(RamTest, Write) {
  RAM ram(1024);
  ram.write(Word(0x0001), Byte(0xAB));
  EXPECT_THAT(ram.data(), UnorderedElementsAre(Pair(Word(0x0001), Byte(0xAB))));
}

TEST(RamTest, WriteOutOfRange) {
  RAM ram(1024);
  EXPECT_THROW(ram.write(Word(0x0400), Byte(0xAB)), std::out_of_range);
}

TEST(RamTest, CanWrite) {
  RAM ram(1024);
  EXPECT_TRUE(ram.can_write());
}

} // namespace irata::sim::components::memory
