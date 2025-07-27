#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/components/word_register.hpp>
#include <stdexcept>

using ::testing::HasSubstr;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Optional;

namespace irata::sim::components {

TEST(WordRegisterTest, SetValue) {
  WordRegister reg("register");
  EXPECT_EQ(reg.value(), Word(0));
  EXPECT_EQ(reg.high(), Byte::from_unsigned(0));
  EXPECT_EQ(reg.low(), Byte::from_unsigned(0));
  reg.set_value(Word(0x1234));
  EXPECT_EQ(reg.value(), Word(0x1234));
  EXPECT_EQ(reg.high(), Byte::from_unsigned(0x12));
  EXPECT_EQ(reg.low(), Byte::from_unsigned(0x34));
}

TEST(WordRegisterTest, SetBytes) {
  WordRegister reg("register");
  EXPECT_EQ(reg.value(), Word(0));
  EXPECT_EQ(reg.high(), Byte::from_unsigned(0));
  EXPECT_EQ(reg.low(), Byte::from_unsigned(0));
  reg.set_high(Byte::from_unsigned(0x12));
  reg.set_low(Byte::from_unsigned(0x34));
  EXPECT_EQ(reg.value(), Word(0x1234));
  EXPECT_EQ(reg.high(), Byte::from_unsigned(0x12));
  EXPECT_EQ(reg.low(), Byte::from_unsigned(0x34));
}

TEST(WordRegisterTest, Idle) {
  Component root("root");
  Bus<Word> bus("bus", &root);
  WordRegister reg("register", &bus, nullptr, &root);
  EXPECT_EQ(reg.value(), Word(0));
  root.tick();
  EXPECT_EQ(reg.value(), Word(0));
}

TEST(WordRegisterTest, Reset) {
  Component root("root");
  WordRegister reg("register", nullptr, nullptr, &root);
  reg.set_value(Word(0x1234));
  EXPECT_EQ(reg.value(), Word(0x1234));
  reg.set_reset(true);
  EXPECT_TRUE(reg.reset());
  root.tick();
  EXPECT_FALSE(reg.reset());
  EXPECT_EQ(reg.value(), Word(0));
}

TEST(WordRegisterTest, WriteThroughWordBus) {
  Component root("root");
  Bus<Word> bus("bus", &root);
  WordRegister a("a", &bus, nullptr, &root);
  WordRegister b("b", &bus, nullptr, &root);

  // a = 0x1234
  a.set_value(Word(0x1234));

  // write a to b
  a.set_write(true);
  b.set_read(true);
  root.tick();

  // a and b controls should be cleared after tick
  EXPECT_THAT(a.write(), Optional(false));
  EXPECT_THAT(b.read(), Optional(false));

  // b should have the value of a
  EXPECT_EQ(b.value(), Word(0x1234));
}

TEST(WordRegisterTest, WriteThroughByteBus) {
  Component root("root");
  Bus<Byte> bus("bus", &root);
  WordRegister a("a", nullptr, &bus, &root);
  WordRegister b("b", nullptr, &bus, &root);

  // a = 0x1234
  a.set_value(Word(0x1234));

  // write a high byte to b low byte
  a.high_register().set_write(true);
  b.high_register().set_read(true);
  root.tick();

  // a and b high byte register controls should be cleared after tick
  EXPECT_THAT(a.high_register().write(), Optional(false));
  EXPECT_THAT(b.high_register().read(), Optional(false));

  // b high byte register should have the value of a high byte register
  EXPECT_EQ(b.value(), Word(0x1200));

  // write a low byte to b low byte
  a.low_register().set_write(true);
  b.low_register().set_read(true);
  root.tick();

  // a and b low byte register controls should be cleared after tick
  EXPECT_THAT(a.low_register().write(), Optional(false));
  EXPECT_THAT(b.low_register().read(), Optional(false));

  // b low byte register should have the value of a low byte register
  EXPECT_EQ(b.value(), Word(0x1234));
}

TEST(WordRegisterTest, ReadOpenWordBus) {
  Component root("root");
  Bus<Word> bus("bus", &root);
  WordRegister a("a", &bus, nullptr, &root);
  a.set_read(true);
  EXPECT_THROW(root.tick(), std::logic_error);
}

TEST(WordRegisterTest, ReadOpenByteBus) {
  Component root("root");
  Bus<Byte> bus("bus", &root);
  WordRegister a("a", nullptr, &bus, &root);
  a.high_register().set_read(true);
  EXPECT_THROW(root.tick(), std::logic_error);
}

TEST(WordRegisterTest, WordBusCreatesControls) {
  Component root("root");
  Bus<Word> bus("bus", &root);
  WordRegister a("a", &bus, nullptr, &root);
  EXPECT_THAT(a.bus(), NotNull());
  EXPECT_TRUE(a.has_write());
  EXPECT_NE(a.write(), std::nullopt);
  EXPECT_TRUE(a.has_read());
  EXPECT_NE(a.read(), std::nullopt);
}

TEST(WordRegisterTest, NoWordBusDoesNotCreateControls) {
  Component root("root");
  WordRegister a("a", nullptr, nullptr, &root);
  EXPECT_THAT(a.bus(), IsNull());
  EXPECT_FALSE(a.has_write());
  EXPECT_EQ(a.write(), std::nullopt);
  EXPECT_THROW(a.set_write(true), std::logic_error);
  EXPECT_FALSE(a.has_read());
  EXPECT_EQ(a.read(), std::nullopt);
  EXPECT_THROW(a.set_read(true), std::logic_error);
}

TEST(WordRegisterTest, ByteBusCreatesControls) {
  Component root("root");
  Bus<Byte> bus("bus", &root);
  WordRegister a("a", nullptr, &bus, &root);
  EXPECT_THAT(a.high_register().bus(), NotNull());
  EXPECT_TRUE(a.high_register().has_write());
  EXPECT_NE(a.high_register().write(), std::nullopt);
  EXPECT_TRUE(a.high_register().has_read());
  EXPECT_NE(a.high_register().read(), std::nullopt);
  EXPECT_THAT(a.low_register().bus(), NotNull());
  EXPECT_TRUE(a.low_register().has_write());
  EXPECT_NE(a.low_register().write(), std::nullopt);
  EXPECT_TRUE(a.low_register().has_read());
  EXPECT_NE(a.low_register().read(), std::nullopt);
}

TEST(WordRegisterTest, NoByteBusDoesNotCreateControls) {
  Component root("root");
  WordRegister a("a", nullptr, nullptr, &root);
  EXPECT_THAT(a.high_register().bus(), IsNull());
  EXPECT_FALSE(a.high_register().has_write());
  EXPECT_EQ(a.high_register().write(), std::nullopt);
  EXPECT_THROW(a.high_register().set_write(true), std::logic_error);
  EXPECT_FALSE(a.high_register().has_read());
  EXPECT_EQ(a.high_register().read(), std::nullopt);
  EXPECT_THROW(a.high_register().set_read(true), std::logic_error);
  EXPECT_THAT(a.low_register().bus(), IsNull());
  EXPECT_FALSE(a.low_register().has_write());
  EXPECT_EQ(a.low_register().write(), std::nullopt);
  EXPECT_THROW(a.low_register().set_write(true), std::logic_error);
  EXPECT_FALSE(a.low_register().has_read());
  EXPECT_EQ(a.low_register().read(), std::nullopt);
  EXPECT_THROW(a.low_register().set_read(true), std::logic_error);
}

TEST(WordRegisterTest, Serialize) {
  WordRegister reg("register");
  reg.set_value(Word(0x1234));
  std::ostringstream os;
  reg.serialize_all(os);
  EXPECT_THAT(os.str(), HasSubstr("value: 0x1234\n"));
}

} // namespace irata::sim::components
