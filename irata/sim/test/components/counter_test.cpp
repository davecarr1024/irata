#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <irata/sim/bytes/byte.hpp>
#include <irata/sim/components/bus.hpp>
#include <irata/sim/components/counter.hpp>

namespace irata::sim::components {

namespace {

class CounterTest : public ::testing::Test {
protected:
  ByteBus bus = ByteBus();
  Counter counter = Counter("counter", &bus);
};

} // namespace

TEST_F(CounterTest, Properties) {
  EXPECT_EQ(counter.name(), "counter");
  EXPECT_EQ(counter.type(), hdl::ComponentType::IncrementableRegister);
  EXPECT_EQ(counter.increment_control().parent(), &counter);
  EXPECT_EQ(counter.increment_control().name(), "increment");
  EXPECT_EQ(counter.decrement_control().parent(), &counter);
  EXPECT_EQ(counter.decrement_control().name(), "decrement");
}

TEST_F(CounterTest, SetIncrement) {
  EXPECT_FALSE(counter.increment());
  counter.set_increment(true);
  EXPECT_TRUE(counter.increment());
}

TEST_F(CounterTest, SetDecrement) {
  EXPECT_FALSE(counter.decrement());
  counter.set_decrement(true);
  EXPECT_TRUE(counter.decrement());
}

TEST_F(CounterTest, Idle) {
  counter.set_increment(false);
  EXPECT_EQ(counter.value(), Byte::from_unsigned(0));
  EXPECT_FALSE(counter.increment());
  counter.tick();
  EXPECT_EQ(counter.value(), Byte::from_unsigned(0));
  EXPECT_FALSE(counter.increment());
}

TEST_F(CounterTest, Increment) {
  counter.set_increment(true);
  EXPECT_EQ(counter.value(), Byte::from_unsigned(0));
  EXPECT_TRUE(counter.increment());
  counter.tick();
  EXPECT_EQ(counter.value(), Byte::from_unsigned(1));
  EXPECT_FALSE(counter.increment());
}

TEST_F(CounterTest, Overflow) {
  counter.set_increment(true);
  counter.set_value(Byte::from_unsigned(255));
  EXPECT_EQ(counter.value(), Byte::from_unsigned(255));
  EXPECT_TRUE(counter.increment());
  counter.tick();
  EXPECT_EQ(counter.value(), Byte::from_unsigned(0));
  EXPECT_FALSE(counter.increment());
}

TEST_F(CounterTest, Decrement) {
  counter.set_value(0x01);
  EXPECT_EQ(counter.value(), 0x01);
  counter.set_decrement(true);
  EXPECT_TRUE(counter.decrement());
  counter.tick();
  EXPECT_EQ(counter.value(), 0x00);
  EXPECT_FALSE(counter.decrement());
}

TEST_F(CounterTest, Underflow) {
  counter.set_decrement(true);
  EXPECT_TRUE(counter.decrement());
  counter.set_value(0x00);
  EXPECT_EQ(counter.value(), 0x00);
  counter.tick();
  EXPECT_EQ(counter.value(), 0xFF);
  EXPECT_FALSE(counter.decrement());
}

TEST_F(CounterTest, Reset) {
  counter.set_reset(true);
  counter.set_value(Byte::from_unsigned(1));
  EXPECT_EQ(counter.value(), Byte::from_unsigned(1));
  EXPECT_TRUE(counter.reset());
  counter.tick();
  EXPECT_EQ(counter.value(), Byte::from_unsigned(0));
  EXPECT_FALSE(counter.reset());
}

TEST_F(CounterTest, ResetOverridesIncrement) {
  counter.set_reset(true);
  counter.set_increment(true);
  counter.set_value(Byte::from_unsigned(1));
  EXPECT_EQ(counter.value(), Byte::from_unsigned(1));
  EXPECT_TRUE(counter.reset());
  EXPECT_TRUE(counter.increment());
  counter.tick();
  EXPECT_EQ(counter.value(), Byte::from_unsigned(0));
  EXPECT_FALSE(counter.reset());
  EXPECT_FALSE(counter.increment());
}

} // namespace irata::sim::components