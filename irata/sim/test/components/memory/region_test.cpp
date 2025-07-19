#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/memory/region.hpp>

using ::testing::_;
using ::testing::Return;

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

TEST(RegionTest, Size) {
  Component root("root");
  MockModule module("module");
  EXPECT_CALL(module, size()).WillRepeatedly(Return(1024));
  Region region("region", module, Word(0), &root);
  EXPECT_EQ(region.size(), 1024);
}

TEST(RegionTest, SizeNotPowerOfTwo) {
  Component root("root");
  MockModule module("module");
  EXPECT_CALL(module, size()).WillRepeatedly(Return(1023));
  EXPECT_THROW(Region("region", module, Word(0), &root), std::invalid_argument);
}

TEST(RegionTest, Offset) {
  Component root("root");
  MockModule module("module");
  EXPECT_CALL(module, size()).WillRepeatedly(Return(1024));
  Region region("region", module, Word(2048), &root);
  EXPECT_EQ(region.offset(), Word(2048));
}

TEST(RegionTest, OffsetNotAligned) {
  Component root("root");
  MockModule module("module");
  EXPECT_CALL(module, size()).WillRepeatedly(Return(1024));
  EXPECT_THROW(Region("region", module, Word(2049), &root),
               std::invalid_argument);
}

TEST(RegionTest, ContainsAddress) {
  Component root("root");
  MockModule module("module");
  EXPECT_CALL(module, size()).WillRepeatedly(Return(1024));
  Region region("region", module, Word(2048), &root);
  EXPECT_TRUE(region.contains_address(Word(2048)));
  EXPECT_TRUE(region.contains_address(Word(3071)));
  EXPECT_FALSE(region.contains_address(Word(2047)));
  EXPECT_FALSE(region.contains_address(Word(3072)));
}

TEST(RegionTest, Read) {
  Component root("root");
  MockModule module("module");
  EXPECT_CALL(module, size()).WillRepeatedly(Return(1024));
  Region region("region", module, Word(2048), &root);
  EXPECT_CALL(module, read(Word(0x0001))).WillOnce(Return(Byte(0x12)));
  EXPECT_EQ(region.read(Word(2049)), Byte(0x12));
}

TEST(RegionTest, ReadOutOfRange) {
  Component root("root");
  MockModule module("module");
  EXPECT_CALL(module, size()).WillRepeatedly(Return(1024));
  Region region("region", module, Word(2048), &root);
  EXPECT_THROW(region.read(Word(2047)), std::out_of_range);
  EXPECT_THROW(region.read(Word(3072)), std::out_of_range);
}

TEST(RegionTest, Write) {
  Component root("root");
  MockModule module("module");
  EXPECT_CALL(module, size()).WillRepeatedly(Return(1024));
  EXPECT_CALL(module, can_write()).WillRepeatedly(Return(true));
  Region region("region", module, Word(2048), &root);
  EXPECT_CALL(module, write(Word(0x0001), Byte(0x12)));
  region.write(Word(2049), Byte(0x12));
}

TEST(RegionTest, WriteOutOfRange) {
  Component root("root");
  MockModule module("module");
  EXPECT_CALL(module, size()).WillRepeatedly(Return(1024));
  EXPECT_CALL(module, can_write()).WillRepeatedly(Return(true));
  EXPECT_CALL(module, write(_, _)).Times(0);
  Region region("region", module, Word(2048), &root);
  EXPECT_THROW(region.write(Word(2047), Byte(0x12)), std::out_of_range);
  EXPECT_THROW(region.write(Word(3072), Byte(0x12)), std::out_of_range);
}

TEST(RegionTest, WriteNotWritable) {
  Component root("root");
  MockModule module("module");
  EXPECT_CALL(module, size()).WillRepeatedly(Return(1024));
  EXPECT_CALL(module, can_write()).WillRepeatedly(Return(false));
  EXPECT_CALL(module, write(_, _)).Times(0);
  Region region("region", module, Word(2048), &root);
  EXPECT_THROW(region.write(Word(2049), Byte(0x12)), std::runtime_error);
}

TEST(RegionTest, CanWrite) {
  Component root("root");
  MockModule module("module");
  EXPECT_CALL(module, size()).WillRepeatedly(Return(1024));
  EXPECT_CALL(module, can_write()).WillRepeatedly(Return(true));
  Region region("region", module, Word(2048), &root);
  EXPECT_TRUE(region.can_write());
}

TEST(RegionTest, CannotWrite) {
  Component root("root");
  MockModule module("module");
  EXPECT_CALL(module, size()).WillRepeatedly(Return(1024));
  EXPECT_CALL(module, can_write()).WillRepeatedly(Return(false));
  Region region("region", module, Word(2048), &root);
  EXPECT_FALSE(region.can_write());
}

TEST(RegionTest, NoOverlap) {
  Component root("root");
  MockModule module1("module1");
  MockModule module2("module2");
  EXPECT_CALL(module1, size()).WillRepeatedly(Return(1024));
  EXPECT_CALL(module2, size()).WillRepeatedly(Return(1024));
  Region region1("region1", module1, Word(2048), &root);
  Region region2("region2", module2, Word(3072), &root);
  EXPECT_FALSE(region1.overlaps(region2));
}

TEST(RegionTest, Overlap) {
  Component root("root");
  MockModule module1("module1");
  MockModule module2("module2");
  EXPECT_CALL(module1, size()).WillRepeatedly(Return(1024));
  EXPECT_CALL(module2, size()).WillRepeatedly(Return(2048));
  Region region1("region1", module1, Word(1024), &root);
  Region region2("region2", module2, Word(0), &root);
  EXPECT_TRUE(region1.overlaps(region2));
  EXPECT_TRUE(region2.overlaps(region1));
}

TEST(RegionTest, RegionHasModuleAsChild) {
  Component root("root");
  MockModule module("module");
  EXPECT_CALL(module, size()).WillRepeatedly(Return(1024));
  Region region("region", module, Word(0), &root);
  EXPECT_EQ(region.child("module"), &module);
}

} // namespace irata::sim::components::memory
